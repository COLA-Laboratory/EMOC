#include "algorithm/moead_dyts/moead_dyts.h"

#include <cmath>
#include <iostream>
#include <algorithm>

#include "core/macro.h"
#include "core/global.h"
#include "core/utility.h"
#include "core/uniform_point.h"
#include "operator/de.h"
#include "operator/polynomial_mutation.h"
#include "random/random.h"

namespace emoc{

	MOEADDYTS::MOEADDYTS(int thread_id) :
		Algorithm(thread_id),
		lambda_(nullptr),
		weight_num_(0),
		neighbour_(nullptr),
		neighbour_selectpro_(0.8),
		ideal_point_(new double[g_GlobalSettings->obj_num_])
	{

	}

	MOEADDYTS::~MOEADDYTS()
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
		delete[] alpha_;
		delete[] beta_;
		lambda_ = nullptr;
		neighbour_ = nullptr;
		ideal_point_ = nullptr;
		selected_indices_ = nullptr;
		old_obj_ = nullptr;
		utility_ = nullptr;
		delta_ = nullptr;
		alpha_ = nullptr;
		beta_ = nullptr;
	}

	void MOEADDYTS::Solve()
	{
		Initialization();

		// calculate and store the old fitness
		CalculateFitness(g_GlobalSettings->parent_population_.data(), weight_num_, old_obj_);
		Individual* offspring = g_GlobalSettings->offspring_population_[0];

		while (!IsTermination())
		{

			SelectCurrentSubproblem();
			for (int i = 0; i < selected_size_; ++i)
			{
				int op = SelectOperator();
				int index = selected_indices_[i];

				// set current iteration's neighbour type
				if (randomperc() < neighbour_selectpro_)
					neighbour_type_ = NEIGHBOUR;
				else
					neighbour_type_ = GLOBAL;

				// generate offspring for current subproblem
				Crossover(op, g_GlobalSettings->parent_population_.data(), index, offspring);
				PolynomialMutation(offspring, g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, mutation_para_);
				EvaluateInd(offspring);

				// update ideal point
				UpdateIdealpoint(offspring, ideal_point_, g_GlobalSettings->obj_num_);

				// update neighbours' subproblem 
				double fi = UpdateSubproblem(offspring, index);

				// update beta distribution
				UpdateBetaDis(op, fi);
			}

			if (g_GlobalSettings->iteration_num_ % 50 == 0)
			{
				UpdateUtility();
			}
		}
	}

	void MOEADDYTS::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

		// generate weight vectors
		lambda_ = UniformPoint(g_GlobalSettings->population_num_, &weight_num_, g_GlobalSettings->obj_num_);
		real_popnum_ = weight_num_;
		replace_num_ = 2;

		// set the neighbours of each individual
		SetNeighbours();

		// initialize ideal point
		UpdateIdealpoint(g_GlobalSettings->parent_population_.data(), weight_num_, ideal_point_, g_GlobalSettings->obj_num_);

		// set selected size
		selected_size_ = weight_num_ / 5;
		selected_indices_ = new int[selected_size_];

		// initialize utility related parameter
		old_obj_ = new double[weight_num_];
		delta_ = new double[weight_num_];
		utility_ = new double[weight_num_];
		for (int i = 0; i < weight_num_; ++i)
			utility_[i] = 1.0;

		// initialize dyts parameter
		C = 100;
		operator_num_ = 5;
		alpha_ = new double[operator_num_];
		beta_ = new double[operator_num_];
		for (int i = 0; i < operator_num_; i++)
		{
			alpha_[i] = 1.0f;
			beta_[i] = 1.0f;
		}

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;
	}

	void MOEADDYTS::SetNeighbours()
	{
		// set neighbour size and allocate memory
		neighbour_num_ = 20;
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


	int MOEADDYTS::SelectOperator()
	{
		// select operator according to thompsom sampling
		int op = 0, i = 0;
		double max = 0, temp = 0;

		max = BetaRandom(alpha_[0], beta_[0]);
		for (i = 1; i < operator_num_; i++)
		{
			temp = BetaRandom(alpha_[i], beta_[i]);
			if (max < temp)
			{
				max = temp;
				op = i;
			}
		}

		return op;
	}

	void MOEADDYTS::UpdateBetaDis(int op, double reward)
	{
		if (alpha_[op] + beta_[op] < C)
		{
			if (reward > 0)
				alpha_[op] += 1;
			else
				beta_[op] += 1;
		}
		else
		{
			if (reward > 0)
			{
				alpha_[op] = (alpha_[op] + 1) * C / (1.0 + C);
				beta_[op] = beta_[op] * C / (1.0 + C);
			}
			else
			{
				alpha_[op] = alpha_[op] * C / (1.0 + C);
				beta_[op] = (beta_[op] + 1) * C / (1.0 + C);
			}
		}
	}

	void MOEADDYTS::Crossover(int operator_index, Individual** parent_pop, int current_index, Individual* offspring)
	{
		int size = neighbour_type_ == NEIGHBOUR ? neighbour_num_ : weight_num_;
		int parent2_index = 0, parent3_index = 0, parent4_index = 0, parent5_index = 0, parent6_index = 0;
		std::vector<int> permutation(size);
		random_permutation(permutation.data(), size);

		// randomly select extra 5 parents according to neighbour type
		if (neighbour_type_ == NEIGHBOUR)
		{
			parent2_index = neighbour_[current_index][permutation[0]];
			parent3_index = neighbour_[current_index][permutation[1]];
			parent4_index = neighbour_[current_index][permutation[2]];
			parent5_index = neighbour_[current_index][permutation[3]];
			parent6_index = neighbour_[current_index][permutation[4]];
		}
		else
		{
			parent2_index = permutation[0];
			parent3_index = permutation[1];
			parent4_index = permutation[2];
			parent5_index = permutation[3];
			parent6_index = permutation[4];
		}

		Individual* parent1 = parent_pop[current_index];
		Individual* parent2 = parent_pop[parent2_index];
		Individual* parent3 = parent_pop[parent3_index];
		Individual* parent4 = parent_pop[parent4_index];
		Individual* parent5 = parent_pop[parent5_index];
		Individual* parent6 = parent_pop[parent6_index];

		// use selected operator to crossover
		double value = 0;
		double um_pro = 1.0 / (double)g_GlobalSettings->dec_num_;
		for (int i = 0; i < g_GlobalSettings->dec_num_; ++i)
		{
			switch (operator_index)
			{
			case 0:
				value = parent1->dec_[i] + g_GlobalSettings->de_parameter_.F * (parent2->dec_[i] - parent3->dec_[i]);
				break;
			case 1:
				value = parent1->dec_[i] + g_GlobalSettings->de_parameter_.F * (parent2->dec_[i] - parent3->dec_[i])
					+ g_GlobalSettings->de_parameter_.F * (parent4->dec_[i] - parent5->dec_[i]);
				break;
			case 2:
				value = parent1->dec_[i] + g_GlobalSettings->de_parameter_.K * (parent1->dec_[i] - parent2->dec_[i])
					+ g_GlobalSettings->de_parameter_.F * (parent3->dec_[i] - parent4->dec_[i])
					+ g_GlobalSettings->de_parameter_.F * (parent5->dec_[i] - parent6->dec_[i]);
				break;
			case 3:
				value = parent1->dec_[i] + g_GlobalSettings->de_parameter_.K * (parent1->dec_[i] - parent2->dec_[i])
					+ g_GlobalSettings->de_parameter_.F * (parent3->dec_[i] - parent4->dec_[i]);
				break;
			case 4:
				value = randomperc() < um_pro ?
					parent1->dec_[i] + rndreal(-1.0,1.0) * (g_GlobalSettings->dec_upper_bound_[i]-g_GlobalSettings->dec_lower_bound_[i]) : parent1->dec_[i];
				break;
			default:
				break;
			}

			// repair if necessary
			if (value > g_GlobalSettings->dec_upper_bound_[i])
				value = g_GlobalSettings->dec_upper_bound_[i];
			if (value < g_GlobalSettings->dec_lower_bound_[i])
				value = g_GlobalSettings->dec_lower_bound_[i];

			if (operator_index < 2)
			{
				if (randomperc() < g_GlobalSettings->de_parameter_.crossover_pro)
					offspring->dec_[i] = (value);
				else
					offspring->dec_[i] = (parent1->dec_[i]);
			}
			else
			{
				offspring->dec_[i] = (value);
			}
		}
	}

	double MOEADDYTS::UpdateSubproblem(Individual* offspring, int current_index)
	{
		double offspring_fitness = 0.0;
		double neighbour_fitness = 0.0;
		std::vector<DistanceInfo> sort_list(weight_num_);

		// calculate fitness improvement for each subproblem;
		for (int i = 0; i < weight_num_; ++i)
		{
			offspring_fitness = CalInverseChebycheff(offspring, lambda_[i], ideal_point_, g_GlobalSettings->obj_num_);
			neighbour_fitness = CalInverseChebycheff(g_GlobalSettings->parent_population_[i], lambda_[i], ideal_point_, g_GlobalSettings->obj_num_);

			sort_list[i].index = i;
			sort_list[i].distance = (neighbour_fitness - offspring_fitness) / neighbour_fitness;
		}

		std::sort(sort_list.begin(), sort_list.end(), [](DistanceInfo& left, DistanceInfo& right) {
			return left.distance < right.distance;
			});

		// update population
		int index = sort_list[weight_num_ - 1].index;
		offspring_fitness = CalInverseChebycheff(offspring, lambda_[index], ideal_point_, g_GlobalSettings->obj_num_);
		neighbour_fitness = CalInverseChebycheff(g_GlobalSettings->parent_population_[index], lambda_[index], ideal_point_, g_GlobalSettings->obj_num_);
		if (offspring_fitness < neighbour_fitness)
			CopyIndividual(offspring, g_GlobalSettings->parent_population_[index]);

		return neighbour_fitness - offspring_fitness;
	}

	void MOEADDYTS::SelectCurrentSubproblem()
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

	void MOEADDYTS::CalculateFitness(Individual** pop, int pop_num, double* fitness)
	{
		for (int i = 0; i < pop_num; ++i)
		{
			double fit = CalInverseChebycheff(pop[i], lambda_[i], ideal_point_, g_GlobalSettings->obj_num_);
			fitness[i] = fit;
		}
	}

	void MOEADDYTS::UpdateUtility()
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
}
