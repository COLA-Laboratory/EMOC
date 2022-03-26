#include "problem/griewank/griewank.h"

#include <cmath>

#include "core/macro.h"
#include "core/global.h"

namespace emoc {

	Griewank::Griewank(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = -600.0;
			upper_bound_[i] = 600.0;
		}
	}

	Griewank::~Griewank()
	{

	}

	void Griewank::CalObj(Individual* ind)
	{
		double  sum = 0, product = 1;
		for (int i = 0; i < dec_num_; i++)
		{
			sum += (ind->dec_[i] * ind->dec_[i] / 4000.0);
			product *= cos(ind->dec_[i] / sqrt((double)(i + 1)));
		}

		ind->obj_[0] = sum - product + 1;
	}

}