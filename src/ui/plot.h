#pragma once
#include <cstdio>
#include <queue>
#include <string>
#include <mutex>

namespace emoc {

	class PlotManager
	{
	public:
		static PlotManager* Instance();

		void Run();
		void Stop();
		void RefreshPipe();
		void Plot(const std::string& cmd);
		void OpenPlotPipe();
		void ClosePlotPipe();

		inline std::queue<std::string>& GetPlotCMDQueue() { return plot_cmds_; }

	private:
		PlotManager();
		~PlotManager();
		PlotManager(const PlotManager&);
		PlotManager& operator=(const PlotManager&);

	private:
		// for release PlotManager instance
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
		static PlotManager* s_Instance;
		static std::mutex singleton_mutex_;

		FILE* gp_;
		bool is_finish_;
		bool is_window_close_;
		std::queue<std::string> plot_cmds_;
		std::queue<std::string> plot_execute_cmds_;
	};

}