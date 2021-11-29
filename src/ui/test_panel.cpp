#include "ui/test_panel.h"

#include <iostream>

#include "emoc_app.h"
#include "core/emoc_manager.h"
#include "ui/uipanel_manager.h"
#include "ui/ui_utility.h"

namespace emoc {

	TestPanel::TestPanel():
		algorithm_index(0),
		problem_index(0),
		display_index(0),
		current_evaluation(0),
		max_evaluation(1000000)
	{
		InitAlgorithmList(algorithm_names);
		InitProlbemList(problem_names);
		InitDisplayList(display_names);
	}

	TestPanel::~TestPanel()
	{

	}

	void TestPanel::Render()
	{
		// update EMOC running state
		bool is_finish = EMOCManager::Instance()->GetFinish();
		bool is_pause = EMOCManager::Instance()->GetPause();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// Test Module Parameter Setting Window
		{
			// Algorithm selection part
			ImGui::Begin("EMOC Parameter Setting##Test");
			TextCenter("Algorithm Selection");
			ImGui::Dummy(ImVec2(0.0f, 2.0f));
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::Combo("##AlgorithmTestCombo", &algorithm_index, algorithm_names.data(), algorithm_names.size());
			ImGui::Dummy(ImVec2(0.0f, 20.0f));
			ImGui::Separator();

			// Problem selection part
			TextCenter("Problem Selection");
			ImGui::Dummy(ImVec2(0.0f, 2.0f));
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::Combo("##ProblemTestCombo", &problem_index, problem_names.data(), problem_names.size());

			float window_width = ImGui::GetWindowSize().x;
			float window_height = ImGui::GetWindowSize().y;
			float max_text_width = ImGui::CalcTextSize("Evaluation").x;
			float remain_width = window_width - max_text_width;
			float input_pos = max_text_width + 0.18f * remain_width;
			ImGui::PushItemWidth(0.81f * remain_width);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("D"); ImGui::SameLine();
			ImGui::SetCursorPosX(input_pos);
			ImGui::InputInt("##DTest", &D, 0);

			ImGui::AlignTextToFramePadding();
			ImGui::Text("M"); ImGui::SameLine();
			ImGui::SetCursorPosX(input_pos);
			ImGui::InputInt("##MTest", &M, 0);

			ImGui::AlignTextToFramePadding();
			ImGui::Text("N"); ImGui::SameLine();
			ImGui::SetCursorPosX(input_pos);
			ImGui::InputInt("##NTest", &N, 0);

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Evaluation"); ImGui::SameLine();
			ImGui::SetCursorPosX(input_pos);
			ImGui::InputInt("##EvaluationTest", &Evaluation, 0);
			ImGui::PopItemWidth();
			ImGui::Dummy(ImVec2(0.0f, 20.0f));
			ImGui::Separator();


			// Plot setting part
			static int plot_size[2] = { 500,500 };
			static int plot_position[2] = { 400, 800 };
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
			if (!is_finish) ImGui::BeginDisabled();
			if (ImGui::Button("Start##Test", ImVec2(window_width * 0.95f, window_height * 0.1f)))
			{
				EMOCManager::Instance()->SetIsExperiment(false);

				std::cout << algorithm_names[algorithm_index] << "\n"
					<< problem_names[problem_index] << "\n"
					<< "population number: " << N << "\n"
					<< "obj dim: " << M << "\n"
					<< "dec dim: " << D << "\n"
					<< "evaluation number: " << Evaluation << "\n";

				current_evaluation = 0;
				max_evaluation = Evaluation;

				EMOCParameters para;
				para.algorithm_name = algorithm_names[algorithm_index];
				para.problem_name = problem_names[problem_index];
				para.is_plot = true;
				para.objective_num = M;
				para.decision_num = D;
				para.population_num = N;
				para.max_evaluation = Evaluation;
				para.output_interval = 100000;
				para.runs_num = 1;

				EMOCManager::Instance()->SetTaskParameters(para);
				std::thread algorithm_thread(&EMOCManager::Run, EMOCManager::Instance());
				algorithm_thread.detach();
			}
			if (!is_finish) ImGui::EndDisabled();

			ImGui::End();
		}

		// Test Moduel Control Window
		{
			ImGui::Begin("Control##Test");
			float window_width = ImGui::GetWindowSize().x;
			float window_height = ImGui::GetWindowSize().y;
			float text_width = ImGui::CalcTextSize("1000000 evaluations").x;
			float remain_width = window_width - text_width;
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
				std::cout << "Continue!\n";
				std::lock_guard<std::mutex> locker(EMOCLock::pause_mutex);
				EMOCManager::Instance()->SetPause(false);
				std::cout << "After click continue button, the pause value is: " << EMOCManager::Instance()->GetPause() << "\n";
				EMOCLock::pause_cond.notify_all();
			}
			ImGui::SameLine(); ImGui::Dummy(ImVec2(10.0f, 0.0f)); ImGui::SameLine();
			if (is_finish || (!is_finish && !is_pause)) ImGui::EndDisabled();

			// When algorithm (is not running) or (is running but paused), we diable 'Pause' and 'Stop' button.
			if (is_finish || (!is_finish && is_pause)) ImGui::BeginDisabled();
			if (ImGui::Button("Pause##Test", ImVec2(100, 60)))
			{
				std::cout << "Pause\n";
				std::lock_guard<std::mutex> locker(EMOCLock::pause_mutex);
				EMOCManager::Instance()->SetPause(true);
			}
			ImGui::SameLine(); ImGui::Dummy(ImVec2(10.0f, 0.0f)); ImGui::SameLine();

			if (ImGui::Button("Stop##Test", ImVec2(100, 60)))
			{
				std::cout << "Stop!\n";
				std::lock_guard<std::mutex> locker(EMOCLock::finish_mutex);
				EMOCManager::Instance()->SetFinish(true);
				std::cout << "After click continue button, the finish value is: " << EMOCManager::Instance()->GetFinish() << "\n";
			}
			ImGui::SameLine(); ImGui::Dummy(ImVec2(10.0f, 0.0f)); ImGui::SameLine();\
			if (is_finish || (!is_finish && is_pause)) ImGui::EndDisabled();

			ImGui::End();
		}

		// Test Moduel Result Window
		{
			ImGui::Begin("Result Infomation##Test");
			const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
			const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
			static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;

			std::vector<std::string> columns;
			TextCenter("Result Info Display");
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			static bool is_displayN = true; ImGui::Checkbox("N##Test", &is_displayN); ImGui::SameLine();
			static bool is_displayM = true; ImGui::Checkbox("M##Test", &is_displayM); ImGui::SameLine();
			static bool is_displayD = true; ImGui::Checkbox("D##Test", &is_displayD); ImGui::SameLine();
			static bool is_displayEvaluation = true; ImGui::Checkbox("Evaluation##Test", &is_displayEvaluation);

			// set this frame's columns
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
					//ImGui::PushID(column);

					// use combobox
					if (columns[c] == "Runtime")
					{
						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
						ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.18f, 0.18f, 0.32f, 1.00f));
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						ImGui::Combo(("##DisplayTest" + std::to_string(c)).c_str(), &display_index, display_names.data(), display_names.size());
						ImGui::PopStyleColor();
						ImGui::PopStyleVar();
						//ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
						//ImGui::TableHeader("##DisplayHeader");
					}
					else
						ImGui::TableHeader((column_name + std::string("##Test")).c_str());

					//ImGui::PopID();
				}
				columns[columns.size() - 1] = display_names[display_index];

				int rows = EMOCManager::Instance()->GetSingleThreadResultSize();
				for (int row = 0; row < rows; row++)
				{
					ImGui::TableNextRow();
					const EMOCSingleThreadResult& res = EMOCManager::Instance()->GetSingleThreadResult(row);
					for (int c = 0; c < columns.size(); c++)
					{
						ImGui::TableSetColumnIndex(c);
						DisplayAccordingToColumn(res, columns[c], row);
					}
				}

				ImGui::EndTable();
			}
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 260.0f);
			if (ImGui::Button("Open Plot Window##Test", ImVec2(250.0f, 40.0f)))
			{
				UIPanelManager::Instance()->SetUIPanelState(UIPanel::ExperimentPanel);
			}
			ImGui::End();


		}

		//ImGui::ShowMetricsWindow();
		// 
		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static char text[1024 * 16] = " ";
			//std::cout << "text size:" << IM_ARRAYSIZE(text) << "text str lenght" << strlen(text) << "\n";

			const char* displayInfo[] = { "Runtime", "IGD", "HV", "Spacing" };
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Result##Test");                          // Create a window called "Hello, world!" and append into it.



			ImGui::Text("Result Information:");

			static int displayIndex = 0; // If the selection isn't within 0..count, Combo won't display a preview

			//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 100.0f);
			ImGui::Spacing();
			ImGui::Combo("##Display", &displayIndex, displayInfo, IM_ARRAYSIZE(displayInfo));
			if (EMOCManager::Instance()->GetSingleThreadResultSize() > 0)
			{
				EMOCSingleThreadResult result = EMOCManager::Instance()->GetSingleThreadResult(EMOCManager::Instance()->GetSingleThreadResultSize() - 1);

				std::string algorithm = result.para.algorithm_name;
				std::string problem = result.para.problem_name;
				int M = result.para.objective_num;
				int D = result.para.decision_num;
				int N = result.para.population_num;
				int max_evaluation = result.para.max_evaluation;
				double runtime = result.runtime;
				double igd = result.last_igd;
				double hv = result.last_hv;

				static std::string extra_info;
				if (displayIndex == 0) extra_info = "Runtime: " + std::to_string(runtime) + "s";
				else if (displayIndex == 1) extra_info = "IGD: " + std::to_string(igd);
				else if (displayIndex == 2) extra_info = "HV: " + std::to_string(hv);

				sprintf(text,
					"Algorithm: %s\n\n"
					"Problem: %s\n"
					"M: %d\n"
					"D: %d\n"
					"N: %d\n"
					"Evaluation: %d\n\n"
					"%s"
					, algorithm.c_str(), problem.c_str(), M, D, N, max_evaluation, extra_info.c_str());
			}





			static ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly;
			//ImGui::CheckboxFlags("ImGuiInputTextFlags_ReadOnly", &flags, ImGuiInputTextFlags_ReadOnly);
			//ImGui::CheckboxFlags("ImGuiInputTextFlags_AllowTabInput", &flags, ImGuiInputTextFlags_AllowTabInput);
			//ImGui::CheckboxFlags("ImGuiInputTextFlags_CtrlEnterForNewLine", &flags, ImGuiInputTextFlags_CtrlEnterForNewLine);
			ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 10), flags);


			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
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
			ImGui::Text(std::to_string(res.last_igd).c_str());
		else if (col_name == "HV")
			ImGui::Text(std::to_string(res.last_hv).c_str());

	}

}