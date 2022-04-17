#include "algorithm/moead_m2m/moead_m2m.h"

#include <cmath>
#include <iostream>
#include <algorithm>
#include <unordered_map>

#include "core/macro.h"
#include "core/nd_sort.h"
#include "core/global.h"
#include "core/utility.h"
#include "core/uniform_point.h"
#include "operator/de.h"
#include "operator/polynomial_mutation.h"
#include "random/random.h"

namespace emoc {

	MOEADM2M::MOEADM2M(int thread_id) :
		Algorithm(thread_id),
		lambda_(nullptr),
		weight_num_(0)
	{

	}

	MOEADM2M::~MOEADM2M()
	{
		for (int i = 0; i < weight_num_; ++i)
		{
			delete[] lambda_[i];
			lambda_[i] = nullptr;
		}
		delete[] lambda_;
		lambda_ = nullptr;
	}

	void MOEADM2M::Solve()
	{
		Initialization();
		Individual* offspring = g_GlobalSettings->offspring_population_[0];

		while (!IsTermination())
		{
			// generate offspring population
			GenerateOffspring(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->offspring_population_.data());
			EvaluatePop(g_GlobalSettings->offspring_population_.data(), real_popnum_);
			MergePopulation(g_GlobalSettings->parent_population_.data(), real_popnum_, g_GlobalSettings->offspring_population_.data(),
				real_popnum_, g_GlobalSettings->mixed_population_.data());

			// select next generation's population
			AssociatePopulation(g_GlobalSettings->mixed_population_.data(), real_popnum_ * 2, g_GlobalSettings->parent_population_.data());
		}
	}

	void MOEADM2M::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

		// select K value, we use 10 as default value for all dimension except 3
		if (g_GlobalSettings->obj_num_ == 3) K = 17;
		else K = 10;						

		// generate weight vectors
		lambda_ = UniformPoint(K, &weight_num_, g_GlobalSettings->obj_num_);
		K = weight_num_;
		S = g_GlobalSettings->population_num_ / K;
		real_popnum_ = S * K;
		
		// associate intialized population to different subproblem
		std::vector<Individual*> pop_copy;
		for (int i = 0; i < real_popnum_; ++i)
		{
			pop_copy.push_back(new Individual(g_GlobalSettings->dec_num_, g_GlobalSettings->obj_num_));
			CopyIndividual(g_GlobalSettings->parent_population_[i], pop_copy[i]);
		}
		AssociatePopulation(pop_copy.data(), real_popnum_, g_GlobalSettings->parent_population_.data());

		for (int i = 0; i < real_popnum_; ++i)
			delete pop_copy[i];
	}

	void MOEADM2M::GenerateOffspring(Individual** parent_pop, Individual** offspring_pop)
	{
		Individual* parent1 = nullptr, * parent2 = nullptr, *offspring = nullptr;
		for (int i = 0; i < K; i++)
		{
			for (int j = 0; j < S; j++)
			{
				offspring = offspring_pop[i * S + j];
				parent1 = parent_pop[i * S + j];
				if (randomperc() < 0.7)
					parent2 = parent_pop[rnd(i * S, i * S + S - 1)];
				else
					parent2 = parent_pop[rnd(0, K * S - 1)];

				double rc = 0, rm = 0;
				double rand = 0;
				double yl = 0; double yu = 0; double value = 0;
				double temp = -pow(1 - (double)g_GlobalSettings->current_evaluation_ / g_GlobalSettings->max_evaluation_, 0.7);
				rc = 2 * (randomperc() - 0.5) * (1 - pow(randomperc(), temp));
				rm = 0.25 * (2 * randomperc() - 1) * (1 - pow(randomperc(), temp));
				for (int k = 0; k < g_GlobalSettings->dec_num_; k++)
				{
					// crossover
					yl = g_GlobalSettings->dec_lower_bound_[k];
					yu = g_GlobalSettings->dec_upper_bound_[k];
					value = parent1->dec_[k] + rc * (parent1->dec_[k] - parent2->dec_[k]); 

					// mutation
					if (randomperc() < g_GlobalSettings->pm_parameter_.muatation_pro)
					{
						value = value + rm * (yu - yl);
					}

					// repair
					if (value > yu) 
					{
						rand = randomperc();
						value = yu - 0.5 * rand * (yu - parent1->dec_[k]);
					}
					if (value < yl) 
					{
						rand = randomperc();
						value = yl + 0.5 * rand * (parent1->dec_[k] - yl);
					}

					offspring->dec_[k] = value;
				}
			}
		}
	}

	void MOEADM2M::AssociatePopulation(Individual** pop_src, int pop_num, Individual** pop_dest)
	{
		std::unordered_map<int, std::vector<int>> partition;	// (subproblem index - corresponding individual indices)

		for (int i = 0; i < pop_num; i++)
		{
			int max_index = 0;
			double max = CalculateCos(pop_src[i]->obj_.data(), lambda_[0], g_GlobalSettings->obj_num_);
			for (int j = 1; j < K; j++)
			{
				double current_cos = CalculateCos(pop_src[i]->obj_.data(), lambda_[j], g_GlobalSettings->obj_num_);
				if (max < current_cos)
				{
					max = current_cos;
					max_index = j;
				}
			}

			partition[max_index].push_back(i);
		}

		for (int i = 0; i < K; i++)
		{
			if (partition[i].size() < S)
			{
				// randomly select individuals and join to the current subproblem
				int remain_number = S - partition[i].size();
				for (int j = 0; j < remain_number; j++)
					partition[i].push_back(rnd(0, real_popnum_ - 1));
			}
			else if (partition[i].size() > S)
			{
				// delete individuals from the current subproblem by nondominated sorting and crowding distance
				std::vector<Individual*> temp_pop;
				for (int j = 0; j < partition[i].size(); j++)
					temp_pop.push_back(pop_src[partition[i][j]]);

				// nodominate sort and find the max rank index which make population number beyond S
				NonDominatedSort(temp_pop.data(), partition[i].size(), g_GlobalSettings->obj_num_);
				std::unordered_map<int, int> rank_count; // (rank - the number of ind with that rank)
				for (int j = 0; j < partition[i].size(); j++)
					rank_count[temp_pop[j]->rank_]++;

				int temp_sum = 0;
				int current_rank = -1;
				while (temp_sum < S)
				{
					current_rank++;
					temp_sum += rank_count[current_rank];
				}

				// delete the individuals with larger rank than current_rank
				for (int j = 0; j < partition[i].size(); j++)
					if (temp_pop[j]->rank_ > current_rank) partition[i][j] = -1;

				if (temp_sum > S)
				{
					// delete the individuals with bigger crowding distance in the current_rank
					std::vector<int>pop_sort(partition[i].size(), 0);
					int number = CrowdingDistance(temp_pop.data(), partition[i].size(), pop_sort.data(), current_rank);
					int beyond_number = temp_sum - S;
					for (int j = 0; j < beyond_number; j++)
						partition[i][pop_sort[j]] = -1;
				}
			}
		}

		// copy ordered individual to pop_dest
		int count = 0;
		for (int i = 0; i < K; i++)
		{
			for (int j = 0; j < partition[i].size(); j++)
			{
				if (partition[i][j] != -1)
				{
					CopyIndividual(pop_src[partition[i][j]], pop_dest[count]);
					count++;
				}
			}
		}
	}

	void MOEADM2M::SetDistanceInfo(std::vector<DistanceInfo>& distanceinfo_vec, int target_index, double distance)
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

	int MOEADM2M::CrowdingDistance(Individual** pop, int pop_num, int* pop_sort, int rank_index)
	{
		int num_in_rank = 0;
		std::vector<int> sort_arr(pop_num, 0);
		std::vector<DistanceInfo> distanceinfo_vec(pop_num, { -1,0.0 });

		// find all the indviduals with rank rank_index
		for (int i = 0; i < pop_num; i++)
		{
			pop[i]->fitness_ = 0;
			if (pop[i]->rank_ == rank_index)
			{
				distanceinfo_vec[num_in_rank].index = i;
				sort_arr[num_in_rank] = i;
				num_in_rank++;
			}
		}

		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
		{
			// sort the population with i-th obj
			std::sort(sort_arr.begin(), sort_arr.begin() + num_in_rank, [=](int left, int right) {
				return pop[left]->obj_[i] < pop[right]->obj_[i];
				});

			// set the first and last individual with INF fitness (crowding distance)
			pop[sort_arr[0]]->fitness_ = EMOC_INF;
			SetDistanceInfo(distanceinfo_vec, sort_arr[0], EMOC_INF);
			pop[sort_arr[num_in_rank - 1]]->fitness_ = EMOC_INF;
			SetDistanceInfo(distanceinfo_vec, sort_arr[num_in_rank - 1], EMOC_INF);

			// calculate each solution's crowding distance
			for (int j = 1; j < num_in_rank - 1; j++)
			{
				if (pop[sort_arr[num_in_rank - 1]]->obj_[i] == pop[sort_arr[0]]->obj_[i])
				{
					pop[sort_arr[j]]->fitness_ += 0;
				}
				else
				{
					double distance = (pop[sort_arr[j + 1]]->obj_[i] - pop[sort_arr[j - 1]]->obj_[i]) /
						(pop[sort_arr[num_in_rank - 1]]->obj_[i] - pop[sort_arr[0]]->obj_[i]);
					pop[sort_arr[j]]->fitness_ += distance;
					SetDistanceInfo(distanceinfo_vec, sort_arr[j], distance);
				}
			}
		}

		std::sort(distanceinfo_vec.begin(), distanceinfo_vec.begin() + num_in_rank, [](DistanceInfo& left, DistanceInfo& right) {
			return left.distance < right.distance;
			});

		// copy sort result
		for (int i = 0; i < num_in_rank; i++)
		{
			pop_sort[i] = distanceinfo_vec[i].index;
		}

		return num_in_rank;
	}

}