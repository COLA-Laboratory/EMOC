#include "problem/dcdtlz/dcdtlz.h"

#include <cmath>

#include "core/macro.h"
#include "core/global.h"

namespace emoc {

	DC1DTLZ1::DC1DTLZ1(int dec_num, int obj_num):Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	DC1DTLZ1::~DC1DTLZ1()
	{

	}

	void DC1DTLZ1::CalObj(Individual* ind)
	{
		int i, j, k, aux;
		double gx;
		double* xreal, * obj;

		obj = ind->obj_.data();
		xreal = ind->dec_.data();

		gx = 0.0;
		k = dec_num_ - obj_num_ + 1;
		for (i = dec_num_ - k; i < dec_num_; i++)
			gx += std::pow((xreal[i] - 0.5), 2.0) - std::cos(20.0 * PI * (xreal[i] - 0.5));
		gx = 100.0 * (k + gx);

		for (i = 0; i < obj_num_; i++)
			obj[i] = (1.0 + gx) * 0.5;

		for (i = 0; i < obj_num_; i++)
		{
			for (j = 0; j < obj_num_ - (i + 1); j++)
				obj[i] *= xreal[j];
			if (i != 0)
			{
				aux = obj_num_ - (i + 1);
				obj[i] *= 1 - xreal[aux];
			}
		}
	}

	void DC1DTLZ1::CalCon(Individual* ind)
	{
		ind->con_[0] = 0.5 - std::cos(3 * PI * ind->dec_[0]);
	}

	DC1DTLZ3::DC1DTLZ3(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	DC1DTLZ3::~DC1DTLZ3()
	{

	}

	void DC1DTLZ3::CalObj(Individual* ind)
	{
		int i, j, k, aux;

		double gx, fsum;
		double* xreal, * obj;

		obj = ind->obj_.data();
		xreal = ind->dec_.data();

		gx = 0.0;
		k = dec_num_ - obj_num_ + 1;
		for (i = dec_num_ - k; i < dec_num_; i++)
			gx += std::pow((xreal[i] - 0.5), 2.0) - std::cos(20.0 * PI * (xreal[i] - 0.5));
		gx = 10.0 * (k + gx);

		for (i = 0; i < obj_num_; i++)
			obj[i] = 1.0 + gx;

		for (i = 0; i < obj_num_; i++)
		{
			for (j = 0; j < obj_num_ - (i + 1); j++)
				obj[i] *= std::cos(PI * 0.5 * xreal[j]);
			if (i != 0)
			{
				aux = obj_num_ - (i + 1);
				obj[i] *= std::sin(PI * 0.5 * xreal[aux]);
			}
		}
	}

	void DC1DTLZ3::CalCon(Individual* ind)
	{
		ind->con_[0] = 0.5 - std::cos(3 * PI * ind->dec_[0]);
	}

	DC2DTLZ1::DC2DTLZ1(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	DC2DTLZ1::~DC2DTLZ1()
	{

	}

	void DC2DTLZ1::CalObj(Individual* ind)
	{
		int i, j, k, aux;
		double gx;
		double* xreal, * obj;

		obj = ind->obj_.data();
		xreal = ind->dec_.data();

		gx = 0.0;
		k = dec_num_ - obj_num_ + 1;
		for (i = dec_num_ - k; i < dec_num_; i++)
			gx += std::pow((xreal[i] - 0.5), 2.0) - std::cos(20.0 * PI * (xreal[i] - 0.5));
		gx = 100.0 * (k + gx);

		for (i = 0; i < obj_num_; i++)
			obj[i] = (1.0 + gx) * 0.5;

		for (i = 0; i < obj_num_; i++)
		{
			for (j = 0; j < obj_num_ - (i + 1); j++)
				obj[i] *= xreal[j];
			if (i != 0)
			{
				aux = obj_num_ - (i + 1);
				obj[i] *= 1 - xreal[aux];
			}
		}
	}

	void DC2DTLZ1::CalCon(Individual* ind)
	{
		int i, k;
		double gx;
		double* xreal, * obj;
		obj = ind->obj_.data();
		xreal = ind->dec_.data();
		gx = 0.0;
		k = dec_num_ - obj_num_ + 1;
		for (i = dec_num_ - k; i < dec_num_; i++)
			gx += std::pow((xreal[i] - 0.5), 2.0) - std::cos(20.0 * PI * (xreal[i] - 0.5));
		gx = 100.0 * (k + gx);

		ind->con_[0] = 0.9 - std::cos(3 * PI * gx);
		ind->con_[1] = 0.9 - std::exp(-gx);
	}

	DC2DTLZ3::DC2DTLZ3(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	DC2DTLZ3::~DC2DTLZ3()
	{

	}

	void DC2DTLZ3::CalObj(Individual* ind)
	{
		int i, j, k, aux;

		double gx, fsum;
		double* xreal, * obj;

		obj = ind->obj_.data();
		xreal = ind->dec_.data();

		gx = 0.0;
		k = dec_num_ - obj_num_ + 1;
		for (i = dec_num_ - k; i < dec_num_; i++)
			gx += std::pow((xreal[i] - 0.5), 2.0) - std::cos(20.0 * PI * (xreal[i] - 0.5));
		gx = 10.0 * (k + gx);

		for (i = 0; i < obj_num_; i++)
			obj[i] = 1.0 + gx;

		for (i = 0; i < obj_num_; i++)
		{
			for (j = 0; j < obj_num_ - (i + 1); j++)
				obj[i] *= std::cos(PI * 0.5 * xreal[j]);
			if (i != 0)
			{
				aux = obj_num_ - (i + 1);
				obj[i] *= std::sin(PI * 0.5 * xreal[aux]);
			}
		}
	}

	void DC2DTLZ3::CalCon(Individual* ind)
	{
		int i, k;
		double gx;
		double* xreal, * obj;
		obj = ind->obj_.data();
		xreal = ind->dec_.data();
		gx = 0.0;
		k = dec_num_ - obj_num_ + 1;
		for (i = dec_num_ - k; i < dec_num_; i++)
			gx += std::pow((xreal[i] - 0.5), 2.0) - std::cos(20.0 * PI * (xreal[i] - 0.5));
		gx = 10.0 * (k + gx);

		ind->con_[0] = 0.5 - std::cos(3 * PI * gx);
		ind->con_[1] = 0.5 - std::exp(-gx);
	}

	DC3DTLZ1::DC3DTLZ1(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	DC3DTLZ1::~DC3DTLZ1()
	{

	}

	void DC3DTLZ1::CalObj(Individual* ind)
	{
		int i, j, k, aux;
		double gx;
		double* xreal, * obj;

		obj = ind->obj_.data();
		xreal = ind->dec_.data();

		gx = 0.0;
		k = dec_num_ - obj_num_ + 1;
		for (i = dec_num_ - k; i < dec_num_; i++)
			gx += std::pow((xreal[i] - 0.5), 2.0) - std::cos(20.0 * PI * (xreal[i] - 0.5));
		gx = 100.0 * (k + gx);

		for (i = 0; i < obj_num_; i++)
			obj[i] = (1.0 + gx) * 0.5;

		for (i = 0; i < obj_num_; i++)
		{
			for (j = 0; j < obj_num_ - (i + 1); j++)
				obj[i] *= xreal[j];
			if (i != 0)
			{
				aux = obj_num_ - (i + 1);
				obj[i] *= 1 - xreal[aux];
			}
		}
	}

	void DC3DTLZ1::CalCon(Individual* ind)
	{
		int i, k;
		double gx;
		double* xreal, * obj;
		obj = ind->obj_.data();
		xreal = ind->dec_.data();
		gx = 0.0;
		k = dec_num_ - obj_num_ + 1;
		for (i = dec_num_ - k; i < dec_num_; i++)
			gx += std::pow((xreal[i] - 0.5), 2.0) - std::cos(20.0 * PI * (xreal[i] - 0.5));
		gx = 100.0 * (k + gx);

		for (int i = 0; i < obj_num_ - 1; i++)
		{
			ind->con_[i] = 0.5 - std::cos(3 * PI * ind->dec_[i]);
		}
		ind->con_[obj_num_ - 1] = 0.5 - std::cos(3 * PI * gx);
	}

	DC3DTLZ3::DC3DTLZ3(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	DC3DTLZ3::~DC3DTLZ3()
	{

	}

	void DC3DTLZ3::CalObj(Individual* ind)
	{
		int i, j, k, aux;

		double gx, fsum;
		double* xreal, * obj;

		obj = ind->obj_.data();
		xreal = ind->dec_.data();

		gx = 0.0;
		k = dec_num_ - obj_num_ + 1;
		for (i = dec_num_ - k; i < dec_num_; i++)
			gx += std::pow((xreal[i] - 0.5), 2.0) - std::cos(20.0 * PI * (xreal[i] - 0.5));
		gx = 10.0 * (k + gx);

		for (i = 0; i < obj_num_; i++)
			obj[i] = 1.0 + gx;

		for (i = 0; i < obj_num_; i++)
		{
			for (j = 0; j < obj_num_ - (i + 1); j++)
				obj[i] *= std::cos(PI * 0.5 * xreal[j]);
			if (i != 0)
			{
				aux = obj_num_ - (i + 1);
				obj[i] *= std::sin(PI * 0.5 * xreal[aux]);
			}
		}
	}

	void DC3DTLZ3::CalCon(Individual* ind)
	{
		int i, k;
		double gx;
		double* xreal, * obj;
		obj = ind->obj_.data();
		xreal = ind->dec_.data();
		gx = 0.0;
		k = dec_num_ - obj_num_ + 1;
		for (i = dec_num_ - k; i < dec_num_; i++)
			gx += std::pow((xreal[i] - 0.5), 2.0) - std::cos(20.0 * PI * (xreal[i] - 0.5));
		gx = 10.0 * (k + gx);

		for (int i = 0; i < obj_num_ - 1; i++)
		{
			ind->con_[i] = 0.5 - std::cos(3 * PI * ind->dec_[i]);
		}
		ind->con_[obj_num_ - 1] = 0.5 - std::cos(3 * PI * gx);
	}
}