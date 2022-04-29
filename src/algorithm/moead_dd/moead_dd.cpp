#include "algorithm/moead_dd/moead_dd.h"

#include <cmath>
#include <iostream>
#include <algorithm>
#include <unordered_set>

#include "core/macro.h"
#include "core/nd_sort.h"
#include "core/global.h"
#include "core/utility.h"
#include "core/uniform_point.h"
#include "operator/sbx.h"
#include "operator/polynomial_mutation.h"
#include "random/random.h"

namespace emoc {

	MOEADD::MOEADD(int thread_id) :
		Algorithm(thread_id),
		lambda_(nullptr),
		weight_num_(0),
		neighbour_(nullptr),
		neighbour_selectpro_(0.9),
		ideal_point_(new double[g_GlobalSettings->obj_num_])
	{

	}

	MOEADD::~MOEADD()
	{
		for (int i = 0; i < weight_num_; ++i)
		{
			delete[] lambda_[i];
			delete[] neighbour_[i];
			lambda_[i] = nullptr;
			neighbour_[i] = nullptr;
		}
		delete[] lambda_;
		delete[] neighbour_;
		delete[] ideal_point_;
		lambda_ = nullptr;
		neighbour_ = nullptr;
		ideal_point_ = nullptr;
	}

	void MOEADD::Solve()
	{
		Initialization();
		Individual* offspring = g_GlobalSettings->offspring_population_[0];

		while (!IsTermination())
		{
			for (int i = 0; i < weight_num_; ++i)
			{
				// generate offspring for current subproblem
				Crossover(g_GlobalSettings->parent_population_.data(), i, offspring);
				PolynomialMutation(offspring, g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, mutation_para_);
				EvaluateInd(offspring);

				// associate new offspring
				Association(g_GlobalSettings->offspring_population_.data(), 1);

				// update ideal point
				UpdateIdealpoint(offspring, ideal_point_, g_GlobalSettings->obj_num_);

				MergePopulation(g_GlobalSettings->parent_population_.data(), weight_num_, 
					g_GlobalSettings->offspring_population_.data(), 1, g_GlobalSettings->mixed_population_.data());

				EnvironmentSelection(g_GlobalSettings->mixed_population_.data(), weight_num_ + 1, g_GlobalSettings->parent_population_.data());
			}
		}
	}

	void MOEADD::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

		// generate weight vectors
		lambda_ = UniformPoint(g_GlobalSettings->population_num_, &weight_num_, g_GlobalSettings->obj_num_);
		real_popnum_ = weight_num_;

		// associate each member in current population with a unique subregion;
		cluster_.resize(weight_num_);
		Association(g_GlobalSettings->parent_population_.data(), weight_num_);

		// set the neighbours of each individual
		SetNeighbours();

		// nd sort
		NonDominatedSort(g_GlobalSettings->parent_population_.data(), real_popnum_, g_GlobalSettings->obj_num_);

		// initialize ideal point
		UpdateIdealpoint(g_GlobalSettings->parent_population_.data(), weight_num_, ideal_point_, g_GlobalSettings->obj_num_);

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 20.0;
	}

	void MOEADD::Association(Individual** pop, int pop_num)
	{
		for (int i = 0; i < pop_num; i++)
		{
			int max_index = 0;
			double max = CalculateCos(pop[i]->obj_.data(), lambda_[0], g_GlobalSettings->obj_num_);
			for (int j = 1; j < weight_num_; j++)
			{
				double current_cos = CalculateCos(pop[i]->obj_.data(), lambda_[j], g_GlobalSettings->obj_num_);
				if (max < current_cos)
				{
					max = current_cos;
					max_index = j;
				}
			}

			// for differentiate the index is from parent population or offspring population
			if (pop_num == 1)
			{
				cluster_[max_index].push_back(i + real_popnum_);
				subregion_map_[i + real_popnum_] = max_index;
			}
			else
			{
				cluster_[max_index].push_back(i);
				subregion_map_[i] = max_index;
			}
		}
	}

	void MOEADD::SetNeighbours()
	{
		// set neighbour size and allocate memory
		neighbour_num_ = (weight_num_ / 10) + 1;
		neighbour_ = new int* [weight_num_];
		for (int i = 0; i < weight_num_; ++i)
		{
			neighbour_[i] = new int[neighbour_num_];
		}

		std::vector<DistanceInfo> sort_list(weight_num_);
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

			std::sort(sort_list.begin(), sort_list.end(), [](DistanceInfo& left, DistanceInfo& right) {
				return left.distance < right.distance;
				});

			for (int j = 0; j < neighbour_num_; j++)
			{
				neighbour_[i][j] = sort_list[j + 1].index;
			}
		}
	}

	void MOEADD::Crossover(Individual** parent_pop, int current_index, Individual* offspring)
	{
		std::vector<int> candidate_set;
		int parent2_index = -1, parent3_index = -1;
		if (randomperc() < neighbour_selectpro_)
		{
			for (int i = 0; i < neighbour_num_; i++)
			{
				int index = neighbour_[current_index][i];
				for (int j = 0; j < cluster_[index].size(); j++)
					candidate_set.push_back(cluster_[index][j]);

				if (candidate_set.size() < 2)
				{
					parent2_index = rnd(0, weight_num_ - 1);
					parent3_index = rnd(0, weight_num_ - 1);
					while (parent3_index == parent2_index)
						parent3_index = rnd(0, weight_num_ - 1);
				}
				else
				{
					parent2_index = rnd(0, candidate_set.size() - 1);
					parent3_index = rnd(0, candidate_set.size() - 1);
					while (parent3_index == parent2_index)
						parent3_index = rnd(0, candidate_set.size() - 1);
					parent2_index = candidate_set[parent2_index];
					parent3_index = candidate_set[parent3_index];
				}
			}
		}
		else
		{
			parent2_index = rnd(0, weight_num_ - 1);
			parent3_index = rnd(0, weight_num_ - 1);
			while (parent3_index == parent2_index)
				parent3_index = rnd(0, weight_num_ - 1);
		}

		Individual* parent2 = parent_pop[parent2_index];
		Individual* parent3 = parent_pop[parent3_index];
		if (randomperc() < 0.5)
			SBX(parent2, parent3, offspring, g_GlobalSettings->offspring_population_[1],
				g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, cross_para_);
		else
			SBX(parent2, parent3, g_GlobalSettings->offspring_population_[1], offspring,
				g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, cross_para_);
	}

	void MOEADD::EnvironmentSelection(Individual** mixed_pop, int mixedpop_num, Individual** parent_pop)
	{
		std::vector<std::vector<int>> front_index;
		NonDominatedSort(mixed_pop, mixedpop_num, g_GlobalSettings->obj_num_);

		// initialize front index
		int max_rank = 0;
		for (int i = 0; i < mixedpop_num; i++)
			if (mixed_pop[i]->rank_ > max_rank) max_rank = mixed_pop[i]->rank_;
		front_index.resize(max_rank + 1);
		for (int i = 0; i < mixedpop_num; i++)
			front_index[mixed_pop[i]->rank_].push_back(i);

		// find remove index
		int remove_index = -1;
		if (max_rank == 0)
		{
			remove_index = LocateWorst(mixed_pop, mixedpop_num, front_index);
		}
		else
		{
			if (front_index[max_rank].size() == 1)
			{
				int individual_index = front_index[max_rank][0];
				int subregion_index = subregion_map_[individual_index];
				if (cluster_[subregion_index].size() > 1)
				{
					remove_index = individual_index;
				}
				else
				{
					remove_index = LocateWorst(mixed_pop, mixedpop_num, front_index);
				}
			}
			else
			{
				// find candidate subregion
				std::unordered_set<int> candidate_subregion;
				for (int i = 0; i < front_index[max_rank].size(); i++)
				{
					int individual_index = front_index[max_rank][i];
					candidate_subregion.insert(subregion_map_[individual_index]);
				}
				
				// find the most crowded subregion
				std::vector<int> most_crowded_subregion;
				int max_crowd = 0;
				for (auto subregion_index:candidate_subregion)
				{
					if (max_crowd < cluster_[subregion_index].size())
						max_crowd = cluster_[subregion_index].size();
				}
				for (auto subregion_index : candidate_subregion)
				{
					if (max_crowd == cluster_[subregion_index].size())
						most_crowded_subregion.push_back(subregion_index);
				}

				// break the tie
				int real_crowded_region = most_crowded_subregion[0];
				if (most_crowded_subregion.size() > 1)
				{
					int maxpbi_index = -1;
					double max_pbisum = -1.0;
					for (int i = 0; i < most_crowded_subregion.size(); i++)
					{
						int subregion_index = most_crowded_subregion[i];
						double current_pbisum = 0.0;
						for (int j = 0; j < cluster_[subregion_index].size(); j++)
						{
							int index = cluster_[subregion_index][j];
							current_pbisum += CalPBI(mixed_pop[index], lambda_[subregion_index], ideal_point_, g_GlobalSettings->obj_num_);
						}
						if (current_pbisum > max_pbisum)
						{
							max_pbisum = current_pbisum;
							maxpbi_index = subregion_index;
						}
					}
					real_crowded_region = maxpbi_index;
				}

				if (cluster_[real_crowded_region].size() > 1)
				{
					// find the worst solution
					double max_pbi = 0.0;
					for (int i = 0; i < cluster_[real_crowded_region].size(); i++)
					{
						int individual_index = cluster_[real_crowded_region][i];
						double current_pbi = CalPBI(mixed_pop[individual_index], lambda_[real_crowded_region], ideal_point_, g_GlobalSettings->obj_num_);
						if (max_pbi < current_pbi)
						{
							max_pbi = current_pbi;
							remove_index = individual_index;
						}
					}
				}
				else
				{
					remove_index = LocateWorst(mixed_pop, mixedpop_num, front_index);
				}
			}
		}

		
		int count = 0;
		for (int i = 0; i < mixedpop_num; i++)
		{
			if (i != remove_index)
				CopyIndividual(mixed_pop[i], parent_pop[count++]);
		}

		// update cluster and subregion_map
		for (int i = 0; i < cluster_.size(); i++)
		{
			auto pos = std::find(cluster_[i].begin(), cluster_[i].end(), remove_index);
			if (pos != cluster_[i].end())
			{
				cluster_[i].erase(pos);
				break;
			}
		}
		for (int i = 0; i < cluster_.size(); i++)
		{
			for (int j = 0; j < cluster_[i].size(); j++)
			{
				if (cluster_[i][j] > remove_index)
				{
					cluster_[i][j]--;
				}
			}
		}

		subregion_map_.erase(remove_index);
		for (int i = remove_index+1; i <= real_popnum_; i++)
		{
			int element = subregion_map_[i];
			subregion_map_.erase(i);
			subregion_map_[i - 1] = element;
		}
	}

	int MOEADD::LocateWorst(Individual** mixed_pop, int mixedpop_num, const std::vector<std::vector<int>>& front_index)
	{
		// find the most crowded subregion
		std::vector<int> most_crowded_subregion;
		int max_crowd = 0;
		for (int i = 0;i < weight_num_;i++)
		{
			if (max_crowd < cluster_[i].size())
				max_crowd = cluster_[i].size();
		}
		for (int i = 0; i < weight_num_; i++)
		{
			if (max_crowd == cluster_[i].size())
				most_crowded_subregion.push_back(i);
		}

		// break the tie
		int real_crowded_region = most_crowded_subregion[0];
		if (most_crowded_subregion.size() > 1)
		{
			int maxpbi_index = -1;
			double max_pbisum = -1.0;
			for (int i = 0; i < most_crowded_subregion.size(); i++)
			{
				int subregion_index = most_crowded_subregion[i];
				double current_pbisum = 0.0;
				for (int j = 0; j < cluster_[subregion_index].size(); j++)
				{
					int index = cluster_[subregion_index][j];
					current_pbisum += CalPBI(mixed_pop[index], lambda_[subregion_index], ideal_point_, g_GlobalSettings->obj_num_);
				}
				if (current_pbisum > max_pbisum)
				{
					max_pbisum = current_pbisum;
					maxpbi_index = subregion_index;
				}
			}
			real_crowded_region = maxpbi_index;
		}

		// find candidate ind to remove
		std::vector<int> candidate_ind;
		int max_rank = -1;
		for (int i = 0; i < cluster_[real_crowded_region].size(); i++)
		{
			int individual_index = cluster_[real_crowded_region][i];
			if (max_rank < mixed_pop[individual_index]->rank_)
				max_rank = mixed_pop[individual_index]->rank_;
		}
		for (int i = 0; i < cluster_[real_crowded_region].size(); i++)
		{
			int individual_index = cluster_[real_crowded_region][i];
			if (max_rank == mixed_pop[individual_index]->rank_)
				candidate_ind.push_back(individual_index);
		}

		// find the worst solution
		int remove_index = -1;
		double max_pbi = 0.0;
		for (int i = 0; i < candidate_ind.size(); i++)
		{
			int individual_index = candidate_ind[i];
			double current_pbi = CalPBI(mixed_pop[individual_index], lambda_[real_crowded_region], ideal_point_, g_GlobalSettings->obj_num_);
			if (max_pbi < current_pbi)
			{
				max_pbi = current_pbi;
				remove_index = individual_index;
			}
		}

		return remove_index;
	}

}