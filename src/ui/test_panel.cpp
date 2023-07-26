#include "ui/test_panel.h"

#include <iostream>
#include <thread>
#include <cmath>
#include "core/emoc_app.h"
#include "core/file.h"
#include "core/emoc_manager.h"
#include "metric/igd.h"
#include "ui/uipanel_manager.h"
#include "ui/plot_manager.h"
#include "IconsFontAwesome5.h"

namespace emoc {

	TestPanel::TestPanel():
		algorithm_index(0),
		problem_index(0),
		display_index(0),
		algorithm_category_index(0),
		problem_category_index(0),
		current_algorithm_names(nullptr),
		current_problem_names(nullptr),
		run_index(0),
		is_plot_window_open(false)
	{
		// init basic display lists
		InitDisplayList(display_names);
		InitPlotMetricList(plot_metrics);
		InitAlgorithmCategoryList(algorithm_category_list);
		InitProblemCategoryList(problem_category_list);
	}

	TestPanel::~TestPanel()
	{

	}

	void TestPanel::Render()
	{		
		//// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		//static bool show_demo_window = false;
		//if (show_demo_window)
		//	ImGui::ShowDemoWindow(&show_demo_window);

		// get EMOC running state
		bool is_finish = EMOCManager::Instance()->GetTestFinish();
		bool is_pause = EMOCManager::Instance()->GetTestPause();
		bool is_plot_open = is_plot_window_open;	// make a copy to avoid imgui assert problems

		if (is_plot_open) ImGui::BeginDisabled();	// disable all other windows when open plot window

		// menu bar and parameter window
		if (!is_finish) ImGui::BeginDisabled();
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
		DisplayParameterWindow(is_finish, is_pause);
		if (!is_finish) ImGui::EndDisabled();

		if (is_finish) ImGui::BeginDisabled();
		DisplayControlWindow(is_finish, is_pause);
		if (is_finish) ImGui::EndDisabled();

		DisplayResultWindow(is_finish, is_pause);

		if (is_plot_open) ImGui::EndDisabled(); // disable all other windows when open plot window

		if (is_plot_window_open)
			DisplayPlotWindow();
	}

	void TestPanel::DisplayParameterWindow(bool is_finish, bool is_pause)
	{
		// Test Module Parameter Setting Window
		{
			// Algorithm selection part
			ImGui::Begin("EMOC Parameter Setting##Test");
			TextCenter("Algorithm Selection");
			ImGui::Dummy(ImVec2(0.0f, 2.0f));
			ImGui::Text("Catergory"); ImGui::SameLine(); ImGui::Dummy(ImVec2(10.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(-FLT_MIN);

			bool is_value_changed = ImGui::Combo("##AlgorithmCategortyTestCombo", &algorithm_category_index, ItemGet, 
				reinterpret_cast<void*>(&algorithm_category_list), algorithm_category_list.size());

			if (is_value_changed) algorithm_index = 0;
			//UpdateCurrentAlgorithmCombo();
			SelectCurrentAlgorithmCombo(algorithm_category_list[algorithm_category_index], &current_algorithm_names);
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::Combo("##AlgorithmTestCombo", &algorithm_index, (*current_algorithm_names).data(), (*current_algorithm_names).size());
			
			ImGui::Dummy(ImVec2(0.0f, 20.0f));
			ImGui::Separator();

			// Problem selection part
			TextCenter("Problem Selection");
			ImGui::Dummy(ImVec2(0.0f, 2.0f));
			ImGui::Text("Catergory"); ImGui::SameLine(); ImGui::Dummy(ImVec2(10.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(-FLT_MIN);
			is_value_changed = ImGui::Combo("##ProblemCategortyTestCombo", &problem_category_index, ItemGet, 
				reinterpret_cast<void*>(&problem_category_list), problem_category_list.size());

			if (is_value_changed) problem_index = 0;
			SelectCurrentProblemCombo(problem_category_list[problem_category_index], &current_problem_names);
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::Combo("##ProblemTestCombo", &problem_index, (*current_problem_names).data(), (*current_problem_names).size());

			float window_width = ImGui::GetWindowSize().x;
			float window_height = ImGui::GetWindowSize().y;
			float max_text_width = ImGui::CalcTextSize("Evaluation").x;
			float remain_width = (window_width - max_text_width) > 0.0 ? (window_width - max_text_width) : 0.0;
			float input_pos = max_text_width + 0.18f * remain_width;
			ImGui::PushItemWidth(0.81f * remain_width);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("n"); ImGui::SameLine();
			ImGui::SetCursorPosX(input_pos);
			ImGui::InputInt("##DTest", &D, 0);

			ImGui::AlignTextToFramePadding();
			ImGui::Text("m"); ImGui::SameLine();
			ImGui::SetCursorPosX(input_pos);
			ImGui::InputInt("##MTest", &M, 0);

			ImGui::AlignTextToFramePadding();
			ImGui::Text("N"); ImGui::SameLine();
			ImGui::SetCursorPosX(input_pos);
			ImGui::InputInt("##NTest", &N, 0);

			ImGui::AlignTextToFramePadding();
			ImGui::Text("nFE"); ImGui::SameLine();
			ImGui::SetCursorPosX(input_pos);
			ImGui::InputInt("##EvaluationTest", &Evaluation, 0);
			ImGui::PopItemWidth();
			ImGui::Dummy(ImVec2(0.0f, 20.0f));
			ImGui::Separator();


			// Plot setting part
			TextCenter("Plot Setting");
			ImGui::Dummy(ImVec2(0.0f, 2.0f));
			max_text_width = ImGui::CalcTextSize("Position").x;
			remain_width = window_width - ImGui::CalcTextSize("Position(??)").x;
			input_pos = max_text_width + 0.19f * remain_width;

			ImGui::PushItemWidth(0.8f * remain_width);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Size"); ImGui::SameLine();
			ImGui::SetCursorPosX(input_pos);
			ImGui::InputInt2("##SizeTest", plot_size);
			ImGui::SameLine(); HelpMarker("Width and height of plot in pixels");

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Position"); ImGui::SameLine();
			ImGui::SetCursorPosX(input_pos);
			ImGui::InputInt2("##PositionTest", plot_position);
			ImGui::SameLine(); HelpMarker("Length in pixels to the left edge and up edge of your screen");
			ImGui::PopItemWidth();


			// set start button position
			float current_posY = ImGui::GetCursorPosY();
			float height_pos = window_height * 0.88f > current_posY ? window_height * 0.88f : current_posY + 10.0f;
			ImGui::SetCursorPos(ImVec2(0.025f * window_width, height_pos));

			// When algorithm is running, we diable 'Start' button.
			static std::string description = "";
			if (!is_finish) ImGui::BeginDisabled();
			if (ImGui::Button("Start##Test", ImVec2(window_width * 0.95f, window_height * 0.05f)))
			{
				// Reopen the pipe and set position and size.
				char pos_size_cmd[256];
				sprintf(pos_size_cmd, "set term %s position %d,%d size %d,%d\n",GNUPLOT_TERMINAL,  plot_position[0], plot_position[1], plot_size[0], plot_size[1]);
				PlotManager::Instance()->Send(pos_size_cmd);

				EMOCManager::Instance()->SetIsExperiment(false);

				std::cout << "\n-----------TEST MODULE TASK------------\n";
				std::cout << (*current_algorithm_names)[algorithm_index] << "\n"
					<< (*current_problem_names)[problem_index] << "\n"
					<< "population number: " << N << "\n"
					<< "obj dim: " << M << "\n"
					<< "dec dim: " << D << "\n"
					<< "evaluation number: " << Evaluation << "\n";
				std::cout << "---------------------------------------\n";

				current_evaluation = 0;
				max_evaluation = Evaluation;

				// create EMOC task
				EMOCParameters para;
				para.algorithm_name = (*current_algorithm_names)[algorithm_index];
				para.problem_name = (*current_problem_names)[problem_index];
				para.is_plot = true;
				para.objective_num = M;
				para.decision_num = D;
				para.population_num = N;
				para.max_evaluation = Evaluation;
				para.output_interval = 1;			// We store population of each iteration for plotting
				para.runs_num = 1;
				para.is_open_multithread = false;

				bool is_optimization_legal = CheckOptimizationType({ para.algorithm_name }, { para.problem_name }, description);
				bool is_valid1 = CheckProblemParameters(para.problem_name, D, M, N, Evaluation, description);
				bool is_valid2 = CheckTestPlotSettings(plot_size[0], plot_size[1], plot_position[0], plot_position[1], description);

				// start to run when the parameter is valid
				if (is_optimization_legal && is_valid1 && is_valid2)
				{
					EMOCManager::Instance()->SetTaskParameters(para);
					std::thread algorithm_thread(&EMOCManager::Run, EMOCManager::Instance());
					algorithm_thread.detach();
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

	void TestPanel::DisplayControlWindow(bool is_finish, bool is_pause)
	{
		// Test Moduel Control Window
		{
			ImGui::Begin("Control##Test");
			float window_width = ImGui::GetWindowSize().x;
			float window_height = ImGui::GetWindowSize().y;
			float text_width = ImGui::CalcTextSize("1000000 evaluations").x;
			float remain_width = window_width - text_width;		// width of progress bar 
			float remain_height = (window_height - 150.0f) > 0.0f ? window_height - 150.0f : 0.0f;
			ImGui::Dummy(ImVec2(0.0f, remain_height * 0.5f));	// for vertical center

			// a simple progress bar
			static float progress = 0.0f;
			progress = (float)current_evaluation / (float)max_evaluation;
			if (progress > 1.0f) progress = 1.0f;
			ImGui::SetNextItemWidth(remain_width * 0.95f);
			ImGui::ProgressBar(progress, ImVec2(0.f, 0.f));
			ImGui::SameLine(); ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine(); ImGui::Text("%d evaluations", current_evaluation);
			ImGui::Dummy(ImVec2(0.0f, 10.0f));

			// put the button at the appropriate position
			float button_pos = remain_width * 0.95f > 320.0f ? (remain_width * 0.95 - 320.0) * 0.5f : 0.0f;
			ImGui::SetCursorPosX(button_pos);

			// When algorithm (is not running) or (running but not paused), we diable 'Continue' button.
			if (is_finish || (!is_finish && !is_pause)) ImGui::BeginDisabled();
			if (ImGui::Button("Continue##Test", ImVec2(100, 60)))
			{
				std::lock_guard<std::mutex> locker(EMOCLock::test_pause_mutex);
				EMOCManager::Instance()->SetTestPause(false);
				EMOCLock::test_pause_cond.notify_all();
			}
			ImGui::SameLine(); ImGui::Dummy(ImVec2(10.0f, 0.0f)); ImGui::SameLine();
			if (is_finish || (!is_finish && !is_pause)) ImGui::EndDisabled();

			// When algorithm (is not running) or (is running but paused), we diable 'Pause' and 'Stop' button.
			if (is_finish || (!is_finish && is_pause)) ImGui::BeginDisabled();
			if (ImGui::Button("Pause##Test", ImVec2(100, 60)))
			{
				std::lock_guard<std::mutex> locker(EMOCLock::test_pause_mutex);
				EMOCManager::Instance()->SetTestPause(true);
			}
			ImGui::SameLine(); ImGui::Dummy(ImVec2(10.0f, 0.0f)); ImGui::SameLine();

			if (ImGui::Button("Stop##Test", ImVec2(100, 60)))
			{
				std::lock_guard<std::mutex> locker(EMOCLock::test_finish_mutex);
				EMOCManager::Instance()->SetTestFinish(true);
			}
			ImGui::SameLine(); ImGui::Dummy(ImVec2(10.0f, 0.0f)); ImGui::SameLine();
			if (is_finish || (!is_finish && is_pause)) ImGui::EndDisabled();

			ImGui::End();
		}
	}

	void TestPanel::DisplayResultWindow(bool is_finish, bool is_pause)
	{
		// Test Moduel Result Window
		{
			ImGui::Begin("Result Infomation##Test");
			const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
			const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
			static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;

			std::vector<std::string> columns;	// display table's columns
			TextCenter("Result Info Display");
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			static bool is_displayN = true; ImGui::Checkbox("N##Test", &is_displayN); ImGui::SameLine();
			static bool is_displayM = true; ImGui::Checkbox("m##Test", &is_displayM); ImGui::SameLine();
			static bool is_displayD = true; ImGui::Checkbox("n##Test", &is_displayD); ImGui::SameLine();
			static bool is_displayEvaluation = true; ImGui::Checkbox("Evaluation##Test", &is_displayEvaluation);

			// set this frame's table columns
			columns.push_back("Run #"); columns.push_back("Algorithm"); columns.push_back("Problem");
			if (is_displayN) columns.push_back("N");
			if (is_displayM) columns.push_back("M");
			if (is_displayD) columns.push_back("D");
			if (is_displayEvaluation) columns.push_back("Evaluation");
			columns.push_back("Runtime");

			// When using ScrollX or ScrollY we need to specify a size for our table container!
			// Otherwise by default the table will fit all available space, like a BeginChild() call.
			ImVec2 outer_size = ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 45.0f);
			if (ImGui::BeginTable("ResultTable##Test", columns.size(), flags, outer_size))
			{
				ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible

				// Instead of calling TableHeadersRow() we'll submit custom headers ourselves
				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
				for (int c = 0; c < columns.size(); c++)
				{
					ImGui::TableSetColumnIndex(c);
					const char* column_name = columns[c].c_str(); // Retrieve name passed to TableSetupColumn()

					// use combobox
					if (columns[c] == "Runtime")
					{
						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
						ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.18f, 0.18f, 0.32f, 1.00f));
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						ImGui::Combo(("##DisplayTest" + std::to_string(c)).c_str(), &display_index, display_names.data(), display_names.size());
						ImGui::PopStyleColor();
						ImGui::PopStyleVar();
					}
					else
						ImGui::TableHeader((column_name + std::string("##Test")).c_str());
				}
				columns[columns.size() - 1] = display_names[display_index];

				int rows = EMOCManager::Instance()->GetSingleThreadResultSize();
				for (int row = 0; row < rows; row++)
				{
					ImGui::TableNextRow();
					EMOCSingleThreadResult res = EMOCManager::Instance()->GetSingleThreadResult(row);
					for (int c = 0; c < columns.size(); c++)
					{
						ImGui::TableSetColumnIndex(c);
						DisplayAccordingToColumn(res, columns[c], row);
					}
				}

				ImGui::EndTable();
			}

			ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 260.0f);
			if (!is_finish) ImGui::BeginDisabled();
			if (ImGui::Button("Open Plot Window##Test", ImVec2(250.0f, 40.0f)))
			{
				// Open a new ImGui window for test module plotting analysis
				is_plot_window_open = true;

				// close the current gnuplot pipe
				PlotManager::Instance()->ClosePlotPipe();
			}
			if (!is_finish) ImGui::EndDisabled();

			ImGui::End();
		}
	}

	void TestPanel::DisplayPlotWindow()
	{
		{
			ImGui::Begin("Plotting Analysis##Test", &is_plot_window_open);

			// get some basic window property
			float w = ImGui::GetContentRegionAvail().x;
			float h = ImGui::GetContentRegionAvail().y;
			const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
			int list_height = (int)(0.7f * h / TEXT_BASE_HEIGHT);					// adaptive list height in number of items
			list_height = list_height > 3 ? list_height : 3;						// minimal height of list


			 float height1 = 0.79f * h;
			 float height2 = 0.19f * h;
			ImGui::BeginChild("1", ImVec2(w, height1), false, ImGuiWindowFlags_NoBackground);
			{
				 float width1 = 0.5f * w;
				 float width2 = 0.5f * w;

				ImGui::BeginChild("11", ImVec2(width1, height1), true);
				TextCenter("Runs Selection");
				ImGui::Dummy(ImVec2(0.0f, 2.0f));
				ImGui::SetNextItemWidth(-FLT_MIN);
				bool is_value_changed = ImGui::ListBox("##AvailableRunsListbox", &run_index, avail_runs.data(), avail_runs.size(), list_height);
				if (is_value_changed)
				{
					selected_runs.push_back(run_index);
					plot_metric_indexes.push_back(0);
					plot_display_nums.push_back(10);
				}
				ImGui::EndChild();
				ImGui::SameLine();

				ImGui::BeginChild("12", ImVec2(width2, height1), true);

				TextCenter("Selected Runs");
				ImGui::Dummy(ImVec2(0.0f, 2.0f));
				for (int i = 0; i < selected_runs.size(); i++)
					DisplaySelectedRun(i);

				ImGui::EndChild();
			}

			ImGui::EndChild();



			ImGui::BeginChild("2", ImVec2(w, height2), true);

			float window_width = ImGui::GetContentRegionAvail().x;
			float window_height = ImGui::GetContentRegionAvail().y;
			float text_width = ImGui::CalcTextSize("1000000 evaluations").x;
			float remain_width = window_width - text_width;
			float remain_height = (window_height - 150.0f) > 0.0f ? window_height - 150.0f : 0.0f;
			ImGui::Dummy(ImVec2(0.0f, remain_height * 0.5f));	// for vertical center

			// put the button at the appropriate position
			float button_pos = window_width * 0.95f > 400 ? (window_width * 0.95 - 400) * 0.5f : 0.0f;
			
			ImGui::SetCursorPosX(button_pos);
			if (ImGui::Button("Plot##PlotTest", ImVec2(400, 100)))
			{
				std::cout << "Plot!\n";
				ConstructAndSendPlotCMD();
			}
			ImGui::SameLine(); ImGui::Dummy(ImVec2(10.0f, 0.0f)); ImGui::SameLine();

			ImGui::EndChild();

			ImGui::End();
		}
	}

	void TestPanel::DisplayAccordingToColumn(const EMOCSingleThreadResult& res, const std::string& col_name, int row)
	{
		if (col_name == "Run #")
			ImGui::Text("Run %d", row);
		else if (col_name == "Algorithm")
			ImGui::Text(res.para.algorithm_name.c_str());
		else if (col_name == "Problem")
			ImGui::Text(res.para.problem_name.c_str());
		else if (col_name == "N")
			ImGui::Text(std::to_string(res.para.population_num).c_str());
		else if (col_name == "M")
			ImGui::Text(std::to_string(res.para.objective_num).c_str());
		else if (col_name == "D")
			ImGui::Text(std::to_string(res.para.decision_num).c_str());
		else if (col_name == "Evaluation")
			ImGui::Text(std::to_string(res.para.max_evaluation).c_str());
		else if (col_name == "Runtime")
			ImGui::Text((std::to_string(res.runtime) + "s").c_str());
		else if (col_name == "IGD")
		{
			if (std::fabs(res.last_igd + 1) < EMOC_EPS)
				ImGui::Text("None");
			else
				ImGui::Text(std::to_string(res.last_igd).c_str());
		}
		else if (col_name == "HV")
		{
			if (std::fabs(res.last_hv + 1) < EMOC_EPS)
				ImGui::Text("None");
			else
				ImGui::Text(std::to_string(res.last_hv).c_str());
		}
		else if (col_name == "GD")
		{
			if (std::fabs(res.last_gd + 1) < EMOC_EPS)
				ImGui::Text("None");
			else
				ImGui::Text(std::to_string(res.last_gd).c_str());
		}
		else if (col_name == "Spacing")
		{
			if (std::fabs(res.last_spacing + 1) < EMOC_EPS)
				ImGui::Text("None");
			else
				ImGui::Text(std::to_string(res.last_spacing).c_str());
		}
		else if (col_name == "IGDPlus")
		{
			if (std::fabs(res.last_igdplus + 1) < EMOC_EPS)
				ImGui::Text("None");
			else
				ImGui::Text(std::to_string(res.last_igdplus).c_str());
		}
		else if (col_name == "GDPlus")
		{
			if (std::fabs(res.last_gdplus + 1) < EMOC_EPS)
				ImGui::Text("None");
			else
				ImGui::Text(std::to_string(res.last_gdplus).c_str());
		}
		else if (col_name == "BestValue")
		{
			if (std::fabs(res.best_value + 1) < EMOC_EPS)
				ImGui::Text("None");
			else
				ImGui::Text(std::to_string(res.best_value).c_str());
		}
	}

	void TestPanel::DisplaySelectedRun(int index)
	{
		float window_width = ImGui::GetContentRegionAvail().x;
		float max_text_width = ImGui::CalcTextSize("Number of Display").x;
		float remain_width = (window_width - max_text_width) > 0.0 ? (window_width - max_text_width) :0.0;
		float input_pos = max_text_width + 0.10f * remain_width;
		float operation_button_pos = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("X ").x;

		int selected_index = selected_runs[index];
		const std::string& description = avail_runs[selected_index];
		std::string header_name = description + "##" + std::to_string(index);
		bool is_open = false;
		bool is_delete = false;

		if (is_open = ImGui::CollapsingHeader(header_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap))
		{
			DisplayMovePopup(index, operation_button_pos, is_delete);
			if (!is_delete)
			{
				char comboname[128];
				sprintf(comboname, "##PlotAnalysisTest%d", index);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Plot Metrics"); ImGui::SameLine();
				ImGui::SetCursorPosX(input_pos);
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::Combo(comboname, &plot_metric_indexes[index],plot_metrics.data(), plot_metrics.size());

				if (plot_metrics[plot_metric_indexes[index]] != std::string("Population") && plot_metrics[plot_metric_indexes[index]] != std::string("Runtime"))
				{
					char settingname[128];
					sprintf(settingname, "##PlotTest%s%d", plot_metrics[plot_metric_indexes[index]], index);
					ImGui::AlignTextToFramePadding();
					ImGui::Text("Number of Display");
					// tooltip
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted("'Number of Display' generations will be evenly selected to plot.");
						ImGui::PopTextWrapPos();
						ImGui::EndTooltip();
					}


					ImGui::SameLine();
					ImGui::SetCursorPosX(input_pos);
					ImGui::SetNextItemWidth(remain_width);
					ImGui::InputInt(settingname, &plot_display_nums[index], 0);
				}
			}
		}
		if (!is_open) DisplayMovePopup(index, operation_button_pos, is_delete);
	}

	void TestPanel::DisplayMovePopup(int index, float button_pos, bool& is_delete)
	{
		char icon_name[128];
		char popup_name[128];

		sprintf(icon_name, "%s##TestPlotCog%d", ICON_FA_COG, index);
		sprintf(popup_name, "Popup##TestPlot%d", index);

		ImGui::SameLine();
		ImGui::SetCursorPosX(button_pos);
		if (ImGui::Button(icon_name))
		{
			ImGui::OpenPopup(popup_name);
		}

		if (ImGui::BeginPopup(popup_name))
		{
			if (ImGui::Button("Move Up     "))
			{
				if (index >= 1)
				{
					std::swap(selected_runs[index], selected_runs[index - 1]);
					std::swap(plot_metric_indexes[index], plot_metric_indexes[index - 1]);
					std::swap(plot_display_nums[index], plot_display_nums[index - 1]);
				}
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("Move Down"))
			{

				if (index < selected_runs.size() - 1)
				{
					std::swap(selected_runs[index], selected_runs[index + 1]);
					std::swap(plot_metric_indexes[index], plot_metric_indexes[index + 1]);
					std::swap(plot_display_nums[index], plot_display_nums[index + 1]);
				}
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("Delete         "))
			{
				selected_runs.erase(selected_runs.begin() + index);
				plot_metric_indexes.erase(plot_metric_indexes.begin() + index);
				plot_display_nums.erase(plot_display_nums.begin() + index);

				is_delete = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	
	void TestPanel::ConstructAndSendPlotCMD()
	{
		PlotManager::Instance()->ClosePlotPipe();
		PlotManager::Instance()->OpenPlotPipe();

		int graph_width = 400;
		int graph_height = 400;
		int graph_per_row = 3;
		int plot_graph = selected_runs.size();
		if (plot_graph > 0)
		{
			int row = plot_graph / graph_per_row;
			if (plot_graph % graph_per_row) row++;
			int width = graph_per_row * graph_width;
			int height = row * graph_height;

			char canvas_size_cmd[256];
			char multiplot_set[256];
			sprintf(canvas_size_cmd, "set term %s size %d,%d\n", GNUPLOT_TERMINAL, width, height);
			sprintf(multiplot_set, "set multiplot layout %d,%d\n", row, graph_per_row);
			PlotManager::Instance()->Send(canvas_size_cmd);
			PlotManager::Instance()->Send(multiplot_set);

			for (int i = 0; i < plot_graph; i++)
			{
				char current_cmd[1024];
				int plot_metric_index = plot_metric_indexes[i];
				if (plot_metrics[plot_metric_index] == std::string("Population"))
					ConstructPopulationPlotCMD(current_cmd, i);
				else 
					ConstructMetricPlotCMD(current_cmd, i, plot_metrics[plot_metric_index], plot_display_nums[i]);

				PlotManager::Instance()->Send(current_cmd);
			}

			sprintf(multiplot_set, "unset multiplot\n");
			PlotManager::Instance()->Send(multiplot_set);
		}
	}



	void TestPanel::ConstructPopulationPlotCMD(char* cmd, int selected_run_intex)
	{
		int avail_run_index = selected_runs[selected_run_intex];
		EMOCSingleThreadResult &res = EMOCManager::Instance()->GetSingleThreadResult(avail_run_index);
		int last_generation = res.max_iteration;
		char data_file[1024];
		// TODO: the file position needs to be changed
		sprintf(data_file, "./output/test_module/run%d/pop_%d.txt", avail_run_index, last_generation); 
		
		int obj_num = res.para.objective_num;
		if (obj_num == 2)
		{
			sprintf(cmd,
				"set grid\n"
				"set autoscale\n"
				"set title '%s'\n"
				"set xlabel 'f1'\n"
				"set ylabel 'f2'\n"
				"unset key\n"
				"plot '%s' w p pt 6\n"
				,res.description.c_str(), data_file);
		}
		else if (obj_num == 3)
		{
			sprintf(cmd,
				"set grid\n"
				"set autoscale\n"
				"set title '%s'\n"
				"set xlabel 'f1'\n"
				"set ylabel 'f2'\n"
				"set zlabel 'f3'\n"
				"set ticslevel 0.0\n"
				"set view 45,45\n"
				"unset key\n"
				"splot  '%s' w p pt 6\n"
				, res.description.c_str(), data_file);
		}
		
	}

	void TestPanel::ConstructMetricPlotCMD(char* cmd, int selected_run_intex, const std::string& metric_name, int display_num)
	{
		std::vector<int> generations;

		int avail_run_index = selected_runs[selected_run_intex];
		EMOCSingleThreadResult &res = EMOCManager::Instance()->GetSingleThreadResult(avail_run_index);
		int last_generation = res.max_iteration;
		int interval = (last_generation) / (display_num - 1);
		int remain = (last_generation) % (display_num - 1);

		int generation = 0;
		if (display_num <= last_generation + 1)
		{
			for (int i = 0; i < display_num; i++)
			{
				if (i == 1) generation += remain;
				generations.push_back(generation);
				generation += interval;
			}
		}
		else
			for (int i = 0; i <= last_generation; i++)
				generations.push_back(i);
		

		FILE* data_file = nullptr;
		char data_file_name[256];
		sprintf(data_file_name, "./plotfile/plot_metric%d.txt", selected_run_intex);
		data_file = fopen(data_file_name, "w");		
		if (!data_file)
		{
			std::cerr << "<Error!!!> Could not open plot data file" << std::endl;
			exit(-1);
		}

		// write data
		int calculations = last_generation + 1 >= display_num ? display_num : last_generation + 1;
		for (int i = 0; i < calculations; i++)
		{
			char pop_data_file[1024];
			sprintf(pop_data_file, "./output/test_module/run%d/pop_%d.txt", avail_run_index, generations[i]);
			std::vector<std::vector<double>> pop = ReadPop(pop_data_file, res.para.objective_num);

			double igd_vale = 0.0;
			if (metric_name == "IGD")
			{
				if (res.igd_history.find(generations[i]) != res.igd_history.end())
					igd_vale = res.igd_history[generations[i]];
				else
				{
					igd_vale = CalculateIGD(pop, pop.size(), res.para.objective_num, res.para.problem_name);
					res.igd_history[generations[i]] = igd_vale;
				}
			}
			else if (metric_name == "HV")
			{
			}
			else
			{
			}
			fprintf(data_file, "%d\t%f\n", generations[i], igd_vale);
		}
		fflush(data_file);
		fclose(data_file);

		sprintf(cmd,
			"set grid\n"
			"set autoscale\n"
			"set title '%s'\n"
			"set xlabel 'Generation'\n"
			"set ylabel '%s'\n"
			"unset key\n"
			"plot '%s' w lp lc 3 lw 2 pt 2 ps 0.7\n"
			,res.description.c_str(), metric_name.c_str(), data_file_name);
	}

}
