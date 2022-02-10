#include "core/emoc_manager.h"

#include <ctime>
#include <algorithm>
#include <iostream>
#include <thread>

#include "emoc_app.h"
#include "random/random.h"
#include "ui/ui_utility.h"
#include "ui/uipanel_manager.h"
#include "alglib/src/statistics.h"
#include "metric/metric_head_collect.h"
#include "algorithms/algorithm_head_collect.h"
#include "problem/problem_head_collect.h"

namespace emoc {

	EMOCManager* EMOCManager::s_Instance = nullptr;
	std::mutex EMOCManager::singleton_mutex_;
	EMOCManager::Garbo EMOCManager::garbo_;


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
			if(!emoc_thread_tasks[i].empty())
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
		int output_interval = para_.output_interval;


		HVCalculator hv_calculator;
		hv_calculator.Init(obj_num, population_num);

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
			int pop_num = g_GlobalSettingsArray[thread_id]->algorithm_->GetRealPopNum();
			double igd = CalculateIGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), pop_num, obj_num, problem_name);
			double hv = hv_calculator.Calculate(g_GlobalSettingsArray[thread_id]->parent_population_.data(), pop_num, obj_num, problem_name);
			double gd = CalculateGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), pop_num, obj_num, problem_name);
			double spacing = CalculateSpacing(g_GlobalSettingsArray[thread_id]->parent_population_.data(), pop_num, obj_num);
			double igdplus = CalculateIGDPlus(g_GlobalSettingsArray[thread_id]->parent_population_.data(), pop_num, obj_num, problem_name);
			double gdplus = CalculateGDPlus(g_GlobalSettingsArray[thread_id]->parent_population_.data(), pop_num, obj_num, problem_name);

			EMOCSingleThreadResult result;
			int count = (int)single_thread_result_historty_.size();
			result.para = para_;
			result.description = para_.algorithm_name + " on " + para_.problem_name + " Run" + std::to_string(count);
			result.last_igd = igd;
			result.last_hv = hv;
			result.last_gd = gd;
			result.last_spacing = spacing;
			result.last_igdplus = igdplus;
			result.last_gdplus = gdplus;
			result.runtime = g_GlobalSettingsArray[thread_id]->algorithm_->GetRuntime();
			result.pop_num = g_GlobalSettingsArray[thread_id]->algorithm_->GetRealPopNum();
			result.max_iteration = g_GlobalSettingsArray[thread_id]->iteration_num_;
			result.igd_history[result.max_iteration] = igd;
			result.hv_history[result.max_iteration] = hv;
			single_thread_result_historty_.push_back(result);
			if(is_gui_) UIPanelManager::Instance()->AddAvailSingleThreadResult(single_thread_result_historty_[count].description);

			//printf("run %d time: %fs  igd: %f\n", run, (double)(end - start) / CLOCKS_PER_SEC, igd);
			printf("run %d time: %fs   igd: %f \n", run, result.runtime, igd);

			// release the memory per run
			delete g_GlobalSettingsArray[thread_id];
		}

		hv_calculator.CleanUp();
	}

	void EMOCManager::EMOCMultiThreadRun()
	{
		// initialize experiment result
		multi_thread_result_history_.clear();
		multi_thread_result_history_.push_back(EMOCMultiThreadResult(para_.runs_num));
		
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
        
		// update EMOC experiment module state (it is also necessary for none gui mode)
		{
			std::lock_guard<std::mutex> locker1(EMOCLock::experiment_finish_mutex);
			std::lock_guard<std::mutex> locker2(EMOCLock::experiment_pause_mutex);
			EMOCManager::Instance()->SetExperimentFinish(false);
			EMOCManager::Instance()->SetExperimentPause(false);
		}

		std::vector<std::thread> experiment_threads;
		for (int i = 0; i < para_.thread_num; i++)
			if(!emoc_thread_tasks[i].empty())
				experiment_threads.push_back(std::thread(&EMOCManager::ExperimentWorker, EMOCManager::Instance(), emoc_thread_tasks[i], i));

		for (int i = 0; i < experiment_threads.size(); i++)
				experiment_threads[i].join();

		// update EMOC experiment module state (it is also necessary for none gui mode)
		{
			std::lock_guard<std::mutex> locker1(EMOCLock::experiment_finish_mutex);
			EMOCManager::Instance()->SetExperimentFinish(true);
		}

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

			HVCalculator hv_calculator;
			hv_calculator.Init(obj_num, population_num);

			// algorithm main entity
			g_GlobalSettingsArray[thread_id] = new emoc::Global(algorithm_name, problem_name, population_num, dec_num, obj_num, max_eval, thread_id, output_interval, run_index);
			g_GlobalSettingsArray[thread_id]->Init();
			SetIsPlot(is_plot);
			g_GlobalSettingsArray[thread_id]->Start();

			std::string problem = g_GlobalSettingsArray[thread_id]->problem_name_;
			int pop_num = g_GlobalSettingsArray[thread_id]->algorithm_->GetRealPopNum();
			double igd = CalculateIGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), pop_num, obj_num, problem);
			double hv = hv_calculator.Calculate(g_GlobalSettingsArray[thread_id]->parent_population_.data(), pop_num, obj_num, problem);
			double gd = CalculateGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), pop_num, obj_num, problem);
			double spacing = CalculateSpacing(g_GlobalSettingsArray[thread_id]->parent_population_.data(), pop_num, obj_num);
			double igdplus = CalculateIGDPlus(g_GlobalSettingsArray[thread_id]->parent_population_.data(), pop_num, obj_num, problem);
			double gdplus = CalculateGDPlus(g_GlobalSettingsArray[thread_id]->parent_population_.data(), pop_num, obj_num, problem);
			double runtime = g_GlobalSettingsArray[thread_id]->algorithm_->GetRuntime();

			// In experiment module, we record the result when it is really finished
			if (g_GlobalSettingsArray[thread_id]->current_evaluation_ >= g_GlobalSettingsArray[thread_id]->max_evaluation_)
			{
				multi_thread_result_history_[parameter_index].runtime.metric_history[run_index] = runtime;
				multi_thread_result_history_[parameter_index].igd.metric_history[run_index] = igd;
				multi_thread_result_history_[parameter_index].hv.metric_history[run_index] = hv;
				multi_thread_result_history_[parameter_index].gd.metric_history[run_index] = gd;
				multi_thread_result_history_[parameter_index].spacing.metric_history[run_index] = spacing;
				multi_thread_result_history_[parameter_index].igdplus.metric_history[run_index] = igdplus;
				multi_thread_result_history_[parameter_index].gdplus.metric_history[run_index] = gdplus;
				multi_thread_result_history_[parameter_index].runtime.is_record[run_index] = true;
				multi_thread_result_history_[parameter_index].igd.is_record[run_index] = true;
				multi_thread_result_history_[parameter_index].hv.is_record[run_index] = true;
				multi_thread_result_history_[parameter_index].gd.is_record[run_index] = true;
				multi_thread_result_history_[parameter_index].spacing.is_record[run_index] = true;
				multi_thread_result_history_[parameter_index].igdplus.is_record[run_index] = true;
				multi_thread_result_history_[parameter_index].gdplus.is_record[run_index] = true;

				// update statistic result only in gui mode
				if (EMOCManager::Instance()->GetIsGUI())
				{
					// update counter, mean, std, etc...
					UpdateExpResult(multi_thread_result_history_[parameter_index], run_index, parameter_index);

					// update hypothesis test data
					UpdateExpStatTest(parameter_index);
				}
			}

			std::string print_problem = problem + "_" + std::to_string(obj_num) + "_" + std::to_string(dec_num);
			printf("current thread id : %d, parameter: %d runs: %d, runtime: %f igd:%f algorithm:%s, problem:%s\n", 
				thread_id, parameter_index, run_index, runtime, igd, algorithm_name, print_problem.c_str());
			delete g_GlobalSettingsArray[thread_id];
			hv_calculator.CleanUp();
		}

	}

	void EMOCManager::UpdateExpStatTest(int parameter_index)
	{
		std::lock_guard<std::mutex> locker(EMOCLock::mutex_pool[parameter_index % EMOCLock::mutex_pool.size()]);
		int range_index = parameter_index / UIPanelManager::Instance()->GetExpAlgorithmNum();
		int range_start = range_index * UIPanelManager::Instance()->GetExpAlgorithmNum(), range_end = (range_index + 1) * UIPanelManager::Instance()->GetExpAlgorithmNum();
		
		bool is_ready = true;
		for (int i = range_start; i < range_end; i++)
			if (multi_thread_result_history_[i].valid_res_count < multi_thread_result_history_[i].runtime.metric_history.size())
				is_ready = false;

		// Only do statistic test the when datas in the same row are ready. 
		if (!is_ready) return;

		// Take the last column algorithm to be the compared object as default.
		int compared_parameter_index = range_end - 1;
		EMOCMultiThreadResult& default_compared_res = multi_thread_result_history_[compared_parameter_index];
		int igdmean_best_index = GetBestParameterIndex(range_start, range_end, "IGD", "Mean");
		int igdmedian_best_index = GetBestParameterIndex(range_start, range_end, "IGD", "Median");
		int runtimemean_best_index = GetBestParameterIndex(range_start, range_end, "Runtime", "Mean");
		int runtimemedian_best_index = GetBestParameterIndex(range_start, range_end, "Runtime", "Median");
		int hvmean_best_index = GetBestParameterIndex(range_start, range_end, "HV", "Mean");
		int hvmedian_best_index = GetBestParameterIndex(range_start, range_end, "HV", "Median");
		int gdmean_best_index = GetBestParameterIndex(range_start, range_end, "GD", "Mean");
		int gdmedian_best_index = GetBestParameterIndex(range_start, range_end, "GD", "Median");
		int spacingmean_best_index = GetBestParameterIndex(range_start, range_end, "Spacing", "Mean");
		int spacingmedian_best_index = GetBestParameterIndex(range_start, range_end, "Spacing", "Median");
		int igdplusmean_best_index = GetBestParameterIndex(range_start, range_end, "IGDPlus", "Mean");
		int igdplusmedian_best_index = GetBestParameterIndex(range_start, range_end, "IGDPlus", "Median");
		int gdplusmean_best_index = GetBestParameterIndex(range_start, range_end, "GDPlus", "Mean");
		int gdplusmedian_best_index = GetBestParameterIndex(range_start, range_end, "GDPlus", "Median");

		for (int i = range_start; i < range_end; i++)
		{
			EMOCMultiThreadResult& res = multi_thread_result_history_[i];

			// default comparision
			if (i != range_end - 1)
			{
				StatisticTestAccordingMetric(res, default_compared_res, "Runtime", "Default");
				StatisticTestAccordingMetric(res, default_compared_res, "IGD", "Default");
				StatisticTestAccordingMetric(res, default_compared_res, "HV", "Default");
				StatisticTestAccordingMetric(res, default_compared_res, "GD", "Default");
				StatisticTestAccordingMetric(res, default_compared_res, "Spacing", "Default");
				StatisticTestAccordingMetric(res, default_compared_res, "IGDPlus", "Default");
				StatisticTestAccordingMetric(res, default_compared_res, "GDPlus", "Default");
			}

			// mean best comparision
			if (i != runtimemean_best_index)
			{
				EMOCMultiThreadResult& meanbest_compared_res = multi_thread_result_history_[runtimemean_best_index];
				StatisticTestAccordingMetric(res, meanbest_compared_res, "Runtime", "Mean");
			}
			if (i != igdmean_best_index)
			{
				EMOCMultiThreadResult& meanbest_compared_res = multi_thread_result_history_[igdmean_best_index];
				StatisticTestAccordingMetric(res, meanbest_compared_res, "IGD", "Mean");
			}
			if (i != hvmean_best_index)
			{
				EMOCMultiThreadResult& meanbest_compared_res = multi_thread_result_history_[hvmean_best_index];
				StatisticTestAccordingMetric(res, meanbest_compared_res, "HV", "Mean");
			}
			if (i != gdmean_best_index)
			{
				EMOCMultiThreadResult& meanbest_compared_res = multi_thread_result_history_[gdmean_best_index];
				StatisticTestAccordingMetric(res, meanbest_compared_res, "GD", "Mean");
			}
			if (i != spacingmean_best_index)
			{
				EMOCMultiThreadResult& meanbest_compared_res = multi_thread_result_history_[spacingmean_best_index];
				StatisticTestAccordingMetric(res, meanbest_compared_res, "Spacing", "Mean");
			}
			if (i != igdplusmean_best_index)
			{
				EMOCMultiThreadResult& meanbest_compared_res = multi_thread_result_history_[igdplusmean_best_index];
				StatisticTestAccordingMetric(res, meanbest_compared_res, "IGDPlus", "Mean");
			}
			if (i != gdplusmean_best_index)
			{
				EMOCMultiThreadResult& meanbest_compared_res = multi_thread_result_history_[gdplusmean_best_index];
				StatisticTestAccordingMetric(res, meanbest_compared_res, "GDPlus", "Mean");
			}

			// median best comparision
			if (i != runtimemedian_best_index)
			{
				EMOCMultiThreadResult& medianbest_compared_res = multi_thread_result_history_[runtimemedian_best_index];
				StatisticTestAccordingMetric(res, medianbest_compared_res, "Runtime", "Median");
			}
			if (i != igdmedian_best_index)
			{
				EMOCMultiThreadResult& medianbest_compared_res = multi_thread_result_history_[igdmedian_best_index];
				StatisticTestAccordingMetric(res, medianbest_compared_res, "IGD", "Median");
			}
			if (i != hvmedian_best_index)
			{
				EMOCMultiThreadResult& medianbest_compared_res = multi_thread_result_history_[hvmedian_best_index];
				StatisticTestAccordingMetric(res, medianbest_compared_res, "HV", "Median");
			}
			if (i != gdmedian_best_index)
			{
				EMOCMultiThreadResult& medianbest_compared_res = multi_thread_result_history_[gdmedian_best_index];
				StatisticTestAccordingMetric(res, medianbest_compared_res, "GD", "Median");
			}
			if (i != spacingmedian_best_index)
			{
				EMOCMultiThreadResult& medianbest_compared_res = multi_thread_result_history_[spacingmedian_best_index];
				StatisticTestAccordingMetric(res, medianbest_compared_res, "Spacing", "Median");
			}
			if (i != igdplusmedian_best_index)
			{
				EMOCMultiThreadResult& medianbest_compared_res = multi_thread_result_history_[igdplusmedian_best_index];
				StatisticTestAccordingMetric(res, medianbest_compared_res, "IGDPlus", "Median");
			}
			if (i != gdplusmedian_best_index)
			{
				EMOCMultiThreadResult& medianbest_compared_res = multi_thread_result_history_[gdplusmedian_best_index];
				StatisticTestAccordingMetric(res, medianbest_compared_res, "GDPlus", "Median");
			}
		}
	}

	void EMOCManager::StatisticTestAccordingMetric(EMOCMultiThreadResult& res, EMOCMultiThreadResult& compared_res, const std::string& metric, const std::string& format)
	{
		int index = 2;
		if (format == "Mean") index = 0;
		else if (format == "Median") index = 1;

		if (metric == "Runtime")
		{
			bool is_diff_ranksum = RankSumTest(res.runtime.metric_history, compared_res.runtime.metric_history);
			bool is_diff_signrank = SignRankTest(res.runtime.metric_history, compared_res.runtime.metric_history);
			if (res.runtime.metric_mean_ranksum[index] == -2) res.runtime.metric_mean_ranksum[index] = is_diff_ranksum ? (res.runtime.metric_mean < compared_res.runtime.metric_mean ? 1 : -1) : 0;
			if (res.runtime.metric_median_ranksum[index] == -2)res.runtime.metric_median_ranksum[index] = is_diff_ranksum ? (res.runtime.metric_median < compared_res.runtime.metric_median ? 1 : -1) : 0;
			if (res.runtime.metric_mean_signrank[index] == -2) res.runtime.metric_mean_signrank[index] = is_diff_signrank ? (res.runtime.metric_mean < compared_res.runtime.metric_mean ? 1 : -1) : 0;
			if (res.runtime.metric_median_signrank[index] == -2)res.runtime.metric_median_signrank[index] = is_diff_signrank ? (res.runtime.metric_median < compared_res.runtime.metric_median ? 1 : -1) : 0;
		}
		else if (metric == "IGD")
		{
			bool is_diff_ranksum = RankSumTest(res.igd.metric_history, compared_res.igd.metric_history);
			bool is_diff_signrank = SignRankTest(res.igd.metric_history, compared_res.igd.metric_history);
			if (res.igd.metric_mean_ranksum[index] == -2) res.igd.metric_mean_ranksum[index] = is_diff_ranksum ? (res.igd.metric_mean < compared_res.igd.metric_mean ? 1 : -1) : 0;
			if (res.igd.metric_median_ranksum[index] == -2)res.igd.metric_median_ranksum[index] = is_diff_ranksum ? (res.igd.metric_median < compared_res.igd.metric_median ? 1 : -1) : 0;
			if (res.igd.metric_mean_signrank[index] == -2) res.igd.metric_mean_signrank[index] = is_diff_signrank ? (res.igd.metric_mean < compared_res.igd.metric_mean ? 1 : -1) : 0;
			if (res.igd.metric_median_signrank[index] == -2)res.igd.metric_median_signrank[index] = is_diff_signrank ? (res.igd.metric_median < compared_res.igd.metric_median ? 1 : -1) : 0;
		}
		else if (metric == "HV")
		{
			// note that hv is the bigger the better
			bool is_diff_ranksum = RankSumTest(res.hv.metric_history, compared_res.hv.metric_history);
			bool is_diff_signrank = SignRankTest(res.hv.metric_history, compared_res.hv.metric_history);
			if (res.hv.metric_mean_ranksum[index] == -2) res.hv.metric_mean_ranksum[index] = is_diff_ranksum ? (res.hv.metric_mean > compared_res.hv.metric_mean ? 1 : -1) : 0;
			if (res.hv.metric_median_ranksum[index] == -2)res.hv.metric_median_ranksum[index] = is_diff_ranksum ? (res.hv.metric_median > compared_res.hv.metric_median ? 1 : -1) : 0;
			if (res.hv.metric_mean_signrank[index] == -2) res.hv.metric_mean_signrank[index] = is_diff_signrank ? (res.hv.metric_mean > compared_res.hv.metric_mean ? 1 : -1) : 0;
			if (res.hv.metric_median_signrank[index] == -2)res.hv.metric_median_signrank[index] = is_diff_signrank ? (res.hv.metric_median > compared_res.hv.metric_median ? 1 : -1) : 0;
		}
		else if(metric == "GD")
		{
			bool is_diff_ranksum = RankSumTest(res.gd.metric_history, compared_res.gd.metric_history);
			bool is_diff_signrank = SignRankTest(res.gd.metric_history, compared_res.gd.metric_history);
			if (res.gd.metric_mean_ranksum[index] == -2) res.gd.metric_mean_ranksum[index] = is_diff_ranksum ? (res.gd.metric_mean < compared_res.gd.metric_mean ? 1 : -1) : 0;
			if (res.gd.metric_median_ranksum[index] == -2)res.gd.metric_median_ranksum[index] = is_diff_ranksum ? (res.gd.metric_median < compared_res.gd.metric_median ? 1 : -1) : 0;
			if (res.gd.metric_mean_signrank[index] == -2) res.gd.metric_mean_signrank[index] = is_diff_signrank ? (res.gd.metric_mean < compared_res.gd.metric_mean ? 1 : -1) : 0;
			if (res.gd.metric_median_signrank[index] == -2)res.gd.metric_median_signrank[index] = is_diff_signrank ? (res.gd.metric_median < compared_res.gd.metric_median ? 1 : -1) : 0;
		}
		else if (metric == "Spacing")
		{
			bool is_diff_ranksum = RankSumTest(res.spacing.metric_history, compared_res.spacing.metric_history);
			bool is_diff_signrank = SignRankTest(res.spacing.metric_history, compared_res.spacing.metric_history);
			if (res.spacing.metric_mean_ranksum[index] == -2) res.spacing.metric_mean_ranksum[index] = is_diff_ranksum ? (res.spacing.metric_mean < compared_res.spacing.metric_mean ? 1 : -1) : 0;
			if (res.spacing.metric_median_ranksum[index] == -2)res.spacing.metric_median_ranksum[index] = is_diff_ranksum ? (res.spacing.metric_median < compared_res.spacing.metric_median ? 1 : -1) : 0;
			if (res.spacing.metric_mean_signrank[index] == -2) res.spacing.metric_mean_signrank[index] = is_diff_signrank ? (res.spacing.metric_mean < compared_res.spacing.metric_mean ? 1 : -1) : 0;
			if (res.spacing.metric_median_signrank[index] == -2)res.spacing.metric_median_signrank[index] = is_diff_signrank ? (res.spacing.metric_median < compared_res.spacing.metric_median ? 1 : -1) : 0;
		}
		else if (metric == "IGDPlus")
		{
			bool is_diff_ranksum = RankSumTest(res.igdplus.metric_history, compared_res.igdplus.metric_history);
			bool is_diff_signrank = SignRankTest(res.igdplus.metric_history, compared_res.igdplus.metric_history);
			if (res.igdplus.metric_mean_ranksum[index] == -2) res.igdplus.metric_mean_ranksum[index] = is_diff_ranksum ? (res.igdplus.metric_mean < compared_res.igdplus.metric_mean ? 1 : -1) : 0;
			if (res.igdplus.metric_median_ranksum[index] == -2)res.igdplus.metric_median_ranksum[index] = is_diff_ranksum ? (res.igdplus.metric_median < compared_res.igdplus.metric_median ? 1 : -1) : 0;
			if (res.igdplus.metric_mean_signrank[index] == -2) res.igdplus.metric_mean_signrank[index] = is_diff_signrank ? (res.igdplus.metric_mean < compared_res.igdplus.metric_mean ? 1 : -1) : 0;
			if (res.igdplus.metric_median_signrank[index] == -2)res.igdplus.metric_median_signrank[index] = is_diff_signrank ? (res.igdplus.metric_median < compared_res.igdplus.metric_median ? 1 : -1) : 0;
		}
		else if (metric == "GDPlus")
		{
			bool is_diff_ranksum = RankSumTest(res.gdplus.metric_history, compared_res.gdplus.metric_history);
			bool is_diff_signrank = SignRankTest(res.gdplus.metric_history, compared_res.gdplus.metric_history);
			if (res.gdplus.metric_mean_ranksum[index] == -2) res.gdplus.metric_mean_ranksum[index] = is_diff_ranksum ? (res.gdplus.metric_mean < compared_res.gdplus.metric_mean ? 1 : -1) : 0;
			if (res.gdplus.metric_median_ranksum[index] == -2)res.gdplus.metric_median_ranksum[index] = is_diff_ranksum ? (res.gdplus.metric_median < compared_res.gdplus.metric_median ? 1 : -1) : 0;
			if (res.gdplus.metric_mean_signrank[index] == -2) res.gdplus.metric_mean_signrank[index] = is_diff_signrank ? (res.gdplus.metric_mean < compared_res.gdplus.metric_mean ? 1 : -1) : 0;
			if (res.gdplus.metric_median_signrank[index] == -2)res.gdplus.metric_median_signrank[index] = is_diff_signrank ? (res.gdplus.metric_median < compared_res.gdplus.metric_median ? 1 : -1) : 0;
		}
		else
		{
			// TODO... ADD MORE METRICS

		}
	}


	int EMOCManager::GetBestParameterIndex(int start, int end, const std::string& metric, const std::string& format)
	{
		int res = start;
		double best_value1 = 0.0f;
		double best_value2 = 0.0f;
	
		GetComparedMetric(metric, format, start, best_value1, best_value2);

		bool is_min_better = true;
		if (metric == "HV") is_min_better = false;
		for (int i = start + 1; i < end; i++)
		{
			double current_value1, current_value2;
			GetComparedMetric(metric, format, i, current_value1, current_value2);
			if ((is_min_better && best_value1 > current_value1) || (!is_min_better && best_value1 < current_value1))
			{
				res = i;
				best_value1 = current_value1;
				best_value2 = current_value2;
			}
			else if (std::fabs(best_value1 - current_value1) < EMOC_EPS)
			{
				if (format == "Median")
				{
					if ((is_min_better && best_value2 > current_value2) || (!is_min_better && best_value2 < current_value2))
					{
						res = i;
						best_value1 = current_value1;
						best_value2 = current_value2;
					}
				}
				else if (format == "Mean")  // standard deviation is still the smaller the better
				{
					if (best_value2 > current_value2)
					{
						res = i;
						best_value1 = current_value1;
						best_value2 = current_value2;
					}
				}
			}
		}

		return res;
	}


	int EMOCManager::RankSumTest(const std::vector<double>& array1, const std::vector<double>& array2)
	{
		int res = 0;

		alglib::real_1d_array a1, a2;
		a1.setcontent(array1.size(), array1.data());
		a2.setcontent(array2.size(), array2.data());
		double p1, p2, p3;
		alglib::mannwhitneyutest(a1, array1.size(), a2, array2.size(), p1, p2, p3);

		//if (count == 1) std::cout << p1 << " " << p2 << " " << p3 << "\n";

		if (p1 > 0.05)
			res = 0;
		else
			res = 1;

		return res;
	}

	int EMOCManager::SignRankTest(const std::vector<double>& array1, const std::vector<double>& array2)
	{
		int res = 0;

		alglib::real_1d_array a;
		a.setlength(array1.size());
		for (int i = 0; i < array1.size();i++)
			a[i] = array1[i] - array2[i];

		double p1, p2, p3;
		alglib::wilcoxonsignedranktest(a, array1.size(), 0, p1, p2, p3);


		if (p1 > 0.05)
			res = 0;
		else
			res = -1;

		return res;
	}

	void EMOCManager::UpdateExpResult(EMOCMultiThreadResult& res, int new_res_index, int parameter_index)
	{
		std::lock_guard<std::mutex> locker(EMOCLock::mutex_pool[parameter_index % EMOCLock::mutex_pool.size()]);
		res.valid_res_count++;

		UpdateExpMetricStat(res.igd.metric_history, res.igd.is_record, res.igd.metric_mean, res.igd.metric_std, res.igd.metric_median, res.igd.metric_iqr);
		UpdateExpMetricStat(res.hv.metric_history, res.hv.is_record, res.hv.metric_mean, res.hv.metric_std, res.hv.metric_median, res.hv.metric_iqr);
		UpdateExpMetricStat(res.runtime.metric_history, res.runtime.is_record, res.runtime.metric_mean, res.runtime.metric_std, res.runtime.metric_median, res.runtime.metric_iqr);
		UpdateExpMetricStat(res.gd.metric_history, res.gd.is_record, res.gd.metric_mean, res.gd.metric_std, res.gd.metric_median, res.gd.metric_iqr);
		UpdateExpMetricStat(res.spacing.metric_history, res.spacing.is_record, res.spacing.metric_mean, res.spacing.metric_std, res.spacing.metric_median, res.spacing.metric_iqr);
		UpdateExpMetricStat(res.igdplus.metric_history, res.igdplus.is_record, res.igdplus.metric_mean, res.igdplus.metric_std, res.igdplus.metric_median, res.igdplus.metric_iqr);
		UpdateExpMetricStat(res.gdplus.metric_history, res.gdplus.is_record, res.gdplus.metric_mean, res.gdplus.metric_std, res.gdplus.metric_median, res.gdplus.metric_iqr);
	
	}

	void EMOCManager::UpdateExpMetricStat(std::vector<double>& indicator_history, std::vector<bool>& is_indicator_record,
		double& mean, double& std, double& median, double& iqr)
	{
		mean = 0.0, std = 0.0;
		int count = 0;
		for (int c = 0; c < indicator_history.size(); c++)
		{
			if (is_indicator_record[c])
			{
				mean += indicator_history[c];
				count++;
			}
		}
		mean = mean / count;
		for (int c = 0; c < indicator_history.size(); c++)
		{
			if (is_indicator_record[c])
				std += (indicator_history[c] - mean) * (indicator_history[c] - mean);
		}
		std = std / count; std = std::sqrt(std);


		std::vector<double> temp_ind;
		for (int c = 0; c < indicator_history.size(); c++)
			if (is_indicator_record[c])
				temp_ind.push_back(indicator_history[c]);
		std::sort(temp_ind.begin(), temp_ind.end());
		
		// early return when the size of result is not enough
		if (temp_ind.size() == 1)
		{
			median = temp_ind[0];
			iqr = 0.0;
			return;
		}

		// calculate median
		if (temp_ind.size() % 2)
			median = temp_ind[temp_ind.size() / 2];
		else
			median = (temp_ind[temp_ind.size() / 2] + temp_ind[temp_ind.size() / 2 - 1])/ 2.0;

		// calculate iqr
		double Q1 = 0, Q3 = 0;
		int Q1_remain = temp_ind.size() * 25 % 100;
		int Q1_index = temp_ind.size() * 25 / 100;		// be careful with the start index is 0 which is different from the calculation in WiKi
		if (Q1_remain == 0)
			Q1 = (temp_ind[Q1_index] + temp_ind[Q1_index - 1]) / 2.0;
		else
			Q1 = temp_ind[Q1_index];

		int Q3_remain = temp_ind.size() * 75 % 100;
		int Q3_index = temp_ind.size() * 75 / 100;		// be careful with the start index is 0 which is different from the calculation in WiKi
		if (Q3_remain == 0)
			Q3 = (temp_ind[Q3_index] + temp_ind[Q3_index - 1]) / 2.0;
		else
			Q3 = temp_ind[Q3_index];
			
		iqr = Q3 - Q1;
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
