#pragma once
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"

#include <vector>

namespace emoc {

	class MOEADM2M : public Algorithm
	{
	public:
		typedef struct
		{
			int index;
			double distance;
		}DistanceInfo;  

		MOEADM2M(int thread_id);
		virtual ~MOEADM2M();

		void Solve();

	private:
		void Initialization();
		void GenerateOffspring(Individual** parent_pop, Individual** offspring_pop);

		void AssociatePopulation(Individual** pop_src, int pop_num, Individual** pop_dest);
		void SetDistanceInfo(std::vector<DistanceInfo>& distanceinfo_vec, int target_index, double distance);
		int CrowdingDistance(Individual** pop, int pop_num, int* pop_sort, int rank_index);
	


	private:
		double** lambda_;                  // weight vector
		int weight_num_;                   // the number of weight vector

		// MOEADM2M parameters
		int K;							   // the number of different small multi-objective problems
		int S;                             // the number of solutions in each subregion


	};

}