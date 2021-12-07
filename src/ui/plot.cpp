#include "ui/plot.h"

#include <iostream>

#include "emoc_app.h"


namespace emoc {

	PlotManager::Garbo PlotManager::garbo_;
	PlotManager* PlotManager::s_Instance = nullptr;
	std::mutex PlotManager::singleton_mutex_;

	PlotManager* PlotManager::Instance()
	{
		if (s_Instance == nullptr)
		{
			std::lock_guard<std::mutex> locker(singleton_mutex_);
			if (s_Instance == nullptr)
			{
				s_Instance = new PlotManager();
			}
		}

		return s_Instance;
	}

	void PlotManager::RefreshPipe()
	{
		if (is_window_close_)
			OpenPlotPipe();
		fprintf(gp_, "\n\n\n");
		fflush(gp_);
	}

	void PlotManager::Send(const std::string& cmd)
	{
		if (is_window_close_)
			OpenPlotPipe();
		fprintf(gp_, cmd.c_str());
		fflush(gp_);
	}

	void PlotManager::OpenPlotPipe()
	{
		// only open pipe when window is closed
		if (is_window_close_)
		{
			gp_ = _popen("gnuplot", "w");
			if (!gp_)
			{
				std::cout << "<Error> Could not open a pipe to gnuplot, check the definition of GNUPLOT_COMMAND" << std::endl;
				exit(-1);
			}
			is_window_close_ = false;
		}
	}

	void PlotManager::ClosePlotPipe()
	{
		// only close pipe when window is opened
		if (!is_window_close_)
		{
			if (gp_ != nullptr)
			{
				_pclose(gp_);
				gp_ = nullptr;
			}
			is_window_close_ = true;
		}
	}

	PlotManager::PlotManager():
		is_window_close_(true),
		gp_(nullptr)
	{
		OpenPlotPipe();
	}

	PlotManager::~PlotManager()
	{
		ClosePlotPipe();
	}

}


