#pragma once
#include <mutex>
#include <condition_variable>
#include <unordered_set>
#include <array>

#include "ui/plot_manager.h"
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

}