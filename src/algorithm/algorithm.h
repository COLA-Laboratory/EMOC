#pragma once
#include <ctime>

#include "problem/problem.h"
#include "core/individual.h"


namespace emoc {

	class Global;
	// Basic class of all algorithms, it provides some useful functions to derived classes.
	// All derived classes need to override Run() which implement the real algorithm details
	class Algorithm
	{
	public:
		Algorithm(int thread_id);
		virtual ~Algorithm();

		void PrintPop();
		virtual void Solve() = 0;

		inline int GetRealPopNum() { return real_popnum_; }
		inline double GetRuntime() { return runtime_; }

	protected:
		bool IsTermination();
		void EvaluatePop(Individual **pop, int pop_num);
		void EvaluateInd(Individual *ind);

		// copy population pop_src1 and population pop_src2 to population pop_dest
		int MergePopulation(Individual** pop_src1, int pop_num1, Individual** pop_src2, int pop_num2, Individual** pop_dest);
		void CopyIndividual(Individual* ind_src, Individual* ind_dest);
		void SwapIndividual(Individual* ind1, Individual* ind2);

		void TrackPopulation(int generation);
		void PlotPopulation(Individual** pop, int gen);
		bool CheckStopAndPause();

	protected:
		Global *g_GlobalSettings;	// pointer to current run's global settings
		int thread_id_;				// current thread id
		int real_popnum_;

		clock_t start_, end_;
		double runtime_;
		double record_file_time_;

	};

}