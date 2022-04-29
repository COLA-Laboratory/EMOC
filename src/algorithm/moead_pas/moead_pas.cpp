#include "algorithm/moead_pas/moead_pas.h"

#include <cmath>
#include <iostream>
#include <algorithm>

#include "core/macro.h"
#include "core/global.h"
#include "core/utility.h"
#include "core/nd_sort.h"
#include "core/uniform_point.h"
#include "operator/de.h"
#include "operator/polynomial_mutation.h"
#include "random/random.h"

namespace emoc {

	MOEADPAS::MOEADPAS(int thread_id) :
		Algorithm(thread_id),
		lambda_(nullptr),
		weight_num_(0),
		neighbour_(nullptr),
		neighbour_selectpro_(0.9),
		ideal_point_(new double[g_GlobalSettings->obj_num_]),
		nadir_point_(new double[g_GlobalSettings->obj_num_])
	{

	}

	MOEADPAS::~MOEADPAS()
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
		delete[] nadir_point_;
		delete[] Pi;
		lambda_ = nullptr;
		neighbour_ = nullptr;
		ideal_point_ = nullptr;
		nadir_point_ = nullptr;
		Pi = nullptr;
	}

	void MOEADPAS::Solve()
	{
		Initialization();
		Individual* offspring = g_GlobalSettings->offspring_population_[0];

		while (!IsTermination())
		{
			for (int i = 0; i < weight_num_; ++i)
			{
				// set current iteration's neighbour type
				if (randomperc() < neighbour_selectpro_)
					neighbour_type_ = NEIGHBOUR;
				else
					neighbour_type_ = GLOBAL;

				// generate offspring for current subproblem
				Crossover(g_GlobalSettings->parent_population_.data(), i, offspring);
				PolynomialMutation(offspring, g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, mutation_para_);
				EvaluateInd(offspring);

				// update ideal point
				UpdateIdealpoint(offspring, ideal_point_, g_GlobalSettings->obj_num_);

				// update neighbours' subproblem 
				UpdateSubproblem(offspring, i);
			}

			// update nadir point
			NonDominatedSort(g_GlobalSettings->parent_population_.data(), real_popnum_, g_GlobalSettings->obj_num_);
			UpdateNadirpointWithRank(g_GlobalSettings->parent_population_.data(), real_popnum_, nadir_point_, g_GlobalSettings->obj_num_, 0);

			// Update p parameter for each subproblem
			UpdatePiArray(g_GlobalSettings->parent_population_.data(), Pi);
		}
	}

	void MOEADPAS::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

		// generate weight vectors
		lambda_ = UniformPoint(g_GlobalSettings->population_num_, &weight_num_, g_GlobalSettings->obj_num_);
		real_popnum_ = weight_num_;

		// initialize p parameter for each subproblem
		Pi = new int[real_popnum_];
		for (int i = 0; i < real_popnum_; i++)
			Pi[i] = candidate_p_[candidate_p_.size() - 1];

		// set the neighbours of each individual
		SetNeighbours();

		// initialize ideal point and nadir point
		UpdateIdealpoint(g_GlobalSettings->parent_population_.data(), weight_num_, ideal_point_, g_GlobalSettings->obj_num_);
		NonDominatedSort(g_GlobalSettings->parent_population_.data(), real_popnum_, g_GlobalSettings->obj_num_);
		UpdateNadirpointWithRank(g_GlobalSettings->parent_population_.data(), real_popnum_, nadir_point_, g_GlobalSettings->obj_num_, 0);

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 0.5;
	}

	void MOEADPAS::SetNeighbours()
	{
		// set neighbour size, replace number and allocate memory
		neighbour_num_ = 0.1 * g_GlobalSettings->population_num_;
		replace_num_ = (0.1 * neighbour_num_) >= 1 ? (0.1 * neighbour_num_) : 1;
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

	void MOEADPAS::Crossover(Individual** parent_pop, int current_index, Individual* offspring)
	{
		int size = neighbour_type_ == NEIGHBOUR ? neighbour_num_ : weight_num_;
		int parent2_index = 0, parent3_index = 0;

		// randomly select two parents according to neighbour type
		if (neighbour_type_ == NEIGHBOUR)
		{
			parent2_index = neighbour_[current_index][rnd(0, size - 1)];
			parent3_index = neighbour_[current_index][rnd(0, size - 1)];
		}
		else
		{
			parent2_index = rnd(0, size - 1);
			parent3_index = rnd(0, size - 1);
		}

		Individual* parent1 = parent_pop[current_index];
		Individual* parent2 = parent_pop[parent2_index];
		Individual* parent3 = parent_pop[parent3_index];
		DE(parent1, parent2, parent3, offspring, g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, cross_para_);
	}

	void MOEADPAS::UpdateSubproblem(Individual* offspring, int current_index)
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
				weight_index = neighbour_[current_index][perm_index[i]];
			else
				weight_index = perm_index[i];

			Individual* current_ind = g_GlobalSettings->parent_population_[weight_index];
			offspring_fitness = CalWeightedLpScalarizing(offspring, lambda_[weight_index], ideal_point_,nadir_point_, g_GlobalSettings->obj_num_, Pi[weight_index]);
			neighbour_fitness = CalWeightedLpScalarizing(current_ind, lambda_[weight_index], ideal_point_, nadir_point_, g_GlobalSettings->obj_num_, Pi[weight_index]);
			if (offspring_fitness < neighbour_fitness)
			{
				CopyIndividual(offspring, g_GlobalSettings->parent_population_[weight_index]);
				count++;
			}
		}
	}

	void MOEADPAS::UpdatePiArray(Individual** parent_pop, int* Pi)
	{
		// the index of individual which has minimum weighted Lp scalarizing value for each p with a specified weight vector
		std::vector<int> min_weightedLp_ind(candidate_p_.size(), -1);
		
		// the perpendicular distance for the best ind to the weight vector
		std::vector<double> perpendicular_distance(candidate_p_.size(), -1.0);

		for (int i = 0; i < real_popnum_; i++)
		{
			if (randomperc() >= (double)g_GlobalSettings->current_evaluation_ / (double)g_GlobalSettings->max_evaluation_)
			{
				// Update p paramater for ith subproblem
				for (int j = 0; j < candidate_p_.size(); j++)
				{
					int current_p = candidate_p_[j];
					double min = CalWeightedLpScalarizing(g_GlobalSettings->parent_population_[0], lambda_[i], ideal_point_, nadir_point_,g_GlobalSettings->obj_num_, current_p);
					min_weightedLp_ind[j] = 0;
					for (int k = 1; k < real_popnum_; k++)
					{
						double current_value = CalWeightedLpScalarizing(g_GlobalSettings->parent_population_[k], lambda_[i], ideal_point_, nadir_point_, g_GlobalSettings->obj_num_, current_p);
						if (min > current_value)
						{
							min = current_value;
							min_weightedLp_ind[j] = k;
						}
					}

					int best_index = min_weightedLp_ind[j];
					perpendicular_distance[j] = CalPerpendicularDistanceNormalization(g_GlobalSettings->parent_population_[best_index]->obj_.data(), lambda_[i], g_GlobalSettings->obj_num_, ideal_point_, nadir_point_);
				}

				int closest_index = 0;
				double closest_distance = perpendicular_distance[0];
				for (int j = 1; j < perpendicular_distance.size(); j++)
				{
					if (closest_distance > perpendicular_distance[j])
					{
						closest_index = j;
						closest_distance = perpendicular_distance[j];
					}
				}

				Pi[i] = candidate_p_[closest_index];
			}
		}
	}

}