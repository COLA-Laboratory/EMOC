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

	const std::unordered_set<std::string> IMPLEMENT_ALGORITHM = {
		"MOEAD", "MOEADDE", "MOEADDRA", "MOEADFRRMAB", "MOEADGRA", "MOEADIRA", "ENSMOEAD", "MOEADDYTS","MOEADCDE","MOEADSTM",
		"NSGA2", "SPEA2",
		"IBEA", "SMSEMOA", "HYPE",
	};

	const std::unordered_set<std::string> IMPLEMENT_PROBLEM = {
		"ZDT1", "ZDT2", "ZDT3", "ZDT4", "ZDT6",
		"DTLZ1", "DTLZ2","DTLZ3","DTLZ4","DTLZ5","DTLZ6","DTLZ7",
		"MDTLZ1", "MDTLZ2","MDTLZ3","MDTLZ4",
		"UF1", "UF2","UF3","UF4", "UF5","UF6","UF7", "UF8","UF9","UF10",
		"WFG1", "WFG2","WFG3","WFG4", "WFG5","WFG6","WFG7", "WFG8","WFG9",
		"LSMOP1", "LSMOP2","LSMOP3","LSMOP4", "LSMOP5","LSMOP6","LSMOP7", "LSMOP8","LSMOP9",
		"MOEADDE_F1", "MOEADDE_F2","MOEADDE_F3","MOEADDE_F4", "MOEADDE_F5","MOEADDE_F6","MOEADDE_F7", "MOEADDE_F8","MOEADDE_F9",
		"BT1", "BT2","BT3","BT4", "BT5","BT6","BT7", "BT8","BT9",
		"IMMOEA_F1", "IMMOEA_F2","IMMOEA_F3","IMMOEA_F4", "IMMOEA_F5","IMMOEA_F6","IMMOEA_F7", "IMMOEA_F8","IMMOEA_F9","IMMOEA_F10",
	};
}