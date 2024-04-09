// Algorithm: I-MOEA/D-PLVF 
// Paper: Li, Ke, et al. "Interactive decomposition multiobjective optimization via progressively learned value functions." IEEE Transactions on Fuzzy Systems 27.5 (2018): 849-860. 
// Link: https://ieeexplore.ieee.org/abstract/document/8531708
// created by Tian Huang on 09/04/2024

#include "algorithm/moead_plvf/moead_plvf.h"

#include <cmath>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <string.h>

#include "core/global.h"
#include "core/utility.h"
#include "core/file.h"
#include "core/uniform_point.h"
#include "core/emoc_manager.h"
#include "operator/sbx.h"
#include "operator/polynomial_mutation.h"
#include "random/random.h"
# define INF 1.0e14
#include "pinv/calculatePinv.h"
#include "operator/de.h"
// using namespace std

namespace emoc{
    MOEADPLVF::MOEADPLVF(int thread_id):
    Algorithm(thread_id),
	lambda_(nullptr),
	weight_num_(0),
	neighbour_(nullptr),
	ideal_point_(new double[g_GlobalSettings->obj_num_]), 
	aggregation_type_(0),
    neighbour_selectpro_(0.9),
    replace_num_(2),
	pbi_theta_(5.0),
    weight(nullptr),
    goldenPoint_(nullptr),
    alpha(5.0),
    alpha_DM(5.0),
    rbf_output_(nullptr),
    rbfnet_c(nullptr),
    rbfnet_weight_(nullptr),
    mu_(0) ,
    sigma_(20),
    stepSize_(0.3)
    {

    }

    MOEADPLVF::~MOEADPLVF()
    {
        for (int i = 0; i < weight_num_; ++i)
        {
            delete[] lambda_[i];
            delete[] neighbour_[i];
            lambda_[i] = nullptr;
            neighbour_[i] = nullptr;
        }
        for (int i = 0; i < 40; ++i)
        {
            delete[] rbfnet_c[i];
            rbfnet_c[i] = nullptr;
        }
        delete[] lambda_;
        delete[] weight;
        delete[] goldenPoint_;
        delete[] neighbour_;
        delete[] ideal_point_;
        delete[] rbf_output_;
        delete[] rbfnet_weight_;
        delete[] rbfnet_c;
        lambda_ = nullptr;
        weight = nullptr;
        goldenPoint_ = nullptr;
        neighbour_ = nullptr;
        ideal_point_ = nullptr;
        rbf_output_ = nullptr;
        rbfnet_weight_ = nullptr;
        rbfnet_c = nullptr;
    }

    void MOEADPLVF::Solve()
    {
        Initialization();
        Individual *offspring=g_GlobalSettings->offspring_population_[0];
        int first_tau = 0.4 * g_GlobalSettings->max_evaluation_ / g_GlobalSettings->population_num_;
        int tau = 25;
        // int tau = g_GlobalSettings->tau_;

       int number_of_inqueries;
       int randNum;
       SortList *fitnessList = new SortList[weight_num_];
       double dis,minDis;
       int idx_of_nearest, currPromisingIndex, maxAttractionNum, currTuned;
       bool flag[weight_num_];
       int solveNum;
       
       while (!IsTermination())
       {
            if(g_GlobalSettings->iteration_num_ >= first_tau && g_GlobalSettings->iteration_num_ % tau == 0)
            {
                if(g_GlobalSettings->iteration_num_ == first_tau)
                {
                    // select randomly for the first consultation
                    number_of_inqueries = mu_;
                    for(int i = 0; i < number_of_inqueries; ++i)
                    {
                        randNum = rnd(0, weight_num_ - 1);
                        rbf_output_[i] = CalInverseChebycheff(g_GlobalSettings->parent_population_[randNum], weight, 
                        ideal_point_, g_GlobalSettings->obj_num_);
                        for(int j = 0; j < g_GlobalSettings->obj_num_; ++j)
                        {
                            rbfnet_c[i][j] = g_GlobalSettings->parent_population_[randNum]->obj_[j];
                        }
                    }
                }
                else
                {
                    // select the best top-10 individuals by RBFnet
                    // TODO: add a global param inquiresNum
                    // int inquiresNum = 10;
                    number_of_inqueries = 10;

                    for(int i = 0; i < weight_num_; ++ i)
                    {
                        UsingRBFNet(g_GlobalSettings->parent_population_[i], rbfnet_c, rbfnet_weight_, sigma_, number_of_inqueries, g_GlobalSettings->obj_num_);
                        fitnessList[i].value = g_GlobalSettings->parent_population_[i]->fitness_;
                        fitnessList[i].index = i;
                    }
                    std::sort(fitnessList, fitnessList + weight_num_, [](const SortList &left, const SortList &right){
                        return left.value < right.value;
                        });
                    solveNum = number_of_inqueries;
                    memset(flag, true, weight_num_ * sizeof(bool));
                    // true means adjustable, false means has been adjusted
                    for(int i = 0; i < number_of_inqueries; ++i)
                    {
                        flag[fitnessList[i].index] = false;
                        rbf_output_[i] = CalInverseChebycheff(g_GlobalSettings->parent_population_[fitnessList[i].index], weight, ideal_point_, g_GlobalSettings->obj_num_);
                        for(int j = 0; j < g_GlobalSettings->obj_num_; ++j)
                        {
                            rbfnet_c[i][j] = g_GlobalSettings->parent_population_[fitnessList[i].index]->obj_[j];
                        }
                    }
                    maxAttractionNum = ceil((double)(weight_num_ - number_of_inqueries) / number_of_inqueries);
                    for(int i = 0; (i < number_of_inqueries) && (solveNum < weight_num_); ++i)
                    {
                        currPromisingIndex = fitnessList[i].index;
                        currTuned = 0;
                        while( (currTuned < maxAttractionNum) && (solveNum < weight_num_))
                        {
                            minDis = INF;
                            // find the nearest weight vector from the remaining set of weight vector
                            for(int j = 0; j < weight_num_; ++j)
                            {
                                if(flag[j])
                                {
                                    dis = CalEuclidianDistance(lambda_[currPromisingIndex],lambda_[j], g_GlobalSettings->obj_num_);
                                    if(dis < minDis)
                                    {
                                        minDis = dis;
                                        idx_of_nearest = j;
                                    }
                                }
                            }
                            // printf("currentPromising weight");
                            // for(int j = 0; j <g_GlobalSettings->obj_num_; j++)
                            // {
                            //     printf("%f",lambda_[currPromisingIndex][j]);
                            // }
                            // printf("\n");
                            // move the closest weight vector to the current promising wegith vector
                            for(int j = 0; j < g_GlobalSettings->obj_num_; ++j)
                            {
                                lambda_[idx_of_nearest][j] += stepSize_ * (lambda_[currPromisingIndex][j] - lambda_[idx_of_nearest][j]);
                            }
                            flag[idx_of_nearest] = false; // means have been changed
                            solveNum ++;
                            currTuned ++;
                        }
                    }
                    
                }
                TrainRBFNet(rbfnet_c, rbfnet_weight_, rbf_output_, sigma_, number_of_inqueries, g_GlobalSettings->obj_num_);
                UpdateNeighbours();
            }
            for(int i=0;i<weight_num_;i++)
            {   
                if(randomperc() < neighbour_selectpro_)
                    neighbour_type_ = NEIGHBOUR;
                else
                    neighbour_type_ = GLOBAL;

                Crossover(g_GlobalSettings->parent_population_.data(),i,offspring);
                PolynomialMutation(offspring,g_GlobalSettings->dec_lower_bound_,g_GlobalSettings->dec_upper_bound_,mutation_para_);
                EvaluateInd(offspring);

                UpdateIdealpoint(offspring,ideal_point_,g_GlobalSettings->obj_num_);
                UpdateSubproblem(offspring,i,aggregation_type_);
            }
            // CalculateTotalDis(weight);
       }
    //    CalculateMinDis(weight, g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->obj_num_, g_GlobalSettings->dec_num_, g_GlobalSettings->problem_name_, g_GlobalSettings->algorithm_name_);
    //    CalculateExpectDis(weight, g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->obj_num_, g_GlobalSettings->dec_num_, g_GlobalSettings->problem_name_, g_GlobalSettings->algorithm_name_);
        
    }

    void MOEADPLVF::Initialization()
    {
        // initialize rbf parameters
        weight_num_ = g_GlobalSettings->population_num_;
        // TODO: rbfnet memory allocation
        rbf_output_ = new double[weight_num_];
        rbfnet_c = new double*[40];
        for (int i = 0; i < 40; i++) 
        {
            rbfnet_c[i] = new double[g_GlobalSettings->obj_num_];
        }
        rbfnet_weight_ = new double[40];
        mu_ = 2 * g_GlobalSettings->obj_num_ + 1;
        // printf("mu_ = %d \n",mu_);
        if (mu_ > 40)
        {
            std::cerr << "mu_>40" << std::endl;
            exit(1);
        }

        // weight = SetWeight(g_GlobalSettings->weight_StringType_);
        weight = SetWeight(g_GlobalSettings->obj_num_, g_GlobalSettings->problem_name_);
        // printf("weight for DM:\n");
        // for(int i = 0; i < g_GlobalSettings->obj_num_; i++)
        // {
        //     printf("%f",weight[i]);
        // }
        // printf("\n");

        // initialize parent pop
        // weight_num_=g_GlobalSettings->population_num_;
        // std::cout<<"weight_num_="<<weight_num_<<endl;
        g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(),g_GlobalSettings->population_num_);
        EvaluatePop(g_GlobalSettings->parent_population_.data(),g_GlobalSettings->population_num_);
        // std::cout<<"weight_num_="<<weight_num_<<endl;

        // char file[256];
        // sprintf(file,"./UniformWeights/%dd_%d.txt",g_GlobalSettings->obj_num_,g_GlobalSettings->population_num_);
        // lambda_=LoadUniformWeights(g_GlobalSettings->population_num_,g_GlobalSettings->obj_num_,file);
        lambda_=UniformPoint(g_GlobalSettings->population_num_,&weight_num_,g_GlobalSettings->obj_num_);
        real_popnum_=weight_num_;


        SetNeighbours();

        UpdateIdealpoint(g_GlobalSettings->parent_population_.data(),weight_num_,ideal_point_,g_GlobalSettings->obj_num_);

        mutation_para_.pro=1.0/g_GlobalSettings->dec_num_;
        mutation_para_.index1=20.0;

        cross_para_.pro=1.0;
        cross_para_.index1=20.0;


    }

    void MOEADPLVF::SetNeighbours()
    {
        // neighbour_num_=weight_num_/10;
        neighbour_num_ = 10;
        neighbour_=new int *[weight_num_];
        for(int i=0; i< weight_num_; ++i)
        {
            neighbour_[i]=new int[neighbour_num_];
        }
        DistanceInfo *sort_list = new DistanceInfo[weight_num_];
        // std::vector<DistanceInfo>sort_list(weight_num_);
        for(int i = 0; i < weight_num_; ++i)
        {
            for(int j = 0; j < weight_num_; ++j)
            {
                // double distance_temp=0;
                // for(int k=0; k<g_GlobalSettings->obj_num_; k++)
                // {
                //     distance_temp+=(lambda_[i][k] - lambda_[j][k])*(lambda_[i][k] - lambda_[j][k]);
                // }
                // sort_list[i].distance=sqrt(distance_temp);
                sort_list[j].distance = CalculateCos(lambda_[i],lambda_[j], g_GlobalSettings->obj_num_);
                sort_list[j].index=j;
            }
            std::sort(sort_list,sort_list + weight_num_,[](const DistanceInfo &left,const DistanceInfo &right){
                return left.distance > right.distance;
            });
            for(int j = 0; j < neighbour_num_; ++j)
            {
                neighbour_[i][j] = sort_list[j + 1].index;
            }
        }

        delete[] sort_list;
    }

    void MOEADPLVF::Crossover(Individual **parent_pop, int current_index, Individual *offspring)
    {
        int size = neighbour_type_ == NEIGHBOUR ? neighbour_num_ : weight_num_;
        int parent1_index = 0,parent2_index = 0;

        if(neighbour_type_ == NEIGHBOUR)
        {
            int k = rnd(0,size-1);
            int l = rnd(0,size-1);
            parent1_index = neighbour_[current_index][k];
            parent2_index = neighbour_[current_index][l];
        }
        else
        {
            parent1_index = rnd(0,size-1);
            parent2_index = rnd(0,size-1);
        }
        Individual *parent1=parent_pop[parent1_index];
        Individual *parent2=parent_pop[parent2_index];

        SBX(parent1,parent2,g_GlobalSettings->offspring_population_[1],offspring,
        g_GlobalSettings->dec_lower_bound_,g_GlobalSettings->dec_upper_bound_,cross_para_);
    }

    void MOEADPLVF::UpdateSubproblem(Individual *offspring, int current_index, int aggregration_type)
    {
        int size = neighbour_type_ == NEIGHBOUR ? neighbour_num_ : weight_num_;
		std::vector<int> perm_index(size);
		random_permutation(perm_index.data(), size);

		int count = 0, weight_index = 0;
		double offspring_fitness = 0.0;
		double neighbour_fitness = 0.0;

		// calculate fitness and update subproblem;
		for (int i = 0; i < size; ++i)
		{
			if (count >= replace_num_)
				break;
			
			if (neighbour_type_ == NEIGHBOUR)
            {
                // std::cout<<i<<":"<<current_index<<"  "<<perm_index[i]<<endl;
                weight_index = neighbour_[current_index][perm_index[i]];
            }
			else
				weight_index = perm_index[i];

			Individual *current_ind = g_GlobalSettings->parent_population_[weight_index];

            switch(aggregation_type_)
            {
                case 0:
                // inver chebycheff
                    offspring_fitness = CalInverseChebycheff(offspring, lambda_[weight_index], ideal_point_, g_GlobalSettings->obj_num_);
			        neighbour_fitness = CalInverseChebycheff(current_ind, lambda_[weight_index], ideal_point_, g_GlobalSettings->obj_num_);
                    break;
                
                case 1:
                // weighted sum
                    offspring_fitness=CalWeightedSum(current_ind,lambda_[weight_index],ideal_point_,g_GlobalSettings->obj_num_);
                    neighbour_fitness=CalWeightedSum(offspring,lambda_[weight_index],ideal_point_,g_GlobalSettings->obj_num_);
                    break;
                case 2:
                // PBI
                    offspring_fitness=CalPBI(current_ind,lambda_[weight_index],ideal_point_,g_GlobalSettings->obj_num_,pbi_theta_);
                    neighbour_fitness=CalPBI(offspring,lambda_[weight_index],ideal_point_,g_GlobalSettings->obj_num_,pbi_theta_);
                    break;
                default:
                    break;
            }
			
			if (offspring_fitness < neighbour_fitness)
			{
				CopyIndividual(offspring, g_GlobalSettings->parent_population_[weight_index]);
				count++;
			}
		}
        perm_index.clear();
    }

    void MOEADPLVF::UpdateNeighbours()
    {
        DistanceInfo *sort_list=new DistanceInfo[weight_num_];
        // std::vector<DistanceInfo>sort_list(weight_num_);
        for(int i = 0; i < weight_num_; ++i)
        {
            for(int j = 0; j < weight_num_; ++j)
            {
                // double distance_temp=0;
                // for(int k=0;k<g_GlobalSettings->obj_num_;++k)
                // {
                //     distance_temp+=(lambda_[i][k]-lambda_[j][k])*(lambda_[i][k]-lambda_[j][k]);
                // }
                // sort_list[j].distance=sqrt(distance_temp);
                sort_list[j].distance = CalculateCos(lambda_[i], lambda_[j], g_GlobalSettings->obj_num_);
                sort_list[j].index=j;
            }
            std::sort(sort_list,sort_list + weight_num_,[](const DistanceInfo &left, const DistanceInfo &right){
                return left.distance > right.distance;
            });

           for(int j = 0 ; j < neighbour_num_; j++)
           {
                neighbour_[i][j] = sort_list[j + 1].index;
           }
        }
        delete[] sort_list;
    }

    void MOEADPLVF::UsingRBFNet(Individual *ind, double **c, double *weight, double sigma, int size, int dimension)
    {
        double *active_function, *center;
        active_function = center = nullptr;
        active_function = new double[size];
        center = new double[dimension];
        memset(active_function, 0, size * sizeof(double));
        for (int i = 0; i < size; ++i)
        {
            for (int j = 0; j < dimension; ++j)
            {
                center[j] = c[i][j];
            }
            for (int j = 0; j < dimension; ++j)
            {
                active_function[i] += pow(center[j] - ind->obj_[j], 2.0);
            }
            active_function[i] = exp(-1.0 * sqrt(active_function[i]) * sigma);
        }
        ind->fitness_ = 0.0;
        for (int i = 0; i < size; ++i)
        {
            ind->fitness_ += weight[i] * active_function[i];
        }
        //free memory
        delete[] active_function;
        delete[] center;
        active_function = nullptr;
        center = nullptr;
    }

    void MOEADPLVF::TrainRBFNet(double **c, double *weight, double *out, double sigma, int size, int dimension)
    {
        double **active_function = nullptr;
        double **active_function_pinv = nullptr;

        active_function = new double*[size];
        for(int i = 0; i < size; i++)
        {
            active_function[i] = new double[size];
        }
        double *center = nullptr;
        center = new double[dimension];
        for(int i = 0; i < size; i++)
        {
            for(int j = 0; j < size; j++)
            {
                active_function[i][j] = 0;
            }
        }
        for(int i = 0; i < size; i++)
        {
            for(int j = 0; j < dimension; j++)
            {
                center[j] = c[i][j];
            }
            for(int j = 0; j < size; j++)
            {
                for(int k = 0; k < dimension; k++)
                {
                    active_function[j][i] += pow(center[k] - c[j][k], 2);
                }
                active_function[j][i] = exp(sqrt(active_function[j][i])*(-sigma));
            }
        }
        active_function_pinv = calculatePinv(active_function, size, size);
        for(int i = 0; i < size; ++i)
        {
            weight[i] = 0.0;
            for(int j = 0; j < size; j++)
            {
                weight[i] += out[j] * active_function_pinv[i][j];
            }
        }
        // free memory
        for(int i = 0; i < size; i++)
        {
            delete[] active_function[i];
            delete[] active_function_pinv[i];
        }
        delete[] active_function;
        delete[] active_function_pinv;
        active_function = nullptr;
        active_function_pinv = nullptr;
        delete[] center;
        center = nullptr;
    }

}