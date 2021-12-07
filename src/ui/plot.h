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

		void Send(const std::string& cmd);
		void RefreshPipe();
		void OpenPlotPipe();
		void ClosePlotPipe();

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
		bool is_window_close_;
	};

}