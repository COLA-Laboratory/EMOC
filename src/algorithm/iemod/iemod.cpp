// Algorithm: IEMO/D
// Paper: Tomczyk, Michał K., and Miłosz Kadziński. "Decomposition-based interactive evolutionary algorithm for multiple objective optimization." IEEE Transactions on Evolutionary Computation 24.2 (2019): 320-334.
// Link: https://ieeexplore.ieee.org/abstract/document/8710313
// created by Tian Huang 10/04/2024
// make sure you have installed java
// IEMO/D also requires a hitandrun folder, run the follong command before run IEMOD:
// mkdir hitandrun

#include "algorithm/iemod/iemod.h"

#include <cmath>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <numeric>
#include <cstring>

#include "core/global.h"
#include "core/utility.h"
#include "core/file.h"
#include "core/uniform_point.h"
#include "operator/sbx.h"
#include "operator/polynomial_mutation.h"
#include "random/random.h"
using namespace std;

namespace emoc{

    IEMOD::IEMOD(int thread_id):
    Algorithm(thread_id),
    lambda_(nullptr),
    weight_(nullptr),
    goldenPoint_(nullptr),
    weight_num_(0),
    neighbour_(nullptr),
    ideal_point_(new double[g_GlobalSettings->obj_num_]),
    aggregation_type_(0),
    pbi_theta_(5.0),
    alpha_(5.0),
    alpha_DM_(5.0)
    {

    }

    IEMOD::~IEMOD()
    {
        for(int i = 0; i < weight_num_; i++)
        {
            delete[] lambda_[i];
            delete[] neighbour_[i];
            lambda_[i] = nullptr;
            neighbour_[i] = nullptr;
        }
        delete[] lambda_;
        delete[] weight_;
        delete[] goldenPoint_;
        delete[] neighbour_;
        delete[] ideal_point_;
        lambda_ = nullptr;
        weight_ = nullptr;
        goldenPoint_ = nullptr;
        neighbour_ = nullptr;
        ideal_point_ = nullptr;
    }

    void IEMOD::Solve()
    {
        int totalPairsNum = 0;
        int Q = 1000;
        int T = 100000;
        double winners[1000][g_GlobalSettings->obj_num_];
        double losers[1000][g_GlobalSettings->obj_num_];
        double L_S[Q][g_GlobalSettings->obj_num_];
        double L_M[g_GlobalSettings->population_num_][g_GlobalSettings->obj_num_];
        double boundaries[g_GlobalSettings->obj_num_][2];
        char inputFile[100];
        char hitandrunFile[100];
        char shFile[100];

        sprintf(inputFile, "./hitandrun/input_%s_%dd_%d.txt", g_GlobalSettings->problem_name_.c_str(), g_GlobalSettings->obj_num_, g_GlobalSettings->run_id_);
        sprintf(hitandrunFile, "./hitandrun/hitandrun_%s_%dd_%d.txt", g_GlobalSettings->problem_name_.c_str(), g_GlobalSettings->obj_num_, g_GlobalSettings->run_id_);
        sprintf(shFile, "./hitandrun/run_%s_%dd_%d.sh", g_GlobalSettings->problem_name_.c_str(), g_GlobalSettings->obj_num_, g_GlobalSettings->run_id_);
        // sprintf(inputFile, "./hitandrun/input_%s_%dd_%s_%d.txt", g_GlobalSettings->problem_name_.c_str(), g_GlobalSettings->obj_num_, g_GlobalSettings->weight_StringType_.c_str(), g_GlobalSettings->run_id_);
        // sprintf(hitandrunFile, "./hitandrun/hitandrun_%s_%dd_%s_%d.txt", g_GlobalSettings->problem_name_.c_str(), g_GlobalSettings->obj_num_, g_GlobalSettings->weight_StringType_.c_str(), g_GlobalSettings->run_id_);
        // sprintf(shFile, "./hitandrun/run_%s_%dd_%s_%d.sh", g_GlobalSettings->problem_name_.c_str(), g_GlobalSettings->obj_num_, g_GlobalSettings->weight_StringType_.c_str(), g_GlobalSettings->run_id_);

        // initialize the boundaries
        for(int i = 0; i < g_GlobalSettings->obj_num_; i++)
        {
            boundaries[i][0] = 0.0;
            boundaries[i][1] = 1.0;
        }
        std::ofstream out;
        out.open(inputFile);
        for(int i = 0; i < g_GlobalSettings->obj_num_; i++)
        {
            for(int j = 0; j < g_GlobalSettings->obj_num_; j++)
            {
                out << (i==j) << " ";
            }
            out << ">= " << boundaries[i][0] << endl;
            for(int j = 0; j < g_GlobalSettings->obj_num_; j++)
            {
                out << (i==j) << " ";
            }
            out << "<= " << boundaries[i][1] << endl;
        }
        for(int i = 0; i < g_GlobalSettings->obj_num_; i++)
        {
            out << 1 << " ";
        }
        out << "= " << 1;
        out.flush();
        out.close();
        Initialization();
        out.open(shFile);
        char shell[1024];
        sprintf(shell, "java -jar polyrun-1.0.0-jar-with-dependencies.jar -i %s -n %d > %s\n"
                       "wait\n"
                       "exit",
                        inputFile, g_GlobalSettings->population_num_, hitandrunFile);
        out << shell;
        out.flush();
        out.close();
        char giveRight[100];
        sprintf(giveRight, "chmod 777 %s", shFile);
        system(giveRight);
        
        lambda_ = HitAndRun(g_GlobalSettings->population_num_, g_GlobalSettings->obj_num_, shFile, hitandrunFile);
        out.open(shFile);
        sprintf(shell, "java -jar polyrun-1.0.0-jar-with-dependencies.jar -i %s -n %d > %s\n"
                       "wait\nexit",
                        inputFile, Q, hitandrunFile);
        out << shell;
        out.flush();
        out.close();

        InitializeNeighbours();
        for(int i = 0; i < g_GlobalSettings->population_num_; i++)
        {
            for(int j = 0; j < g_GlobalSettings->obj_num_; j++)
            {
                L_M[i][j] = lambda_[i][j];
            }
        }
        Individual *offspring = g_GlobalSettings->offspring_population_[0];
        int maxGen = (int)(g_GlobalSettings->max_evaluation_ / g_GlobalSettings->population_num_);
        int first_tau = 0.5 * g_GlobalSettings->max_evaluation_ / g_GlobalSettings->population_num_;
        first_tau = 100;
        printf("first tau = %d \n", first_tau);
        int tau = 25;
        
        while (!IsTermination())
        {
            if(g_GlobalSettings->iteration_num_ >= first_tau && g_GlobalSettings->iteration_num_ % tau == 0)
            {
                int number_of_inquiries = (int)(maxGen / tau);
                while (number_of_inquiries--)
                {
                    int player_1, player_2;
                    player_1 = player_2 = 0;
                    while (player_1 == player_2 || 
                    CheckDominance(g_GlobalSettings->parent_population_[player_1], g_GlobalSettings->parent_population_[player_2], g_GlobalSettings->obj_num_) != 0 )
                    {
                        player_1 = rnd(0, g_GlobalSettings->population_num_ - 1);
                        player_2 = rnd(0, g_GlobalSettings->population_num_ - 1);
                    }

                    if(CalInverseChebycheff(g_GlobalSettings->parent_population_[player_1], weight_, g_GlobalSettings->obj_num_)
                    < CalInverseChebycheff(g_GlobalSettings->parent_population_[player_2], weight_, g_GlobalSettings->obj_num_))
                    {
                        for(int i = 0; i < g_GlobalSettings->obj_num_; i++)
                        {
                            winners[totalPairsNum][i] = g_GlobalSettings->parent_population_[player_1]->obj_[i];
                            losers[totalPairsNum][i] = g_GlobalSettings->parent_population_[player_2]->obj_[i];
                        }
                    }
                    else
                    {
                        for(int i = 0; i < g_GlobalSettings->obj_num_; i++)
                        {
                            winners[totalPairsNum][i] = g_GlobalSettings->parent_population_[player_2]->obj_[i];
                            losers[totalPairsNum][i] = g_GlobalSettings->parent_population_[player_1]->obj_[i];
                        }
                    }
                    totalPairsNum++;
                    if(totalPairsNum >= 1000)
                    {
                        cerr << "\nToo many pairs of comparisons!\nThus program exits\n";
                        exit(1);
                    }
                    
                }
                for(int i = 0; i < g_GlobalSettings->obj_num_; i++)
                {
                    boundaries[i][0] = 1.0;
                    boundaries[i][1] = 0.0;
                }
                int trial = 0;
                int len_L_S = 0;
                double **testWeights = nullptr;
                while(len_L_S < Q)
                {
                    testWeights = HitAndRun(Q, g_GlobalSettings->obj_num_, shFile, hitandrunFile);
                    trial += Q;
                    for(int i = 0; i < Q; i++)
                    {
                        int j;
                        for( j = 0; j < totalPairsNum; j++)
                        {
                            if(CalInverseChebycheff(winners[j], testWeights[i], ideal_point_, g_GlobalSettings->obj_num_)
                            > CalInverseChebycheff(losers[j], testWeights[i], ideal_point_, g_GlobalSettings->obj_num_))
                            {
                                break;
                            }
                            
                        }
                        if(j == totalPairsNum)
                        {
                            for(int k = 0; k < g_GlobalSettings->obj_num_; k++)
                            {
                                L_S[len_L_S][k] = testWeights[i][k];
                                if(boundaries[k][0] > L_S[len_L_S][k])
                                {
                                    boundaries[k][0] = L_S[len_L_S][k];
                                }
                                if(boundaries[k][1] < L_S[len_L_S][k])
                                {
                                    boundaries[k][1] = L_S[len_L_S][k];
                                }
                            }
                            len_L_S++;
                            if(len_L_S == Q)
                            {
                                break;
                            }
                        }
                        delete[] testWeights[i];
                        testWeights[i] = nullptr;
                        
                        
                    }
                    delete[] testWeights;
                    testWeights = nullptr;
                    if(trial > T)
                    {
                        break;
                    }
                }
                // 2
                out.open(inputFile);
                for (int i = 0; i < g_GlobalSettings->obj_num_; ++i)
                {
                    for (int j = 0; j < g_GlobalSettings->obj_num_; ++j)
                    {
                        out<<(i==j)<<" ";
                    }
                    out<<">= "<<boundaries[i][0]<<std::endl;
                    for (int j = 0; j < g_GlobalSettings->obj_num_; ++j)
                    {
                        out<<(i==j)<<" ";
                    }
                    out<<"<= "<<boundaries[i][1]<<std::endl;
                }
                for (int i = 0; i < g_GlobalSettings->obj_num_; ++i)
                {
                    out<<1<<" ";
                }
                out<<"= "<<1;
                out.flush();
                out.close();

                /**
                 * lambda_: select randomly from L_S
                 */
                if (len_L_S >= g_GlobalSettings->population_num_)
                {
                    for (int i = 0; i < g_GlobalSettings->population_num_; ++i)
                    {
                        for (int j = 0; j < g_GlobalSettings->obj_num_; ++j)
                        {
                            lambda_[i][j] = L_S[i][j];
                        }
                    }
                }
                else
                {// len_L_S is smaller than g_GlobalSettings->population_num_
                    for (int i = 0; i < len_L_S; ++i)
                    {
                        for (int j = 0; j < g_GlobalSettings->obj_num_; ++j)
                        {
                            lambda_[i][j] = L_S[i][j];
                        }
                    }
                    int unqualifiedIndex[g_GlobalSettings->population_num_];
                    int unqualifiedNum = 0;
                    for (int i = 0; i < g_GlobalSettings->population_num_; ++i)
                    {
                        int j;
                        for (j = 0; j < totalPairsNum; ++j)
                        {
                            /*if (L_alpha_norm(L_M[i], winners[j], ideal_point_, g_GlobalSettings->obj_num_, alpha_)>
                                L_alpha_norm(L_M[i], losers[j], ideal_point_, g_GlobalSettings->obj_num_, alpha_)
                                    )*/
                            if (CalInverseChebycheff(winners[j], L_M[i], ideal_point_, g_GlobalSettings->obj_num_)>
                                    CalInverseChebycheff(losers[j], L_M[i], ideal_point_, g_GlobalSettings->obj_num_))
                            {
                                unqualifiedIndex[unqualifiedNum] = i;
                                unqualifiedNum++;
                                break;
                            }
                        }
                        if (j == totalPairsNum)
                        {
                            for (int k = 0; k < g_GlobalSettings->obj_num_; ++k)
                            {
                                lambda_[len_L_S][k] = L_M[i][k];
                            }
                            len_L_S++;
                            if (len_L_S == g_GlobalSettings->population_num_)
                            {
                                break;
                            }
                        }
                    }
                    int index = 0;
                    while (len_L_S < g_GlobalSettings->population_num_)
                    {
                        for (int i = 0; i < g_GlobalSettings->obj_num_; ++i)
                        {
                            lambda_[len_L_S][i] = L_M[unqualifiedIndex[index]][i];
                        }
                        len_L_S++;
                        index++;
                    }
                }
                for (int i = 0; i < g_GlobalSettings->population_num_; ++i)
                {
                    for (int j = 0; j < g_GlobalSettings->obj_num_; ++j)
                    {
                        L_M[i][j] = lambda_[i][j];
                    }
                }
                // Update the neighborhood
                UpdateNeighbours();
                
            }

            for (int i = 0; i < weight_num_; ++i)
            {
                // generate offspring for current subproblem
                Crossover(g_GlobalSettings->parent_population_.data(), i, offspring);
                // MutationInd(offspring, g_GlobalSettings);
                PolynomialMutation(offspring, g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, mutation_para_);
                EvaluateInd(offspring);//g_GlobalSettings->current_evaluation_++;
                // update ideal point
                UpdateIdealpoint(offspring, ideal_point_, g_GlobalSettings->obj_num_);

                // update neighbours' subproblem
                UpdateSubproblem(offspring, i, aggregation_type_);
            }

            // CalculateTotalDis(weight_);
        }
        // CalculateMinDis(weight_, g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->obj_num_, g_GlobalSettings->dec_num_, g_GlobalSettings->problem_name_, g_GlobalSettings->algorithm_name_);
        // CalculateExpectDis(weight_, g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->obj_num_, g_GlobalSettings->dec_num_, g_GlobalSettings->problem_name_, g_GlobalSettings->algorithm_name_);
        
    }

    void IEMOD::Initialization()
    {
        weight_num_ = g_GlobalSettings->population_num_;

        // set weight
        // weight_ = SetWeight(g_GlobalSettings->weight_StringType_);
        weight_ = SetWeight(g_GlobalSettings->obj_num_, g_GlobalSettings->problem_name_);


        // initialize parent population
        g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(),g_GlobalSettings->population_num_);
        EvaluatePop(g_GlobalSettings->parent_population_.data(),g_GlobalSettings->population_num_);
        // std::cout<<"weight_num_="<<weight_num_<<endl;

        // char file[256];
        // sprintf(file,"./UniformWeights/%dd_%d.txt",g_GlobalSettings->obj_num_,g_GlobalSettings->population_num_);
        // lambda_=LoadUniformWeights(g_GlobalSettings->population_num_,g_GlobalSettings->obj_num_,file);
        // lambda_=UniformPoint(g_GlobalSettings->population_num_,&weight_num_,g_GlobalSettings->obj_num_);
        // real_popnum_=weight_num_;


        // SetNeighbours();
        // InitializeNeighbours();

        UpdateIdealpoint(g_GlobalSettings->parent_population_.data(),weight_num_,ideal_point_,g_GlobalSettings->obj_num_);

        mutation_para_.pro=1.0/g_GlobalSettings->dec_num_;
        mutation_para_.index1=20.0;

        cross_para_.pro=1.0;
        cross_para_.index1=20.0; 
    }

    void IEMOD::InitializeNeighbours()
    {
        // set neighbour size and allocate memory
        neighbour_num_ = weight_num_ / 10;//todo: neighbour size
        neighbour_ = new int*[weight_num_];
        for (int i = 0; i < weight_num_; ++i)
        {
            neighbour_[i] = new int[neighbour_num_];
        }

        DistanceInfo *sort_list = new DistanceInfo[weight_num_];
        for (int i = 0; i < weight_num_; ++i)
        {
            for (int j = 0; j < weight_num_; ++j)
            {
                // calculate distance to each weight vector
                double distance_temp = 0;
                for (int k = 0; k < g_GlobalSettings->obj_num_; ++k)
                {
                    distance_temp += (lambda_[i][k] - lambda_[j][k]) * (lambda_[i][k] - lambda_[j][k]);
                }

                sort_list[j].distance = sqrt(distance_temp);
                sort_list[j].index = j;
            }

            std::sort(sort_list, sort_list+weight_num_, [](const DistanceInfo &left, const DistanceInfo &right) {
                return left.distance < right.distance;
            });

            for (int j = 0; j < neighbour_num_; j++)
            {
                neighbour_[i][j] = sort_list[j+1].index;
            }
        }

        delete[] sort_list;
    }

    void IEMOD::UpdateNeighbours()
    {
        DistanceInfo *sort_list = new DistanceInfo[weight_num_];
        for (int i = 0; i < weight_num_; ++i)
        {
            for (int j = 0; j < weight_num_; ++j)
            {
                // calculate distance to each weight vector
                double distance_temp = 0;
                for (int k = 0; k < g_GlobalSettings->obj_num_; ++k)
                {
                    distance_temp += (lambda_[i][k] - lambda_[j][k]) * (lambda_[i][k] - lambda_[j][k]);
                }

                sort_list[j].distance = sqrt(distance_temp);
                sort_list[j].index = j;
            }

            std::sort(sort_list, sort_list+weight_num_, [](const DistanceInfo &left, const DistanceInfo &right) {
                return left.distance < right.distance;
            });

            for (int j = 0; j < neighbour_num_; j++)
            {
                neighbour_[i][j] = sort_list[j+1].index;
            }
        }
        delete[] sort_list;
    }

    void IEMOD::Crossover(Individual **parent_pop, int current_index, Individual *offspring)
    {
        // randomly select two parent from current individual's neighbours
        int k = rnd(0, neighbour_num_ - 1);
        int l = rnd(0, neighbour_num_ - 1);
        Individual *parent1 = parent_pop[neighbour_[current_index][k]];
        Individual *parent2 = parent_pop[neighbour_[current_index][l]];
        SBX(parent1, parent2, g_GlobalSettings->offspring_population_[1], offspring, 
        g_GlobalSettings->dec_lower_bound_,g_GlobalSettings->dec_upper_bound_,cross_para_);
    }

    void IEMOD::UpdateSubproblem(Individual *offspring, int current_index, int aggregation_type)
    {
            double *offspring_fitness = new double[neighbour_num_];
        double *neighbour_fitness = new double[neighbour_num_];

        // calculate fitness;
        switch (aggregation_type)
        {
            case 0:
                // inverse chebycheff
                for (int i = 0; i < neighbour_num_; ++i)
                {
                    int weight_index = neighbour_[current_index][i];
                    Individual *current_ind = g_GlobalSettings->parent_population_[weight_index];
                    neighbour_fitness[i] = CalInverseChebycheff(current_ind, lambda_[weight_index], ideal_point_, g_GlobalSettings->obj_num_);
                    offspring_fitness[i] = CalInverseChebycheff(offspring, lambda_[weight_index], ideal_point_, g_GlobalSettings->obj_num_);
                }
                break;

            case 1:
                // weighted sum
                for (int i = 0; i < neighbour_num_; ++i)
                {
                    int weight_index = neighbour_[current_index][i];
                    Individual *current_ind = g_GlobalSettings->parent_population_[weight_index];
                    neighbour_fitness[i] = CalWeightedSum(current_ind, lambda_[weight_index], ideal_point_, g_GlobalSettings->obj_num_);
                    offspring_fitness[i] = CalWeightedSum(offspring, lambda_[weight_index], ideal_point_, g_GlobalSettings->obj_num_);
                }
                break;

            case 2:
                // PBI
                for (int i = 0; i < neighbour_num_; ++i)
                {
                    int weight_index = neighbour_[current_index][i];
                    Individual *current_ind = g_GlobalSettings->parent_population_[weight_index];
                    neighbour_fitness[i] = CalPBI(current_ind, lambda_[weight_index], ideal_point_, g_GlobalSettings->obj_num_, pbi_theta_);
                    offspring_fitness[i] = CalPBI(offspring, lambda_[weight_index], ideal_point_, g_GlobalSettings->obj_num_, pbi_theta_);
                }
                break;

            default:
                break;
        }

        // update subproblem
        for (int i = 0; i < neighbour_num_; ++i)
        {
            if (offspring_fitness[i] < neighbour_fitness[i])
            {
                CopyIndividual(offspring, g_GlobalSettings->parent_population_[neighbour_[current_index][i]]);
            }
        }


        delete[] neighbour_fitness;
        delete[] offspring_fitness;
    }


}