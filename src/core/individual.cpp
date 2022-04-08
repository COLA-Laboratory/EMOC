#include "core/individual.h"

namespace emoc {

	Individual::Individual(int dec_num, int obj_num) :
		rank_(0), fitness_(0.0)
	{
		con.resize(obj_num, 0.0);
		dec.resize(dec_num, 0.0);
		obj.resize(obj_num, 0.0);
		dec_ = dec.data();
		obj_ = obj.data();
	}

	Individual::~Individual()
	{
		dec_ = nullptr;
		obj_ = nullptr;
	}
}


