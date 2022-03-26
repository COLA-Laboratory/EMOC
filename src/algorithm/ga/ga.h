#pragma once
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"

#include <vector>

namespace emoc {

	class GA : public Algorithm
	{
	public:
		GA(int thread_id);
		virtual ~GA();
		
		void Run();

	private:
		void Initialization();
		void Crossover(Individual **parent_pop, Individual **offspring_pop);
		void EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop);
	};

}