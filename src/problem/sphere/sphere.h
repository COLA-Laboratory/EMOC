#pragma once
#include "core/individual.h"
#include "problem/problem.h"

namespace emoc {

	class Sphere :public Problem
	{
	public:
		Sphere(int dec_num, int obj_num);
		virtual ~Sphere();

		void CalObj(Individual* ind);
	};

}