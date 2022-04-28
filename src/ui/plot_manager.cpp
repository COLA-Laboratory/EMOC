#include "ui/plot_manager.h"

#include <cstdio>
#include <iostream>

#include "core/emoc_app.h"

#if defined(_WIN32) // windows
#define POPEN _popen
#define PCLOSE _pclose
#elif defined(__linux) || defined(linux) // linux
#define POPEN popen
#define PCLOSE pclose
#elif defined(__APPLE__)// macos
#define POPEN popen
#define PCLOSE pclose
#endif

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
			gp_ = POPEN("gnuplot", "w");
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
				PCLOSE(gp_);
				gp_ = nullptr;
			}
			is_window_close_ = true;
		}
	}

	void PlotManager::Scatter2D(char* plot_cmd, int gen, char* data_file_name)
	{
		sprintf(plot_cmd,
			"reset session\n"
			"set border\n"
			"set grid\n"
			"set autoscale\n"
			"set title 'Generation #%d'\n"
			"set xlabel 'f1'\n"
			"set ylabel 'f2'\n"
			"unset key\n"
			"plot '%s' w p pt 6 lc rgb \"dark-blue\"\n"
			//"plot 'PF_ZDT3.txt' w l lt -1 lw 2, 'plot.txt' w p pt 6 ps 1 lc 3, 'golden_point_zdt3_1_1.txt' w p pt 3 ps 2 lc 1\n"
			, gen, data_file_name);
	}

	void PlotManager::Scatter3D(char* plot_cmd, int gen, char* data_file_name)
	{
		sprintf(plot_cmd,
			"reset session\n"
			"set border\n"
			"set grid\n"
			"set autoscale\n"
			"set title 'Generation #%d'\n"
			"set xlabel 'f1'\n"
			"set ylabel 'f2'\n"
			"set zlabel 'f3'\n"
			"set ticslevel 0.0\n"
			"set view 45,45\n"
			"unset key\n"
			"splot  '%s' w p pt 6 lc rgb \"dark-blue\"\n"
			, gen, data_file_name);
	}

	void PlotManager::ParallelAxisPlot(char* plot_cmd, int gen, int obj_num, char* data_file_name)
	{
		char columns[128]="";
		int index = 0;
		for (int i = 0; i < obj_num - 1; i++)
			sprintf(columns, "%s%d:", columns, i+1);
		sprintf(columns, "%s%d", columns, obj_num);

		char xtics[1024] = "";
		char ytics[1024] = "";
		sprintf(xtics, "set xtics(");
		for (int i = 0; i < obj_num - 1; i++)
			sprintf(xtics, "%s%d,", xtics, i + 1);
		sprintf(xtics, "%s%d)", xtics, obj_num);
		for (int i = 0; i < obj_num - 1; i++)
			sprintf(ytics, "%sset paxis %d tics\n", ytics, i + 1);
		sprintf(ytics, "%sset paxis %d tics", ytics, obj_num);
		sprintf(plot_cmd,
			"reset session\n"
			"unset key\n"
			"unset ytics\n"
			"unset border\n"
			"set title 'Generation #%d'\n"
			"%s\n"
			"%s\n"
			"plot  '%s' u %s w parallel lc 6\n"
			, gen,xtics,ytics, data_file_name, columns);
	}

	void PlotManager::BinaryHeatMap(char* plot_cmd, int gen, int pop_num, int dec_num, char* data_file_name)
	{
		sprintf(plot_cmd,
			"reset session\n"
			"set view map\n"
			"set palette defined(0 \"white\", 1 \"black\")\n"
			"set xrange[-0.5:%f]\n"
			"set yrange[-0.5:%f]\n"
			"set title 'Generation #%d'\n"
			"set xlabel 'Dimension No.'\n"
			"set ylabel 'Individual No.'\n"
			"unset grid\n"
			"unset key\n"
			"unset border\n"
			"unset colorbox\n"
			"splot  '%s' matrix with image\n"
			, dec_num+0.5, pop_num+0.5, gen, data_file_name);
	}

	void PlotManager::TSPVisulization(char* plot_cmd, int gen, char* data_file_name)
	{
		sprintf(plot_cmd,
			"reset session\n"
			"set title 'Generation #%d'\n"
			"set xlabel 'position x'\n"
			"set ylabel 'position y'\n"
			"unset key\n"
			"unset grid\n"
			"set xrange[0:1]\n"
			"set yrange[0:1]\n"
			"plot '%s' with lp lc 6 lw 2 pt 7 ps 1\n"
			, gen, data_file_name);
	}

	PlotManager::PlotManager() :
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


