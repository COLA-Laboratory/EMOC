#include "core/uniform_point.h"

#include "core/macro.h"

namespace emoc {

	static void SetWeight(double *weight, double unit, double sum, int dim, int *column, double **lambda, int obj_num)
	{
		int i;

		if (dim == obj_num)
		{
			for (i = 0; i < obj_num; i++)
				weight[i] = 0;
		}

		if (dim == 1)
		{
			weight[0] = unit - sum;
			for (i = 0; i < obj_num; i++)
				lambda[*column][i] = weight[i];
			*column = *column + 1;
			return;
		}
		for (i = 0; i <= unit - sum; i++)
		{
			weight[dim - 1] = i;
			SetWeight(weight, unit, sum + i, dim - 1, column, lambda, obj_num);
		}

		return;
	}

	double** UniformPoint(int num, int *weight_num, int obj_num)
	{
		int column = 0;
		double *vec = nullptr;
		double **lambda = nullptr;

		int gaps = 1;
		*weight_num = 0;
		while (1)
		{
			int layer_size = Combination(obj_num + gaps - 1, gaps);

			if (layer_size > num) break;
			*weight_num = layer_size;
			gaps = gaps + 1;
		}

		gaps = gaps - 1;
		lambda = new double*[*weight_num];
		for (int i = 0; i < *weight_num; i++)
		{
			lambda[i] = new double[obj_num];
		}

		vec = new double[obj_num];
		for (int i = 0; i < obj_num; i++)
			vec[i] = 0;
		SetWeight(vec, gaps, 0, obj_num, &column, lambda,obj_num);

		for (int i = 0; i < *weight_num; i++)
		{
			for (int j = 0; j < obj_num; j++)
			{
				lambda[i][j] = lambda[i][j] / gaps;
				if (lambda[i][j] < EMOC_EPS) lambda[i][j] = EMOC_EPS;
			}
		}
		delete[] vec;
		return lambda;
	}

}