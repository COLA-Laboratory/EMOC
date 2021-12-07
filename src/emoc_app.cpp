#include "emoc_app.h"

namespace emoc {

	// init static lock related variables
	std::mutex EMOCLock::test_finish_mutex;
	std::mutex EMOCLock::experiment_finish_mutex;
	std::mutex EMOCLock::test_pause_mutex;
	std::mutex EMOCLock::experiment_pause_mutex;
	std::mutex EMOCLock::multithread_data_mutex;
	std::condition_variable EMOCLock::test_pause_cond;
	std::condition_variable EMOCLock::experiment_pause_cond;

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
	void EMOCApplication::Init()
	{
		plot_manager_ = PlotManager::Instance();
		emoc_manager_ = EMOCManager::Instance();
		ui_manager_ = UIPanelManager::Instance();

		ui_manager_->Init(1600, 900, "EMOC"); 

		// default use gui mode
		emoc_manager_->SetIsGUI(true);
	}

	void EMOCApplication::Run()
	{
		while (ui_manager_->IsTerminate())
		{
			// rendering
			ui_manager_->RenderPanel();

			// poll events
			ui_manager_->Update();
		}
	}

}