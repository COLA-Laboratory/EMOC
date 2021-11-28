#include "algorithms/moead/moead.h"

#include <cmath>
#include <ctime>
#include <iostream>
#include <algorithm>

#include "core/global.h"
#include "core/utility.h"
#include "core/uniform_point.h"
#include "core/emoc_manager.h"
#include "operator/sbx.h"
#include "operator/mutation.h"
#include "random/random.h"

namespace emoc {

	MOEAD::MOEAD(Problem *problem, int thread_num):
		Algorithm(problem,thread_num),
		lambda_(nullptr),
		weight_num_(0),
		neighbour_(nullptr),
		ideal_point_(new double[g_GlobalSettings->obj_num_]), 
		aggregation_type_(0),
		pbi_theta_(5.0)
	{

	}

	MOEAD::~MOEAD()
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

	void MOEAD::Run()
	{
		Initialization();
		Individual *offspring = g_GlobalSettings->offspring_population_[0];

		while (!IsTermination())
		{	
			for (int i = 0; i < weight_num_; ++i)
			{
                // generate offspring for current subproblem
				Crossover(g_GlobalSettings->parent_population_.data(), i, offspring);
				MutationInd(offspring, g_GlobalSettings);
				EvaluateInd(offspring);

				// update ideal point
				UpdateIdealpoint(offspring, ideal_point_, g_GlobalSettings->obj_num_);

				// update neighbours' subproblem 
				UpdateSubproblem(offspring, i, aggregation_type_);
			}
		}
	}

	void MOEAD::Initialization()
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

	void MOEAD::SetNeighbours()
	{	
		// set neighbour size and allocate memory
		neighbour_num_ = weight_num_ / 10;
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

			std::sort(sort_list, sort_list+weight_num_, [](DistanceInfo &left, DistanceInfo &right) {
				return left.distance < right.distance;
			});	

			for (int j = 0; j < neighbour_num_; j++)
			{
				neighbour_[i][j] = sort_list[j+1].index;
			}
		}

		delete[] sort_list;
	}

	void MOEAD::Crossover(Individual **parent_pop, int current_index, Individual *offspring)
	{
		// randomly select two parent from current individual's neighbours
		int k = rnd(0, neighbour_num_ - 1);
		int l = rnd(0, neighbour_num_ - 1);
		Individual *parent1 = parent_pop[neighbour_[current_index][k]];
		Individual *parent2 = parent_pop[neighbour_[current_index][l]];

		SBX(parent1, parent2, g_GlobalSettings->offspring_population_[1], offspring, g_GlobalSettings);

	}

	void MOEAD::UpdateSubproblem(Individual *offspring, int current_index, int aggregation_type)
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