#pragma once
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"

#include <vector>

namespace emoc {

	class IEMOD : public Algorithm
	{
	public:
        typedef struct
        {
            int index;
            double distance;
        }DistanceInfo;  // store euclidian distance to the index-th weight vector
        
		IEMOD(int thread_id);
		virtual ~IEMOD();

		void Solve();

	private:
		void Initialization();
        void InitializeNeighbours();
        void UpdateNeighbours();
        void Crossover(Individual **parent_pop, int current_index, Individual *offspring);


        void UpdateSubproblem(Individual *offspring, int current_index, int aggregation_type);
		// double CalEpsIndicator(Individual *ind1, Individual *ind2);
		// void CalFitness(Individual **pop, int pop_num, std::vector<double>& fitness);
		// void EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop);
        // void CalculateTotalDis(double *best);
        // void CalculateMinDis(double *best);
        // void CalculateExpectDis(double *best);

	private:
		// double kappa;
        double **lambda_;                  // weight vector
        double *weight_;                   // weight for the DM
        double *goldenPoint_;              // the golden point
        int weight_num_;                   // the number of weight vector
        int **neighbour_;	               // neighbours of each individual
        int neighbour_num_;                // the number of neighbours
        double *ideal_point_;
        int aggregation_type_;
        double pbi_theta_;
        double alpha_;
        double alpha_DM_;
	};

}