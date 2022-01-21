#pragma once
#include <vector>
#include <string>
#include <unordered_map>

#include "core/emoc_manager.h"
#include "ui/ui_utility.h"
#include "ui/experiment_table.h"

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
		void DisplayTableProblemProperty(const std::string& col_name, int row);	
		void DisplaySelectedAlgorithm(int index);								// algorithm display in experiment module's parameter window 
		void DisplaySelectedProblem(int index, int item_width, int item_pos);	// problem display in experiment module's parameter window
		void DisplayMovePopup(int index, float button_pos, bool is_algorithm_popup, bool &is_delete);				// popups in experiment module's parameter window for moving or deleting selected algorithms and problems

		//
		void ConstructTasks();

	public:
		// UI data for experiment module's list and combo
		int algorithm_index, problem_index, display_index, format_index, hypothesis_index, compared_index;	// index need to be reset when switching the panel
		int algorithm_category_index, problem_category_index;
		std::vector<std::string> algorithm_category_list;
		std::vector<std::string> problem_category_list;
		std::vector<char*> *current_algorithm_names;
		std::vector<char*> *current_problem_names;
		std::vector<char*> display_names;
		std::vector<char*> format_names;
		std::vector<char*> hypothesis_names;
		std::vector<char*> compared_names = {"Best", "Default"};

		// UI data for experiment module
		std::vector<std::string> selected_algorithms;
		std::unordered_map<std::string, int> selected_algorithm_map;
		std::vector<std::string> selected_problems;
		std::vector<int> Ns;
		std::vector<int> Ds;
		std::vector<int> Ms;
		std::vector<int> Evaluations;
		int run_num = 20;
		int thread_num = 8;
		int save_interval = 1000000;
		std::vector<EMOCParameters> experiment_tasks;

		// Table data for experiment module, seperate from UI data for static display, i.e. don't change with UI data when current experiment has been finished.
		std::vector<std::string> table_algorithms;
		std::vector<std::string> table_problems;
		std::vector<int> table_Ns;
		std::vector<int> table_Ds;
		std::vector<int> table_Ms;
		std::vector<int> table_Evaluations;
		ExperimentTable table;

	};

}