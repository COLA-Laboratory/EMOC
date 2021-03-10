#pragma once
#include "core/individual.h"
#include "algorithms/algorithm.h"
#include "problem/problem.h"

namespace emoc {

	class IBEA : public Algorithm
	{
	public:
		IBEA(Problem *problem, int thread_num);
		virtual ~IBEA();

		void Run();

	private:
		void Initialization();
		void Crossover(Individual **parent_pop, Individual **offspring_pop);

		double CalEpsIndicator(Individual *ind1, Individual *ind2);
		void CalFitness(Individual **pop, int pop_num, double *fitness);
		void EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop);

	private:
		double kappa;
	};

}