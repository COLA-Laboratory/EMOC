#include "algorithm/ga/ga.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

#include "core/macro.h"
#include "core/global.h"
#include "core/nd_sort.h"
#include "operator/tournament_selection.h"
#include "core/emoc_utility_structures.h"
#include "operator/polynomial_mutation.h"
#include "operator/swap_mutation.h"
#include "operator/bit_mutation.h"
#include "operator/sbx.h"
#include "operator/uniform_crossover.h"
#include "operator/order_crossover.h"
#include "random/random.h"

namespace emoc {

	GA::GA(int thread_id) :Algorithm(thread_id)
	{
		real_popnum_ = g_GlobalSettings->population_num_;
	}

	GA::~GA()
	{

	}

	void GA::Solve()
	{
		Initialization();
		while (!IsTermination())
		{
			// generate offspring population
			Crossover(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->offspring_population_.data());
			Mutation(g_GlobalSettings->offspring_population_.data(), 2 * (real_popnum_ / 2));
			EvaluatePop(g_GlobalSettings->offspring_population_.data(), 2 * (real_popnum_ / 2));
			MergePopulation(g_GlobalSettings->parent_population_.data(), real_popnum_, g_GlobalSettings->offspring_population_.data(),
				2 * (real_popnum_ / 2), g_GlobalSettings->mixed_population_.data());
			
			// select next generation's population
			EnvironmentalSelection(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->mixed_population_.data());
		}
	}

	void GA::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 20.0;
	}

	void GA::Crossover(Individual **parent_pop, Individual **offspring_pop)
	{
		// generate random permutation index for tournment selection
		std::vector<int> index1(g_GlobalSettings->population_num_);
		std::vector<int> index2(g_GlobalSettings->population_num_);
		random_permutation(index1.data(), g_GlobalSettings->population_num_);
		random_permutation(index2.data(), g_GlobalSettings->population_num_);

		for (int i = 0; i < g_GlobalSettings->population_num_ / 2; ++i)
		{
			double fitness1 = CalFitness(parent_pop[index1[2 * i]]);
			double fitness2 = CalFitness(parent_pop[index1[2 * i + 1]]);
			Individual *parent1 = TournamentByCustom(parent_pop[index1[2 * i]], fitness1,
				parent_pop[index1[2 * i + 1]], fitness2);

			fitness1 = CalFitness(parent_pop[index2[2 * i]]);
			fitness2 = CalFitness(parent_pop[index2[2 * i + 1]]);
			Individual *parent2 = TournamentByCustom(parent_pop[index2[2 * i]], fitness1,
				parent_pop[index2[2 * i + 1]], fitness2);

			if (g_GlobalSettings->problem_->encoding_ == Problem::REAL)
			{
				SBX(parent1, parent2, offspring_pop[2 * i], offspring_pop[2 * i + 1],
					g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, cross_para_);
			}
			else if (g_GlobalSettings->problem_->encoding_ == Problem::BINARY)
			{
				UniformCrossover(parent1, parent2, offspring_pop[2 * i], offspring_pop[2 * i + 1]);
			}
			else if (g_GlobalSettings->problem_->encoding_ == Problem::PERMUTATION)
			{
				OrderCrossover(parent1, parent2, offspring_pop[2 * i], offspring_pop[2 * i + 1]);
			}
		}
	}
	
	void GA::Mutation(Individual** pop, int pop_num)
	{
		for (int i = 0; i < pop_num; i++)
		{
			if (g_GlobalSettings->problem_->encoding_ == Problem::REAL)
			{
				mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
				mutation_para_.index1 = 20.0;
				PolynomialMutation(pop[i], g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, mutation_para_);
			}
			else if (g_GlobalSettings->problem_->encoding_ == Problem::BINARY)
			{
				mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
				BitFlipMutation(pop[i], mutation_para_);
			}
			else if (g_GlobalSettings->problem_->encoding_ == Problem::PERMUTATION)
			{
				mutation_para_.pro = 1.0 / g_GlobalSettings->population_num_;
				SwapMutation(pop[i], mutation_para_);
			}
		}
	}

	void GA::EnvironmentalSelection(Individual** parent_pop, Individual** mixed_pop)
	{
		int mixed_num = real_popnum_ + 2 * (real_popnum_ / 2);
		std::vector<int> sorted_index(mixed_num);
		for (int i = 0; i < mixed_num; i++)
			sorted_index[i] = i;

		for (int i = 0; i < mixed_num; i++)
			mixed_pop[i]->fitness_ = CalFitness(mixed_pop[i]);

		std::sort(sorted_index.begin(), sorted_index.end(), [&](int& left, int& right) {
			return mixed_pop[left]->fitness_ < mixed_pop[right]->fitness_;
			});

		// copy individuals
		for (int i = 0; i < g_GlobalSettings->population_num_; i++)
			CopyIndividual(mixed_pop[sorted_index[i]], parent_pop[i]);
	}

	double GA::CalFitness(Individual* ind)
	{
		double con = 0.0;
		for (int j = 0; j < ind->con_.size(); j++)
			con += std::max(0.0, ind->con_[j]);
		return ind->obj_[0] + (con > 0 ? (1e10+con) : 0.0);
	}

}