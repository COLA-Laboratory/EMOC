#pragma once
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"

#include <vector>

namespace emoc {

	class tDEA : public Algorithm
	{
	public:
		typedef struct
		{
			int index;
			double distance;
		}DistanceInfo; 

		tDEA(int thread_id);
		virtual ~tDEA();

		void Solve();

	private:
		void Initialization();
		void Crossover(Individual** parent_pop, Individual** offspring_pop);

		// do tdea's environment selection on mixed_pop, the result is stored in parent_pop
		void EnvironmentalSelection(Individual** parent_pop, int mixpop_num, Individual** mixed_pop);
		void GetNdPop(Individual** mixed_pop, int mixpop_num, Individual** ndpop,  int& ndpop_num);
		void GetExtremePop(Individual** ndpop, int ndpop_num, Individual** extreme_pop);
		void GetIntercepts(Individual** extreme_pop, Individual** ndpop, int ndpop_num, double* intercepts);
		void Normalization(Individual** ndpop, int ndpop_num, double** normalized_pop);
		void Cluster(double** normalized_pop, int pop_num);
		void thetaNDSort(Individual** ndpop, double** normalized_pop, int pop_num);

	private:
		double** lambda_;                  // weight vector
		int weight_num_;                   // the number of weight vector
		double* ideal_point_;
		double* nadir_point_;
		int** cluster_;
		int* cluster_count_;
		std::vector<Individual*> ndpop_;
		std::vector<Individual*> extreme_pop_;
	};

}