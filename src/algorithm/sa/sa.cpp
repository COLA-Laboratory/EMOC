#include "algorithm/sa/sa.h"

#include <vector>
#include <algorithm>
#include <iostream>

#include "cmath"
#include "core/macro.h"
#include "core/global.h"
#include "core/utility.h"
#include "random/random.h"

namespace emoc {

	SA::SA(int thread_id) :Algorithm(thread_id)
	{
	}

	SA::~SA()
	{

	}

	void SA::Solve()
	{
		Initialization();
		while (!IsTermination())
		{
			for (int i = 0; i < g_GlobalSettings->population_num_; i++)
			{
				// generate offspring population
				Mutation(g_GlobalSettings->parent_population_[0], g_GlobalSettings->offspring_population_[0]);
				EvaluateInd(g_GlobalSettings->offspring_population_[0]);

				if (randomperc() < CalAnnealProb(g_GlobalSettings->parent_population_[0]->obj_[0], g_GlobalSettings->offspring_population_[0]->obj_[0]))
					CopyIndividual(g_GlobalSettings->offspring_population_[0], g_GlobalSettings->parent_population_[0]);

				T_ = 0.99 * T_;
				for (auto& v : sigma_)
					v = v * 0.99;
			}
		}
	}

	void SA::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), 1);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), 1);

		// simulate annealing parameters setting
		T_ = 0.1;
		sigma_ = std::vector<double>(g_GlobalSettings->dec_num_);
		for (int i = 0; i < g_GlobalSettings->dec_num_; i++)
			sigma_[i] = 0.2 * (g_GlobalSettings->dec_upper_bound_[i] - g_GlobalSettings->dec_lower_bound_[i]);
	}

	
	void SA::Mutation(Individual* parent, Individual* offspring)
	{
		for (int i = 0; i < g_GlobalSettings->dec_num_; i++)
		{
			if (randomperc() < 0.5)
			{
				offspring->dec_[i] = parent->dec_[i] + sigma_[i] * GaussianRandom(0, 1);

				// repair
				if (offspring->dec_[i] > g_GlobalSettings->dec_upper_bound_[i] || offspring->dec_[i] < g_GlobalSettings->dec_lower_bound_[i])
					offspring->dec_[i] = rndreal(g_GlobalSettings->dec_lower_bound_[i], g_GlobalSettings->dec_upper_bound_[i]);
			}
			else
				offspring->dec_[i] = parent->dec_[i];
		}
	}

	double SA::CalAnnealProb(double parent_fitness, double offspring_fitness)
	{
		double diff = (offspring_fitness - parent_fitness) / (fabs(parent_fitness) + 1e-6);
		return exp(-diff / T_);
	}

}