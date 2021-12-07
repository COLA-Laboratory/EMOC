#pragma once
#include <vector>
#include <string>
#include <unordered_map>

#include "imgui.h"
#include "core/emoc_manager.h"

namespace emoc {

	class TestPanel
	{
	public:
		TestPanel();
		~TestPanel();

		void Render();

	private:
		// variouse windows display function
		void DisplayParameterWindow(bool is_finish, bool is_pause);
		void DisplayControlWindow(bool is_finish, bool is_pause);
		void DisplayResultWindow(bool is_finish, bool is_pause);
		void DisplayPlotWindow();

		// utility display function
		void DisplaySelectedRun(int index);
		void DisplayMovePopup(int index, bool& is_delete);		
		void DisplayAccordingToColumn(const EMOCSingleThreadResult& res, const std::string& col_name, int row);	// column display in default test module's table 

		// construct and send plot command (for plot window)
		void ConstructAndSendPlotCMD();
		void ConstructPopulationPlotCMD(char *cmd, int selected_run_intex);
		void ConstructMetricPlotCMD(char *cmd, int selected_run_intex, const std::string &metric_name, int display_num);

	public:
		// data for test module parameter settings
		int algorithm_index, problem_index, display_index;	
		std::vector<char*> algorithm_names;
		std::vector<char*> problem_names;
		std::vector<char*> display_names;
		int current_evaluation = 0;
		int max_evaluation = 1000000;
		int N = 100;
		int D = 30;
		int M = 2;
		int Evaluation = 25000;
		int plot_size[2] = {500, 500};
		int	plot_position[2] = {400, 800}; // settings for test module population plot

		// for plot analysis window
		bool is_plot_window_open = false;

		// for plot runs selection
		int run_index;
		std::vector<std::string> avail_runs_string;	 // updated when a single result has been collected
		std::vector<const char*> avail_runs;		 // transfer above string vector to c-style string vector, because of the imgui resons

		// for selected runs settings
		std::vector<char*> plot_metrics;
		std::vector<int> selected_runs;
		std::vector<int> plot_metric_indexes;
		std::vector<int> plot_display_nums;

	};

}