#pragma once

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
		double *dec_;
		double *obj_;
	};

}