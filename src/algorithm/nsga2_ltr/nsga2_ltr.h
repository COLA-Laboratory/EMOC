#pragma once
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"
#include <queue>
#include <numeric>
#include <vector>

namespace emoc {

	class NSGA2_LTR : public Algorithm
	{
	public:
		typedef struct
		{
			int index;
			double distance;
		}DistanceInfo;  // store crowding distance of index-th individual

        typedef struct 
        {
            /* data */
            int index;
            double value;
        }SortList;
        

		NSGA2_LTR(int thread_id);
		virtual ~NSGA2_LTR();
		
		void Solve();

	private:
		void Initialization();
		void Crossover(Individual **parent_pop, Individual **offspring_pop);

		// set the crowding distance of given individual index
		void SetDistanceInfo(std::vector<DistanceInfo> &distanceinfo_vec, int target_index, double distance);
        void SetPreferenceInfo(std::vector<SortList> &preferenceInfo_vec, int target_index, double distance);

		// use crowding distance to sort the individuals with rank rank_index, the result is stored in pop_sort
		// return the number of indviduals of rank rank_index
		int CrowdingDistance(Individual **mixed_pop, int pop_num, int *pop_sort, int rank_index);

        int ArbitrarilySelect(Individual **mixed_pop, int pop_num, int *pop_sort, int rank_index);

        int RankViaPreference(Individual **mixed_pop, int pop_num, int *pop_sort, int rank_index, int nneed);

		// do nsga2's environment selection on mixed_pop, the result is stored in parent_pop
		void EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop);
        bool EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop, bool usingPreferenceModel);
		// void CalculateTotalDis(double *best);
		// void CalculateMinDis(double *best);
		// void CalculateExpectDis(double *best);
		
        private:
        double *weight_;        // weight for DM
        int weight_num_;        // the number of weight vector
        int tau;                // consultation frequency
        int first_tau;          // first time to consult
        std::queue<double *> winners_;
        std::queue<double *> losers_;
	};

}