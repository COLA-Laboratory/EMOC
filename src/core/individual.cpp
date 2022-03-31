#include "core/individual.h"

namespace emoc {

	Individual::Individual(int dec_num, int obj_num) :
		rank_(0), fitness_(0.0)
	{
		dec.resize(dec_num);
		obj.resize(obj_num);
		dec_ = dec.data();
		obj_ = obj.data();
	}

	Individual::~Individual()
	{
		dec_ = nullptr;
		obj_ = nullptr;
	}
}


