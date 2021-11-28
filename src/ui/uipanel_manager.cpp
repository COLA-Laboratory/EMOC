#include "ui/uipanel_manager.h"

#include <iostream>
#include <thread>

#include "emoc_app.h"
#include "core/file.h"
#include "core/emoc_manager.h"
#include "ui/imgui_impl_glfw.h"
#include "ui/imgui_impl_opengl3.h"
#include "imgui_internal.h"

namespace emoc {

	UIPanelManager::Garbo UIPanelManager::garbo_;
	UIPanelManager* UIPanelManager::s_Instance = nullptr;
	std::mutex UIPanelManager::singleton_mutex_;

	// Center Help Function
	static void TextCenter(std::string text) 
	{
		float window_width = ImGui::GetWindowSize().x;
		float text_width = ImGui::CalcTextSize(text.c_str()).x;
		ImGui::SetCursorPosX(window_width / 2 - text_width / 2);
		ImGui::Text(text.c_str());
	}

	// Tooltip Help Function
	static void HelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}


	emoc::UIPanelManager* UIPanelManager::Instance()
	{
		if (s_Instance == nullptr)
		{
			std::lock_guard<std::mutex> locker(singleton_mutex_);
			if (s_Instance == nullptr)
			{
				s_Instance = new UIPanelManager();
			}
		}

		return s_Instance;
	}

	void UIPanelManager::Init(int width, int height, const std::string &title)
	{
		width_ = width;
		height_ = height;
		title_ = title;

		InitDisplayList();
		InitAlgorithmList();
		InitProlbemList();
		InitGLFW(width, height, title);
		InitImGui();
	}

	void UIPanelManager::RenderPanel()
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		switch (panel_state_)
		{
		case UIPanel::TestPanel:
			RenderTestPanel();
			break;
		case UIPanel::ExperimentPanel:
			RenderExperimentPanel();
			break;
		case UIPanel::AnalysePanel:
			RenderAnalysePanel();
			break;
		default:
			break;
		}
		
		// Rendering and Setting basical parameters
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window_, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void UIPanelManager::Update()
	{
		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(window_);
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();
	}

	UIPanelManager::UIPanelManager():
		width_(1600),
		height_(900),
		title_("EMOC"),
		glsl_version_("#version 130"),
		window_(nullptr),
		panel_state_(UIPanel::ExperimentPanel),
		algorithm_index(0),
		problem_index(0),
		display_index(0),
		current_evaluation(0),
		max_evaluation(100000)
	{

	}

	void UIPanelManager::InitGLFW(int width, int height, const std::string& title)
	{
		// Setup window
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit())
		{
			std::cout << "ERROR: glfw init false!\n";
			exit(-1);
		}

		// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
		// GL ES 2.0 + GLSL 100
		glsl_version_ = "#version 100";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
		// GL 3.2 + GLSL 150
		glsl_version_ = "#version 150";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
		// GL 3.0 + GLSL 130
		glsl_version_ = "#version 130";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

		// Create window with graphics context
		window_ = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
		if (window_ == NULL)
		{
			std::cout << "ERROR: glfw create window false!\n";
			exit(-1);
		}
		glfwMakeContextCurrent(window_);
		glfwSwapInterval(1); // Enable vsync
	}

	void UIPanelManager::InitImGui()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		//(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		EmbraceTheClassic();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(window_, true);
		ImGui_ImplOpenGL3_Init(glsl_version_.c_str());

		// Load Fonts
		// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
		// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
		// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
		// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
		// - Read 'docs/FONTS.md' for more instructions and details.
		// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
		//io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF("./fonts/Roboto-Medium.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("./fonts/Cousine-Regular.ttf", 15.0f);
		io.Fonts->AddFontFromFileTTF("./resource/fonts/DroidSans.ttf", 22.0f);
		//io.Fonts->AddFontFromFileTTF("./fonts/ProggyTiny.ttf", 10.0f);
		//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
		//IM_ASSERT(font != NULL);
	}

	void UIPanelManager::RenderTestPanel()
	{
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
			if (!EMOCManager::Instance()->GetFinish()) ImGui::BeginDisabled();
			if (ImGui::Button("Start##Test", ImVec2(window_width * 0.95f, window_height * 0.1f)))
			{
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
			if (!EMOCManager::Instance()->GetFinish()) ImGui::EndDisabled();

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
			if (EMOCManager::Instance()->GetFinish() ||
				(!EMOCManager::Instance()->GetFinish() && !EMOCManager::Instance()->GetPause())) ImGui::BeginDisabled();
			if (ImGui::Button("Continue##Test", ImVec2(100, 60)))
			{
				std::cout << "Continue!\n";
				std::lock_guard<std::mutex> locker(EMOCLock::pause_mutex);
				EMOCManager::Instance()->SetPause(false);
				std::cout << "After click continue button, the pause value is: " << EMOCManager::Instance()->GetPause() << "\n";
				EMOCLock::pause_cond.notify_all();
			}
			ImGui::SameLine(); ImGui::Dummy(ImVec2(10.0f, 0.0f)); ImGui::SameLine();
			if (EMOCManager::Instance()->GetFinish() ||
				(!EMOCManager::Instance()->GetFinish() && !EMOCManager::Instance()->GetPause())) ImGui::EndDisabled();

			// When algorithm (is not running) or (is running but paused), we diable 'Pause' and 'Stop' button.
			if (EMOCManager::Instance()->GetFinish() ||
				(!EMOCManager::Instance()->GetFinish() && EMOCManager::Instance()->GetPause())) ImGui::BeginDisabled();
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
			ImGui::SameLine(); ImGui::Dummy(ImVec2(10.0f, 0.0f)); ImGui::SameLine();
			if (EMOCManager::Instance()->GetFinish() ||
				(!EMOCManager::Instance()->GetFinish() && EMOCManager::Instance()->GetPause())) ImGui::EndDisabled();

			ImGui::End();
		}

		// Test Moduel Result Window
		{
			ImGui::Begin("Result Infomation##Test");
			const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
			const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
			static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY  |  ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;

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
			if(ImGui::Button("Open Plot Window##Test", ImVec2(250.0f, 40.0f)))
			{
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
			ImGui::Checkbox("Another Window", &show_another_window);

			//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			//ImGui::ColorEdit3("clear color", testArray); // Edit 3 floats representing a color

			//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			//	counter++;
			//ImGui::SameLine();
			//ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			
			if (ImGui::Button("test", ImVec2(100, 60)))
			{
				panel_state_ = UIPanel::ExperimentPanel;
			}
			ImGui::End();
		}

	}

	void UIPanelManager::RenderExperimentPanel()
	{
		// normal way to set table columns
		//ImGui::TableSetupColumn("Run #", ImGuiTableColumnFlags_None);
		//ImGui::TableSetupColumn("Algorithm", ImGuiTableColumnFlags_None);
		//ImGui::TableSetupColumn("Problem", ImGuiTableColumnFlags_None);
		//ImGui::TableSetupColumn("N", ImGuiTableColumnFlags_None);
		//ImGui::TableSetupColumn("M", ImGuiTableColumnFlags_None);
		//ImGui::TableSetupColumn("D", ImGuiTableColumnFlags_None);
		//ImGui::TableSetupColumn("Evaluation", ImGuiTableColumnFlags_None);
		//ImGui::TableSetupColumn("Runtime", ImGuiTableColumnFlags_None);
		//ImGui::TableHeadersRow();

		// Experiment Module Algorithm and Problem Selection Window
		{
			ImGui::Begin("Algorithm and Problem Selection##Experiment");

			// get some basic window property
			float window_width = ImGui::GetWindowSize().x;
			float window_height = ImGui::GetWindowSize().y;
			const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
			int list_height = (int)(0.28f * window_height / TEXT_BASE_HEIGHT);		// adaptive list height in number of items
			list_height = list_height > 3 ? list_height : 3;						// minimal height of list

			// Algorithm selection part
			TextCenter("Algorithm Selection");
			ImGui::Dummy(ImVec2(0.0f, 2.0f));
			ImGui::SetNextItemWidth(-FLT_MIN);
			bool is_value_changed = ImGui::ListBox("##AlgorithmExperimentListbox", &algorithm_index, algorithm_names.data(), algorithm_names.size(), list_height);
			if (is_value_changed && selected_algorithm_map.count(algorithm_names[algorithm_index]) == 0)
			{
				selected_algorithm_map[algorithm_names[algorithm_index]] = 1;
				selected_algorithms.push_back(algorithm_names[algorithm_index]);
			}
			ImGui::Dummy(ImVec2(0.0f, 20.0f));


			// Problem selection part
			ImGui::Separator();
			TextCenter("Problem Selection");
			ImGui::Dummy(ImVec2(0.0f, 2.0f));
			ImGui::SetNextItemWidth(-FLT_MIN);
			is_value_changed = ImGui::ListBox("##ProblemExperimentListbox", &problem_index, problem_names.data(), problem_names.size(), list_height);
			if (is_value_changed && selected_problem_map.count(problem_names[problem_index]) == 0)
			{
				selected_problem_map[problem_names[problem_index]] = 1;
				selected_problems.push_back(problem_names[problem_index]);

				// add some default problem settings
				Ns.push_back(100);
				Ms.push_back(2);
				Ds.push_back(30);
				Evaluations.push_back(25000);
			}
			ImGui::Dummy(ImVec2(0.0f, 20.0f));

			// set number of run and save interval inputs position
			float current_posY = ImGui::GetCursorPosY();
			float max_text_width = ImGui::CalcTextSize("Number of runs").x;
			float remain_width = window_width - max_text_width;
			float input_pos = max_text_width + 0.28f * remain_width;
			float height_pos = (window_height * 0.902f - TEXT_BASE_HEIGHT * 3.0f);
			height_pos = height_pos > current_posY ? height_pos : current_posY + 10.0f;

			ImGui::SetCursorPosY(height_pos);
			ImGui::PushItemWidth(0.71f * remain_width);
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

			// When algorithm is running, we diable 'Start' button.
			if (!EMOCManager::Instance()->GetFinish()) ImGui::BeginDisabled();
			if (ImGui::Button("Start##Experiment", ImVec2(window_width * 0.95f, window_height * 0.08f)))
			{
				std::cout << Ns.size() << "\n";
			}
			if (!EMOCManager::Instance()->GetFinish()) ImGui::EndDisabled();



			ImGui::End();
		}

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
			{
				DisplaySelectedProblem(i, item_width, item_pos);
			}
			ImGui::End();
		}

		// Experiment Module Result Infomation Window
		{
			ImGui::Begin("Result Infomation##Experiment");
			const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
			const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
			static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;

			std::vector<std::string> columns;
			TextCenter("Result Info Display");
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			static bool is_displayN = true; ImGui::Checkbox("N", &is_displayN); ImGui::SameLine();
			static bool is_displayM = true; ImGui::Checkbox("M", &is_displayM); ImGui::SameLine();
			static bool is_displayD = true; ImGui::Checkbox("D", &is_displayD); ImGui::SameLine();
			static bool is_displayEvaluation = true; ImGui::Checkbox("Evaluation", &is_displayEvaluation);

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
			if (ImGui::BeginTable("ResultTable", columns.size(), flags, outer_size))
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
						ImGui::Combo(("##Display" + std::to_string(c)).c_str(), &display_index, display_names.data(), display_names.size());
						ImGui::PopStyleColor();
						ImGui::PopStyleVar();
						//ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
						//ImGui::TableHeader("##DisplayHeader");
					}
					else
						ImGui::TableHeader(column_name);

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
			if (ImGui::Button("Open Plot Window", ImVec2(250.0f, 40.0f)))
			{
				panel_state_ = UIPanel::TestPanel;
			}
			ImGui::End();
		}
	}

	void UIPanelManager::RenderAnalysePanel()
	{

	}

	UIPanelManager::~UIPanelManager(){}


	void UIPanelManager::EmbraceTheClassic()
	{
		ImGui::GetStyle().FrameRounding = 4.0f;
		ImGui::GetStyle().GrabRounding = 4.0f;

		ImVec4* colors = ImGui::GetStyle().Colors;
		//ImGui::GetStyle().FrameBorderSize = 1.0f;
		colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.18f, 0.18f, 0.32f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.07f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.00f, 0.00f, 0.00f, 0.07f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 0.58f, 0.94f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 0.69f, 0.89f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	}

	void UIPanelManager::EmbraceTheDarkness()
	{
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);

		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(8.00f, 8.00f);
		style.FramePadding = ImVec2(5.00f, 2.00f);
		style.CellPadding = ImVec2(6.00f, 6.00f);
		style.ItemSpacing = ImVec2(6.00f, 6.00f);
		style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
		style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
		style.IndentSpacing = 25;
		style.ScrollbarSize = 15;
		style.GrabMinSize = 10;
		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 1;
		style.FrameBorderSize = 1;
		style.TabBorderSize = 1;
		style.WindowRounding = 7;
		style.ChildRounding = 4;
		style.FrameRounding = 3;
		style.PopupRounding = 4;
		style.ScrollbarRounding = 9;
		style.GrabRounding = 3;
		style.LogSliderDeadzone = 4;
		style.TabRounding = 4;
	}

	void UIPanelManager::InitAlgorithmList()
	{
		algorithm_names.push_back("MOEAD");
		algorithm_names.push_back("MOEADDE");
		algorithm_names.push_back("MOEADDRA");
		algorithm_names.push_back("MOEADFRRMAB");
		algorithm_names.push_back("MOEADGRA");
		algorithm_names.push_back("MOEADIRA");
		algorithm_names.push_back("NSGA2");
		algorithm_names.push_back("SPEA2");
		algorithm_names.push_back("IBEA");
		algorithm_names.push_back("ENSMOEAD");
		algorithm_names.push_back("HYPE");
		algorithm_names.push_back("SMSEMOA");
	}

	void UIPanelManager::InitProlbemList()
	{
		problem_names.push_back("DTLZ1");
		problem_names.push_back("DTLZ2");
		problem_names.push_back("DTLZ3");
		problem_names.push_back("DTLZ4");
		problem_names.push_back("DTLZ5");
		problem_names.push_back("DTLZ6");
		problem_names.push_back("DTLZ7");
		problem_names.push_back("UF1");
		problem_names.push_back("UF2");
		problem_names.push_back("UF3");
		problem_names.push_back("UF4");
		problem_names.push_back("UF5");
		problem_names.push_back("UF6");
		problem_names.push_back("UF7");
		problem_names.push_back("UF8");
		problem_names.push_back("UF9");
		problem_names.push_back("UF10");
		problem_names.push_back("ZDT1");
		problem_names.push_back("ZDT2");
		problem_names.push_back("ZDT3");
		problem_names.push_back("ZDT4");
		problem_names.push_back("ZDT6");
	}

	void UIPanelManager::InitDisplayList()
	{
		display_names.push_back("Runtime");
		display_names.push_back("IGD");
		display_names.push_back("HV");
	}

	void UIPanelManager::DisplayAccordingToColumn(const EMOCSingleThreadResult& res, const std::string& col_name, int row)
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

	void UIPanelManager::DisplayAlgorithmParameters(const std::string& algorithm)
	{
		// TODO... according the algorithm name to display different parameter settings
		//if(algorithm == "MOEAD")
		//else if()
	}

	void UIPanelManager::DisplaySelectedAlgorithm(int index)
	{
		const std::string &algorithm = selected_algorithms[index];
		std::string header_name = algorithm + "##" + std::to_string(index);
		bool is_open = false;
		if (is_open = ImGui::CollapsingHeader(header_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			DisplayMovePopup(index, true);
			DisplayAlgorithmParameters(algorithm);
		}
		if (!is_open) DisplayMovePopup(index, true);
	}

	void UIPanelManager::DisplaySelectedProblem(int index, int item_width, int item_pos)
	{
		char name[256];
		const std::string& problem = selected_problems[index];
		std::string header_name = problem + "##" + std::to_string(index);
		bool is_open = false;
		if (is_open = ImGui::CollapsingHeader(header_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			DisplayMovePopup(index, false);
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
		if (!is_open) DisplayMovePopup(index, false);
	}

	void UIPanelManager::DisplayMovePopup(int index, bool is_algorithm_popup)
	{
		if (ImGui::BeginPopupContextItem())
		{
			ImGui::Button("Move Up     ");
			ImGui::Button("Move Down");
			ImGui::Button("Delete          ");

			ImGui::EndPopup();
		}
	}

}