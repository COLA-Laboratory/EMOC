#define _CRT_SECURE_NO_WARNINGS

#include "metric/igd.h"

#include <fstream>
#include <iostream>

#include "core/macro.h"
#include "core/global.h"
#include "core/utility.h"


namespace emoc {

	double CalculateIGD(Individual **pop, int pop_num, int obj_num, double** pf_data, int pf_size)
	{
		// calculate igd value
		double igd_value = 0;
		Individual *temp_ind = nullptr;
		double min_distance = 0, temp_distance = 0;

		for (int i = 0; i < pf_size; i++)
		{
			min_distance = EMOC_INF;
			for (int j = 0; j < pop_num; j++)
			{
				temp_ind = pop[j];
				temp_distance = CalEuclidianDistance(pf_data[i],temp_ind->obj_.data(), obj_num);

				if (min_distance > temp_distance)
				{
					min_distance = temp_distance;
				}
			}
			igd_value += min_distance;
		}

		igd_value /= pf_size;
		return 	igd_value;
	}

	double CalculateIGD(const std::vector<std::vector<double>>& pop, int pop_num, int obj_num, std::string& problem_name)
	{
		// load pf data
		int pf_size = 0;
		double** pfdata = nullptr;
		pfdata = LoadPFData(pf_size, obj_num, problem_name);

		// calculate igd value
		double igd_value = 0;
		double min_distance = 0, temp_distance = 0;

		for (int i = 0; i < pf_size; i++)
		{
			min_distance = EMOC_INF;
			for (int j = 0; j < pop_num; j++)
			{
				temp_distance = CalEuclidianDistance(pfdata[i], pop[j].data(), obj_num);

				if (min_distance > temp_distance)
				{
					min_distance = temp_distance;
				}
			}
			igd_value += min_distance;
		}
		igd_value /= pf_size;

		// free pf data memory
		for (int i = 0; i < pf_size; ++i)
			delete[] pfdata[i];
		delete[] pfdata;

		return 	igd_value;
	}

}