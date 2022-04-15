#include <iostream>

#include "core/file.h"
#include "core/emoc_manager.h"
#include "emoc_app.h"
#include "core/uniform_point.h"

using emoc::EMOCManager;
using emoc::PlotManager;
using emoc::UIPanelManager;
using emoc::EMOCApplication;
using emoc::EMOCParameters;

int main(int argc, char* argv[])
{
	int obj_num = 5;
	char columns[128] = "";
	int index = 0;
	for (int i = 0; i < obj_num - 1; i++)
		sprintf(columns, "%s%d:", columns, i + 1);
	sprintf(columns, "%s%d", columns, obj_num);

	char xtics[1024] = "";
	char ytics[1024] = "";
	sprintf(xtics, "set xtics(");
	for (int i = 0; i < obj_num - 1; i++)
		sprintf(xtics, "%s%d,", xtics, i + 1);
	sprintf(xtics, "%s%d)", xtics, obj_num);
	std::cout << columns << "\n";
	std::cout << xtics << "\n";


	// parse parameters
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