#include "problem/moeadde_f/moeadde_f.h"

#include <cmath>

#include "core/macro.h"

namespace emoc{

	MOEADDE_F1::MOEADDE_F1(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	MOEADDE_F1::~MOEADDE_F1()
	{

	}

	void MOEADDE_F1::CalObj(Individual* ind)
	{
		int i = 0;
		double sum1 = 0, sum2 = 0;
		int count1 = 0, count2 = 0;
		int n = dec_num_;

		for (i = 1; i < dec_num_; i++)
		{
			if (i % 2 == 0)
			{
				sum1 += pow(ind->dec_[i] - pow(ind->dec_[0], 0.5 * (1.0 + (double)(3.0 * (i + 1 - 2)) / (double)(n - 2))), 2.0);
				count1++;
			}
			else
			{
				sum2 += pow(ind->dec_[i] - pow(ind->dec_[0], 0.5 * (1.0 + (double)(3.0 * (i + 1 - 2)) / (double)(n - 2))), 2.0);
				count2++;
			}
		}

		ind->obj_[0] = ind->dec_[0] + sum1 * 2.0 / (double)count1;
		ind->obj_[1] = 1 - sqrt(ind->dec_[0]) + sum2 * 2.0 / (double)count2;
	}

	MOEADDE_F2::MOEADDE_F2(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = -1.0;
			upper_bound_[i] = 1.0;
		}

		lower_bound_[0] = 0.0;
	}

	MOEADDE_F2::~MOEADDE_F2()
	{

	}

	void MOEADDE_F2::CalObj(Individual* ind)
	{
		int i = 0;
		double sum1 = 0, sum2 = 0;
		int count1 = 0, count2 = 0;
		int n = dec_num_;

		for (i = 1; i < dec_num_; i++)
		{
			if (i % 2 == 0)
			{
				sum1 += pow(ind->dec_[i] - sin(6 * PI * ind->dec_[0] + (double)(i + 1) * PI / (double)n), 2.0);
				count1++;
			}
			else
			{
				sum2 += pow(ind->dec_[i] - sin(6 * PI * ind->dec_[0] + (double)(i + 1) * PI / (double)n), 2.0);
				count2++;
			}
		}

		ind->obj_[0] = ind->dec_[0] + sum1 * 2.0 / (double)count1;
		ind->obj_[1] = 1 - sqrt(ind->dec_[0]) + sum2 * 2.0 / (double)count2;
	}

	MOEADDE_F3::MOEADDE_F3(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = -1.0;
			upper_bound_[i] = 1.0;
		}

		lower_bound_[0] = 0.0;
	}

	MOEADDE_F3::~MOEADDE_F3()
	{

	}

	void MOEADDE_F3::CalObj(Individual* ind)
	{
		int i = 0;
		double sum1 = 0, sum2 = 0;
		int count1 = 0, count2 = 0;
		int n = dec_num_;

		for (i = 1; i < dec_num_; i++)
		{
			if (i % 2 == 0)
			{
				sum1 += pow(ind->dec_[i] - 0.8 * ind->dec_[0] * cos(6 * PI * ind->dec_[0] + (double)(i + 1) * PI / (double)n), 2.0);
				count1++;
			}
			else
			{
				sum2 += pow(ind->dec_[i] - 0.8 * ind->dec_[0] * sin(6 * PI * ind->dec_[0] + (double)(i + 1) * PI / (double)n), 2.0);
				count2++;
			}
		}

		ind->obj_[0] = ind->dec_[0] + sum1 * 2.0 / (double)count1;
		ind->obj_[1] = 1 - sqrt(ind->dec_[0]) + sum2 * 2.0 / (double)count2;
	}

	MOEADDE_F4::MOEADDE_F4(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = -1.0;
			upper_bound_[i] = 1.0;
		}

		lower_bound_[0] = 0.0;
	}

	MOEADDE_F4::~MOEADDE_F4()
	{

	}

	void MOEADDE_F4::CalObj(Individual* ind)
	{
		int i = 0;
		double sum1 = 0, sum2 = 0;
		int count1 = 0, count2 = 0;
		int n = dec_num_;

		for (i = 1; i < dec_num_; i++)
		{
			if (i % 2 == 0)
			{
				sum1 += pow(ind->dec_[i] - 0.8 * ind->dec_[0] * cos((6 * PI * ind->dec_[0] + (double)(i + 1) * PI / (double)n) / (double)3), 2.0);
				count1++;
			}
			else
			{
				sum2 += pow(ind->dec_[i] - 0.8 * ind->dec_[0] * sin(6 * PI * ind->dec_[0] + (double)(i + 1) * PI / (double)n), 2.0);
				count2++;
			}
		}

		ind->obj_[0] = ind->dec_[0] + sum1 * 2.0 / (double)count1;
		ind->obj_[1] = 1 - sqrt(ind->dec_[0]) + sum2 * 2.0 / (double)count2;
	}

	MOEADDE_F5::MOEADDE_F5(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = -1.0;
			upper_bound_[i] = 1.0;
		}

		lower_bound_[0] = 0.0;
	}

	MOEADDE_F5::~MOEADDE_F5()
	{

	}

	void MOEADDE_F5::CalObj(Individual* ind)
	{
		int i = 0;
		double sum1 = 0, sum2 = 0;
		int count1 = 0, count2 = 0;
		int n = dec_num_;

		double temp1 = 0, temp2 = 0;
		double x1 = ind->dec_[0];

		for (i = 1; i < dec_num_; i++)
		{
			if (i % 2 == 0)
			{
				temp1 = ind->dec_[i] - (0.3 * x1 * x1 * cos(24 * PI * x1 + (double)(4 * (i + 1) * PI) / (double)n) + 0.6 * x1) * cos(6 * PI * x1 + (double)(i + 1) * PI / (double)n);
				sum1 += pow(temp1, 2.0);
				count1++;
			}
			else
			{
				temp2 = ind->dec_[i] - (0.3 * x1 * x1 * cos(24 * PI * x1 + (double)(4 * (i + 1) * PI) / (double)n) + 0.6 * x1) * sin(6 * PI * x1 + (double)(i + 1) * PI / (double)n);
				sum2 += pow(temp2, 2.0);
				count2++;
			}
		}

		ind->obj_[0] = ind->dec_[0] + sum1 * 2.0 / (double)count1;
		ind->obj_[1] = 1 - sqrt(ind->dec_[0]) + sum2 * 2.0 / (double)count2;
	}

	MOEADDE_F6::MOEADDE_F6(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 2; i < dec_num; ++i)
		{
			lower_bound_[i] = -2.0;
			upper_bound_[i] = 2.0;
		}
		for (int i = 0; i < 2; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	MOEADDE_F6::~MOEADDE_F6()
	{

	}

	void MOEADDE_F6::CalObj(Individual* ind)
	{
		int i = 0;
		double sum1 = 0, sum2 = 0, sum3 = 0;
		int count1 = 0, count2 = 0, count3 = 0;
		int n = dec_num_;
		double x1 = ind->dec_[0];
		double x2 = ind->dec_[1];

		for (i = 2; i < dec_num_; i++)
		{
			if (i % 3 == 1)
			{
				sum1 += pow(ind->dec_[i] - 2 * x2 * sin(2 * PI * ind->dec_[0] + (double)(i + 1) * PI / (double)n), 2.0);
				count1++;
			}
			else if (i % 3 == 2)
			{
				sum2 += pow(ind->dec_[i] - 2 * x2 * sin(2 * PI * ind->dec_[0] + (double)(i + 1) * PI / (double)n), 2.0);
				count2++;
			}
			else
			{
				sum3 += pow(ind->dec_[i] - 2 * x2 * sin(2 * PI * ind->dec_[0] + (double)(i + 1) * PI / (double)n), 2.0);
				count3++;
			}
		}

		ind->obj_[0] = cos(0.5 * x1 * PI) * cos(0.5 * x2 * PI) + sum1 * 2.0 / (double)count1;
		ind->obj_[1] = cos(0.5 * x1 * PI) * sin(0.5 * x2 * PI) + sum2 * 2.0 / (double)count2;
		ind->obj_[2] = sin(0.5 * x1 * PI) + sum3 * 2.0 / (double)count3;
	}

	MOEADDE_F7::MOEADDE_F7(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	MOEADDE_F7::~MOEADDE_F7()
	{

	}

	void MOEADDE_F7::CalObj(Individual* ind)
	{
		int i = 0;
		double sum1 = 0, sum2 = 0;
		int count1 = 0, count2 = 0;
		int n = dec_num_;

		double temp1 = 0, temp2 = 0;
		double x1 = ind->dec_[0];
		double y = 0;
		for (i = 1; i < dec_num_; i++)
		{
			if (i % 2 == 0)
			{
				y = ind->dec_[i] - pow(x1, 0.5 * (1.0 + (double)(3 * (i + 1 - 2)) / (double)(n - 2)));
				temp1 = 4 * pow(y, 2.0) - cos(8 * y * PI) + 1.0;
				sum1 += temp1;
				count1++;
			}
			else
			{
				y = ind->dec_[i] - pow(x1, 0.5 * (1.0 + (double)(3 * (i + 1 - 2)) / (double)(n - 2)));
				temp2 = 4 * pow(y, 2.0) - cos(8 * y * PI) + 1.0;
				sum2 += temp2;
				count2++;
			}
		}

		ind->obj_[0] = ind->dec_[0] + sum1 * 2.0 / (double)count1;
		ind->obj_[1] = 1 - sqrt(ind->dec_[0]) + sum2 * 2.0 / (double)count2;
	}


	MOEADDE_F8::MOEADDE_F8(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	MOEADDE_F8::~MOEADDE_F8()
	{

	}

	void MOEADDE_F8::CalObj(Individual* ind)
	{
		int i = 0;
		double sum1 = 0, sum2 = 0;
		int count1 = 0, count2 = 0;
		int n = dec_num_;
		double temp1 = 0, temp2 = 0;
		double x1 = ind->dec_[0];
		double multi1 = 1, multi2 = 1;
		double y = 0;

		for (i = 1; i < dec_num_; i++)
		{
			if (i % 2 == 0)
			{
				y = ind->dec_[i] - pow(x1, 0.5 * (1.0 + (double)(3 * (i + 1 - 2)) / (double)(n - 2)));
				sum1 += y * y;
				multi1 *= cos(20 * y * PI / sqrt((double)(i + 1)));
				count1++;
			}
			else
			{
				y = ind->dec_[i] - pow(x1, 0.5 * (1.0 + (double)(3 * (i + 1 - 2)) / (double)(n - 2)));
				sum2 += y * y;
				multi1 *= cos(20 * y * PI / sqrt((double)(i + 1)));
				count2++;
			}
		}

		sum1 *= 4; sum2 *= 4;
		multi1 *= 2; multi2 *= 2;
		sum1 -= multi1; sum2 -= multi2;
		sum1 += 2; sum2 += 2;

		ind->obj_[0] = ind->dec_[0] + sum1 * 2.0 / (double)count1;
		ind->obj_[1] = 1 - sqrt(ind->dec_[0]) + sum2 * 2.0 / (double)count2;
	}

	MOEADDE_F9::MOEADDE_F9(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = -1.0;
			upper_bound_[i] = 1.0;
		}
		lower_bound_[0] = 0.0;
	}

	MOEADDE_F9::~MOEADDE_F9()
	{

	}

	void MOEADDE_F9::CalObj(Individual* ind)
	{
		int i = 0;
		double sum1 = 0, sum2 = 0;
		int count1 = 0, count2 = 0;
		int n = dec_num_;

		for (i = 1; i < dec_num_; i++)
		{
			if (i % 2 == 0)
			{
				sum1 += pow(ind->dec_[i] - sin(6 * PI * ind->dec_[0] + (double)(i + 1) * PI / (double)n), 2.0);
				count1++;
			}
			else
			{
				sum2 += pow(ind->dec_[i] - sin(6 * PI * ind->dec_[0] + (double)(i + 1) * PI / (double)n), 2.0);
				count2++;
			}
		}

		ind->obj_[0] = ind->dec_[0] + sum1 * 2.0 / (double)count1;
		ind->obj_[1] = 1 - pow(ind->dec_[0], 2.0) + sum2 * 2.0 / (double)count2;
	}

}