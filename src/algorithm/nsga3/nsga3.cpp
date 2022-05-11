#include "algorithm/nsga3/nsga3.h"

#include <vector>
#include <algorithm>
#include <iostream>

#include "core/macro.h"
#include "core/global.h"
#include "core/utility.h"
#include "core/nd_sort.h"
#include "core/uniform_point.h"
#include "operator/tournament_selection.h"
#include "operator/polynomial_mutation.h"
#include "operator/sbx.h"
#include "random/random.h"

namespace emoc {

	NSGA3::NSGA3(int thread_id) :Algorithm(thread_id),ideal_point_(nullptr)
	{
	}

	NSGA3::~NSGA3()
	{
		delete[] ideal_point_;
		delete[] lambda_;
		ideal_point_ = nullptr;
		lambda_ = nullptr;

		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
			delete extreme_pop_[i];
		for (int i = 0; i < 2 * real_popnum_; i++)
			delete ndpop_[i];
	}

	void NSGA3::Solve()
	{
		Initialization();

		while (!IsTermination())
		{
			// generate offspring population
			Crossover(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->offspring_population_.data());
			PolynomialMutation(g_GlobalSettings->offspring_population_.data(), 2 * (real_popnum_ / 2), g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, mutation_para_);
			EvaluatePop(g_GlobalSettings->offspring_population_.data(), 2 * (real_popnum_ / 2));
			MergePopulation(g_GlobalSettings->parent_population_.data(), real_popnum_, g_GlobalSettings->offspring_population_.data(),
				2 * (real_popnum_ / 2), g_GlobalSettings->mixed_population_.data());
			
			// update ideal point
			for (int i = 0; i < 2 * (real_popnum_ / 2); i++)
				UpdateIdealpoint(g_GlobalSettings->offspring_population_[i], ideal_point_, g_GlobalSettings->obj_num_);

			// select next generation's population
			EnvironmentalSelection(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->mixed_population_.data(), 2 * (real_popnum_ / 2) + real_popnum_);
		}
	}

	void NSGA3::Initialization()
	{
		// generate weight vectors
		lambda_ = UniformPoint(g_GlobalSettings->population_num_, &weight_num_, g_GlobalSettings->obj_num_);
		real_popnum_ = weight_num_;

		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), real_popnum_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), real_popnum_);

		// initialize ideal point
		ideal_point_ = new double[g_GlobalSettings->obj_num_];
		UpdateIdealpoint(g_GlobalSettings->parent_population_.data(), real_popnum_, ideal_point_, g_GlobalSettings->obj_num_);

		// initialize nd pop and extreme pop
		for (int i = 0; i < 2 * real_popnum_; i++)
			ndpop_.push_back(new Individual(g_GlobalSettings->dec_num_, g_GlobalSettings->obj_num_));
		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
			extreme_pop_.push_back(new Individual(g_GlobalSettings->dec_num_, g_GlobalSettings->obj_num_));

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 20.0;
	}

	void NSGA3::Crossover(Individual **parent_pop, Individual **offspring_pop)
	{
		// generate random permutation index for tournment selection
		std::vector<int> index1(real_popnum_);
		std::vector<int> index2(real_popnum_);
		random_permutation(index1.data(), real_popnum_);
		random_permutation(index2.data(), real_popnum_);

		for (int i = 0; i < real_popnum_ / 2; ++i)
		{
			Individual *parent1 = TournamentByRank(parent_pop[index1[2 * i]], parent_pop[index1[2 * i + 1]]);
			Individual *parent2 = TournamentByRank(parent_pop[index2[2 * i]], parent_pop[index2[2 * i + 1]]);
			SBX(parent1, parent2, offspring_pop[2 * i], offspring_pop[2 * i + 1],
				g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, cross_para_);
		}
	}

	void NSGA3::EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop, int mixpop_num)
	{
		// get non-dominated population
		int ndpop_num = 0;
		GetNdPop(mixed_pop, mixpop_num, ndpop_.data(), ndpop_num);
		if (ndpop_num == real_popnum_)
		{
			// direct copy the non-dominated population to next generation
			for (int i = 0; i < ndpop_num; i++)
				CopyIndividual(ndpop_[i], parent_pop[i]);
			return;
		}

		// get extreme population and intercepts
		std::vector<double> intercepts(g_GlobalSettings->obj_num_);
		GetExtremePop(ndpop_.data(), ndpop_num, extreme_pop_.data());
		GetIntercepts(extreme_pop_.data(), ndpop_.data(), ndpop_num, intercepts.data());

		// normalization
		double** normalized_pop = new double* [ndpop_num];
		for (int i = 0; i < ndpop_num; i++)
			normalized_pop[i] = new double[g_GlobalSettings->obj_num_];
		for (int i = 0; i < ndpop_num; i++)
			for (int j = 0; j < g_GlobalSettings->obj_num_; j++)
				normalized_pop[i][j] = (ndpop_[i]->obj_[j] - ideal_point_[j]) / (intercepts[j] - ideal_point_[j]);

		// associate to reference point
		std::vector<int> pi(ndpop_num);
		std::vector<double> distance(ndpop_num);
		Association(normalized_pop, ndpop_num, pi.data(), distance.data());

		// get the number of associated solutions except for the last front of each reference point
		std::vector<int> rho(real_popnum_, 0);
		std::vector<int> pop1_indices, pop2_indices;
		int max_front = ndpop_[0]->rank_;
		for (int i = 1; i < ndpop_num; i++)
			if (max_front < ndpop_[i]->rank_)
				max_front = ndpop_[i]->rank_;
		
		for (int i = 0; i < ndpop_num; i++)
		{
			if (ndpop_[i]->rank_ < max_front)
				pop1_indices.push_back(i);
			else if (ndpop_[i]->rank_ == max_front)
				pop2_indices.push_back(i);
		}

		for (int i = 0; i < pop1_indices.size(); i++)
		{
			int index = pop1_indices[i];
			rho[pi[index]]++;
		}

		// environment selection
		int remain = real_popnum_ - pop1_indices.size();
		std::vector<int> selected_indices, lambda_choosed(real_popnum_, 0);
		std::vector<int> pop2_choosed(pop2_indices.size(), 0);
		while (remain)
		{
			std::vector<int> Jmin;
			std::vector<int> I;

			// initialize Jmin
			int j_min = EMOC_INF;
			for (int i = 0; i < real_popnum_; i++)
				if (!lambda_choosed[i] && j_min > rho[i]) j_min = rho[i];
			for (int i = 0; i < real_popnum_; i++)
				if (!lambda_choosed[i] && rho[i] == j_min) Jmin.push_back(i);

			// initialize I
			int selected_j = Jmin[rnd(0, Jmin.size() - 1)];
			for (int i = 0; i < pop2_indices.size(); i++)
				if (!pop2_choosed[i] && pi[pop2_indices[i]] == selected_j) I.push_back(i);

			if (I.empty())
			{
				lambda_choosed[selected_j] = 1;
			}
			else
			{
				int selected_index_in_pop2 = -1;
				int selected_index = -1;
				if (rho[selected_j] == 0)
				{
					selected_index_in_pop2 = I[0];
					selected_index = pop2_indices[I[0]];
					double min_distance = distance[selected_index];
					for (int i = 1; i < I.size(); i++)
					{
						int current_index = pop2_indices[I[i]];
						if (min_distance > distance[current_index])
						{
							min_distance = distance[current_index];
							selected_index = current_index;
							selected_index_in_pop2 = I[i];
						}
					}
				}
				else
				{ 
					int random_int = rnd(0, I.size() - 1);
					selected_index_in_pop2 = I[random_int];
					selected_index = pop2_indices[I[random_int]];
				}

				rho[selected_j]++;
				remain--;
				selected_indices.push_back(selected_index);
				pop2_choosed[selected_index_in_pop2] = 1;
			}
		}

		int count = 0;
		for (int i = 0; i < pop1_indices.size(); i++)
			CopyIndividual(ndpop_[pop1_indices[i]], parent_pop[count++]);
		for (int i = 0; i < selected_indices.size(); i++)
			CopyIndividual(ndpop_[selected_indices[i]], parent_pop[count++]);

		for (int i = 0; i < ndpop_num; i++)
			delete[] normalized_pop[i];
		delete[] normalized_pop;
	}

	void NSGA3::GetNdPop(Individual** mixed_pop, int mixpop_num, Individual** ndpop, int& ndpop_num)
	{
		int  current_pop_num = 0, temp_number = 0, rank_index = 0;
		NonDominatedSort(mixed_pop, mixpop_num, g_GlobalSettings->obj_num_);

		while (1)
		{
			temp_number = 0;
			for (int i = 0; i < mixpop_num; i++)
			{
				if (mixed_pop[i]->rank_ == rank_index)
				{
					temp_number++;
				}
			}
			if (current_pop_num + temp_number < weight_num_)
			{
				current_pop_num += temp_number;
				rank_index++;
			}
			else
				break;
		}

		ndpop_num = current_pop_num + temp_number;

		int index = 0;
		for (int i = 0; i < mixpop_num; i++)
		{
			if (mixed_pop[i]->rank_ <= rank_index)
			{
				CopyIndividual(mixed_pop[i], ndpop[index++]);
			}
		}
	}

	void NSGA3::GetExtremePop(Individual** ndpop, int ndpop_num, Individual** extreme_pop)
	{
		std::vector<double> max_value(ndpop_num);
		std::vector<std::vector<double>> weight_vec(g_GlobalSettings->obj_num_, std::vector<double>(g_GlobalSettings->obj_num_));

		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
		{
			for (int j = 0; j < g_GlobalSettings->obj_num_; j++)
			{
				if (i == j)
					weight_vec[i][j] = 1;
				else
					weight_vec[i][j] = 1e-6;
			}
		}

		// calculate the ASF
		int min_idx = 0;
		double temp_ASF = 0, temp_max_value = 0, min_value = 0;
		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
		{
			for (int j = 0; j < ndpop_num; j++)
			{
				temp_max_value = 0;
				for (int k = 0; k < g_GlobalSettings->obj_num_; k++)
				{
					temp_ASF = (ndpop[j]->obj_[k] - ideal_point_[k]) / weight_vec[i][k];
					if (temp_ASF > temp_max_value)
					{
						temp_max_value = temp_ASF;
					}
				}
				max_value[j] = temp_max_value;
			}

			min_idx = 0;
			min_value = max_value[0];
			for (int j = 1; j < ndpop_num; j++)
			{
				if (max_value[j] < min_value)
				{
					min_value = max_value[j];
					min_idx = j;
				}
			}

			CopyIndividual(ndpop[min_idx], extreme_pop[i]);
		}
	}

	void NSGA3::GetIntercepts(Individual** extreme_pop, Individual** ndpop, int ndpop_num, double* intercepts)
	{
		double** arg;
		arg = (double**)malloc(sizeof(double*) * g_GlobalSettings->obj_num_);
		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
			arg[i] = (double*)malloc(sizeof(double) * g_GlobalSettings->obj_num_);

		std::vector<double> max_obj_value(g_GlobalSettings->obj_num_);
		std::vector<double> u(g_GlobalSettings->obj_num_);

		// initialize 
		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
			max_obj_value[i] = -EMOC_INF;

		/* traverse all the individuals of the population and get their maximum value of objective (The simplest way of
		 * calculating the nadir point is to get these maximum values among the first front individuals) */
		for (int i = 0; i < ndpop_num; i++)
		{
			for (int j = 0; j < g_GlobalSettings->obj_num_; j++)
			{
				if (max_obj_value[j] < ndpop[i]->obj_[j])
					max_obj_value[j] = ndpop[i]->obj_[j];
			}
		}

		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
			u[i] = 1;

		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
			for (int j = 0; j < g_GlobalSettings->obj_num_; j++)
				arg[i][j] = extreme_pop[i]->obj_[j] - ideal_point_[j];

		if (GaussianElimination(arg, u.data(), intercepts, g_GlobalSettings->obj_num_) == nullptr)
		{
			for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
				intercepts[i] = max_obj_value[i];
		}
		else
		{
			for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
				intercepts[i] = 1 / intercepts[i] + ideal_point_[i];
		}

		/* If any of the intercepts is still Zero (which means that one of the nadir values is Zero), then use the maximum
		 * value of each objective instead (remember that these values were calculated among all the individuals, not just
		 * the first-front individuals) */
		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
		{
			if (intercepts[i] < EMOC_EPS)
			{
				for (int j = 0; j < g_GlobalSettings->obj_num_; j++)
					intercepts[j] = max_obj_value[j];
				break;
			}
		}

		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
			free(arg[i]);
		free(arg);
	}

	void NSGA3::Association(double** pop, int pop_num, int* pi, double* distance)
	{
		for (int i = 0; i < pop_num; i++)
		{
			int min_index = 0;
			double min = CalPerpendicularDistance(pop[i], lambda_[0], g_GlobalSettings->obj_num_);
			for (int j = 1; j < weight_num_; j++)
			{
				double current_distance = CalPerpendicularDistance(pop[i], lambda_[j], g_GlobalSettings->obj_num_);
				if (min > current_distance)
				{
					min = current_distance;
					min_index = j;
				}
			}

			pi[i] = min_index;
			distance[i] = min;
		}
	}
}