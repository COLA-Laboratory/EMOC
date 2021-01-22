#include "individual.h"

namespace emoc {

	Individual::Individual(int dec_num, int obj_num):
		rank_(0),fitness_(0.0),dec_(new double[dec_num]),obj_(new double[obj_num])
	{

	}

	Individual::~Individual()
	{
		delete[] dec_;
		delete[] obj_;
		dec_ = nullptr;
		obj_ = nullptr;
	}
}


