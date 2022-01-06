#include <iostream>

#include "core/file.h"
#include "emoc_app.h"

using emoc::EMOCManager;
using emoc::PlotManager;
using emoc::UIPanelManager;
using emoc::EMOCApplication;
using emoc::EMOCParameters;


int main(int argc, char* argv[])
{
	// parse command line input
	bool is_gui = false;
	EMOCParameters para;
	emoc::EMOCParamerterParse(argc, argv, para, is_gui);

	// start to run
	EMOCApplication emoc_app;
	is_gui = true; // for debug
	emoc_app.Init(is_gui, para);
	emoc_app.Run();

	return 0;
}