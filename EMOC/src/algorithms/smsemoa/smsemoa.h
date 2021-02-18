#pragma once

#include "algorithms/algorithm.h"
namespace emoc {

	class SMSEMOA : public Algorithm
	{
	public:
		SMSEMOA(Problem *problem);
		virtual ~SMSEMOA();

		void Run();

	private:
		void Initialization();
		void Crossover(Individual **parent_pop, Individual *offspring);
		int FindMinVolumeIndex(Individual **pop, int pop_num);
		void EnvironmentalSelection(Individual **parent_pop, Individual *offspring);

	private:
		double *nadir_point_;
	};

}