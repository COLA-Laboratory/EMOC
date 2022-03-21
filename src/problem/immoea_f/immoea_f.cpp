#include "problem/immoea_f/immoea_f.h"

#include <cmath>

#include "core/macro.h"
#include "core/global.h"

namespace emoc {

	IMMOEA_F::IMMOEA_F(int dec_num, int obj_num) : Problem(dec_num, obj_num)
	{

	}

	IMMOEA_F::~IMMOEA_F()
	{

	}

	IMMOEA_F1::IMMOEA_F1(int dec_num, int obj_num) :IMMOEA_F(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	IMMOEA_F1::~IMMOEA_F1()
	{

	}

	void IMMOEA_F1::CalObj(Individual* ind)
	{
		int i = 0;
		double f1 = 0; double f2 = 0; double g = 0; double t = 0;


		double sum = 0; double temp = 0;
		for (i = 1; i < dec_num_; i++)
		{
			t = (1 + IMF_ALPHA * (double)(i + 1) / (double)dec_num_) * ind->dec_[i];
			temp = pow((t - ind->dec_[0]), 2.0);
			sum += temp;
		}

		g = 1 + 9 * sum / (double)(dec_num_ - 1);

		f1 = ind->dec_[0];
		f2 = g * (1 - sqrt(f1 / g));
		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}

	IMMOEA_F2::IMMOEA_F2(int dec_num, int obj_num) :IMMOEA_F(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	IMMOEA_F2::~IMMOEA_F2()
	{

	}

	void IMMOEA_F2::CalObj(Individual* ind)
	{
		int i = 0;
		double f1 = 0; double f2 = 0; double g = 0; double t = 0;


		double sum = 0; double temp = 0;
		for (i = 1; i < dec_num_; i++)
		{
			t = (1 + IMF_ALPHA * (double)(i + 1) / (double)dec_num_) * ind->dec_[i];
			temp = pow((t - ind->dec_[0]), 2.0);
			sum += temp;
		}

		g = 1 + 9 * sum / (double)(dec_num_ - 1);

		f1 = ind->dec_[0];
		f2 = g * (1 - pow((f1 / g), 2.0));
		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}

	IMMOEA_F3::IMMOEA_F3(int dec_num, int obj_num) :IMMOEA_F(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	IMMOEA_F3::~IMMOEA_F3()
	{

	}

	void IMMOEA_F3::CalObj(Individual* ind)
	{
		int i = 0;
		double f1 = 0; double f2 = 0; double g = 0; double t = 0;


		double sum = 0; double temp = 0;
		for (i = 1; i < dec_num_; i++)
		{
			t = (1 + IMF_ALPHA * (double)(i + 1) / (double)dec_num_) * ind->dec_[i];
			temp = pow((t - ind->dec_[0]), 2.0);
			sum += temp;
		}

		g = 1 + 9 * sum / (double)(dec_num_ - 1);

		f1 = 1 - exp(-4.0 * ind->dec_[0]) * pow(sin(6 * PI * ind->dec_[0]), 6.0);
		f2 = g * (1 - pow((f1 / g), 2.0));
		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}

	IMMOEA_F4::IMMOEA_F4(int dec_num, int obj_num) :IMMOEA_F(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	IMMOEA_F4::~IMMOEA_F4()
	{

	}

	void IMMOEA_F4::CalObj(Individual* ind)
	{
		int i = 0;
		double f1 = 0; double f2 = 0; double f3 = 0; double g = 0; double t = 0;


		double sum = 0; double temp = 0;
		for (i = 2; i < dec_num_; i++)
		{
			t = (1 + IMF_ALPHA * (double)(i + 1) / (double)dec_num_) * ind->dec_[i];
			temp = pow((t - ind->dec_[0]), 2.0);
			sum += temp;
		}

		g = sum;

		f1 = (1 + g) * cos(PI * ind->dec_[0] / 2.0) * cos(PI * ind->dec_[1] / 2.0);
		f2 = (1 + g) * cos(PI * ind->dec_[0] / 2.0) * sin(PI * ind->dec_[1] / 2.0);
		f3 = (1 + g) * sin(PI * ind->dec_[0] / 2.0);
		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
		ind->obj_[2] = f3;
	}

	IMMOEA_F5::IMMOEA_F5(int dec_num, int obj_num) :IMMOEA_F(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	IMMOEA_F5::~IMMOEA_F5()
	{

	}

	void IMMOEA_F5::CalObj(Individual* ind)
	{
		int i = 0;
		double f1 = 0; double f2 = 0; double g = 0; double t = 0;


		double sum = 0; double temp = 0;
		for (i = 1; i < dec_num_; i++)
		{
			t = pow(ind->dec_[i], 1.0 / (1.0 + IMF_BETA * (double)(i + 1) / (double)dec_num_));
			temp = pow((t - ind->dec_[0]), 2.0);
			sum += temp;
		}

		g = 1 + 9 * sum / (double)(dec_num_ - 1);

		f1 = ind->dec_[0];
		f2 = g * (1 - sqrt(f1 / g));
		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}

	IMMOEA_F6::IMMOEA_F6(int dec_num, int obj_num) :IMMOEA_F(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	IMMOEA_F6::~IMMOEA_F6()
	{

	}

	void IMMOEA_F6::CalObj(Individual* ind)
	{
		int i = 0;
		double f1 = 0; double f2 = 0; double g = 0; double t = 0;


		double sum = 0; double temp = 0;
		for (i = 1; i < dec_num_; i++)
		{
			t = pow(ind->dec_[i], 1.0 / (1.0 + IMF_BETA * (double)(i + 1) / (double)dec_num_));
			temp = pow((t - ind->dec_[0]), 2.0);
			sum += temp;
		}

		g = 1 + 9 * sum / (double)(dec_num_ - 1);

		f1 = ind->dec_[0];
		f2 = g * (1 - pow(f1 / g, 2.0));
		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}

	IMMOEA_F7::IMMOEA_F7(int dec_num, int obj_num) :IMMOEA_F(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	IMMOEA_F7::~IMMOEA_F7()
	{

	}

	void IMMOEA_F7::CalObj(Individual* ind)
	{
		int i = 0;
		double f1 = 0; double f2 = 0; double g = 0; double t = 0;

		double sum = 0; double temp = 0;
		for (i = 1; i < dec_num_; i++)
		{
			t = pow(ind->dec_[i], 1.0 / (1.0 + IMF_BETA * (double)(i + 1) / (double)dec_num_));
			temp = pow((t - ind->dec_[0]), 2.0);
			sum += temp;
		}

		g = 1 + 9 * sum / (double)(dec_num_ - 1);

		f1 = 1 - exp(-4.0 * ind->dec_[0]) * pow(sin(6 * PI * ind->dec_[0]), 6.0);
		f2 = g * (1 - pow((f1 / g), 2.0));
		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}


	IMMOEA_F8::IMMOEA_F8(int dec_num, int obj_num) :IMMOEA_F(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	IMMOEA_F8::~IMMOEA_F8()
	{

	}

	void IMMOEA_F8::CalObj(Individual* ind)
	{
		int i = 0;
		double f1 = 0; double f2 = 0; double f3 = 0; double g = 0; double t = 0;


		double sum = 0; double temp = 0;
		for (i = 2; i < dec_num_; i++)
		{
			t = pow(ind->dec_[i], 1.0 / (1.0 + IMF_BETA * (double)(i + 1) / (double)dec_num_));
			temp = pow((t - ind->dec_[0]), 2.0);
			sum += temp;
		}

		g = sum;

		f1 = (1 + g) * cos(PI * ind->dec_[0] / 2.0) * cos(PI * ind->dec_[1] / 2.0);
		f2 = (1 + g) * cos(PI * ind->dec_[0] / 2.0) * sin(PI * ind->dec_[1] / 2.0);
		f3 = (1 + g) * sin(PI * ind->dec_[0] / 2.0);
		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
		ind->obj_[2] = f3;
	}

	IMMOEA_F9::IMMOEA_F9(int dec_num, int obj_num) :IMMOEA_F(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 10.0;
		}
		upper_bound_[0] = 1.0;
	}

	IMMOEA_F9::~IMMOEA_F9()
	{

	}

	void IMMOEA_F9::CalObj(Individual* ind)
	{
		int i = 0;
		double f1 = 0; double f2 = 0; double g = 0; double t = 0; double product = 1;


		double sum = 0; double temp1 = 0; double temp2 = 0;
		for (i = 1; i < dec_num_; i++)
		{
			t = pow(ind->dec_[i], 1.0 / (1.0 + IMF_BETA * (double)(i + 1) / (double)dec_num_));
			temp1 = pow((t - ind->dec_[0]), 2.0) / 4000.0;
			temp2 = cos((t - ind->dec_[0]) / sqrt((double)i));
			product *= temp2;
			sum += temp1;
		}

		g = sum - product + 2;

		f1 = ind->dec_[0];
		f2 = g * (1 - sqrt(f1 / g));
		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}

	IMMOEA_F10::IMMOEA_F10(int dec_num, int obj_num) :IMMOEA_F(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 10.0;
		}
		upper_bound_[0] = 1.0;
	}

	IMMOEA_F10::~IMMOEA_F10()
	{

	}

	void IMMOEA_F10::CalObj(Individual* ind)
	{
		int i = 0;
		double f1 = 0; double f2 = 0; double g = 0; double t = 0; double product = 1;


		double sum = 0; double temp1 = 0; double temp2 = 0;
		for (i = 1; i < dec_num_; i++)
		{
			t = pow(ind->dec_[i], 1.0 / (1.0 + IMF_BETA * (double)(i + 1) / (double)dec_num_));
			temp1 = pow((t - ind->dec_[0]), 2.0);
			temp2 = 10 * cos(2 * PI * (t - ind->dec_[0]));
			sum += (temp1 - temp2);
		}

		g = 1 + 10 * (dec_num_ - 1) + sum;

		f1 = ind->dec_[0];
		f2 = g * (1 - sqrt(f1 / g));
		ind->obj_[0] = f1;
		ind->obj_[1] = f2;
	}


}