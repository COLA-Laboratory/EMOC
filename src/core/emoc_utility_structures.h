#include <string>
#include <vector>
#include <unordered_map>

#include "core/macro.h"

namespace emoc
{
	// EMOCParameters structure cannot forward declaration, so we put the definition here
	struct EMOCParameters
	{
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

		double metric_mean = 0.0, metric_std = 0.0, metric_median = 0.0, metric_iqr = 0.0;
		int metric_mean_ranksum[3] = { -2, -2, -2 }, metric_mean_signrank[3] = { -2, -2, -2 };			// 0:mean best compared result 1:median best compared result 2:last column compared result
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
		}


		//// runtime results
		//std::vector<double> runtime_history;
		//std::vector<bool> is_runtime_record;
		//double runtime_mean = 0.0, runtime_std = 0.0, runtime_median = 0.0, runtime_iqr = 0.0;
		//int runtime_mean_ranksum[3]= { -2, -2, -2 }, runtime_mean_signrank[3] = { -2, -2, -2 };			// 0:mean best compared result 1:median best compared result 2:last column compared result
		//int runtime_median_ranksum[3] = { -2, -2 , -2 }, runtime_median_signrank[3] = { -2, -2, -2 };

		//// igd results
		//std::vector<double> igd_history;
		//std::vector<bool> is_igd_record;
		//double igd_mean = 0.0, igd_std = 0.0, igd_median = 0.0, igd_iqr = 0.0;
		//int igd_mean_ranksum[3] = { -2, -2, -2 }, igd_mean_signrank[3] = { -2, -2, -2 };
		//int igd_median_ranksum[3] = { -2, -2, -2 }, igd_median_signrank[3] = { -2, -2, -2 };

		//// hv results
		//std::vector<double> hv_history;
		//std::vector<bool> is_hv_record;
		//double hv_mean = 0.0, hv_std = 0.0, hv_median = 0.0, hv_iqr = 0.0;
		//int hv_mean_ranksum[3] = { -2, -2, -2 },	hv_mean_signrank[3] = { -2, -2, -2 };
		//int hv_median_ranksum[3] = { -2, -2, -2 }, hv_median_signrank[3] = { -2, -2, -2 };
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
		double runtime;
		double pop_num;
		int max_iteration;

		// history metric for each generation
		std::unordered_map<int, double> igd_history;
		std::unordered_map<int, double> hv_history;
		std::unordered_map<int, double> spacing_history;
		std::unordered_map<int, double> spread_history;

		EMOCSingleThreadResult() {}
	};
}