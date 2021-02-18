// This file provides some important utility functions
#pragma once
#include "core/global.h"
#include "core/individual.h"

namespace emoc {

	enum DominateReleation 
	{
		DOMINATED = -1,
		NON_DOMINATED = 0,
		DOMINATE = 1
	};
	DominateReleation CheckDominance(Individual *ind1, Individual *ind2);

	int Combination(int n, int k);
	double CalEuclidianDistance(double *a, double *b, int dimension);

	void UpdateIdealpoint(Individual *ind, double *ideal_point);
	void UpdateNadirpoint(Individual *ind, double *nadir_point);
	void UpdateIdealpoint(Individual **pop, int pop_num, double *ideal_point);
	void UpdateNadirpoint(Individual **pop, int pop_num, double *nadir_point);

	// aggregation functions
	double CalWeightedSum(Individual *ind, double *weight_vector, double *ideal_point);
	double CalInverseChebycheff(Individual *ind, double *weight_vector, double *ideal_point);
	double CalPBI(Individual *ind, double *weight_vector, double *ideal_point, double theta = 0.0 );

}