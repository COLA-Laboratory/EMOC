#include "core/utility.h"

#include <cmath>

namespace emoc {

	DominateReleation CheckDominance(Individual *ind1, Individual *ind2)
	{
		int flag1 = 0, flag2 = 0;

		for (int i = 0; i < g_GlobalSettings->obj_num_; ++i)
		{
			if (ind1->obj_[i] < ind2->obj_[i])
				flag1 = 1;
			else
			{
				if (ind1->obj_[i] > ind2->obj_[i])
					flag2 = 1;
			}
		}

		if (flag1 == 1 && flag2 == 0)
			return (DOMINATE);
		else
		{
			if (flag1 == 0 && flag2 == 1)
				return (DOMINATED);
			else
				return (NON_DOMINATED);
		}
	}

	double CalEuclidianDistance(double *a, double *b, int dimension)
	{
		double distance = 0.0;
		for (int i = 0; i < dimension; i++)
			distance += (a[i] - b[i]) * (a[i] - b[i]);
		return sqrt(distance);
	}

	int Combination(int n, int k)
	{
		if (n < k)
			return -1;

		double ans = 1;
		for (int i = k + 1; i <= n; i++)
		{
			ans = ans * i;
			ans = ans / (double)(i - k);
		}

		return (int)ans;
	}


	void UpdateIdealpoint(Individual *ind, double *ideal_point)
	{
		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
		{
			if (ind->obj_[i] < ideal_point[i])
				ideal_point[i] = ind->obj_[i];
		}
	}


	void UpdateIdealpoint(Individual **pop, int pop_num, double *ideal_point)
	{
		for (int i = 0; i < g_GlobalSettings->obj_num_; ++i)
			ideal_point[i] = INF;

		for (int i = 0; i < pop_num; i++)
		{
			for (int j = 0; j < g_GlobalSettings->obj_num_; j++)
			{
				if (pop[i]->obj_[j] < ideal_point[j])
					ideal_point[j] = pop[i]->obj_[j];
			}
		}
	}
}