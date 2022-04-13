#pragma once
#include <vector>

#include "core/individual.h"
#include "problem/problem.h"

namespace emoc {

	class Knapsack :public Problem
	{
	public:
		Knapsack(int dec_num, int obj_num);
		virtual ~Knapsack();

		void CalObj(Individual* ind);
		void CalCon(Individual* ind);

	private:
		void Init();

		std::vector<int> profit_;
		std::vector<int> weight_;
	};

}