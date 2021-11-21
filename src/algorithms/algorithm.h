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
		Algorithm(Problem *problem, int thread_num);
		virtual ~Algorithm();

		void PrintPop();
		virtual void Run() = 0;

		inline int GetRealPopNum() { return real_popnum_; }
		inline bool GetFinish() { return is_finish_; }
		inline bool GetPause() { return is_finish_; }
		inline void SetPlot(bool is_plotting) { is_plotting_ = is_plotting; }
		inline void SetPause(bool is_pause) { is_pause_ = is_pause; }
		inline void SetFinish(bool is_finish) { is_finish_ = is_finish; }

	protected:
		void EvaluatePop(Individual **pop, int pop_num);
		void EvaluateInd(Individual *ind);

		// copy population pop_src1 and population pop_src2 to population pop_dest
		int MergePopulation(Individual **pop_src1, int pop_num1, Individual **pop_src2, int pop_num2, Individual **pop_dest);
		void CopyIndividual(Individual *ind_src, Individual *ind_dest);

		void TrackPopulation(int generation);
		void PlotPopulation(Individual** pop, int gen);

	public:
		double record_file_time_;

	protected:
		Global *g_GlobalSettings;
		Problem *problem_; // it is released outside
		int thread_id_;   // current thread number
		int real_popnum_;
		bool is_plotting_;
		bool is_pause_;
		bool is_finish_;

		clock_t start_, end_;
	};

}