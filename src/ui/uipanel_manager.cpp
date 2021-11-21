#include "ui/uipanel_manager.h"

#include <iostream>
#include <thread>

#include "core/file.h"
#include "core/emoc_manager.h"
#include "ui/imgui_impl_glfw.h"
#include "ui/imgui_impl_opengl3.h"

#include "emoc_app.h"

namespace emoc {

	UIPanelManager::Garbo UIPanelManager::garbo_;
	UIPanelManager* UIPanelManager::s_Instance = nullptr;
	std::mutex UIPanelManager::singleton_mutex_;


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
		panel_state_(UIPanel::TestPanel)
	{}

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

		// this is a simple try for design the ui for EMOC
		{
			ImGui::Begin("Algorithm and Problem Selection");
			std::vector<char*> test = { "MOEAD", "NSGA2", "IBEA", "MOEADDE" };
			//char* algorithmName[] = { "MOEAD", "NSGA2", "IBEA", "MOEADDE", };
			
			const char* problemName[] = { "ZDT1", "ZDT2", "ZDT3", "ZDT4", "DTLZ1"};
			static int algorithmIndex = 0; // If the selection isn't within 0..count, Combo won't display a preview
			static int problemIndex = 0; // If the selection isn't within 0..count, Combo won't display a preview

			//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 100.0f);
			ImGui::Text("Algorithm");
			ImGui::Spacing();
			ImGui::Combo("##Algorithm", &algorithmIndex, test.data(), test.size());

			//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 100.0f);
			ImGui::Text("Problem");
			ImGui::Spacing();
			ImGui::Combo("##Problem", &problemIndex, problemName, IM_ARRAYSIZE(problemName));

			if (ImGui::Button("Start", ImVec2(100, 100)))
			{
				std::cout << test[algorithmIndex] << "\n"
					<< problemName[problemIndex] << "\n"
					<< "population number: " << N << "\n"
					<< "obj dim: " << M << "\n"
					<< "dec dim: " << D << "\n"
					<< "evaluation number: " << Evaluation << "\n";
				// multithread running
				EMOCParameters para;
				para.algorithm_name = test[algorithmIndex];
				para.problem_name = problemName[problemIndex];
				para.is_plotting = true;
				para.objective_num = M;
				para.decision_num = D;
				para.population_num = N;
				para.max_evaluation = Evaluation;
				para.output_interval = 100000;
				para.runs_num = 1;

				EMOCManager::Instance()->SetTaskParameters(para);
				std::thread algorithm_thread(&EMOCManager::Run, EMOCManager::Instance());
				algorithm_thread.detach();

				//pthread_t thread1;
				//pthread_create(&thread1, nullptr, TestWork, (void*)para);
			}

			if (ImGui::Button("Stop", ImVec2(100, 100)))
			{
				std::cout << "Stop!\n";
				std::unique_lock<std::mutex> locker(finish_mutex);
				EMOCManager::Instance()->GetGlobalSetting(0)->SetFinish(true);
			}

			if (ImGui::Button("Continue", ImVec2(100, 100)))
			{
				std::cout << "Continue!\n";
				std::unique_lock<std::mutex> locker(pause_mutex);
				EMOCManager::Instance()->GetGlobalSetting(0)->SetPause(false);
				cond.notify_one();
			}

			if (ImGui::Button("Pause", ImVec2(100, 100)))
			{
				std::cout << "Pause\n";
				std::unique_lock<std::mutex> locker(pause_mutex);
				EMOCManager::Instance()->GetGlobalSetting(0)->SetPause(true);
			}
			ImGui::End();
		}

		// this is a simple try for design the ui for EMOC
		{
			ImGui::Begin("Parameter Setting");

			ImGui::AlignTextToFramePadding();
			ImGui::Text("D"); ImGui::SameLine();
			ImGui::InputInt("##D", &D, 0);

			ImGui::AlignTextToFramePadding();
			ImGui::Text("M"); ImGui::SameLine();
			ImGui::InputInt("##M", &M, 0);

			ImGui::AlignTextToFramePadding();
			ImGui::Text("N"); ImGui::SameLine();
			ImGui::InputInt("##N", &N, 0);

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Evaluation"); ImGui::SameLine();
			ImGui::InputInt("##Evaluation", &Evaluation, 0);

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

			ImGui::Begin("Result");                          // Create a window called "Hello, world!" and append into it.



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
			ImGui::End();
		}

	}

	void UIPanelManager::RenderExperimentPanel()
	{

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
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	}


}