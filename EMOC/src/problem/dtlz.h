#pragma once
#include "problem/problem.h"

namespace emoc {

	class DTLZ1 :public Problem
	{
	public:
		DTLZ1(int dec_num, int obj_num);
		virtual ~DTLZ1();

		void CalObj(Individual *ind);
	};

}