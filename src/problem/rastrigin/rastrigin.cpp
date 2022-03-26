#include "problem/rastrigin/rastrigin.h"

#include <cmath>

#include "core/macro.h"
#include "core/global.h"

namespace emoc {

	Rastrigin::Rastrigin(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = -5.12;
			upper_bound_[i] = 5.12;
		}
	}

	Rastrigin::~Rastrigin()
	{

	}

	void Rastrigin::CalObj(Individual* ind)
	{
		double  sum = 0;
		for (int i = 0; i < dec_num_; i++)
		{
			sum += (ind->dec_[i] * ind->dec_[i] - 10 * cos(2 * PI * ind->dec_[i]) + 10);
		}
		ind->obj_[0] = sum;
	}

}