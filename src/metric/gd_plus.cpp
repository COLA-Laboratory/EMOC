#include "metric/gd_plus.h"

#include <fstream>
#include <iostream>
#include <cmath>

#include "core/macro.h"
#include "core/global.h"
#include "core/utility.h"


namespace emoc {

	double CalculateGDPlus(Individual** pop, int pop_num, int obj_num, std::string& problem_name)
	{
		// load pf data
		int pf_size = 0;
		double** pfdata = nullptr;
		pfdata = LoadPFData(pf_size, obj_num, problem_name);

		// calculate igd value
		double gd_value = 0;
		Individual* temp_ind = nullptr;
		double min_distance = 0, temp_distance = 0;
		double* new_distance = new double[obj_num];

		for (int i = 0; i < pop_num; i++)
		{
			min_distance = EMOC_INF;
			temp_ind = pop[i];

			for (int j = 0; j < pf_size; j++)
			{
				temp_distance = 0;
				for (int k = 0; k < obj_num; k++)
				{
					new_distance[k] = pop[i]->obj_[k] > pfdata[j][k] ? pop[i]->obj_[k] - pfdata[j][k] : 0;
					temp_distance += new_distance[k] * new_distance[k];
				}

				temp_distance = sqrt(temp_distance);
				if (min_distance > temp_distance)
				{
					min_distance = temp_distance;
				}
			}
			gd_value += min_distance;
		}
		gd_value /= pop_num;

		// free pf data memory
		for (int i = 0; i < pf_size; ++i)
			delete[] pfdata[i];
		delete[] pfdata;
		delete[] new_distance;

		return 	gd_value;
	}

}