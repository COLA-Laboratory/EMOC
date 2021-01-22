#include <ctime>
#include <iostream>
#include <memory>
#include <vector>


#include "core/global.h"
#include "random/random.h"
#include "individual.h"
#include "problem/zdt.hpp"
#include "algorithm/nsga2.h"


using emoc::g_GlobalSettings;

int main()
{
	clock_t start, end;
	start = clock();

 	randomize();
// 	for (int i = 0; i < 100; ++i)
// 		std::cout << randomperc()<<std::endl;

	g_GlobalSettings = new emoc::Global("nsga2", "zdt1", 10, 10, 2, 25000);


	g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

	//for (auto ind : g_GlobalSettings->parent_population_)
	//{
	//	for (int i = 0; i < g_GlobalSettings->dec_num_; ++i)
	//	{
	//		std::cout << ind->dec_[i] << " ";
	//	}
	//	std::cout << "\n";
	//}
	emoc::Problem *problem = new emoc::ZDT1(g_GlobalSettings->dec_num_, g_GlobalSettings->obj_num_);
	emoc::Algorithm *algorithm = new emoc::NSGA2(problem);

	algorithm->Run();
	algorithm->PrintPop();
	for (int i = 0; i < g_GlobalSettings->population_num_; ++i)
	{
		std::cout << "off_population[" << i << "]: \n";
		std::cout << "    dec:";
		for (int j = 0; j < g_GlobalSettings->dec_num_; ++j)
		{
			std::cout << g_GlobalSettings->offspring_population_[i]->dec_[j] << " ";
		}
		std::cout << " obj:";
		for (int j = 0; j < g_GlobalSettings->obj_num_; ++j)
		{
			std::cout << g_GlobalSettings->offspring_population_[i]->obj_[j] << " ";
		}
		std::cout << std::endl;
	}

	for (int i = 0; i < g_GlobalSettings->population_num_ * 2; ++i)
	{
		std::cout << "mixed_population[" << i << "]: \n";
		std::cout << "    dec:";
		for (int j = 0; j < g_GlobalSettings->dec_num_; ++j)
		{
			std::cout << g_GlobalSettings->mixed_population_[i]->dec_[j] << " ";
		}
		std::cout << " obj:";
		for (int j = 0; j < g_GlobalSettings->obj_num_; ++j)
		{
			std::cout << g_GlobalSettings->mixed_population_[i]->obj_[j] << " ";
		}
		std::cout << std::endl;
	}


	delete g_GlobalSettings;
	delete problem;
	delete algorithm;

	end = clock();
	double time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("runtime : %fs\n", time);

	getchar();
	return 0;
}