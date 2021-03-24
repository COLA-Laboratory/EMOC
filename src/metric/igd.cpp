#define _CRT_SECURE_NO_WARNINGS

#include "metric/igd.h"

#include <fstream>
#include <iostream>

#include "core/global.h"
#include "core/utility.h"


namespace emoc {

	static double **LoadPFData(int &pf_size, int obj_num, std::string &problem_name)
	{
		// reset pf_size
		pf_size = 0;
		
		

		// get problem name without number
		int pos = -1;
		for (auto c : problem_name)
		{
			pos++;
			if (c >= '0' && c <= '9')
				break;
		}
		
		std::string temp_problemname = problem_name.substr(0, pos);
		for (auto &c : temp_problemname)
		{
			if(c >= '0' && c <= '9') continue;
			c = tolower(c);
		}
		
		for (auto &c : problem_name)
		{
			if(c >= '0' && c <= '9') continue;
			c = tolower(c);
		}
		
		// open pf data file
		double **pf_data = nullptr;
		char pf_filename[255] = { 0 };
		sprintf(pf_filename, "pf_data/%s/%s.%dD.pf", temp_problemname.c_str(), problem_name.c_str(), obj_num);
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
				pf_data[i] = new double[obj_num];

			// read the pf data
			pf_file.clear();
			pf_file.seekg(0);
			for (int i = 0; i < pf_size; i++)
				for (int j = 0; j < obj_num; j++)
					pf_file >> pf_data[i][j];
		}

		return pf_data;
	}

	double CalculateIGD(Individual **pop, int pop_num, int obj_num, std::string &problem_name)
	{
		//char buff[1000];
		//_getcwd(buff, 1000);
		//std::cout << "current path£º" << buff << std::endl;

		// load pf data
		int pf_size = 0;
		double **pfdata = nullptr;
		pfdata = LoadPFData(pf_size,obj_num,problem_name);

		// calculate igd value
		double igd_value = 0;
		Individual *temp_ind = nullptr;
		double min_distance = 0, temp_distance = 0;

		for (int i = 0; i < pf_size; i++)
		{
			min_distance = INF;
			for (int j = 0; j < pop_num; j++)
			{
				temp_ind = pop[j];
				temp_distance = CalEuclidianDistance(pfdata[i],temp_ind->obj_, obj_num);

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