#include "algorithms/algorithm.h"

#include <iostream>

#include "core/global.h"

namespace emoc {

	Algorithm::Algorithm(Problem *problem):problem_(problem)
	{

	}

	Algorithm::~Algorithm()
	{

	}

	void Algorithm::PrintPop()
	{
		for (int i = 0; i < 2; ++i)
		{
			std::cout << "population[" << i << "]: \n";
			std::cout << "--dec--:";
			for (int j = 0; j < g_GlobalSettings->dec_num_; ++j)
			{
				std::cout << g_GlobalSettings->parent_population_[i]->dec_[j] << " ";
			}
			std::cout << "\n--obj--:";
			for (int j = 0; j < g_GlobalSettings->obj_num_; ++j)
			{
				std::cout << g_GlobalSettings->parent_population_[i]->obj_[j] << " ";
			}
			std::cout << std::endl;
		}
	}

	void Algorithm::EvaluatePop(Individual **pop, int pop_num)
	{
		for (int i = 0; i < pop_num; ++i)
		{
			EvaluateInd(pop[i]);
		}
	}

	void Algorithm::EvaluateInd(Individual *ind)
	{
		problem_->CalObj(ind);
		g_GlobalSettings->current_evaluation_++;
	}

	int Algorithm::MergePopulation(Individual **pop_src1, int pop_num1, Individual **pop_src2, int pop_num2, Individual **pop_dest)
	{
		int i = 0, j = 0;

		// copy fist population
		for (i = 0; i < pop_num1; i++)
		{
			CopyIndividual(pop_src1[i], pop_dest[i]);
		}

		// copy second population
		for (j = 0; j < pop_num2; j++, i++)
		{
			CopyIndividual(pop_src2[j], pop_dest[i]);
		}
		return i;
	}

	void Algorithm::CopyIndividual(Individual *ind_src, Individual *ind_dest)
	{
		// copy individual properties
		ind_dest->fitness_ = ind_src->fitness_;
		ind_dest->rank_ = ind_src->rank_;

		// copy individual decision and objective datas
		memcpy(ind_dest->dec_, ind_src->dec_, sizeof(double) * g_GlobalSettings->dec_num_);
		memcpy(ind_dest->obj_, ind_src->obj_, sizeof(double) * g_GlobalSettings->obj_num_);
	}

}