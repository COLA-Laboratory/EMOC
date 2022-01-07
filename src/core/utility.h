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
	double CalEuclidianDistance(const double* a, const double* b, int dimension);
	double CalPerpendicularDistance(double *a, double *weight, int dimension);
	double CalPerpendicularDistanceNormalization(double* a, double* weight, int dimension, double* ideal, double* nadir);

	void UpdateIdealpoint(Individual *ind, double *ideal_point, int obj_num);
	void UpdateNadirpoint(Individual *ind, double *nadir_point, int obj_num);
	void UpdateIdealpoint(Individual **pop, int pop_num, double *ideal_point, int obj_num);
	void UpdateNadirpoint(Individual **pop, int pop_num, double *nadir_point, int obj_num);


	// aggregation functions
	double CalWeightedSum(Individual *ind, double *weight_vector, double *ideal_point, int obj_num);
	double CalInverseChebycheff(Individual *ind, double *weight_vector, double *ideal_point, int obj_num);
	double CalPBI(Individual *ind, double *weight_vector, double *ideal_point, int obj_num, double theta = 0.0 );


	void display_pop(FILE* gp, Individual** pop, int pop_num, int obj_num, int gen);

	// load function
	double** LoadPFData(int& pf_size, int obj_num, std::string problem_name);

	// distribution sampling
	double CauchyRandom(double location, double scale);
	double BetaRandom(double a, double b);
	double GaussianRandom(double mean, double stdev);

}