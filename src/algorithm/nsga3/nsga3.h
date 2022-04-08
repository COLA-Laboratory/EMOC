#pragma once
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"

#include <vector>

namespace emoc {

	class NSGA3 : public Algorithm
	{
	public:
		NSGA3(int thread_id);
		virtual ~NSGA3();
		
		void Solve();

	private:
		void Initialization();
		void Crossover(Individual **parent_pop, Individual **offspring_pop);

		// do nsga3's environment selection on mixed_pop, the result is stored in parent_pop
		void EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop, int mixpop_num);

		void GetNdPop(Individual** mixed_pop, int mixpop_num, Individual** ndpop, int& ndpop_num);
		void GetExtremePop(Individual** ndpop, int ndpop_num, Individual** extreme_pop);
		void GetIntercepts(Individual** extreme_pop, Individual** ndpop, int ndpop_num, double* intercepts);
		void Association(double** pop, int pop_num, int *pi, double *distance);

	private:
		double** lambda_;                  // weight vector
		int weight_num_;                   // the number of weight vector
		double* ideal_point_;
		std::vector<Individual*> ndpop_;
		std::vector<Individual*> extreme_pop_;
	};

}