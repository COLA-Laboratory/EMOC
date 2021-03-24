#pragma once
#include "core/individual.h"

namespace emoc {

	// Basic class of all test problems, it holds some common datas and
	// initializes by constructor. All derived class needs to overide
	// function CalObj() which is used to calculate objectives.
	class Problem
	{
	public:
		Problem(int dec_num, int obj_num);
		virtual ~Problem();

		virtual void CalObj(Individual *ind) = 0;

	public:
		int dec_num_;
		int obj_num_;
		double *lower_bound_;
		double *upper_bound_;
	};
}