#pragma once
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"

#include <vector>

namespace emoc {

	class IBEA : public Algorithm
	{
	public:
		IBEA(int thread_id);
		virtual ~IBEA();

		void Solve();

	private:
		void Initialization();
		void Crossover(Individual **parent_pop, Individual **offspring_pop);

		double CalEpsIndicator(Individual *ind1, Individual *ind2);
		void CalFitness(Individual **pop, int pop_num, std::vector<double>& fitness);
		void EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop);

	private:
		double kappa;
	};

}