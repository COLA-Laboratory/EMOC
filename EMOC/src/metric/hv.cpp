#include "metric/hv.h"

#include "core/global.h"
#include "wfg/iwfg.h"

namespace emoc {

	double CalculateHV(Individual **pop, int pop_num)
	{
		int num_same = 0;

		i_maxn = i_maxm = 0;
		i_maxm = pop_num; i_maxn = g_GlobalSettings->obj_num_;

		// memory allocation
		i_fs = (FRONT *)malloc(sizeof(FRONT) * (10));
		for (int i = 0; i < 10; i++)
		{
			i_fs[i].points = (POINT *)malloc(sizeof(POINT) * i_maxm);
			for (int j = 0; j < i_maxm; j++)
				i_fs[i].points[j].objectives = (OBJECTIVE *)malloc(sizeof(OBJECTIVE) * (i_maxn - i - 1));
		}


		double* ref = (double *)malloc(sizeof(double) * g_GlobalSettings->obj_num_);
		double* obj_min = (double *)malloc(sizeof(double) * g_GlobalSettings->obj_num_);
		double* obj_max = (double *)malloc(sizeof(double) * g_GlobalSettings->obj_num_);

		for (int i = 0; i < g_GlobalSettings->obj_num_; ++i)
			ref[i] = 1.0;

		for (int i = 0; i < g_GlobalSettings->obj_num_; ++i)
		{
			double temp_min = INF, temp_max = -INF;
			for (int j = 0; j < g_GlobalSettings->population_num_; ++j)
			{
				if (temp_min > g_GlobalSettings->parent_population_[j]->obj_[i])
				{
					temp_min = g_GlobalSettings->parent_population_[j]->obj_[i];
				}

				if (temp_max < g_GlobalSettings->parent_population_[j]->obj_[i])
				{
					temp_max = g_GlobalSettings->parent_population_[j]->obj_[i];
				}
			}
			obj_min[i] = temp_min;
			obj_max[i] = temp_max;
		}

		// calculate hv
		i_n = g_GlobalSettings->obj_num_;
		FRONT ps;
		ps.nPoints = pop_num;
		ps.points = (POINT*)malloc(sizeof(POINT) * ps.nPoints);

		for (int i = 0; i < pop_num; i++)
		{
			ps.points[i].objectives = (OBJECTIVE *)malloc(sizeof(double) * i_n);
		}

		for (int i = 0; i < pop_num; i++)
		{
			for (int j = 0; j < g_GlobalSettings->obj_num_; j++)
			{
				double normalized_value = g_GlobalSettings->parent_population_[i]->obj_[j] / (obj_max[j] - obj_min[j]);
				ps.points[i].objectives[j] = ref[j] > normalized_value ?
					(ref[j] - normalized_value) : 0;
			}
		}

		double hv_value = i_hv(ps);


		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < i_maxm; j++)
				free(i_fs[i].points[j].objectives);
		}

		for (int i = 0; i < 3; i++)
			free(i_fs[i].points);

		for (int i = 0; i < pop_num; i++)
		{
			free(ps.points[i].objectives);

		}
		free(ps.points);
		free(ref); free(i_fs); free(obj_max); free(obj_min);
		return hv_value;
	}

}