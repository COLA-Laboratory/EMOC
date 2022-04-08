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
		
		void Solve();

	private:
		void Initialization();
		void Mutation(Individual *parent, Individual *offspring);
		double CalAnnealProb(double parent_fitness, double offspring_fitness);

	private:
		double T_;
		std::vector<double> sigma_;
	};

}