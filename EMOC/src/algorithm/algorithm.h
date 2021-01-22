#pragma once
#include "problem/problem.hpp"
#include "individual.h"

namespace emoc {

	class Algorithm
	{
	public:
		Algorithm(Problem *problem);
		~Algorithm();

		void PrintPop();
		virtual void Run() = 0;

	protected:
		void EvaluatePop(Individual **pop, int pop_num);
		void EvaluateInd(Individual *ind);

		int MergePopulation(Individual **pop_src1, int pop_num1, Individual **pop_src2, int pop_num2, Individual **pop_dest);
		void CopyIndividual(Individual *ind_src, Individual *ind_dest);

	protected:
		Problem *problem_; // released outside
	};

}