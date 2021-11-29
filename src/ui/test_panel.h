#pragma once
#include <vector>

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

	public:
		// for test now
		bool show_demo_window = true;

		// UI data for test module
		int algorithm_index, problem_index, display_index;	
		std::vector<char*> algorithm_names;
		std::vector<char*> problem_names;
		std::vector<char*> display_names;

		int current_evaluation;
		int max_evaluation;
		int N = 100;
		int D = 30;
		int M = 2;
		int Evaluation = 26000;

	};

}