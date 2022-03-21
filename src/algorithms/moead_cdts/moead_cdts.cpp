#include "algorithms/moead_cdts/moead_cdts.h"

#include <cmath>
#include <iostream>
#include <algorithm>

#include "core/macro.h"
#include "core/global.h"
#include "core/utility.h"
#include "core/uniform_point.h"
#include "operator/de.h"
#include "operator/mutation.h"
#include "random/random.h"

namespace emoc {

	MOEADCDTS::MOEADCDTS(int thread_id) :
		Algorithm(thread_id),
		lambda_(nullptr),
		weight_num_(0),
		neighbour_(nullptr),
		neighbour_selectpro_(0.8),
		ideal_point_(new double[g_GlobalSettings->obj_num_])
	{

	}

	MOEADCDTS::~MOEADCDTS()
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

	void MOEADCDTS::Run()
	{
		int num[5] = { 0 };
		int offspring_count = 0;
		Initialization();

		// calculate and store the old fitness
		CalculateFitness(g_GlobalSettings->parent_population_.data(), weight_num_, old_obj_);
		Individual* offspring = g_GlobalSettings->offspring_population_[0];

		while (!IsTermination())
		{
			SelectCurrentSubproblem();
			for (int i = 0; i < selected_size_; ++i)
			{
				int op = SelectOperator(); num[op] += 1;
				int index = selected_indices_[i];

				// set current iteration's neighbour type
				if (randomperc() < neighbour_selectpro_)
					neighbour_type_ = NEIGHBOUR;
				else
					neighbour_type_ = GLOBAL;

				// generate offspring for current subproblem
				Crossover(op, g_GlobalSettings->parent_population_.data(), index, offspring);
				MutationInd(offspring, g_GlobalSettings);
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

	void MOEADCDTS::Initialization()
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
		W = 0.5 * weight_num_;
		H = std::log10((double)g_GlobalSettings->max_evaluation_ / (double)g_GlobalSettings->population_num_);
		count_.resize(5, 0);
		reward_sw_.resize(5, std::vector<int>(g_GlobalSettings->max_evaluation_, -1));
		C = 100;
		operator_num_ = 5;
		alpha_ = new double[operator_num_];
		beta_ = new double[operator_num_];
		for (int i = 0; i < operator_num_; i++)
		{
			alpha_[i] = 1.0f;
			beta_[i] = 1.0f;
		}
	}

	void MOEADCDTS::SetNeighbours()
	{
		// set neighbour size and allocate memory
		neighbour_num_ = 20;
		neighbour_ = new int* [weight_num_];
		for (int i = 0; i < weight_num_; ++i)
		{
			neighbour_[i] = new int[neighbour_num_];
		}

		DistanceInfo* sort_list = new DistanceInfo[weight_num_];
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

			std::sort(sort_list, sort_list + weight_num_, [](DistanceInfo& left, DistanceInfo& right) {
				return left.distance < right.distance;
				});

			for (int j = 0; j < neighbour_num_; j++)
			{
				neighbour_[i][j] = sort_list[j + 1].index;
			}
		}

		delete[] sort_list;
	}


	int MOEADCDTS::SelectOperator()
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

	bool MOEADCDTS::ChangeDetection(int op)
	{
		bool res = false;
		double sum = 0.0;
		double g_plus = 0.0, g_minus = 0.0;

		for (int i = 0; i < W-1; i++)
			sum += reward_sw_[op][i];

		for (int i = W-1; i < count_[op]; i++)
		{
			sum += reward_sw_[op][i];
			double s_plus = reward_sw_[op][i] - sum / (i + 1);
			double s_minus = sum / (i + 1) - reward_sw_[op][i];
			g_plus = std::max(0.0, g_plus + s_plus);
			g_minus = std::max(0.0, g_minus + s_minus);
			if (g_plus >= H || g_minus >= H)
			{
				res = true;
				break;
			}
		}

		return res;
	}

	void MOEADCDTS::UpdateBetaDis(int op, double reward)
	{
		int index = count_[op];
		reward_sw_[op][index] = reward > 0;
		count_[op] += 1;

		// update beta distribution
		if (reward > 0)
			alpha_[op] += 1;
		else
			beta_[op] += 1;

		if (count_[op] >= W)
		{
			// change detection
			if (ChangeDetection(op))
			{
				//std::cout << "Change Detected ! operator: " << op << " seleted times: " << count_[op] << " current eva: " << g_GlobalSettings->current_evaluation_ << "\n";
				alpha_[op] = 1;
				beta_[op] = 1;
				count_[op] = 0;
			}
		}
	}

	void MOEADCDTS::Crossover(int operator_index, Individual** parent_pop, int current_index, Individual* offspring)
	{
		int size = neighbour_type_ == NEIGHBOUR ? neighbour_num_ : weight_num_;
		int parent2_index = 0, parent3_index = 0, parent4_index = 0, parent5_index = 0, parent6_index = 0;
		int* permutation = new int[size];
		random_permutation(permutation, size);

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

		delete[] permutation;
	}

	double MOEADCDTS::UpdateSubproblem(Individual* offspring, int current_index)
	{
		double offspring_fitness = 0.0;
		double neighbour_fitness = 0.0;
		DistanceInfo* sort_list = new DistanceInfo[weight_num_];

		// calculate fitness improvement for each subproblem;
		for (int i = 0; i < weight_num_; ++i)
		{
			offspring_fitness = CalInverseChebycheff(offspring, lambda_[i], ideal_point_, g_GlobalSettings->obj_num_);
			neighbour_fitness = CalInverseChebycheff(g_GlobalSettings->parent_population_[i], lambda_[i], ideal_point_, g_GlobalSettings->obj_num_);

			sort_list[i].index = i;
			sort_list[i].distance = (neighbour_fitness - offspring_fitness) / neighbour_fitness;
		}

		std::sort(sort_list, sort_list + weight_num_, [](DistanceInfo& left, DistanceInfo& right) {
			return left.distance < right.distance;
			});

		// update population
		int index = sort_list[weight_num_ - 1].index;
		offspring_fitness = CalInverseChebycheff(offspring, lambda_[index], ideal_point_, g_GlobalSettings->obj_num_);
		neighbour_fitness = CalInverseChebycheff(g_GlobalSettings->parent_population_[index], lambda_[index], ideal_point_, g_GlobalSettings->obj_num_);
		if (offspring_fitness < neighbour_fitness)
			CopyIndividual(offspring, g_GlobalSettings->parent_population_[index]);

		delete[] sort_list;

		return neighbour_fitness - offspring_fitness;
	}

	void MOEADCDTS::SelectCurrentSubproblem()
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

	void MOEADCDTS::CalculateFitness(Individual** pop, int pop_num, double* fitness)
	{
		for (int i = 0; i < pop_num; ++i)
		{
			double fit = CalInverseChebycheff(pop[i], lambda_[i], ideal_point_, g_GlobalSettings->obj_num_);
			fitness[i] = fit;
		}
	}

	void MOEADCDTS::UpdateUtility()
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
