#include <iostream>

#include "emoc_app.h"
#include "core/emoc_manager.h"
#include "ui/plot.h"
#include "ui/uipanel_manager.h"
#include "core/file.h"
#include "cxxopts/cxxopts.hpp"


using emoc::EMOCManager;
using emoc::PlotManager;
using emoc::UIPanelManager;
using emoc::EMOCApplication;
using emoc::EMOCParameters;




int main(int argc, char* argv[])
{
	//std::vector<double> testarray1 = { 0.00848513,0.00657415,0.00728577,0.008876,0.286776,0.0091205,0.0134237,0.017676,0.0106735,0.00795029,0.00722106,0.0075432,0.00885766,0.012184,0.00745612,0.00804236,0.0084011,0.00650826,0.00645001,0.00807553 };
	//std::vector<double> testarray2 = { 0.00718789,0.00761454,0.0099335,0.00703661,0.00727013,0.00864156,0.00715663,0.00715534,0.00628231,0.00780061,0.00802618,0.00810133,0.00763612,0.00789531,0.0069083,0.0079907,0.00672834,0.00760881,0.0078803,0.0072773 };
	//alglib::real_1d_array t1, t2;
	//t1.setcontent(testarray1.size(), testarray1.data());
	//t2.setcontent(testarray2.size(), testarray2.data());
	//double p1, p2, p3;
	//alglib::mannwhitneyutest(t1, testarray1.size(), t2, testarray2.size(), p1, p2, p3);
	//std::cout << std::thread::hardware_concurrency() << "\n";

	//double test = -1.0;

	//if (test == -1.0)
	//	std::cout << "wuhu!\n";



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
	
	// parse command line input
	bool is_gui = false;
	EMOCParameters para;
	emoc::EMOCParamerterParse(argc, argv, para, is_gui);

	// start to run
	EMOCApplication emoc_app;
	// is_gui = true; 
	//para.is_open_multithread = true;
	emoc_app.Init(is_gui, para);
	emoc_app.Run();


	// multithread test
	//emoc::EMOCParameters parameter;
	//ParseParamerters(argc, argv, &parameter);
	//if (parameter.is_open_multithread) emoc_manager->SetIsExperiment(true);
	//emoc_manager->SetTaskParameters(parameter);
	//emoc_manager->Run();

	
	return 0;
}