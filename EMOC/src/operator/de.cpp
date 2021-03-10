#include "operator/de.h"

#include "random/random.h"

namespace emoc {

	void DE(Individual *parent1, Individual *parent2, Individual *parent3, Individual *offspring, Global *g_GlobalSettings)
	{
		double value = 0.0;

		for (int i = 0; i < g_GlobalSettings->dec_num_; ++i)
		{
			double yl = g_GlobalSettings->dec_lower_bound_[i];
			double yu = g_GlobalSettings->dec_upper_bound_[i];

			if (randomperc() < g_GlobalSettings->de_parameter_.crossover_pro)
			{
				value = parent1->dec_[i] + g_GlobalSettings->de_parameter_.F * (parent2->dec_[i] -parent3->dec_[i]);
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

