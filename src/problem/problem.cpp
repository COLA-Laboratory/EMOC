#include "problem/problem.h"

namespace emoc {

	Problem::Problem(int dec_num, int obj_num) :
		dec_num_(dec_num), obj_num_(obj_num), lower_bound_(new double[dec_num]), upper_bound_(new double[dec_num])
	{
	}

	Problem::~Problem()
	{
		delete[] lower_bound_;
		delete[] upper_bound_;
		lower_bound_ = nullptr;
		upper_bound_ = nullptr;
	}

}
