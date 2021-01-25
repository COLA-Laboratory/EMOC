#include <ctime>
#include <iostream>
#include <memory>
#include <vector>


#include "core/global.h"
#include "core/individual.h"
#include "problem/zdt.h"
#include "algorithms/nsga2.h"
#include "metric/igd.h"
#include "random/random.h"


using emoc::g_GlobalSettings;

int main()
{
	clock_t start, end;
	start = clock();

	g_GlobalSettings = new emoc::Global("nsga2", "zdt1", 100, 30, 2, 25000);
	emoc::Problem *problem = new emoc::ZDT1(g_GlobalSettings->dec_num_, g_GlobalSettings->obj_num_);
	emoc::Algorithm *algorithm = new emoc::NSGA2(problem);

	algorithm->Run();
	//algorithm->PrintPop();

	end = clock();
	double time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("runtime : %fs\n", time);

	double igd = CalculateIGD(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
	printf("igd : %f\n", igd);

	delete g_GlobalSettings;
	delete problem;
	delete algorithm;



	getchar();
	return 0;
}