#include "algorithm/ga/ga.h"

#include <vector>
#include <algorithm>

#include "core/macro.h"
#include "core/global.h"
#include "core/nd_sort.h"
#include "core/tournament_selection.h"
#include "operator/mutation.h"
#include "operator/sbx.h"
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
			PolynomialMutation(g_GlobalSettings->offspring_population_.data(), 2 * (real_popnum_ / 2), g_GlobalSettings);
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
			Individual *parent1 = TournamentByCustom(parent_pop[index1[2 * i]], parent_pop[index1[2 * i]]->obj_[0],
				parent_pop[index1[2 * i + 1]], parent_pop[index1[2 * i + 1]]->obj_[0]);
			Individual *parent2 = TournamentByCustom(parent_pop[index2[2 * i]], parent_pop[index2[2 * i]]->obj_[0],
				parent_pop[index2[2 * i + 1]], parent_pop[index2[2 * i + 1]]->obj_[0]);
			SBX(parent1, parent2, offspring_pop[2 * i], offspring_pop[2 * i + 1], g_GlobalSettings);
		}
	}
	
	void GA::EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop)
	{
		int mixed_num = real_popnum_ + 2 * (real_popnum_ / 2);
		std::vector<int> sorted_index(mixed_num);
		for (int i = 0; i < mixed_num; i++)
			sorted_index[i] = i;

		std::sort(sorted_index.begin(), sorted_index.end(), [&](int& left, int& right) {
			return mixed_pop[left]->obj_[0] < mixed_pop[right]->obj_[0];
			});

		// copy individuals
		for (int i = 0; i < g_GlobalSettings->population_num_; i++)
			CopyIndividual(mixed_pop[sorted_index[i]], parent_pop[i]);
	}

}