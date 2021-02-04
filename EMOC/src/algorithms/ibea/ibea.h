#pragma once
#include "core/individual.h"
#include "algorithms/algorithm.h"
#include "problem/problem.h"

namespace emoc {

	class IBEA : public Algorithm
	{
	public:
		IBEA(Problem *problem);
		virtual ~IBEA();

		void Run();

	private:
		void Initialization();
		void Crossover(Individual **parent_pop, Individual **offspring_pop);

		double CalEpsIndicator(Individual *ind1, Individual *ind2);
		void CalFitness(Individual **mixed_pop, int mixed_popsize, double *fitness);
		void EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop);

	private:
		double kappa;
	};

}