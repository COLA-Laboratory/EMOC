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

#define THREAD_NUM 8
using emoc::g_GlobalSettingsArray;

struct EMOCParameters
{
	std::string algorithm_name;
	std::string problem_name;
	int population_num;
	int decision_num;
	int objective_num;
	int max_evaluation;

	double *igd_value;
};

struct ThreadParamters
{
	EMOCParameters *para;

	int run_start;
	int run_end;
	int thread_id;
};

void *Work(void *args);


int main()
{	
	// initilize some bases for random number
	randomize();
	// paramater
	const char *algorithm_name = "moeadde";
	const char *problem_name = "zdt1";
	int population_num = 300;
	int dec_num = 30;
	int obj_num = 2;
	int max_eval = 300000;
	int runs = 26;

	EMOCParameters *parameter = new EMOCParameters(); // 这里只能用new，因为结构体里面有string，如果malloc的话不会做构造，所以malloc出来string是未初始化的，不能正确使用
	parameter->igd_value = (double*)malloc(sizeof(double) * runs);

	parameter->algorithm_name = algorithm_name;
	parameter->problem_name = problem_name;
	parameter->population_num = population_num;
	parameter->decision_num = dec_num;
	parameter->objective_num = obj_num;
	parameter->max_evaluation = max_eval;

	std::cout << parameter->algorithm_name << std::endl;
	std::cout << parameter->problem_name << std::endl;



	ThreadParamters *thread_para_array[THREAD_NUM];
	for (int i = 0; i < THREAD_NUM; ++i)
	{
		thread_para_array[i] = (ThreadParamters *)malloc(sizeof(ThreadParamters));
		thread_para_array[i]->para = parameter;
	}

	pthread_t tid[THREAD_NUM];
	int job_overload[THREAD_NUM];
	int interval = (double)runs / THREAD_NUM;
	int remainder = runs % THREAD_NUM;
	for (int i = 0; i < THREAD_NUM; ++i)
	{
		job_overload[i] = interval;
		if (remainder-- > 0)
			job_overload[i]++;

		printf("thread %d: %d runs\n",i, job_overload[i]);
	}


	int total_overload = 0;
	for (int i = 0; i < THREAD_NUM; ++i)
	{
		if (job_overload[i] > 0)
		{
			thread_para_array[i]->run_start = total_overload;
			thread_para_array[i]->run_end = total_overload + job_overload[i] - 1;
			thread_para_array[i]->thread_id = i;
			total_overload += job_overload[i];
		}
		else
			continue;

		pthread_create(&tid[i], nullptr, Work, (void *)thread_para_array[i]);
	}

	for (int i = 0; i < THREAD_NUM; ++i)
	{
		if (job_overload[i] > 0)
			pthread_join(tid[i], nullptr);
	}

	for (int i = 0; i < runs; ++i)
	{
		printf("runs %d : %f \n", i, parameter->igd_value[i]);
	}

	system("pause");
	return 0;


}

void *Work(void *args)
{
	ThreadParamters *parameter = (ThreadParamters *)args;
	const char *algorithm_name = parameter->para->algorithm_name.c_str();
	const char *problem_name = parameter->para->problem_name.c_str();

	int population_num = parameter->para->population_num;
	int dec_num = parameter->para->decision_num;
	int obj_num = parameter->para->objective_num;
	int max_eval = parameter->para->max_evaluation;

	for (int run = parameter->run_start; run <= parameter->run_end; ++run)
	{
		int thread_id = parameter->thread_id;
		// run time recording
		//clock_t start, end;
		//start = clock();

		// algorithm main entity
		g_GlobalSettingsArray[thread_id] = new emoc::Global(algorithm_name, problem_name, population_num, dec_num, obj_num, max_eval, thread_id);
		g_GlobalSettingsArray[thread_id]->Start();

		//end = clock();
		//double time = (double)(end - start) / CLOCKS_PER_SEC;

		std::string problem_name = g_GlobalSettingsArray[thread_id]->problem_name_;
		int obj_num = g_GlobalSettingsArray[thread_id]->obj_num_;
		double igd = emoc::CalculateIGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num, problem_name);
		//double gd = emoc::CalculateGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num, problem_name);
		//double hv = emoc::CalculateHV(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num);
		//double spacing = emoc::CalculateSpacing(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num);

		printf("current thread id : %d, runs: %d, igd:%f\n", thread_id, run, igd);
		parameter->para->igd_value[run] = igd;
		//printf("igd : %f\n", igd);
		////printf("hv : %f\n", hv);
		////printf("gd : %f\n", gd);
		////printf("spacing : %f\n", spacing);
		//printf("runtime : %fs\n", time);

		delete g_GlobalSettingsArray[thread_id];
	}

	return nullptr;
}