#include "core/emoc_manager.h"

#include <ctime>
#include <iostream>

#include "emoc_app.h"
#include "metric/igd.h"
#include "metric/hv.h"
#include "random/random.h"
#include "ui/uipanel_manager.h"

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

	// each experiment_task here can be separeted into 'run_num' small tasks which represent by EMOCExperimentTask
	void EMOCManager::ExperimentModuleRun(std::vector<EMOCParameters> experiment_tasks, int thread_num)
	{

		// initialize experiment result
		multi_thread_result_history_.clear();
		for (int i = 0; i < experiment_tasks.size(); i++)
			multi_thread_result_history_.push_back(EMOCMultiThreadResult(experiment_tasks[i].runs_num));

		{
			std::lock_guard<std::mutex> locker(EMOCLock::multithread_data_mutex);
			is_multithread_result_ready = true;
		}

		for (int i = 0; i < experiment_tasks.size(); i++)
		{
			std::cout <<  "-------------------------------------\n";
			std::cout << experiment_tasks[i].algorithm_name << "\n";
			std::cout << experiment_tasks[i].problem_name << "\n";
			std::cout << experiment_tasks[i].population_num << "\n";
			std::cout << experiment_tasks[i].decision_num << "\n";
			std::cout << experiment_tasks[i].objective_num << "\n";
			std::cout << experiment_tasks[i].max_evaluation << "\n";
			std::cout << "-------------------------------------\n\n";
		}

		std::vector<std::vector<EMOCExperimentTask>> emoc_thread_tasks(thread_num);

		int task_count = 0;
		for (int i = 0; i < experiment_tasks.size(); i++)
		{
			for (int j = 0; j < experiment_tasks[i].runs_num; j++)
			{
				EMOCExperimentTask t;
				t.para = experiment_tasks[i];
				t.parameter_index = i;
				t.run_index = j;
				emoc_thread_tasks[task_count % thread_num].push_back(t);
				task_count++;
			}
		}


		// multithread running
		{
			std::lock_guard<std::mutex> locker1(EMOCLock::experiment_finish_mutex);
			std::lock_guard<std::mutex> locker2(EMOCLock::experiment_pause_mutex);
			EMOCManager::Instance()->SetExperimentFinish(false);
			EMOCManager::Instance()->SetExperimentPause(false);
		}

		std::vector<std::thread> experiment_threads;
		for (int i = 0; i < thread_num; i++)
			experiment_threads.push_back(std::thread(&EMOCManager::ExperimentWorker, EMOCManager::Instance(), emoc_thread_tasks[i], i));

		for (int i = 0; i < experiment_threads.size(); i++)
			experiment_threads[i].join();

		{
			std::lock_guard<std::mutex> locker1(EMOCLock::experiment_finish_mutex);
			EMOCManager::Instance()->SetExperimentFinish(true);
		}
	}

	void EMOCManager::EMOCSingleThreadRun()
	{
		clock_t start, end;
		std::string algorithm_name = para_.algorithm_name;
		std::string problem_name = para_.problem_name;
		bool is_plot = para_.is_plot;
		int population_num = para_.population_num;
		int dec_num = para_.decision_num;
		int obj_num = para_.objective_num;
		int max_eval = para_.max_evaluation;

		// In default test module, we record population each generation for the convinience of plotting.
		int output_interval = 1;     

		for (int run = 0; run < para_.runs_num; ++run)
		{
			// For default test module, we only use one thread, so always 0 thread id.
			int thread_id = 0;

			// create EMOC main entity
			start = clock();
			g_GlobalSettingsArray[thread_id] = new emoc::Global(algorithm_name.c_str(), problem_name.c_str(), population_num,
				dec_num, obj_num, max_eval, thread_id, output_interval, run);
			g_GlobalSettingsArray[thread_id]->Init();
			SetIsPlot(is_plot);
			g_GlobalSettingsArray[thread_id]->Start();
			end = clock();

			// collect results
			int obj_num = g_GlobalSettingsArray[thread_id]->obj_num_;
			double igd = CalculateIGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num, problem_name);
			double hv = CalculateHV(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num);

			EMOCSingleThreadResult result;
			int count = (int)single_thread_result_historty_.size();
			result.para = para_;
			result.para.population_num = g_GlobalSettingsArray[thread_id]->algorithm_->GetRealPopNum();
			result.description = para_.algorithm_name + " on" + para_.problem_name + " Run" + std::to_string(count);
			result.last_igd = igd;
			result.last_hv = hv;
			result.runtime = g_GlobalSettingsArray[thread_id]->algorithm_->GetRuntime();
			result.max_iteration = g_GlobalSettingsArray[thread_id]->iteration_num_;
			single_thread_result_historty_.push_back(result);

			printf("run %d time: %fs  igd: %f\n", run, (double)(end - start) / CLOCKS_PER_SEC, igd);
			// release the memory per run
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
		int interval = (int)((double)para_.runs_num / thread_num);
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

	void EMOCManager::MultiThreadWorker(int run_start, int run_end, int thread_id)
	{
		const char* algorithm_name = para_.algorithm_name.c_str();
		const char* problem_name = para_.problem_name.c_str();
		bool is_plot = para_.is_plot;
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
			SetIsPlot(is_plot);
			g_GlobalSettingsArray[thread_id]->Start();

			std::string problem_name = g_GlobalSettingsArray[thread_id]->problem_name_;
			int obj_num = g_GlobalSettingsArray[thread_id]->obj_num_;
			double igd = CalculateIGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num, problem_name);

			printf("current thread id : %d, runs: %d, igd:%f\n", thread_id, run, igd);
			delete g_GlobalSettingsArray[thread_id];
		}
	}

	void EMOCManager::ExperimentWorker(std::vector<EMOCExperimentTask> tasks, int thread_id)
	{
		for (int i = 0; i < tasks.size(); i++)
		{
			const char* algorithm_name = tasks[i].para.algorithm_name.c_str();
			const char* problem_name = tasks[i].para.problem_name.c_str();
			bool is_plot = tasks[i].para.is_plot;
			int population_num = tasks[i].para.population_num;
			int dec_num = tasks[i].para.decision_num;
			int obj_num = tasks[i].para.objective_num;
			int max_eval = tasks[i].para.max_evaluation;
			int output_interval = tasks[i].para.output_interval;
			int parameter_index = tasks[i].parameter_index;
			int run_index = tasks[i].run_index;

			// algorithm main entity
			g_GlobalSettingsArray[thread_id] = new emoc::Global(algorithm_name, problem_name, population_num, dec_num, obj_num, max_eval, thread_id, output_interval, run_index);
			g_GlobalSettingsArray[thread_id]->Init();
			SetIsPlot(is_plot);
			g_GlobalSettingsArray[thread_id]->Start();

			std::string problem = g_GlobalSettingsArray[thread_id]->problem_name_;
			double igd = CalculateIGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num, problem);
			double runtime = g_GlobalSettingsArray[thread_id]->algorithm_->GetRuntime();

			// In experiment module, we record the result when it is really finished
			if (g_GlobalSettingsArray[thread_id]->current_evaluation_ >= g_GlobalSettingsArray[thread_id]->max_evaluation_)
			{
				multi_thread_result_history_[parameter_index].runtime_history[run_index] = runtime;
				multi_thread_result_history_[parameter_index].is_runtime_record[run_index] = true;
				multi_thread_result_history_[parameter_index].igd_history[run_index] = igd;
				multi_thread_result_history_[parameter_index].is_igd_record[run_index] = true;
			}

			printf("current thread id : %d, parameter: %d runs: %d, runtime: %f igd:%f algorithm:%s, problem:%s\n", 
				thread_id, parameter_index, run_index, runtime, igd, algorithm_name, problem_name);
			delete g_GlobalSettingsArray[thread_id];
		}

	}

	EMOCManager::EMOCManager() :
		para_(),
		is_para_set_(false),
		is_plot_(false),
		is_gui_(false),
		is_experiment_(false),
		is_finish_(true),
		is_pause_(false),
		is_experiment_pause_(false),
		is_experiment_finish_(true),
		is_multithread_result_ready(false),
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