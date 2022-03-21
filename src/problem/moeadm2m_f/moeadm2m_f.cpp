#include "problem/moeadm2m_f/moeadm2m_f.h"

#include <cmath>

#include "core/macro.h"

namespace emoc {

	MOEADM2M_F1::MOEADM2M_F1(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	MOEADM2M_F1::~MOEADM2M_F1()
	{

	}

	void MOEADM2M_F1::CalObj(Individual* ind)
	{
		int i = 0;
		double f1 = 0; double f2 = 0; double g = 0; double t = 0;

		double sum = 0; double temp = 0;
		for (i = 1; i < dec_num_; i++)
		{
			t = ind->dec_[i] - sin(0.5 * PI * ind->dec_[0]);
			temp = (-0.9 * pow(t, 2) + pow(fabs(t), 0.6));
			sum += temp;
		}

		g = 2 * sin(PI * ind->dec_[0]) * sum;

		f1 = (1 + g) * ind->dec_[0];
		f2 = (1 + g) * (1 - sqrt(ind->dec_[0]));
		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}

	MOEADM2M_F2::MOEADM2M_F2(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	MOEADM2M_F2::~MOEADM2M_F2()
	{

	}

	void MOEADM2M_F2::CalObj(Individual* ind)
	{
		int i = 0;
		double f1, f2, g, t = 0;

		double sum = 0; double temp = 0;
		for (i = 1; i < dec_num_; i++)
		{
			t = ind->dec_[i] - sin(0.5 * PI * ind->dec_[0]);
			temp = fabs(t) / (1 + exp(5 * fabs(t)));
			sum += temp;
		}

		g = 10 * sin(PI * ind->dec_[0]) * sum;
		f1 = (1 + g) * ind->dec_[0];
		f2 = (1 + g) * (1 - pow(ind->dec_[0], 2));
		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}

	MOEADM2M_F3::MOEADM2M_F3(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	MOEADM2M_F3::~MOEADM2M_F3()
	{

	}

	void MOEADM2M_F3::CalObj(Individual* ind)
	{
		int i = 0;
		double f1, f2, g, t = 0;

		double sum = 0; double temp = 0;
		for (i = 1; i < dec_num_; i++)
		{
			t = ind->dec_[i] - sin(0.5 * PI * ind->dec_[0]);
			temp = fabs(t) / (1 + exp(5 * fabs(t)));
			sum += temp;
		}

		g = 10 * sin(PI * ind->dec_[0] / 2.0) * sum;
		f1 = (1 + g) * cos(PI * ind->dec_[0] / 2.0);
		f2 = (1 + g) * sin(PI * ind->dec_[0] / 2.0);
		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}

	MOEADM2M_F4::MOEADM2M_F4(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	MOEADM2M_F4::~MOEADM2M_F4()
	{

	}

	void MOEADM2M_F4::CalObj(Individual* ind)
	{
		int i = 0;
		double f1, f2, g, t = 0;

		double sum = 0; double temp = 0;
		for (i = 1; i < dec_num_; i++)
		{
			t = ind->dec_[i] - sin(0.5 * PI * ind->dec_[0]);
			temp = fabs(t) / (1 + exp(5 * fabs(t)));
			sum += temp;
		}

		g = 10 * sin(PI * ind->dec_[0]) * sum;
		f1 = (1 + g) * ind->dec_[0];
		f2 = (1 + g) * (1 - pow(ind->dec_[0], 0.5) * pow(cos(2 * PI * ind->dec_[0]), 2.0));
		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}

	MOEADM2M_F5::MOEADM2M_F5(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	MOEADM2M_F5::~MOEADM2M_F5()
	{

	}

	void MOEADM2M_F5::CalObj(Individual* ind)
	{
		int i = 0;
		double f1 = 0; double f2 = 0; double g = 0; double t = 0;

		double sum = 0; double temp = 0;
		for (i = 1; i < dec_num_; i++)
		{
			t = ind->dec_[i] - sin(0.5 * PI * ind->dec_[0]);
			temp = (-0.9 * pow(t, 2) + pow(fabs(t), 0.6));
			sum += temp;
		}

		g = 2 * fabs(cos(PI * ind->dec_[0])) * sum;
		f1 = (1 + g) * ind->dec_[0];
		f2 = (1 + g) * (1 - sqrt(ind->dec_[0]));
		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}

	MOEADM2M_F6::MOEADM2M_F6(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	MOEADM2M_F6::~MOEADM2M_F6()
	{

	}

	void MOEADM2M_F6::CalObj(Individual* ind)
	{
		int i = 0;
		double f1, f2, f3, g, t = 0;

		double sum = 0; double temp = 0;
		for (i = 2; i < dec_num_; i++)
		{
			t = ind->dec_[i] - ind->dec_[0] * ind->dec_[1];
			temp = (-0.9 * pow(t, 2) + pow(fabs(t), (double)0.6));
			sum += temp;
		}

		g = 2 * sin(PI * ind->dec_[0]) * sum;
		f1 = (1 + g) * ind->dec_[0] * ind->dec_[1];
		f2 = (1 + g) * ind->dec_[0] * (1 - ind->dec_[1]);
		f3 = (1 + g) * (1 - ind->dec_[0]);
		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
		ind->obj_[2] = f3;
	}

	MOEADM2M_F7::MOEADM2M_F7(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	MOEADM2M_F7::~MOEADM2M_F7()
	{

	}

	void MOEADM2M_F7::CalObj(Individual* ind)
	{
		int i = 0;
		double f1, f2, f3, g, t = 0;

		double sum = 0; double temp = 0;
		for (i = 2; i < dec_num_; i++)
		{
			t = ind->dec_[i] - ind->dec_[0] * ind->dec_[1];
			temp = (-0.9 * pow(t, 2) + pow(fabs(t), (double)0.6));
			sum += temp;
		}

		g = 2 * sin(PI * ind->dec_[0]) * sum;
		f1 = (1 + g) * cos(PI * ind->dec_[0] / 2.0) * cos(PI * ind->dec_[1] / 2.0);
		f2 = (1 + g) * cos(PI * ind->dec_[0] / 2.0) * sin(PI * ind->dec_[1] / 2.0);
		f3 = (1 + g) * sin(PI * ind->dec_[0] / 2.0);
		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
		ind->obj_[2] = f3;
	}
}