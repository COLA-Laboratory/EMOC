#include "problem/bt/bt.h"

#include <cmath>

#include "core/macro.h"
#include "core/global.h"

namespace emoc {

	BT1::BT1(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	BT1::~BT1()
	{

	}

	void BT1::CalObj(Individual* ind)
	{
		int i = 0;
		double sum1 = 0, sum2 = 0;
		double y, temp;
		int n = dec_num_;

		double ep = 1e-10;
		for (i = 1; i < dec_num_; i++)
		{
			y = ind->dec_[i] - sin((i + 1) * PI / (double)(2 * n));
			temp = y * y + (1 - exp(-y * y / ep)) / 5.0; //D1

			if ((i + 1) % 2 == 0)
			{
				sum1 += temp;
			}
			else
			{
				sum2 += temp;
			}
		}

		ind->obj_[0] = ind->dec_[0] + sum1;
		ind->obj_[1] = 1 - sqrt(ind->dec_[0]) + sum2;
	}

	BT2::BT2(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	BT2::~BT2()
	{

	}

	void BT2::CalObj(Individual* ind)
	{
		int i = 0;
		double sum1 = 0, sum2 = 0;
		double y, temp;
		int n = dec_num_;

		double ep = 0.2;
		for (i = 1; i < dec_num_; i++)
		{
			y = ind->dec_[i] - sin((i + 1) * PI / (double)(2 * n));
			temp = y * y + pow(fabs(y), ep) / 5.0; //D2
			if ((i + 1) % 2 == 0)
			{
				sum1 += temp;
			}
			else
			{
				sum2 += temp;
			}
		}

		ind->obj_[0] = ind->dec_[0] + sum1;
		ind->obj_[1] = 1 - sqrt(ind->dec_[0]) + sum2;
	}

	BT3::BT3(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	BT3::~BT3()
	{

	}

	void BT3::CalObj(Individual* ind)
	{
		int i = 0;
		double sum1 = 0, sum2 = 0;
		double y, temp;
		int n = dec_num_;

		double ep = 1e-8;
		for (i = 1; i < dec_num_; i++)
		{
			y = ind->dec_[i] - sin((i + 1) * PI / (double)(2 * n));
			temp = y * y + (1 - exp(-y * y / ep)) / 5.0; //D1
			if ((i + 1) % 2 == 0)
			{
				sum1 += temp;
			}
			else
			{
				sum2 += temp;
			}
		}

		double gamma = 0.02;
		double S1 = pow(fabs(ind->dec_[0]), gamma);


		ind->obj_[0] = S1 + sum1;
		ind->obj_[1] = 1 - sqrt(S1) + sum2;
	}

	BT4::BT4(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	BT4::~BT4()
	{

	}

	void BT4::CalObj(Individual* ind)
	{
		int i = 0;
		double sum1 = 0, sum2 = 0;
		double y, temp;
		int n = dec_num_;

		double ep = 1e-8;
		for (i = 1; i < dec_num_; i++)
		{
			y = ind->dec_[i] - sin((i + 1) * PI / (double)(2 * n));
			temp = y * y + (1 - exp(-y * y / ep)) / 5.0; //D1
			if ((i + 1) % 2 == 0)
			{
				sum1 += temp;
			}
			else
			{
				sum2 += temp;
			}
		}

		double gamma = 0.06;
		double S2 = 0;
		double x0 = ind->dec_[0];

		if (x0 >= 0 && x0 < 0.25)
		{
			S2 = (1 - pow((1 - 4 * x0), gamma)) / 4.0;
		}
		else if (x0 >= 0.25 && x0 < 0.5)
		{
			S2 = (1 + pow((4 * x0 - 1), gamma)) / 4.0;
		}
		else if (x0 >= 0.5 && x0 < 0.75)
		{
			S2 = (3 - pow((3 - 4 * x0), gamma)) / 4.0;
		}
		else if (x0 >= 0.75 && x0 < 1)
		{
			S2 = (3 + pow((4 * x0 - 3), gamma)) / 4.0;
		}

		ind->obj_[0] = S2 + sum1;
		ind->obj_[1] = 1 - sqrt(S2) + sum2;
	}

	BT5::BT5(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	BT5::~BT5()
	{

	}

	void BT5::CalObj(Individual* ind)
	{
		int i = 0;
		double sum1 = 0, sum2 = 0;
		double y, temp;
		int n = dec_num_;

		double ep = 1e-10;
		for (i = 1; i < dec_num_; i++)
		{
			y = ind->dec_[i] - sin((i + 1) * PI / (double)(2 * n));
			temp = y * y + (1 - exp(-y * y / ep)) / 5.0; //D1
			if ((i + 1) % 2 == 0)
			{
				sum1 += temp;
			}
			else
			{
				sum2 += temp;
			}
		}

		double x0 = ind->dec_[0];
		ind->obj_[0] = x0 + sum1;
		ind->obj_[1] = (1 - x0) * (1 - x0 * sin(8.5 * PI * x0)) + sum2;
	}

	BT6::BT6(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	BT6::~BT6()
	{

	}

	void BT6::CalObj(Individual* ind)
	{
		int i = 0;
		double sum1 = 0, sum2 = 0;
		double y, temp;
		int n = dec_num_;

		double ep = 1e-4;
		for (i = 1; i < dec_num_; i++)
		{
			y = ind->dec_[i] - pow(ind->dec_[0], (0.5 + 1.5 * i / (double)(n - 1)));
			temp = y * y + (1 - exp(-y * y / ep)) / 5.0; //D1
			if ((i + 1) % 2 == 0)
			{
				sum1 += temp;
			}
			else
			{
				sum2 += temp;
			}
		}

		double x0 = ind->dec_[0];
		ind->obj_[0] = x0 + sum1;
		ind->obj_[1] = 1 - sqrt(x0) + sum2;
	}

	BT7::BT7(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = -1.0;
			upper_bound_[i] = 1.0;
		}

		lower_bound_[0] = 0.0;
	}

	BT7::~BT7()
	{

	}

	void BT7::CalObj(Individual* ind)
	{
		int i = 0;
		double sum1 = 0, sum2 = 0;
		double y, temp;
		int n = dec_num_;

		double ep = 1e-3;
		for (i = 1; i < dec_num_; i++)
		{
			y = ind->dec_[i] - sin(6 * PI * ind->dec_[0]);
			temp = y * y + (1 - exp(-y * y / ep)) / 5.0; //D1
			if ((i + 1) % 2 == 0)
			{
				sum1 += temp;
			}
			else
			{
				sum2 += temp;
			}
		}

		double x0 = ind->dec_[0];
		ind->obj_[0] = x0 + sum1;
		ind->obj_[1] = 1 - sqrt(x0) + sum2;
	}


	BT8::BT8(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	BT8::~BT8()
	{

	}

	void BT8::CalObj(Individual* ind)
	{
		int i = 0;
		double sum1 = 0, sum2 = 0;
		double y, temp;
		int n = dec_num_;

		double ep = 1e-3;
		for (i = 1; i < dec_num_; i++)
		{
			y = ind->dec_[i] - pow(ind->dec_[0], (0.5 + 1.5 * i / (double)(n - 1)));
			temp = y * y + (1 - exp(-y * y / ep)) / 5.0; //D1
			temp = 4 * temp * temp - cos(8 * PI * temp) + 1;
			if ((i + 1) % 2 == 0)
			{
				sum1 += temp;
			}
			else
			{
				sum2 += temp;
			}
		}

		double x0 = ind->dec_[0];
		ind->obj_[0] = x0 + sum1;
		ind->obj_[1] = 1 - sqrt(x0) + sum2;
	}

	BT9::BT9(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	BT9::~BT9()
	{

	}

	void BT9::CalObj(Individual* ind)
	{
		int i = 0;
		double sum1 = 0, sum2 = 0, sum3 = 0;
		double y, temp;
		int n = dec_num_;

		double ep = 1e-9;
		for (i = 2; i < dec_num_; i++)
		{
			y = ind->dec_[i] - sin((double)(i + 1) * PI / (double)(2 * n));
			temp = y * y + (1 - exp(-y * y / ep)) / 5.0; //D1
			if ((i + 1) % 3 == 0)
			{
				sum1 += temp;
			}
			else if ((i + 1) % 3 == 1)
			{
				sum2 += temp;
			}
			else
			{
				sum3 += temp;
			}
		}

		double x0 = ind->dec_[0];
		double x1 = ind->dec_[1];
		ind->obj_[0] = cos(0.5 * x0 * PI) * cos(0.5 * x1 * PI) + 10 * sum1;
		ind->obj_[1] = cos(0.5 * x0 * PI) * sin(0.5 * x1 * PI) + 10 * sum2;
		ind->obj_[2] = sin(0.5 * x0 * PI) + 10 * sum3;
	}
}