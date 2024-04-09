// This file provides some important utility functions
#pragma once
#include <vector>
#include <string>

#include "core/global.h"
#include "core/individual.h"
#include "core/emoc_utility_structures.h"

namespace emoc {

	// set weight
	double* SetWeight(const std::string& weightstring);
	double* SetWeight(int obj_num_, std::string problem_name);

	// dominance related functions
	int CheckDominance(Individual* ind1, Individual* ind2, int obj_num);
	int CheckDominanceWithConstraint(Individual* ind1, Individual* ind2, int obj_num);
	int WeaklyDominates(double *point1, double *point2, int obj_num);

	// math functions
	int Combination(int n, int k);
	double CalculateDotProduct(double *vector1, double *vector2, int dimension);
	double CalculateCos(double *a, double *b, int dimension);
	double CalculateSin(double *a, double *b, int dimension);
	double CalculateNorm(double *vector, int dimension);
	double CalEuclidianDistance(const double* a, const double* b, int dimension);
	double CalPerpendicularDistance(double* a, double* weight, int dimension);
	double* GaussianElimination(double** A, double* b, double* x, int obj_num);
	double CalPerpendicularDistanceNormalization(double* a, double* weight, int dimension, double* ideal, double* nadir);

	// ideal point and nadir point related
	void UpdateIdealpoint(Individual *ind, double *ideal_point, int obj_num);
	void UpdateNadirpoint(Individual *ind, double *nadir_point, int obj_num);
	void UpdateIdealpoint(Individual** pop, int pop_num, double* ideal_point, int obj_num);
	void UpdateNadirpoint(Individual** pop, int pop_num, double* nadir_point, int obj_num);
	void UpdateNadirpointWithRank(Individual** pop, int pop_num, double* nadir_point, int obj_num, int rank);

	// aggregation functions
	double CalWeightedLpScalarizing(Individual* ind, double* weight_vector, double* ideal_point, double *nadir_point, int obj_num, int p = -1);
	double CalWeightedSum(Individual *ind, double *weight_vector, double *ideal_point, int obj_num);
	double CalInverseChebycheff(Individual *ind, double *weight_vector, double *ideal_point, int obj_num);
	double CalPBI(Individual *ind, double *weight_vector, double *ideal_point, int obj_num, double theta = 0.0 );

	// load function
	double** LoadPFData(int& pf_size, int obj_num, std::string problem_name);

	// distribution sampling
	double CauchyRandom(double location, double scale);
	double BetaRandom(double a, double b);
	double GaussianRandom(double mean, double stdev);

	// statistic tests
	int RankSumTest(const std::vector<double>& array1, const std::vector<double>& array2);
	int SignRankTest(const std::vector<double>& array1, const std::vector<double>& array2);

	// EMOC running results related functions
	void CollectSingleThreadResult(int run_id, int thread_id, EMOCParameters para);
	void CollectMultiThreadResult(int run_id, int parameter_id, int thread_id);
	int GetBestParameterIndex(int start, int end, const std::string& metric, const std::string& format);
	void StatisticTestAccordingMetric(EMOCMultiThreadResult& res, EMOCMultiThreadResult& compared_res, const std::string& metric, const std::string& format);
	void UpdateExpStatTest(int parameter_index);
	void UpdateExpResult(EMOCMultiThreadResult& res, int new_res_index, int parameter_index);
	void UpdateExpMetricStat(std::vector<double>& indicator_history, std::vector<bool>& is_indicator_record, double& mean, double& std, double& median, double& iqr);
}