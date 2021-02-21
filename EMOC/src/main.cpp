#include <ctime>
#include <iostream>
#include <memory>
#include <algorithm>
#include <vector>

#include "core/global.h"
#include "core/individual.h"
#include "problem/zdt.h"
#include "problem/dtlz.h"
#include "algorithms/moead/moead.h"
#include "algorithms/moead_de/moead_de.h"
#include "algorithms/nsga2/nsga2.h"
#include "algorithms/ibea/ibea.h"
#include "algorithms/spea2/spea2.h"
#include "algorithms/smsemoa/smsemoa.h"
#include "algorithms/moead_dra/moead_dra.h"
#include "algorithms/moead_frrmab/moead_frrmab.h"
#include "metric/hv.h"
#include "metric/igd.h"
#include "random/random.h"


using emoc::g_GlobalSettings;

int main()
{		
	double igd_sum = 0;
	// initilize some bases for random number
	randomize();
	for (int i = 0; i < 10; ++i)
	{
		clock_t start, end;
		start = clock();

		// todo: move this into global member function and take care lower/upper case
		g_GlobalSettings = new emoc::Global("MOEADFRRMAB", "zdt6", 100, 20, 2, 50000);
		emoc::Problem *problem = new emoc::ZDT6(g_GlobalSettings->dec_num_, g_GlobalSettings->obj_num_);
		emoc::Algorithm *algorithm = new emoc::MOEADFRRMAB(problem);

		algorithm->Run();
		//algorithm->PrintPop();

		end = clock();
		double time = (double)(end - start) / CLOCKS_PER_SEC;
		printf("runtime : %fs\n", time);

		double igd = emoc::CalculateIGD(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		igd_sum += igd;
		//double hv = emoc::CalculateHV(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		printf("igd : %f\n", igd);
		//printf("hv : %f\n", hv);

		delete g_GlobalSettings;
		delete problem;
		delete algorithm;
	}
	printf("mean igd : %f\n", igd_sum/10.0);



	getchar();
	return 0;
}