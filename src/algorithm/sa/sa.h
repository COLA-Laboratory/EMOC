#pragma once
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"

#include <vector>

namespace emoc {

	class SA : public Algorithm
	{
	public:
		SA(int thread_id);
		virtual ~SA();
		
		void Run();

	private:
		void Initialization();
		void Crossover(Individual **parent_pop, Individual **offspring_pop);
		void EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop);
	};

}