// I-NSGA-II/LTR
// Paper: Li, Ke, Guiyu Lai, and Xin Yao. "Interactive evolutionary multi-objective optimization via learning-to-rank." IEEE Transactions on Evolutionary Computation (2023).
// Link: https://ieeexplore.ieee.org/abstract/document/10015671
// created by Tian Huang 10/04/2024
// lreaning to rank in RankNet.py, require installing tensorflow
// to run this algorithm, make sure conda activate proper python environment before cmake

#include "algorithm/nsga2_ltr/nsga2_ltr.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <Python.h>

#include "core/macro.h"
#include "core/global.h"
#include "core/file.h"
#include "core/utility.h"
#include "core/nd_sort.h"
#include "operator/tournament_selection.h"
#include "operator/polynomial_mutation.h"
#include "operator/sbx.h"
#include "random/random.h"


namespace emoc {

	NSGA2_LTR::NSGA2_LTR(int thread_id) :
    Algorithm(thread_id),
    weight_(nullptr),
    // tau(g_GlobalSettings->tau_)
    tau(25)
	{

	}

	NSGA2_LTR::~NSGA2_LTR()
	{
        // release weight_, winner and loser queue
        if(weight_)
        {
            delete[] weight_;
            weight_ = nullptr;
        }
        while (winners_.size())
        {
            delete[] winners_.front();
            winners_.pop();
            delete[] losers_.front();
            losers_.pop();
        }
        
	}

	void NSGA2_LTR::Solve()
	{
        bool falg_EnvironmentalSelection;
		Initialization();
        first_tau = 0.4 * g_GlobalSettings->max_evaluation_ / g_GlobalSettings->population_num_;
        printf("first tau = %d \n", first_tau);
        
		while (!IsTermination())
		{
			// generate offspring population
			Crossover(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->offspring_population_.data());
			PolynomialMutation(g_GlobalSettings->offspring_population_.data(), 2 * (real_popnum_ / 2), g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, mutation_para_);
			EvaluatePop(g_GlobalSettings->offspring_population_.data(), 2 * (real_popnum_ / 2));
			MergePopulation(g_GlobalSettings->parent_population_.data(), real_popnum_, g_GlobalSettings->offspring_population_.data(),
				2 * (real_popnum_ / 2), g_GlobalSettings->mixed_population_.data());
			
			// select next generation's population
			// EnvironmentalSelection(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->mixed_population_.data());
            falg_EnvironmentalSelection = EnvironmentalSelection(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->mixed_population_.data(), (g_GlobalSettings->iteration_num_ >= first_tau) && tau && (g_GlobalSettings->iteration_num_ % tau == 0));
            if(!falg_EnvironmentalSelection)
            {
                std::cerr<<"[ERROR] Wrong in EnvironmentalSelection!\n";
                return;
            }

            // CalculateTotalDis(weight_);
            
        }
        // CalculateMinDis(weight_, g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->obj_num_, g_GlobalSettings->dec_num_, g_GlobalSettings->problem_name_, g_GlobalSettings->algorithm_name_);
        // CalculateExpectDis(weight_, g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->obj_num_, g_GlobalSettings->dec_num_, g_GlobalSettings->problem_name_, g_GlobalSettings->algorithm_name_);
	}

	void NSGA2_LTR::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
        
        // set weight for DM
        // weight_ = SetWeight(g_GlobalSettings->weight_StringType_);
        weight_ = SetWeight(g_GlobalSettings->obj_num_, g_GlobalSettings->problem_name_);

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 20.0;
	}

	void NSGA2_LTR::Crossover(Individual **parent_pop, Individual **offspring_pop)
	{
		// generate random permutation index for tournment selection
		std::vector<int> index1(g_GlobalSettings->population_num_);
		std::vector<int> index2(g_GlobalSettings->population_num_);
		random_permutation(index1.data(), g_GlobalSettings->population_num_);
		random_permutation(index2.data(), g_GlobalSettings->population_num_);

		for (int i = 0; i < g_GlobalSettings->population_num_ / 2; ++i)
		{
			Individual *parent1 = TournamentByRank(parent_pop[index1[2 * i]], parent_pop[index1[2 * i + 1]]);
			Individual *parent2 = TournamentByRank(parent_pop[index2[2 * i]], parent_pop[index2[2 * i + 1]]);
			SBX(parent1, parent2, offspring_pop[2 * i], offspring_pop[2 * i + 1],
				g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, cross_para_);
		}
	}
	
	void NSGA2_LTR::SetDistanceInfo(std::vector<DistanceInfo> &distanceinfo_vec, int target_index, double distance)
	{
		// search the target_index and set it's distance
		for (int i = 0; i < distanceinfo_vec.size(); ++i)
		{
			if (distanceinfo_vec[i].index == target_index)
			{
				distanceinfo_vec[i].distance += distance;
				break;
			}
		}
	}


	int NSGA2_LTR::CrowdingDistance(Individual **mixed_pop,  int pop_num, int *pop_sort, int rank_index)
	{
		int num_in_rank = 0;
		std::vector<int> sort_arr(pop_num, 0);
		std::vector<DistanceInfo> distanceinfo_vec(pop_num, { -1,0.0 });

		// find all the indviduals with rank rank_index
		for (int i = 0; i < pop_num; i++)
		{
			mixed_pop[i]->fitness_ = 0;
			if (mixed_pop[i]->rank_ == rank_index)
			{
				distanceinfo_vec[num_in_rank].index = i;
				sort_arr[num_in_rank] = i;
				num_in_rank++;
			}
		}

		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
		{
			// sort the population with i-th obj
			std::sort(sort_arr.begin(), sort_arr.begin()+num_in_rank, [=](int left, int right){
				return mixed_pop[left]->obj_[i] < mixed_pop[right]->obj_[i];
			});

			// set the first and last individual with INF fitness (crowding distance)
			mixed_pop[sort_arr[0]]->fitness_ = EMOC_INF;
			SetDistanceInfo(distanceinfo_vec, sort_arr[0], EMOC_INF);
			mixed_pop[sort_arr[num_in_rank - 1]]->fitness_ = EMOC_INF;
			SetDistanceInfo(distanceinfo_vec, sort_arr[num_in_rank - 1], EMOC_INF);

			// calculate each solution's crowding distance
			for (int j = 1; j < num_in_rank - 1; j++)
			{
				if (EMOC_INF != mixed_pop[sort_arr[j]]->fitness_)
				{
					if (mixed_pop[sort_arr[num_in_rank - 1]]->obj_[i] == mixed_pop[sort_arr[0]]->obj_[i])
					{
						mixed_pop[sort_arr[j]]->fitness_ += 0;
					}
					else
					{
						double distance = (mixed_pop[sort_arr[j + 1]]->obj_[i] - mixed_pop[sort_arr[j - 1]]->obj_[i]) /
							(mixed_pop[sort_arr[num_in_rank - 1]]->obj_[i] - mixed_pop[sort_arr[0]]->obj_[i]);
						mixed_pop[sort_arr[j]]->fitness_ += distance;
						SetDistanceInfo(distanceinfo_vec, sort_arr[j], distance);
					}
				}
			}
		}
        // TODO < or > ? 
		std::sort(distanceinfo_vec.begin(), distanceinfo_vec.begin()+num_in_rank, [](const DistanceInfo &left, const DistanceInfo &right) {
			// return left.distance < right.distance;
            return left.distance > right.distance;
		});

		// copy sort result
		for (int i = 0; i < num_in_rank; i++)
		{
			pop_sort[i] = distanceinfo_vec[i].index;
		}

		return num_in_rank;
	}

    void NSGA2_LTR::SetPreferenceInfo(std::vector<SortList> &preferenceInfo_vec, int target_index, double value)
    {
        // search the target_index and set it's distance
        for (int i = 0; i < preferenceInfo_vec.size(); ++i)
        {
            if (preferenceInfo_vec[i].index == target_index)
            {
                preferenceInfo_vec[i].value += value;
                break;
            }
        }
    }

    int NSGA2_LTR::ArbitrarilySelect(Individual **mixed_pop, int pop_num, int *pop_sort, int rank_index)
    {
        int num_in_rank = 0;
        std::vector<int> sort_arr(pop_num, 0);
        std::vector<DistanceInfo> distanceinfo_vec(pop_num, { -1,0.0 });

        // find all the indviduals with rank rank_index
        for (int i = 0; i < pop_num; i++)
        {
            mixed_pop[i]->fitness_ = 0;
            if (mixed_pop[i]->rank_ == rank_index)
            {
                distanceinfo_vec[num_in_rank].index = i;
                sort_arr[num_in_rank] = i;
                num_in_rank++;
            }
        }
        std::sort(distanceinfo_vec.begin(), distanceinfo_vec.begin()+num_in_rank, [](const DistanceInfo &left, const DistanceInfo &right) {
            return left.distance < right.distance;
        });

        // copy sort result
        for (int i = 0; i < num_in_rank; i++)
        {
            pop_sort[i] = distanceinfo_vec[i].index;
        }
        return num_in_rank;
    }

    int NSGA2_LTR::RankViaPreference(Individual **mixed_pop, int pop_num, int *pop_sort, int rank_index, int nneed)
    {
        Py_Initialize();
        if(!Py_IsInitialized())
        {
            printf("Python init failed!\n");
        }
        std::cout<<"<info> [NSGA2_LTR::RankViaPreference]"<<std::endl;
        PyRun_SimpleString("import sys");
        PyRun_SimpleString("sys.path.append('./')");
        PyObject *pModule = NULL;
        PyObject* pRet = NULL;
        PyObject *PF_py = NULL; // PF in the last layer
        PyObject *ind_obj = NULL;
        PyObject *ind_obj_2 = NULL;
        PyObject *float_py = NULL;
        PyObject *winners_list = NULL;
        PyObject *losers_list = NULL;
        PyObject *pUseModel = NULL;

        wchar_t * s2[] = { L" " }; // 宽字符，长度为2字节
        PySys_SetArgv(1, s2);   // TODO: 加入argv参数 否则出错.PyAPI_FUNC(void) PySys_SetArgv(int, wchar_t **);
        //导入python文件
        // std::cout<<"begin to load ranknet\n";
        pModule = PyImport_ImportModule("RankNet");
        if (!pModule)
        {
            std::cerr << "[ERROR] Can not open python file!"<<std::endl;
            return -1;
        }
        // else
        // {
        //     std::cout<<"load module successfully!\n";
        // }
        /***
         * initialize model in first consultation
         */
        // if (g_GlobalSettings->iteration_num_ == g_GlobalSettings->tau_)
        if (g_GlobalSettings->iteration_num_ == first_tau)
        {
            PyObject *pCreateModel = PyObject_GetAttrString(pModule, "CreateModel");
            PyObject_CallFunctionObjArgs(pCreateModel, PyLong_FromLong((long)(g_GlobalSettings->obj_num_)),PyLong_FromLong((long)(g_GlobalSettings->run_id_)), NULL);
            Py_DECREF(pCreateModel);
        }

        int index_1, index_2, index_of_ind_1, index_of_ind_2, idx2ind;
        int num_in_rank = 0;
        std::vector<int> PF(pop_num, 0);// pop_num: 2 * pop_parent_num
        std::vector<SortList> preferenceInfo_vec(pop_num, { -1,0.0 });
        double *ind_1_obj, *ind_2_obj, *temp_a;
        ind_1_obj = ind_2_obj = nullptr;

        // find all the indviduals with rank rank_index
        for (int i = 0; i < pop_num; i++)
        {
            if (mixed_pop[i]->rank_ == rank_index)
            {
                preferenceInfo_vec[num_in_rank].index = i;
                PF[num_in_rank] = i;
                num_in_rank++;
            }
        }

        // record the PF in the last layer, then throw individuals in it into RankNet.
        PF_py = PyList_New(0);
        for (int i = 0; i < num_in_rank; ++i)
        {
            ind_obj = PyList_New(g_GlobalSettings->obj_num_);
            for (int j = 0; j < g_GlobalSettings->obj_num_; ++j)
            {
                PyList_SetItem(ind_obj, j, PyFloat_FromDouble(g_GlobalSettings->mixed_population_[PF[i]]->obj_[j]));
            }
            PyList_Append(PF_py, ind_obj);
            Py_DECREF(ind_obj);
        }

        /**
         * train RankNet
         */
        // if (g_GlobalSettings->iteration_num_ % g_GlobalSettings->tau_ ==0)
        if (g_GlobalSettings->iteration_num_ % tau ==0)
        {
            PyObject *pTrainModel = NULL;
            pTrainModel = PyObject_GetAttrString(pModule, "TrainModel");

            // int number_of_inquiries = g_GlobalSettings->inquiriesNum_;
            int number_of_inquiries = g_GlobalSettings->max_evaluation_ / tau;
            while (number_of_inquiries--)
            {
                if ((MAX_QUEUE_SIZE != -1) && (winners_.size()==MAX_QUEUE_SIZE))
                {
                    temp_a = winners_.front();
                    if (temp_a)
                    {
                        delete[] temp_a;
                        temp_a = nullptr;
                    }
                    winners_.pop();
                    temp_a = losers_.front();
                    if (temp_a)
                    {
                        delete[] temp_a;
                        temp_a = nullptr;
                    }
                    losers_.pop();
                }

                index_1 = index_2 = 0;
                while((index_1==index_2))
                {
                    index_1 = rnd(0, num_in_rank - 1);
                    index_2 = rnd(0, num_in_rank - 1);
                }
                index_of_ind_1 = PF[index_1];
                index_of_ind_2 = PF[index_2];
                ind_1_obj = new double[g_GlobalSettings->obj_num_];
                ind_2_obj = new double[g_GlobalSettings->obj_num_];
                for (int i = 0; i < g_GlobalSettings->obj_num_; ++i)
                {
                    ind_1_obj[i] = g_GlobalSettings->mixed_population_[index_of_ind_1]->obj_[i];
                    ind_2_obj[i] = g_GlobalSettings->mixed_population_[index_of_ind_2]->obj_[i];
                }

                if (CalInverseChebycheff(g_GlobalSettings->mixed_population_[index_of_ind_1], weight_,g_GlobalSettings->obj_num_)
                <CalInverseChebycheff(g_GlobalSettings->mixed_population_[index_of_ind_2], weight_, g_GlobalSettings->obj_num_))
                {
                    winners_.push(ind_1_obj);
                    losers_.push(ind_2_obj);
                }
                else
                {
                    winners_.push(ind_2_obj);
                    losers_.push(ind_1_obj);
                }
            }

            winners_list = PyList_New(0);
            losers_list = PyList_New(0);
            for (int i = 0; i < winners_.size(); ++i)
            {
                ind_obj = PyList_New(g_GlobalSettings->obj_num_);
                ind_obj_2 = PyList_New(g_GlobalSettings->obj_num_);
                for (int j = 0; j < g_GlobalSettings->obj_num_; ++j)
                {
                    PyList_SetItem(ind_obj, j, PyFloat_FromDouble((winners_.front())[j]));
                    PyList_SetItem(ind_obj_2, j, PyFloat_FromDouble((losers_.front())[j]));
                }
                winners_.push(winners_.front());
                winners_.pop();
                losers_.push(losers_.front());
                losers_.pop();
                PyList_Append(winners_list, ind_obj);
                PyList_Append(losers_list, ind_obj_2);
                Py_DECREF(ind_obj);
                Py_DECREF(ind_obj_2);
            }
        // #if DEBUG
        //     std::cout<<"[INFO] Number of pairwise comparisons used for training RankNet: "<<PyList_Size(winners_list)<<std::endl;
        // #endif

            PyObject_CallFunctionObjArgs(pTrainModel, winners_list,losers_list,PyLong_FromLong((long)(g_GlobalSettings->run_id_)), NULL);

            Py_DECREF(winners_list);
            Py_DECREF(losers_list);
            Py_DECREF(pTrainModel);//TODO: DEBUG -> ref_cnt != 0
        }

        /**
         * use RankNet
         */
        pUseModel = PyObject_GetAttrString(pModule, "UseModel");

        // PyObject* pyParams=PyTuple_New(2);
        // PyTuple_SetItem(pyParams, 0, PF_py);          //PF from last layer
        // PyTuple_SetItem(pyParams, 1, Py_BuildValue("l", g_GlobalSettings->run_id_));
        // PyTuple_SetItem(pyParams, 2, Py_BuildValue("s", "NULL"));                   
        

        

        // pRet=PyObject_CallObject(pUseModel,pyParams);

        pRet = PyObject_CallFunctionObjArgs(pUseModel, PF_py,PyLong_FromLong((long)(g_GlobalSettings->run_id_)), NULL);
        int listSize;
        double score[num_in_rank];
        if (pRet)  // 验证是否调用成功
        {
            std::cout<<"[INFO] Function excuted successfully."<<std::endl;
            listSize = PyList_Size(pRet);
            for (int i = 0; i < listSize; ++i)
            {
                float_py = PyList_GetItem(pRet,i);
                score[i] = PyFloat_AsDouble(float_py);
            }
        }
        else
        {
            std::cerr<<"[ERROR] Function excuted unsuccessfully."<<std::endl;
            return -1;
        }
        int idx_ind;
        for (int i = 0; i < num_in_rank; ++i)
        {
            idx_ind = PF[i];
            mixed_pop[idx_ind]->fitness_ = score[i];
            SetPreferenceInfo(preferenceInfo_vec, idx_ind, score[i]);
        }
        // #if DEBUG
        // std::cout<<"[INFO] Score in the last PF:\n";
        // for (int i = 0; i < num_in_rank; ++i)
        // {
        //     std::cout<<preferenceInfo_vec[i].index<<": "<<preferenceInfo_vec[i].value<<std::endl;
        // }
        // #endif


        // #if UsingGoldenFunction
        // std::cout<<"[INFO] Using golden function.\n";
        // for (int i = 0; i < num_in_rank; ++i)
        // {
        //     preferenceInfo_vec[i].value = 0.0;
        // }
        // for (int i = 0; i < num_in_rank; ++i)
        // {
        //     idx_ind = PF[i];
        //     SetPreferenceInfo(preferenceInfo_vec, idx_ind, 1-CalInverseChebycheff(mixed_pop[idx_ind], weight_, g_GlobalSettings->obj_num_));
        //     score[i] = 1-CalInverseChebycheff(mixed_pop[idx_ind], weight_, g_GlobalSettings->obj_num_);
        // }
        // #endif

        // #if DEBUG
        // // track ranking performance of top-k individuals
        // double goldenPreference[num_in_rank];
        // for (int i = 0; i < num_in_rank; ++i)
        // {
        //     goldenPreference[i] = CalInverseChebycheff(g_GlobalSettings->mixed_population_[PF[i]]->obj_, weight_, g_GlobalSettings->obj_num_);
        // }
        // std::cout<<"[INFO] Accuracy of top-k ranking: "<<topK(score, goldenPreference, num_in_rank, nneed)<<std::endl;
        // #endif

        std::sort(preferenceInfo_vec.begin(), preferenceInfo_vec.begin()+num_in_rank, [](const SortList &left, const SortList &right) {
            return left.value < right.value;
        });
        // #if DEBUG
        // /*std::cout<<"[INFO] Rank of the last PF:\n";
        // for (int i = 0; i < num_in_rank; ++i)
        // {
        //     std::cout<<preferenceInfo_vec[i].index<<": "<<preferenceInfo_vec[i].value<<std::endl;
        // }*/
        // #endif

        // copy sort result
        for (int i = 0; i < num_in_rank; i++)
        {
            pop_sort[i] = preferenceInfo_vec[i].index;
        }

        Py_DECREF(pModule);
        Py_DECREF(pRet);
        Py_DECREF(PF_py);
        Py_DECREF(pUseModel);

        return num_in_rank;
    }

	void NSGA2_LTR::EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop)
	{
		int current_popnum = 0, rank_index = 0;
		int mixed_popnum = g_GlobalSettings->population_num_ + 2 * (g_GlobalSettings->population_num_ / 2);

		NonDominatedSort(mixed_pop, mixed_popnum, g_GlobalSettings->obj_num_);

		// select individuals by rank
		while (1)
		{
			int temp_number = 0;
			for (int i = 0; i < mixed_popnum; i++)
			{
				if (mixed_pop[i]->rank_ == rank_index)
				{
					temp_number++;
				}
			}
			if (current_popnum + temp_number <= g_GlobalSettings->population_num_)
			{
				for (int i = 0; i < mixed_popnum; i++)
				{
					if (mixed_pop[i]->rank_ == rank_index)
					{
						CopyIndividual(mixed_pop[i], parent_pop[current_popnum]);
						current_popnum++;
					}
				}
				rank_index++;
			}
			else
				break;
		}

		// select individuals by crowding distance
		int sort_num = 0;
		std::vector<int> pop_sort(mixed_popnum);

		if (current_popnum < g_GlobalSettings->population_num_)
		{
			sort_num = CrowdingDistance(mixed_pop, mixed_popnum, pop_sort.data(), rank_index);
			while (1)
			{
				if (current_popnum < g_GlobalSettings->population_num_)
				{
					int index = pop_sort[sort_num - 1];
					CopyIndividual(mixed_pop[pop_sort[--sort_num]], parent_pop[current_popnum]);
					current_popnum++;
				}
				else {
					break;
				}
			}
		}

		// clear crowding distance value
		for (int i = 0; i < g_GlobalSettings->population_num_; i++)
		{
			parent_pop[i]->fitness_ = 0;
		}
	}

    bool NSGA2_LTR::EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop, bool usingPreferenceModel)
    {
        std::cout<<"<info> [NSGA2_LTR::EnvironmentalSelection]"<<std::endl;
        int current_popnum = 0, rank_index = 0;
        int mixed_popnum = 2 * g_GlobalSettings->population_num_;
        // do nondominated sorting in the mixed population as the first selection pressure
        NonDominatedSort(mixed_pop, mixed_popnum, g_GlobalSettings->obj_num_);
        // select individuals by rank
        while (1)
        {
            int temp_number = 0;// record the number of Pareto front of rank_index
            for (int i = 0; i < mixed_popnum; i++)
            {
                if (mixed_pop[i]->rank_ == rank_index)
                {
                    temp_number++;
                }
            }
            if (current_popnum + temp_number <= g_GlobalSettings->population_num_)
            {// add all individuals in Pareto front of rank_index to the population of the new offspring
                for (int i = 0; i < mixed_popnum; i++)
                {
                    if (mixed_pop[i]->rank_ == rank_index)
                    {
                        CopyIndividual(mixed_pop[i], parent_pop[current_popnum]);
                        current_popnum++;
                    }
                }
                rank_index++;
            }
            else// only rely on the 1st selection pressure is not enough, so elicit the crowding distance as the second selection pressure
            {
                break;
            }
        }

        // select individuals by crowding distance
        int sort_num = 0;// record
        int *pop_sort = new int[mixed_popnum];

        if (current_popnum < g_GlobalSettings->population_num_)
        {
            if (!usingPreferenceModel)
            {
                // if (g_GlobalSettings->iteration_num_<g_GlobalSettings->tau_)
                if (g_GlobalSettings->iteration_num_ < tau)
                {
                    sort_num = CrowdingDistance(mixed_pop, mixed_popnum, pop_sort, rank_index);
                }
                else
                {
                    sort_num = ArbitrarilySelect(mixed_pop, mixed_popnum, pop_sort, rank_index);
                }
            }
            else
            {
                sort_num = RankViaPreference(mixed_pop, mixed_popnum, pop_sort, rank_index, g_GlobalSettings->population_num_-current_popnum);
                if (sort_num == -1)
                {
                    return false;
                }
            }
            while (1)
            {
                if (current_popnum < g_GlobalSettings->population_num_)
                {
                    CopyIndividual(mixed_pop[pop_sort[--sort_num]], parent_pop[current_popnum]);//(A, B): A->B
                    current_popnum++;
                }
                else
                {
                    break;
                }
            }
        }

        // clear crowding distance value
        for (int i = 0; i < g_GlobalSettings->population_num_; i++)
        {
            parent_pop[i]->fitness_ = 0.0;
        }
        if (pop_sort)
        {
            delete[] pop_sort;
            pop_sort = nullptr;
        }
        return true;
    }

}