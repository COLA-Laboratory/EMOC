#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "core/macro.h"

namespace emoc
{
	// EMOCParameters structure cannot forward declaration, so we put the definition here
	struct EMOCParameters
	{
	public:
		std::string algorithm_name;
		std::string problem_name;
		bool is_plot;
		int population_num;
		int decision_num;
		int objective_num;
		int max_evaluation;
		int output_interval;
		int runs_num;
		int is_open_multithread;
		int thread_num;

		// custom problem and initial population variables for python dlls
		Problem* custom_problem = nullptr;
		std::vector<std::vector<double>> custom_initial_pop;

	public:
		EMOCParameters() :
			algorithm_name("NSGA2"),
			problem_name("ZDT1"),
			is_plot(false),
			population_num(100),
			decision_num(30),
			objective_num(2),
			max_evaluation(25000),
			output_interval(EMOC_INF),
			runs_num(1),
			is_open_multithread(false),
			thread_num(0)
		{
		}

		~EMOCParameters() {}

		// custom problem and initial population function for python dlls
		inline void SetProblem(Problem* problem) { custom_problem = problem; }
		inline void SetInitialPop(std::vector<std::vector<double>> initial_pop) { custom_initial_pop = initial_pop; }
	};


	struct EMOCExperimentTask
	{
		EMOCParameters para;	// We let each task own a copy of parameter to prevent some potential bugs.
		int run_index;			// refer to which run in current parameter needed runs
		int parameter_index;	// refer to which parameter in received parameter vectors
	};

	struct  MetricHistory
	{
		// history metric for each run
		std::vector<double> metric_history;
		std::vector<bool> is_record;

		int median_index = 0, best_index = 0;
		double metric_mean = 0.0, metric_std = 0.0, metric_median = 0.0, metric_iqr = 0.0, metric_best = 0.0;

		// [0]:mean best compared result [1]:median best compared result [2]:last column compared result
		int metric_mean_ranksum[3] = { -2, -2, -2 }, metric_mean_signrank[3] = { -2, -2, -2 };
		int metric_median_ranksum[3] = { -2, -2 , -2 }, metric_median_signrank[3] = { -2, -2, -2 };
	};

	// for (multi-thread or multi-run) epxeriment result
	struct EMOCMultiThreadResult
	{
		// metric results
		MetricHistory runtime;
		MetricHistory igd;
		MetricHistory hv;
		MetricHistory gd;
		MetricHistory spacing;
		MetricHistory igdplus;
		MetricHistory gdplus;

		// for single objective
		MetricHistory best_value;

		int valid_res_count = 0;

		EMOCMultiThreadResult()
		{

		}

		EMOCMultiThreadResult(int run_num)
		{
			runtime.metric_history.resize(run_num, 0.0);
			runtime.is_record.resize(run_num, false);
			igd.metric_history.resize(run_num, 0.0);
			igd.is_record.resize(run_num, false);
			hv.metric_history.resize(run_num, 0.0);
			hv.is_record.resize(run_num, false);
			gd.metric_history.resize(run_num, 0.0);
			gd.is_record.resize(run_num, false);
			spacing.metric_history.resize(run_num, 0.0);
			spacing.is_record.resize(run_num, false);
			igdplus.metric_history.resize(run_num, 0.0);
			igdplus.is_record.resize(run_num, false);
			gdplus.metric_history.resize(run_num, 0.0);
			gdplus.is_record.resize(run_num, false);
			best_value.metric_history.resize(run_num, 0.0);
			best_value.is_record.resize(run_num, false);
		}
	};

	// for (single thread or single run) test result
	struct EMOCSingleThreadResult
	{
		std::string description;
		EMOCParameters para;		// for parameter information accesses easily
		double last_igd;
		double last_hv;
		double last_gd;
		double last_spacing;
		double last_igdplus;
		double last_gdplus;
		double best_value;
		double runtime;
		double pop_num;
		int max_iteration;

		// history metric for each generation
		std::unordered_map<int, double> igd_history;
		std::unordered_map<int, double> hv_history;
		std::unordered_map<int, double> spacing_history;
		std::unordered_map<int, double> spread_history;
		std::unordered_map<int, double> igdplus_history;
		std::unordered_map<int, double> gdplus_history;

		EMOCSingleThreadResult() {}
	};

	// for python dlls
	struct EMOCGeneralResult
	{
		double igd = 0.0;
		double hv = 0.0;
		double gd = 0.0;
		double spacing = 0.0;
		double igdplus = 0.0;
		double gdplus = 0.0;
		double runtime = 0.0;
		double best_value = 0.0;

		int pop_num = 0;
		std::vector<std::vector<double>> pop_decs;
		std::vector<std::vector<double>> pop_objs;
	};

	// crossover parameter
	struct CrossoverParameter
	{
		double pro = 1.0;
		double index1 = 20.0; 
		double index2 = 20.0;	// additional parameter, use it when necessary
	};

	struct MutationParameter
	{
		double pro = 1.0;
		double index1 = 20.0;
		double index2 = 20.0;   // additional parameter, use it when necessary
	};
}