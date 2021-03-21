#include <ctime>
#include <cstdio>
#include <pthread.h>
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include "core/file.h"
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
using emoc::EMOCParameters;


struct ThreadParamters
{
	EMOCParameters *para;

	int run_start;
	int run_end;
	int thread_id;
};

void *Work(void *args);
void EMOCMultiThreadTest(EMOCParameters *parameter);
void EMOCSingleThreadTest(EMOCParameters *parameter);

int main(int argc, char* argv[])
{	
	// initilize some bases for random number
	randomize();

	// initialize parameters
	emoc::EMOCParameters *parameter = new emoc::EMOCParameters(); // 这里只能用new，因为结构体里面有string，如果malloc的话不会做构造，所以malloc出来string是未初始化的，不能正确使用
	ParseParamerters(argc, argv, parameter);
	//ReadParametersFromFile("src/config/config.txt", parameter);
	parameter->igd_value = (double *)malloc(sizeof(double) * parameter->runs_num);

	std::cout << "current task:" << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "problem:              " << parameter->problem_name << std::endl;
	std::cout << "algorithm:            " << parameter->algorithm_name << std::endl;
	std::cout << "population number:    " << parameter->population_num << std::endl;
	std::cout << "decision number:      " << parameter->decision_num << std::endl;
	std::cout << "objective number:     " << parameter->objective_num << std::endl;
	std::cout << "evaluation:           " << parameter->max_evaluation << std::endl;
	std::cout << "runs:                 " << parameter->runs_num << std::endl;
	std::cout << "is open multithread:  " << parameter->is_open_multithread << std::endl;
	std::cout << "multithread number:   " << parameter->thread_num << std::endl;
	std::cout << "-------------------------------------------\n" << std::endl;

	clock_t start, end;
	start = clock();

	// EMOC test run
	if (parameter->is_open_multithread)
		EMOCMultiThreadTest(parameter);
	else
		EMOCSingleThreadTest(parameter);

	end = clock();
	double time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("total run time: %fs\n", time);

	for (int i = 0; i < parameter->runs_num; ++i)
	{
		printf("run %d igd value: %f \n", i, parameter->igd_value[i]);
	}

	delete parameter;

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
	int output_interval = parameter->para->output_interval;

	for (int run = parameter->run_start; run <= parameter->run_end; ++run)
	{
		int thread_id = parameter->thread_id;
		// run time recording
		//clock_t start, end;
		//start = clock();

		// algorithm main entity
		g_GlobalSettingsArray[thread_id] = new emoc::Global(algorithm_name, problem_name, population_num, dec_num, obj_num, max_eval, thread_id, output_interval, run);
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


		//RecordPop(run, 0, g_GlobalSettingsArray[thread_id]);

		delete g_GlobalSettingsArray[thread_id];
	}

	return nullptr;
}

void EMOCMultiThreadTest(EMOCParameters *parameter)
{
	int thread_num = parameter->thread_num;
	std::vector<ThreadParamters*> thread_para_array(thread_num, nullptr);
	for (int i = 0; i < thread_num; ++i)
	{
		thread_para_array[i] = (ThreadParamters *)malloc(sizeof(ThreadParamters));
		thread_para_array[i]->para = parameter;
	}

	std::vector<int> job_overload(thread_num, 0);
	int interval = (double)parameter->runs_num / thread_num;
	int remainder = parameter->runs_num % thread_num;
	for (int i = 0; i < thread_num; ++i)
	{
		job_overload[i] = interval;
		if (remainder-- > 0)
			job_overload[i]++;
		//printf("thread %d: %d runs\n",i, job_overload[i]);
	}

	// multithread running
	std::vector<pthread_t> tid(thread_num);
	int total_overload = 0;
	for (int i = 0; i < thread_num; ++i)
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

	for (int i = 0; i < thread_num; ++i)
	{
		if (job_overload[i] > 0)
			pthread_join(tid[i], nullptr);
	}


	for (int i = 0; i < thread_num; ++i)
		free(thread_para_array[i]);
}

void EMOCSingleThreadTest(EMOCParameters *parameter)
{
	const char *algorithm_name = parameter->algorithm_name.c_str();
	const char *problem_name = parameter->problem_name.c_str();

	int population_num = parameter->population_num;
	int dec_num = parameter->decision_num;
	int obj_num = parameter->objective_num;
	int max_eval = parameter->max_evaluation;
	int output_interval = parameter->output_interval;

	for (int run = 0; run < parameter->runs_num; ++run)
	{
		int thread_id = 0;
		//run time recording
		clock_t start, end;
		start = clock();

		// algorithm main entity
		g_GlobalSettingsArray[thread_id] = new emoc::Global(algorithm_name, problem_name, population_num, dec_num, obj_num, max_eval, thread_id, output_interval, run);
		g_GlobalSettingsArray[thread_id]->Start();

		end = clock();
		double time = (double)(end - start) / CLOCKS_PER_SEC;

		std::string problem_name = g_GlobalSettingsArray[thread_id]->problem_name_;
		int obj_num = g_GlobalSettingsArray[thread_id]->obj_num_;
		double igd = emoc::CalculateIGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num, problem_name);
		double gd = emoc::CalculateGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num, problem_name);
		double hv = emoc::CalculateHV(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num);
		double spacing = emoc::CalculateSpacing(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num);

		printf("run %d time: %fs\n",run, time);

		parameter->igd_value[run] = igd;
		//RecordPop(run, 0, g_GlobalSettingsArray[thread_id]);

		delete g_GlobalSettingsArray[thread_id];
	}
}