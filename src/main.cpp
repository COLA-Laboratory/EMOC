#include <iostream>

#include "emoc_app.h"
#include "core/emoc_manager.h"
#include "ui/plot.h"
#include "ui/uipanel_manager.h"
#include "core/file.h"
#include "cxxopts/cxxopts.hpp"
#include "wfg/iwfg.h"


using emoc::EMOCManager;
using emoc::PlotManager;
using emoc::UIPanelManager;
using emoc::EMOCApplication;
using emoc::EMOCParameters;


int main(int argc, char* argv[])
{



	//std::cout << sizeof(POINT) << "\n ";
	//for (int i = 0; i < 20; i++)
	//	std::cout << points + i << "\n";
	


	// parse command line input
	bool is_gui = false;
	EMOCParameters para;
	emoc::EMOCParamerterParse(argc, argv, para, is_gui);

	// start to run
	EMOCApplication emoc_app;
	is_gui = true;
	emoc_app.Init(is_gui, para);
	emoc_app.Run();

	return 0;
}