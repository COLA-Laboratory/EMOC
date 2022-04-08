#pragma once
#include "algorithm/algorithm.h"
#include "core/individual.h"
#include "problem/problem.h"

#include <vector>

namespace emoc {

	class MOEADSTM : public Algorithm
	{
	public:
		typedef struct
		{
			int index;
			double distance;
		}DistanceInfo;  // store euclidian distance to the index-th weight vector

		typedef struct
		{
			int index;
			double fit;
		}FitnessInfo;

		typedef enum
		{
			NEIGHBOUR,
			GLOBAL
		}NeighbourType;

		MOEADSTM(int thread_id);
		virtual ~MOEADSTM();

		void Solve();

	private:
		void Initialization();
		void SetNeighbours();
		void Crossover(Individual** parent_pop, int current_index, Individual* offspring);

		// use offspring to update the neighbour of current_index-th individual with specified aggregation function
		void CalculatePref(Individual** mixed_pop);
		void StableMatching(Individual **parent_pop, Individual** mixed_pop);

		void UpdateUtility();
		void SelectCurrentSubproblem();
		void CalculateFitness(Individual** pop, int pop_num, double* fitness);

	private:
		// MOEAD basic parameters
		double** lambda_;                  // weight vector
		int weight_num_;                   // the number of weight vector
		int** neighbour_;	               // neighbours of each individual
		int neighbour_num_;                // the number of neighbours
		double neighbour_selectpro_;       // the probability of select neighbour scope
		NeighbourType neighbour_type_;
		double* ideal_point_;
		double* nadir_point_;

		// MOEADDRA parameters
		int* selected_indices_;			   // the indices of selected individuals of current iteration
		int selected_size_;				   // the number of selected individuals
		double* old_obj_;				   // old Tchebycheff function value of each solution on its subproblem
		double* utility_;				   // utility for each subproblem
		double* delta_;					   // difference between new and old individuals' fitness			

		// MOEADSTM parameters
		double** dist_matrix_;						// preference of solution to subproblem
		FitnessInfo** solution_matrix_;			    // preference of solution to subproblem with ranked index info
		FitnessInfo** subproblem_matrix_;			// preference of subproblem to solution with ranked index info
		
	};
}