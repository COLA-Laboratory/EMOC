#include "core/individual.h"

namespace emoc {

	Individual::Individual(int dec_num, int obj_num) :
		rank_(0), fitness_(0.0)
	{
		con_.resize(obj_num, 0.0);
		dec_.resize(dec_num, 0.0);
		obj_.resize(obj_num, 0.0);
	}

	Individual::~Individual()
	{
	}
}


