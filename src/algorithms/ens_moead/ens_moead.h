#pragma once
#include "algorithms/algorithm.h"
#include "core/individual.h"
#include "problem/problem.h"

namespace emoc {

	class ENSMOEAD : public Algorithm
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

		ENSMOEAD(Problem *problem, int thread_num);
		virtual ~ENSMOEAD();

		void Run();

	private:
		void Initialization();
		void SetNeighbours();
		void Crossover(Individual **parent_pop, int current_index, Individual *offspring);
		// use offspring to update the neighbour of current_index-th individual with specified aggregation function
		int UpdateSubproblem(Individual *offspring, int current_index);

		void UpdateUtility();
		void SelectCurrentSubproblem();
		void CalculateFitness(Individual **pop, int pop_num, double *fitness);

		int SelectNS();
		void UpdateProbability();

	private:
		double **lambda_;                  // weight vector
		int weight_num_;                   // the number of weight vector
		int **neighbour_;	               // neighbours of each individual
		int neighbour_num_;                // the number of neighbours
		int replace_num_;                  // the number of maximum replaced individual
		double neighbour_selectpro_;       // the probability of select neighbour scope
		NeighbourType neighbour_type_;
		double *ideal_point_;

		int *selected_indices_;			   // the indices of selected individuals of current iteration
		int selected_size_;				   // the number of selected individuals
		double *old_obj_;				   // old Tchebycheff function value of each solution on its subproblem
		double *utility_;				   // utility for each subproblem
		double *delta_;					   // difference between new and old individuals' fitness	

		// parameters for ENSMOEAD
		double FEs_[4];
		double FEs_success_[4];
		double R_[4];						    // reward of each neighborhood size
		double P_[4];							// probability of choosing each neighborhood size
		int NS_[4];                           // different neighborhood size
		int LP_;

	};

}