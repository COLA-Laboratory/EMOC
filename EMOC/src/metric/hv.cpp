#include "metric/hv.h"

#include "core/global.h"
#include "wfg/iwfg.h"

namespace emoc {

	double CalculateHV(Individual **pop, int pop_num)
	{
		double* ref;
		int i = 0, j = 0, num_same = 0;
		i_maxn = i_maxm = 0;

		i_maxm = pop_num; i_maxn = g_GlobalSettings->obj_num_;
		// memory allocation
		ref = (double *)malloc(sizeof(double) * g_GlobalSettings->obj_num_);
		i_fs = (FRONT *)malloc(sizeof(FRONT) * (10));
		for (i = 0; i < 10; i++)
		{
			i_fs[i].points = (POINT *)malloc(sizeof(POINT) * i_maxm);
			for (j = 0; j < i_maxm; j++)
				i_fs[i].points[j].objectives = (OBJECTIVE *)malloc(sizeof(OBJECTIVE) * (i_maxn - i - 1));
		}



		ref[0] = 1; ref[1] = 1;

		i_n = g_GlobalSettings->obj_num_;

		//    g_algorithm_entity.parent_population[0].obj[0] = 3;g_algorithm_entity.parent_population[0].obj[1] = 4;
		//    g_algorithm_entity.parent_population[1].obj[0] = 4;g_algorithm_entity.parent_population[1].obj[1] = 1;
		//    g_algorithm_entity.parent_population[2].obj[0] = 4;g_algorithm_entity.parent_population[2].obj[1] = 2;
		//    g_algorithm_entity.parent_population[3].obj[0] = 3.5;g_algorithm_entity.parent_population[3].obj[1] = 3.5;
		//    g_algorithm_entity.parent_population[4].obj[0] = 5;g_algorithm_entity.parent_population[4].obj[1] = 0.5;


		FRONT ps;
		ps.nPoints = pop_num;
		ps.points = (POINT*)malloc(sizeof(POINT) * ps.nPoints);

		for (i = 0; i < pop_num; i++)
		{
			ps.points[i].objectives = (OBJECTIVE *)malloc(sizeof(double) * i_n);
		}

		for (i = 0; i < pop_num; i++)
		{

			//        f->nPoints++;
			//        f->points = realloc (f->points, sizeof(POINT) * f->nPoints);
			//        f->n = 0;
			//        f->points[point].objectives = NULL;

			for (j = 0; j < g_GlobalSettings->obj_num_; j++)
			{
				//            ps.points[i].objectives[j] = g_algorithm_entity.parent_population[i].obj[j];
				ps.points[i].objectives[j] = ref[j] > g_GlobalSettings->parent_population_[i]->obj_[j] ? (ref[j] - g_GlobalSettings->parent_population_[i]->obj_[j]) : 0;
				//            printf("%d %d %f   ",i, j, ps.points[i].objectives[j]);
			}
			//        printf("\n");
		}
		//    printf("%f  %f\n",ps.points[0].objectives[0],ps.points[0].objectives[1]);

		double hv_value = i_hv(ps);



		//    double temp = 3;
		//    for(i = 1;i<g_algorithm_entity.algorithm_para.objective_number;i++)
		//        temp *= (g_algorithm_entity.algorithm_para.objective_number *2 + 1);

		//    printf("%f  %f\n",hv_value,temp);
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < i_maxm; j++)
				free(i_fs[i].points[j].objectives);
		}
		for (i = 0; i < 3; i++)
			free(i_fs[i].points);


		for (i = 0; i < pop_num; i++)
		{
			free(ps.points[i].objectives);

		}
		free(ps.points);
		free(ref); free(i_fs);
		return hv_value;
	}

}