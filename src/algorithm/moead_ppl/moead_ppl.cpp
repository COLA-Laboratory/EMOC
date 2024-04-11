// Algorithm: I-MOEA/D-PPL
// Paper: Huang, Tian, and Ke Li. "Preference-based multi-objective optimization with gaussian process." 2023 IEEE International Conference on Systems, Man, and Cybernetics (SMC). IEEE, 2023.
// Link: https://ieeexplore.ieee.org/abstract/document/10394212
// Created by Tian Huang 11/04/2024
// preference learning with gaussian process in folder gp_pref_elicit

#include "algorithm/moead_ppl/moead_ppl.h"

#include <Python.h>
#include <cmath>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <cstdlib>

#include "core/global.h"
#include "core/utility.h"
#include "core/uniform_point.h"
#include "core/emoc_manager.h"
#include "core/file.h"
#include "operator/sbx.h"
#include "operator/polynomial_mutation.h"
#include "random/random.h"
#include "../../vendor/numpy/arrayobject.h"
#include <object.h>
#include <vector>
using namespace std;
#include <numeric>  //iota
#include <string.h> //memset


namespace emoc{

    MOEADPPL :: MOEADPPL(int thread_id) :
    Algorithm(thread_id),
	lambda_(nullptr),
	weight_num_(0),
	neighbour_(nullptr),
	ideal_point_(new double[g_GlobalSettings->obj_num_]), 
	aggregation_type_(0),
    neighbour_selectpro_(0.9),
	pbi_theta_(5.0),
    replace_num_(2),
    nPromisingWeight(10),
    step_size_(0.3),
    weight(new double[g_GlobalSettings->obj_num_])
    {

    }

    MOEADPPL::~MOEADPPL()
	{
		for (int i = 0; i < weight_num_; ++i)
		{
			delete[] lambda_[i];		
			lambda_[i] = nullptr;
            delete[] neighbour_[i];
            neighbour_[i]=nullptr;
		}
        
		delete[] lambda_;
		delete[] neighbour_;
		delete[] ideal_point_;
        delete[] weight;
		lambda_ = nullptr;
		neighbour_ = nullptr;
		ideal_point_ = nullptr;
        weight = nullptr;
	}

    void MOEADPPL::Solve()
    {
        Initialization();
        Individual *offspring=g_GlobalSettings->offspring_population_[0];
        first_tau_ = 100;
        // tau_ = g_GlobalSettings->tau_;
        tau_ = 25;
        while(!IsTermination())
        {   
            if((g_GlobalSettings->iteration_num_ >= first_tau_) && (g_GlobalSettings->iteration_num_% tau_==0) && (g_GlobalSettings->iteration_num_ > 0) && tau_ && first_tau_)
            {
                Consult_DM();
            }
            UpdateNeighbour();
            for(int i=0;i<weight_num_;i++)
            {   
                if(randomperc()<neighbour_selectpro_)
                    neighbour_type_=NEIGHBOUR;
                else
                    neighbour_type_=GLOBAL;

                Crossover(g_GlobalSettings->parent_population_.data(),i,offspring);
                PolynomialMutation(offspring,g_GlobalSettings->dec_lower_bound_,g_GlobalSettings->dec_upper_bound_,mutation_para_);
                EvaluateInd(offspring);

                UpdateIdealpoint(offspring,ideal_point_,g_GlobalSettings->obj_num_);
                UpdateSubproblem(offspring,i,aggregation_type_);
            }
        }
        
        // CalculateMinDis(weight);
    }

    void MOEADPPL::Initialization()
    {
        weight_num_ = g_GlobalSettings->population_num_;
        
        g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(),g_GlobalSettings->population_num_);
        EvaluatePop(g_GlobalSettings->parent_population_.data(),g_GlobalSettings->population_num_);
       

        // char file[256];
        // sprintf(file,"./UniformWeights/%dd_%d.txt",g_GlobalSettings->obj_num_,g_GlobalSettings->population_num_);
        // lambda_ = LoadUniformWeights(g_GlobalSettings->population_num_,g_GlobalSettings->obj_num_,file);
        lambda_ = UniformPoint(g_GlobalSettings->population_num_,&weight_num_,g_GlobalSettings->obj_num_);
        real_popnum_ = weight_num_;


        SetNeighbours();

        // weight = SetWeight(g_GlobalSettings->weight_StringType_);
        weight = SetWeight(g_GlobalSettings->obj_num_, g_GlobalSettings->algorithm_name_);

        UpdateIdealpoint(g_GlobalSettings->parent_population_.data(),weight_num_,ideal_point_,g_GlobalSettings->obj_num_);

        mutation_para_.pro = 1.0/g_GlobalSettings->dec_num_;
        mutation_para_.index1 = 20.0;

        cross_para_.pro = 1.0;
        cross_para_.index1 = 20.0;

    }

    void MOEADPPL::SetNeighbours()
    {
        neighbour_num_ = weight_num_/10;
        neighbour_ = new int*[weight_num_];
        for(int i = 0; i < weight_num_; ++i)
        {
            neighbour_[i] = new int[neighbour_num_];
        }

        std::vector<DistanceInfo>sort_list(weight_num_);
        for(int i = 0; i < weight_num_; ++i)
        {
            for(int j = 0; j < weight_num_; ++j)
            {
                double distance_temp = 0;
                for(int k = 0; k < g_GlobalSettings->obj_num_; k++)
                {
                    distance_temp += (lambda_[i][k]-lambda_[j][k])*(lambda_[i][k]-lambda_[j][k]);
                }
                sort_list[i].distance = sqrt(distance_temp);
                sort_list[j].index = j;
            }
            std::sort(sort_list.begin(),sort_list.end(),[](const DistanceInfo &left,const DistanceInfo &right){
                return left.distance < right.distance;
            });
            for(int j = 0; j < neighbour_num_; ++j)
            {
                neighbour_[i][j] = sort_list[j+1].index;
            }
        }
    }

    void MOEADPPL::Crossover(Individual **parent_pop,int current_index,Individual *offspring)
    {
        int size=neighbour_type_ == NEIGHBOUR ? neighbour_num_ : weight_num_;
        int parent1_index = 0,parent2_index = 0;

        if(neighbour_type_ == NEIGHBOUR)
        {
            int k = rnd(0,size-1);
            int l = rnd(0,size-1);
            parent1_index = neighbour_[current_index][k];
            parent2_index = neighbour_[current_index][l];
        }
        else
        {
            parent1_index = rnd(0,size-1);
            parent2_index = rnd(0,size-1);
        }
        Individual *parent1=parent_pop[parent1_index];
        Individual *parent2=parent_pop[parent2_index];

        SBX(parent1,parent2,g_GlobalSettings->offspring_population_[1],offspring,
        g_GlobalSettings->dec_lower_bound_,g_GlobalSettings->dec_upper_bound_,cross_para_);

    }

    void MOEADPPL::UpdateSubproblem(Individual *offspring,int current_index,int aggregration_type)
    {
        int size = neighbour_type_ == NEIGHBOUR ? neighbour_num_ : weight_num_;
		std::vector<int> perm_index(size);
		random_permutation(perm_index.data(), size);

		int count = 0, weight_index = 0;
		double offspring_fitness = 0.0;
		double neighbour_fitness = 0.0;

		// calculate fitness and update subproblem;
		for (int i = 0; i < size; ++i)
		{
			if (count >= replace_num_)
				break;
			
			if (neighbour_type_ == NEIGHBOUR)
            {
                // std::cout<<i<<":"<<current_index<<"  "<<perm_index[i]<<endl;
                weight_index = neighbour_[current_index][perm_index[i]];
            }
			else
				weight_index = perm_index[i];

			Individual *current_ind = g_GlobalSettings->parent_population_[weight_index];

            switch(aggregation_type_)
            {
                case 0:
                    offspring_fitness = CalInverseChebycheff(offspring, lambda_[weight_index], ideal_point_, g_GlobalSettings->obj_num_);
			        neighbour_fitness = CalInverseChebycheff(current_ind, lambda_[weight_index], ideal_point_, g_GlobalSettings->obj_num_);
                    break;
                
                case 1:
                    offspring_fitness=CalWeightedSum(current_ind,lambda_[weight_index],ideal_point_,g_GlobalSettings->obj_num_);
                    neighbour_fitness=CalWeightedSum(offspring,lambda_[weight_index],ideal_point_,g_GlobalSettings->obj_num_);
                    break;
                case2:
                    offspring_fitness=CalPBI(current_ind,lambda_[weight_index],ideal_point_,g_GlobalSettings->obj_num_,pbi_theta_);
                    neighbour_fitness=CalPBI(offspring,lambda_[weight_index],ideal_point_,g_GlobalSettings->obj_num_,pbi_theta_);
                    break;
                default:
                    break;
            }
			
			if (offspring_fitness < neighbour_fitness)
			{
				CopyIndividual(offspring, g_GlobalSettings->parent_population_[weight_index]);
				count++;
			}
		}
    }

    void MOEADPPL::UpdateNeighbour()
    {
        DistanceInfo *sort_list=new DistanceInfo[weight_num_];
        // std::vector<DistanceInfo>sort_list(weight_num_);
        for(int i=0;i<weight_num_;++i)
        {
            for(int j=0;j<weight_num_;++j)
            {
                double distance_temp=0;
                for(int k=0;k<g_GlobalSettings->obj_num_;++k)
                {
                    distance_temp+=(lambda_[i][k]-lambda_[j][k])*(lambda_[i][k]-lambda_[j][k]);
                }
                sort_list[j].distance=sqrt(distance_temp);
                sort_list[j].index=j;
            }
            std::sort(sort_list,sort_list+weight_num_,[](const DistanceInfo &left, const DistanceInfo &right){
                return left.distance<right.distance;
            });

           for(int j=0;j<neighbour_num_;j++)
           {
            neighbour_[i][j]=sort_list[j+1].index;
           }


        }
        delete[] sort_list;
    }

    void MOEADPPL::Consult_DM()
    {
        Py_Initialize();
        if(!Py_IsInitialized())
        {
            printf("Python init failed!\n");
        }

        PyRun_SimpleString("import sys");
        PyRun_SimpleString("sys.path.append('./')");
        PyRun_SimpleString("sys.path.append('../')");

        // PyRun_SimpleString("from time import time, ctime \n"
        //                     "print('today is',ctime(time()))\n");

        PyObject* pModule=NULL;
        PyObject* pFunc=NULL;
        PyObject *currPop_list = NULL;

        pModule=PyImport_ImportModule("gpforranking");

        if(!pModule)
        {
            printf("Can not open python file!\n");
        }
        
        pFunc=PyObject_GetAttrString(pModule,"ranking");

        int obj_num=g_GlobalSettings->obj_num_;
        int seed = 60;
       
        
        currPop_list = PyList_New(0);
        RecordCurrentPop(currPop_list);

        PyObject* pyParams=PyTuple_New(5);
        PyTuple_SetItem(pyParams,0,Py_BuildValue("i",obj_num));           //obj_num
        PyTuple_SetItem(pyParams,1,Py_BuildValue("i",seed));          //seed
        PyTuple_SetItem(pyParams,2,Py_BuildValue("s","ranking"));   //query_type
        PyTuple_SetItem(pyParams,3,Py_BuildValue("i",20));          //query_num
        PyTuple_SetItem(pyParams,4,currPop_list);

        PyObject *pRetValue=PyObject_CallObject(pFunc,pyParams);
        
        PyArrayObject *r1,*r2,*r3,*r4,*r5,*r6,*r7,*r8,*r9;

        PyArg_UnpackTuple(pRetValue,"ref",9,10,&r1,&r2,&r3,&r4,&r5,&r6,&r7,&r8,&r9);

        npy_intp *shape8=PyArray_SHAPE(r8);     //return the best vector
        // npy_intp *shape9=PyArray_SHAPE(r9);     //recommend 10 best 

        double *best=(double*)PyArray_DATA(r8);   


        //TODO:set biased weight
        SetBiasedWeight(best);
               

        Py_DECREF(pFunc); 
        Py_DECREF(pModule);
        Py_DECREF(pyParams);
        Py_DECREF(pRetValue);
        Py_DECREF(currPop_list);

    }

    void MOEADPPL::RecordCurrentPop(PyObject *pop)
    {
        PyObject *list;
        for (int i = 0; i < g_GlobalSettings->population_num_; ++i)
        {
            list = PyList_New(g_GlobalSettings->obj_num_);//ref_cnt:=1
            for (int j = 0; j < g_GlobalSettings->obj_num_; ++j)
            {
                PyList_SetItem(list, j, PyFloat_FromDouble(g_GlobalSettings->parent_population_[i]->obj_[j]));
            }
            PyList_Append(pop, list);//ref_cnt:=2
            Py_DECREF(list);
        }
    }   

    void MOEADPPL::SetBiasedWeight(double *best)
    {
        std::vector<double>dis(weight_num_);
        for(int i=0;i<weight_num_;++i)
        {
            dis[i]=CalEuclidianDistance(best,lambda_[i],g_GlobalSettings->obj_num_);
        }
        std::vector<size_t>index(dis.size());
        iota(index.begin(),index.end(),0);
        sort(index.begin(),index.end(),
            [&dis](size_t index_1,size_t index_2){return dis[index_1]<dis[index_2];});
        //find the 10 best lambda_ according to our best
        //the 10 best are index[0]~index[9]

        bool flag[weight_num_];
        memset(flag,true,weight_num_*sizeof(bool));
        for(int i=0;i<nPromisingWeight;++i)
        {
            flag[index[i]]=false;//10 best need no change
            // std::cout<<index[i]<<endl;
        }
        int nSolveNum=0;//weights have been changed
        double tempdis,minDis;
        int near_index;
        nSolveNum=nPromisingWeight;
        int nMaxChange=ceil((double)(weight_num_-nPromisingWeight)/nPromisingWeight);
        // std::cout<<nSolveNum<<"  "<<nMaxChange<<endl;
        for(int i=0;i<nPromisingWeight && nSolveNum<weight_num_; ++i)
        {
            int nCurrentTuned=0;
            
            while(nCurrentTuned<nMaxChange && nSolveNum<weight_num_)
            {
                minDis=INF;
                for(int j=0;j<weight_num_;++j)
                {
                    if(flag[j])//flag=true haven't been changed yet
                    {
                        tempdis=CalEuclidianDistance(lambda_[index[i]],lambda_[j],g_GlobalSettings->obj_num_);
                        if(tempdis<minDis)
                        {
                            minDis=tempdis;
                            near_index=j;
                            // std::cout<<"near_index"<<near_index;
                        }
                    }
                }

                for(int j=0;j<g_GlobalSettings->obj_num_;++j)
                {
                    lambda_[near_index][j]+=step_size_*(lambda_[index[i]][j]-lambda_[near_index][j]);
                    // lambda_[near_index][j]=lambda_[index[i]][j];

                }
                flag[near_index]=false;
                nSolveNum++;
                nCurrentTuned++;
            }
            // std::cout<<nSolveNum<<endl;
        }

    }

    void MOEADPPL::CalculateMinDis(double *best)
    {
        double min_dis = 100;
        double dis;
        for(int i = 0; i < g_GlobalSettings->population_num_;++i)
        {
            dis = CalEuclidianDistance(best,g_GlobalSettings->parent_population_[i]->obj_.data(),g_GlobalSettings->obj_num_);
            // std::cout<<i<<":"<<dis[i]<<endl;
            // printf("current dis%f\n",dis);
            if(dis < min_dis)
            {
                min_dis = dis;
            }
        }
        printf("min dis %lf\n", min_dis);
        char output_dir[1024];
        char output_file[1024];

        std::string problem_name(g_GlobalSettings->problem_name_);
        std::string algorithm_name(g_GlobalSettings->algorithm_name_);

        sprintf(output_dir,"./MinDis/%s/",
                g_GlobalSettings->algorithm_name_.c_str()
                );
        CreateDirectory(output_dir);

        sprintf(output_file, "%s/%s_M%d_D%d.txt", 
                output_dir, 
                g_GlobalSettings->problem_name_.c_str(),
                g_GlobalSettings->obj_num_,
                g_GlobalSettings->dec_num_
                );

        FILE *fpt = fopen(output_file, "a");
		
		if(fpt == nullptr)
		{
			std::cout <<"Can not open "<< output_file << " !." << std::endl;
			std::cout << "Press enter to exit" << std::endl;
			std::cout << strerror(errno) << "\n";
			std::cin.get();
			exit(-1);
		}

		
       
        fprintf(fpt, "%lf", min_dis);
        fprintf(fpt, "\n");

		fflush(fpt);
		fclose(fpt);

    }

}