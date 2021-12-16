#include <iostream>

#include "emoc_app.h"
#include "core/emoc_manager.h"
#include "ui/plot.h"
#include "ui/uipanel_manager.h"

#include "alglib/src/statistics.h"


using emoc::EMOCManager;
using emoc::PlotManager;
using emoc::UIPanelManager;
using emoc::EMOCApplication;



int main(int argc, char* argv[])
{
	std::vector<double> testarray1 = { 1,2,3,4,5,6 };
	std::vector<double> testarray2 = { 3,4,3,4,1,6 };
	alglib::real_1d_array t1, t2;
	t1.setcontent(testarray1.size(), testarray1.data());
	t2.setcontent(testarray2.size(), testarray2.data());
	double p1, p2, p3;
	alglib::mannwhitneyutest(t1, testarray1.size(), t2, testarray2.size(), p1, p2, p3);
	std::cout << p1 << "\n";


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