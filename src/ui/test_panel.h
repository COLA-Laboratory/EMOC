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
		void DisplayAccordingToColumn(const EMOCSingleThreadResult& res, const std::string& col_name, int row);	// column display in default test module's table 

		// variouse windows
		void DisplayParameterWindow(bool is_finish, bool is_pause);
		void DisplayControlWindow(bool is_finish, bool is_pause);
		void DisplayResultWindow(bool is_finish, bool is_pause);
		void DisplayPlotWindow();
		void DisplaySelectedRun(int index);
		void DisplayMovePopup(int index, bool &is_delete);

		// construct and send plot command
		void ConstructAndSendPlotCMD();
		void ConstructPopulationPlotCMD(char *cmd, int avail_run_index);
		void ConstructMetricPlotCMD(char *cmd, int avail_run_index, const std::string &metric_name, int display_num);

	public:
		// for test now
		bool show_demo_window = true;

		// UI data for test module
		int algorithm_index, problem_index, display_index;	
		std::vector<char*> algorithm_names;
		std::vector<char*> problem_names;
		std::vector<char*> display_names;

		// for algorithm and problem settings
		int current_evaluation;
		int max_evaluation;
		int N = 100;
		int D = 30;
		int M = 2;
		int Evaluation = 26000;

		// for plot analysis window
		bool is_plot_window_open = false;

		// for plot runs selection
		int run_index;
		std::vector<std::string> avail_runs_string;
		std::vector<const char*> avail_runs;

		// for selected runs settings
		std::vector<char*> plot_metrics;
		std::vector<int> selected_runs;
		std::vector<int> plot_metric_indexes;
		std::vector<int> plot_display_nums;

	};

}