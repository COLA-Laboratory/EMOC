#pragma once
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"

#include <vector>

namespace emoc {

	class RVEA : public Algorithm
	{
	public:
		typedef struct
		{
			int index;
			double distance;
		}DistanceInfo;  // store euclidian distance to the index-th weight vector

		RVEA(int thread_id);
		virtual ~RVEA();

		void Solve();

	private:
		void Initialization();
		void Crossover(Individual** parent_pop, Individual** offspring_pop);

		void ReferenceVectorNormalization(double** lambda);
		int ReferenceVectorGuidedSelection(Individual** parent_pop, Individual** mix_pop, int mix_pop_num);
		void ReferenceVectorAdaption(Individual** parent_pop);

	private:
		double** origin_lambda_;           // weight vector
		double** normalized_lambda_;       // weight vector
		int weight_num_;                   // the number of weight vector
		double* ideal_point_;
		double* nadir_point_;
		double max_gen_;

		// RVEA Parameters
		double alpha_ = 2.0;
		double fr_ = 0.1;
	};

}