#pragma once
#include "problem/problem.h"
#include "core/individual.h"

namespace emoc {
	
	// Basic class of all algorithms, it provides some useful functions to derived classes.
	// All derived classes need to override Run() which implement the real algorithm details
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

		// copy population pop_src1 and population pop_src2 to population pop_dest
		int MergePopulation(Individual **pop_src1, int pop_num1, Individual **pop_src2, int pop_num2, Individual **pop_dest);
		void CopyIndividual(Individual *ind_src, Individual *ind_dest);

	protected:
		Problem *problem_; // it is released outside
	};

}