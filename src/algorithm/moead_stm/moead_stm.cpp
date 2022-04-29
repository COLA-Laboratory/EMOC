#include "algorithm/moead_stm/moead_stm.h"

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

namespace emoc {

	MOEADSTM::MOEADSTM(int thread_id) :
		Algorithm(thread_id),
		lambda_(nullptr),
		weight_num_(0),
		neighbour_(nullptr),
		neighbour_selectpro_(0.9),
		ideal_point_(new double[g_GlobalSettings->obj_num_]),
		nadir_point_(new double[g_GlobalSettings->obj_num_])
	{

	}

	MOEADSTM::~MOEADSTM()
	{
		for (int i = 0; i < weight_num_; ++i)
		{
			delete[] lambda_[i];
			delete[] neighbour_[i];
			delete[] subproblem_matrix_[i];
			lambda_[i] = nullptr;
			neighbour_[i] = nullptr;
			subproblem_matrix_[i] = nullptr;
		}

		for (int i = 0; i < 2 * weight_num_; i++)
		{
			delete[] dist_matrix_[i];
			delete[] solution_matrix_[i];
			dist_matrix_[i] = nullptr;
			solution_matrix_[i] = nullptr;
		}

		delete[] lambda_;
		delete[] neighbour_;
		delete[] ideal_point_;
		delete[] nadir_point_;
		delete[] selected_indices_;
		delete[] old_obj_;
		delete[] utility_;
		delete[] delta_;
		delete[] dist_matrix_;
		delete[] solution_matrix_;
		delete[] subproblem_matrix_;
		lambda_ = nullptr;
		neighbour_ = nullptr;
		ideal_point_ = nullptr;
		nadir_point_ = nullptr;
		selected_indices_ = nullptr;
		old_obj_ = nullptr;
		utility_ = nullptr;
		delta_ = nullptr;
		dist_matrix_ = nullptr;
		solution_matrix_ = nullptr;
		subproblem_matrix_ = nullptr;
	}

	void MOEADSTM::Solve()
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
				offspring = g_GlobalSettings->offspring_population_[i];
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

				// update ideal point and nadir point
				UpdateIdealpoint(offspring, ideal_point_, g_GlobalSettings->obj_num_);
			}

			MergePopulation(g_GlobalSettings->parent_population_.data(), weight_num_, g_GlobalSettings->offspring_population_.data(), selected_size_,
				g_GlobalSettings->mixed_population_.data());
			UpdateNadirpoint(g_GlobalSettings->mixed_population_.data(), weight_num_ + selected_size_, nadir_point_, g_GlobalSettings->obj_num_);
			StableMatching(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->mixed_population_.data());
			
			if (g_GlobalSettings->iteration_num_ % 30 == 0)
			{
				for (int i = 0; i < weight_num_; ++i)
					CalInverseChebycheff(g_GlobalSettings->parent_population_[i], lambda_[i], ideal_point_, g_GlobalSettings->obj_num_);
				UpdateUtility();
			}
		}
	}

	void MOEADSTM::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

		// generate weight vectors
		lambda_ = UniformPoint(g_GlobalSettings->population_num_, &weight_num_, g_GlobalSettings->obj_num_);
		real_popnum_ = weight_num_;

		// set the neighbours of each individual
		SetNeighbours();

		// initialize ideal point and nadir point
		UpdateIdealpoint(g_GlobalSettings->parent_population_.data(), weight_num_, ideal_point_, g_GlobalSettings->obj_num_);
		UpdateNadirpoint(g_GlobalSettings->parent_population_.data(), weight_num_, nadir_point_, g_GlobalSettings->obj_num_);

		// set selected size
		selected_size_ = weight_num_ / 5;
		selected_indices_ = new int[selected_size_];

		// initialize utility related data
		old_obj_ = new double[weight_num_];
		delta_ = new double[weight_num_];
		utility_ = new double[weight_num_];
		for (int i = 0; i < weight_num_; ++i)
			utility_[i] = 1.0;

		// initialize stm related data
		dist_matrix_ = new double* [weight_num_ * 2];
		solution_matrix_ = new FitnessInfo * [weight_num_ * 2];
		subproblem_matrix_ = new FitnessInfo * [weight_num_];
		for (int i = 0; i < 2 * weight_num_; i++)
		{
			dist_matrix_[i] = new double[weight_num_];
			solution_matrix_[i] = new FitnessInfo[weight_num_];
		}
		for (int i = 0; i < weight_num_; i++)
			subproblem_matrix_[i] = new FitnessInfo[weight_num_ * 2];

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 0.5;
	}

	void MOEADSTM::SetNeighbours()
	{
		// set neighbour size and allocate memory
		neighbour_num_ = weight_num_ / 10;
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

	void MOEADSTM::Crossover(Individual** parent_pop, int current_index, Individual* offspring)
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

	void MOEADSTM::CalculatePref(Individual** mixed_pop)
	{
		int size = weight_num_ + selected_size_;
		std::vector<std::vector<double>> normalized_pop(size, std::vector<double>(g_GlobalSettings->obj_num_));
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < g_GlobalSettings->obj_num_; j++)
			{
				normalized_pop[i][j] = (mixed_pop[i]->obj_[j] - ideal_point_[j]) / (nadir_point_[j] - ideal_point_[j]);
			}
		}

		// calculate preference for solutions and subproblems
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < weight_num_; j++)
			{
				subproblem_matrix_[j][i].fit = CalInverseChebycheff(mixed_pop[i], lambda_[j], ideal_point_, g_GlobalSettings->obj_num_);
				subproblem_matrix_[j][i].index = i;
				dist_matrix_[i][j] = CalPerpendicularDistance(normalized_pop[i].data(), lambda_[j], g_GlobalSettings->obj_num_);
				solution_matrix_[i][j].fit = dist_matrix_[i][j];
				solution_matrix_[i][j].index = j;
			}
		}

		// sort subproblem matrix
		for (int i = 0; i < weight_num_; i++)
		{
			std::sort(subproblem_matrix_[i], subproblem_matrix_[i] + size, [](FitnessInfo& left, FitnessInfo& right) {
				return left.fit < right.fit;
				});
		}

		// sort solution matrix
		for (int i = 0; i < size; i++)
		{
			std::sort(solution_matrix_[i], solution_matrix_[i] + weight_num_, [](FitnessInfo& left, FitnessInfo& right) {
				return left.fit < right.fit;
				});
		}
	}

	void MOEADSTM::StableMatching(Individual** parent_pop, Individual** mixed_pop)
	{
		CalculatePref(mixed_pop);

		// initialize some basical data for stable matching
		int size = weight_num_ + selected_size_;
		std::vector<int> Fp(weight_num_);
		std::vector<int> Fx(size);
		std::vector<int> free_subproblem(weight_num_);
		std::vector<std::vector<int>> Phi(weight_num_, std::vector<int>(size));
		for (int i = 0; i < weight_num_; i++)
			for (int j = 0; j < size; j++) 
				Phi[i][j] = 0;
		
		for (int i = 0; i < weight_num_; i++)
		{
			Fp[i] = -1;
			free_subproblem[i] = i;
		}
		for (int i = 0; i < size; i++) Fx[i] = -1;

		int remain = weight_num_;
		while (remain > 0)
		{
			int rand_i = rnd(0, remain - 1);
			int current_subproblem = free_subproblem[rand_i];

			// find the most prefered unpursued solution
			int current_solution = -1;
			for (int i = 0; i < size; i++)
			{
				current_solution = subproblem_matrix_[current_subproblem][i].index;
				if (Phi[current_subproblem][current_solution] == 0)
				{
					Phi[current_subproblem][current_solution] = 1;
					break;
				}
			}

			// pursue the new solution
			if (Fx[current_solution] == -1)
			{
				free_subproblem[rand_i] = free_subproblem[remain - 1];
				Fp[current_subproblem] = current_solution;
				Fx[current_solution] = current_subproblem;
				remain--;
			}
			else
			{
				int predecessor = Fx[current_solution];
				if (dist_matrix_[current_solution][current_subproblem] < dist_matrix_[current_solution][predecessor])
				{
					free_subproblem[rand_i] = predecessor;
					Fp[current_subproblem] = current_solution;
					Fx[current_solution] = current_subproblem;
					Fp[predecessor] = -1;
				}
			}
		}

		for (int i = 0; i < weight_num_; i++)
			CopyIndividual(mixed_pop[Fp[i]], parent_pop[i]);
	}

	void MOEADSTM::SelectCurrentSubproblem()
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

	void MOEADSTM::CalculateFitness(Individual** pop, int pop_num, double* fitness)
	{
		for (int i = 0; i < pop_num; ++i)
		{
			double fit = CalInverseChebycheff(pop[i], lambda_[i], ideal_point_, g_GlobalSettings->obj_num_);
			fitness[i] = fit;
		}
	}

	void MOEADSTM::UpdateUtility()
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