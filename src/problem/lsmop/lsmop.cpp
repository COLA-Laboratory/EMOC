#include "problem/lsmop/lsmop.h"

#include <cmath>
#include <iostream>

#include "core/macro.h"

namespace emoc {

	LSMOP::LSMOP(int dec_num, int obj_num):Problem(dec_num,obj_num)
	{
		Init();

		for (int i = obj_num_ - 1; i < dec_num_; i++)
		{
			lower_bound_[i] = 0;
			upper_bound_[i] = 10;
		}
		for (int i = 0; i < obj_num_ - 1; i++)
		{
			lower_bound_[i] = 0;
			upper_bound_[i] = 1;
		}
	}

	LSMOP::~LSMOP()
	{
		delete[] c;
		delete[] g;
		delete[] sublen;
		delete[] len;
		delete[] variable;
		delete[] lsmop9_sum;
		c = nullptr;
		g = nullptr;
		sublen = nullptr;
		len = nullptr;
		variable = nullptr;
		lsmop9_sum = nullptr;
	}

	void LSMOP::Init()
	{
		// allocate memory
		c = new double[obj_num_];
		g = new double[obj_num_];
		lsmop9_sum = new double[obj_num_];
		sublen = new int[obj_num_];
		len = new int[obj_num_ + 1];
		variable = new double[dec_num_ + obj_num_];
		for(int i = 0; i < obj_num_; i++)
		{
			c[i] = 0; sublen[i] = 0; g[i] = 0; lsmop9_sum[i] = 0;
		}
		for(int i = 0; i <= obj_num_; i++)
			len[i] = 0;
		for(int i = 0; i < dec_num_ + obj_num_; i++)
			variable[i] = 0;

		double sum_c = 0;
		double c_temp = 3.8 * 0.1 * (1 - 0.1);

		c[0] = c_temp;
		sum_c += c[0];

		for(int i = 0; i < obj_num_ - 1; i++)
		{
			c[i + 1] = 3.8 * c[i] * (1 - c[i]);
			sum_c += c[i + 1];
		}

		for(int i = 0; i < obj_num_; i++)
		{
			sublen[i] = (int)((dec_num_ - obj_num_ + 1) / nk * (c[i] / sum_c));
		}

		int temp_sum = 0;
		len[0] = 0;
		for(int i = 0; i < obj_num_; i++)
		{
			temp_sum += sublen[i] * 5;
			len[i + 1] = temp_sum;
		}
	}

	double LSMOP::sphere(int start, int end)
	{
		double sum = 0;
		for(int i = start; i < end; i++)
			sum += (variable[i] * variable[i]);

		return  sum;
	}

	double LSMOP::schwefel_yita2(int start, int end)
	{
		double  max = -1000000;
		for(int i = start; i < end; i++)
		{
			if (max < fabs(variable[i]))
				max = fabs(variable[i]);
		}

		return  max;
	}

	double LSMOP::ronsenbrock_yita3(int start, int end)
	{
		double  sum = 0;
		for(int i = start; i < end - 1; i++)
			sum += (100 * pow((variable[i] * variable[i] - variable[i + 1]), 2.0) + (variable[i] - 1) * (variable[i] - 1));

		return  sum;
	}

	double LSMOP::rastrigin_yita4(int start, int end)
	{
		double  sum = 0;
		for(int i = start; i < end; i++)
		{
			sum += (variable[i] * variable[i] - 10 * cos(2 * PI * variable[i]) + 10);
		}

		return  sum;
	}

	double LSMOP::griewank_yita5(int start, int end)
	{
		double  sum = 0, product = 1;
		for(int i = start; i < end; i++)
		{
			sum += (variable[i] * variable[i] / 4000.0);
			product *= cos(variable[i] / sqrt((double)(i + 1 - start)));
		}

		return  sum - product + 1;

	}

	double LSMOP::ackley_yita6(int start, int end)
	{
		double  sum1 = 0, sum2 = 0;
		for(int i = start; i < end; i++)
		{
			sum1 += (variable[i] * variable[i]);
			sum2 += cos(2 * PI * variable[i]);
		}
		sum1 /= (double)(end - start);
		sum2 /= (double)(end - start);

		return  20 + exp(1) - 20 * exp(-0.2 * sqrt(sum1)) - exp(sum2);
	}

	double LSMOP::CalLSMOP1g(int index)
	{
		double sum = 0, temp = 0;

		if ((index + 1) % 2 == 0)
		{
			for(int i = 0; i < 5; i++)
			{
				temp = sphere(obj_num_ - 1 + len[index] + i * sublen[index], obj_num_ - 1 + len[index] + (i + 1) * sublen[index]);
				sum += temp / (double)sublen[index];
			}

		}
		else
		{
			for(int i = 0; i < 5; i++)
			{
				temp = sphere(obj_num_ - 1 + len[index] + i * sublen[index], obj_num_ - 1 + len[index] + (i + 1) * sublen[index]);
				sum += temp / (double)sublen[index];
			}
		}


		sum /= 5.0;
		return sum;
	}

	double LSMOP::CalLSMOP2g(int index)
	{
		double sum = 0, temp = 0;

		if ((index + 1) % 2 == 0)
		{
			for(int i = 0; i < 5; i++)
			{
				temp = schwefel_yita2(obj_num_ - 1 + len[index] + i * sublen[index], obj_num_ - 1 + len[index] + (i + 1) * sublen[index]);
				sum += temp / (double)sublen[index];
			}

		}
		else
		{
			for(int i = 0; i < 5; i++)
			{
				temp = griewank_yita5(obj_num_ - 1 + len[index] + i * sublen[index], obj_num_ - 1 + len[index] + (i + 1) * sublen[index]);
				sum += temp / (double)sublen[index];
			}
		}

		sum /= 5.0;
		return sum;
	}

	double LSMOP::CalLSMOP3g(int index)
	{
		int  M = obj_num_;
		double sum = 0, temp = 0;

		if ((index + 1) % 2 == 0)
		{
			for(int i = 0; i < 5; i++)
			{
				temp = ronsenbrock_yita3(M - 1 + len[index] + i * sublen[index], M - 1 + len[index] + (i + 1) * sublen[index]);
				sum += temp / (double)sublen[index];
			}

		}
		else
		{
			for(int i = 0; i < 5; i++)
			{
				temp = rastrigin_yita4(M - 1 + len[index] + i * sublen[index], M - 1 + len[index] + (i + 1) * sublen[index]);
				sum += temp / (double)sublen[index];
			}
		}

		sum /= 5.0;
		return sum;
	}

	double LSMOP::CalLSMOP4g(int index)
	{
		int  M = obj_num_;
		double sum = 0, temp = 0;

		if ((index + 1) % 2 == 0)
		{
			for(int i = 0; i < 5; i++)
			{
				temp = griewank_yita5(M - 1 + len[index] + i * sublen[index], M - 1 + len[index] + (i + 1) * sublen[index]);
				sum += temp / (double)sublen[index];
			}

		}
		else
		{
			for(int i = 0; i < 5; i++)
			{
				temp = ackley_yita6(M - 1 + len[index] + i * sublen[index], M - 1 + len[index] + (i + 1) * sublen[index]);
				sum += temp / (double)sublen[index];
			}
		}

		sum /= 5.0;
		return sum;
	}

	double LSMOP::CalLSMOP5g(int index)
	{
		int  M = obj_num_;
		double sum1 = 0, temp1 = 0, sum2 = 0, temp2 = 0;

		if ((index + 1) % 2 == 0)
		{
			for(int i = 0; i < 5; i++)
			{
				temp1 = sphere(M - 1 + len[index] + i * sublen[index], M - 1 + len[index] + (i + 1) * sublen[index]);
				sum1 += temp1 / (double)sublen[index];
			}

			if (index != obj_num_ - 1)
			{
				for(int i = 0; i < 5; i++)
				{
					temp2 = sphere(M - 1 + len[index + 1] + i * sublen[index + 1], M - 1 + len[index + 1] + (i + 1) * sublen[index + 1]);
					sum2 += temp2 / (double)sublen[index + 1];
				}
			}
		}
		else
		{
			for(int i = 0; i < 5; i++)
			{
				temp1 = sphere(M - 1 + len[index] + i * sublen[index], M - 1 + len[index] + (i + 1) * sublen[index]);
				sum1 += temp1 / (double)sublen[index];
			}

			if (index != obj_num_ - 1)
			{
				for(int i = 0; i < 5; i++)
				{
					temp2 = sphere(M - 1 + len[index + 1] + i * sublen[index + 1], M - 1 + len[index + 1] + (i + 1) * sublen[index + 1]);
					sum2 += temp2 / (double)sublen[index + 1];
				}
			}
		}

		sum1 /= 5.0; sum2 /= 5.0;
		return sum1 + sum2;
	}

	double LSMOP::CalLSMOP6g(int index)
	{
		int  M = obj_num_;
		double sum1 = 0, temp1 = 0, sum2 = 0, temp2 = 0;

		if ((index + 1) % 2 == 0)
		{
			for(int i = 0; i < 5; i++)
			{
				temp1 = schwefel_yita2(M - 1 + len[index] + i * sublen[index], M - 1 + len[index] + (i + 1) * sublen[index]);
				sum1 += temp1 / (double)sublen[index];
			}

			if (index != obj_num_ - 1)
			{
				for(int i = 0; i < 5; i++)
				{
					temp2 = ronsenbrock_yita3(M - 1 + len[index + 1] + i * sublen[index + 1], M - 1 + len[index + 1] + (i + 1) * sublen[index + 1]);
					sum2 += temp2 / (double)sublen[index + 1];
				}
			}
		}
		else
		{
			for(int i = 0; i < 5; i++)
			{
				temp1 = ronsenbrock_yita3(M - 1 + len[index] + i * sublen[index], M - 1 + len[index] + (i + 1) * sublen[index]);
				sum1 += temp1 / (double)sublen[index];
			}

			if (index != obj_num_ - 1)
			{
				for(int i = 0; i < 5; i++)
				{
					temp2 = schwefel_yita2(M - 1 + len[index + 1] + i * sublen[index + 1], M - 1 + len[index + 1] + (i + 1) * sublen[index + 1]);
					sum2 += temp2 / (double)sublen[index + 1];
				}
			}
		}

		sum1 /= 5.0; sum2 /= 5.0;
		return sum1 + sum2;
	}

	double LSMOP::CalLSMOP7g(int index)
	{
		int  M = obj_num_;
		double sum1 = 0, temp1 = 0, sum2 = 0, temp2 = 0;

		if ((index + 1) % 2 == 0)
		{
			for(int i = 0; i < 5; i++)
			{
				temp1 = ronsenbrock_yita3(M - 1 + len[index] + i * sublen[index], M - 1 + len[index] + (i + 1) * sublen[index]);
				sum1 += temp1 / (double)sublen[index];
			}

			if (index != obj_num_ - 1)
			{
				for(int i = 0; i < 5; i++)
				{
					temp2 = ackley_yita6(M - 1 + len[index + 1] + i * sublen[index + 1], M - 1 + len[index + 1] + (i + 1) * sublen[index + 1]);
					sum2 += temp2 / (double)sublen[index + 1];
				}
			}
		}
		else
		{
			for(int i = 0; i < 5; i++)
			{
				temp1 = ackley_yita6(M - 1 + len[index] + i * sublen[index], M - 1 + len[index] + (i + 1) * sublen[index]);
				sum1 += temp1 / (double)sublen[index];
			}

			if (index != obj_num_ - 1)
			{
				for(int i = 0; i < 5; i++)
				{
					temp2 = ronsenbrock_yita3(M - 1 + len[index + 1] + i * sublen[index + 1], M - 1 + len[index + 1] + (i + 1) * sublen[index + 1]);
					sum2 += temp2 / (double)sublen[index + 1];
				}
			}
		}

		sum1 /= 5.0; sum2 /= 5.0;
		return sum1 + sum2;
	}

	double LSMOP::CalLSMOP8g(int index)
	{
		int  M = obj_num_;
		double sum1 = 0, temp1 = 0, sum2 = 0, temp2 = 0;

		if ((index + 1) % 2 == 0)
		{
			for(int i = 0; i < 5; i++)
			{
				temp1 = sphere(M - 1 + len[index] + i * sublen[index], M - 1 + len[index] + (i + 1) * sublen[index]);
				sum1 += temp1 / (double)sublen[index];
			}

			if (index != obj_num_ - 1)
			{
				for(int i = 0; i < 5; i++)
				{
					temp2 = griewank_yita5(M - 1 + len[index + 1] + i * sublen[index + 1], M - 1 + len[index + 1] + (i + 1) * sublen[index + 1]);
					sum2 += temp2 / (double)sublen[index + 1];
				}
			}
		}
		else
		{
			for(int i = 0; i < 5; i++)
			{
				temp1 = griewank_yita5(M - 1 + len[index] + i * sublen[index], M - 1 + len[index] + (i + 1) * sublen[index]);
				sum1 += temp1 / (double)sublen[index];
			}

			if (index != obj_num_ - 1)
			{
				for(int i = 0; i < 5; i++)
				{
					temp2 = sphere(M - 1 + len[index + 1] + i * sublen[index + 1], M - 1 + len[index + 1] + (i + 1) * sublen[index + 1]);
					sum2 += temp2 / (double)sublen[index + 1];
				}
			}
		}

		sum1 /= 5.0; sum2 /= 5.0;
		return sum1 + sum2;
	}

	double LSMOP::CalLSMOP9g(int index)
	{
		int  M = obj_num_;
		double temp = 0, total_sum = 0;

		for (int j = 0; j < M; j++)
			lsmop9_sum[j] = 0;

		for (int j = 0; j < M; j++)
		{
			if ((j + 1) % 2 == 0)
			{
				for(int i = 0; i < 5; i++)
				{
					temp = ackley_yita6(M - 1 + len[j] + i * sublen[j], M - 1 + len[j] + (i + 1) * sublen[j]);
					lsmop9_sum[j] += temp / (double)sublen[j];
				}
			}
			else
			{
				for(int i = 0; i < 5; i++)
				{
					temp = sphere(M - 1 + len[j] + i * sublen[j], M - 1 + len[j] + (i + 1) * sublen[j]);
					lsmop9_sum[j] += temp / (double)sublen[j];
				}
			}
		}

		for (int j = 0; j < M; j++)
			total_sum += lsmop9_sum[j] / 5.0;
		
		return total_sum;
	}

	LSMOP1::LSMOP1(int dec_num, int obj_num):LSMOP(dec_num, obj_num)
	{

	}

	LSMOP1::~LSMOP1()
	{

	}

	void LSMOP1::CalObj(Individual* ind)
	{
		int i = 0, j = 0, k = 0, index;
		double temp = 0;
		int D = len[obj_num_];
		int xs_num = dec_num_;

		double* xreal, * obj;
		obj = ind->obj_.data();
		xreal = ind->dec_.data();

		//linkage
		int temp_index = obj_num_;
		for (i = 0; i < D; i++)
		{
			temp = (1 + (double)temp_index / xs_num) * xreal[i + obj_num_ - 1] - xreal[0] * 10;
			variable[i + obj_num_ - 1] = temp;
			temp_index++;
		}

		for (i = 0; i < obj_num_; i++)
		{
			g[i] = CalLSMOP1g(i);
		}

		for (i = 0; i < obj_num_; i++)
			obj[i] = g[i] + 1;

		for (i = 0; i < obj_num_; i++)
		{
			for (j = 0; j < obj_num_ - (i + 1); j++)
				obj[i] *= xreal[j];
			if (i != 0)
			{
				index = obj_num_ - (i + 1);
				obj[i] *= 1 - xreal[index];
			}
		}
	}

	LSMOP2::LSMOP2(int dec_num, int obj_num) :LSMOP(dec_num, obj_num)
	{

	}

	LSMOP2::~LSMOP2()
	{

	}

	void LSMOP2::CalObj(Individual* ind)
	{
		int i = 0, j = 0, k = 0, index;
		double temp = 0;
		int D = len[obj_num_];
		int xs_num = dec_num_;

		double* xreal, * obj;
		obj = ind->obj_.data();
		xreal = ind->dec_.data();

		//linkage
		int temp_index = obj_num_;

		for (i = 0; i < D; i++)
		{
			temp = (1 + (double)temp_index / xs_num) * xreal[i + obj_num_ - 1] - xreal[0] * 10;
			variable[i + obj_num_ - 1] = temp;

			temp_index++;
		}


		for (i = 0; i < obj_num_; i++)
		{
			g[i] = CalLSMOP2g(i);
		}




		for (i = 0; i < obj_num_; i++)
			obj[i] = g[i] + 1;

		for (i = 0; i < obj_num_; i++)
		{
			for (j = 0; j < obj_num_ - (i + 1); j++)
				obj[i] *= xreal[j];
			if (i != 0)
			{
				index = obj_num_ - (i + 1);
				obj[i] *= 1 - xreal[index];
			}
		}
	}

	LSMOP3::LSMOP3(int dec_num, int obj_num) :LSMOP(dec_num, obj_num)
	{

	}

	LSMOP3::~LSMOP3()
	{

	}

	void LSMOP3::CalObj(Individual* ind)
	{
		int i = 0, j = 0, k = 0, index;
		double temp = 0;
		int D = len[obj_num_];
		int xs_num = dec_num_;

		double* xreal, * obj;
		obj = ind->obj_.data();
		xreal = ind->dec_.data();

		//linkage
		int temp_index = obj_num_;

		for (i = 0; i < D; i++)
		{
			temp = (1 + (double)temp_index / xs_num) * xreal[i + obj_num_ - 1] - xreal[0] * 10;
			variable[i + obj_num_ - 1] = temp;

			temp_index++;
		}

		for (i = 0; i < obj_num_; i++)
		{
			g[i] = CalLSMOP3g(i);
		}

		for (i = 0; i < obj_num_; i++)
			obj[i] = g[i] + 1;

		for (i = 0; i < obj_num_; i++)
		{
			for (j = 0; j < obj_num_ - (i + 1); j++)
				obj[i] *= xreal[j];
			if (i != 0)
			{
				index = obj_num_ - (i + 1);
				obj[i] *= 1 - xreal[index];
			}
		}
	}

	LSMOP4::LSMOP4(int dec_num, int obj_num) :LSMOP(dec_num, obj_num)
	{

	}

	LSMOP4::~LSMOP4()
	{

	}

	void LSMOP4::CalObj(Individual* ind)
	{
		int i = 0, j = 0, k = 0, index;
		double temp = 0;
		int D = len[obj_num_];
		int xs_num = dec_num_;

		double* xreal, * obj;
		obj = ind->obj_.data();
		xreal = ind->dec_.data();

		//linkage
		int temp_index = obj_num_;

		for (i = 0; i < D; i++)
		{
			temp = (1 + (double)temp_index / xs_num) * xreal[i + obj_num_ - 1] - xreal[0] * 10;
			variable[i + obj_num_ - 1] = temp;

			temp_index++;
		}

		for (i = 0; i < obj_num_; i++)
		{
			g[i] = CalLSMOP4g(i);
		}

		for (i = 0; i < obj_num_; i++)
			obj[i] = g[i] + 1;

		for (i = 0; i < obj_num_; i++)
		{
			for (j = 0; j < obj_num_ - (i + 1); j++)
				obj[i] *= xreal[j];
			if (i != 0)
			{
				index = obj_num_ - (i + 1);
				obj[i] *= 1 - xreal[index];
			}
		}
	}

	LSMOP5::LSMOP5(int dec_num, int obj_num) :LSMOP(dec_num, obj_num)
	{

	}

	LSMOP5::~LSMOP5()
	{

	}

	void LSMOP5::CalObj(Individual* ind)
	{
		int i = 0, j = 0, k = 0, index;
		double temp = 0;
		int D = len[obj_num_];
		int xs_num = dec_num_;

		double* xreal, * obj;
		obj = ind->obj_.data();
		xreal = ind->dec_.data();

		//linkage
		int temp_index = obj_num_;

		for (i = 0; i < D; i++)
		{
			temp = (1 + cos(0.5 * PI * (double)temp_index / (double)xs_num)) * xreal[i + obj_num_ - 1] - xreal[0] * 10;
			variable[i + obj_num_ - 1] = temp;

			temp_index++;
		}

		for (i = 0; i < obj_num_; i++)
		{
			g[i] = CalLSMOP5g(i);
		}

		for (i = 0; i < obj_num_; i++)
			obj[i] = g[i] + 1;

		for (i = 0; i < obj_num_; i++)
		{
			for (j = 0; j < obj_num_ - (i + 1); j++)
				obj[i] *= cos(PI * 0.5 * xreal[j]);
			if (i != 0)
			{
				index = obj_num_ - (i + 1);
				obj[i] *= sin(PI * 0.5 * xreal[index]);
			}
		}
	}

	LSMOP6::LSMOP6(int dec_num, int obj_num) :LSMOP(dec_num, obj_num)
	{

	}

	LSMOP6::~LSMOP6()
	{

	}

	void LSMOP6::CalObj(Individual* ind)
	{
		int i = 0, j = 0, k = 0, index;
		double temp = 0;
		int D = len[obj_num_];
		int xs_num = dec_num_;

		double* xreal, * obj;
		obj = ind->obj_.data();
		xreal = ind->dec_.data();

		//linkage
		int temp_index = obj_num_;

		for (i = 0; i < D; i++)
		{
			temp = (1 + cos(0.5 * PI * (double)temp_index / (double)xs_num)) * xreal[i + obj_num_ - 1] - xreal[0] * 10;
			variable[i + obj_num_ - 1] = temp;

			temp_index++;
		}

		for (i = 0; i < obj_num_; i++)
		{
			g[i] = CalLSMOP6g(i);
		}

		for (i = 0; i < obj_num_; i++)
			obj[i] = g[i] + 1;

		for (i = 0; i < obj_num_; i++)
		{
			for (j = 0; j < obj_num_ - (i + 1); j++)
				obj[i] *= cos(PI * 0.5 * xreal[j]);
			if (i != 0)
			{
				index = obj_num_ - (i + 1);
				obj[i] *= sin(PI * 0.5 * xreal[index]);
			}
		}
	}

	LSMOP7::LSMOP7(int dec_num, int obj_num) :LSMOP(dec_num, obj_num)
	{

	}

	LSMOP7::~LSMOP7()
	{

	}

	void LSMOP7::CalObj(Individual* ind)
	{
		int i = 0, j = 0, k = 0, index;
		double temp = 0;
		int D = len[obj_num_];
		int xs_num = dec_num_;

		double* xreal, * obj;
		obj = ind->obj_.data();
		xreal = ind->dec_.data();

		//linkage
		int temp_index = obj_num_;

		for (i = 0; i < D; i++)
		{
			temp = (1 + cos(0.5 * PI * (double)temp_index / (double)xs_num)) * xreal[i + obj_num_ - 1] - xreal[0] * 10;
			variable[i + obj_num_ - 1] = temp;

			temp_index++;
		}

		for (i = 0; i < obj_num_; i++)
		{
			g[i] = CalLSMOP7g(i);
		}

		for (i = 0; i < obj_num_; i++)
			obj[i] = g[i] + 1;

		for (i = 0; i < obj_num_; i++)
		{
			for (j = 0; j < obj_num_ - (i + 1); j++)
				obj[i] *= cos(PI * 0.5 * xreal[j]);
			if (i != 0)
			{
				index = obj_num_ - (i + 1);
				obj[i] *= sin(PI * 0.5 * xreal[index]);
			}
		}
	}

	LSMOP8::LSMOP8(int dec_num, int obj_num) :LSMOP(dec_num, obj_num)
	{
	}

	LSMOP8::~LSMOP8()
	{

	}

	void LSMOP8::CalObj(Individual* ind)
	{

		int i = 0, j = 0, k = 0, index;
		double temp = 0;
		int D = len[obj_num_];
		int xs_num = dec_num_;

		double* xreal, * obj;
		obj = ind->obj_.data();
		xreal = ind->dec_.data();

		//linkage
		int temp_index = obj_num_;

		for (i = 0; i < D; i++)
		{
			temp = (1 + cos(0.5 * PI * (double)temp_index / (double)xs_num)) * xreal[i + obj_num_ - 1] - xreal[0] * 10;
			variable[i + obj_num_ - 1] = temp;
			//        printf("%f ",variable[i]);
			temp_index++;
		}

		for (i = 0; i < obj_num_; i++)
		{
			g[i] = CalLSMOP8g(i);
		}

		for (i = 0; i < obj_num_; i++)
			obj[i] = g[i] + 1;

		//    printf("%f %f\n\n\n\n",obj[0],obj[1]);

		for (i = 0; i < obj_num_; i++)
		{
			for (j = 0; j < obj_num_ - (i + 1); j++)
				obj[i] *= cos(PI * 0.5 * xreal[j]);
			if (i != 0)
			{
				index = obj_num_ - (i + 1);
				obj[i] *= sin(PI * 0.5 * xreal[index]);
			}
		}
	}

	LSMOP9::LSMOP9(int dec_num, int obj_num) :LSMOP(dec_num, obj_num)
	{

	}

	LSMOP9::~LSMOP9()
	{

	}

	void LSMOP9::CalObj(Individual* ind)
	{
		int i = 0, j = 0, k = 0, index;
		double temp = 0;
		int D = len[obj_num_];
		int xs_num = dec_num_;

		double* xreal, * obj;
		obj = ind->obj_.data();
		xreal = ind->dec_.data();

		//linkage
		int temp_index = obj_num_;

		for (i = 0; i < D; i++)
		{
			temp = (1 + cos(0.5 * PI * (double)temp_index / (double)xs_num)) * xreal[i + obj_num_ - 1] - xreal[0] * 10;
			variable[i + obj_num_ - 1] = temp;

			temp_index++;
		}


		g[0] = CalLSMOP9g(obj_num_ - 1);


		for (i = 0; i < obj_num_; i++)
			obj[i] = xreal[i];

		double h = 0;
		for (i = 0; i < obj_num_ - 1; i++)
		{
			h += xreal[i] * (1.0 + sin(3.0 * PI * xreal[i])) / (2 + g[0]);
		}

		h = obj_num_ - h;

		obj[obj_num_ - 1] = h * (2 + g[0]);
	}

}