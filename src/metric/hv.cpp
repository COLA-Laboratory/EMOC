#include "metric/hv.h"

#include <iostream>

#include "core/utility.h"
#include "core/global.h"
#include "wfg/iwfg.h"

namespace emoc {

	double CalculateHV(Individual **pop, int pop_num, int obj_num, std::string problem_name)
	{
		int num_same = 0;

		i_maxn = i_maxm = 0;
		i_maxm = pop_num; i_maxn = obj_num;

		// memory allocation
		i_fs = (FRONT *)malloc(sizeof(FRONT) * (20));
		for (int i = 0; i < 10; i++)
		{
			i_fs[i].points = (POINT *)malloc(sizeof(POINT) * i_maxm);
			for (int j = 0; j < i_maxm; j++)
				i_fs[i].points[j].objectives = (OBJECTIVE *)malloc(sizeof(OBJECTIVE) * (i_maxn - i - 1));
		}


		double* ref = (double *)malloc(sizeof(double) * obj_num);
		double* obj_min = (double *)malloc(sizeof(double) * obj_num);
		double* obj_max = (double *)malloc(sizeof(double) * obj_num);

		for (int i = 0; i < obj_num; ++i)
			ref[i] = 1.0;

		int pf_size = 0;
		double** pfdata = nullptr;
		pfdata = LoadPFData(pf_size, obj_num, problem_name);

		// get normalized bound from pf data
		for (int i = 0; i < obj_num; ++i)
		{
			double temp_min = EMOC_INF, temp_max = -EMOC_INF;
			for (int j = 0; j < pf_size; ++j)
			{
				if (temp_min > pfdata[j][i])
				{
					temp_min = pfdata[j][i];
				}

				if (temp_max < pfdata[j][i])
				{
					temp_max = pfdata[j][i];
				}
			}
			obj_min[i] = temp_min;
			obj_max[i] = temp_max;
		}

		// calculate hv
		i_n = obj_num;
		FRONT ps;
		ps.nPoints = pop_num;
		ps.points = (POINT*)malloc(sizeof(POINT) * ps.nPoints);

		for (int i = 0; i < pop_num; i++)
		{
			ps.points[i].objectives = (OBJECTIVE *)malloc(sizeof(double) * i_n);
		}

		for (int i = 0; i < pop_num; i++)
		{
			for (int j = 0; j < obj_num; j++)
			{
				double normalized_value = (pop[i]->obj_[j] - obj_min[j])/ (obj_max[j] - obj_min[j]) * 1.1;
				ps.points[i].objectives[j] = ref[j] > normalized_value ?
					(ref[j] - normalized_value) : 0;

			}
		}

		double hv_value = i_hv(ps);

		for (int i = 0; i < pf_size; ++i)
			delete[] pfdata[i];
		delete[] pfdata;

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
