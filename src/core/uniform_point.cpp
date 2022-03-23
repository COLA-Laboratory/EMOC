#include "core/uniform_point.h"

#include "core/macro.h"
#include "core/utility.h"

#include <iostream>

namespace emoc {

	static void SetWeight(double *weight, double unit, double sum, int dim, int *column, double **lambda, int obj_num)
	{
		if (dim == obj_num)
		{
			for (int i = 0; i < obj_num; i++)
				weight[i] = 0;
		}

		if (dim == 1)
		{
			weight[0] = unit - sum;
			for (int i = 0; i < obj_num; i++)
				lambda[*column][i] = weight[i];
			*column = *column + 1;
			return;
		}
		for (int i = 0; i <= unit - sum; i++)
		{
			weight[dim - 1] = i;
			SetWeight(weight, unit, sum + i, dim - 1, column, lambda, obj_num);
		}

		return;
	}

	double** UniformPoint(int num, int* weight_num, int obj_num)
	{
		int H1 = 1, H2 = 0;
		int column1 = 0, column2 = 0;
		int weight_num1 = 0, weight_num2 = 0;
		double** lambda1 = nullptr, **lambda2 = nullptr;
		double* vec1 = new double[obj_num], *vec2 = new double[obj_num];

		while (Combination(H1 + obj_num, obj_num - 1) <= num)
			H1++;
		weight_num1 = Combination(H1 + obj_num - 1, obj_num - 1);
		lambda1 = new double* [weight_num1];
		for (int i = 0; i < weight_num1; i++)
			lambda1[i] = new double[obj_num];
		for (int i = 0; i < obj_num; i++)
		{
			vec1[i] = 0;
			vec2[i] = 0;
		}
		SetWeight(vec1, H1, 0, obj_num, &column1, lambda1, obj_num);
		for (int i = 0; i < weight_num1; i++)
		{
			for (int j = 0; j < obj_num; j++)
			{
				lambda1[i][j] = lambda1[i][j] / H1;
				if (lambda1[i][j] < EMOC_EPS) lambda1[i][j] = EMOC_EPS;
			}
		}

		if (H1 < obj_num)
		{
			while ((Combination(H1 + obj_num - 1, obj_num - 1) + Combination(H2 + obj_num, obj_num - 1)) <= num)
				H2++;
			if (H2 > 0)
			{
				weight_num2 = Combination(H2 + obj_num - 1, obj_num - 1);
				lambda2 = new double* [weight_num2];
				for (int i = 0; i < weight_num2; i++)
					lambda2[i] = new double[obj_num];

				SetWeight(vec2, H2, 0, obj_num, &column2, lambda2, obj_num);
				for (int i = 0; i < weight_num2; i++)
				{
					for (int j = 0; j < obj_num; j++)
					{
						lambda2[i][j] = lambda2[i][j] / H2;
						lambda2[i][j] = 0.5 / obj_num + 0.5 * lambda2[i][j];
						if (lambda2[i][j] < EMOC_EPS) lambda2[i][j] = EMOC_EPS;
					}
				}
			}
		}

		*weight_num = weight_num1 + weight_num2;
		double** lambda = new double* [*weight_num];
		for (int i = 0; i < *weight_num; i++)
			lambda[i] = new double[obj_num];
		for (int i = 0; i < *weight_num; i++)
		{
			for (int j = 0; j < obj_num; j++)
			{
				if (i < weight_num1)
				{
					lambda[i][j] = lambda1[i][j];
				}
				else
				{
					lambda[i][j] = lambda2[i - weight_num1][j];
				}
			}
		}

		for (int i = 0; i < *weight_num; i++)
		{
			if (i < weight_num1)
				delete[] lambda1[i];
			else
				delete[] lambda2[i - weight_num1];
		}
		delete[] lambda1;
		if (weight_num2 != 0) delete[] lambda2;
		delete[] vec1;
		delete[] vec2;
		return lambda;
	}

}