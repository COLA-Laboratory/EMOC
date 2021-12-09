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
	
	// this Run() is for command line mode and test module in gui mode
	void EMOCManager::Run()
	{
		if (is_para_set_ == false)
			std::cout << "Task parameters are not set, running with default parameters!\n";
		
		if (para_.is_open_multithread)
			EMOCMultiThreadRun();
		else
			EMOCSingleThreadRun();
	}

	// This ExperimentModuleRun() is for experiment module in gui mode
	void EMOCManager::ExperimentModuleRun(std::vector<EMOCParameters> experiment_tasks, int thread_num)
	{
		// Each experiment_task here can be separeted into 'run_num' small tasks which represent by EMOCExperimentTask
		
		// initialize experiment result
		multi_thread_result_history_.clear();
		for (int i = 0; i < experiment_tasks.size(); i++)
			multi_thread_result_history_.push_back(EMOCMultiThreadResult(experiment_tasks[i].runs_num));

		{
			std::lock_guard<std::mutex> locker(EMOCLock::multithread_data_mutex);
			is_multithread_result_ready_ = true;
		}

		for (int i = 0; i < experiment_tasks.size(); i++)
		{
			std::cout <<  "----------TASK PARAMETER " <<i <<" ---------\n";
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


		// update EMOC experiment module state
		{
			std::lock_guard<std::mutex> locker1(EMOCLock::experiment_finish_mutex);
			std::lock_guard<std::mutex> locker2(EMOCLock::experiment_pause_mutex);
			EMOCManager::Instance()->SetExperimentFinish(false);
			EMOCManager::Instance()->SetExperimentPause(false);
		}

		// EMOC experiment task running
		std::vector<std::thread> experiment_threads;
		for (int i = 0; i < thread_num; i++)
			experiment_threads.push_back(std::thread(&EMOCManager::ExperimentWorker, EMOCManager::Instance(), emoc_thread_tasks[i], i));

		for (int i = 0; i < experiment_threads.size(); i++)
			experiment_threads[i].join();

		// update EMOC experiment module state
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
		int output_interval = is_gui_ ? 1 : para_.output_interval;

		for (int run = 0; run < para_.runs_num; ++run)
		{
			// For default test module, we only use one thread, so always thread id 0.
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
			result.description = para_.algorithm_name + " on " + para_.problem_name + " Run" + std::to_string(count);
			result.last_igd = igd;
			result.last_hv = hv;
			result.runtime = g_GlobalSettingsArray[thread_id]->algorithm_->GetRuntime();
			result.pop_num = g_GlobalSettingsArray[thread_id]->algorithm_->GetRealPopNum();
			result.max_iteration = g_GlobalSettingsArray[thread_id]->iteration_num_;
			result.igd_history[result.max_iteration] = igd;
			result.hv_history[result.max_iteration] = hv;
			single_thread_result_historty_.push_back(result);
			if(is_gui_) UIPanelManager::Instance()->AddAvailSingleThreadResult(single_thread_result_historty_[count].description);

			printf("run %d time: %fs  igd: %f\n", run, (double)(end - start) / CLOCKS_PER_SEC, igd);
			printf("run %d time: %fs \n", run, result.runtime);

			// release the memory per run
			delete g_GlobalSettingsArray[thread_id];
		}
	}

	void EMOCManager::EMOCMultiThreadRun()
	{
		// initialize experiment result
		multi_thread_result_history_.clear();
		multi_thread_result_history_.push_back(EMOCMultiThreadResult(para_.runs_num));

		std::cout << "-------------------------------------\n";
		std::cout << para_.algorithm_name << "\n";
		std::cout << para_.problem_name << "\n";
		std::cout << para_.population_num << "\n";
		std::cout << para_.decision_num << "\n";
		std::cout << para_.objective_num << "\n";
		std::cout << para_.max_evaluation << "\n";
		std::cout << "-------------------------------------\n\n";
		
		if (para_.thread_num <= 0) para_.thread_num = 8;
		std::vector<std::vector<EMOCExperimentTask>> emoc_thread_tasks(para_.thread_num);

		int task_count = 0;
		for (int j = 0; j < para_.runs_num; j++)
		{
			EMOCExperimentTask t;
			t.para = para_;
			t.parameter_index = 0;
			t.run_index = j;
			emoc_thread_tasks[task_count % para_.thread_num].push_back(t);
			task_count++;
		}

		//// multithread running
		//{
		//	std::lock_guard<std::mutex> locker1(EMOCLock::experiment_finish_mutex);
		//	std::lock_guard<std::mutex> locker2(EMOCLock::experiment_pause_mutex);
		//	EMOCManager::Instance()->SetExperimentFinish(false);
		//	EMOCManager::Instance()->SetExperimentPause(false);
		//}

		std::vector<std::thread> experiment_threads;
		for (int i = 0; i < para_.thread_num; i++)
			experiment_threads.push_back(std::thread(&EMOCManager::ExperimentWorker, EMOCManager::Instance(), emoc_thread_tasks[i], i));

		for (int i = 0; i < experiment_threads.size(); i++)
			experiment_threads[i].join();

		//{
		//	std::lock_guard<std::mutex> locker1(EMOCLock::experiment_finish_mutex);
		//	EMOCManager::Instance()->SetExperimentFinish(true);
		//}
	}

	void EMOCManager::ExperimentWorker(std::vector<EMOCExperimentTask> tasks, int thread_id)
	{
		for (int i = 0; i < tasks.size(); i++)
		{
			// early stop
			if (is_experiment_finish_) break;

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

			std::string print_problem = problem + "_" + std::to_string(obj_num) + "_" + std::to_string(dec_num);
			printf("current thread id : %d, parameter: %d runs: %d, runtime: %f igd:%f algorithm:%s, problem:%s\n", 
				thread_id, parameter_index, run_index, runtime, igd, algorithm_name, print_problem.c_str());
			delete g_GlobalSettingsArray[thread_id];
		}

	}

	EMOCManager::EMOCManager() :
		para_(),
		is_para_set_(false),
		is_plot_(false),
		is_gui_(false),
		is_experiment_(false),
		is_test_finish_(true),
		is_test_pause_(false),
		is_experiment_pause_(false),
		is_experiment_finish_(true),
		is_multithread_result_ready_(false),
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