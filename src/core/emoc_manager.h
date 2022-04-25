#pragma once
#include <vector>
#include <mutex>
#include <unordered_map>
#include <mutex>

#include "core/macro.h"
#include "core/global.h"
#include "core/emoc_utility_structures.h"

namespace emoc {

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
		inline void SetPythonResult(EMOCGeneralResult& py_res) { py_res_ = py_res; }

		// Getters for some other private variables
		inline Global* GetGlobalSetting(int index) { return g_GlobalSettingsArray[index]; }
		inline void AddSingleThreadResult(const EMOCSingleThreadResult& result) { single_thread_result_historty_.push_back(result); }
		inline int GetSingleThreadResultSize() { return (int)single_thread_result_historty_.size(); }
		inline int GetMultiThreadResultSize() { return (int)multi_thread_result_history_.size(); }
		inline  EMOCSingleThreadResult& GetSingleThreadResult(int index) { return single_thread_result_historty_[index]; }
		inline  EMOCMultiThreadResult& GetMultiThreadResult(int index) { return multi_thread_result_history_[index]; }
		inline EMOCGeneralResult GetPythonResult() { return py_res_; }

	private:
		EMOCManager();
		~EMOCManager();
		EMOCManager(const EMOCManager&);
		EMOCManager& operator=(const EMOCManager&);

		// emoc run functions for non-experiment module
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


		// EMOC setting variables
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

		// optimization result information for python side 
		EMOCGeneralResult py_res_;
	};


}