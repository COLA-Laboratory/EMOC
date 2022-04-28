#include "core/emoc_app.h"

namespace emoc {

	// init static lock related variables
	std::mutex EMOCLock::test_finish_mutex;
	std::mutex EMOCLock::experiment_finish_mutex;
	std::mutex EMOCLock::test_pause_mutex;
	std::mutex EMOCLock::experiment_pause_mutex;
	std::mutex EMOCLock::multithread_data_mutex;
	std::condition_variable EMOCLock::test_pause_cond;
	std::condition_variable EMOCLock::experiment_pause_cond;
	std::array<std::mutex, 32> EMOCLock::mutex_pool;

	EMOCApplication::EMOCApplication() :
		plot_manager_(nullptr),
		ui_manager_(nullptr),
		emoc_manager_(nullptr)
	{
	}

	EMOCApplication::~EMOCApplication()
	{
		// All the singleton instance will be released by a garbage collection variable in their own class implementation.
	}

	// TODO: Init() can receive more parameters from cmd line or any other kind of customized settings.
	void EMOCApplication::Init(bool is_gui, const EMOCParameters& para)
	{
		if (is_gui)
		{
			plot_manager_ = PlotManager::Instance();
			ui_manager_ = UIPanelManager::Instance();
			ui_manager_->Init(1700, 900, "EMOC");
		}

		emoc_manager_ = EMOCManager::Instance();
		emoc_manager_->SetIsGUI(is_gui);
		emoc_manager_->SetTaskParameters(para);
		if (para.is_open_multithread) emoc_manager_->SetIsExperiment(true); // For population store, we need set it to experiment mode. 
	}

	void EMOCApplication::Run()
	{
		if (emoc_manager_->GetIsGUI())  // gui mode
		{
			while (ui_manager_->IsTerminate())
			{
				// rendering
				ui_manager_->RenderPanel();

				// swap buffers and poll events
				ui_manager_->Update();
			}
			ui_manager_->CleanUp();
		}
		else							// none gui mode
		{
			emoc_manager_->Run();
		}
	}

}