#include "core/emoc_manager.h"

#include <ctime>
#include <iostream>

#include "metric/igd.h"
#include "metric/hv.h"
#include "random/random.h"

namespace emoc {

	EMOCManager::Garbo EMOCManager::garbo_;
	EMOCManager* EMOCManager::s_Instance = nullptr;
	std::mutex EMOCManager::singleton_mutex_;

	emoc::EMOCManager* EMOCManager::Instance()
	{
		if (s_Instance == nullptr)
		{
			std::lock_guard<std::mutex> locker(singleton_mutex_);
			if (s_Instance == nullptr)
			{
				s_Instance = new EMOCManager();
			}
		}

		return s_Instance;
	}
	
	void EMOCManager::Run()
	{
		if (is_para_set_ == false)
			std::cout << "Task parameters are not set, running with default parameters!\n";

		if (para_.is_open_multithread)
			EMOCMultiThreadRun();
		else
			EMOCSingleThreadRun();
	}

	void EMOCManager::EMOCSingleThreadRun()
	{
		std::string algorithm_name = para_.algorithm_name;
		std::string problem_name = para_.problem_name;
		bool is_plotting = para_.is_plotting;
		int population_num = para_.population_num;
		int dec_num = para_.decision_num;
		int obj_num = para_.objective_num;
		int max_eval = para_.max_evaluation;
		int output_interval = para_.output_interval;


		for (int run = 0; run < para_.runs_num; ++run)
		{
			// for single thread test run, we use default 0 thread id
			int thread_id = 0;

			//run time recording
			clock_t start, end;
			start = clock();

			// algorithm main entity
			g_GlobalSettingsArray[thread_id] = new emoc::Global(algorithm_name.c_str(), problem_name.c_str(), population_num,
				dec_num, obj_num, max_eval, thread_id, output_interval, run);
			g_GlobalSettingsArray[thread_id]->Init();
			g_GlobalSettingsArray[thread_id]->SetPlot(is_plotting);
			g_GlobalSettingsArray[thread_id]->Start();

			end = clock();
			double time = (double)(end - start) / CLOCKS_PER_SEC;

			int obj_num = g_GlobalSettingsArray[thread_id]->obj_num_;
			double igd = CalculateIGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num, problem_name);
			double hv = CalculateHV(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num);

			EMOCSingleThreadResult result;
			int count = single_thread_result_historty_.size();
			result.para = para_;
			result.para.population_num = g_GlobalSettingsArray[thread_id]->algorithm_->GetRealPopNum();
			result.description = para_.algorithm_name + " on" + para_.problem_name + " Run" + std::to_string(count);
			result.last_igd = igd;
			result.last_hv = hv;
			result.runtime = time;
			single_thread_result_historty_.push_back(result);

			printf("run %d time: %fs  igd: %f\n", run, time, igd);
			printf("---------------------store file time %f--------------------\n", g_GlobalSettingsArray[thread_id]->RecordFileTime());

			// release the memory per run
			delete g_GlobalSettingsArray[thread_id];
		}
	}

	void EMOCManager::MultiThreadWorker(int run_start, int run_end, int thread_id)
	{
		const char* algorithm_name = para_.algorithm_name.c_str();
		const char* problem_name = para_.problem_name.c_str();
		bool is_plotting = para_.is_plotting;
		int population_num = para_.population_num;
		int dec_num = para_.decision_num;
		int obj_num = para_.objective_num;
		int max_eval = para_.max_evaluation;
		int output_interval = para_.output_interval;

		for (int run = run_start; run <= run_end; ++run)
		{
			// algorithm main entity
			g_GlobalSettingsArray[thread_id] = new emoc::Global(algorithm_name, problem_name, population_num, dec_num, obj_num, max_eval, thread_id, output_interval, run);
			g_GlobalSettingsArray[thread_id]->Init();
			g_GlobalSettingsArray[thread_id]->SetPlot(is_plotting);
			g_GlobalSettingsArray[thread_id]->Start();

			std::string problem_name = g_GlobalSettingsArray[thread_id]->problem_name_;
			int obj_num = g_GlobalSettingsArray[thread_id]->obj_num_;
			double igd = CalculateIGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num, problem_name);

			printf("current thread id : %d, runs: %d, igd:%f\n", thread_id, run, igd);
			delete g_GlobalSettingsArray[thread_id];
		}
	}

	void EMOCManager::EMOCMultiThreadRun()
	{
		int thread_num = para_.thread_num;
		std::vector<int> run_start_indexes;
		std::vector<int> run_end_indexes;
		std::vector<int> job_overload(thread_num, 0);

		// allocate runs to different threads
		int interval = (double)para_.runs_num / thread_num;
		int remainder = para_.runs_num % thread_num;
		for (int i = 0; i < thread_num; ++i)
		{
			job_overload[i] = interval;
			if (remainder-- > 0)
				job_overload[i]++;
		}

		// multithread running
		std::vector<std::thread> emoc_threads;
		int total_overload = 0;
		for (int i = 0; i < thread_num; ++i)
		{
			int run_start = 0, run_end = 0;
			if (job_overload[i] >= 0)
			{
				run_start = total_overload;
				run_end = total_overload + job_overload[i] - 1;
				total_overload += job_overload[i];
			}
			std::cout <<"start intex:" << run_start << "  end intex:" << run_end << "  \n";
			run_start_indexes.push_back(run_start);
			run_end_indexes.push_back(run_end);

			if (job_overload[i] > 0)
				emoc_threads.push_back(std::thread(&EMOCManager::MultiThreadWorker, EMOCManager::Instance(), run_start_indexes[i], run_end_indexes[i], i));
		}

		for (int i = 0; i < emoc_threads.size(); ++i)
			emoc_threads[i].join();

	}


	EMOCManager::EMOCManager() :
		para_(),
		is_para_set_(false),
		g_GlobalSettingsArray(MAX_THREAD_NUM,nullptr)
	{
		// for random number initialization
		randomize();

		for (int i = 0; i < MAX_THREAD_NUM; i++)
			g_GlobalSettingsArray[i] = nullptr;
		
	}

	EMOCManager::~EMOCManager()
	{

	}

}