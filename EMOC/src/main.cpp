#include <ctime>
#include <cstdio>
#include <pthread.h>
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include "core/global.h"
#include "core/individual.h"
#include "problem/zdt.h"
#include "problem/dtlz.h"
#include "algorithms/moead/moead.h"
#include "algorithms/moead_de/moead_de.h"
#include "algorithms/nsga2/nsga2.h"
#include "algorithms/ibea/ibea.h"
#include "algorithms/spea2/spea2.h"
#include "algorithms/smsemoa/smsemoa.h"
#include "algorithms/moead_dra/moead_dra.h"
#include "algorithms/moead_frrmab/moead_frrmab.h"
#include "algorithms/hype/hype.h"
#include "metric/hv.h"
#include "metric/igd.h"
#include "metric/gd.h"
#include "metric/spacing.h"
#include "random/random.h"

#define NUM_THREADS 5
using emoc::g_GlobalSettingsArray;

//struct EMOCParameters
//{
//	std::string algorithm_name
//};

void *Work(void *args);


int main()
{
	int thread_id = 1;
	double igd_sum = 0;
	// initilize some bases for random number
	randomize();
	int run = 5;
	for (int i = 0; i < run; ++i)
	{
		// run time recording
		clock_t start, end;
		start = clock();

		// algorithm main entity
		g_GlobalSettingsArray[thread_id] = new emoc::Global("moeadfrrmab", "dtlz5", 100, 30, 3, 220000, thread_id);
		g_GlobalSettingsArray[thread_id]->Start();

		end = clock();
		double time = (double)(end - start) / CLOCKS_PER_SEC;

		std::string problem_name = g_GlobalSettingsArray[thread_id]->problem_name_;
		int obj_num = g_GlobalSettingsArray[thread_id]->obj_num_;
		double igd = emoc::CalculateIGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num, problem_name);
		double gd = emoc::CalculateGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num, problem_name);
		double hv = emoc::CalculateHV(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num);
		double spacing = emoc::CalculateSpacing(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num);
		igd_sum += igd;
		printf("igd : %f\n", igd);
		//printf("hv : %f\n", hv);
		//printf("gd : %f\n", gd);
		//printf("spacing : %f\n", spacing);
		printf("runtime : %fs\n", time);

		delete g_GlobalSettingsArray[0];
	}

	system("pause");
	return 0;


}

void *Work(void *args)
{
	int thread_id = 1;
	double igd_sum = 0;
	// initilize some bases for random number
	randomize();
	int run = 5;
	for (int i = 0; i < run; ++i)
	{
		// run time recording
		clock_t start, end;
		start = clock();

		// algorithm main entity
		g_GlobalSettingsArray[thread_id] = new emoc::Global("moeadfrrmab", "dtlz5", 100, 30, 3, 220000, thread_id);
		g_GlobalSettingsArray[thread_id]->Start();

		end = clock();
		double time = (double)(end - start) / CLOCKS_PER_SEC;

		std::string problem_name = g_GlobalSettingsArray[thread_id]->problem_name_;
		int obj_num = g_GlobalSettingsArray[thread_id]->obj_num_;
		double igd = emoc::CalculateIGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num, problem_name);
		double gd = emoc::CalculateGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num, problem_name);
		double hv = emoc::CalculateHV(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num);
		double spacing = emoc::CalculateSpacing(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num);
		igd_sum += igd;
		printf("igd : %f\n", igd);
		//printf("hv : %f\n", hv);
		//printf("gd : %f\n", gd);
		//printf("spacing : %f\n", spacing);
		printf("runtime : %fs\n", time);

		delete g_GlobalSettingsArray[0];
	}
}