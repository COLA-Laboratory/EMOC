#pragma once
#include <mutex>
#include <condition_variable>

#include "ui/plot.h"
#include "ui/uipanel_manager.h"
#include "core/emoc_manager.h"

namespace emoc {

	class EMOCApplication
	{
	public:
		EMOCApplication();
		~EMOCApplication();

		void Init();
		void Run();

	private:
		PlotManager* plot_manager_;
		UIPanelManager* ui_manager_;
		EMOCManager* emoc_manager_;
	};

	extern std::mutex finish_mutex;
	extern std::mutex pause_mutex;
	extern std::condition_variable cond;
}