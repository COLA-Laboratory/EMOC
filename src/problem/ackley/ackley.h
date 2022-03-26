#pragma once
#include "core/individual.h"
#include "problem/problem.h"

namespace emoc {

	class Ackley:public Problem
	{
	public:
		Ackley(int dec_num, int obj_num);
		virtual ~Ackley();

		void CalObj(Individual *ind);
	};

}