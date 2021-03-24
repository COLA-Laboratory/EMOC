#pragma once
#include "algorithms/algorithm.h"
#include "core/individual.h"
#include "problem/problem.h"

namespace emoc {

	class HypE : public Algorithm
	{
	public:
		typedef struct
		{
			int index;
			double value;
		}FitnessInfo;

		HypE(Problem *problem, int thread_num);
		virtual ~HypE();

		void Run();

	private:
		void Initialization();
		void Crossover(Individual **parent_pop, Individual **offspring_pop);
		void CalculateFitness(Individual **pop, int pop_num, int parameter_k);

		// hv calculate related
		void HypeSampling(FitnessInfo *fitness_info, int samples_num, int parameter_k, double *rho, Individual **pop, int pop_num);
		void HypeExact(FitnessInfo *fitness_info, int parameter_k, double *rho, Individual **pop, int pop_num);
		void HypeExactRecursive(double* input_p, int pnts, int dim, int nrOfPnts, int actDim, double* bounds, int* input_pvec, double* fitness, double* rho, int param_k);
		void RearrangeIndicesByColumn(double *mat, int rows, int columns, int col, int *ind);

		void EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop);

	private:
		double *ideal_point_;
		double *nadir_point_;

		// memory for HypeExactRecursive function
		int *pvec_recursive_;
		double *p_recursive_;
		double *tmpfit_recursive_;
	};

}