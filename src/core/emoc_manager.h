#pragma once
#include <vector>
#include <mutex>

#include "core/file.h"
#include "core/global.h"

namespace emoc {

	struct EMOCExperimentTask
	{
		EMOCParameters para;	// We let each task own a copy of parameter to prevent some potential bugs.
		int run_index;			// refer to which run in current parameter needed runs
		int parameter_index;	// refer to which parameter in received parameter vectors
	};

	// for (multi-thread or multi-run) epxeriment result
	struct EMOCMultiThreadResult
	{
		std::vector<double> runtime_history;
		std::vector<bool> is_runtime_record;
		std::vector<double> igd_history;
		std::vector<bool> is_igd_record;
		std::vector<double> hv_history;
		std::vector<bool> is_hv_record;

		EMOCMultiThreadResult()
		{

		}

		EMOCMultiThreadResult(int run_num)
		{
			runtime_history.resize(run_num, 0.0);
			is_runtime_record.resize(run_num, false);
			igd_history.resize(run_num, 0.0);
			is_igd_record.resize(run_num, false);
			hv_history.resize(run_num, 0.0);
			is_hv_record.resize(run_num, false);
		}
	};

	// for (single thread or single run) test result
	struct EMOCSingleThreadResult
	{
		std::string description;
		EMOCParameters para;		// for parameter information accesses easily
		double last_igd;
		double last_hv;
		double last_spread;
		double last_spacing;
		double runtime;
		double pop_num;
		int max_iteration;

		std::vector<double> igd_history;
		std::vector<double> hv_history;
		std::vector<double> spacing_history;
		std::vector<double> spread_history;
	};

	class EMOCManager
	{
	public:
		static EMOCManager* Instance();
		
		void Run();
		void ExperimentModuleRun(std::vector<EMOCParameters> experiment_tasks, int thread_num);

		// Simple Getters
		inline bool GetIsPlot() { return is_plot_; }
		inline bool GetIsGUI() { return is_gui_; }
		inline bool GetIsExperiment() { return is_experiment_; }
		inline bool GetTestPause() { return is_test_pause_; }
		inline bool GetExperimentPause() { return is_experiment_pause_; }
		inline bool GetTestFinish() { return is_test_finish_; }
		inline bool GetExperimentFinish() { return is_experiment_finish_; }
		inline bool GetMultiThreadDataState() { return is_multithread_result_ready_; }
		inline const EMOCParameters& GetParameters() { return para_; }

		// Simple Setters
		inline void SetIsPlot(bool is_plot) { is_plot_ = is_plot; }
		inline void SetIsGUI(bool is_gui) { is_gui_ = is_gui; }
		inline void SetIsExperiment(bool is_experiment) { is_experiment_ = is_experiment; }
		inline void SetTestPause(bool is_pause) { is_test_pause_ = is_pause; }
		inline void SetExperimentPause(bool is_pause) { is_experiment_pause_ = is_pause; }
		inline void SetTestFinish(bool is_finish) { is_test_finish_ = is_finish; }
		inline void SetExperimentFinish(bool is_finish) { is_experiment_finish_ = is_finish; }
		inline void SetMultiThreadDataState(bool state) { is_multithread_result_ready_ = state; }
		inline void SetTaskParameters(const EMOCParameters& para) { para_ = para; is_para_set_ = true; }

		// Getters for some other private variables
		inline Global* GetGlobalSetting(int index) { return g_GlobalSettingsArray[index]; }
		inline int GetSingleThreadResultSize() { return (int)single_thread_result_historty_.size(); }
		inline int GetMultiThreadResultSize() { return (int)multi_thread_result_history_.size(); }
		inline  EMOCSingleThreadResult& GetSingleThreadResult(int index) { return single_thread_result_historty_[index]; }
		inline  EMOCMultiThreadResult& GetMultiThreadResult(int index) { return multi_thread_result_history_[index]; }

	private:
		EMOCManager();
		~EMOCManager();
		EMOCManager(const EMOCManager &);
		EMOCManager& operator=(const EMOCManager &);

		void EMOCSingleThreadRun();
		void EMOCMultiThreadRun();
		void ExperimentWorker(std::vector<EMOCExperimentTask> tasks, int thread_id);

	private:
		// for release EMOCManager instance
		class Garbo 
		{
		public:
			Garbo() {};
			~Garbo()
			{
				if (s_Instance != nullptr)
				{
					delete s_Instance;
					s_Instance = nullptr;
				}
			}
		};
		static Garbo garbo_;
		static EMOCManager* s_Instance;
		static std::mutex singleton_mutex_;


		// EMOC state variables
		EMOCParameters para_;
		bool is_para_set_;
		bool is_plot_;			// whether to activate plot function
		bool is_gui_;			// whether use gui
		bool is_experiment_;	// whether in experiment module

		// state variables for test module and experiment module in gui mode
		bool is_test_pause_;
		bool is_test_finish_;
		bool is_experiment_pause_;
		bool is_experiment_finish_;

		// runs results
		std::vector<EMOCSingleThreadResult> single_thread_result_historty_;
		std::vector<EMOCMultiThreadResult> multi_thread_result_history_;
		bool is_multithread_result_ready_;

		// reserved Global arrays
		std::vector<Global*> g_GlobalSettingsArray;
	};

}