#include "operator/swap_mutation.h"

#include <cmath>

#include "random/random.h"

namespace emoc {

	void SwapMutation(Individual* ind, MutationParameter& mutation_para)
	{
		int dec_num = ind->dec_.size();

		int rand1 = rnd(0, dec_num - 1);
		int rand2 = rnd(0, dec_num - 1);

		if (randomperc() < 1.0 / mutation_para.pro)
		{
			double temp = ind->dec_[rand1];
			ind->dec_[rand1] = ind->dec_[rand2];
			ind->dec_[rand2] = temp;
		}
	}
}

