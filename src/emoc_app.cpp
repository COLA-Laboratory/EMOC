#include "emoc_app.h"

namespace emoc {

	std::mutex EMOCLock::finish_mutex;
	std::mutex EMOCLock::pause_mutex;
	std::condition_variable EMOCLock::pause_cond;

	EMOCApplication::EMOCApplication() :
		plot_manager_(nullptr),
		ui_manager_(nullptr),
		emoc_manager_(nullptr)
	{

	}

	EMOCApplication::~EMOCApplication()
	{
	}

	void EMOCApplication::Init()
	{
		plot_manager_ = PlotManager::Instance();
		ui_manager_ = UIPanelManager::Instance();
		emoc_manager_ = EMOCManager::Instance();

		ui_manager_->Init(1600, 900, "EMOC"); 
	}

	void EMOCApplication::Run()
	{
		while (ui_manager_->IsTerminate())
		{
			ui_manager_->RenderPanel();
			ui_manager_->Update();
		}
	}

}