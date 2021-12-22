#pragma once
#include <mutex>
#include <condition_variable>
#include <unordered_set>
#include <array>

#include "ui/plot.h"
#include "ui/uipanel_manager.h"
#include "core/emoc_manager.h"

namespace emoc {

	struct EMOCLock
	{
		static std::mutex test_finish_mutex;
		static std::mutex experiment_finish_mutex;
		static std::mutex test_pause_mutex;
		static std::mutex experiment_pause_mutex;
		static std::mutex multithread_data_mutex;
		static std::condition_variable test_pause_cond;
		static std::condition_variable experiment_pause_cond;
	
		static std::array<std::mutex, 32> mutex_pool;
	};

	class EMOCApplication
	{
	public:
		EMOCApplication();
		~EMOCApplication();

		void Init(bool is_gui, const EMOCParameters &para);
		void Run();

	private:
		PlotManager* plot_manager_;
		UIPanelManager* ui_manager_;
		EMOCManager* emoc_manager_;
	};

	const std::unordered_set<std::string> IMPLEMENT_ALGORITHM ={
		"MOEAD", "MOEADDE", "MOEADDRA", "MOEADFRRMAB", "MOEADGRA", "MOEADIRA", "ENSMOEAD",
		"NSGA2", "SPEA2",
		"IBEA", "SMSEMOA", "HYPE"
	};
	const std::unordered_set<std::string> IMPLEMENT_PROBLEM = {
		"ZDT1", "ZDT2", "ZDT3", "ZDT4", "ZDT6",
		"DTLZ1", "DTLZ2","DTLZ3","DTLZ4","DTLZ5","DTLZ6","DTLZ7",
		"MDTLZ1", "MDTLZ2","MDTLZ3","MDTLZ4",
		"UF1", "UF2","UF3","UF4", "UF5","UF6","UF7", "UF8","UF9","UF10",
		"WFG1", "WFG2","WFG3","WFG4", "WFG5","WFG6","WFG7", "WFG8","WFG9"
	};
}