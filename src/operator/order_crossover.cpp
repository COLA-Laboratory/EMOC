#include "operator/order_crossover.h"

#include <vector>
#include <cmath>

#include "random/random.h"

namespace emoc {

	void OrderCrossover(Individual *parent1, Individual *parent2, Individual *offspring1, Individual *offspring2)
	{
		int dec_num = parent1->dec_.size();
		std::vector<int> ht1(dec_num, 0), ht2(dec_num,0);
		int k = rnd(0, dec_num -1);

		for (int i = 0; i < dec_num; i++)
		{
			if (i <= k)
			{
				offspring1->dec_[i] = parent1->dec_[i];
				offspring2->dec_[i] = parent2->dec_[i];
				ht1[(int)(offspring1->dec_[i])] = 1;
				ht2[(int)(offspring2->dec_[i])] = 1;
			}
		}

		int index1 = k + 1, index2 = k + 1;
		for (int i = 0; i < dec_num; i++)
		{
			if (ht1[(int)(parent2->dec_[i])] == 0)
			{
				offspring1->dec_[index1++] = parent2->dec_[i];
				ht1[(int)(parent2->dec_[i])] = 1;
			}

			if (ht2[(int)(parent1->dec_[i])] == 0)
			{
				offspring2->dec_[index2++] = parent1->dec_[i];
				ht2[(int)(parent1->dec_[i])] = 1;
			}
		}
	}
}

