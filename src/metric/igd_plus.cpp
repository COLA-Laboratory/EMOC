#include "metric/igd_plus.h"

#include <fstream>
#include <iostream>
#include <cmath>

#include "core/macro.h"
#include "core/global.h"
#include "core/utility.h"


namespace emoc {

	double CalculateIGDPlus(Individual** pop, int pop_num, int obj_num, double** pf_data, int pf_size)
	{
		// calculate igd value
		double igd_value = 0;
		Individual* temp_ind = nullptr;
		double min_distance = 0, temp_distance = 0;
		double* new_distance = new double[obj_num];

		for (int i = 0; i < pf_size; i++)
		{
			min_distance = EMOC_INF;
			for (int j = 0; j < pop_num; j++)
			{
				temp_distance = 0;
				temp_ind = pop[j];

				for (int k = 0; k < obj_num; k++)
				{
					new_distance[k] = pop[j]->obj_[k] > pf_data[i][k] ? pop[j]->obj_[k] - pf_data[i][k] : 0;
					temp_distance += new_distance[k] * new_distance[k];
				}

				temp_distance = sqrt(temp_distance);
				if (min_distance > temp_distance)
				{
					min_distance = temp_distance;
				}
			}
			igd_value += min_distance;
		}
		igd_value /= pf_size;
		delete[] new_distance;
		return 	igd_value;
	}

}