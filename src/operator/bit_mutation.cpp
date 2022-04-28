#include "operator/bit_mutation.h"

#include <cmath>

#include "random/random.h"

namespace emoc {

	void BitFlipMutation(Individual* ind, MutationParameter& mutation_para)
	{
		int dec_num = ind->dec_.size();

		for (int i = 0; i < dec_num; i++)
		{
			if (randomperc() < mutation_para.pro)
				ind->dec_[i] = 1 - ind->dec_[i];
		}
	}

}

