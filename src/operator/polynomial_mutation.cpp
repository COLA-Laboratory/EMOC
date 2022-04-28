#include "operator/polynomial_mutation.h"

#include <cmath>

#include "random/random.h"

namespace emoc {

	void PolynomialMutation(Individual* ind, std::vector<double>& lower_bound, std::vector<double>& upper_bound, MutationParameter& mutation_para)
	{
		int dec_num = ind->dec_.size();
		double rnd, delta1, delta2, mut_pow, deltaq;
		double y, yl, yu, val, xy;

		double mutation_pro = mutation_para.pro;
		double eta_m = mutation_para.index1;
		for (int i = 0; i < dec_num; i++)
		{
			yl = lower_bound[i];
			yu = upper_bound[i];
			y = ind->dec_[i];
			if (randomperc() <= mutation_pro)
			{
				delta1 = (y - yl) / (yu - yl);
				delta2 = (yu - y) / (yu - yl);
				rnd = randomperc();
				mut_pow = 1.0 / (eta_m + 1.0);
				if (rnd <= 0.5)
				{
					xy = 1.0 - delta1;
					val = 2.0 * rnd + (1.0 - 2.0 * rnd) * (pow(xy, (eta_m + 1.0)));
					deltaq = pow(val, mut_pow) - 1.0;
				}
				else
				{
					xy = 1.0 - delta2;
					val = 2.0 * (1.0 - rnd) + 2.0 * (rnd - 0.5) * (pow(xy, (eta_m + 1.0)));
					deltaq = 1.0 - (pow(val, mut_pow));
				}
				y = y + deltaq * (yu - yl);
			}
			if (y < yl)
				y = yl;
			if (y > yu)
				y = yu;
			ind->dec_[i] = y;
		}
	}


	void PolynomialMutation(Individual** pop, int pop_num, std::vector<double>& lower_bound, std::vector<double>& upper_bound, MutationParameter& mutation_para)
	{
		for (int i = 0; i < pop_num; ++i)
			PolynomialMutation(pop[i],lower_bound,upper_bound, mutation_para);
	}

}