#include "algorithm/nsga2/nsga2.h"

#include <vector>
#include <algorithm>
#include <iostream>

#include "core/macro.h"
#include "core/global.h"
#include "core/nd_sort.h"
#include "operator/tournament_selection.h"
#include "operator/polynomial_mutation.h"
#include "operator/sbx.h"
#include "random/random.h"

namespace emoc {

	NSGA2::NSGA2(int thread_id) :Algorithm(thread_id)
	{

	}

	NSGA2::~NSGA2()
	{

	}

	void NSGA2::Solve()
	{
		Initialization();
		while (!IsTermination())
		{
			// generate offspring population
			Crossover(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->offspring_population_.data());
			PolynomialMutation(g_GlobalSettings->offspring_population_.data(), 2 * (real_popnum_ / 2), g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, mutation_para_);
			EvaluatePop(g_GlobalSettings->offspring_population_.data(), 2 * (real_popnum_ / 2));
			MergePopulation(g_GlobalSettings->parent_population_.data(), real_popnum_, g_GlobalSettings->offspring_population_.data(),
				2 * (real_popnum_ / 2), g_GlobalSettings->mixed_population_.data());
			
			// select next generation's population
			EnvironmentalSelection(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->mixed_population_.data());
		}
	}

	void NSGA2::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 20.0;
	}

	void NSGA2::Crossover(Individual **parent_pop, Individual **offspring_pop)
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
	
	void NSGA2::SetDistanceInfo(std::vector<DistanceInfo> &distanceinfo_vec, int target_index, double distance)
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


	int NSGA2::CrowdingDistance(Individual **mixed_pop,  int pop_num, int *pop_sort, int rank_index)
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

		std::sort(distanceinfo_vec.begin(), distanceinfo_vec.begin()+num_in_rank, [](DistanceInfo &left, DistanceInfo &right) {
			return left.distance < right.distance;
		});

		// copy sort result
		for (int i = 0; i < num_in_rank; i++)
		{
			pop_sort[i] = distanceinfo_vec[i].index;
		}

		return num_in_rank;
	}

	void NSGA2::EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop)
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
			sort_num = CrowdingDistance(mixed_pop, mixed_popnum, pop_sort.data(), rank_index);
			while (1)
			{
				if (current_popnum < g_GlobalSettings->population_num_)
				{
					int index = pop_sort[sort_num - 1];
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
	}

}