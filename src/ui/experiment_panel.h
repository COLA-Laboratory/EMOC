#pragma once
#include <vector>
#include <string>
#include <unordered_map>

#include "core/emoc_manager.h"
#include "ui/ui_utility.h"

namespace emoc {

	class ExperimentPanel
	{
	public:
		ExperimentPanel();
		~ExperimentPanel();

		void Render();

	private:
		// various window
		void DisplaySelectionWindow(bool is_finish, bool is_pause);
		void DisplayParameterWindow(bool is_finish, bool is_pause);
		void DisplayControlWindow(bool is_finish, bool is_pause);
		void DisplayResultWindow(bool is_finish, bool is_pause);

		// display functions according current settings		
		void DisplayTableResult(const EMOCMultiThreadResult& res, const std::string& para);	// display table content according to selected display parameter 
		void DisplayTableProblemProperty(const std::string& col_name, int row);	// column display in default test module's table 
		void DisplaySelectedAlgorithm(int index);								// algorithm display in experiment module's parameter window 
		void DisplaySelectedProblem(int index, int item_width, int item_pos);	// problem display in experiment module's parameter window
		void DisplayMovePopup(int index, float button_pos, bool is_algorithm_popup, bool &is_delete);				// popups in experiment module's parameter window for moving or deleting selected algorithms and problems

		//
		void UpdateCurrentAlgorithmList();
		void UpdateCurrentProblemList();
		

		void ConstructTasks();

	public:
		// UI data for experiment module's list and combo
		int algorithm_index, problem_index, display_index;	// index need to be reset when switching the panel
		CategorizedAlgorithmList algorithm_list;
		CategorizedProblemList problem_list;
		std::vector<char*> *current_algorithm_names;
		std::vector<char*> *current_problem_names;
		std::vector<char*> display_names;

		// UI data for experiment module
		std::vector<std::string> selected_algorithms;
		std::unordered_map<std::string, int> selected_algorithm_map;
		std::vector<std::string> selected_problems;
		std::unordered_map<std::string, int> selected_problem_map;
		std::vector<int> Ns;
		std::vector<int> Ds;
		std::vector<int> Ms;
		std::vector<int> Evaluations;
		int run_num = 30;
		int thread_num = 8;
		int save_interval = 1000000;
		std::vector<EMOCParameters> experiment_tasks;

		// Table data for experiment module
		std::vector<std::string> table_algorithms;
		std::vector<std::string> table_problems;
		std::vector<int> table_Ns;
		std::vector<int> table_Ds;
		std::vector<int> table_Ms;
		std::vector<int> table_Evaluations;

	};

}