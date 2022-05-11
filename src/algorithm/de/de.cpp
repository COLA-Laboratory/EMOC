#include "algorithm/de/de.h"

#include <vector>
#include <algorithm>
#include <iostream>

#include "core/macro.h"
#include "core/global.h"
#include "operator/tournament_selection.h"
#include "operator/polynomial_mutation.h"
#include "operator/de.h"
#include "random/random.h"

namespace emoc {

	DifferentialEvolution::DifferentialEvolution(int thread_id) :Algorithm(thread_id)
	{
		real_popnum_ = g_GlobalSettings->population_num_;
		g_GlobalSettings->de_parameter_.crossover_pro = 0.9;
		g_GlobalSettings->de_parameter_.F = 0.5;
	}

	DifferentialEvolution::~DifferentialEvolution()
	{

	}

	void DifferentialEvolution::Solve()
	{
		Initialization();
		while (!IsTermination())
		{
			// generate offspring population
			Crossover(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->offspring_population_.data());
			PolynomialMutation(g_GlobalSettings->offspring_population_.data(), real_popnum_, g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, mutation_para_);
			EvaluatePop(g_GlobalSettings->offspring_population_.data(), real_popnum_);

			// select next generation's population
			EnvironmentalSelection(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->offspring_population_.data());
		}
	}

	void DifferentialEvolution::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 0.5;
	}

	void DifferentialEvolution::Crossover(Individual **parent_pop, Individual **offspring_pop)
	{
		// generate random permutation index for tournment selection
		std::vector<int> index1(g_GlobalSettings->population_num_);
		std::vector<int> index2(g_GlobalSettings->population_num_);
		std::vector<int> index3(g_GlobalSettings->population_num_);
		std::vector<int> index4(g_GlobalSettings->population_num_);
		random_permutation(index1.data(), g_GlobalSettings->population_num_);
		random_permutation(index2.data(), g_GlobalSettings->population_num_);
		random_permutation(index3.data(), g_GlobalSettings->population_num_);
		random_permutation(index4.data(), g_GlobalSettings->population_num_);

		for (int i = 0; i < g_GlobalSettings->population_num_; ++i)
		{
			Individual* parent1 = TournamentByCustom(parent_pop[index1[i]], parent_pop[index1[i]]->obj_[0],
				parent_pop[index2[i]], parent_pop[index2[i]]->obj_[0]);
			Individual* parent2 = TournamentByCustom(parent_pop[index3[i]], parent_pop[index3[i]]->obj_[0],
				parent_pop[index4[i]], parent_pop[index4[i]]->obj_[0]);
			DE(parent_pop[i], parent1, parent2, offspring_pop[i], g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, cross_para_);
		}
	}
	
	void DifferentialEvolution::EnvironmentalSelection(Individual** parent_pop, Individual** offspring_pop)
	{
		double best_value = parent_pop[0]->obj_[0];
		int best_index = 0;

		// copy individuals
		for (int i = 0; i < g_GlobalSettings->population_num_; i++)
		{
			if (parent_pop[i]->obj_[0] > offspring_pop[i]->obj_[0])
				CopyIndividual(offspring_pop[i], parent_pop[i]);

			if (parent_pop[i]->obj_[0] < best_value)
				best_index = i;
		}

		// swap the best individual to the first position
		Individual* temp = parent_pop[0];
		parent_pop[0] = parent_pop[best_index];
		parent_pop[best_index] = temp;
	}

}