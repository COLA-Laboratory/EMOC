#pragma once
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"

#include <vector>

namespace emoc {

	class MOEADPLVF : public Algorithm
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

        typedef struct 
        {
            /* data */
            double value;
            int index;
        }SortList;
        

		MOEADPLVF(int thread_id);
		virtual ~MOEADPLVF();

		void Solve();

	private:
		void Initialization();
		void SetNeighbours();
		void Crossover(Individual **parent_pop, int current_index, Individual *offspring);
		void UpdateSubproblem(Individual *offspring, int current_index, int aggregration_type);
		void UpdateNeighbours();
        // void CalculateMinDis(double *best);

		void UsingRBFNet(Individual *ind, double **c, double *weight, double sigma, int size, int dimension);
		void TrainRBFNet(double **c, double *weight,double *out,  double sigma, int size, int dimension);
		// void CalculateTotalDis(double *best);
		// void CalculateExpectDis(double *best);

		// void NearNeighbours(double **seed,Individual **pop,int miu,int popsize);
		// void rbf_first_train(double **a,double *rbf,double *weights_rbf,double sigma,int miu);
		// double rbf_test(Individual *individual,double *weights_rbf,double **a,double sigma);
		
		// void update_neighborhood(double *weight,int miu,int number,double eta);
		// void rbf_train(double **a,double *rbf,double *weights_rbf,double sigma);
		// //double **calculatePinv(double **n,int row,int col);
		// void init_reference();
	private:
		//MOEAD parameters
		double **lambda_;                  // weight vector
		int weight_num_;                   // the number of weight vector
		int **neighbour_;	               // neighbours of each individual
		int neighbour_num_;                // the number of neighbours
		double *ideal_point_;
		int aggregation_type_;
		double pbi_theta_;

        double *weight;						// weight for the DM 
        double *goldenPoint_;
        
        double alpha;
        double alpha_DM;
        double **rbfnet_c;
        double *rbfnet_weight_;
        double sigma_;
        double *rbf_output_;
        int mu_;							// input size of rbfnet
        double stepSize_;

        NeighbourType neighbour_type_;
		double neighbour_selectpro_; // the probability of select neighbour scope
		int replace_num_;
		//plvf parameters
		//double *weights_obj;
		// int *permutation;
		// int subproblem_id;
		// double neighbour_selectpro_; // the probability of select neighbour scope
		// double *reference_point;
		// double *rbf;
    	// int *rbf_index;
    	// int best_index;

	};

}