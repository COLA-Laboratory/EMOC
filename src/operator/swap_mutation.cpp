#include "operator/swap_mutation.h"

#include <cmath>

#include "random/random.h"

namespace emoc {

	void SwapMutation(Individual* ind, Global* g_GlobalSettings)
	{
		int rand1 = rnd(0, g_GlobalSettings->dec_num_ - 1);
		int rand2 = rnd(0, g_GlobalSettings->dec_num_ - 1);

		if (randomperc() < 1.0 / g_GlobalSettings->population_num_)
		{
			double temp = ind->dec_[rand1];
			ind->dec_[rand1] = ind->dec_[rand2];
			ind->dec_[rand2] = temp;
		}
	}
}

