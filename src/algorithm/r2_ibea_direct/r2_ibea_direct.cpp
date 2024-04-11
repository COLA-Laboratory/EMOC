// Algorithm: D-PBEMO-R2-IBEA
// Paper: (ICML 2024 under review) Huang, Tian, and Ke Li. "Direct Preference-Based Evolutionary Multi-Objective Optimization with Dueling Bandit." arXiv preprint arXiv:2311.14003 (2023).
// Link: https://arxiv.org/abs/2311.14003
// Created by Tian Huang 11/04/2024

#include "algorithm/r2_ibea_direct/r2_ibea_direct.h"

#include <cmath>
#include <ctime>
#include <iostream>
#include <algorithm>

#include "core/file.h"
#include "core/global.h"
#include "operator/tournament_selection.h"
#include "operator/sbx.h"
#include "operator/polynomial_mutation.h"
#include "random/random.h"
#include "core/utility.h"
#include "core/uniform_point.h"

#include <numeric>  //iota
#include "../vendor/C-DTS/stdafx.h"
#include "../vendor/C-DTS/CDTS.h"
using namespace std;

namespace emoc {

	DPBEMO_R2_IBEA::DPBEMO_R2_IBEA(int thread_id):
    Algorithm(thread_id),
    kappa(0.05),
    ideal_point(new double[g_GlobalSettings->obj_num_]),
    lambda_(nullptr),
    weight(nullptr),
    step_size_(0.3),
    retention_rate_(0.2),
    score_RankNet_(new double[g_GlobalSettings->population_num_]),
    sigma(0.5),
    utl(new double[g_GlobalSettings->population_num_]),
    select_sum(10),
    max_consultation(10),
    nPromisingWeight(10),
    util_prob1(new double[g_GlobalSettings->population_num_]),
	util_prob2(new double[g_GlobalSettings->population_num_]),
    gamma(0.9),
    best_index(0),
    best_region(0),
    count(0),
    subset_solution_num(10),
    win_times(subset_solution_num, fill::zeros),
    best_weight(new double[g_GlobalSettings->obj_num_]),
    best_weights(nullptr)
	{
		real_popnum_ = g_GlobalSettings->population_num_;
	}

	DPBEMO_R2_IBEA::~DPBEMO_R2_IBEA()
	{
        for(int i = 0; i < g_GlobalSettings->population_num_; i++)
        {
            delete[] lambda_[i];
            lambda_[i] = nullptr;
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
        delete[] ideal_point;
        delete[] score_RankNet_;
        delete[] weight;
        delete[] utl;
        delete[] recommend_point;
        delete[] util_prob1;
        delete[] util_prob2;
        delete[] best_weight;
        delete[] best_weights;
        lambda_ = nullptr;
        ideal_point = nullptr;
        score_RankNet_ = nullptr;
        weight = nullptr;
        utl = nullptr;
        recommend_point = nullptr;
        util_prob1 = nullptr;
        util_prob2 = nullptr;
        best_weight = nullptr;
        best_weights = nullptr;

        while (winners_.size())
        {
            delete[] winners_.front();
            winners_.pop();
            delete[] losers_.front();
            losers_.pop();
        }
        

	}

	void DPBEMO_R2_IBEA::Solve()
	{
		Initialization();
        int first_tau = 0.5 * g_GlobalSettings->max_evaluation_ / g_GlobalSettings->population_num_;
        char usingPref = false;
        std::vector<size_t>index_pop(g_GlobalSettings->population_num_);

		while (!IsTermination())
		{
            
            Individual** pop = g_GlobalSettings->parent_population_.data();
            iota(index_pop.begin(),index_pop.end(),0);
            std::sort(index_pop.begin(),index_pop.end(), [&pop](size_t index1, size_t index2){return pop[index1]->obj_[0] > pop[index2]->obj_[0];});
            
            if(count == 0 && g_GlobalSettings->iteration_num_ >= first_tau)
            {
                printf("the %d time to consult in generation %d \n", count + 1, g_GlobalSettings->iteration_num_);
                UtilityFunction_Gaussian(g_GlobalSettings->parent_population_.data());
                StorePreference(g_GlobalSettings->parent_population_.data());

                std::tuple<int, int, vec> result;
                result = Consult_DM();
                std::tie(best_region, best_index, win_times)= result;

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

				count++;
                usingPref = true;

                // SetBiasedWeight(g_GlobalSettings->parent_population_[index_pop[best_index]]->obj_.data());
                // SetBiasedWeight();
                // UpdateNeighbour();
                
            }
            if(count && count < max_consultation)
            {
                double KL = CalculateKLDivergence(g_GlobalSettings->parent_population_.data());
                // if(KL > 1e-2)
                if(KL > 1e-6)
                {
                    printf("KL = %f \n", KL);
					printf("the %d time to consult in generation %d\n", count + 1, g_GlobalSettings->iteration_num_);
					UtilityFunction_Gaussian(g_GlobalSettings->parent_population_.data());
					StorePreference(g_GlobalSettings->parent_population_.data());
					
					std::tuple<int, int, vec> result;
                    result = Consult_DM();
                    std::tie(best_region, best_index, win_times)= result;

					printf("the recommend population is inedx %d: ", index_pop[best_index]);
                    for(int i = 0; i < g_GlobalSettings->obj_num_; i++)
                    {
                        cout << g_GlobalSettings->parent_population_[index_pop[best_index]]->obj_[i];
                        recommend_point[count][i] = g_GlobalSettings->parent_population_[index_pop[best_index]]->obj_[i];
                        best_weight[i] = lambda_[index_pop[best_index]][i];
                    }

                    for(int i = 0; i < subset_solution_num; i++)
                    {    
                        for(int j = 0; j < g_GlobalSettings->obj_num_; j++)
                            best_weights[i][j] = lambda_[index_pop[best_region * subset_solution_num + i]][j];
                    }

					cout << endl;
					count++;
                    
                    usingPref = true;

                    // SetBiasedWeight();
                    // SetBiasedWeight(g_GlobalSettings->parent_population_[index_pop[best_index]]->obj_.data());
                    // UpdateNeighbour();
                }
            }
            if(count)
                SetBiasedWeight();
            
			// generate offspring population
			Crossover(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->offspring_population_.data());
			PolynomialMutation(g_GlobalSettings->offspring_population_.data(), 2 * g_GlobalSettings->population_num_ / 2, g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, mutation_para_);
			EvaluatePop(g_GlobalSettings->offspring_population_.data(), 2 * g_GlobalSettings->population_num_ / 2);
			MergePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->offspring_population_.data(),
				2 * g_GlobalSettings->population_num_ / 2, g_GlobalSettings->mixed_population_.data());
			
            // update ideal point
            UpdateIdealpoint(g_GlobalSettings->offspring_population_.data(),g_GlobalSettings->population_num_, ideal_point, g_GlobalSettings-> obj_num_);


			// select next generation's population
			EnvironmentalSelection(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->mixed_population_.data());
		}
        // cout << lambda_[0][0] << lambda_[0][1] << endl;
        printf("total consultation is %d times\n", count);
        // CalculateMinDis(weight, g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->obj_num_, g_GlobalSettings->dec_num_, g_GlobalSettings->problem_name_, g_GlobalSettings->algorithm_name_);
        // CalculateExpectDis(weight, g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->obj_num_, g_GlobalSettings->dec_num_, g_GlobalSettings->problem_name_, g_GlobalSettings->algorithm_name_);
    }

	void DPBEMO_R2_IBEA::Initialization()
	{
        weight_num_ = g_GlobalSettings->population_num_;
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

        // weight = SetWeight(g_GlobalSettings->weight_StringType_);
        weight = SetWeight(g_GlobalSettings->obj_num_, g_GlobalSettings->algorithm_name_);

        // initialize recommend point
		recommend_point = new double*[max_consultation];
		for(int i = 0; i < max_consultation; i++)
		{
			recommend_point[i] = new double[g_GlobalSettings->obj_num_];
		}
        
        char file[256];
        sprintf(file,"./UniformWeights/%dd_%d.txt",g_GlobalSettings->obj_num_,g_GlobalSettings->population_num_);
        lambda_=LoadUniformWeights(g_GlobalSettings->population_num_,g_GlobalSettings->obj_num_,file);
        // lambda_=UniformPoint(g_GlobalSettings->population_num_, &weight_num_,g_GlobalSettings->obj_num_);

        best_weights = new double*[subset_solution_num];
        for(int i = 0; i < subset_solution_num; i++)
        {
            best_weights[i] = new double[g_GlobalSettings->obj_num_];
        }

        UpdateIdealpoint(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, ideal_point, g_GlobalSettings->obj_num_);

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 20.0;
	}

	void DPBEMO_R2_IBEA::Crossover(Individual **parent_pop, Individual **offspring_pop)
	{
		// generate random permutation index for tournment selection
		std::vector<int> index1(g_GlobalSettings->population_num_);
		std::vector<int> index2(g_GlobalSettings->population_num_);
		random_permutation(index1.data(), g_GlobalSettings->population_num_);
		random_permutation(index2.data(), g_GlobalSettings->population_num_);

		for (int i = 0; i < g_GlobalSettings->population_num_ / 2; ++i)
		{
			// Individual *parent1 = TournamentByFitness(parent_pop[index1[2 * i]], parent_pop[index1[2 * i + 1]]);
			// Individual *parent2 = TournamentByFitness(parent_pop[index2[2 * i]], parent_pop[index2[2 * i + 1]]);
            Individual *parent1 = TournamentByBiR2Indicator(parent_pop[index1[2 * i]], parent_pop[index1[2 * i + 1]]);
            Individual *parent2 = TournamentByBiR2Indicator(parent_pop[index2[2 * i]], parent_pop[index2[2 * i + 1]]);
			SBX(parent1, parent2, offspring_pop[2 * i], offspring_pop[2 * i + 1],
				g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, cross_para_);
		}
    
	}

	double DPBEMO_R2_IBEA::CalEpsIndicator(Individual *ind1, Individual *ind2)
	{
		double max_eps = 0.0, temp_eps = 0.0;
		double r = g_GlobalSettings->dec_upper_bound_[0] - g_GlobalSettings->dec_lower_bound_[0];

		max_eps = (ind1->obj_[0] - g_GlobalSettings->dec_upper_bound_[0]) / r - (ind2->obj_[0] - g_GlobalSettings->dec_upper_bound_[0]) / r;
		for (int i = 1; i < g_GlobalSettings->obj_num_; ++i)
		{
			r = g_GlobalSettings->dec_upper_bound_[i] - g_GlobalSettings->dec_lower_bound_[1];
			temp_eps = (ind1->obj_[i] - g_GlobalSettings->dec_upper_bound_[i]) / r - (ind2->obj_[i] - g_GlobalSettings->dec_upper_bound_[i]) / r;

			if (temp_eps > max_eps)
				max_eps = temp_eps;
		}

		return max_eps;
	}

	void DPBEMO_R2_IBEA::CalFitness(Individual **pop, int pop_num, std::vector<double> &fitness)
	{
		// determine indicator values and their maximum
		double max_fitness = 0;
		for (int i = 0; i < pop_num; ++i)
		{
			for (int j = 0; j < pop_num; ++j)
			{
				// fitness[i * pop_num + j] = CalEpsIndicator(pop[i], pop[j]);
                fitness[i * pop_num + j] = CalBiR2Indicator(pop[i], pop[j]);
				if (max_fitness < fabs(fitness[i * pop_num + j]))
					max_fitness = fabs(fitness[i * pop_num + j]);
			}
		}

		// calculate for each pair of individuals the corresponding value component
		for (int i = 0; i < pop_num;++i)
			for (int j = 0; j < pop_num; j++)
				fitness[i * pop_num + j] = exp((-fitness[i * pop_num + j] / max_fitness) / kappa);

		// set individual's fitness
		for (int i = 0; i < pop_num; ++i)
		{
			double sum = 0;
			for (int j = 0; j < pop_num; ++j)
				if (i != j)
					sum += fitness[j * pop_num + i];

			pop[i]->fitness_ = sum;
		}
	}

	void DPBEMO_R2_IBEA::EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop)
	{
		int mixed_popnum = g_GlobalSettings->population_num_ + 2 * g_GlobalSettings->population_num_ / 2;

		// calculate fitness and store it in fitness array
		std::vector<double> fitness(mixed_popnum * mixed_popnum);
		CalFitness(mixed_pop, mixed_popnum, fitness);

		// select next generation's individuals
		int worst = -1;
		std::vector<int> flag(mixed_popnum,0);

		for (int i = g_GlobalSettings->population_num_; i > 0; --i)
		{
			int j = 0;
			for (j = 0; j < mixed_popnum && flag[j] == 1; ++j);
			worst = j;

			for (j = j + 1; j < mixed_popnum; ++j)
			{
				if (flag[j] != 1)
				{
					if (mixed_pop[j]->fitness_ > mixed_pop[worst]->fitness_)
						worst = j;
				}
			}

			for (j = 0; j < mixed_popnum; ++j)
			{
				if (flag[j] != 1)
				{
					mixed_pop[j]->fitness_ -= fitness[worst * mixed_popnum + j];
				}
			}

			flag[worst] = 1;
		}

		int n = 0;
		for (int i = 0; i < mixed_popnum; ++i)
		{
			if (flag[i] != 1)
				CopyIndividual(mixed_pop[i], parent_pop[n++]);
		}
	}

    double DPBEMO_R2_IBEA::CalR2Indicator(Individual *x)
    {
        double res = 0;
        double a, ChebycheffValue;
        for (int i = 0; i < g_GlobalSettings->population_num_; ++i)
        {
            ChebycheffValue = -INF;
            for (int j = 0; j < g_GlobalSettings->obj_num_; ++j)
            {
                if(count)
                    // a = fabs(0 - x->obj_[j]) / lambda_[i][j];
                    a = fabs(weight[j] - x->obj_[j]) / lambda_[i][j];
                    // a = fabs(round(10 * recommend_point[count - 1][i]) / 10 - x->obj_[j]) / lambda_[i][j];
                else
                    // a = fabs(0 - x->obj_[j]) / lambda_[i][j];
                    a = fabs(ideal_point[j] - x->obj_[j]) / lambda_[i][j];
                // a = fabs(ideal_point[j] - x->obj_[j]) * lambda_[i][j];
                if (a > ChebycheffValue)
                {
                    ChebycheffValue = a;
                }
            }
            res += ChebycheffValue;
        }
        res /= g_GlobalSettings->population_num_;
        return res;
    }

    double DPBEMO_R2_IBEA::CalR2Indicator(Individual *x, Individual *y)
    {
        double res = 0;
        double a, b, ChebycheffValue_x, ChebycheffValue_y;
        for (int i = 0; i < g_GlobalSettings->population_num_; ++i)
        {
            ChebycheffValue_x = -INF;
            ChebycheffValue_y = -INF;
            for (int j = 0; j < g_GlobalSettings->obj_num_; ++j)
            {
                if(count)
                {
                    // a = fabs(0 - x->obj_[j]) / lambda_[i][j];
                    // b = fabs(0 - y->obj_[j]) / lambda_[i][j];
                    // a = fabs(round(recommend_point[count - 1][j] * 10) / 10 - x->obj_[j]) / lambda_[i][j];
                    // b = fabs(round(recommend_point[count - 1][j] * 10) / 10 - y->obj_[j]) / lambda_[i][j];
                    a = fabs(weight[j] - x->obj_[j]) / lambda_[i][j];
                    b = fabs(weight[j] - y->obj_[j]) / lambda_[i][j];
                }
                else
                {
                    a = fabs(0 - x->obj_[j]) / lambda_[i][j];
                    b = fabs(0 - y->obj_[j]) / lambda_[i][j];
                    // a = fabs(ideal_point[j] - x->obj_[j]) / lambda_[i][j];
                    // b = fabs(ideal_point[j] - y->obj_[j]) / lambda_[i][j];
                }
                
                // a = fabs(ideal_point[j] - x->obj_[j]) * lambda_[i][j];
                // b = fabs(ideal_point[j] - y->obj_[j]) * lambda_[i][j];
                if (a > ChebycheffValue_x)
                {
                    ChebycheffValue_x = a;
                }
                if (b > ChebycheffValue_y)
                {
                    ChebycheffValue_y = b;
                }
            }
            if (ChebycheffValue_x < ChebycheffValue_y)
            {
                res += ChebycheffValue_x;
            }
            else
            {
                res += ChebycheffValue_y;
            }
        }
        res /= g_GlobalSettings->population_num_;
        return res;
    }

    double DPBEMO_R2_IBEA::CalBiR2Indicator(Individual *x, Individual *y)
    {
        return CalR2Indicator(x) - CalR2Indicator(x,y);
    }

    Individual* DPBEMO_R2_IBEA::TournamentByBiR2Indicator(Individual *ind1, Individual *ind2)
    {
        double I_R2_1,I_R2_2;
        I_R2_1 = CalBiR2Indicator(ind1, ind2);
        I_R2_2 = CalBiR2Indicator(ind2, ind1);
        /*if (I_R2_1 < I_R2_2)
        {
            #if DEBUG
            std::cout<<"[INFO] Select parent 1."<<std::endl;
            #endif
            return ind1;
        }
        else if(I_R2_1 > I_R2_2)
        {
            #if DEBUG
            std::cout<<"[INFO] Select parent 2."<<std::endl;
            #endif
            return ind2;
        }*/
        if (I_R2_1 == 0)
        {
            return ind1;
        }
        else if(I_R2_2 == 0)
        {
            return ind2;
        }
        else
        {
            #if DEBUG
            std::cout<<"[INFO] Select parent randomly."<<std::endl;
            #endif
            if (randomperc() <= 0.5)
            {
                return (ind1);
            }
            else
            {
                return (ind2);
            }
        }
    }

    // get the utility function value of all solutions
    void  DPBEMO_R2_IBEA::UtilityFunction_Gaussian(Individual **pop)
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
            printf("%.2f\n", pop[max_index]->obj_[i]);   
        }

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
    void DPBEMO_R2_IBEA::StorePreference(Individual **pop)
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
                    mat temp = PVals.submat(span(subset_solution_num * i, subset_solution_num * i + 9), 
                    span(subset_solution_num * j, subset_solution_num * j + 9));
                    PSubset(i,j) = mean(mean(temp));
                }
            }
        }
        // PSubset.save("subset.dat",raw_ascii);
    }

    // consult the RUCB dueling bandit and return the best point index
    std::tuple<int, int, vec> DPBEMO_R2_IBEA::Consult_DM()
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
        return result;
    }

    // arrange the weight vectors uniformally to the 10 best weight
    void DPBEMO_R2_IBEA::SetBiasedWeight(double *best)
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
    void DPBEMO_R2_IBEA::SetBiasedWeight()
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

    float DPBEMO_R2_IBEA::CalculateKLDivergence(Individual **pop)
	{
		// reinitialize util_prob2 to 0
		for(int i = 0; i < g_GlobalSettings->population_num_; i++)
		{
			util_prob2[i] = 0;
		}
		// sort current population descendently according to obj0
		std::vector<size_t>index(g_GlobalSettings->population_num_);
        iota(index.begin(),index.end(),0);
        std::sort(index.begin(),index.end(),
            [&pop](size_t index1, size_t index2){return pop[index1]->obj_[0] > pop[index2]->obj_[0];});

		// calculate util_prob2 according to updated utility distribution
        sigma = (double) var(win_times / win_times.max());
		for(int i = 0 ; i < g_GlobalSettings->population_num_; i++)
		{
			for(int j = 0; j < g_GlobalSettings->obj_num_; j++)
			{
				util_prob2[i] += UpdateUtilityPrediction(g_GlobalSettings->parent_population_[index[i]]->obj_[j], j);
			}
		}

		// normalize util_prob2
		double max_prob = 0;
		double min_prob = EMOC_INF;
		double sum = 0.0;
		for(int i = 0; i < g_GlobalSettings->population_num_; i++)
		{
			sum += util_prob2[i];
		}

		for(int i = 0; i < g_GlobalSettings->population_num_; i++)
		{
			util_prob2[i] = util_prob2[i] / sum;
		}
		
	

		// calculate KL divergence between util_prob1 and util_prob2
		float KLDivergence = 0.0;
		for(int i = 0; i < g_GlobalSettings->population_num_; i++)
		{
			// if(abs(util_prob2[i] - util_prob1[i]) < 0.1)
			if(util_prob2[i] == 0.0)
				util_prob2[i] += 1e-6;
			
			
			KLDivergence += util_prob2[i] * log(util_prob2[i] / util_prob1[i]);
		
		}

		// store util_prob2 value in util_prob1 for next calculation
		for(int i = 0; i < g_GlobalSettings->population_num_; i++)
		{
			util_prob1[i] = util_prob2[i];
		}

		return KLDivergence;
	}

    double DPBEMO_R2_IBEA::UpdateUtilityPrediction(double obj, int index)
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

    

}