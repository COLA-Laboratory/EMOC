#include "operator/de.h"

#include "random/random.h"

namespace emoc {

	void DE(Individual *parent1, Individual *parent2, Individual *parent3, Individual *offspring, 
		std::vector<double>& lower_bound, std::vector<double>& upper_bound, CrossoverParameter& cross_para)
	{
		double value = 0.0;
		int dec_num = parent1->dec_.size();
		double F = cross_para.index1;

		for (int i = 0; i < dec_num; ++i)
		{
			double yl = lower_bound[i];
			double yu = upper_bound[i];

			if (randomperc() < cross_para.pro)
			{
				value = parent1->dec_[i] + F * (parent2->dec_[i] -parent3->dec_[i]);
				value = (value > yu) ? yu : (value < yl) ? yl : value;
			}
			else
			{
				value = parent1->dec_[i];
			}
			offspring->dec_[i] = value;
		}
	}

}

