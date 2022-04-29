#include "operator/uniform_crossover.h"

#include <cmath>

#include "random/random.h"

namespace emoc {

	void UniformCrossover(Individual *parent1, Individual *parent2, Individual *offspring1, Individual *offspring2)
	{
		int dec_num = parent1->dec_.size();

		for (int i = 0; i < dec_num; i++)
		{
			if (randomperc() < 0.5)
			{
				offspring1->dec_[i] = parent2->dec_[i];
				offspring2->dec_[i] = parent1->dec_[i];
			}
			else
			{
				offspring1->dec_[i] = parent1->dec_[i];
				offspring2->dec_[i] = parent2->dec_[i];
			}
		}
	}
}

