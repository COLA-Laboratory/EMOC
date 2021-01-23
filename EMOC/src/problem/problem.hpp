#pragma once
#include "core/individual.h"

namespace emoc {

	class Problem
	{
	public:
		Problem(int dec_num, int obj_num) :
			dec_num_(dec_num), obj_num_(obj_num), lower_bound_(new double[dec_num]), upper_bound_(new double[dec_num])
		{
		}
		
		virtual ~Problem()
		{
			delete[] lower_bound_;
			delete[] upper_bound_;
			lower_bound_ = nullptr;
			upper_bound_ = nullptr;
		}

		virtual void CalObj(Individual *ind) = 0;

	public:
		int dec_num_;
		int obj_num_;
		double *lower_bound_;
		double *upper_bound_;
	};
}