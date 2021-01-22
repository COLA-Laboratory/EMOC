#include "algorithm/nsga2.h"

#include "core/global.h"
#include "operator/mutation.h"

namespace emoc {

	NSGA2::NSGA2(Problem *problem):Algorithm(problem)
	{

	}

	NSGA2::~NSGA2()
	{

	}

	void NSGA2::Run()
	{
		Initialization();
		while (!g_GlobalSettings->IsTermination())
		{
			Crossover(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->offspring_population_.data());
			MutationPop(g_GlobalSettings->offspring_population_.data(), 2 * g_GlobalSettings->population_num_ / 2);
			EvaluatePop(g_GlobalSettings->offspring_population_.data(), 2 * g_GlobalSettings->population_num_ / 2);
			MergePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->offspring_population_.data(), 
				2 * g_GlobalSettings->population_num_ / 2, g_GlobalSettings->mixed_population_.data());
			EnvironmentalSelection(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->mixed_population_.data());
		}
	}

	void NSGA2::Initialization()
	{
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
	}

	void NSGA2::Crossover(Individual **parent_pop, Individual **offspring_pop)
	{

	}
	
	void NSGA2::CrowdingDistance()
	{

	}

	void NSGA2::EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop)
	{

	}



}