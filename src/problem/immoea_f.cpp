#include "problem/immoea_f.h"

#include <cmath>

#include "core/macro.h"
#include "core/global.h"

namespace emoc {

	IMMOEA_F1::IMMOEA_F1(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = -1.0;
			upper_bound_[i] = 1.0;
		}

		lower_bound_[0] = 0.0;
	}

	IMMOEA_F1::~IMMOEA_F1()
	{

	}

	void IMMOEA_F1::CalObj(Individual* ind)
	{
		int count1 = 0, count2 = 0;
		double sum1 = 0.0, sum2 = 0.0, yj = 0.0;

		double* obj = ind->obj_;
		double* xreal = ind->dec_;

		for (int i = 2; i <= dec_num_; i++)
		{
			yj = xreal[i - 1] - sin(6.0 * PI * xreal[0] + i * PI / dec_num_);
			yj = yj * yj;
			if (i % 2 == 0)
			{
				sum2 += yj;
				count2++;
			}
			else
			{
				sum1 += yj;
				count1++;
			}
		}

		obj[0] = xreal[0] + 2.0 * sum1 / (double)count1;
		obj[1] = 1.0 - sqrt(xreal[0]) + 2.0 * sum2 / (double)count2;
	}

	IMMOEA_F2::IMMOEA_F2(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = -1.0;
			upper_bound_[i] = 1.0;
		}

		lower_bound_[0] = 0.0;
	}

	IMMOEA_F2::~IMMOEA_F2()
	{

	}

	void IMMOEA_F2::CalObj(Individual* ind)
	{
		int count1 = 0, count2 = 0;
		double sum1 = 0.0, sum2 = 0.0, yj = 0.0;

		double* obj = ind->obj_;
		double* xreal = ind->dec_;

		for (int i = 2; i <= dec_num_; i++)
		{
			if (i % 2 == 0)
			{
				yj = xreal[i - 1] - 0.3 * xreal[0] * (xreal[0] * cos(24.0 * PI * xreal[0] + 4.0 * i * PI / dec_num_) + 2.0) *
					sin(6.0 * PI * xreal[0] + i * PI / dec_num_);
				sum2 += yj * yj;
				count2++;
			}
			else
			{
				yj = xreal[i - 1] - 0.3 * xreal[0] * (xreal[0] * cos(24.0 * PI * xreal[0] + 4.0 * i * PI / dec_num_) + 2.0) *
					cos(6.0 * PI * xreal[0] + i * PI / dec_num_);
				sum1 += yj * yj;
				count1++;
			}
		}

		obj[0] = xreal[0] + 2.0 * sum1 / (double)count1;
		obj[1] = 1.0 - sqrt(xreal[0]) + 2.0 * sum2 / (double)count2;
	}

	IMMOEA_F3::IMMOEA_F3(int dec_num, int obj_num) :Problem(dec_num, obj_num)
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
		int count1 = 0, count2 = 0;
		double sum1 = 0.0, sum2 = 0.0;
		double prod1 = 1.0, prod2 = 1.0, yj = 0.0, pj = 0.0;

		double* obj = ind->obj_;
		double* xreal = ind->dec_;

		for (int i = 2; i <= dec_num_; i++)
		{
			yj = xreal[i - 1] - pow(xreal[0], 0.5 * (1.0 + 3.0 * (i - 2.0) / (dec_num_ - 2.0)));
			pj = cos(20.0 * yj * PI / sqrt(i + 0.0));
			if (i % 2 == 0)
			{
				sum2 += yj * yj;
				prod2 *= pj;
				count2++;
			}
			else
			{
				sum1 += yj * yj;
				prod1 *= pj;
				count1++;
			}
		}

		obj[0] = xreal[0] + 2.0 * (4.0 * sum1 - 2.0 * prod1 + 2.0) / (double)count1;
		obj[1] = 1.0 - sqrt(xreal[0]) + 2.0 * (4.0 * sum2 - 2.0 * prod2 + 2.0) / (double)count2;
	}

	IMMOEA_F4::IMMOEA_F4(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = -2.0;
			upper_bound_[i] = 2.0;
		}
		lower_bound_[0] = 0.0;
		upper_bound_[0] = 1.0;
	}

	IMMOEA_F4::~IMMOEA_F4()
	{

	}

	void IMMOEA_F4::CalObj(Individual* ind)
	{
		int count1 = 0, count2 = 0;
		double sum1 = 0.0, sum2 = 0.0;
		double yj = 0.0, hj = 0.0;

		double* obj = ind->obj_;
		double* xreal = ind->dec_;

		for (int i = 2; i <= dec_num_; i++)
		{
			yj = xreal[i - 1] - sin(6.0 * PI * xreal[0] + i * PI / dec_num_);
			hj = fabs(yj) / (1.0 + exp(2.0 * fabs(yj)));
			if (i % 2 == 0)
			{
				sum2 += hj;
				count2++;
			}
			else
			{
				sum1 += hj;
				count1++;
			}
		}

		obj[0] = xreal[0] + 2.0 * sum1 / (double)count1;
		obj[1] = 1.0 - xreal[0] * xreal[0] + 2.0 * sum2 / (double)count2;
	}

	IMMOEA_F5::IMMOEA_F5(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = -1.0;
			upper_bound_[i] = 1.0;
		}

		lower_bound_[0] = 0.0;
	}

	IMMOEA_F5::~IMMOEA_F5()
	{

	}

	void IMMOEA_F5::CalObj(Individual* ind)
	{
		int count1 = 0, count2 = 0;
		double sum1 = 0.0, sum2 = 0.0;
		double yj = 0.0, hj = 0.0, Nm = 10.0, Em = 0.1;

		double* obj = ind->obj_;
		double* xreal = ind->dec_;

		for (int i = 2; i <= dec_num_; i++)
		{
			yj = xreal[i - 1] - sin(6.0 * PI * xreal[0] + i * PI / dec_num_);
			hj = 2.0 * yj * yj - cos(4.0 * PI * yj) + 1.0;
			if (i % 2 == 0)
			{
				sum2 += hj;
				count2++;
			}
			else
			{
				sum1 += hj;
				count1++;
			}
		}
		hj = (0.5 / Nm + Em) * fabs(sin(2.0 * Nm * PI * xreal[0]));

		obj[0] = xreal[0] + hj + 2.0 * sum1 / (double)count1;
		obj[1] = 1.0 - xreal[0] + hj + 2.0 * sum2 / (double)count2;
	}

	IMMOEA_F6::IMMOEA_F6(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = -1.0;
			upper_bound_[i] = 1.0;
		}

		lower_bound_[0] = 0.0;
	}

	IMMOEA_F6::~IMMOEA_F6()
	{

	}

	void IMMOEA_F6::CalObj(Individual* ind)
	{
		int count1 = 0, count2 = 0;
		double sum1 = 0.0, sum2 = 0.0;
		double prod1 = 1.0, prod2 = 1.0;
		double yj = 0.0, hj = 0.0, pj = 0.0, Nm = 2.0, Em = 0.1;

		double* obj = ind->obj_;
		double* xreal = ind->dec_;

		for (int i = 2; i <= dec_num_; i++)
		{
			yj = xreal[i - 1] - sin(6.0 * PI * xreal[0] + i * PI / dec_num_);
			pj = cos(20.0 * yj * PI / sqrt(i + 0.0));
			if (i % 2 == 0)
			{
				sum2 += yj * yj;
				prod2 *= pj;
				count2++;
			}
			else
			{
				sum1 += yj * yj;
				prod1 *= pj;
				count1++;
			}
		}
		hj = 2.0 * (0.5 / Nm + Em) * sin(2.0 * Nm * PI * xreal[0]);
		if (hj < 0.0) hj = 0.0;

		obj[0] = xreal[0] + hj + 2.0 * (4.0 * sum1 - 2.0 * prod1 + 2.0) / (double)count1;
		obj[1] = 1.0 - xreal[0] + hj + 2.0 * (4.0 * sum2 - 2.0 * prod2 + 2.0) / (double)count2;
	}

	IMMOEA_F7::IMMOEA_F7(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = -1.0;
			upper_bound_[i] = 1.0;
		}

		lower_bound_[0] = 0.0;
	}

	IMMOEA_F7::~IMMOEA_F7()
	{

	}

	void IMMOEA_F7::CalObj(Individual* ind)
	{
		int count1 = 0, count2 = 0;
		double sum1 = 0.0, sum2 = 0.0, yj = 0.0;

		double* obj = ind->obj_;
		double* xreal = ind->dec_;

		for (int i = 2; i <= dec_num_; i++)
		{
			yj = xreal[i - 1] - sin(6.0 * PI * xreal[0] + i * PI / dec_num_);
			if (i % 2 == 0)
			{
				sum2 += yj * yj;
				count2++;
			}
			else
			{
				sum1 += yj * yj;
				count1++;
			}
		}
		yj = pow(xreal[0], 0.2);

		obj[0] = yj + 2.0 * sum1 / (double)count1;
		obj[1] = 1.0 - yj + 2.0 * sum2 / (double)count2;
	}


	IMMOEA_F8::IMMOEA_F8(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = -2.0;
			upper_bound_[i] = 2.0;
		}

		lower_bound_[0] = 0.0;
		lower_bound_[1] = 0.0;
		upper_bound_[0] = 1.0;
		upper_bound_[1] = 1.0;
	}

	IMMOEA_F8::~IMMOEA_F8()
	{

	}

	void IMMOEA_F8::CalObj(Individual* ind)
	{
		int count1 = 0, count2 = 0, count3 = 0;
		double sum1 = 0.0, sum2 = 0.0, sum3 = 0.0, yj = 0.0;

		double* obj = ind->obj_;
		double* xreal = ind->dec_;

		for (int i = 3; i <= dec_num_; i++)
		{
			yj = xreal[i - 1] - 2.0 * xreal[1] * sin(2.0 * PI * xreal[0] + i * PI / dec_num_);
			if (i % 3 == 1)
			{
				sum1 += yj * yj;
				count1++;
			}
			else if (i % 3 == 2)
			{
				sum2 += yj * yj;
				count2++;
			}
			else
			{
				sum3 += yj * yj;
				count3++;
			}
		}

		obj[0] = cos(0.5 * PI * xreal[0]) * cos(0.5 * PI * xreal[1]) + 2.0 * sum1 / (double)count1;
		obj[1] = cos(0.5 * PI * xreal[0]) * sin(0.5 * PI * xreal[1]) + 2.0 * sum2 / (double)count2;
		obj[2] = sin(0.5 * PI * xreal[0]) + 2.0 * sum3 / (double)count3;
	}

	IMMOEA_F9::IMMOEA_F9(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = -2.0;
			upper_bound_[i] = 2.0;
		}

		lower_bound_[0] = 0.0;
		lower_bound_[1] = 0.0;
		upper_bound_[0] = 1.0;
		upper_bound_[1] = 1.0;
	}

	IMMOEA_F9::~IMMOEA_F9()
	{

	}

	void IMMOEA_F9::CalObj(Individual* ind)
	{
		int count1 = 0, count2 = 0, count3 = 0;
		double sum1 = 0.0, sum2 = 0.0, sum3 = 0.0, yj = 0.0, Em = 0.1;

		double* obj = ind->obj_;
		double* xreal = ind->dec_;

		for (int i = 3; i <= dec_num_; i++)
		{
			yj = xreal[i - 1] - 2.0 * xreal[1] * sin(2.0 * PI * xreal[0] + i * PI / dec_num_);
			if (i % 3 == 1)
			{
				sum1 += yj * yj;
				count1++;
			}
			else if (i % 3 == 2)
			{
				sum2 += yj * yj;
				count2++;
			}
			else
			{
				sum3 += yj * yj;
				count3++;
			}
		}
		yj = (1.0 + Em) * (1.0 - 4.0 * (2.0 * xreal[0] - 1.0) * (2.0 * xreal[0] - 1.0));
		if (yj < 0.0) yj = 0.0;

		obj[0] = 0.5 * (yj + 2 * xreal[0]) * xreal[1] + 2.0 * sum1 / (double)count1;
		obj[1] = 0.5 * (yj - 2 * xreal[0] + 2.0) * xreal[1] + 2.0 * sum2 / (double)count2;
		obj[2] = 1.0 - xreal[1] + 2.0 * sum3 / (double)count3;
	}

	IMMOEA_F10::IMMOEA_F10(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = -2.0;
			upper_bound_[i] = 2.0;
		}

		lower_bound_[0] = 0.0;
		lower_bound_[1] = 0.0;
		upper_bound_[0] = 1.0;
		upper_bound_[1] = 1.0;
	}

	IMMOEA_F10::~IMMOEA_F10()
	{

	}

	void IMMOEA_F10::CalObj(Individual* ind)
	{
		int count1 = 0, count2 = 0, count3 = 0;
		double sum1 = 0.0, sum2 = 0.0, sum3 = 0.0, yj = 0.0, hj = 0.0;

		double* obj = ind->obj_;
		double* xreal = ind->dec_;

		for (int i = 3; i <= dec_num_; i++)
		{
			yj = xreal[i - 1] - 2.0 * xreal[1] * sin(2.0 * PI * xreal[0] + i * PI / dec_num_);
			hj = 4.0 * yj * yj - cos(8.0 * PI * yj) + 1.0;
			if (i % 3 == 1)
			{
				sum1 += hj;
				count1++;
			}
			else if (i % 3 == 2)
			{
				sum2 += hj;
				count2++;
			}
			else
			{
				sum3 += hj;
				count3++;
			}
		}

		obj[0] = cos(0.5 * PI * xreal[0]) * cos(0.5 * PI * xreal[1]) + 2.0 * sum1 / (double)count1;
		obj[1] = cos(0.5 * PI * xreal[0]) * sin(0.5 * PI * xreal[1]) + 2.0 * sum2 / (double)count2;
		obj[2] = sin(0.5 * PI * xreal[0]) + 2.0 * sum3 / (double)count3;
	}
}