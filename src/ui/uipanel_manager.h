#pragma once
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>

#include "core/emoc_manager.h"
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

		inline bool IsTerminate() { return !glfwWindowShouldClose(window_);}

		// two setters for the convinence of calculating progress, invoked by algorithm
		void SetCurrentEvaluation(int num) { current_evaluation = num; }
		void SetMaxEvaluation(int num) { max_evaluation = num; }

	private:
		UIPanelManager();
		~UIPanelManager();
		UIPanelManager(const UIPanelManager&);
		UIPanelManager& operator=(const UIPanelManager&);

		// initialiation functions
		void InitGLFW(int width, int height, const std::string& title);
		void InitImGui();
		void InitAlgorithmList();
		void InitProlbemList();
		void InitDisplayList();

		// display functions according current settings
		void DisplayAccordingToColumn(const EMOCSingleThreadResult& res, const std::string& col_name, int row);	// column display in default test module's table 
		void DisplayAlgorithmParameters(const std::string& algorithm);				// display algorithm's parameters
		void DisplaySelectedAlgorithm(int index);									// algorithm display in experiment module's parameter window 
		void DisplaySelectedProblem(int index, int item_width, int item_pos);		// problem display in experiment module's parameter window
		void DisplayMovePopup(int index, bool is_algorithm_popup);					// popups in experiment module's parameter window for moving or deleting selected algorithms and problems


		// ui layout for different panels
		void RenderTestPanel();
		void RenderExperimentPanel();
		void RenderAnalysePanel();

		

		// UI style
		void EmbraceTheClassic();
		void EmbraceTheDarkness();

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

		// for test now

		// UI data for test and experiment module
		int algorithm_index, problem_index, display_index;	// index need to be reset when switching the panel
		std::vector<char*> algorithm_names;
		std::vector<char*> problem_names;
		std::vector<char*> display_names;

		// UI data for test module
		int current_evaluation;
		int max_evaluation;
		int N = 100;
		int D = 30;
		int M = 2;
		int Evaluation = 26000;

		// UI data for experiment module
		std::vector<std::string> selected_algorithms;
		std::unordered_map<std::string, int> selected_algorithm_map;
		std::vector<std::string> selected_problems;
		std::unordered_map<std::string, int> selected_problem_map;
		std::vector<int> Ns;
		std::vector<int> Ds;
		std::vector<int> Ms;
		std::vector<int> Evaluations;
		int run_num = 30;
		int save_interval = 100000;
	};

}