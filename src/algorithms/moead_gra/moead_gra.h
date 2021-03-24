#pragma once
#include "algorithms/algorithm.h"
#include "core/individual.h"
#include "problem/problem.h"

namespace emoc {

	class MOEADGRA : public Algorithm
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

		MOEADGRA(Problem *problem, int thread_num);
		virtual ~MOEADGRA();

		void Run();

	private:
		void Initialization();
		void SetNeighbours();
		void Crossover(Individual **parent_pop, int current_index, Individual *offspring);

		// use offspring to update the neighbour of current_index-th individual with specified aggregation function
		void UpdateSubproblem(Individual *offspring, int current_index);

		void CalculateFitness(Individual **pop, int pop_num, double *fitness);
		void UpdateProbability();

	private:
		double **lambda_;                  // weight vector
		int weight_num_;                   // the number of weight vector
		int **neighbour_;	               // neighbours of each individual
		int neighbour_num_;                // the number of neighbours
		double neighbour_selectpro_;       // the probability of select neighbour scope
		NeighbourType neighbour_type_;
		double *ideal_point_;

		double **fitness_history_;		   // fitness of each subproblem for recent generations
		double *delta_;					   // difference between new and old individuals' fitness	
		double *P_;						   // selection probability for each subproblem
	};
}