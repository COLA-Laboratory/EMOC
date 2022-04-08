#pragma once
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"

#include <unordered_map>
#include <vector>

namespace emoc {

	class SPEA2 : public Algorithm
	{
	public:
		SPEA2(int thread_id);
		virtual ~SPEA2();

		void Solve();

	private:
		void Initialization();
		void Crossover(Individual **parent_pop, Individual **offspring_pop);

		void CalDistance(Individual **pop, int pop_num, double **distance, bool is_sort);
		void CalFitness(Individual **pop, int pop_num);
		std::unordered_map<int, int> TruncatePop(double **distance, int candidate_num);
		void EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop);

	private:
		int para_k;
	};

}