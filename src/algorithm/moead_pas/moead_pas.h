#pragma once
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"

#include <vector>

namespace emoc {

	class MOEADPAS : public Algorithm
	{
	public:
		typedef struct
		{
			int index;
			double distance;
		}DistanceInfo;  // store euclidian distance to the index-th weight vector

		typedef enum
		{
			NEIGHBOUR,
			GLOBAL
		}NeighbourType;

		MOEADPAS(int thread_id);
		virtual ~MOEADPAS();

		void Solve();

	private:
		void Initialization();
		void SetNeighbours();
		void Crossover(Individual** parent_pop, int current_index, Individual* offspring);

		// use offspring to update the neighbour of current_index-th individual with specified aggregation function
		void UpdateSubproblem(Individual* offspring, int current_index);
		// update the p parameter of aggregation function for each subproblem
		void UpdatePiArray(Individual** parent_pop, int* Pi);


	private:
		double** lambda_;                  // weight vector
		int weight_num_;                   // the number of weight vector
		int** neighbour_;	               // neighbours of each individual
		int neighbour_num_;                // the number of neighbours
		int replace_num_;                  // the number of maximum replaced individual
		double neighbour_selectpro_;       // the probability of select neighbour scope
		NeighbourType neighbour_type_;
		double* ideal_point_;
		double* nadir_point_;

		// MOEADPAS parameters
		int* Pi;
		std::vector<int> candidate_p_ = { 1,2,3,4,5,6,7,8,9,10,-1};   // '-1' represent for infinite
		
	};

}