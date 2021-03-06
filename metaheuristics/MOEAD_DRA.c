/*
 * MOEAD_DRA.c:
 *  This file implements the main procedures of MOEAD_DRA. It is based on the following reference:
 *
 *  M. Wagner and F. Neumann, "A fast approximation-guided evolutionary multi-objective algorithm".
 *  Annual Conference on Genetic and Evolutionary Computation. 687-694, 2013.
 *
 * Authors:
 *  Peili Mao
 *  Lei Sun
 *  Longfei Zhang
 *  Ke Li <k.li@exeter.ac.uk>
 *  Xinyu Shan
 *  Renzhi Chen
 *
 * Institution:
 *  Computational Optimization for Learning and Adaptive System (COLA) Laboratory @ University of Exeter
 *
 * Copyright (c) 2019 Peili Mao, Lei Sun, Longfei Zhang ,Ke Li
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>;.
 */
#include "../headers/global.h"
#include "../headers/metaheuristics.h"
#include "../headers/crossover.h"
#include "../headers/mutation.h"
#include "../headers/problem.h"
#include "../headers/print.h"
#include "../headers/utility.h"
#include "../headers/sort.h"
#include "../headers/selection.h"
#include "../headers/random.h"
#include "../headers/analysis.h"

static void MOEAD_dra_free()
{
    int i = 0;

    if (NULL != g_algorithm_entity.MOEAD_para.delta)
    {
        free(g_algorithm_entity.MOEAD_para.delta);
    }
    if (NULL != g_algorithm_entity.MOEAD_para.utility)
    {
        free(g_algorithm_entity.MOEAD_para.utility);
    }
    if (NULL != g_algorithm_entity.MOEAD_para.old_function)
    {
        free(g_algorithm_entity.MOEAD_para.old_function);
    }
    if (NULL != g_algorithm_entity.MOEAD_para.frequency)
    {
        free(g_algorithm_entity.MOEAD_para.frequency);
    }

    for (i = 0; i < weight_num; ++i)
    {
        if (NULL != g_algorithm_entity.MOEAD_para.neighbor_table[i].neighbor)
        {
            free(g_algorithm_entity.MOEAD_para.neighbor_table[i].neighbor);
        }
    }
    if (NULL != g_algorithm_entity.MOEAD_para.neighbor_table)
    {
        free(g_algorithm_entity.MOEAD_para.neighbor_table);
    }

    for (i = 0; i < weight_num; i++)
        free (lambda[i]);
    free (lambda);

    return;
}

static void ini_MOEAD_dra()
{
    int i = 0, j = 0, k = 0;
    double difference = 0, distance_temp = 0, Euc_distance = 0;
    Distance_info_t sort_list[MAX_SIZE];

    lambda = initialize_uniform_point (g_algorithm_entity.algorithm_para.pop_size, &weight_num);

    g_algorithm_entity.MOEAD_para.neighbor_table = (MOEAD_NEIGHBOR*)malloc(sizeof(MOEAD_NEIGHBOR) * weight_num);
    if(NULL == g_algorithm_entity.MOEAD_para.neighbor_table)
    {
        printf("In the state of initiate parameter malloc neighbor table Fail\n");
        return;
    }
    g_algorithm_entity.MOEAD_para.utility = (double *)malloc(sizeof(double) * weight_num);
    if (NULL == g_algorithm_entity.MOEAD_para.utility)
    {
        printf("In the state of initiate parameter malloc utility Fail\n");
        return;
    }

    g_algorithm_entity.MOEAD_para.delta = (double *)malloc(sizeof(double) * weight_num);
    if (NULL == g_algorithm_entity.MOEAD_para.delta)
    {
        printf("In the state of initiate parameter malloc frequency Fail\n");
        return;
    }

    g_algorithm_entity.MOEAD_para.old_function = (double *)malloc(sizeof(double) * weight_num);
    if (NULL == g_algorithm_entity.MOEAD_para.old_function)
    {
        printf("In the state of initiate parameter malloc frequency Fail\n");
        return;
    }
    g_algorithm_entity.MOEAD_para.frequency = (int *)malloc(sizeof(int) * weight_num);
    if (NULL == g_algorithm_entity.MOEAD_para.old_function)
    {
        printf("In the state of initiate parameter malloc frequency Fail\n");
        return;
    }

    for (i = 0; i < weight_num; i++)
    {
        for (j = 0; j < weight_num; j++)
        {
            distance_temp = 0;
            for (k = 0; k < g_algorithm_entity.algorithm_para.objective_number; k++)
            {
                difference = fabs(lambda[i][k] -  lambda[j][k]);
                distance_temp += (double)difference * difference;
            }

            Euc_distance = sqrt((double)distance_temp);
            sort_list[j].value = Euc_distance;
            sort_list[j].idx = j;
        }
        distance_quick_sort(sort_list, 0, weight_num - 1);

        g_algorithm_entity.MOEAD_para.neighbor_table[i].neighbor = (int *)malloc(sizeof(int) * g_algorithm_entity.MOEAD_para.neighbor_size);
        if(NULL == g_algorithm_entity.MOEAD_para.neighbor_table[i].neighbor)
        {
            printf("In the state of initiate parameter malloc weight neighbor Fail\n");
            return ;
        }

        for (j = 0; j < g_algorithm_entity.MOEAD_para.neighbor_size; j++)
        {
            g_algorithm_entity.MOEAD_para.neighbor_table[i].neighbor[j] = sort_list[j].idx;
        }
    }

    for (i = 0; i < weight_num; i++)
    {
        g_algorithm_entity.MOEAD_para.delta[i] = 0;
        g_algorithm_entity.MOEAD_para.utility[i] = 1.0;
        g_algorithm_entity.MOEAD_para.old_function[i] = 0;
        g_algorithm_entity.MOEAD_para.frequency[i] = 0;
    }

    return ;
}

extern void _MOEAD_DRA_(SMRT_individual *pop, SMRT_individual *offspring_pop, SMRT_individual *mixed_pop) {

    int i, j;
    SMRT_individual *offspring, *parent;
    NeighborType type;
    double rand = 0;
    int *selected, selected_size = g_algorithm_entity.algorithm_para.pop_size / 5;

    g_algorithm_entity.iteration_number = 1;
    g_algorithm_entity.algorithm_para.current_evaluation = 0;
    printf("|\tThe %d run\t|\t1%%\t|", g_algorithm_entity.run_index_current);

    // initialization process
    ini_MOEAD_dra();

    if (g_algorithm_entity.algorithm_para.pop_size < weight_num || selected_size > weight_num)
    {
        printf("must set pop size bigger than weightnum,current weight num is :%d\n", weight_num);
        return;
    }
    initialize_population_real(pop, weight_num);

    evaluate_population(pop, weight_num);

    initialize_idealpoint(pop, weight_num, &g_algorithm_entity.ideal_point);

    selected = (double *) malloc(sizeof(double) * weight_num);
    if (NULL == selected)
    {
        printf("In the MOEAD_dra_framework malloc candidate\n");
        return;
    }

    for (i = 0; i < weight_num; ++i)
    {
        g_algorithm_entity.MOEAD_para.old_function[i] = cal_moead_fitness(pop + i, lambda[i], g_algorithm_entity.MOEAD_para.function_type);
    }

    track_evolution (pop, g_algorithm_entity.iteration_number, 0);

    while (g_algorithm_entity.algorithm_para.current_evaluation < g_algorithm_entity.algorithm_para.max_evaluation)
    {
        //create empty head for selected and candidate

        print_progress ();

        //select the current most active subproblems to evolve (based on utility)
        tour_selection_subproblem (selected, weight_num);

        for (i = 0; i < selected_size; i++)
        {
            j = selected[i];
            g_algorithm_entity.MOEAD_para.frequency[j]++;
            offspring = offspring_pop + i;
            parent = pop + j;

            rand = randomperc();
            if (rand < g_algorithm_entity.MOEAD_para.neighborhood_selection_probability)
            {
                type = NEIGHBOR;
            }
            else
            {
                type = GLOBAL_PARENT;
            }

            // crossover and mutation
            crossover_MOEAD (pop, parent, j, offspring, type);
            mutation_ind(offspring);
            evaluate_individual (offspring);

            update_ideal_point_by_ind (offspring);

            // update the subproblem
            update_subproblem(offspring, j, type);
        }

        g_algorithm_entity.iteration_number++;

        if (g_algorithm_entity.iteration_number % 30 == 0)
        {
            comp_utility ();

            for (i = 0; i < weight_num; ++i)
            {
                g_algorithm_entity.MOEAD_para.delta[i] = fabs(g_algorithm_entity.parent_population[i].fitness - g_algorithm_entity.MOEAD_para.old_function[i]) / g_algorithm_entity.MOEAD_para.old_function[i];
                g_algorithm_entity.MOEAD_para.old_function[i] = g_algorithm_entity.parent_population[i].fitness;

            }
        }

        track_evolution (pop, g_algorithm_entity.iteration_number, g_algorithm_entity.algorithm_para.current_evaluation >= g_algorithm_entity.algorithm_para.max_evaluation);
    }

    free(selected);

    MOEAD_dra_free();

    return;
}


