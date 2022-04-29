#include "algorithm/cmoead/cmoead.h"

#include <cmath>
#include <ctime>
#include <iostream>
#include <algorithm>

#include "core/global.h"
#include "core/utility.h"
#include "core/uniform_point.h"
#include "core/emoc_manager.h"
#include "operator/sbx.h"
#include "operator/polynomial_mutation.h"
#include "random/random.h"

namespace emoc {

	CMOEAD::CMOEAD(int thread_id) :
		Algorithm(thread_id),
		lambda_(nullptr),
		weight_num_(0),
		neighbour_(nullptr),
		ideal_point_(new double[g_GlobalSettings->obj_num_]), 
		neighbour_selectpro_(0.9),
		pbi_theta_(5.0)
	{

	}

	CMOEAD::~CMOEAD()
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

	void CMOEAD::Solve()
	{
		Initialization();
		Individual *offspring = g_GlobalSettings->offspring_population_[0];

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
		}
	}

	void CMOEAD::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

		// generate weight vectors
		lambda_ = UniformPoint(g_GlobalSettings->population_num_, &weight_num_, g_GlobalSettings->obj_num_);
		real_popnum_ = weight_num_;
		nr_ = std::ceil(real_popnum_ / 100.0);

		// set the neighbours of each individual
		SetNeighbours();

		// initialize ideal point
		UpdateIdealpoint(g_GlobalSettings->parent_population_.data(), weight_num_, ideal_point_, g_GlobalSettings->obj_num_);

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 20.0;
	}

	void CMOEAD::SetNeighbours()
	{	
		// set neighbour size and allocate memory
		neighbour_num_ = weight_num_ / 10;
		neighbour_ = new int*[weight_num_];
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

			std::sort(sort_list.begin(), sort_list.end(), [](DistanceInfo &left, DistanceInfo &right) {
				return left.distance < right.distance;
			});	

			for (int j = 0; j < neighbour_num_; j++)
			{
				neighbour_[i][j] = sort_list[j+1].index;
			}
		}
	}

	void CMOEAD::Crossover(Individual **parent_pop, int current_index, Individual *offspring)
	{
		int size = neighbour_type_ == NEIGHBOUR ? neighbour_num_ : weight_num_;
		int parent1_index = 0, parent2_index = 0;

		// randomly select two parents according to neighbour type
		if (neighbour_type_ == NEIGHBOUR)
		{
			parent1_index = neighbour_[current_index][rnd(0, size - 1)];
			parent2_index = neighbour_[current_index][rnd(0, size - 1)];
		}
		else
		{
			parent1_index = rnd(0, size - 1);
			parent2_index = rnd(0, size - 1);
		}

		Individual* parent1 = parent_pop[parent1_index];
		Individual* parent2 = parent_pop[parent2_index];

		SBX(parent1, parent2, offspring, g_GlobalSettings->offspring_population_[1],
			g_GlobalSettings->dec_lower_bound_,g_GlobalSettings->dec_upper_bound_,cross_para_);
	}

	void CMOEAD::UpdateSubproblem(Individual *offspring, int current_index)
	{
		int replace_num = 0;
		double neighbour_fitness = 0.0, offspring_fitness = 0.0;

		int size = neighbour_type_ == NEIGHBOUR ? neighbour_num_ : weight_num_;
		std::vector<int> perm_index(size);
		random_permutation(perm_index.data(), size);

		for (int i = 0; i < size; ++i)
		{
			int weight_index;
			if (neighbour_type_ == NEIGHBOUR)
				weight_index = neighbour_[current_index][perm_index[i]];
			else
				weight_index = perm_index[i];

			Individual* current_ind = g_GlobalSettings->parent_population_[weight_index];

			// get constraint values
			double CVO = 0.0, CVP = 0.0;
			for (int j = 0; j < offspring->con_.size(); j++)
				CVO += std::max(0.0, offspring->con_[j]);
			for (int j = 0; j < current_ind->con_.size(); j++)
				CVP += std::max(0.0, current_ind->con_[j]);

			// calculate fitness;
			neighbour_fitness = CalPBI(current_ind, lambda_[weight_index], ideal_point_, g_GlobalSettings->obj_num_, pbi_theta_);
			offspring_fitness = CalPBI(offspring, lambda_[weight_index], ideal_point_, g_GlobalSettings->obj_num_, pbi_theta_);


			// update subproblem
			if ((offspring_fitness <= neighbour_fitness && std::fabs(CVO - CVP) < EMOC_EPS) || CVO < CVP)
			{
				CopyIndividual(offspring, g_GlobalSettings->parent_population_[weight_index]);
				replace_num++;
			}

			if (replace_num >= nr_) break;
		}
	}
}