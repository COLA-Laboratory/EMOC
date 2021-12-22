#include "algorithms/algorithm.h"

#include <cstring>
#include <thread>
#include <chrono>
#include <iostream>

#include "emoc_app.h"
#include "core/global.h"
#include "core/file.h"
#include "core/emoc_manager.h"
#include "ui/plot.h"
#include "ui/uipanel_manager.h"

namespace emoc {

	Algorithm::Algorithm(int thread_id) :
		g_GlobalSettings(nullptr),
		thread_id_(thread_id),
		real_popnum_(0),
		record_file_time_(0.0f),
		runtime_(0.0)
	{
		g_GlobalSettings = EMOCManager::Instance()->GetGlobalSetting(thread_id);
		real_popnum_ = g_GlobalSettings->population_num_;
	}

	Algorithm::~Algorithm()
	{

	}

	void Algorithm::PrintPop()
	{
		for (int i = 0; i < 2; ++i)
		{
			std::cout << "population[" << i << "]: \n";
			std::cout << "--dec--:";
			for (int j = 0; j < g_GlobalSettings->dec_num_; ++j)
			{
				std::cout << g_GlobalSettings->parent_population_[i]->dec_[j] << " ";
			}
			std::cout << "\n--obj--:";
			for (int j = 0; j < g_GlobalSettings->obj_num_; ++j)
			{
				std::cout << g_GlobalSettings->parent_population_[i]->obj_[j] << " ";
			}
			std::cout << std::endl;
		}
	}

	bool Algorithm::IsTermination()
	{
		// record runtime
		end_ = clock();
		if(g_GlobalSettings->iteration_num_ >= 1) runtime_ += (double)(end_ - start_) / CLOCKS_PER_SEC;
		
		// get current emoc mode
		bool is_gui = EMOCManager::Instance()->GetIsGUI();
		bool is_experiment = EMOCManager::Instance()->GetIsExperiment();
		bool is_plot = EMOCManager::Instance()->GetIsPlot();

		// update uipanel's data when necessary
		if(is_gui && !is_experiment)
			UIPanelManager::Instance()->SetCurrentEvaluation(g_GlobalSettings->current_evaluation_);

		// record the population every interval generations and the first and last genration 
		bool is_terminate = g_GlobalSettings->current_evaluation_ >= g_GlobalSettings->max_evaluation_;
		if (g_GlobalSettings->iteration_num_ % g_GlobalSettings->output_interval_ == 0 || is_terminate)
			TrackPopulation(g_GlobalSettings->iteration_num_);
		if (is_plot)
			PlotPopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->iteration_num_);

		// check stop and pause
		if (is_gui && CheckStopAndPause()) return true;

		// increase iteration number if is not terminated
		if (!is_terminate)	g_GlobalSettings->iteration_num_++;

		start_ = clock();
		return is_terminate;
	}

	void Algorithm::EvaluatePop(Individual** pop, int pop_num)
	{
		for (int i = 0; i < pop_num; ++i)
		{
			EvaluateInd(pop[i]);
		}
	}

	void Algorithm::EvaluateInd(Individual *ind)
	{
		g_GlobalSettings->problem_->CalObj(ind);
		g_GlobalSettings->current_evaluation_++;
	}

	int Algorithm::MergePopulation(Individual **pop_src1, int pop_num1, Individual **pop_src2, int pop_num2, Individual **pop_dest)
	{
		int i = 0, j = 0;

		// copy fist population
		for (i = 0; i < pop_num1; i++)
		{
			CopyIndividual(pop_src1[i], pop_dest[i]);
		}

		// copy second population
		for (j = 0; j < pop_num2; j++, i++)
		{
			CopyIndividual(pop_src2[j], pop_dest[i]);
		}
		return i;
	}

	void Algorithm::CopyIndividual(Individual *ind_src, Individual *ind_dest)
	{
		// copy individual properties
		ind_dest->fitness_ = ind_src->fitness_;
		ind_dest->rank_ = ind_src->rank_;

		// copy individual decision and objective datas
		memcpy(ind_dest->dec_, ind_src->dec_, sizeof(double) * g_GlobalSettings->dec_num_);
		memcpy(ind_dest->obj_, ind_src->obj_, sizeof(double) * g_GlobalSettings->obj_num_);
	}

	void Algorithm::TrackPopulation(int generation)
	{
		int is_terminal = g_GlobalSettings->current_evaluation_ >= g_GlobalSettings->max_evaluation_;
		RecordPop(g_GlobalSettings->run_id_, generation, g_GlobalSettings,real_popnum_);
	}

	double testTime = 0.0;
	void Algorithm::PlotPopulation(Individual** pop, int gen)
	{
		int current_run = EMOCManager::Instance()->GetSingleThreadResultSize();
		clock_t start_ = clock();
		clock_t end_ = clock();

		// open data file and script file for gnuplot
		FILE  *script_file = nullptr;
		char data_file_name[256];
		char script_file_name[256];
		sprintf(data_file_name, "./output/test_module/run%d/pop_%d.txt", current_run, gen);
		sprintf(script_file_name, "./plotfile/%d.gnu", gen);
		script_file = fopen(script_file_name, "w");

		int obj_num = g_GlobalSettings->obj_num_;

		// construct plot command
		char plot_cmd[1024];
		if (obj_num == 2)
		{
			sprintf(plot_cmd,
				"set grid\n"
				"set autoscale\n"
				"set title 'Generation #%d'\n"
				"set xlabel 'f1'\n"
				"set ylabel 'f2'\n"
				"unset key\n"
				"plot '%s' w p pt 6\n"
				//"plot 'PF_ZDT3.txt' w l lt -1 lw 2, 'plot.txt' w p pt 6 ps 1 lc 3, 'golden_point_zdt3_1_1.txt' w p pt 3 ps 2 lc 1\n"
				, gen, data_file_name);
		}
		else if (obj_num == 3)
		{
			sprintf(plot_cmd, 
				"set grid\n"
				"set autoscale\n"
				"set title 'Generation #%d'\n"
				"set xlabel 'f1'\n"
				"set ylabel 'f2'\n"
				"set zlabel 'f3'\n"
				"set ticslevel 0.0\n"
				"set view 45,45\n"
				"unset key\n"
				"splot  '%s' w p pt 6\n"
				,gen, data_file_name);
		}
		else
		{
			std::cerr << "Error!!! in display_pop(...)" << std::endl;
			fclose(script_file);
			return;
			//exit(-1);
		}

		// write plot cmd script file
		fprintf(script_file, "%s", plot_cmd);
		fflush(script_file);
		fclose(script_file);

		char real_cmd[128];

		sprintf(real_cmd, "load '%s'\n", script_file_name);

		PlotManager* plot_manager = PlotManager::Instance();
		if (g_GlobalSettings->iteration_num_ == 0)
			PlotManager::Instance()->RefreshPipe();

		//start_ = clock();
		plot_manager->Send(real_cmd);
		//end_ = clock();
		//std::cout << (double)end_ << " " << (double)start_ << "\n";
		//testTime += (double)(end_ - start_) / CLOCKS_PER_SEC;
		//std::cout << (double)(end_ - start_) / CLOCKS_PER_SEC << " total draw time:" << testTime << "\n";

		double waiting_time = 15.0 * real_popnum_ / 100.0;
		waiting_time = waiting_time > 27.5 ? waiting_time : 27.5;

		std::this_thread::sleep_for(std::chrono::milliseconds((int)waiting_time));
	}

	bool Algorithm::CheckStopAndPause()
	{
		// check stop and pause for test module
		if (!EMOCManager::Instance()->GetIsExperiment())
		{
			{
				std::lock_guard<std::mutex> locker(EMOCLock::test_finish_mutex);
				if (EMOCManager::Instance()->GetTestFinish()) return true;
			}

			{
				std::unique_lock<std::mutex> locker(EMOCLock::test_pause_mutex);
				if (EMOCManager::Instance()->GetTestPause())
					EMOCLock::test_pause_cond.wait(locker, [&]() {return !EMOCManager::Instance()->GetTestPause(); });
			}
		}
		// check stop and pause for experiment module
		else
		{
			{
				std::lock_guard<std::mutex> locker(EMOCLock::experiment_finish_mutex);
				if (EMOCManager::Instance()->GetExperimentFinish()) return true;
			}

			{
				std::unique_lock<std::mutex> locker(EMOCLock::experiment_pause_mutex);
				if (EMOCManager::Instance()->GetExperimentPause())
					EMOCLock::experiment_pause_cond.wait(locker, [&]() {return !EMOCManager::Instance()->GetExperimentPause(); });
			}
		}


		return false;
	}

}