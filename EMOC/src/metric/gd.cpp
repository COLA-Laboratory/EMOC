#define _CRT_SECURE_NO_WARNINGS

#include "metric/gd.h"

#include <string>
#include <fstream>
#include <iostream>

#include "core/global.h"
#include "core/utility.h"


namespace emoc {

	static double **LoadPFData(int &pf_size)
	{
		// reset pf_size
		pf_size = 0;

		// get problem name without number
		int pos = -1;
		for (auto c : g_GlobalSettings->problem_name_)
		{
			pos++;
			if (c >= '0' && c <= '9')
				break;
		}
		std::string temp_problemname = g_GlobalSettings->problem_name_.substr(0, pos);

		// open pf data file
		double **pf_data = nullptr;
		char pf_filename[255] = { 0 };
		sprintf(pf_filename, "pf_data/%s/%s.%dD.pf", temp_problemname.c_str(), g_GlobalSettings->problem_name_.c_str(), g_GlobalSettings->obj_num_);
		std::fstream pf_file(pf_filename);


		if (!pf_file)
		{
			std::cerr << pf_filename << " file doesn't exist!\n";
		}
		else
		{
			std::string line;
			while (getline(pf_file, line))
				pf_size++;

			// init memory for pf data
			pf_data = new double*[pf_size];
			for (int i = 0; i < pf_size; ++i)
				pf_data[i] = new double[g_GlobalSettings->obj_num_];

			// read the pf data
			pf_file.clear();
			pf_file.seekg(0);
			for (int i = 0; i < pf_size; i++)
				for (int j = 0; j < g_GlobalSettings->obj_num_; j++)
					pf_file >> pf_data[i][j];
		}

		return pf_data;
	}

	double CalculateGD(Individual **pop, int pop_num)
	{
		// load pf data
		int pf_size = 0;
		double **pfdata = nullptr;
		pfdata = LoadPFData(pf_size);

		// calculate igd value
		double gd_value = 0;
		Individual *temp_ind = nullptr;
		double min_distance = 0, temp_distance = 0;

		for (int i = 0; i < pop_num; i++)
		{
			min_distance = INF;
			temp_ind = pop[i];
			for (int j = 0; j < pf_size; j++)
			{
				temp_distance = CalEuclidianDistance(pfdata[j], temp_ind->obj_, g_GlobalSettings->obj_num_);

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

		return 	gd_value;
	}

}


