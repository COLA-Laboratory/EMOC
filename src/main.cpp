#include <iostream>

#include "core/file.h"
#include "core/emoc_manager.h"
#include "core/emoc_app.h"
#include "core/uniform_point.h"

using emoc::EMOCManager;
using emoc::PlotManager;
using emoc::UIPanelManager;
using emoc::EMOCApplication;
using emoc::EMOCParameters;

int main(int argc, char* argv[])
{

	// parse parameters
	bool is_gui = false;
	EMOCParameters para;
	emoc::EMOCParamerterParse(argc, argv, para, is_gui);

	// start to run
	EMOCApplication emoc_app;
	emoc_app.Init(is_gui, para);
	emoc_app.Run();

	return 0;
}