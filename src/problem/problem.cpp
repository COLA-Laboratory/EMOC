#include "problem/problem.h"

namespace emoc {

	Problem::Problem(int dec_num, int obj_num) :
		dec_num_(dec_num), obj_num_(obj_num)
	{
		lower_bound_.resize(dec_num);
		upper_bound_.resize(dec_num);
	}

	Problem::~Problem()
	{

	}

	void Problem::CalCon(Individual* ind)
	{
		// do nothing
	}

}
