#pragma once
#include "core/individual.h"
#include "algorithms/algorithm.h"
#include "problem/problem.h"

namespace emoc {

	class MOEADDE : public Algorithm
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

		MOEADDE(Problem *problem, int thread_num);
		virtual ~MOEADDE();

		void Run();

	private:
		void Initialization();
		void SetNeighbours();
		void Crossover(Individual **parent_pop, int current_index, Individual *offspring);

		// use offspring to update the neighbour of current_index-th individual with specified aggregation function
		void UpdateSubproblem(Individual *offspring, int current_index);


	private:
		double **lambda_;                  // weight vector
		int weight_num_;                   // the number of weight vector
		int **neighbour_;	               // neighbours of each individual
		int neighbour_num_;                // the number of neighbours
		int replace_num_;                   // the number of maximum replaced individual
		double neighbour_selectpro_;       // the probability of select neighbour scope
		NeighbourType neighbour_type_;
		double *ideal_point_;
	};

}