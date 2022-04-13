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
		
		void Solve();
		 
	private:
		void Initialization();
		void Crossover(Individual** parent_pop, Individual** offspring_pop);
		void Mutation(Individual** pop, int pop_num);
		void EnvironmentalSelection(Individual** parent_pop, Individual** mixed_pop);
		double CalFitness(Individual* ind);
	};

}