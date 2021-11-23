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


	//do
	//{
	//	std::unique_lock<std::mutex> lock(m_Mutex);
	//	while (m_data.empty())
	//	{
	//		m_cv.wait(lock, [&]() { return !m_data.empty(); }); // predicate an while loop - protection from spurious wakeups
	//	}
	//	while (!m_data.empty()) // consume all elements from queue
	//	{
	//		std::cout << "Consumer Thread, queue element: " << m_data.front() << " size: " << m_data.size() << std::endl;
	//		m_data.pop();
	//	}
	//} while (!m_bFinished);


	// 暂时不考虑画图的多线程了，直接嵌入在算法线程里面
	void PlotManager::Run()
	{
		double testTime = 0.0f;
		int count = 0, number = 0;
		while (!is_finish_)
		{
			{
				std::unique_lock<std::mutex> locker(EMOCLock::finish_mutex);
				// predicate for protection from spurious wakeups
				
				EMOCLock::pause_cond.wait(locker, [&]() { return is_finish_ || !plot_cmds_.empty();});
				
				count++;
				if (is_finish_)
					break;
				number += plot_cmds_.size();
				// consume all elements from queue
				while (!plot_cmds_.empty())
				{
					plot_execute_cmds_.push(plot_cmds_.front());
					plot_cmds_.pop();
				}
			}
			std::cout << count << " cmd number" << number << "\n";
			clock_t start_ = clock();
			clock_t end_ = clock();


			start_ = clock();
			// execute the cmd without mutex
			while (!plot_execute_cmds_.empty())
			{
				//std::cout << "i'm plotting!\n";
				std::string cmd = plot_execute_cmds_.front();
				plot_execute_cmds_.pop();

				fprintf(gp_, cmd.c_str());
				fflush(gp_);
			}

			end_ = clock();
			//std::cout << (double)end_ << " " << (double)start_ << "\n";
			testTime += (double)(end_ - start_) / CLOCKS_PER_SEC;
			std::cout << (double)(end_ - start_) / CLOCKS_PER_SEC << " total draw time:" << testTime << "\n";
		}
	}

	void PlotManager::Stop()
	{
		is_finish_ = true;
		ClosePlotPipe();
	}

	void PlotManager::RefreshPipe()
	{
		if (is_window_close_)
			OpenPlotPipe();
		fprintf(gp_, "\n\n\n");
		fflush(gp_);
	}

	void PlotManager::Plot(const std::string& cmd)
	{
		if (is_window_close_)
			OpenPlotPipe();
		fprintf(gp_, cmd.c_str());
		fflush(gp_);
	}

	void PlotManager::OpenPlotPipe()
	{
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
		if (!is_window_close_)
		{
			if (gp_ != nullptr)
			{
				_pclose(gp_);
				gp_ = nullptr;
			}
			is_window_close_ = true;
		}

		//cond.notify_one();
	}

	PlotManager::PlotManager():
		is_finish_(false),
		is_window_close_(true),
		gp_(nullptr),
		plot_cmds_(),
		plot_execute_cmds_()
	{
		OpenPlotPipe();
	}

	PlotManager::~PlotManager()
	{
		ClosePlotPipe();
	}

}


