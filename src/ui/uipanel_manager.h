#pragma once
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>

#include "ui/test_panel.h"
#include "ui/experiment_panel.h"
#include "core/emoc_manager.h"
#include "imgui.h"
#include "GLFW/glfw3.h"

namespace emoc {

	enum class UIPanel
	{
		TestPanel,
		ExperimentPanel,
		AnalysePanel
	};

	class UIPanelManager
	{
	public:
		enum class EMOCUIStyle
		{
			Classic,
			Dark,
		};

	public:
		static UIPanelManager* Instance();
		
		void Init(int width, int height, const std::string& title);
		void RenderPanel();
		void Update();

		inline bool IsTerminate() { return !glfwWindowShouldClose(window_);}

		// setters for the convinence of calculating progress, invoked by algorithm
		void SetCurrentEvaluation(int num) { test_panel_.current_evaluation = num; }
		void SetUIPanelState(UIPanel state) { panel_state_ = state; }

	private:
		UIPanelManager();
		~UIPanelManager();
		UIPanelManager(const UIPanelManager&);
		UIPanelManager& operator=(const UIPanelManager&);

		// initialiation functions
		void InitGLFW(int width, int height, const std::string& title);
		void InitImGui();

		// UI style
		void EmbraceTheClassic();
		void EmbraceTheDarkness();

		static void glfw_error_callback(int error, const char* description)
		{
			fprintf(stderr, "Glfw Error %d: %s\n", error, description);
		}

	private:
		// for release UIPanelManager instance
		class Garbo
		{
		public:
			Garbo() {};
			~Garbo()
			{
				if (s_Instance != nullptr)
				{
					delete s_Instance;
					s_Instance = nullptr;
				}
			}
		};
		static Garbo garbo_;
		static UIPanelManager* s_Instance;
		static std::mutex singleton_mutex_;

		// UI parameters
		int width_;
		int height_;
		std::string title_;
		std::string glsl_version_;
		GLFWwindow* window_;
		UIPanel panel_state_;

		// UI panels
		TestPanel test_panel_;
		ExperimentPanel experiment_panel_;


		// for test now
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	};

}