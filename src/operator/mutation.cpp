#include "operator/mutation.h"

#include <cmath>

#include "random/random.h"

namespace emoc {

	void PolynomialMutation(Individual *ind, Global *g_GlobalSettings)
	{
		double rnd, delta1, delta2, mut_pow, deltaq;
		double y, yl, yu, val, xy;

		double mutation_pro = g_GlobalSettings->pm_parameter_.muatation_pro;
		double eta_m = g_GlobalSettings->pm_parameter_.eta_m;
		for (int i = 0; i < g_GlobalSettings->dec_num_; i++)
		{
			yl = g_GlobalSettings->dec_lower_bound_[i];
			yu = g_GlobalSettings->dec_upper_bound_[i];
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


	void PolynomialMutation(Individual **pop, int pop_num, Global *g_GlobalSettings)
	{
		for (int i = 0; i < pop_num; ++i)
			PolynomialMutation(pop[i], g_GlobalSettings);
	}

}