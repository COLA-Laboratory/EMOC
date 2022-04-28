#include "ui/experiment_panel.h"

#include <cmath>
#include <iostream>
#include <thread>

#include "core/emoc_app.h"
#include "imgui.h"
#include "ui/uipanel_manager.h"
#include "IconsFontAwesome5.h"

namespace emoc {

	static double CalculateDisplayIndicator(const std::vector<double>& indicator_history, const std::vector<bool>& is_indicator_record, bool &is_valid)
	{
		double res = 0.0;
		int count = 0;
		for (int i = 0; i < indicator_history.size(); i++)
		{
			if (is_indicator_record[i])
			{
				res += indicator_history[i];
				count++;
			}
		}
		if (count > 0) is_valid = true;
		return res / (double)count;
	}


	ExperimentPanel::ExperimentPanel():
		algorithm_index(0),
		problem_index(0),
		display_index(0),
		format_index(0),
		hypothesis_index(0),
		compared_index(0),
		problem_category_index(0),
		algorithm_category_index(0),
		current_algorithm_names(nullptr),
		current_problem_names(nullptr)
	{
		InitDisplayList(display_names);
		InitFormatList(format_names);
		InitHypothesisList(hypothesis_names);
		InitAlgorithmCategoryList(algorithm_category_list);
		InitProblemCategoryList(problem_category_list);
	}

	ExperimentPanel::~ExperimentPanel()
	{

	}

	void ExperimentPanel::Render()
	{		
		// update EMOC experiment module running state
		bool is_finish = EMOCManager::Instance()->GetExperimentFinish();
		bool is_pause = EMOCManager::Instance()->GetExperimentPause();

		if (!is_finish) ImGui::BeginDisabled();
		// menu bar
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Mode"))
			{
				if (ImGui::MenuItem("Test Module")) { UIPanelManager::Instance()->SetUIPanelState(UIPanel::TestPanel); PlotManager::Instance()->ClosePlotPipe(); }
				if (ImGui::MenuItem("Experiment Module")) { UIPanelManager::Instance()->SetUIPanelState(UIPanel::ExperimentPanel); PlotManager::Instance()->ClosePlotPipe(); }
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		DisplaySelectionWindow(is_finish, is_pause);
		DisplayParameterWindow(is_finish, is_pause);
		if (!is_finish) ImGui::EndDisabled();

		if (is_finish) ImGui::BeginDisabled();
		DisplayControlWindow(is_finish, is_pause);
		if (is_finish) ImGui::EndDisabled();

		DisplayResultWindow(is_finish, is_pause);
	}

	void ExperimentPanel::DisplaySelectionWindow(bool is_finish, bool is_pause)
	{
		// for testing
		clock_t start, end;

		ImGui::ShowDemoWindow();

		// Experiment Module Algorithm and Problem Selection Window
		{
			ImGui::Begin("Algorithm and Problem Selection##Experiment");

			// get some basic window property
			float window_width = ImGui::GetWindowSize().x;
			float window_height = ImGui::GetWindowSize().y;
			const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
			int list_height = (int)(0.2f * window_height / TEXT_BASE_HEIGHT);		// adaptive list height in number of items
			list_height = list_height > 3 ? list_height : 3;						// minimal height of list

			// Algorithm selection part
			TextCenter("Algorithm Selection");
			ImGui::Dummy(ImVec2(0.0f, 2.0f));
			ImGui::Text("Catergory"); ImGui::SameLine(); ImGui::Dummy(ImVec2(10.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(-FLT_MIN);
			bool is_value_changed = ImGui::Combo("##AlgorithmCategortyExpCombo", &algorithm_category_index, ItemGet,
				reinterpret_cast<void*>(&algorithm_category_list), algorithm_category_list.size());
			if (is_value_changed) algorithm_index = 0;
			SelectCurrentAlgorithmCombo(algorithm_category_list[algorithm_category_index], &current_algorithm_names);
			ImGui::SetNextItemWidth(-FLT_MIN);
			is_value_changed = ImGui::ListBox("##AlgorithmExperimentListbox", &algorithm_index, (*current_algorithm_names).data(), (*current_algorithm_names).size(), list_height);
			if (is_value_changed && selected_algorithm_map.count((*current_algorithm_names)[algorithm_index]) == 0)
			{
				selected_algorithm_map[(*current_algorithm_names)[algorithm_index]] = 1;
				selected_algorithms.push_back((*current_algorithm_names)[algorithm_index]);
			}
			ImGui::Dummy(ImVec2(0.0f, 20.0f));


			// Problem selection part
			ImGui::Separator();
			TextCenter("Problem Selection");
			ImGui::Dummy(ImVec2(0.0f, 2.0f));
			ImGui::Text("Catergory"); ImGui::SameLine(); ImGui::Dummy(ImVec2(10.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(-FLT_MIN);
			is_value_changed = ImGui::Combo("##ProblemCategortyExpCombo", &problem_category_index, ItemGet,
				reinterpret_cast<void*>(&problem_category_list), problem_category_list.size());
			if (is_value_changed) problem_index = 0;
			SelectCurrentProblemCombo(problem_category_list[problem_category_index], &current_problem_names);
			ImGui::SetNextItemWidth(-FLT_MIN);
			is_value_changed = ImGui::ListBox("##ProblemExperimentListbox", &problem_index, (*current_problem_names).data(), (*current_problem_names).size(), list_height);
			if (is_value_changed)
			{
				selected_problems.push_back((*current_problem_names)[problem_index]);

				// add some default problem settings
				Ns.push_back(100);
				Ms.push_back(2);
				Ds.push_back(30);
				Evaluations.push_back(100000);
			}
			ImGui::Dummy(ImVec2(0.0f, 20.0f));

			// set number of run and save interval inputs position
			float current_posY = ImGui::GetCursorPosY();
			float max_text_width = ImGui::CalcTextSize("Number of runs").x;
			float remain_width = window_width - max_text_width;
			float input_pos = max_text_width + 0.28f * remain_width;
			float height_pos = (window_height * 0.902f - TEXT_BASE_HEIGHT * 4.0f);
			height_pos = height_pos > current_posY ? height_pos : current_posY + 10.0f;

			ImGui::SetCursorPosY(height_pos);
			ImGui::PushItemWidth(0.71f * remain_width);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Thread Num");
			ImGui::SameLine(); ImGui::SetCursorPosX(input_pos);
			ImGui::InputInt("##ThreadNum", &thread_num, 0);

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Number of Runs");
			ImGui::SameLine(); ImGui::SetCursorPosX(input_pos);
			ImGui::InputInt("##Runs", &run_num, 0);

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Save Interval");
			ImGui::SameLine(); HelpMarker("Population file is saved per 'Save Interval' generations.\n"
				"If 'Save Interval' > max generation number, it will save\n"
				" the first and last generation as default. ");
			ImGui::SameLine(); ImGui::SetCursorPosX(input_pos);
			ImGui::InputInt("##SaveInterval", &save_interval, 0);
			ImGui::PopItemWidth();

			// set start button position
			current_posY = ImGui::GetCursorPosY();
			height_pos = window_height * 0.902f > current_posY ? window_height * 0.902f : current_posY + 10.0f;
			ImGui::SetCursorPos(ImVec2(0.025f * window_width, height_pos));

			static std::string description = "";
			// When algorithm is running, we diable 'Start' button.
			if (!is_finish) ImGui::BeginDisabled();
			if (ImGui::Button("Start##Experiment", ImVec2(window_width * 0.95f, window_height * 0.08f)))
			{
				EMOCManager::Instance()->SetIsExperiment(true);

				// set the data display in table
				table_algorithms = selected_algorithms;
				table_problems = selected_problems;
				table_Ns = Ns;
				table_Ds = Ds;
				table_Ms = Ms;
				table_Evaluations = Evaluations;

				start = clock();
				std::vector<int> parameter_indexes;
				for (int i = 0; i < table_problems.size(); i++)
					parameter_indexes.push_back(i * table_algorithms.size());
				
				table.UpdateExperimentTable(table_algorithms, table_problems, table_Ms,
					table_Ds, table_Ns, table_Evaluations, parameter_indexes);
				end = clock();

				ConstructTasks();
				int optimizaiton_type;
				bool is_optimization_legal = CheckOptimizationType(selected_algorithms, selected_problems, description, &optimizaiton_type);
				bool is_valid1 = CheckExpSettings(thread_num, run_num, save_interval, description);
				bool is_valid2 = true;
				for (int i = 0; i < table_problems.size(); i++)
					is_valid2 &= CheckProblemParameters(table_problems[i], table_Ds[i], table_Ms[i], table_Ns[i], table_Evaluations[i], description);

				// reset display names
				display_index = 0;
				if (optimizaiton_type == 0) InitSingleDisplayList(display_names);
				else if (optimizaiton_type == 1) InitMultiDisplayList(display_names);

				// start to run when the parameter is valid
				if (is_optimization_legal && is_valid1 && is_valid2)
				{
					if (experiment_tasks.size() > 0)
					{
						{
							std::lock_guard<std::mutex> locker(EMOCLock::multithread_data_mutex);
							EMOCManager::Instance()->SetMultiThreadDataState(false);
						}

						std::thread algorithm_thread(&EMOCManager::ExperimentModuleRun, EMOCManager::Instance(), experiment_tasks, thread_num);
						algorithm_thread.detach();
					}
				}
				else
				{
					ImGui::OpenPopup("Parameter Checking##Test");
				}

			}
			if (!is_finish) ImGui::EndDisabled();

			// parameter checking popup
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(350.0f, 0.0f));
			if (ImGui::BeginPopupModal("Parameter Checking##Test", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::TextWrapped("%s", description.c_str());
				ImGui::Separator();

				float width = ImGui::GetWindowSize().x;
				ImGui::SetCursorPosX((width - 150) / 2.0f);
				if (ImGui::Button("OK", ImVec2(150, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}


			ImGui::End();
		}
	}

	void ExperimentPanel::DisplayParameterWindow(bool is_finish, bool is_pause)
	{
		// Experiment Module Parameter Setting Window
		{
			ImGui::Begin("Parameter Setting##Experiment");

			TextCenter("Selected Algorithm");
			ImGui::Dummy(ImVec2(0.0f, 2.0f));
			for (int i = 0; i < selected_algorithms.size(); i++)
				DisplaySelectedAlgorithm(i);
			ImGui::Dummy(ImVec2(0.0f, 20.0f));
			ImGui::Separator();


			TextCenter("Problem Selection");
			// set selected problem display position
			float window_width = ImGui::GetWindowSize().x;
			float window_height = ImGui::GetWindowSize().y;
			float max_text_width = ImGui::CalcTextSize("Evaluation").x;
			float item_width = (window_width - max_text_width) * 0.81f;
			float item_pos = max_text_width + 0.18f * (window_width - max_text_width);

			ImGui::Dummy(ImVec2(0.0f, 2.0f));
			for (int i = 0; i < selected_problems.size(); i++)
				DisplaySelectedProblem(i, item_width, item_pos);
			
			ImGui::End();
		}
	}

	void ExperimentPanel::DisplayControlWindow(bool is_finish, bool is_pause)
	{
		// Experiment Moduel Control Window
		{
			ImGui::Begin("Control##Experiment");
			float window_width = ImGui::GetWindowSize().x;
			float window_height = ImGui::GetWindowSize().y;
			float text_width = ImGui::CalcTextSize("1000000 evaluations").x;
			float remain_width = window_width - text_width;
			float remain_height = (window_height - 150.0f) > 0.0f ? window_height - 150.0f : 0.0f;
			ImGui::Dummy(ImVec2(0.0f, remain_height * 0.5f));	// for vertical center

			// put the button at the appropriate position
			float button_pos = remain_width * 0.95f > 320.0f ? (remain_width * 0.95 - 320.0) * 0.5f : 0.0f;
			ImGui::SetCursorPosX(button_pos);

			// When algorithm (is not running) or (running but not paused), we diable 'Continue' button.
			if (is_finish || (!is_finish && !is_pause)) ImGui::BeginDisabled();
			if (ImGui::Button("Continue##Experiment", ImVec2(100, 60)))
			{
				std::lock_guard<std::mutex> locker(EMOCLock::experiment_pause_mutex);
				EMOCManager::Instance()->SetExperimentPause(false);
				EMOCLock::experiment_pause_cond.notify_all();
			}
			ImGui::SameLine(); ImGui::Dummy(ImVec2(10.0f, 0.0f)); ImGui::SameLine();
			if (is_finish || (!is_finish && !is_pause)) ImGui::EndDisabled();

			// When algorithm (is not running) or (is running but paused), we diable 'Pause' and 'Stop' button.
			if (is_finish || (!is_finish && is_pause)) ImGui::BeginDisabled();
			if (ImGui::Button("Pause##Experiment", ImVec2(100, 60)))
			{
				std::lock_guard<std::mutex> locker(EMOCLock::experiment_pause_mutex);
				EMOCManager::Instance()->SetExperimentPause(true);
			}
			ImGui::SameLine(); ImGui::Dummy(ImVec2(10.0f, 0.0f)); ImGui::SameLine();

			if (ImGui::Button("Stop##Experiment", ImVec2(100, 60)))
			{
				std::lock_guard<std::mutex> locker(EMOCLock::experiment_finish_mutex);
				EMOCManager::Instance()->SetExperimentFinish(true);
			}
			ImGui::SameLine(); ImGui::Dummy(ImVec2(10.0f, 0.0f)); ImGui::SameLine(); 
				if (is_finish || (!is_finish && is_pause)) ImGui::EndDisabled();

			ImGui::End();
		}
	}

	void ExperimentPanel::DisplayResultWindow(bool is_finish, bool is_pause)
	{
		// Experiment Module Result Infomation Window
		{
			ImGui::Begin("Result Infomation##Experiment");
			const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
			const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
			static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;
			
			// table display options
			std::vector<std::string> columns;
			TextCenter("Result Info Display");
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			static bool is_displayM = true; ImGui::Checkbox("M##Experiment", &is_displayM); ImGui::SameLine();
			static bool is_displayD = true; ImGui::Checkbox("D##Experiment", &is_displayD); ImGui::SameLine();
			static bool is_displayN = false; ImGui::Checkbox("N##Experiment", &is_displayN); ImGui::SameLine();
			static bool is_displayEvaluation = false; ImGui::Checkbox("Evaluation##Experiment", &is_displayEvaluation); ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::CalcTextSize("Runtimexxxx").x);
			ImGui::Combo("Metric##DisplayExperiment", &display_index, display_names.data(), display_names.size()); ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::CalcTextSize("Median(IQR)xxxx").x);
			ImGui::Combo("Format##DisplayFormatExperiment", &format_index, format_names.data(), format_names.size()); ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::CalcTextSize("RankSumTestxxxx").x);
			ImGui::Combo("Test##HypothesisExperiment", &hypothesis_index, hypothesis_names.data(), hypothesis_names.size()); ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::CalcTextSize("Defaultxxxx").x);
			ImGui::Combo("Comp##ComparedExperiment", &compared_index, compared_names.data(), compared_names.size()); ImGui::SameLine();

			// table save event
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x-60.0f);
			static char save_button[256] = ICON_FA_SAVE "##ExperimentSave";
			static bool is_save_success = true;
			if (ImGui::Button(save_button, ImVec2(50.0f, 0.0f)))
			{
				is_save_success = table.Save();
				ImGui::OpenPopup("Save Result##Experiment");
			}

			// Always center this window when appearing
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(400.0f, 0.0f));
			char popup_text[256];
			if (ImGui::BeginPopupModal("Save Result##Experiment", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				if (is_save_success)
					sprintf(popup_text, "Save Success!\nThe table content has been saved to output\/table\/new.tex and output\/table\/new.csv!\n\n");
				else if (!is_save_success)
					sprintf(popup_text, "Save Failure!\nOutput\/table\/new.csv or output\/table\/new.tex cannot be opened!\n\n");

				ImGui::TextWrapped("%s", popup_text);
				ImGui::Separator();

				float width = ImGui::GetWindowSize().x;
				ImGui::SetCursorPosX((width - 150) / 2.0f);
				if (ImGui::Button("OK", ImVec2(150, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}

			// set this frame's columns
			if (is_displayM) columns.push_back("M");
			if (is_displayD) columns.push_back("D");
			if (is_displayN) columns.push_back("N");
			if (is_displayEvaluation) columns.push_back("Evaluation");

			table.Render(is_displayM, is_displayD, is_displayN, is_displayEvaluation,
				display_names[display_index],format_names[format_index], hypothesis_names[hypothesis_index], compared_names[compared_index]);

			ImGui::End();
		}
	}

	void ExperimentPanel::DisplayTableProblemProperty(const std::string& col_name, int row)
	{
		if (col_name == "N")
			ImGui::Text("%d", table_Ns[row]);
		else if (col_name == "M")
			ImGui::Text("%d", table_Ms[row]);
		else if (col_name == "D")
			ImGui::Text("%d", table_Ds[row]);
		else if (col_name == "Evaluation")
			ImGui::Text("%d", table_Evaluations[row]);
	}

	void ExperimentPanel::DisplaySelectedAlgorithm(int index)
	{
		const std::string& algorithm = selected_algorithms[index];
		std::string header_name = algorithm + "##" + std::to_string(index);
		bool is_open = false;
		bool is_delete = false;

		float operation_button_pos = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("X").x;

		if (is_open = ImGui::CollapsingHeader(header_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap))
		{
			DisplayMovePopup(index, operation_button_pos, true, is_delete);
		}
		if (!is_open) DisplayMovePopup(index, operation_button_pos, true, is_delete);
	}

	void ExperimentPanel::DisplaySelectedProblem(int index, int item_width, int item_pos)
	{
		char name[256];
		const std::string& problem = selected_problems[index];
		std::string header_name = problem + "##" + std::to_string(index);
		bool is_open = false;
		bool is_delete = false;

		float operation_button_pos = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("X").x;

		if (is_open = ImGui::CollapsingHeader(header_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap))
		{
			DisplayMovePopup(index, operation_button_pos,false, is_delete);
			if (!is_delete)
			{
				ImGui::PushItemWidth(item_width);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("D"); ImGui::SameLine();
				ImGui::SetCursorPosX(item_pos);
				sprintf(name, "##DExperiment%d", index);
				ImGui::InputInt(name, &Ds[index], 0);

				ImGui::AlignTextToFramePadding();
				ImGui::Text("M"); ImGui::SameLine();
				ImGui::SetCursorPosX(item_pos);
				sprintf(name, "##MExperiment%d", index);
				ImGui::InputInt(name, &Ms[index], 0);

				ImGui::AlignTextToFramePadding();
				ImGui::Text("N"); ImGui::SameLine();
				ImGui::SetCursorPosX(item_pos);
				sprintf(name, "##NExperiment%d", index);
				ImGui::InputInt(name, &Ns[index], 0);

				ImGui::AlignTextToFramePadding();
				ImGui::Text("Evaluation"); ImGui::SameLine();
				ImGui::SetCursorPosX(item_pos);
				sprintf(name, "##EvaluationExperiment%d", index);
				ImGui::InputInt(name, &Evaluations[index], 0);
				ImGui::PopItemWidth();
			}
		}
		if (!is_open) DisplayMovePopup(index, operation_button_pos, false, is_delete);
	}

	void ExperimentPanel::DisplayMovePopup(int index, float button_pos, bool is_algorithm_popup,bool &is_delete)
	{
		char icon_name[128];
		char popup_name[128];
		if (is_algorithm_popup)
		{
			sprintf(icon_name, "%s##ExpAlgorithmCog%d", ICON_FA_COG, index);
			sprintf(popup_name, "Popup##ExpAlgorithm%d", index);
		}
		else
		{
			sprintf(icon_name, "%s##ExpProblemCog%d", ICON_FA_COG, index);
			sprintf(popup_name, "Popup##ExpProblem%d", index);
		}
		ImGui::SameLine();
		ImGui::SetCursorPosX(button_pos);
		if (ImGui::Button(icon_name))
		{
			ImGui::OpenPopup(popup_name);
		}

		if (ImGui::BeginPopup(popup_name))
		{
			if(ImGui::Button("Move Up     "))
			{
				if (index >= 1)
				{
					if (is_algorithm_popup) 
						std::swap(selected_algorithms[index], selected_algorithms[index - 1]);
					else
					{
						std::swap(selected_problems[index], selected_problems[index - 1]);
						std::swap(Ns[index], Ns[index - 1]);
						std::swap(Ds[index], Ds[index - 1]);
						std::swap(Ms[index], Ms[index - 1]);
						std::swap(Evaluations[index], Evaluations[index - 1]);
					}
				}
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("Move Down"))
			{
				if (is_algorithm_popup)
				{
					if (index < selected_algorithms.size() - 1)
						std::swap(selected_algorithms[index], selected_algorithms[index + 1]);
					
				}
				else
				{
					if (index < selected_problems.size() - 1)
					{
						std::swap(selected_problems[index], selected_problems[index + 1]);
						std::swap(Ns[index], Ns[index + 1]);
						std::swap(Ds[index], Ds[index + 1]);
						std::swap(Ms[index], Ms[index + 1]);
						std::swap(Evaluations[index], Evaluations[index + 1]);
					}
				}
				ImGui::CloseCurrentPopup();
			}
			if(ImGui::Button("Delete         "))
			{
				// Note that vector::erase() will cause invalid index for traversing,
				// but it only display wrong ui in current frame and fixed in next frame.
				// So we leave the kind of "wrong" code here alone for the simplicity.
				// It is not recommended in normal programming.
				if (is_algorithm_popup)
				{
					selected_algorithm_map.erase(selected_algorithms[index]);
					selected_algorithms.erase(selected_algorithms.begin() + index);
				}
				else
				{
					selected_problems.erase(selected_problems.begin() + index);
					Ns.erase(Ns.begin() + index);
					Ds.erase(Ds.begin() + index);
					Ms.erase(Ms.begin() + index);
					Evaluations.erase(Evaluations.begin() + index);
				}
				is_delete = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void ExperimentPanel::ConstructTasks()
	{
		experiment_tasks.clear();
		for (int i = 0; i < selected_problems.size(); i++)
		{
			std::string problem = selected_problems[i];
			int N = Ns[i];
			int M = Ms[i];
			int D = Ds[i];
			int Evaluation = Evaluations[i];

			for (int j = 0; j < selected_algorithms.size(); j++)
			{
				std::string algorithm = selected_algorithms[j];

				EMOCParameters para;
				para.is_plot = false;
				para.algorithm_name = algorithm;
				para.problem_name = problem;
				para.population_num = N;
				para.objective_num = M;
				para.decision_num = D;
				para.max_evaluation = Evaluation;
				para.runs_num = run_num;
				para.output_interval = save_interval;
				experiment_tasks.push_back(para);
			}
		}
	}

}
