#pragma once
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"

#include <vector>

namespace emoc {

	class CNSGA2 : public Algorithm
	{
	public:
		typedef struct
		{
			int index;
			double distance;
		}DistanceInfo;  // store crowding distance of index-th individual

		CNSGA2(int thread_id);
		virtual ~CNSGA2();
		
		void Solve();

	private:
		void Initialization();
		void Crossover(Individual **parent_pop, Individual **offspring_pop);

		// set the crowding distance of given individual index
		void SetDistanceInfo(std::vector<DistanceInfo> &distanceinfo_vec, int target_index, double distance);

		// use crowding distance to sort the individuals with rank rank_index, the result is stored in pop_sort
		// return the number of indviduals of rank rank_index
		int CrowdingDistance(Individual **mixed_pop, int pop_num, int *pop_sort, int rank_index);

		// do nsga2's environment selection on mixed_pop, the result is stored in parent_pop
		void EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop);
	};

}