#include "core/uniform_point.h"

namespace emoc {

	static void SetWeight(double *weight, double unit, double sum, int dim, int *column, double **lambda)
	{
		int i;

		if (dim == g_GlobalSettings->obj_num_)
		{
			for (i = 0; i < g_GlobalSettings->obj_num_; i++)
				weight[i] = 0;
		}

		if (dim == 1)
		{
			weight[0] = unit - sum;
			for (i = 0; i < g_GlobalSettings->obj_num_; i++)
				lambda[*column][i] = weight[i];
			*column = *column + 1;
			return;
		}
		for (i = 0; i <= unit - sum; i++)
		{
			weight[dim - 1] = i;
			SetWeight(weight, unit, sum + i, dim - 1, column, lambda);
		}

		return;
	}

	double** UniformPoint(int num, int *weight_num)
	{
		int column = 0;
		double *vec = nullptr;
		double **lambda = nullptr;

		int gaps = 1;
		*weight_num = 0;
		while (1)
		{
			int layer_size = Combination(g_GlobalSettings->obj_num_ + gaps - 1, gaps);

			if (layer_size > num) break;
			*weight_num = layer_size;
			gaps = gaps + 1;
		}

		gaps = gaps - 1;
		lambda = new double*[*weight_num];
		for (int i = 0; i < *weight_num; i++)
		{
			lambda[i] = new double[g_GlobalSettings->obj_num_];
		}

		vec = new double[g_GlobalSettings->obj_num_];
		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
			vec[i] = 0;
		SetWeight(vec, gaps, 0, g_GlobalSettings->obj_num_, &column, lambda);

		for (int i = 0; i < *weight_num; i++)
			for (int j = 0; j < g_GlobalSettings->obj_num_; j++) 
				lambda[i][j] = lambda[i][j] / gaps;
			
		delete[] vec;
		return lambda;
	}

}