// Algorithm: D-PBEMO-MOEA/D
// Paper: (ICML 2024 under review) Huang, Tian, and Ke Li. "Direct Preference-Based Evolutionary Multi-Objective Optimization with Dueling Bandit." arXiv preprint arXiv:2311.14003 (2023).
// Link: https://arxiv.org/abs/2311.14003
// Created by Tian Huang 11/04/2024

#include "algorithm/moead_direct/meoad_direct.h"

#include <cmath>
#include <ctime>
#include <iostream>
#include <algorithm>

#include "core/file.h"
#include "core/global.h"
#include "core/utility.h"
#include "core/uniform_point.h"
#include "core/emoc_manager.h"
#include "operator/sbx.h"
#include "operator/polynomial_mutation.h"
#include "random/random.h"

#include <numeric>  //iota
// #include "../vendor/Bandit/stdafx.h"
// #include "../vendor/Bandit/DTS.h"

#include "../vendor/C-DTS/stdafx.h"
#include "../vendor/C-DTS/CDTS.h"

// #include "DTS/stdafx.h"
// #include "DTS/DTS.h"
using namespace std;

namespace emoc{
    DPBEMO_MOEAD::DPBEMO_MOEAD( int thread_id ):
    Algorithm(thread_id),
    lambda_(nullptr),
    weight_num_(0),
    neighbour_(nullptr),
    ideal_point_(new double[g_GlobalSettings->obj_num_]),
	aggregation_type_(0),
    neighbour_selectpro_(0.9),
	pbi_theta_(5.0),
    count(0),
    replace_num_(2),
    nPromisingWeight(10),
    step_size_(0.3),
    utl(new double[g_GlobalSettings->population_num_]),
    select_sum(10),
    compare(new size_t[select_sum]),
    weight(new double[g_GlobalSettings->obj_num_]),
    sigma(0.3),
    subset_solution_num(10),
    best_index(0),
    best_region(0),
    win_times(subset_solution_num, fill::zeros),
    max_consultation(10),
    pref_prob1(new double[g_GlobalSettings->population_num_]),
	pref_prob2(new double[g_GlobalSettings->population_num_]),
    gamma(0.9),
    best_weights(nullptr),
    best_weight(new double[g_GlobalSettings->obj_num_])
    {

    }

    DPBEMO_MOEAD::~DPBEMO_MOEAD()
    {
        for (int i = 0; i < weight_num_; ++i)
		{
			delete[] lambda_[i];		
			lambda_[i] = nullptr;
            delete[] neighbour_[i];
            neighbour_[i]=nullptr;
		}
        for(int i = 0; i < max_consultation; ++i)
        {
            delete[] recommend_point[i];
            recommend_point[i] = nullptr;
        }
        for(int i = 0; i < subset_solution_num; ++i)
        {
            delete[] best_weights[i];
            best_weights[i] = nullptr;
        }
        
		delete[] lambda_;
		delete[] neighbour_;
		delete[] ideal_point_;
        delete[] utl;
        delete[] compare;
        delete[] weight;
        delete[] recommend_point;
        delete[] pref_prob1;
        delete[] pref_prob2;
        delete[] best_weights;
        delete[] best_weight;
		lambda_ = nullptr;
		neighbour_ = nullptr;
		ideal_point_ = nullptr;
        utl = nullptr;
        compare = nullptr;
        weight = nullptr;
        recommend_point = nullptr;
        pref_prob1 = nullptr;
        pref_prob2 = nullptr;
        best_weights = nullptr;
        best_weight = nullptr;
    }

    void DPBEMO_MOEAD::Solve()
    {
        Initialization();
        Individual *offspring = g_GlobalSettings->offspring_population_[0];
        int first_tau = 0.5 * g_GlobalSettings->max_evaluation_ / g_GlobalSettings->population_num_;
        std::vector<size_t>index_pop(g_GlobalSettings->population_num_);

        while(!IsTermination())
        {
            // rank according to obj1
            
            Individual** pop = g_GlobalSettings->parent_population_.data();
            iota(index_pop.begin(),index_pop.end(),0);
            std::sort(index_pop.begin(),index_pop.end(), [&pop](size_t index1, size_t index2){return pop[index1]->obj_[0] > pop[index2]->obj_[0];});
            
            // first time to consult when run 50% iteration budget
            if(g_GlobalSettings->iteration_num_ >= first_tau && count == 0)
            {
                printf("the %d time to consult in generation %d \n", count + 1, g_GlobalSettings->iteration_num_);
                UtilityFunction_Gaussian(g_GlobalSettings->parent_population_.data());
                StorePreference(g_GlobalSettings->parent_population_.data());
                               
                std::tuple<int, int, vec> result;
                result = Consult_DM();
                std::tie(best_region, best_index, win_times)= result;
                cout << "The variance in winning time vector" << var(win_times) << endl;

                printf("the recommend population is inedx %d: ", index_pop[best_index]);
                for(int i = 0; i < g_GlobalSettings->obj_num_; i++)
                {
                    cout << g_GlobalSettings->parent_population_[index_pop[best_index]]->obj_[i];
					recommend_point[count][i] = g_GlobalSettings->parent_population_[index_pop[best_index]]->obj_[i];
                    best_weight[i] = lambda_[index_pop[best_index]][i];
                }
                cout << endl;
                
                for(int i = 0; i < subset_solution_num; i++)
                {    
                    for(int j = 0; j < g_GlobalSettings->obj_num_; j++)
                        best_weights[i][j] = lambda_[index_pop[best_region * subset_solution_num + i]][j];
                }
                count ++;
                

            }
            // the 2nd and afterwards consultation
            if(g_GlobalSettings->iteration_num_ > first_tau && count < max_consultation)
            {   
                double KL = CalculateKLDivergence(g_GlobalSettings->parent_population_.data());
                if(KL > 1e-3)
                // if(KL > 1e-6)
                // if(KL > 1e-1)
                {
                    printf("KL = %f \n", KL);
					printf("the %d time to consult in generation %d\n", count + 1, g_GlobalSettings->iteration_num_);
					UtilityFunction_Gaussian(g_GlobalSettings->parent_population_.data());
					StorePreference(g_GlobalSettings->parent_population_.data());
					
                    
                    std::tuple<int, int, vec> result;
                    result = Consult_DM();
                    std::tie(best_region, best_index, win_times)= result;
                    cout << "The variance in winning time vector" << var(win_times) << endl;

                    printf("the recommend population is inedx %d: ", index_pop[best_index]);
                    for(int i = 0; i < g_GlobalSettings->obj_num_; i++)
                    {
                        cout << g_GlobalSettings->parent_population_[index_pop[best_index]]->obj_[i];
                        recommend_point[count][i] = g_GlobalSettings->parent_population_[index_pop[best_index]]->obj_[i];
                        best_weight[i] = lambda_[index_pop[best_index]][i];
                    }
                    cout << endl;
                    
                    for(int i = 0; i < subset_solution_num; i++)
                    {
                        for(int j = 0; j < g_GlobalSettings->obj_num_; j++)
                            best_weights[i][j] = lambda_[index_pop[best_region * subset_solution_num + i]][j];
                    }

                    count ++;

                    // SetBiasedWeight(lambda_[index[best_index]]);
                    // UpdateNeighbour();
                }
            }
            
            if(count)
            {
                // SetBiasedWeight(lambda_[index[best_index]]);
                SetBiasedWeight();
                
                UpdateNeighbour();

            }          
            
            
            for(int i = 0; i < weight_num_ ; i++)
            {
                if(randomperc() < neighbour_selectpro_)
                {
                    neighbour_type_ = NEIGHBOUR;
                }
                else
                {
                    neighbour_type_ = GLOBAL;
                }
                Crossover(g_GlobalSettings->parent_population_.data(),i,offspring);
                PolynomialMutation(offspring,g_GlobalSettings->dec_lower_bound_,g_GlobalSettings->dec_upper_bound_,mutation_para_);
                EvaluateInd(offspring);

                UpdateIdealpoint(offspring,ideal_point_,g_GlobalSettings->obj_num_);
                UpdateSubproblem(offspring,i,aggregation_type_);
            }

            
        }
        printf("total consultation is %d times\n", count);
        // CalculateMinDis(weight, g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->obj_num_, g_GlobalSettings->dec_num_, g_GlobalSettings->problem_name_, g_GlobalSettings->algorithm_name_);
        // CalculateExpectDis(weight, g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->obj_num_, g_GlobalSettings->dec_num_, g_GlobalSettings->problem_name_, g_GlobalSettings->algorithm_name_);
    }

    void DPBEMO_MOEAD::Initialization()
    {
        weight_num_=g_GlobalSettings->population_num_;
        // std::cout<<"weight_num_="<<weight_num_<<endl;
        g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(),g_GlobalSettings->population_num_);
        EvaluatePop(g_GlobalSettings->parent_population_.data(),g_GlobalSettings->population_num_);
        // std::cout<<"weight_num_="<<weight_num_<<endl;

        char file[256];
        sprintf(file,"./UniformWeights/%dd_%d.txt",g_GlobalSettings->obj_num_,g_GlobalSettings->population_num_);
        lambda_=LoadUniformWeights(g_GlobalSettings->population_num_,g_GlobalSettings->obj_num_,file);
        // lambda_=UniformPoint(g_GlobalSettings->population_num_,&weight_num_,g_GlobalSettings->obj_num_);
        real_popnum_=weight_num_;
        
        // weight = SetWeight(g_GlobalSettings->weight_StringType_);
        weight = SetWeight(g_GlobalSettings->obj_num_, g_GlobalSettings->problem_name_);
        
        best_weights = new double*[subset_solution_num];
        for(int i = 0; i < subset_solution_num; i++)
        {
            best_weights[i] = new double[g_GlobalSettings->obj_num_];
        }
        
        // initialize recommend point
		recommend_point = new double*[max_consultation];
		for(int i = 0; i < max_consultation; i++)
		{
			recommend_point[i] = new double[g_GlobalSettings->obj_num_];
		}

        SetNeighbours();

        UpdateIdealpoint(g_GlobalSettings->parent_population_.data(), weight_num_, ideal_point_, g_GlobalSettings->obj_num_);

        mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
        mutation_para_.index1 = 20.0;

        cross_para_.pro = 1.0;
        cross_para_.index1 = 20.0;     

    }

    void DPBEMO_MOEAD::SetNeighbours()
    {
        neighbour_num_=weight_num_ / 10;
        neighbour_=new int*[weight_num_];
        for(int i = 0; i < weight_num_; ++i)
        {
            neighbour_[i]=new int[neighbour_num_];
        }

        std::vector<DistanceInfo> sort_list(weight_num_);
        for(int i = 0; i < weight_num_; ++i)
        {
            for(int j = 0; j < weight_num_; ++j)
            {
                double distance_temp = 0;
                for(int k = 0;k < g_GlobalSettings->obj_num_; k++)
                {
                    distance_temp += (lambda_[i][k]-lambda_[j][k])*(lambda_[i][k]-lambda_[j][k]);
                }
                sort_list[i].distance = sqrt(distance_temp);
                sort_list[j].index = j;
            }
            std::sort(sort_list.begin(),sort_list.end(),[](const DistanceInfo &left,const DistanceInfo &right){
                return left.distance < right.distance;
            });
            for(int j = 0; j < neighbour_num_; ++j)
            {
                neighbour_[i][j] = sort_list[j+1].index;
            }
        }
    }

    void DPBEMO_MOEAD::Crossover(Individual **parent_pop,int current_index,Individual *offspring)
    {
        int size=neighbour_type_ == NEIGHBOUR ? neighbour_num_:weight_num_;
        int parent1_index = 0, parent2_index = 0;

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
        Individual *parent1 = parent_pop[parent1_index];
        Individual *parent2 = parent_pop[parent2_index];

        SBX(parent1,parent2,g_GlobalSettings->offspring_population_[1],offspring,
        g_GlobalSettings->dec_lower_bound_,g_GlobalSettings->dec_upper_bound_,cross_para_);

    }

    void DPBEMO_MOEAD::UpdateSubproblem(Individual *offspring,int current_index,int aggregration_type)
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
                    offspring_fitness = CalInverseChebycheff(offspring, lambda_[weight_index], ideal_point_, g_GlobalSettings->obj_num_);
			        neighbour_fitness = CalInverseChebycheff(current_ind, lambda_[weight_index], ideal_point_, g_GlobalSettings->obj_num_);
                    break;
                
                case 1:
                    offspring_fitness = CalWeightedSum(current_ind,lambda_[weight_index],ideal_point_,g_GlobalSettings->obj_num_);
                    neighbour_fitness = CalWeightedSum(offspring,lambda_[weight_index],ideal_point_,g_GlobalSettings->obj_num_);
                    break;
                case2:
                    offspring_fitness = CalPBI(current_ind,lambda_[weight_index],ideal_point_,g_GlobalSettings->obj_num_,pbi_theta_);
                    neighbour_fitness = CalPBI(offspring,lambda_[weight_index],ideal_point_,g_GlobalSettings->obj_num_,pbi_theta_);
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
    }

    void DPBEMO_MOEAD::UpdateNeighbour()
    {
        DistanceInfo *sort_list = new DistanceInfo[weight_num_];
        // std::vector<DistanceInfo>sort_list(weight_num_);
        for(int i = 0; i < weight_num_; ++i)
        {
            for(int j = 0; j < weight_num_; ++j)
            {
                double distance_temp = 0;
                for(int k = 0; k<g_GlobalSettings->obj_num_; ++k)
                {
                    distance_temp += (lambda_[i][k] - lambda_[j][k]) * (lambda_[i][k] - lambda_[j][k]);
                }
                sort_list[j].distance = sqrt(distance_temp);
                sort_list[j].index = j;
            }
            std::sort(sort_list,sort_list + weight_num_,[](const DistanceInfo &left, const DistanceInfo &right){
                return left.distance < right.distance;
            });

           for(int j = 0; j < neighbour_num_; j++)
           {
            neighbour_[i][j] = sort_list[j+1].index;
           }


        }
        delete[] sort_list;
    }

    // consult the RUCB dueling bandit and return the best point index
    std::tuple<int, int, vec> DPBEMO_MOEAD::Consult_DM()
    {
        // mat PVals;
        int best;
        // PVals.load("compare.dat");
        // PSubset.load("subset.dat");

        double learning_rate = 0.51;
        int T_round = 100;
        int Budget = 40;
        double kappa = 1.0;
        // Run run(PVals, learning_rate);
        CDTS dts(PVals, PSubset,learning_rate);
        std::tuple<int, int, vec> result = dts.timestep(T_round, 0, subset_solution_num);
        // int best_subset = result.first;
        // int best_solution = result.second;
        // best = dts.timestep(T_round, 0);
       
        // best = run.winner();
        // printf("recommend area is %d\n", best_subset);
        // printf("recommend solution is %d\n", best_solution);
        return result;
    }

    // arrange the weight vectors uniformally to the 10 best weight
    void DPBEMO_MOEAD::SetBiasedWeight(double *best)
    {
        std::vector<double>dis(weight_num_);
        for(int i = 0; i < weight_num_; ++i)
        {
            dis[i] = CalEuclidianDistance(best,lambda_[i],g_GlobalSettings->obj_num_);
            // std::cout<<i<<":"<<dis[i]<<endl;
        }
        std::vector<size_t>index(dis.size());
        iota(index.begin(),index.end(),0);
        sort(index.begin(),index.end(),
            [&dis](size_t index_1,size_t index_2){return dis[index_1]<dis[index_2];});
        //find the 10 best lambda_ according to our best
        //the 10 best are index[0]~index[9]

        bool flag[weight_num_];
        memset(flag, true, weight_num_ * sizeof(bool));
        for(int i = 0; i < nPromisingWeight; ++i)
        {
            flag[index[i]] = false;//10 best need no change
            // std::cout<<index[i]<<endl;
        }
        int nSolveNum = 0;//weights have been changed
        double tempdis,minDis;
        int near_index;
        nSolveNum = nPromisingWeight;
        int nMaxChange = ceil((double)(weight_num_ - nPromisingWeight) / nPromisingWeight);
        // std::cout<<nSolveNum<<"  "<<nMaxChange<<endl;
        for(int i = 0; i < nPromisingWeight && nSolveNum < weight_num_; ++i)
        {
            int nCurrentTuned = 0;
            
            while(nCurrentTuned < nMaxChange && nSolveNum < weight_num_)
            {
                minDis = INF;
                for(int j = 0; j < weight_num_; ++j)
                {
                    if(flag[j])//flag=true haven't been changed yet
                    {
                        tempdis = CalEuclidianDistance(lambda_[index[i]], lambda_[j], g_GlobalSettings->obj_num_);
                        if(tempdis < minDis)
                        {
                            minDis = tempdis;
                            near_index = j;
                            // std::cout<<"near_index"<<near_index;
                        }
                    }
                }

                for(int j = 0; j < g_GlobalSettings->obj_num_; ++j)
                {
                    lambda_[near_index][j] += step_size_ * (lambda_[index[i]][j] - lambda_[near_index][j]);
                    // lambda_[near_index][j]=lambda_[index[i]][j];

                }
                flag[near_index] = false;
                nSolveNum++;
                nCurrentTuned++;
            }
            // std::cout<<nSolveNum<<endl;
        }
    }

    // arrange the weight vectors to the weights in best region according to winning times
    void DPBEMO_MOEAD::SetBiasedWeight()
    {
        bool flag[weight_num_];
        memset(flag, true, weight_num_ * sizeof(bool));

        // mark the best weight vectors as true, because they have no need to move
        for(int i = 0; i < weight_num_; i++)
        {
            for(int j = 0; j < subset_solution_num; j++)
            {
                if(lambda_[i] == best_weights[j])
                {
                    flag[i] = false;
                }
            }
        }
        
        // set the number of weight vectors converging to best weights according to winning times
        int *n_MaxChange = new int[subset_solution_num];
        for(int i = 0; i < subset_solution_num; i++)
        {
            n_MaxChange[i] = (int) round(win_times[i] / sum(win_times) * g_GlobalSettings->population_num_);
        }

        int nSolveNum = subset_solution_num;//weights have been changed
        double tempdis,minDis;
        int near_index;

        for(int i = 0; i < subset_solution_num && nSolveNum < weight_num_; i++)
        {
            int nCurrentTuned = 0;
            while(nCurrentTuned < n_MaxChange[i] && nSolveNum < weight_num_)
            {
                minDis = INF;
                for(int j = 0; j < weight_num_; ++j)
                {
                    if(flag[j])//flag=true haven't been changed yet
                    {
                        tempdis = CalEuclidianDistance(best_weights[i], lambda_[j], g_GlobalSettings->obj_num_);
                        if(tempdis < minDis)
                        {
                            minDis = tempdis;
                            near_index = j;
                        }
                    }
                }

                for(int j = 0; j < g_GlobalSettings->obj_num_; ++j)
                {
                    lambda_[near_index][j] += step_size_ * (best_weights[i][j] - lambda_[near_index][j]);
                    // lambda_[near_index][j]=lambda_[index[i]][j];

                }
                flag[near_index] = false;
                nSolveNum++;
                nCurrentTuned++;
            }
        }
        delete[] n_MaxChange;
    }

    // get the utility function value of all solutions
    void  DPBEMO_MOEAD::UtilityFunction_Gaussian(Individual **pop)
    {
        double **f = new double*[g_GlobalSettings->population_num_];
        double *w = new double[g_GlobalSettings->obj_num_];
        double max = 0.0;
        int max_index = 0;
        // set weight for utility in range 0.4~0.6
        std::random_device rd;  // generate seed
        std::mt19937 mt(rd());  // use seed initialize generator
        std::uniform_real_distribution<double> prob(0.4,0.6);
        for(int i = 0; i < g_GlobalSettings->obj_num_; i++)
        {
            w[i] = prob(mt);
            // printf("w[%d]=%f\n",i,w[i]);
        }
        for(int i = 0; i < g_GlobalSettings->population_num_; i++)
        {
            f[i] = new double[g_GlobalSettings->obj_num_];
        }

        for(int i = 0; i < g_GlobalSettings->population_num_; i++)
        {
            for(int j = 0;j < g_GlobalSettings->obj_num_; j++)
            {
                // std::default_random_engine generator;
                // std::normal_distribution<double> distribution(weight[j], 0.5);
                // f[i][j] = pow(c[j] * pop[i]->obj_[j] - 1, 3);
                f[i][j] =  1 / (sigma * sqrt(2*PI)) * exp((double)-1/2* pow ((pop[i]->obj_[j] - weight[j]) / sigma, 2));

            }
        }

        for(int i = 0;i < g_GlobalSettings->population_num_; i++)
        {
            // cout << "i" << i << endl;
            utl[i] = 0;
            for(int j = 0;j < g_GlobalSettings->obj_num_; j++)
            {
                utl[i] += w[j] * f[i][j];
            }
            // cout << "utility" << utl[i] << endl;
            if(utl[i] > max)
            {
                max = utl[i];
                max_index = i;
                // cout << "max = " << max ;
                // cout << "max_index:" <<max_index << endl;
            }
            // if(g_GlobalSettings->obj_num_ == 3)
            //     utl[i] *= 1e6;
        }
        cout << "max = " << max ;
        cout << "max_index:" <<max_index << endl;
        printf("the best point is :\n");
        for(int i = 0; i < g_GlobalSettings->obj_num_; i++)
        {
            printf("%.2f ", pop[max_index]->obj_[i]);   
        }
        printf("\n");

        for(int i = 0;i < g_GlobalSettings->population_num_; i++)
        {
            delete[] f[i];
            f[i] = nullptr;
        }
        delete[] f;
        delete[] w;
        f = nullptr;
        w = nullptr;
    }

    // uniformly choose 10 compare objects, and store there utility function difference in a dat file
    // store the pairwise comparison results of all solutions in the form of utility function difference, 
    // and divide them into subsets in which there are 10 points, also store the subsets comparison results
    void DPBEMO_MOEAD::StorePreference(Individual **pop)
    {
        
        std::vector<size_t>index(g_GlobalSettings->population_num_);
        
        
        // std::vector<size_t>compare(select_sum);
        iota(index.begin(),index.end(),0);
        std::sort(index.begin(),index.end(),
            [&pop](size_t index1, size_t index2){return pop[index1]->obj_[0] > pop[index2]->obj_[0];});


        int select_interval = ceil(g_GlobalSettings->population_num_ / select_sum);
        int selected = 0;

        PVals = mat(g_GlobalSettings->population_num_, g_GlobalSettings->population_num_);
        for(int i = 0; i < g_GlobalSettings->population_num_; i++)
        {
            for(int j = 0;j < g_GlobalSettings->population_num_; j++)
            {
                double diff = utl[index[i]] - utl[index[j]];          
                // PVals(i, j) = diff;
                PVals(i, j) = 1 / (1 + exp(-1000* diff));
            }
        }
        // PVals.save("compare.dat",raw_ascii);

        // subset comparison
        int num_subset = ceil(g_GlobalSettings->population_num_ / subset_solution_num);
        // cout << num_subset << endl;
        PSubset = mat(num_subset, num_subset);
        
        for(int i = 0; i < num_subset; i++)
        {
            for(int j = 0; j < num_subset; j++)
            {
                if(i == j)
                    PSubset(i,j) = 0.5;
                else
                {
                    mat temp = PVals.submat(span(subset_solution_num * i, subset_solution_num * (i + 1) - 1), 
                    span(subset_solution_num * j, subset_solution_num * (j + 1) - 1));
                    PSubset(i,j) = mean(mean(temp));
                }
            }
        }
        // PSubset.save("subset.dat",raw_ascii);
    }


    float DPBEMO_MOEAD::CalculateKLDivergence(Individual **pop)
	{
		// reinitialize pref_prob2 to 0
		for(int i = 0; i < g_GlobalSettings->population_num_; i++)
		{
			pref_prob2[i] = 0;
		} 
		// sort current population descendently according to obj0
		std::vector<size_t>index(g_GlobalSettings->population_num_);
        iota(index.begin(),index.end(),0);
        std::sort(index.begin(),index.end(),
            [&pop](size_t index1, size_t index2){return pop[index1]->obj_[0] > pop[index2]->obj_[0];});

		// calculate pref_prob2 according to updated utility distribution
        sigma = (double) var(win_times / win_times.max());
		for(int i = 0 ; i < g_GlobalSettings->population_num_; i++)
		{
			for(int j = 0; j < g_GlobalSettings->obj_num_; j++)
			{
				pref_prob2[i] += UpdateUtilityPrediction(g_GlobalSettings->parent_population_[index[i]]->obj_[j], j);
			}
		}

		// normalize pref_prob2
		double max_prob = 0;
		double min_prob = EMOC_INF;
		double sum = 0.0;
		for(int i = 0; i < g_GlobalSettings->population_num_; i++)
		{
			sum += pref_prob2[i];
		}

		for(int i = 0; i < g_GlobalSettings->population_num_; i++)
		{
			pref_prob2[i] = pref_prob2[i] / sum;
		}

		// calculate KL divergence between pref_prob1 and pref_prob2
		float KLDivergence = 0.0;
		for(int i = 0; i < g_GlobalSettings->population_num_; i++)
		{
			// if(abs(pref_prob2[i] - pref_prob1[i]) < 0.1)
			if(pref_prob2[i] == 0.0)
				pref_prob2[i] += 1e-6;
			
			
			KLDivergence += pref_prob2[i] * log(pref_prob2[i] / pref_prob1[i]);
		
		}

		// store pref_prob2 value in pref_prob1 for next calculation
		for(int i = 0; i < g_GlobalSettings->population_num_; i++)
		{
			pref_prob1[i] = pref_prob2[i];
		}

		return KLDivergence;
	}

    double DPBEMO_MOEAD::UpdateUtilityPrediction(double obj, int index)
	{
		double probability = 0.0;
		// U = gamma * U + u_c
        
        
		for(int i = 0; i < count; i ++)
		{
			double u = 0;
			// for(int j = 0; j < g_GlobalSettings->obj_num_; j ++)
			// {
			// 	 u += 1 / (sigma * sqrt(2*PI)) * exp((double)-1/2* pow ((obj - recommend_point[i][j]) / sigma, 2));
			// }
			u = 1.0 / (sigma * sqrt(2*PI)) * exp((double)-1/2* pow ((obj - recommend_point[i][index]) / sigma, 2));
			probability = probability * gamma + u;
		}
		return probability;
	}

    void DPBEMO_MOEAD::CalculateMinMaxDis(Individual **pop)
    {
        minDis = INF;
        maxDis = 0;
        for(int i = 0; i < g_GlobalSettings->population_num_; i++)
        {
            double tempDis = CalEuclidianDistance(pop[i]->obj_.data(), weight, g_GlobalSettings->obj_num_);
            if(tempDis < minDis)
            {
                minDis = tempDis;
            }
            if(tempDis > maxDis)
            {
                maxDis = tempDis;
            }
        }

    }

}