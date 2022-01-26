#include "problem/dtlz.h"

#include <cmath>

#include "core/macro.h"
#include "core/global.h"

namespace emoc {

	DTLZ1::DTLZ1(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	DTLZ1::~DTLZ1()
	{

	}

	void DTLZ1::CalObj(Individual *ind)
	{
		double gx = 0.0;
		int k = dec_num_ - obj_num_ + 1;

		for (int i = dec_num_ - k; i < dec_num_; i++)
			gx += pow((ind->dec_[i] - 0.5), 2.0) - cos(20.0 * PI * (ind->dec_[i] - 0.5));
		gx = 100.0 * (k + gx);

		for (int i = 0; i < obj_num_; i++)
			ind->obj_[i] = (1.0 + gx) * 0.5;

		for (int i = 0; i < obj_num_; i++)
		{
			for (int j = 0; j < obj_num_ - (i + 1); j++)
				ind->obj_[i] *= ind->dec_[j];
			if (i != 0)
			{
				int aux = obj_num_ - (i + 1);
				ind->obj_[i] *= 1 - ind->dec_[aux];
			}
		}
	}

	DTLZ2::DTLZ2(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	DTLZ2::~DTLZ2()
	{

	}

	void DTLZ2::CalObj(Individual *ind)
	{
		double gx = 0.0;
		int k = dec_num_ - obj_num_ + 1;

		for (int i = dec_num_ - k; i < dec_num_; i++)
			gx += pow((ind->dec_[i] - 0.5), 2.0);

		for (int i = 0; i < obj_num_; i++)
			ind->obj_[i] = 1.0 + gx;

		for (int i = 0; i < obj_num_; i++)
		{
			for (int j = 0; j < obj_num_ - (i + 1); j++)
				ind->obj_[i] *= cos(PI * 0.5 * ind->dec_[j]);
			if (i != 0)
			{
				int aux = obj_num_ - (i + 1);
				ind->obj_[i] *= sin(PI * 0.5 * ind->dec_[aux]);
			}
		}
	}

	DTLZ3::DTLZ3(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	DTLZ3::~DTLZ3()
	{

	}

	void DTLZ3::CalObj(Individual *ind)
	{
		double gx = 0.0;
		int k =dec_num_ -obj_num_ + 1;

		for (int i =dec_num_ - k; i <dec_num_; i++)
			gx += pow((ind->dec_[i] - 0.5), 2.0) - cos(20.0 * PI * (ind->dec_[i] - 0.5));
		gx = 100.0 * (k + gx);

		for (int i = 0; i <obj_num_; i++)
			ind->obj_[i] = 1.0 + gx;

		for (int i = 0; i <obj_num_; i++)
		{
			for (int j = 0; j <obj_num_ - (i + 1); j++)
				ind->obj_[i] *= cos(PI * 0.5 * ind->dec_[j]);
			if (i != 0)
			{
				int aux =obj_num_ - (i + 1);
				ind->obj_[i] *= sin(PI * 0.5 * ind->dec_[aux]);
			}
		}
	}

	DTLZ4::DTLZ4(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	DTLZ4::~DTLZ4()
	{

	}

	void DTLZ4::CalObj(Individual *ind)
	{
		double gx = 0.0, alpha = 100.0;
		int k =dec_num_ -obj_num_ + 1;

		for (int i =dec_num_ - k; i <dec_num_; i++)
			gx += pow((ind->dec_[i] - 0.5), 2.0);

		for (int i = 0; i <obj_num_; i++)
			ind->obj_[i] = 1.0 + gx;

		for (int i = 0; i <obj_num_; i++)
		{
			for (int j = 0; j <obj_num_ - (i + 1); j++)
				ind->obj_[i] *= cos(PI * 0.5 * pow(ind->dec_[j], alpha));
			if (i != 0)
			{
				int aux =obj_num_ - (i + 1);
				ind->obj_[i] *= sin(PI * 0.5 * pow(ind->dec_[aux], alpha));
			}
		}
	}


	DTLZ5::DTLZ5(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		theta = new double[dec_num];
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	DTLZ5::~DTLZ5()
	{
		delete[] theta;
		theta = nullptr;
	}

	void DTLZ5::CalObj(Individual *ind)
	{
		double gx = 0.0;
		int k = dec_num_ -obj_num_ + 1;

		for (int i = dec_num_ - k; i < dec_num_; i++)
			gx += pow((ind->dec_[i] - 0.5), 2.0);

		double temp = PI / (4.0 * (1.0 + gx));
		theta[0] = ind->dec_[0] * PI / 2.0;
		for (int i = 1; i < (obj_num_ - 1); i++)
			theta[i] = temp * (1.0 + 2.0 * gx * ind->dec_[i]);

		for (int i = 0; i <obj_num_; i++)
			ind->obj_[i] = 1.0 + gx;

		for (int i = 0; i <obj_num_; i++)
		{
			for (int j = 0; j <obj_num_ - (i + 1); j++)
				ind->obj_[i] *= cos(theta[j]);
			if (i != 0)
			{
				int aux =obj_num_ - (i + 1);
				ind->obj_[i] *= sin(theta[aux]);
			}
		}

	}

	DTLZ6::DTLZ6(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		theta = new double[dec_num];
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	DTLZ6::~DTLZ6()
	{
		delete[] theta;
		theta = nullptr;
	}

	void DTLZ6::CalObj(Individual *ind)
	{
		double gx = 0.0;
		int k = dec_num_ -obj_num_ + 1;

		for (int i = dec_num_ - k; i < dec_num_; i++)
			gx += pow(ind->dec_[i], 0.1);

		double temp = PI / (4.0 * (1.0 + gx));
		theta[0] = ind->dec_[0] * PI / 2.0;
		for (int i = 1; i < (obj_num_ - 1); i++)
			theta[i] = temp * (1.0 + 2.0 * gx * ind->dec_[i]);

		for (int i = 0; i <obj_num_; i++)
			ind->obj_[i] = 1.0 + gx;

		for (int i = 0; i <obj_num_; i++)
		{
			for (int j = 0; j <obj_num_ - (i + 1); j++)
				ind->obj_[i] *= cos(theta[j]);
			if (i != 0)
			{
				int aux =obj_num_ - (i + 1);
				ind->obj_[i] *= sin(theta[aux]);
			}
		}
	}

	DTLZ7::DTLZ7(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	DTLZ7::~DTLZ7()
	{

	}

	void DTLZ7::CalObj(Individual *ind)
	{
		double gx = 0.0;
		int k = dec_num_ -obj_num_ + 1;

		for (int i = dec_num_ - k; i < dec_num_; i++)
			gx += ind->dec_[i];
		gx = 1.0 + (9.0 * gx) / k;

		for (int i = 0; i <obj_num_; i++)
			ind->obj_[i] = ind->dec_[i];

		double h = 0.0;
		for (int i = 0; i <obj_num_ - 1; i++)
			h += (ind->obj_[i] / (1.0 + gx)) * (1.0 + sin(3.0 * PI * ind->obj_[i]));
		h =obj_num_ - h;

		ind->obj_[obj_num_ - 1] = (1 + gx) * h;
	}

	MinusDTLZ1::MinusDTLZ1(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	MinusDTLZ1::~MinusDTLZ1()
	{

	}

	void MinusDTLZ1::CalObj(Individual* ind)
	{
		double gx = 0.0;
		int k = dec_num_ - obj_num_ + 1;

		for (int i = dec_num_ - k; i < dec_num_; i++)
			gx += pow((ind->dec_[i] - 0.5), 2.0) - cos(20.0 * PI * (ind->dec_[i] - 0.5));
		gx = 100.0 * (k + gx);

		for (int i = 0; i < obj_num_; i++)
			ind->obj_[i] = (1.0 + gx) * 0.5;

		for (int i = 0; i < obj_num_; i++)
		{
			for (int j = 0; j < obj_num_ - (i + 1); j++)
				ind->obj_[i] *= ind->dec_[j];
			if (i != 0)
			{
				int aux = obj_num_ - (i + 1);
				ind->obj_[i] *= 1 - ind->dec_[aux];
			}
		}

		for (int i = 0; i < obj_num_; i++)
			ind->obj_[i] = -1 * ind->obj_[i];
	}

	MinusDTLZ2::MinusDTLZ2(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}

	}

	MinusDTLZ2::~MinusDTLZ2()
	{

	}

	void MinusDTLZ2::CalObj(Individual* ind)
	{
		double gx = 0.0;
		int k = dec_num_ - obj_num_ + 1;

		for (int i = dec_num_ - k; i < dec_num_; i++)
			gx += pow((ind->dec_[i] - 0.5), 2.0);

		for (int i = 0; i < obj_num_; i++)
			ind->obj_[i] = 1.0 + gx;

		for (int i = 0; i < obj_num_; i++)
		{
			for (int j = 0; j < obj_num_ - (i + 1); j++)
				ind->obj_[i] *= cos(PI * 0.5 * ind->dec_[j]);
			if (i != 0)
			{
				int aux = obj_num_ - (i + 1);
				ind->obj_[i] *= sin(PI * 0.5 * ind->dec_[aux]);
			}
		}

		for (int i = 0; i < obj_num_; i++)
			ind->obj_[i] = -1 * ind->obj_[i];
	}

	MinusDTLZ3::MinusDTLZ3(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	MinusDTLZ3::~MinusDTLZ3()
	{

	}

	void MinusDTLZ3::CalObj(Individual* ind)
	{
		double gx = 0.0;
		int k = dec_num_ - obj_num_ + 1;

		for (int i = dec_num_ - k; i < dec_num_; i++)
			gx += pow((ind->dec_[i] - 0.5), 2.0) - cos(20.0 * PI * (ind->dec_[i] - 0.5));
		gx = 100.0 * (k + gx);

		for (int i = 0; i < obj_num_; i++)
			ind->obj_[i] = 1.0 + gx;

		for (int i = 0; i < obj_num_; i++)
		{
			for (int j = 0; j < obj_num_ - (i + 1); j++)
				ind->obj_[i] *= cos(PI * 0.5 * ind->dec_[j]);
			if (i != 0)
			{
				int aux = obj_num_ - (i + 1);
				ind->obj_[i] *= sin(PI * 0.5 * ind->dec_[aux]);
			}
		}

		for (int i = 0; i < obj_num_; i++)
			ind->obj_[i] = -1 * ind->obj_[i];
	}

	MinusDTLZ4::MinusDTLZ4(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	MinusDTLZ4::~MinusDTLZ4()
	{

	}

	void MinusDTLZ4::CalObj(Individual* ind)
	{
		double gx = 0.0, alpha = 100.0;
		int k = dec_num_ - obj_num_ + 1;

		for (int i = dec_num_ - k; i < dec_num_; i++)
			gx += pow((ind->dec_[i] - 0.5), 2.0);

		for (int i = 0; i < obj_num_; i++)
			ind->obj_[i] = 1.0 + gx;

		for (int i = 0; i < obj_num_; i++)
		{
			for (int j = 0; j < obj_num_ - (i + 1); j++)
				ind->obj_[i] *= cos(PI * 0.5 * pow(ind->dec_[j], alpha));
			if (i != 0)
			{
				int aux = obj_num_ - (i + 1);
				ind->obj_[i] *= sin(PI * 0.5 * pow(ind->dec_[aux], alpha));
			}
		}

		for (int i = 0; i < obj_num_; i++)
			ind->obj_[i] = -1 * ind->obj_[i];
	}

	MDTLZ1::MDTLZ1(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	MDTLZ1::~MDTLZ1()
	{

	}

	void MDTLZ1::CalObj(Individual* ind)
	{
		int k, size, index;
		double h, g;
		for (int i = 0; i < obj_num_; ++i)
		{
			//h(x_1)
			h = 1.0;
			for (int j = 0; j < obj_num_ - i - 1; ++j)
			{
				h *= ind->dec_[j];
			}
			if (i > 0)
			{
				h *= 1 - ind->dec_[obj_num_ - i - 1];
			}
			h = 0.5 * (1 - h);
			//g(x_2)
			k = i + 1;// [1,m]
			g = 0.0;
			size = (dec_num_ + 1 - k) / obj_num_;
			for (int j = 1; j <= size; ++j)
			{
				index = j * obj_num_ - 1 + k;
				g += pow(ind->dec_[index - 1] - 0.5, 2) - cos(20 * PI * (ind->dec_[index - 1] - 0.5));
			}
			g = 100 * (size + g);
			ind->obj_[i] = h * (1 + g);
		}
	}

	MDTLZ2::MDTLZ2(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	MDTLZ2::~MDTLZ2()
	{

	}

	void MDTLZ2::CalObj(Individual* ind)
	{
		int k, size, index;
		double h, g;
		for (int i = 0; i < obj_num_; ++i)
		{
			//h(x_1)
			h = 1.0;
			for (int j = 0; j < obj_num_ - i - 1; ++j)
			{
				h *= cos(0.5 * PI * ind->dec_[j]);
			}
			if (i > 0)
			{
				h *= sin(0.5 * PI * ind->dec_[obj_num_ - i - 1]);
			}
			h = 1 - h;
			//g(x_2)
			k = i + 1;// [1,m]
			g = 0.0;
			size = (dec_num_ + 1 - k) / obj_num_;
			for (int j = 1; j <= size; ++j)
			{
				index = j * obj_num_ - 1 + k;
				g += pow(ind->dec_[index - 1] - 0.5, 2);
			}
			ind->obj_[i] = h * (1 + g);
		}
	}

	MDTLZ3::MDTLZ3(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	MDTLZ3::~MDTLZ3()
	{

	}

	void MDTLZ3::CalObj(Individual* ind)
	{
		int k, size, index;
		double h, g;
		for (int i = 0; i < obj_num_; ++i)
		{
			//h(x_1)
			h = 1.0;
			for (int j = 0; j < obj_num_ - i - 1; ++j)
			{
				h *= cos(0.5 * PI * ind->dec_[j]);
			}
			if (i > 0)
			{
				h *= sin(0.5 * PI * ind->dec_[obj_num_ - i - 1]);
			}
			h = 1 - h;
			//g(x_2)
			k = i + 1;// [1,m]
			g = 0.0;
			size = (dec_num_ + 1 - k) / obj_num_;
			for (int j = 1; j <= size; ++j)
			{
				index = j * obj_num_ - 1 + k;
				g += pow(ind->dec_[index - 1] - 0.5, 2) - cos(20 * PI * (ind->dec_[index - 1] - 0.5));
			}
			g = 100 * (size + g);
			ind->obj_[i] = h * (1 + g);
		}
	}

	MDTLZ4::MDTLZ4(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	MDTLZ4::~MDTLZ4()
	{

	}

	void MDTLZ4::CalObj(Individual* ind)
	{

		int alpha = 100;
		int k, size, index;
		double h, g;
		for (int i = 0; i < obj_num_; ++i)
		{
			//h(x_1)
			h = 1.0;
			for (int j = 0; j < obj_num_ - i - 1; ++j)
			{
				h *= cos(0.5 * PI * pow(ind->dec_[j], alpha));
			}
			if (i > 0)
			{
				h *= sin(0.5 * PI * pow(ind->dec_[obj_num_ - i - 1], alpha));
			}
			h = 1 - h;
			//g(x_2)
			k = i + 1;// [1,m]
			g = 0.0;
			size = (dec_num_ + 1 - k) / obj_num_;
			for (int j = 1; j <= size; ++j)
			{
				index = j * obj_num_ - 1 + k;
				g += pow(ind->dec_[index - 1] - 0.5, 2);
			}
			ind->obj_[i] = h * (1 + g);
		}
	}

}