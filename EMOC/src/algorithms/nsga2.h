#pragma once
#include <vector>

#include "core/individual.h"
#include "algorithms/algorithm.h"
#include "problem/problem.hpp"

namespace emoc {

	class NSGA2 : public Algorithm
	{
	public:
		typedef struct
		{
			int index;
			double distance;
		}DistanceInfo;

		NSGA2(Problem *problem);
		~NSGA2();

		void Run();

	private:
		void Initialization();
		void Crossover(Individual **parent_pop, Individual **offspring_pop);

		void SetDistanceInfo(std::vector<DistanceInfo> &distanceinfo_vec, int target_index, double distance);
		int CrowdingDistance(Individual **mixed_pop, int pop_num, int *pop_sort, int rank_index);
		void EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop);

	};

}