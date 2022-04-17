#pragma once

#include <vector>

namespace emoc {

	// Class for each solution in optimization process, and
	// it holds some basic datas of each solution.
	class Individual
	{
	public:
		Individual(int dec_num, int obj_num);
		~Individual();

	public:
		int rank_;
		double fitness_;

		// This is for data exchange between c++ and python, you should use the dec_ and obj_ 
		// in the c++ side.  TODO: this should be modified, just use the vector directly in c++.
		std::vector<double> dec_;
		std::vector<double> obj_;
		std::vector<double> con_;
	};

}