//#include <ctime>
//#include <cstdio>
//#include <pthread.h>
//#include <memory>
//#include <vector>
//#include <string>
//#include <iostream>
//#include <algorithm>
//
//#include "core/file.h"
//#include "core/global.h"
//#include "core/individual.h"
//#include "problem/zdt.h"
//#include "problem/dtlz.h"
//#include "algorithms/moead/moead.h"
//#include "algorithms/moead_de/moead_de.h"
//#include "algorithms/nsga2/nsga2.h" 
//#include "algorithms/ibea/ibea.h"
//#include "algorithms/spea2/spea2.h"
//#include "algorithms/smsemoa/smsemoa.h"
//#include "algorithms/moead_dra/moead_dra.h"
//#include "algorithms/moead_frrmab/moead_frrmab.h"
//#include "algorithms/hype/hype.h"
//#include "metric/hv.h"
//#include "metric/igd.h"
//#include "metric/gd.h"
//#include "metric/spacing.h"
//#include "random/random.h"
// 
//#if defined(__linux) || defined(linux)
//#include <sys/time.h>
//#endif
//
//#define THREAD_NUM 8
//using emoc::g_GlobalSettingsArray;
//using emoc::EMOCParameters;
//
//
//struct ThreadParamters
//{
//	EMOCParameters *para;
//
//	int run_start;
//	int run_end;
//	int thread_id;
//};
//
//void *Work(void *args);
//void EMOCMultiThreadTest(EMOCParameters *parameter);
//void EMOCSingleThreadTest(EMOCParameters *parameter, double *runtime);
//
//int main(int argc, char* argv[])
//{	
//	// initilize some bases for random number
//	randomize();
//
//	// initialize parameters
//	emoc::EMOCParameters *parameter = new emoc::EMOCParameters(); // 这里只能用new，因为结构体里面有string，如果malloc的话不会做构造，所以malloc出来string是未初始化的，不能正确使用
//	ParseParamerters(argc, argv, parameter);
//	//ReadParametersFromFile("src/config/config.txt", parameter);
//	parameter->igd_value = (double *)malloc(sizeof(double) * parameter->runs_num);
//	double *runtime = (double *)malloc(sizeof(double) * parameter->runs_num);
//
//	std::cout << "current task:" << std::endl;
//	std::cout << "-------------------------------------------" << std::endl;
//	std::cout << "problem:              " << parameter->problem_name << std::endl;
//	std::cout << "algorithm:            " << parameter->algorithm_name << std::endl;
//	std::cout << "population number:    " << parameter->population_num << std::endl;
//	std::cout << "decision number:      " << parameter->decision_num << std::endl;
//	std::cout << "objective number:     " << parameter->objective_num << std::endl;
//	std::cout << "evaluation:           " << parameter->max_evaluation << std::endl;
//	std::cout << "runs:                 " << parameter->runs_num << std::endl;
//	std::cout << "is open multithread:  " << parameter->is_open_multithread << std::endl;
//	std::cout << "multithread number:   " << parameter->thread_num << std::endl;
//	std::cout << "-------------------------------------------\n" << std::endl;
//
//	clock_t start, end;
//	start = clock();
//
//	// EMOC test run
//	if (parameter->is_open_multithread)
//		EMOCMultiThreadTest(parameter);
//	else
//		EMOCSingleThreadTest(parameter,runtime);
//
//	end = clock();
//	double time = (double)(end - start) / CLOCKS_PER_SEC;
//
//	//printf("------total run time: %fs--------\n\n", time);
//
//	//for (int i = 0; i < parameter->runs_num; ++i)
//	//{
//	//	printf("run %d igd value: %f \n", i, parameter->igd_value[i]);
//	//}
//
//	free(runtime);
//	free(parameter->igd_value);
//	delete parameter;
//
//	std::cout<<"\nTask has finished, please enter to exit."<<std::endl;
//
//	
//	return 0;
//}
//
//void *Work(void *args)
//{
//	ThreadParamters *parameter = (ThreadParamters *)args;
//	const char *algorithm_name = parameter->para->algorithm_name.c_str();
//	const char *problem_name = parameter->para->problem_name.c_str();
//
//	int population_num = parameter->para->population_num;
//	int dec_num = parameter->para->decision_num;
//	int obj_num = parameter->para->objective_num;
//	int max_eval = parameter->para->max_evaluation;
//	int output_interval = parameter->para->output_interval;
//
//	for (int run = parameter->run_start; run <= parameter->run_end; ++run)
//	{
//		int thread_id = parameter->thread_id;
//
//		// algorithm main entity
//		g_GlobalSettingsArray[thread_id] = new emoc::Global(algorithm_name, problem_name, population_num, dec_num, obj_num, max_eval, thread_id, output_interval, run);
//		g_GlobalSettingsArray[thread_id]->Start();
//
//
//		std::string problem_name = g_GlobalSettingsArray[thread_id]->problem_name_;
//		int obj_num = g_GlobalSettingsArray[thread_id]->obj_num_;
//		double igd = emoc::CalculateIGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num, problem_name);
//
//		printf("current thread id : %d, runs: %d, igd:%f\n", thread_id, run, igd);
//		parameter->para->igd_value[run] = igd;
//
//
//		//RecordPop(run, 0, g_GlobalSettingsArray[thread_id]);
//
//
//
//
//		delete g_GlobalSettingsArray[thread_id];
//	}
//
//	return nullptr;
//}
//
//void EMOCMultiThreadTest(EMOCParameters *parameter)
//{
//	int thread_num = parameter->thread_num;
//	std::vector<ThreadParamters*> thread_para_array(thread_num, nullptr);
//	for (int i = 0; i < thread_num; ++i)
//	{
//		thread_para_array[i] = (ThreadParamters *)malloc(sizeof(ThreadParamters));
//		thread_para_array[i]->para = parameter;
//	}
//
//	std::vector<int> job_overload(thread_num, 0);
//	int interval = (double)parameter->runs_num / thread_num;
//	int remainder = parameter->runs_num % thread_num;
//	for (int i = 0; i < thread_num; ++i)
//	{
//		job_overload[i] = interval;
//		if (remainder-- > 0)
//			job_overload[i]++;
//		//printf("thread %d: %d runs\n",i, job_overload[i]);
//	}
//
//	// multithread running
//	std::vector<pthread_t> tid(thread_num);
//	int total_overload = 0;
//	for (int i = 0; i < thread_num; ++i)
//	{
//		if (job_overload[i] > 0)
//		{
//			thread_para_array[i]->run_start = total_overload;
//			thread_para_array[i]->run_end = total_overload + job_overload[i] - 1;
//			thread_para_array[i]->thread_id = i;
//			total_overload += job_overload[i];
//		}
//		else
//			continue;
//
//		pthread_create(&tid[i], nullptr, Work, (void *)thread_para_array[i]);
//	}
//
//	for (int i = 0; i < thread_num; ++i)
//	{
//		if (job_overload[i] > 0)
//			pthread_join(tid[i], nullptr);
//	}
//
//
//	for (int i = 0; i < thread_num; ++i)
//		free(thread_para_array[i]);
//}
//
//void EMOCSingleThreadTest(EMOCParameters *parameter, double *runtime)
//{
//	const char *algorithm_name = parameter->algorithm_name.c_str();
//	const char *problem_name = parameter->problem_name.c_str();
//
//	int population_num = parameter->population_num;
//	int dec_num = parameter->decision_num;
//	int obj_num = parameter->objective_num;
//	int max_eval = parameter->max_evaluation;
//	int output_interval = parameter->output_interval;
//
//	for (int run = 0; run < parameter->runs_num; ++run)
//	{
//		int thread_id = 0;
//		//run time recording
//		clock_t start, end;
//		start = clock();
//
//		// algorithm main entity
//		g_GlobalSettingsArray[thread_id] = new emoc::Global(algorithm_name, problem_name, population_num, dec_num, obj_num, max_eval, thread_id, output_interval, run);
//		g_GlobalSettingsArray[thread_id]->Start();
//
//		end = clock();
//		double time = (double)(end - start) / CLOCKS_PER_SEC;
//
//		std::string problem_name = g_GlobalSettingsArray[thread_id]->problem_name_;
//		int obj_num = g_GlobalSettingsArray[thread_id]->obj_num_;
//		double igd = emoc::CalculateIGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num, problem_name);
//		//double gd = emoc::CalculateGD(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num, problem_name);
//		//double hv = emoc::CalculateHV(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num);
//		//double spacing = emoc::CalculateSpacing(g_GlobalSettingsArray[thread_id]->parent_population_.data(), g_GlobalSettingsArray[thread_id]->population_num_, obj_num);
//
//		printf("run %d time: %fs  igd: %f\n",run, time,igd);
//
//		runtime[run] = time;
//		parameter->igd_value[run] = igd;
//		//RecordPop(run, 0, g_GlobalSettingsArray[thread_id]);
//
//
//		if (!parameter->is_open_multithread && parameter->runs_num == 1)
//		{
//			time = runtime[0];
//			time -= g_GlobalSettingsArray[0]->RecordFileTime();
//			RecordTime(0, parameter, time);
//			printf("---------------------run time file has been recorded %f--------------------\n", time);
//		}
//
//
//		delete g_GlobalSettingsArray[thread_id];
//	}
//}
















// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include <iostream>

#include "imgui.h"
#include "./ui/imgui_impl_glfw.h"
#include "./ui/imgui_impl_opengl3.h"
#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

//// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
//// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
//// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
//#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
//#pragma comment(lib, "legacy_stdio_definitions")
//#endif


void embraceTheDarkness();
void embraceTheClassic();

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) return 1;


	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1600, 900, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

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
	embraceTheClassic();
	//ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}





	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

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
	io.Fonts->AddFontFromFileTTF("./resource/fonts/DroidSans.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("./fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	float testArray[4] = { 0.21f, 0.3f, 0.4f, 0.5f };

	static int D = 123;
	static int M = 123;
	static int N = 123;
	static int Evaluation = 123;

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());




		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// this is a simple try for design the ui for EMOC
		{
			ImGui::Begin("Algorithm and Problem Selection");
			const char* algorithmName[] = { "MOEAD", "NSGA2", "IBEA", "MOEADDE", };
			const char* problemName[] = { "ZDT1", "ZDT2", "ZDT3", "ZDT4", };
			static int algorithmIndex = 0; // If the selection isn't within 0..count, Combo won't display a preview
			static int problemIndex = 0; // If the selection isn't within 0..count, Combo won't display a preview

			//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 100.0f);
			ImGui::Text("Algorithm");
			ImGui::Spacing();
			ImGui::Combo("##Algorithm", &algorithmIndex, algorithmName, IM_ARRAYSIZE(algorithmName));

			//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 100.0f);
			ImGui::Text("Problem");
			ImGui::Spacing();
			ImGui::Combo("##Problem", &problemIndex, problemName, IM_ARRAYSIZE(problemName));

			if (ImGui::Button("Start", ImVec2(100, 100)))
			{
				std::cout << algorithmName[algorithmIndex] << "\n"
					<< problemName[problemIndex] << "\n"
					<< "population number: " << N << "\n"
					<< "obj dim: " << M << "\n"
					<< "dec dim: " << D << "\n"
					<< "evaluation number: " << Evaluation << "\n";
			}

			if (ImGui::Button("Stop", ImVec2(100, 100)))
			{
				std::cout << "Stop!\n";
			}

			if (ImGui::Button("Pause", ImVec2(100, 100)))
			{
				std::cout << "Pause\n";
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
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Log");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", testArray); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

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

		// Rendering and Setting basical parameters
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}



void embraceTheClassic()
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

void embraceTheDarkness()
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