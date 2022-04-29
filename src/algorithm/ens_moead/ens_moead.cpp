#include "algorithm/ens_moead/ens_moead.h"

#include <cmath>
#include <algorithm>
#include <iostream>

#include "core/macro.h"
#include "core/global.h"
#include "core/utility.h"
#include "core/uniform_point.h"
#include "operator/de.h"
#include "operator/polynomial_mutation.h"
#include "random/random.h"

namespace emoc {

	ENSMOEAD::ENSMOEAD(int thread_id) :
		Algorithm(thread_id),
		lambda_(nullptr),
		weight_num_(0),
		neighbour_(nullptr),
		neighbour_selectpro_(0.9),
		ideal_point_(new double[g_GlobalSettings->obj_num_])
	{
		for (int i = 0; i < 4; ++i)
		{
			R_[i] = 0.0001;
			FEs_[i] = 1.0;
			FEs_success_[i] = 0.0;
			P_[i] = 0.25;
			NS_[i] = 24 * (i + 1);
		}
		LP_ = 50;
	}

	ENSMOEAD::~ENSMOEAD()
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
		delete[] selected_indices_;
		delete[] old_obj_;
		delete[] utility_;
		delete[] delta_;
		lambda_ = nullptr;
		neighbour_ = nullptr;
		ideal_point_ = nullptr;
		selected_indices_ = nullptr;
		old_obj_ = nullptr;
		utility_ = nullptr;
		delta_ = nullptr;
	}

	void ENSMOEAD::Solve()
	{
		Initialization();

		// calculate and store the old fitness
		CalculateFitness(g_GlobalSettings->parent_population_.data(), weight_num_, old_obj_);
		Individual *offspring = g_GlobalSettings->offspring_population_[0];

		while (!IsTermination())
		{
			for (int subgeneration = 0; subgeneration < 5; ++subgeneration)
			{
				SelectCurrentSubproblem();
				for (int i = 0; i < selected_size_; ++i)
				{
					int ns_index = SelectNS();
					int index = selected_indices_[i];
					// set current iteration's neighbour type
					if (randomperc() < neighbour_selectpro_)
						neighbour_type_ = NEIGHBOUR;
					else
						neighbour_type_ = GLOBAL;

					// generate offspring for current subproblem
					Crossover(g_GlobalSettings->parent_population_.data(), index, offspring);
					PolynomialMutation(offspring, g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, mutation_para_);
					EvaluateInd(offspring);

					// update ideal point
					UpdateIdealpoint(offspring, ideal_point_, g_GlobalSettings->obj_num_);

					// update neighbours' subproblem 
					int updated_num = UpdateSubproblem(offspring, index);

					FEs_[ns_index] += 1;
					FEs_success_[ns_index] += (updated_num ? 1 : 0);
				}
			}

			if (g_GlobalSettings->iteration_num_ % 10 == 0)
			{
				UpdateUtility();
			}

			if (g_GlobalSettings->iteration_num_ % LP_ == 0)
			{
				UpdateProbability();
			}
		}
	}

	void ENSMOEAD::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

		// generate weight vectors
		lambda_ = UniformPoint(g_GlobalSettings->population_num_, &weight_num_, g_GlobalSettings->obj_num_);
		real_popnum_ = weight_num_;
		
		// check the population size
		if (weight_num_ < 97)
		{
			std::cout << "In ENSMOEAD, the population size should be more than 97, or you need to change the NS parameters for algorithm.\n" << std::endl;
			exit(1);
		}
		replace_num_ = (weight_num_ / 100) ? (weight_num_ / 100) : 2;

		// set the neighbours of each individual
		SetNeighbours();

		// initialize ideal point
		UpdateIdealpoint(g_GlobalSettings->parent_population_.data(), weight_num_, ideal_point_, g_GlobalSettings->obj_num_);

		// set selected size
		selected_size_ = weight_num_ / 5;
		selected_indices_ = new int[selected_size_];

		// initialize utility related data
		old_obj_ = new double[weight_num_];
		delta_ = new double[weight_num_];
		utility_ = new double[weight_num_];
		for (int i = 0; i < weight_num_; ++i)
			utility_[i] = 1.0;

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 0.5;
	}

	void ENSMOEAD::SetNeighbours()
	{
		// set neighbour size and allocate memory
		neighbour_num_ = weight_num_ - 1;
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
				neighbour_[i][j] = sort_list[j + 1].index;
			}
		}

	}

	void ENSMOEAD::Crossover(Individual **parent_pop, int current_index, Individual *offspring)
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
		DE(parent1, parent2, parent3, offspring,g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, cross_para_);
	}

	int ENSMOEAD::UpdateSubproblem(Individual *offspring, int current_index)
	{
		int size = neighbour_type_ == NEIGHBOUR ? neighbour_num_ : weight_num_;
		std::vector<int> perm_index(size, 0);
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

			Individual *current_ind = g_GlobalSettings->parent_population_[weight_index];
			offspring_fitness = CalInverseChebycheff(offspring, lambda_[weight_index], ideal_point_, g_GlobalSettings->obj_num_);
			neighbour_fitness = CalInverseChebycheff(current_ind, lambda_[weight_index], ideal_point_, g_GlobalSettings->obj_num_);
			if (offspring_fitness < neighbour_fitness)
			{
				CopyIndividual(offspring, g_GlobalSettings->parent_population_[weight_index]);
				count++;
			}
		}

		return count;
	}

	void ENSMOEAD::SelectCurrentSubproblem()
	{
		int rand[10] = { 0 }, current_max_index = 0;

		// add boundry first
		for (int i = 0; i < g_GlobalSettings->obj_num_; ++i)
		{
			for (int j = 0; j < weight_num_; ++j)
			{
				if (fabs(lambda_[j][i] - 1) < EMOC_EPS)
					selected_indices_[i] = j;
			}
		}

		// 10-tournment selection
		for (int i = g_GlobalSettings->obj_num_; i < selected_size_; ++i)
		{
			for (int j = 0; j < 10; ++j)
			{
				rand[j] = rnd(0, weight_num_ - 1);
			}

			double max = -EMOC_INF;
			for (int j = 0; j < 10; ++j)
			{
				if (utility_[rand[j]] > max)
				{
					max = utility_[rand[j]];
					current_max_index = rand[j];
				}
			}
			selected_indices_[i] = current_max_index;
		}
	}

	void ENSMOEAD::CalculateFitness(Individual **pop, int pop_num, double *fitness)
	{
		for (int i = 0; i < pop_num; ++i)
		{
			double fit = CalInverseChebycheff(pop[i], lambda_[i], ideal_point_, g_GlobalSettings->obj_num_);
			fitness[i] = fit;
		}
	}

	void ENSMOEAD::UpdateUtility()
	{
		// calculate delta
		for (int i = 0; i < weight_num_; ++i)
		{
			delta_[i] = fabs(g_GlobalSettings->parent_population_[i]->fitness_ - old_obj_[i]) / old_obj_[i];
			old_obj_[i] = g_GlobalSettings->parent_population_[i]->fitness_;
		}

		// update utility
		for (int i = 0; i < weight_num_; ++i)
		{
			if (delta_[i] > 0.001)
			{
				utility_[i] = 1.0;
			}
			else
			{
				utility_[i] = utility_[i] * (0.95 + 0.05 * (delta_[i] / 0.001));
			}
		}
	}

	int ENSMOEAD::SelectNS()
	{
		int ns_index = 0;
		double rand = randomperc();

		double temp_sum = 0;
		for (int i = 0; i < 4; i++)
		{
			if (rand >= temp_sum && rand < temp_sum + P_[i])
			{
				ns_index = i;
				break;
			}
			temp_sum += P_[i];
		}

		// set the neighbour size according to the selection result
		neighbour_num_ = NS_[ns_index];

		return ns_index;
	}

	void ENSMOEAD::UpdateProbability()
	{
		double sum = 0;

		// calculate the probability
		for (int i = 0; i < 4; ++i)
		{
			R_[i] = FEs_success_[i] / FEs_[i] + 0.0001;
		}

		for (int i = 0; i < 4; ++i)
		{
			sum += R_[i];
		}

		for (int i = 0; i < 4; i++)
		{
			P_[i] = R_[i] / sum;
		}

		// recover
		for (int i = 0; i < 4; i++)
		{
			R_[i] = 0.0001;
			FEs_[i] = 1;
			FEs_success_[i] = 0;
		}
	}

}