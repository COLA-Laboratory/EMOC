#include "operator/bit_mutation.h"

#include <cmath>

#include "random/random.h"

namespace emoc {

	void BitMutation(Individual* ind, Global* g_GlobalSettings)
	{
		for (int i = 0; i < g_GlobalSettings->dec_num_; i++)
		{
			if (randomperc() < g_GlobalSettings->pm_parameter_.muatation_pro)
				ind->dec_[i] = 1 - ind->dec_[i];
		}
	}

}

