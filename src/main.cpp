#include <iostream>

#include "emoc_app.h"
#include "core/emoc_manager.h"
#include "ui/plot.h"
#include "ui/uipanel_manager.h"


using emoc::EMOCManager;
using emoc::PlotManager;
using emoc::UIPanelManager;
using emoc::EMOCApplication;



int main(int argc, char* argv[])
{
	double test = -1.0;

	if (test == -1.0)
		std::cout << "wuhu!\n";

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


	// multithread test
	//emoc::EMOCParameters parameter;
	//ParseParamerters(argc, argv, &parameter);
	//if (parameter.is_open_multithread) emoc_manager->SetIsExperiment(true);
	//emoc_manager->SetTaskParameters(parameter);
	//emoc_manager->Run();

	
	return 0;
}