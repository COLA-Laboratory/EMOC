#pragma once

namespace emoc {

	class Individual
	{
	public:
		Individual(int dec_num, int obj_num);
		~Individual();

	public:
		int rank_;
		double fitness_;
		double *dec_;
		double *obj_;
	};

}