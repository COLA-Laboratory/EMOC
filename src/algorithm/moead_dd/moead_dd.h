#pragma once
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"

#include <vector>
#include <unordered_map>

namespace emoc {

	class MOEADD : public Algorithm
	{
	public:
		typedef struct
		{
			int index;
			double distance;
		}DistanceInfo;  // store euclidian distance to the index-th weight vector

		MOEADD(int thread_id);
		virtual ~MOEADD();

		void Solve();

	private:
		void Initialization();
		void Association(Individual **pop, int pop_num);
		void SetNeighbours();
		void Crossover(Individual** parent_pop, int current_index, Individual* offspring);

		void EnvironmentSelection(Individual** mixed_pop, int mixedpop_num, Individual** parent_pop);
		int LocateWorst(Individual** mixed_pop, int mixedpop_num, const std::vector<std::vector<int>> &front_index);


	private:
		double** lambda_;                  // weight vector
		int weight_num_;                   // the number of weight vector
		int** neighbour_;	               // neighbours of each individual
		int neighbour_num_;                // the number of neighbours
		double neighbour_selectpro_;       // the probability of select neighbour scope

		// MOEADD parameters
		std::vector<std::vector<int>> cluster_;
		std::unordered_map<int,int> subregion_map_;

		double* ideal_point_;
	};

}