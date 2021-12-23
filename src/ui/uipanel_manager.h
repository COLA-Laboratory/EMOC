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
		void CleanUp();

		// getter
		inline int GetExpAlgorithmNum() { return experiment_panel_.table_algorithms.size(); }

		// setter for the convinence of calculating progress, invoked by algorithm
		inline void SetCurrentEvaluation(int num) { test_panel_.current_evaluation = num; }
		inline void SetUIPanelState(UIPanel state) { panel_state_ = state; }

		// utility functions
		inline bool IsTerminate() { return !glfwWindowShouldClose(window_); }
		void AddAvailSingleThreadResult(std::string res);	// for test panel plot window
		void AddAvailMultiThreadResult(std::string res);	// for experiment panel plot window

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

		// erroe callback
		static void glfw_error_callback(int error, const char* description){ fprintf(stderr, "Glfw Error %d: %s\n", error, description);}

	public:
		ImFont* font_bold;
		ImFont* font_normal;

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

		// different UI panels
		TestPanel test_panel_;
		ExperimentPanel experiment_panel_;

	};

}