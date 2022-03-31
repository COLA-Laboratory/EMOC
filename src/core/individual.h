#pragma once

#include <vector>

namespace emoc {

	// Class for each solution in optimization process, and
	// it holds some basic datas of each solution.
	class Individual
	{
	public:
		Individual(int dec_num, int obj_num);
		Individual(const Individual& ind);
		~Individual();

	public:
		int rank_;
		double fitness_;
		double* dec_;
		double* obj_;

		// This is for data exchange between c++ and python, you should use the dec_ and obj_ 
		// in the c++ side.
		std::vector<double> dec;
		std::vector<double> obj;
	};

}