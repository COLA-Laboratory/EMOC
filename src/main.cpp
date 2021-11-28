#include <iostream>
#include <thread>

#include "emoc_app.h"
#include "core/emoc_manager.h"
#include "ui/plot.h"
#include "ui/uipanel_manager.h"


using emoc::EMOCManager;
using emoc::PlotManager;
using emoc::UIPanelManager;
using emoc::EMOCApplication;

//EMOCApplication emoc_app;
//emoc_app.Init();
//emoc_app.Run();

int main(int argc, char* argv[])
{
	// init
	//PlotManager* plot_manager = PlotManager::Instance();
	//EMOCManager* emoc_manager = EMOCManager::Instance();
	//UIPanelManager* ui_manager = UIPanelManager::Instance();

	//ui_manager->Init(1600, 900, "EMOC");
	//while (ui_manager->IsTerminate())
	//{
	//	ui_manager->RenderPanel();
	//	ui_manager->Update();
	//}
	
	EMOCApplication emoc_app;
	emoc_app.Init();
	emoc_app.Run();


	//// multithread test
	//emoc::EMOCParameters parameter;
	//ParseParamerters(argc, argv, &parameter);
	//emoc_manager->SetTaskParameters(parameter);
	//emoc_manager->Run();

	
	return 0;
}