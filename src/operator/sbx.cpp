#include "operator/sbx.h"

#include <cmath>

#include "random/random.h"

namespace emoc {

	void SBX(Individual *parent1, Individual *parent2, Individual *offspring1, Individual *offspring2, 
		std::vector<double>& lower_bound, std::vector<double>& upper_bound, CrossoverParameter& cross_para)
	{
		double rand;
		double y1, y2, yl, yu;
		double c1, c2;
		double alpha, beta, betaq;
		double eta_c = cross_para.index1;
		int dec_num = parent1->dec_.size();
		

		if (randomperc() <= cross_para.pro)
		{

			for (int i = 0; i < dec_num; i++)
			{
				if (randomperc() <= 0.5)
				{
					if (fabs(parent1->dec_[i] - parent2->dec_[i]) > 1e-9)
					{
						if (parent1->dec_[i] < parent2->dec_[i])
						{
							y1 = parent1->dec_[i];
							y2 = parent2->dec_[i];
						}
						else
						{
							y1 = parent2->dec_[i];
							y2 = parent1->dec_[i];
						}
						yl = lower_bound[i];
						yu = upper_bound[i];
						rand = randomperc();
						beta = 1.0 + (2.0 * (y1 - yl) / (y2 - y1));
						alpha = 2.0 - pow(beta, -(eta_c + 1.0));
						if (rand <= (1.0 / alpha))
						{
							betaq = pow((rand * alpha), (1.0 / (eta_c + 1.0)));
						}
						else
						{
							betaq = pow((1.0 / (2.0 - rand * alpha)), (1.0 / (eta_c + 1.0)));
						}
						c1 = 0.5 * ((y1 + y2) - betaq * (y2 - y1));
						beta = 1.0 + (2.0 * (yu - y2) / (y2 - y1));
						alpha = 2.0 - pow(beta, -(eta_c + 1.0));
						if (rand <= (1.0 / alpha))
						{
							betaq = pow((rand * alpha), (1.0 / (eta_c + 1.0)));
						}
						else
						{
							betaq = pow((1.0 / (2.0 - rand * alpha)), (1.0 / (eta_c + 1.0)));
						}
						c2 = 0.5 * ((y1 + y2) + betaq * (y2 - y1));
						if (c1 < yl)
							c1 = yl;
						if (c2 < yl)
							c2 = yl;
						if (c1 > yu)
							c1 = yu;
						if (c2 > yu)
							c2 = yu;
						if (randomperc() <= 0.5)
						{
							offspring1->dec_[i] = c2;
							offspring2->dec_[i] = c1;
						}
						else
						{
							offspring1->dec_[i] = c1;
							offspring2->dec_[i] = c2;
						}
					}
					else
					{
						offspring1->dec_[i] = parent1->dec_[i];
						offspring2->dec_[i] = parent2->dec_[i];
					}
				}
				else
				{
					offspring1->dec_[i] = parent1->dec_[i];
					offspring2->dec_[i] = parent2->dec_[i];
				}
			}
		}
		else
		{
			for (int i = 0; i < dec_num; i++)
			{
				offspring1->dec_[i] = parent1->dec_[i];
				offspring2->dec_[i] = parent2->dec_[i];
			}
		}
	}
}

