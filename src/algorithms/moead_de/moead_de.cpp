#include "algorithms/moead_de/moead_de.h"

#include <cmath>
#include <iostream>
#include <algorithm>

#include "core/global.h"
#include "core/utility.h"
#include "core/uniform_point.h"
#include "operator/de.h"
#include "operator/mutation.h"
#include "random/random.h"

namespace emoc {

	MOEADDE::MOEADDE(Problem *problem, int thread_num) :
		Algorithm(problem,thread_num),
		lambda_(nullptr),
		weight_num_(0),
		neighbour_(nullptr),
		replace_num_(2),
		neighbour_selectpro_(0.9),
		ideal_point_(new double[g_GlobalSettings->obj_num_])
	{

	}

	MOEADDE::~MOEADDE()
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

	void MOEADDE::Run()
	{
		Initialization();
		Individual *offspring = g_GlobalSettings->offspring_population_[0];
		TrackPopulation(g_GlobalSettings->iteration_num_);

		while (!g_GlobalSettings->IsTermination())
		{
			// begin each iteration
			g_GlobalSettings->iteration_num_++;

			for (int i = 0; i < weight_num_; ++i)
			{
				// set current iteration's neighbour type
				if (randomperc() < neighbour_selectpro_)
					neighbour_type_ = NEIGHBOUR;
				else
					neighbour_type_ = GLOBAL;

				// generate offspring for current subproblem
				Crossover(g_GlobalSettings->parent_population_.data(), i, offspring);
				MutationInd(offspring,g_GlobalSettings);
				EvaluateInd(offspring);

				// update ideal point
				UpdateIdealpoint(offspring, ideal_point_, g_GlobalSettings->obj_num_);

				// update neighbours' subproblem 
				UpdateSubproblem(offspring, i);
			}

			// record the population every interval generations and the first and last genration 
			if (g_GlobalSettings->iteration_num_ % g_GlobalSettings->output_interval_ == 0 || g_GlobalSettings->iteration_num_ == 1
				|| g_GlobalSettings->IsTermination())
			{
				TrackPopulation(g_GlobalSettings->iteration_num_);
			}
		}
	}

	void MOEADDE::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);


		// generate weight vectors
		lambda_ = UniformPoint(g_GlobalSettings->population_num_, &weight_num_, g_GlobalSettings->obj_num_);
		real_popnum_ = weight_num_;

		// set the neighbours of each individual
		SetNeighbours();

		// initialize ideal point
		UpdateIdealpoint(g_GlobalSettings->parent_population_.data(), weight_num_, ideal_point_, g_GlobalSettings->obj_num_);
	}

	void MOEADDE::SetNeighbours()
	{
		// set neighbour size and allocate memory
		neighbour_num_ = 20;
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

			std::sort(sort_list, sort_list + weight_num_, [](DistanceInfo &left, DistanceInfo &right) {
				return left.distance < right.distance;
			});

			for (int j = 0; j < neighbour_num_; j++)
			{
				neighbour_[i][j] = sort_list[j + 1].index;
			}
		}

		delete[] sort_list;
	}

	void MOEADDE::Crossover(Individual **parent_pop, int current_index, Individual *offspring)
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

		Individual *parent1 = parent_pop[current_index];
		Individual *parent2 = parent_pop[parent2_index];
		Individual *parent3 = parent_pop[parent3_index];
		DE(parent1, parent2, parent3, offspring,g_GlobalSettings);
	}

	void MOEADDE::UpdateSubproblem(Individual *offspring, int current_index)
	{
		int size = neighbour_type_ == NEIGHBOUR ? neighbour_num_ : weight_num_;
		int *perm_index = new int[size];
		random_permutation(perm_index, size);

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

			Individual *current_ind = g_GlobalSettings->parent_population_[weight_index];
			offspring_fitness = CalInverseChebycheff(offspring, lambda_[weight_index], ideal_point_, g_GlobalSettings->obj_num_);
			neighbour_fitness = CalInverseChebycheff(current_ind, lambda_[weight_index], ideal_point_, g_GlobalSettings->obj_num_);
			if (offspring_fitness < neighbour_fitness)
			{
				CopyIndividual(offspring, g_GlobalSettings->parent_population_[weight_index]);
				count++;
			}
		}

		delete[] perm_index;
	}
}