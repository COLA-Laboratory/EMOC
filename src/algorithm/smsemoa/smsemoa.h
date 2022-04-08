#pragma once
#include "algorithm/algorithm.h"
#include "core/individual.h"
#include "problem/problem.h"
#include "metric/hv.h"

#include <vector>

namespace emoc {

	class SMSEMOA : public Algorithm
	{
	public:
		SMSEMOA(int thread_id);
		virtual ~SMSEMOA();

		void Solve();

	private:
		void Initialization();
		void Crossover(Individual **parent_pop, Individual *offspring);
		int FindMinVolumeIndex(Individual **pop, int pop_num);
		void EnvironmentalSelection(Individual **parent_pop, Individual *offspring);

	private:
		double *nadir_point_;
		HVCalculator hv_calculator_;

	};

}