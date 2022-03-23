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
	int num = 0;
	int obj_num = 5;
	double** lambda2 = emoc::UniformPoint(10, &num, obj_num);
	std::cout << num << "\n";
	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < obj_num; j++)
		{
			std::cout << lambda2[i][j] << " ";
		}
		std::cout << "\n";
	}


	// initialization for EMOC
	//EMOCManager::Instance()->Init();

	//std::unordered_map<std::string, std::vector<char*>>& IMPLEMENTED_ALGORITHMS = EMOCManager::Instance()->GetImplementedAlgorithms();
	//for (const auto& e : IMPLEMENTED_ALGORITHMS)
	//{
	//	std::cout << e.first << ":\n";
	//	for (const auto& v : e.second)
	//	{
	//		std::cout << v << "\n";
	//	}
	//}

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