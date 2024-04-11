#include "algorithm/nsga2_dts/nsga2_dts.h"


#include <algorithm>
#include <iostream>
// #include <ctime>
// #include <cmath>
// #include <vector>
#include <random>

#include "core/macro.h"
#include "core/global.h"
#include "core/file.h"
#include "core/nd_sort.h"
#include "operator/tournament_selection.h"
#include "operator/polynomial_mutation.h"
#include "operator/sbx.h"
#include "random/random.h"
#include "core/utility.h"
#include "../vendor/C-DTS/stdafx.h"
#include "../vendor/C-DTS/DTS.h"

#include <numeric> //iota

using namespace std;

namespace emoc {

	DPBEMO_DTS::DPBEMO_DTS(int thread_id) :
    Algorithm(thread_id),
    pf_size(0),
	usingPref(false),
	count(0),
	weight(new double[g_GlobalSettings->obj_num_]),
	sigma(0.5),
	utl(new double[g_GlobalSettings->population_num_]),
	select_sum(10),
    subset_solution_num(10),
	best_index(0),
    // best_region(0),
    // win_times(subset_solution_num, fill::zeros),
	KL(0),
	gamma(0.9),
	max_consultation(20),
	pref_prob1(new double[g_GlobalSettings->population_num_]),
	pref_prob2(new double[g_GlobalSettings->population_num_])      
	{

	}

	DPBEMO_DTS::~DPBEMO_DTS()
	{
		// free memory for pf_data
        for (int i = 0; i < pf_size; ++i)
		{
			delete[] pf_data[i];
			pf_data[i] = nullptr;
		}
			
		for(int i = 0; i < max_consultation; i++)
		{
			delete[] recommend_point[i];
			recommend_point[i] = nullptr;
		}
			
		delete[] pf_data;
		delete[] weight;
		delete[] utl;
		delete[] recommend_point;
		delete[] pref_prob1;
		delete[] pref_prob2;
		pf_data = nullptr;
		weight = nullptr;
		utl = nullptr;
		recommend_point = nullptr;
		pref_prob1 = nullptr;
		pref_prob2 = nullptr;

	}

	void DPBEMO_DTS::Solve()
	{
		Initialization();
		int first_tau = 0.5 * g_GlobalSettings->max_evaluation_ / g_GlobalSettings->population_num_;
		std::vector<size_t>index_pop(g_GlobalSettings->population_num_);

		while (!IsTermination())
		{
			usingPref = false;
			Individual** pop = g_GlobalSettings->parent_population_.data();
            iota(index_pop.begin(),index_pop.end(),0);
            std::sort(index_pop.begin(),index_pop.end(), [&pop](size_t index1, size_t index2){return pop[index1]->obj_[0] > pop[index2]->obj_[0];});

			// first time to consult, after using 50% evaluation budget
			if(g_GlobalSettings->iteration_num_ >= first_tau && count == 0)
			{
				printf("the %d time to consult in generation %d \n", count + 1, g_GlobalSettings->iteration_num_);
				UtilityFunction_Gaussian(g_GlobalSettings->parent_population_.data());
				StorePreference(g_GlobalSettings->parent_population_.data());

				// std::tuple<int, int, vec> result;
                // result = Consult_DM();
                // std::tie(best_region, best_index, win_times)= result;
                // cout << "The variance in winning time vector" << var(win_times) << endl;
                int best_index = Consult_DM();

                printf("the recommend population is inedx %d: ", index_pop[best_index]);
				for(int i = 0; i < g_GlobalSettings->obj_num_; i++)
				{
					cout << g_GlobalSettings->parent_population_[index_pop[best_index]]->obj_[i];
					recommend_point[count][i] = g_GlobalSettings->parent_population_[index_pop[best_index]]->obj_[i];
				}
				cout << endl;
				count++;
				// cout << "consult num:" << count << endl;
				usingPref = true;
				
			}
			// double KL = CalculateKLDivergence(g_GlobalSettings->parent_population_.data());
			// printf("KL = %f \n", KL);
			if(count && count < 10)
			{
				double KL = CalculateKLDivergence(g_GlobalSettings->parent_population_.data());
				// printf("KL = %f \n", KL);
				if(KL > 1e-3)
				{
					printf("KL = %f \n", KL);
					printf("the %d time to consult in generation %d\n", count + 1, g_GlobalSettings->iteration_num_);
					UtilityFunction_Gaussian(g_GlobalSettings->parent_population_.data());
					StorePreference(g_GlobalSettings->parent_population_.data());
					// ActiveStorePreference(g_GlobalSettings->parent_population_.data());
					// std::tuple<int, int, vec> result;
                    // result = Consult_DM();
                    // std::tie(best_region, best_index, win_times)= result;
                    // cout << "The variance in winning time vector" << var(win_times) << endl;
                    int best_index = Consult_DM();

					printf("the recommend population is inedx %d: ", index_pop[best_index]);
                    for(int i = 0; i < g_GlobalSettings->obj_num_; i++)
                    {
                        cout << g_GlobalSettings->parent_population_[index_pop[best_index]]->obj_[i];
                        recommend_point[count][i] = g_GlobalSettings->parent_population_[index_pop[best_index]]->obj_[i];
                    }
					cout << endl;
					count++;
					// cout << "consult num:" << count << endl;
					usingPref = true;
				}
				
			}

			
			// generate offspring population
			Crossover(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->offspring_population_.data());
			PolynomialMutation(g_GlobalSettings->offspring_population_.data(), 2 * (real_popnum_ / 2), g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, mutation_para_);
			EvaluatePop(g_GlobalSettings->offspring_population_.data(), 2 * (real_popnum_ / 2));
			MergePopulation(g_GlobalSettings->parent_population_.data(), real_popnum_, g_GlobalSettings->offspring_population_.data(),
				2 * (real_popnum_ / 2), g_GlobalSettings->mixed_population_.data());
			
			// select next generation's population
			bool envSelect = EnvironmentalSelection(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->mixed_population_.data());
			if(!envSelect)
            {
                std::cerr<<"[ERROR] Wrong in EnvironmentalSelection!\n";
                return;
            }
			// CalculateTotalDis(weight);
		}
		printf("total consultation is %d times\n", count + 1);
		// CalculateMinDis(weight, g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->obj_num_, g_GlobalSettings->dec_num_, g_GlobalSettings->problem_name_, g_GlobalSettings->algorithm_name_);
        // CalculateExpectDis(weight, g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->obj_num_, g_GlobalSettings->dec_num_, g_GlobalSettings->problem_name_, g_GlobalSettings->algorithm_name_);
	}

	void DPBEMO_DTS::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		
		// initialize the weight for DM
		// weight = SetWeight(g_GlobalSettings->weight_StringType_);
        weight = SetWeight(g_GlobalSettings->obj_num_, g_GlobalSettings->problem_name_);
		
        // initialize hv calculator
        hv_calculator_.Init(g_GlobalSettings->obj_num_, g_GlobalSettings->population_num_);
        pf_data = LoadPFData(pf_size, g_GlobalSettings->obj_num_, g_GlobalSettings->problem_name_);
        printf("pf_size:%d\n", pf_size);

		// initialize recommend point
		recommend_point = new double*[max_consultation];
		for(int i = 0; i < max_consultation; i++)
		{
			recommend_point[i] = new double[g_GlobalSettings->obj_num_];
		}

		// initialize probability1 and probability2
		for(int i = 0; i < g_GlobalSettings->population_num_; i++)
		{
			pref_prob1[i] = (double)1./g_GlobalSettings->population_num_;
			pref_prob2[i] = 0;
		}

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 20.0;
	}

	void DPBEMO_DTS::Crossover(Individual **parent_pop, Individual **offspring_pop)
	{
		// generate random permutation index for tournment selection
		std::vector<int> index1(g_GlobalSettings->population_num_);
		std::vector<int> index2(g_GlobalSettings->population_num_);
		random_permutation(index1.data(), g_GlobalSettings->population_num_);
		random_permutation(index2.data(), g_GlobalSettings->population_num_);

		for (int i = 0; i < g_GlobalSettings->population_num_ / 2; ++i)
		{
			Individual *parent1 = TournamentByRank(parent_pop[index1[2 * i]], parent_pop[index1[2 * i + 1]]);
			Individual *parent2 = TournamentByRank(parent_pop[index2[2 * i]], parent_pop[index2[2 * i + 1]]);
			SBX(parent1, parent2, offspring_pop[2 * i], offspring_pop[2 * i + 1],
				g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, cross_para_);
		}
	}
	
	void DPBEMO_DTS::SetDistanceInfo(std::vector<DistanceInfo> &distanceinfo_vec, int target_index, double distance)
	{
		// search the target_index and set it's distance
		for (int i = 0; i < distanceinfo_vec.size(); ++i)
		{
			if (distanceinfo_vec[i].index == target_index)
			{
				distanceinfo_vec[i].distance += distance;
				break;
			}
		}
	}


	int DPBEMO_DTS::CrowdingDistance(Individual **mixed_pop,  int pop_num, int *pop_sort, int rank_index)
	{
		int num_in_rank = 0;
		std::vector<int> sort_arr(pop_num, 0);
		std::vector<DistanceInfo> distanceinfo_vec(pop_num, { -1,0.0 });

		// find all the indviduals with rank rank_index
		for (int i = 0; i < pop_num; i++)
		{
			mixed_pop[i]->fitness_ = 0;
			if (mixed_pop[i]->rank_ == rank_index)
			{
				distanceinfo_vec[num_in_rank].index = i;
				sort_arr[num_in_rank] = i;
				num_in_rank++;
			}
		}

		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
		{
			// sort the population with i-th obj
			std::sort(sort_arr.begin(), sort_arr.begin()+num_in_rank, [=](int left, int right){
				return mixed_pop[left]->obj_[i] < mixed_pop[right]->obj_[i];
			});

			// set the first and last individual with INF fitness (crowding distance)
			mixed_pop[sort_arr[0]]->fitness_ = EMOC_INF;
			SetDistanceInfo(distanceinfo_vec, sort_arr[0], EMOC_INF);
			mixed_pop[sort_arr[num_in_rank - 1]]->fitness_ = EMOC_INF;
			SetDistanceInfo(distanceinfo_vec, sort_arr[num_in_rank - 1], EMOC_INF);

			// calculate each solution's crowding distance
			for (int j = 1; j < num_in_rank - 1; j++)
			{
				if (EMOC_INF != mixed_pop[sort_arr[j]]->fitness_)
				{
					if (mixed_pop[sort_arr[num_in_rank - 1]]->obj_[i] == mixed_pop[sort_arr[0]]->obj_[i])
					{
						mixed_pop[sort_arr[j]]->fitness_ += 0;
					}
					else
					{
						double distance = (mixed_pop[sort_arr[j + 1]]->obj_[i] - mixed_pop[sort_arr[j - 1]]->obj_[i]) /
							(mixed_pop[sort_arr[num_in_rank - 1]]->obj_[i] - mixed_pop[sort_arr[0]]->obj_[i]);
						mixed_pop[sort_arr[j]]->fitness_ += distance;
						SetDistanceInfo(distanceinfo_vec, sort_arr[j], distance);
					}
				}
			}
		}

		std::sort(distanceinfo_vec.begin(), distanceinfo_vec.begin()+num_in_rank, [](const DistanceInfo &left, const DistanceInfo &right) {
			return left.distance < right.distance;
		});

		// copy sort result
		for (int i = 0; i < num_in_rank; i++)
		{
			pop_sort[i] = distanceinfo_vec[i].index;
		}

		return num_in_rank;
	}

	bool DPBEMO_DTS::EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop)
	{
		int current_popnum = 0, rank_index = 0;
		int mixed_popnum = g_GlobalSettings->population_num_ + 2 * (g_GlobalSettings->population_num_ / 2);

		NonDominatedSort(mixed_pop, mixed_popnum, g_GlobalSettings->obj_num_);

		// select individuals by rank
		while (1)
		{
			int temp_number = 0;
			for (int i = 0; i < mixed_popnum; i++)
			{
				if (mixed_pop[i]->rank_ == rank_index)
				{
					temp_number++;
				}
			}
			if (current_popnum + temp_number <= g_GlobalSettings->population_num_)
			{
				for (int i = 0; i < mixed_popnum; i++)
				{
					if (mixed_pop[i]->rank_ == rank_index)
					{
						CopyIndividual(mixed_pop[i], parent_pop[current_popnum]);
						current_popnum++;
					}
				}
				rank_index++;
			}
			else
				break;
		}

		// select individuals by crowding distance
		int sort_num = 0;
		std::vector<int> pop_sort(mixed_popnum);

		if (current_popnum < g_GlobalSettings->population_num_)
		{
			if(!usingPref)
			{
				// sort_num = CrowdingDistance(mixed_pop, mixed_popnum, pop_sort.data(), rank_index);
				if(!count)
					sort_num = CrowdingDistance(mixed_pop, mixed_popnum, pop_sort.data(), rank_index);
				else
				{
					// printf("already consult but not in this round\n");
					sort_num = FitnessViaPreference(mixed_pop, mixed_popnum, pop_sort.data(), rank_index, g_GlobalSettings->population_num_ - current_popnum);
					if (sort_num == -1)
					{
						return false;
					}
				}
			}
			else
			{
				sort_num = FitnessViaPreference(mixed_pop, mixed_popnum, pop_sort.data(), rank_index, g_GlobalSettings->population_num_ - current_popnum);
				if (sort_num == -1)
                {
                    return false;
                }
			}
				
			
			while (1)
			{
				if (current_popnum < g_GlobalSettings->population_num_)
				{
					// int index = pop_sort[sort_num - 1];
					CopyIndividual(mixed_pop[pop_sort[--sort_num]], parent_pop[current_popnum]);
					current_popnum++;
				}
				else {
					break;
				}
			}
		}

		// clear crowding distance value
		for (int i = 0; i < g_GlobalSettings->population_num_; i++)
		{
			parent_pop[i]->fitness_ = 0;
		}
		
		pop_sort.clear();

		return true;
	}


	// get the utility function value of all solutions
    void  DPBEMO_DTS::UtilityFunction_Gaussian(Individual **pop)
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
    void DPBEMO_DTS::StorePreference(Individual **pop)
    {
        
        std::vector<size_t>index(g_GlobalSettings->population_num_);
        
        
        iota(index.begin(),index.end(),0);
        std::sort(index.begin(),index.end(),
            [&pop](size_t index1, size_t index2){return pop[index1]->obj_[0] > pop[index2]->obj_[0];});



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
        PVals.save("compare.dat",raw_ascii);

    }

	int DPBEMO_DTS::Consult_DM()
    {
        // mat PVals;
        int best;
        PVals.load("compare.dat");

        double learning_rate = 0.51;
        int T_round = 100;
        int Budget = 40;
        double kappa = 1.0;
        // Run run(PVals, learning_rate);
        DTS dts(PVals, learning_rate);
        int result = dts.timestep(T_round, 0);
        // int best_subset = result.first;
        // int best_solution = result.second;
        // best = dts.timestep(T_round, 0);
       
        // best = run.winner();
        // printf("recommend area is %d\n", best_subset);
        // printf("recommend solution is %d\n", best_solution);
        return result;
    }

	int DPBEMO_DTS::FitnessViaPreference(Individual **mixed_pop, int pop_num, int *pop_sort, int rank_index, int nneed)
	{
		int num_in_rank = 0;
		std::vector<int> sort_arr(pop_num, 0);
		std::vector<DistanceInfo> distanceinfo_vec(pop_num, {-1, 0.0});

		// find all the individuals with rank rank_index
		for(int i = 0; i < pop_num; i++)
		{
			mixed_pop[i]->fitness_ = 0;
			if(mixed_pop[i]->rank_ == rank_index)
			{
				distanceinfo_vec[num_in_rank].index = i;
				sort_arr[num_in_rank] = i;
				num_in_rank ++;
			}
		}
		for(int i = 0; i < g_GlobalSettings->obj_num_; i++)
		{
			std::sort(sort_arr.begin(), sort_arr.end(),[=](int left, int right){
				return mixed_pop[left]->obj_[i] < mixed_pop[right]->obj_[i];
			});
			
			// assign the border fitness to 0 
			mixed_pop[sort_arr[0]]->fitness_ = 0;
			SetDistanceInfo(distanceinfo_vec, sort_arr[0], 0);
			mixed_pop[sort_arr[num_in_rank - 1]]->fitness_ = 0;
			SetDistanceInfo(distanceinfo_vec, sort_arr[num_in_rank - 1], 0);
			// sigma = (double) var(win_times / win_times.max());
			// assign all distance according to predicted utility distribution
			for(int j = 0; j < num_in_rank - 1; j++)
			{
				double distance = UpdateUtilityPrediction(mixed_pop[sort_arr[j]]->obj_[i], i);
				mixed_pop[sort_arr[j]]->fitness_ += distance;
				SetDistanceInfo(distanceinfo_vec, sort_arr[j], distance);
			}
		}
		std::sort(distanceinfo_vec.begin(), distanceinfo_vec.begin()+num_in_rank, [](const DistanceInfo &left, const DistanceInfo &right){
			return left.distance < right.distance;
		});

		// copy result to pop_sort
		for(int i = 0; i < pop_num; i++)
		{
			pop_sort[i] = distanceinfo_vec[i].index;
		}
		return num_in_rank;

	}

	double DPBEMO_DTS::UpdateUtilityPrediction(double obj, int index)
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

	float DPBEMO_DTS::CalculateKLDivergence(Individual **pop)
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
		// sigma = (double) var(win_times / win_times.max());
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


}