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

}