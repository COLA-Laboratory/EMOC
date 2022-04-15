#pragma once
#include <vector>

#include "core/individual.h"

namespace emoc {

	// Basic class of all test problems, it holds some common datas and
	// initializes by constructor. All derived class needs to overide
	// function CalObj() which is used to calculate objectives.
	class Problem
	{
	public:
		enum EncodingType
		{
			REAL = 1,
			BINARY = 2,
			PERMUTATION = 4
		};

	public:
		Problem(int dec_num, int obj_num);
		virtual ~Problem();

		virtual void CalObj(Individual* ind) = 0;
		virtual void CalCon(Individual* ind);

	public:
		int dec_num_;
		int obj_num_;
		std::vector<double> lower_bound_;
		std::vector<double> upper_bound_;
		EncodingType encoding_;
	};

}