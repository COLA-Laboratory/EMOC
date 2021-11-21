#pragma once
#include <mutex>
#include <string>

#include "imgui.h"
#include "GLFW/glfw3.h"

namespace emoc {

	

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
		void SwitchUIStyle(EMOCUIStyle style);

		inline bool IsTerminate() { return !glfwWindowShouldClose(window_);}

	private:
		UIPanelManager();
		~UIPanelManager();
		UIPanelManager(const UIPanelManager&);
		UIPanelManager& operator=(const UIPanelManager&);


		void InitGLFW(int width, int height, const std::string& title);
		void InitImGui();
		void EmbraceTheClassic();
		void EmbraceTheDarkness();
		void RenderTestPanel();
		void RenderExperimentPanel();
		void RenderAnalysePanel();

		static void glfw_error_callback(int error, const char* description)
		{
			fprintf(stderr, "Glfw Error %d: %s\n", error, description);
		}

	private:
		enum class UIPanel
		{
			TestPanel,
			ExperimentPanel,
			AnalysePanel
		};

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

		// for test now
		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		float testArray[4] = { 0.21f, 0.3f, 0.4f, 0.5f };

		int D = 30;
		int M = 2;
		int N = 100;
		int Evaluation = 26000;
		// for test now
	};

}