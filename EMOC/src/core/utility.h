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
	DominateReleation CheckDominance(Individual *ind1, Individual *ind2, int obj_num);
	int WeaklyDominates(double *point1, double *point2, int obj_num);

	int Combination(int n, int k);
	double CalculateDotProduct(double *vector1, double *vector2, int dimension);
	double CalculateCos(double *a, double *b, int dimension);
	double CalculateSin(double *a, double *b, int dimension);
	double CalculateNorm(double *vector, int dimension);
	double CalEuclidianDistance(double *a, double *b, int dimension);
	double CalPerpendicularDistance(double *a, double *weight, int dimension);

	void UpdateIdealpoint(Individual *ind, double *ideal_point, int obj_num);
	void UpdateNadirpoint(Individual *ind, double *nadir_point, int obj_num);
	void UpdateIdealpoint(Individual **pop, int pop_num, double *ideal_point, int obj_num);
	void UpdateNadirpoint(Individual **pop, int pop_num, double *nadir_point, int obj_num);

	// aggregation functions
	double CalWeightedSum(Individual *ind, double *weight_vector, double *ideal_point, int obj_num);
	double CalInverseChebycheff(Individual *ind, double *weight_vector, double *ideal_point, int obj_num);
	double CalPBI(Individual *ind, double *weight_vector, double *ideal_point, int obj_num, double theta = 0.0 );

}