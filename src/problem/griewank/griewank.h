#pragma once
#include "core/individual.h"
#include "problem/problem.h"

namespace emoc {

	class Griewank :public Problem
	{
	public:
		Griewank(int dec_num, int obj_num);
		virtual ~Griewank();

		void CalObj(Individual* ind);
	};

}