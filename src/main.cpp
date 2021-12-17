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
	std::vector<double> testarray1 = { 0.00665973 ,0.00775907, 0.0105039 ,0.0136247 ,0.00649665 ,0.0115674 ,0.00465236 ,0.0071315 ,0.00578957 ,0.0102293 ,0.0108844 ,0.00884142 ,0.00812817 ,0.0215179 ,0.0114513 };
	std::vector<double> testarray2 = { 2.09561, 8.17319 ,0.00494026 ,0.0114405 ,0.357844 ,000420253 ,0.696275 ,7.54302 ,0.00444581 ,2.52816 ,0.238478 ,3.08217 ,2.15439 ,0.00441019 ,0.00436881 };
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