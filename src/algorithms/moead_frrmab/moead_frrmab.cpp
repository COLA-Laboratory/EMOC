#include "algorithms/moead_frrmab/moead_frrmab.h"

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

	MOEADFRRMAB::MOEADFRRMAB(int thread_id) :
		Algorithm(thread_id),
		lambda_(nullptr),
		weight_num_(0),
		neighbour_(nullptr),
		neighbour_selectpro_(0.9),
		ideal_point_(new double[g_GlobalSettings->obj_num_])
	{

	}

	MOEADFRRMAB::~MOEADFRRMAB()
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
		delete[] sliding_window_;
		delete[] frr_;
		lambda_ = nullptr;
		neighbour_ = nullptr;
		ideal_point_ = nullptr;
		selected_indices_ = nullptr;
		old_obj_ = nullptr;
		utility_ = nullptr;
		delta_ = nullptr;
		sliding_window_ = nullptr;
		frr_ = nullptr;
	}

	void MOEADFRRMAB::Run()
	{
		int num[4] = {0};
		int offspring_count = 0;
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
					int op = SelectOperator();
					int index = selected_indices_[i];
					// set current iteration's neighbour type
					if (randomperc() < neighbour_selectpro_)
						neighbour_type_ = NEIGHBOUR;
					else
						neighbour_type_ = GLOBAL;

					// generate offspring for current subproblem
					Crossover(op,g_GlobalSettings->parent_population_.data(), index, offspring);
					MutationInd(offspring,g_GlobalSettings);
					EvaluateInd(offspring);

					// update ideal point
					UpdateIdealpoint(offspring, ideal_point_, g_GlobalSettings->obj_num_);

					// update neighbours' subproblem 
					double fir = UpdateSubproblem(offspring, index);

					// update sliding window
					sliding_window_[offspring_count % sliding_window_size_].op = op;
					sliding_window_[offspring_count % sliding_window_size_].fir = fir;
					offspring_count++;
					CreditAssignment();

					num[op] += 1;
				}
			}

			if (g_GlobalSettings->iteration_num_ % 10 == 0)
			{
				UpdateUtility();
			}
		}
		//std::cout << "evaluation:" << g_GlobalSettings->current_evaluation_ << "\n";

		for (int i = 0; i < 4; ++i)
		{
			//printf("op%d: %d\n", i, num[i]);
		}
	}

	void MOEADFRRMAB::Initialization()
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
		old_obj_	= new double[weight_num_];
		delta_		= new double[weight_num_];
		utility_	= new double[weight_num_];
		for (int i = 0; i < weight_num_; ++i)
			utility_[i] = 1.0;

		// initialize frrmab parameter
		D = 1; C = 5;
		operator_num_ = 4;
		frr_					= new double[operator_num_];
		sliding_window_size_	= (int)(0.5 * weight_num_);
		sliding_window_			= new FitnessInfo[sliding_window_size_];
		for (int i = 0; i < sliding_window_size_; ++i)
		{
			sliding_window_[i].op = -1; 
			sliding_window_[i].fir = 0;
		}
		for (int i = 0; i < operator_num_; ++i)
			frr_[i] = 0.0;
	}

	void MOEADFRRMAB::SetNeighbours()
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

	void MOEADFRRMAB::CreditAssignment()
	{
		double *reward = new double[operator_num_];
		int *sort_index = new int[operator_num_];
		int *rank = new int[operator_num_];
		double decaySum = 0;

		for (int i = 0; i < operator_num_; ++i)
		{
			reward[i] = 0;
			sort_index[i] = i;
		}

		// calculate reward
		for (int i = 0; i < sliding_window_size_; ++i)
		{
			if (sliding_window_[i].op != -1)
			{
				reward[sliding_window_[i].op] += sliding_window_[i].fir;
			}
		}
		
		// sort operator index based on reward
		std::sort(sort_index, sort_index + operator_num_, [=](int left, int right) {
			return reward[left] < reward[right];
		});

		// set rank array
		for (int i = 0; i < operator_num_; ++i)
		{
			rank[sort_index[i]] = operator_num_ - i;
		}
		
		// calculate frr
		for (int i = 0; i < operator_num_;  ++i)
		{
			reward[i] = reward[i] * pow(D, (double)rank[i]);
			decaySum += reward[i];
		}
		for (int i = 0; i < operator_num_; ++i)
		{
			frr_[i] = reward[i] / decaySum;
		}

		delete[] sort_index;
		delete[] rank;
		delete[] reward;
	}

	int MOEADFRRMAB::SelectOperator()
	{
		int op = 0;
		int flag = 0;
		int N[10], sum_N = 0; // record the number of usage of each operator
		double *ucb_value = new double[operator_num_];

		for (int i = 0; i < operator_num_; ++i)
			N[i] = 0;

		// check whether all operator has reward
		for (int i = 0; i < operator_num_; ++i)
		{
			if (frr_[i] == 0)
			{
				flag = 1;
				break;
			}
		}

		// select operator
		if (flag)
		{
			op = rnd(0, operator_num_ - 1);
		}
		else
		{		
			for (int i = 0; i < sliding_window_size_; ++i)
				if(sliding_window_[i].op != -1)
					N[sliding_window_[i].op] += 1;
			
			for (int i = 0; i < operator_num_; ++i)
				sum_N += N[i];
			
			for (int i = 0; i < operator_num_; ++i)
				ucb_value[i] = frr_[i] + C * sqrt(2 * log((double)sum_N) / (double)N[i]);

			int max_op = 0;
			double max = ucb_value[0];
			for (int i = 1; i < operator_num_; ++i)
			{
				if (max < ucb_value[i])
				{
					max = ucb_value[i];
					max_op = i;
				}
			}

			op = max_op;
		}

		delete[] ucb_value;


		return op;
	}

	void MOEADFRRMAB::Crossover(int operator_index, Individual **parent_pop, int current_index, Individual *offspring)
	{
		int size = neighbour_type_ == NEIGHBOUR ? neighbour_num_ : weight_num_;
		int parent2_index = 0, parent3_index = 0, parent4_index = 0, parent5_index = 0, parent6_index = 0;
		int *permutation = new int[size];
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

		Individual *parent1 = parent_pop[current_index];
		Individual *parent2 = parent_pop[parent2_index];
		Individual *parent3 = parent_pop[parent3_index];
		Individual *parent4 = parent_pop[parent4_index];
		Individual *parent5 = parent_pop[parent5_index];
		Individual *parent6 = parent_pop[parent6_index];

		// use selected operator to crossover
		double value = 0;
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

	double MOEADFRRMAB::UpdateSubproblem(Individual *offspring, int current_index)
	{
		double fir = 0;
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
				fir += (neighbour_fitness - offspring_fitness) / neighbour_fitness;
				CopyIndividual(offspring, g_GlobalSettings->parent_population_[weight_index]);
				count++;
			}
		}

		delete[] perm_index;
		return fir;
	}

	void MOEADFRRMAB::SelectCurrentSubproblem()
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

	void MOEADFRRMAB::CalculateFitness(Individual **pop, int pop_num, double *fitness)
	{
		for (int i = 0; i < pop_num; ++i)
		{
			double fit = CalInverseChebycheff(pop[i], lambda_[i], ideal_point_, g_GlobalSettings->obj_num_);
			fitness[i] = fit;
		}
	}

	void MOEADFRRMAB::UpdateUtility()
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