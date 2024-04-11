#pragma once
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"
#include "metric/metric_head_collect.h"
#include "../vendor/armadillo/armadillo"

#include <vector>
#include <stack>
using namespace arma;

namespace emoc {

	class DPBEMO_DTS : public Algorithm
	{
	public:
		typedef struct
		{
			int index;
			double distance;
		}DistanceInfo;  // store crowding distance of index-th individual

		DPBEMO_DTS(int thread_id);
		virtual ~DPBEMO_DTS();
		
		void Solve();

	private:
		void Initialization();
		void Crossover(Individual **parent_pop, Individual **offspring_pop);

		// set the crowding distance of given individual index
		void SetDistanceInfo(std::vector<DistanceInfo> &distanceinfo_vec, int target_index, double distance);

		// use crowding distance to sort the individuals with rank rank_index, the result is stored in pop_sort
		// return the number of indviduals of rank rank_index
		int CrowdingDistance(Individual **mixed_pop, int pop_num, int *pop_sort, int rank_index);

		// do nsga2's environment selection on mixed_pop, the result is stored in parent_pop
		bool EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop);
		
		// calculate the virtual utility function value
		void UtilityFunction_Gaussian(Individual **pop);
		void StorePreference(Individual **pop);
		int Consult_DM();
		int FitnessViaPreference(Individual **mixed_pop, int pop_num, int *pop_sort, int rank_index, int nneed);
		double UpdateUtilityPrediction(double obj, int index);
		float CalculateKLDivergence(Individual **pop);
	
    private:
        HVCalculator hv_calculator_;
        int pf_size;
        double** pf_data;

		bool usingPref;
		int count;              // count the number of consultation times

		double *weight;	        // weight for DM
		double sigma; 	        // param for utility function
		double *utl;	        // store the utility function value
		int select_sum;
		mat PVals;              // store the winning probability of each solution
		// mat PSubset;			// store the winning probability of each subset
		int best_index;
        // int best_region;
        // vec win_times;

		double KL;
		double gamma;
		
		int max_consultation;
		double **recommend_point;
		double *pref_prob1;
		double *pref_prob2;
        int subset_solution_num;		
    
    };

}