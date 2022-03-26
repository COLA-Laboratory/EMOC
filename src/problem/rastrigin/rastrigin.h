#pragma once
#include "core/individual.h"
#include "problem/problem.h"

namespace emoc {

	class Rastrigin :public Problem
	{
	public:
		Rastrigin(int dec_num, int obj_num);
		virtual ~Rastrigin();

		void CalObj(Individual* ind);
	};

}