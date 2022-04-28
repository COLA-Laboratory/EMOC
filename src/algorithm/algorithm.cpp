#include "algorithm/algorithm.h"

#include <cstring>
#include <utility>
#include <thread>
#include <chrono>
#include <iostream>

#include "core/emoc_app.h"
#include "core/global.h"
#include "core/file.h"
#include "core/emoc_manager.h"
#include "ui/plot_manager.h"
#include "ui/uipanel_manager.h"

namespace emoc {

	Algorithm::Algorithm(int thread_id) :
		g_GlobalSettings(nullptr),
		thread_id_(thread_id),
		real_popnum_(0),
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
		for (int i = 0; i < real_popnum_; ++i)
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
		if (g_GlobalSettings->iteration_num_ >= 1) runtime_ += (double)(end_ - start_) / CLOCKS_PER_SEC;

		// get current emoc mode
		bool is_gui = EMOCManager::Instance()->GetIsGUI();
		bool is_experiment = EMOCManager::Instance()->GetIsExperiment();
		bool is_plot = EMOCManager::Instance()->GetIsPlot();

		// update uipanel's data when necessary
		if (is_gui && !is_experiment)
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

	void Algorithm::EvaluateInd(Individual* ind)
	{
		g_GlobalSettings->problem_->CalObj(ind);
		g_GlobalSettings->problem_->CalCon(ind);
		g_GlobalSettings->current_evaluation_++;
	}

	int Algorithm::MergePopulation(Individual** pop_src1, int pop_num1, Individual** pop_src2, int pop_num2, Individual** pop_dest)
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

	void Algorithm::CopyIndividual(Individual* ind_src, Individual* ind_dest)
	{
		// copy individual properties
		ind_dest->fitness_ = ind_src->fitness_;
		ind_dest->rank_ = ind_src->rank_;

		// copy individual decision, objective and constraint datas
		for (int i = 0; i < ind_src->dec_.size(); i++)
			ind_dest->dec_[i] = ind_src->dec_[i];
		for (int i = 0; i < ind_src->obj_.size(); i++)
			ind_dest->obj_[i] = ind_src->obj_[i];
		for (int i = 0; i < ind_src->con_.size(); i++)
			ind_dest->con_[i] = ind_src->con_[i];
	}

	void Algorithm::SwapIndividual(Individual* ind1, Individual* ind2)
	{
		std::swap(ind1->fitness_, ind2->fitness_);
		std::swap(ind1->rank_, ind2->rank_);

		for (int i = 0; i < g_GlobalSettings->dec_num_; i++)
		{
			double t = ind1->dec_[i];
			ind1->dec_[i] = ind2->dec_[i];
			ind2->dec_[i] = t;
		}

		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
		{
			double t = ind1->obj_[i];
			ind1->obj_[i] = ind2->obj_[i];
			ind2->obj_[i] = t;
		}

		for (int i = 0; i < ind1->con_.size(); i++)
		{
			double t = ind1->con_[i];
			ind1->con_[i] = ind2->con_[i];
			ind2->con_[i] = t;
		}
	}

	void Algorithm::TrackPopulation(int generation)
	{
		RecordPop(g_GlobalSettings->run_id_, generation, g_GlobalSettings, real_popnum_);
	}

	void Algorithm::PlotPopulation(Individual** pop, int gen)
	{
		int current_run = EMOCManager::Instance()->GetSingleThreadResultSize();

		// construct data file name and open script file for gnuplot
		FILE* script_file = nullptr;
		char data_file_name[256];
		char script_file_name[256];
		sprintf(data_file_name, "./output/test_module/run%d/pop_%d.txt", current_run, gen);
		sprintf(script_file_name, "./plotfile/%d.gnu", gen);

		CreateDirectory(script_file_name);
		script_file = fopen(script_file_name, "w");
		if (!script_file)
		{
			std::cout << "Can not create the plot script file: " << script_file_name << "\n";
			exit(1);
		}


		// construct plot command
		char plot_cmd[1024];
		if (g_GlobalSettings->problem_->encoding_ == Problem::BINARY)
		{
			PlotManager::Instance()->BinaryHeatMap(plot_cmd,gen,real_popnum_,g_GlobalSettings->dec_num_,data_file_name);
		}
		else if (g_GlobalSettings->problem_->encoding_ == Problem::PERMUTATION)
		{
			if(g_GlobalSettings->problem_name_ == "TSP")
				PlotManager::Instance()->TSPVisulization(plot_cmd,gen,data_file_name);
			else
			{
				// For other permutation problems, there is no visuliazation now.
				fclose(script_file);
				return;
			}
		}
		else if (g_GlobalSettings->problem_->encoding_ == Problem::REAL)
		{
			if (g_GlobalSettings->obj_num_ == 1)
			{
				// do nothing
				fclose(script_file);
				return;
			}
			else if (g_GlobalSettings->obj_num_ == 2)
			{
				PlotManager::Instance()->Scatter2D(plot_cmd, gen, data_file_name);
			}
			else if (g_GlobalSettings->obj_num_ == 3)
			{
				PlotManager::Instance()->Scatter3D(plot_cmd, gen, data_file_name);
			}
			else if (g_GlobalSettings->obj_num_ <= 7)
			{
				PlotManager::Instance()->ParallelAxisPlot(plot_cmd, gen, g_GlobalSettings->obj_num_,data_file_name);
			}
			else
			{
				// Gnuplot didn't support
				std::cerr << "Error!!! in display_pop(...)" << std::endl;
				fclose(script_file);
				return;
			}
		}
		// write plot cmd to script file
		fprintf(script_file, "%s", plot_cmd);
		fflush(script_file);
		fclose(script_file);

		// construct the real command
		char real_cmd[128];
		sprintf(real_cmd, "load '%s'\n", script_file_name);

		// send the real command to gnuplot
		PlotManager* plot_manager = PlotManager::Instance();
		if (g_GlobalSettings->iteration_num_ == 0)
			PlotManager::Instance()->RefreshPipe();
		plot_manager->Send(real_cmd);

		// sleep some time for moderating the delay of events e.g. button click.
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