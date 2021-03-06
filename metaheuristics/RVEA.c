/*
 * RVEA.c:
 *  This file implements the main procedures of RVEA. It is based on the following reference:
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
#include "../headers/utility.h"
#include "../headers/sort.h"
#include "../headers/population.h"
#include "../headers/analysis.h"
#include "../headers/problem.h"
#include "../headers/crossover.h"
#include "../headers/mutation.h"
#include "../headers/print.h"


static void RVEA_normalization(double **lambda)
{
    int i = 0, j = 0;
    double norm = 0;

    for(i = 0;i < weight_num;i++ )
    {
        norm = CalNorm(lambda[i],g_algorithm_entity.algorithm_para.objective_number);

        for(j = 0;j < g_algorithm_entity.algorithm_para.objective_number;j++)
        {
            lambda[i][j] = lambda[i][j]/norm;
        }
    }
}

static int RVEA_referBasedSelection(SMRT_individual *parent_table, SMRT_individual *mixedPop_table, double **lambda,
                                    int mixedPopNum, int maxGen, double alpha)
{
    int popCount = 0, i = 0, j = 0;
    double cosVal = 0, tempValue = 0;
    int M = g_algorithm_entity.algorithm_para.objective_number;

    int *index = NULL;                      //wait to release
    double *zmin = NULL;                    //wait to release
    double *gama = NULL;                    //wait to release
    double **popObj = NULL;                 //wait to release
    int **partitionRes = NULL;              //wait to release
    Angle_info_t **angleInfo = NULL;        //wait to release
    Distance_info_t **APDInfo = NULL;        //wait to release

    index = (int *)malloc(sizeof(int) * weight_num);
    for(i = 0; i < weight_num; i++)
    {
        index[i] = 0;
    }

    zmin = (double *)malloc(sizeof(double) * M);
    for(i = 0; i < M; i++)
    {
        zmin[i] = 100000000;
    }

    gama = (double *)malloc(sizeof(double) * weight_num);

    popObj = (double **)malloc(sizeof(double *) * mixedPopNum);
    for(i = 0; i < mixedPopNum; i++)
    {
        popObj[i] = (double*)malloc(sizeof(double) * M);
    }

    partitionRes = (int **)malloc(sizeof(int *) * weight_num);
    for(i = 0; i < weight_num; i++)
    {
        partitionRes[i] = (int*)malloc(sizeof(int) * mixedPopNum);
    }

    angleInfo = (Angle_info_t **)malloc(sizeof(Angle_info_t *) * mixedPopNum);
    for(i = 0; i < mixedPopNum; i++)
    {
        angleInfo[i] = (Angle_info_t *)malloc(sizeof(Angle_info_t) * weight_num);
    }

    APDInfo = (Distance_info_t **)malloc(sizeof(Distance_info_t *) * weight_num);
    for(i = 0; i < weight_num; i++)
    {
        APDInfo[i] = (Distance_info_t *)malloc(sizeof(Distance_info_t) * mixedPopNum);
    }

    for(i = 0; i < weight_num; i++)
    {
        for(j = 0; j < mixedPopNum; j++)
        {
            APDInfo[i][j].idx = -1;
            APDInfo[i][j].value = 10000000;
        }
    }

    /* Objective Value Translation */
    for(i = 0; i < mixedPopNum; i++)
    {
        for(j = 0; j < M; j++)
            if(zmin[j] > mixedPop_table[i].obj[j])
                zmin[j] = mixedPop_table[i].obj[j];
    }



    for(i = 0; i < mixedPopNum; i++)
    {
        for(j = 0; j < M; j++)
        {
            popObj[i][j] = mixedPop_table[i].obj[j] - zmin[j];
        }
    }

    /* Population Partition */
    for(i = 0; i < mixedPopNum; i++)
    {
        for(j = 0; j < weight_num; j++)
        {
            cosVal = CalDotProduct(popObj[i], lambda[j], M)/(CalNorm(popObj[i], M) * CalNorm(lambda[j], M));
            angleInfo[i][j].idx = j;
            angleInfo[i][j].value = cosVal;
        }
    }

    for(i = 0; i < mixedPopNum; i++)
    {
        angle_quick_sort(angleInfo[i], 0, weight_num - 1);
        int tempIndex = angleInfo[i][weight_num-1].idx;

        partitionRes[tempIndex][index[tempIndex]] = i;
        index[tempIndex] += 1;
    }

    /* Angle-Penalized Distance Calculation */
    for(i = 0; i < weight_num; i++)
    {
        gama[i] = 100000;
        for(j = 0; j < weight_num; j++)
        {
            if(i != j )
            {
                cosVal = CalDotProduct(lambda[i], lambda[j], M)/(CalNorm(lambda[i], M) * CalNorm(lambda[j], M));
                if(gama[i] > acos(cosVal))
                    gama[i] = acos(cosVal);
            }
        }
    }

    for(i = 0; i < weight_num; i++)
    {
        if(index[i] != 0)
        {
            for(j = 0; j < index[i]; j++)
            {
                cosVal = CalDotProduct(lambda[i], popObj[partitionRes[i][j]],M)/(CalNorm(popObj[partitionRes[i][j]], M) * CalNorm(lambda[i], M));
                tempValue = M * pow((double)g_algorithm_entity.iteration_number/maxGen, alpha) * (acos(cosVal)/gama[i]);
                APDInfo[i][j].idx = partitionRes[i][j];
                APDInfo[i][j].value = (1 + tempValue) * CalNorm(popObj[partitionRes[i][j]], M);
            }
        }
    }

    /* Elitism Selection */
    for(i = 0; i < weight_num; i++)
    {
        if(index[i]!=0)
        {
            distance_quick_sort(APDInfo[i], 0, index[i]-1);
            copy_individual(mixedPop_table+APDInfo[i][0].idx, parent_table + popCount);
            popCount++;
        }
    }

    free(index);
    free(zmin);
    free(gama);

    for(i = 0;i < mixedPopNum;i++)
    {
        free(popObj[i]);
        free(angleInfo[i]);
    }
    free(popObj);
    free(angleInfo);

    for(i = 0; i < weight_num; i++)
    {
        free(partitionRes[i]);
        free(APDInfo[i]);
    }
    free(partitionRes);
    free(APDInfo);

    return popCount;
}

static void RVEA_referAdaptation(double **lambda, SMRT_individual *pop_table)
{
    int i = 0, j = 0;

    update_ideal_point(pop_table, weight_num);
    update_nadir_point(pop_table,weight_num);

    for(i = 0; i < weight_num; i++)
    {
        for(j = 0; j < g_algorithm_entity.algorithm_para.objective_number; j++)
        {
            lambda[i][j] = lambda[i][j] * (g_algorithm_entity.nadir_point.obj[j] - g_algorithm_entity.ideal_point.obj[j]);
        }
    }

    RVEA_normalization(lambda);

    return;
}

extern void _RVEA_ (SMRT_individual *parent_pop, SMRT_individual *offspring_pop, SMRT_individual *mixed_pop)
{

    int i = 0, j = 0, currentPopNum = 0,maxGen = 0;

    //parameter
    double alpha = 2.0;
    double fr = 0.1;
    double **originLambda = NULL;

    g_algorithm_entity.iteration_number = 0;
    g_algorithm_entity.algorithm_para.current_evaluation = 0;

    lambda = initialize_uniform_point(g_algorithm_entity.algorithm_para.pop_size, &weight_num);
    currentPopNum = weight_num;
    maxGen = g_algorithm_entity.algorithm_para.max_evaluation / weight_num;

    originLambda = (double **)malloc(sizeof(double *) * weight_num);
    for(i = 0; i < weight_num; i++)
    {
        originLambda[i] = (double* )malloc(sizeof(double) * g_algorithm_entity.algorithm_para.objective_number);
    }

    for(i = 0; i < weight_num; i++)
    {
        for(j = 0; j < g_algorithm_entity.algorithm_para.objective_number; j++)
        {
            originLambda[i][j] = lambda[i][j];
        }
    }

    initialize_population_real(parent_pop, weight_num);
    evaluate_population(parent_pop, weight_num);

    RVEA_normalization(lambda);

    track_evolution (parent_pop, g_algorithm_entity.iteration_number, 0);
    while(g_algorithm_entity.algorithm_para.current_evaluation < g_algorithm_entity.algorithm_para.max_evaluation)
    {
        print_progress();
        g_algorithm_entity.iteration_number++;

        RVEA_crossover_operator(parent_pop, offspring_pop, currentPopNum);
        mutation_pop(offspring_pop);
        evaluate_population(offspring_pop, (currentPopNum / 2) * 2);

        merge_population(mixed_pop, parent_pop, currentPopNum, offspring_pop, (currentPopNum / 2) * 2);

        currentPopNum = RVEA_referBasedSelection(parent_pop, mixed_pop, lambda, currentPopNum + (currentPopNum / 2) * 2,
                                                 maxGen, alpha);

        if((g_algorithm_entity.iteration_number % (int)(fr * maxGen)) == 0)
            RVEA_referAdaptation(originLambda, parent_pop);

        track_evolution (parent_pop, g_algorithm_entity.iteration_number, g_algorithm_entity.algorithm_para.current_evaluation >= g_algorithm_entity.algorithm_para.max_evaluation);
    }


    return ;
}