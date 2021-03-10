#include "metric/spacing.h"

#include <cmath>

#include "core/global.h"
#include "core/utility.h"

namespace emoc {

	double CalculateSpacing(Individual **pop, int pop_num, int obj_num)
	{
		// calculate igd value
		double *spacing = new double[pop_num];
		double spacing_sum = 0;
		double min_distance = 0, temp_distance = 0;

		for (int i = 0; i < pop_num; i++)
		{
			min_distance = INF;
			for (int j = 0; j < pop_num; j++)
			{
				if(i == j)
					continue;

				temp_distance = CalEuclidianDistance(pop[i]->obj_, pop[j]->obj_, obj_num);

				if (min_distance > temp_distance)
				{
					min_distance = temp_distance;
				}
			}
			spacing[i] = min_distance;
			spacing_sum += min_distance;
		}
		spacing_sum /= pop_num;

		double S = 0;
		for (int i = 0; i < pop_num; ++i)
		{
			S += (spacing[i] - spacing_sum) * (spacing[i] - spacing_sum);
		}
		S /= pop_num;
		S = sqrt(S);

		delete[] spacing;
		return S;
	}

}