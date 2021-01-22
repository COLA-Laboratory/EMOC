#pragma once
#include "individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.hpp"

namespace emoc {

	class NSGA2 : public Algorithm
	{
	public:
		NSGA2(Problem *problem);
		~NSGA2();

		void Run();

	private:
		void Initialization();
		void Crossover(Individual **parent_pop, Individual **offspring_pop);

		void CrowdingDistance();
		void EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop);

	};

}