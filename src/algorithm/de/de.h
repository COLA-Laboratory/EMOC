#pragma once
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"

#include <vector>

namespace emoc {

	class DifferentialEvolution : public Algorithm
	{
	public:
		DifferentialEvolution(int thread_id);
		virtual ~DifferentialEvolution();
		
		void Solve();

	private:
		void Initialization();
		void Crossover(Individual **parent_pop, Individual **offspring_pop);
		void EnvironmentalSelection(Individual** parent_pop, Individual** offspring_pop);
	};

}