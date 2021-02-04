#include <ctime>
#include <iostream>
#include <memory>
#include <algorithm>
#include <vector>

#include "core/global.h"
#include "core/individual.h"
#include "problem/zdt.h"
#include "algorithms/moead/moead.h"
#include "algorithms/moead_de/moead_de.h"
#include "algorithms/nsga2/nsga2.h"
#include "algorithms/ibea/ibea.h"
#include "metric/hv.h"
#include "metric/igd.h"
#include "random/random.h"


using emoc::g_GlobalSettings;

int main()
{
	clock_t start, end;
	start = clock();

	g_GlobalSettings = new emoc::Global("moead", "zdt4", 100, 10, 2,20000);
	emoc::Problem *problem = new emoc::ZDT4(g_GlobalSettings->dec_num_, g_GlobalSettings->obj_num_);
	emoc::Algorithm *algorithm = new emoc::IBEA(problem);

	algorithm->Run();
	//algorithm->PrintPop();

	end = clock();
	double time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("runtime : %fs\n", time);

	double igd = emoc::CalculateIGD(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
	double hv = emoc::CalculateHV(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
	printf("igd : %f\n", igd);
	printf("hv : %f\n", hv);

	delete g_GlobalSettings;
	delete problem;
	delete algorithm;



	getchar();
	return 0;
}