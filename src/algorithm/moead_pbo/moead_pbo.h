#pragma once
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"
#include <Python.h>

#include <vector>

namespace emoc {

	class MOEADPBO : public Algorithm
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

		MOEADPBO(int thread_id);
		virtual ~MOEADPBO();

		void Solve();

	private:
		void Initialization();
		void SetNeighbours();
		void Crossover(Individual **parent_pop, int current_index, Individual *offspring);
		// use offspring to update the neighbour of current_index-th individual with specified aggregation function
		//void UpdateSubproblem(Individual *offspring, int current_index, int aggregation_type);
		void UpdateSubproblem(Individual *offspring, int current_index,int aggregration_type);
		void NearNeighbours(double **seed,Individual **pop,int miu,int popsize);
		//void rbf_first_train(double **a,double *rbf,double *weights_rbf,double sigma,int miu);
		//double rbf_test(Individual *individual,double *weights_rbf,double **a,double sigma);
		
		void update_neighborhood(double *weight,int miu,int number,double eta);
		//double **calculatePinv(double **n,int row,int col);
		void init_reference();
		void Consult_DM();
		void RecordCurrentPop(PyObject *pop);
		void UpdateNeighbour();
		void SetBiasedWeight(double *best);
		void RecordBest(double *best);
		// void CalculateMinDis(double *best);
		// void CalculateExpectDis(double *best);
	private:
		//MOEAD parameters
		double **lambda_;                  // weight vector
		int weight_num_;                   // the number of weight vector
		int **neighbour_;	               // neighbours of each individual
		int neighbour_num_;                // the number of neighbours
		double *ideal_point_;
		int aggregation_type_;
		double pbi_theta_;

		int first_tau_;
		int tau_; //consultation interval
		//ppl parameters
		//double *weights_obj;
		// int *permutation;
		NeighbourType neighbour_type_;
		int subproblem_id;
		double neighbour_selectpro_; // the probability of select neighbour scope
		double* weight;				// weight for DM


		/* common paramters */
    	int miu;                                //number of incumbent candidates.
    	int tau;                                // number of generations between two consecutive consultation.
    	int first_tau;                          //  number of generations in first consecutive consultation.
    	double eta;                             //step size.
    	double sigma;                            // the width of the Gaussian function.
		
		//param for pairwise preference learning
		int replace_num_;  
		int nPromisingWeight;	//num of promising weights
		double step_size_;		//change the weight to what extent
	};

}