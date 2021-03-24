#include "core/utility.h"

#include <cmath>

namespace emoc {

	DominateReleation CheckDominance(Individual *ind1, Individual *ind2, int obj_num)
	{
		int flag1 = 0, flag2 = 0;

		for (int i = 0; i < obj_num; ++i)
		{
			if (ind1->obj_[i] < ind2->obj_[i])
				flag1 = 1;
			else
			{
				if (ind1->obj_[i] > ind2->obj_[i])
					flag2 = 1;
			}
		}

		if (flag1 == 1 && flag2 == 0)
			return (DOMINATE);
		else
		{
			if (flag1 == 0 && flag2 == 1)
				return (DOMINATED);
			else
				return (NON_DOMINATED);
		}
	}

	int WeaklyDominates(double *point1, double *point2, int obj_num)
	{
		int i = 0, better = 1;
		while (i < obj_num && better)
		{
			better = point1[i] <= point2[i];
			i++;
		}
		return better;
	}

	double CalEuclidianDistance(double *a, double *b, int dimension)
	{
		double distance = 0.0;
		for (int i = 0; i < dimension; i++)
			distance += (a[i] - b[i]) * (a[i] - b[i]);
		return sqrt(distance);
	}

	double CalPerpendicularDistance(double *a, double *weight, int dimension)
	{
		double sin = CalculateSin(a, weight, dimension);
		double d2 = CalculateNorm(a, dimension);
		d2 = d2 * sin;

		return d2;
	}

	int Combination(int n, int k)
	{
		if (n < k)
			return -1;

		double ans = 1;
		for (int i = k + 1; i <= n; i++)
		{
			ans = ans * i;
			ans = ans / (double)(i - k);
		}

		return (int)ans;
	}

	double CalculateDotProduct(double *vector1, double *vector2, int dimension)
	{
		double dot_product = 0;
		for (int i = 0; i < dimension; i++)
			dot_product += vector1[i] * vector2[i];

		return dot_product;
	}

	double CalculateCos(double *a, double *b, int dimension)
	{
		return CalculateDotProduct(a, b, dimension) / (CalculateNorm(a, dimension) * CalculateNorm(b, dimension));
	}

	double CalculateSin(double *a, double *b, int dimension)
	{
		double cos = CalculateCos(a, b, dimension);
		return sqrt(1 - pow(cos, 2.0));
	}

	double CalculateNorm(double *vector, int dimension)
	{
		double norm = 0;
		for (int i = 0; i < dimension; i++)
		{
			norm += (vector[i] * vector[i]);
		}

		return sqrt(norm);
	}

	void UpdateIdealpoint(Individual *ind, double *ideal_point, int obj_num)
	{
		for (int i = 0; i < obj_num; i++)
		{
			if (ind->obj_[i] < ideal_point[i])
				ideal_point[i] = ind->obj_[i];
		}
	}

	void UpdateIdealpoint(Individual **pop, int pop_num, double *ideal_point, int obj_num)
	{
		for (int i = 0; i < obj_num; ++i)
			ideal_point[i] = INF;

		for (int i = 0; i < pop_num; i++)
		{
			for (int j = 0; j < obj_num; j++)
			{
				if (pop[i]->obj_[j] < ideal_point[j])
					ideal_point[j] = pop[i]->obj_[j];
			}
		}
	}

	void UpdateNadirpoint(Individual *ind, double *nadir_point, int obj_num)
	{
		for (int i = 0; i < obj_num; i++)
		{
			if (ind->obj_[i] > nadir_point[i])
				nadir_point[i] = ind->obj_[i];
		}
	}

	void UpdateNadirpoint(Individual **pop, int pop_num, double *nadir_point, int obj_num)
	{
		for (int i = 0; i < obj_num; ++i)
			nadir_point[i] = -INF;

		for (int i = 0; i < pop_num; i++)
		{
			for (int j = 0; j < obj_num; j++)
			{
				if (pop[i]->obj_[j] > nadir_point[j])
					nadir_point[j] = pop[i]->obj_[j];
			}
		}
	}

	double CalWeightedSum(Individual *ind, double *weight_vector, double *ideal_point, int obj_num)
	{
		double fitness = 0;
		for (int i = 0; i < obj_num; i++)
		{
			fitness += ind->obj_[i] * weight_vector[i];
		}

		ind->fitness_ = fitness;

		return fitness;
	}

	double CalInverseChebycheff(Individual *ind, double *weight_vector, double *ideal_point, int obj_num)
	{
		double fitness = 0, max = -1.0e+20;

		for (int i = 0; i < obj_num; ++i)
		{
			double diff = fabs(ind->obj_[i] - ideal_point[i]);
			if (weight_vector[i] < EPS)
				fitness = diff / 0.000001;
			else
				fitness = diff / weight_vector[i];

			if (fitness > max)
				max = fitness;
		}

		fitness = max;
		ind->fitness_ = fitness;

		return fitness;
	}

	double CalPBI(Individual *ind, double *weight_vector, double *ideal_point, int obj_num, double theta)
	{
		theta == 0.0 ? 5.0 : theta;
		double d1 = 0.0, d2 = 0.0, nl = 0.0;

		for (int i = 0; i < obj_num; ++i)
		{
			d1 += (ind->obj_[i] - ideal_point[i]) * weight_vector[i];
			nl += weight_vector[i] * weight_vector[i];
		}
		nl = sqrt(nl);
		d1 = fabs(d1) / nl;

		for (int i = 0; i < obj_num; ++i)
			d2 += ((ind->obj_[i] - ideal_point[i]) - d1 * (weight_vector[i] / nl)) * ((ind->obj_[i] - ideal_point[i]) - d1 * (weight_vector[i] / nl));
		d2 = sqrt(d2);

		ind->fitness_ = d1 + theta * d2;
		return  (d1 + theta * d2);
	}



}