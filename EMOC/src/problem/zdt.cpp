#include "problem/zdt.h"

#include <cmath>

#include "core/global.h"

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
		double f1 = 0, f2 = 0;
		double g = 0, h = 0;

		f1 = ind->dec_[0];
		for (int i = 1; i < dec_num_; i++)
		{
			g += ind->dec_[i];
		}

		g = 1 + g * 9.0 / (dec_num_ - 1);
		h = 1 - sqrt((double)(f1 / g));
		f2 = g * h;

		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}

	ZDT2::ZDT2(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	ZDT2::~ZDT2()
	{

	}

	void ZDT2::CalObj(Individual *ind)
	{
		double f1 = 0, f2 = 0;
		double g = 0, h = 0;

		f1 = ind->dec_[0];
		for (int i = 1; i < dec_num_; i++)
		{
			g += ind->dec_[i];
		}

		g = 1 + g * 9.0 / (dec_num_ - 1);
		h = 1 - (f1 / g) * (f1 / g);
		f2 = g * h;

		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}

	ZDT3::ZDT3(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	ZDT3::~ZDT3()
	{

	}

	void ZDT3::CalObj(Individual *ind)
	{
		double f1 = 0, f2 = 0;
		double g = 0, h = 0;

		f1 = ind->dec_[0];
		for (int i = 1; i < dec_num_; i++)
		{
			g += ind->dec_[i];
		}

		g = 1 + g * 9.0 / (dec_num_ - 1);
		h = 1.0 - sqrt(f1 / g) - (f1 / g) * sin(10.0 * PI * f1);
		f2 = g * h;

		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}

	ZDT4::ZDT4(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		lower_bound_[0] = 0.0;
		upper_bound_[0] = 1.0;
		for (int i = 1; i < dec_num; i++)
		{
			lower_bound_[i] = -5.0;
			upper_bound_[i] = 5.0;
		}
	}

	ZDT4::~ZDT4()
	{

	}

	void ZDT4::CalObj(Individual *ind)
	{
		double f1 = 0, f2 = 0;
		double g = 0, h = 0;

		f1 = ind->dec_[0];
		for (int i = 1; i < dec_num_; i++)
		{
			g += ind->dec_[i] * ind->dec_[i] - 10.0 * cos(4.0 * PI * ind->dec_[i]);
		}

		g += 10.0 * (dec_num_ - 1) + 1.0;
		h = 1.0 - sqrt(f1 / g);
		f2 = g * h;

		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}

	ZDT6::ZDT6(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	ZDT6::~ZDT6()
	{

	}

	void ZDT6::CalObj(Individual *ind)
	{
		double f1 = 0, f2 = 0;
		double g = 0, h = 0;

		f1 = 1.0 - exp(-4.0 * ind->dec_[0]) * pow(sin(6.0 * PI * ind->dec_[0]), 6.0);
		for (int i = 1; i < dec_num_; i++)
		{
			g += ind->dec_[i];
		}

		g = 9.0 * pow(g / (dec_num_ - 1), 0.25) + 1.0;
		h = 1.0 - (f1 / g) *  (f1 / g);
		f2 = g * h;

		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}

}