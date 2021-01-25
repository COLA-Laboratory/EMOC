#include "problem/zdt.h"

namespace emoc {



	ZDT1::ZDT1(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	ZDT1::~ZDT1()
	{

	}

	void ZDT1::CalObj(Individual *ind)
	{
		double f1 = 0, f2 = 0, sigema = 0;

		f1 = ind->dec_[0];
		for (int i = 1; i < dec_num_; i++)
		{
			sigema += ind->dec_[i];
		}

		double g = 1 + sigema * 9.0 / (dec_num_ - 1);
		double h = 1 - sqrt((double)(f1 / g));
		f2 = g * h;

		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}

}