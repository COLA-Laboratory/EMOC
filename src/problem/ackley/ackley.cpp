#include "problem/ackley/ackley.h"

#include <cmath>

#include "core/macro.h"
#include "core/global.h"

namespace emoc {

	Ackley::Ackley(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = -32.768;
			upper_bound_[i] = 32.768;
		}
	}

	Ackley::~Ackley()
	{

	}

	void Ackley::CalObj(Individual *ind)
	{
		double  sum1 = 0, sum2 = 0;
		for (int i = 0; i < dec_num_; i++)
		{
			sum1 += (ind->dec_[i] * ind->dec_[i]);
			sum2 += cos(2 * PI * ind->dec_[i]);
		}
		sum1 /= (double)dec_num_;
		sum2 /= (double)dec_num_;
		ind->obj_[0] = 20 + exp(1) - 20 * exp(-0.2 * sqrt(sum1)) - exp(sum2);
	}

}