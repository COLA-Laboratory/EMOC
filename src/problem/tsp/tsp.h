#pragma once
#include <vector>

#include "core/individual.h"
#include "problem/problem.h"

namespace emoc {

	class TSP :public Problem
	{
	public:
		TSP(int dec_num, int obj_num);
		virtual ~TSP();

		void CalObj(Individual* ind);

	private:
		void Init();

		std::vector<std::vector<double>> position_;
		std::vector<std::vector<double>> distance_;
	};

}