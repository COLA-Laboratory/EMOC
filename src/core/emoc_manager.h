#pragma once
#include <vector>
#include <mutex>

#include "core/file.h"
#include "core/global.h"

namespace emoc{

	struct EMOCTask
	{

	};

	struct EMOCMultiThreadResult
	{

	};

	struct EMOCSingleThreadResult
	{
		std::string description;
		EMOCParameters para;
		double last_igd;
		double last_hv;
		double last_spread;
		double last_spacing;
		double runtime;
		int max_iteration;

		std::vector<double> igd_history;
		std::vector<double> hv_history;
		std::vector<double> spacing_history;
		std::vector<double> spread_history;
	};

	class EMOCManager
	{
	public:
		static EMOCManager* Instance();
		
		void Run();

		// Getters
		inline bool GetPause() { return is_pause_; }
		inline bool GetPlot() { return is_plot_; }
		inline bool GetFinish() { return is_finish_; }
		inline const EMOCParameters &GetParameters() { return para_; }
		inline Global* GetGlobalSetting(int index) { return g_GlobalSettingsArray[index]; }
		inline int GetSingleThreadResultSize() { return (int)single_thread_result_historty_.size(); }
		inline const EMOCSingleThreadResult& GetSingleThreadResult(int index) { return single_thread_result_historty_[index]; }

		// Setters
		inline void SetPlot(bool is_plot) { is_plot_ = is_plot; }
		inline void SetPause(bool is_pause) { is_pause_ = is_pause; }
		inline void SetFinish(bool is_finish) { is_finish_ = is_finish; }
		inline void SetTaskParameters(const EMOCParameters& para) { para_ = para; is_para_set_ = true;}


	private:
		EMOCManager();
		~EMOCManager();
		EMOCManager(const EMOCManager &);
		EMOCManager& operator=(const EMOCManager &);

		void EMOCSingleThreadRun();
		void EMOCMultiThreadRun();
		void MultiThreadWorker(int run_start, int run_end, int thread_id);

	private:
		// for release EMOCManager instance
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
		static EMOCManager* s_Instance;
		static std::mutex singleton_mutex_;

		// EMOC state variables
		EMOCParameters para_;
		bool is_para_set_;
		bool is_plot_;
		bool is_pause_;
		bool is_finish_;


		std::vector<Global*> g_GlobalSettingsArray;
		std::vector<EMOCSingleThreadResult> single_thread_result_historty_;

	};

}