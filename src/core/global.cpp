#include "core/global.h"

#include <iostream>
#include <cstdlib>

#include "random/random.h"
#include "core/emoc_manager.h"
#include "problem/problem_head_collect.h"
#include "algorithms/algorithm_head_collect.h"

namespace emoc {

	Global::Global(const char *algorithn_name, const char *problem_name, int population_num,
		int dec_num, int obj_num, int max_evaluation, int thread_id, int output_interval, int run_id) :
		algorithm_name_(algorithn_name),
		problem_name_(problem_name),
		population_num_(population_num),
		dec_num_(dec_num),
		obj_num_(obj_num),
		max_evaluation_(max_evaluation),
		thread_id_(thread_id),
		output_interval_(output_interval),
		run_id_(run_id)
	{
		iteration_num_ = 0;
		current_evaluation_ = 0;
		parent_population_.reserve(population_num);
		offspring_population_.reserve(population_num);
		mixed_population_.reserve(population_num * 2);

		sbx_parameter_.crossover_pro = 1.0;
		sbx_parameter_.eta_c = 20.0;
		de_parameter_.crossover_pro = 1.0;
		de_parameter_.F = 0.5;
		de_parameter_.K = 0.5;
		pm_parameter_.muatation_pro = 1.0 / (double)dec_num;
		pm_parameter_.eta_m = 20.0;

	}

	Global::~Global()
	{
		DestroyMemory();
	}

	void Global::InitializePopulation(Individual **pop,int pop_num)
	{
		for (int i = 0; i < pop_num; ++i)
		{
			InitializeIndividual(pop[i]);
		}
	}

	void Global::InitializeIndividual(Individual *ind)
	{
		for (int i = 0; i < dec_num_; ++i)
		{
			ind->dec_[i] = rndreal(dec_lower_bound_[i], dec_upper_bound_[i]);
		}
	}

	void Global::Start()
	{
		EMOCManager::Instance()->SetPause(false);
		EMOCManager::Instance()->SetFinish(false);
		algorithm_->Run();
		EMOCManager::Instance()->SetFinish(true);
	}

	bool Global::IsTermination()
	{
		return current_evaluation_ >= max_evaluation_;
	}

	double Global::RecordFileTime()
	{
		return algorithm_->record_file_time_;
	}

	void Global::Init()
	{
		AllocateMemory();

		// Because the initialization of algorithm needs the Global object has been created,
		// we need delay the following into Start() or just put it in another function and call it before Start()
		InitializeProblem();
		InitializeAlgorithm();
		SetDecBound();
	}

	void Global::SetDecBound()
	{
		for (int i = 0; i < dec_num_; ++i)
		{
			dec_lower_bound_[i] = problem_->lower_bound_[i];
			dec_upper_bound_[i] = problem_->upper_bound_[i];
		}
	}

	void Global::InitializeProblem()
	{
		// modify problem name to lower case
		std::string problem_name(problem_name_);
		for (auto &c : problem_name)
		{
			if(c >= '0' && c <= '9') continue;
			c = tolower(c);
		}

		//std::cout << problem_name << std::endl;
		if (problem_name == "zdt1")
			problem_ = new ZDT1(dec_num_, obj_num_);
		else if (problem_name == "zdt2")
			problem_ = new ZDT2(dec_num_, obj_num_);
		else if (problem_name == "zdt3")
			problem_ = new ZDT3(dec_num_, obj_num_);
		else if (problem_name == "zdt4")
			problem_ = new ZDT4(dec_num_, obj_num_);
		else if (problem_name == "zdt6")
			problem_ = new ZDT6(dec_num_, obj_num_);
		else if (problem_name == "dtlz1")
			problem_ = new DTLZ1(dec_num_, obj_num_);
		else if (problem_name == "dtlz2")
			problem_ = new DTLZ2(dec_num_, obj_num_);
		else if (problem_name == "dtlz3")
			problem_ = new DTLZ3(dec_num_, obj_num_);
		else if (problem_name == "dtlz4")
			problem_ = new DTLZ4(dec_num_, obj_num_);
		else if (problem_name == "dtlz5")
			problem_ = new DTLZ5(dec_num_, obj_num_);
		else if (problem_name == "dtlz6")
			problem_ = new DTLZ6(dec_num_, obj_num_);
		else if (problem_name == "dtlz7")
			problem_ = new DTLZ7(dec_num_, obj_num_);
		else if (problem_name == "uf1")
			problem_ = new UF1(dec_num_, obj_num_);
		else if (problem_name == "uf2")
			problem_ = new UF2(dec_num_, obj_num_);
		else if (problem_name == "uf3")
			problem_ = new UF3(dec_num_, obj_num_);
		else if (problem_name == "uf4")
			problem_ = new UF4(dec_num_, obj_num_);
		else if (problem_name == "uf5")
			problem_ = new UF5(dec_num_, obj_num_);
		else if (problem_name == "uf6")
			problem_ = new UF6(dec_num_, obj_num_);
		else if (problem_name == "uf7")
			problem_ = new UF7(dec_num_, obj_num_);
		else if (problem_name == "uf8")
			problem_ = new UF8(dec_num_, obj_num_);
		else if (problem_name == "uf9")
			problem_ = new UF9(dec_num_, obj_num_);
		else if (problem_name == "uf10")
			problem_ = new UF10(dec_num_, obj_num_);
		else if (problem_name == "wfg1")
			problem_ = new WFG1(dec_num_, obj_num_);
		else if (problem_name == "wfg2")
			problem_ = new WFG2(dec_num_, obj_num_);
		else if (problem_name == "wfg3")
			problem_ = new WFG3(dec_num_, obj_num_);
		else if (problem_name == "wfg4")
			problem_ = new WFG4(dec_num_, obj_num_);
		else if (problem_name == "wfg5")
			problem_ = new WFG5(dec_num_, obj_num_);
		else if (problem_name == "wfg6")
			problem_ = new WFG6(dec_num_, obj_num_);
		else if (problem_name == "wfg7")
			problem_ = new WFG7(dec_num_, obj_num_);
		else if (problem_name == "wfg8")
			problem_ = new WFG8(dec_num_, obj_num_);
		else if (problem_name == "wfg9")
			problem_ = new WFG9(dec_num_, obj_num_);
		else
		{
			std::cout <<problem_name<< " The problem name is wrong, please check it again" << std::endl;
			std::cout << "Press enter to exit" << std::endl;
			std::cin.get();
			exit(-1);
		}
	}

	void Global::InitializeAlgorithm()
	{
		// modify problem name to lower case
		std::string algorithm_name(algorithm_name_);
		for (auto &c : algorithm_name)
		{
			if (c >= '0' && c <= '9') continue;
			c = tolower(c);
		}
		//std::cout << algorithm_name << std::endl;
		if (algorithm_name == "nsga2")
			algorithm_ = new NSGA2(problem_, thread_id_);
		else if (algorithm_name == "spea2")
			algorithm_ = new SPEA2(problem_, thread_id_);
		else if (algorithm_name == "moead")
			algorithm_ = new MOEAD(problem_, thread_id_);
		else if (algorithm_name == "moeadde")
			algorithm_ = new MOEADDE(problem_, thread_id_);
		else if (algorithm_name == "moeaddra")
			algorithm_ = new MOEADDRA(problem_, thread_id_);
		else if (algorithm_name == "moeadfrrmab")
			algorithm_ = new MOEADFRRMAB(problem_, thread_id_);
		else if (algorithm_name == "ibea")
			algorithm_ = new IBEA(problem_, thread_id_);
		else if (algorithm_name == "smsemoa")
			algorithm_ = new SMSEMOA(problem_, thread_id_);
		else if (algorithm_name == "hype")
			algorithm_ = new HypE(problem_, thread_id_);
		else if (algorithm_name == "ensmoead")
			algorithm_ = new ENSMOEAD(problem_, thread_id_);
		else if (algorithm_name == "moeadgra")
			algorithm_ = new MOEADGRA(problem_, thread_id_);
		else if (algorithm_name == "moeadira")
			algorithm_ = new MOEADIRA(problem_, thread_id_);
		else
		{
			std::cout << "The algorithm name is wrong, please check it again" << std::endl;			
			std::cout << "Press enter to exit" << std::endl;
			std::cin.get();
			exit(-1);
		}
	}

	void Global::AllocateMemory()
	{
		dec_lower_bound_ = new double[dec_num_];
		dec_upper_bound_ = new double[dec_num_];

		for (int i = 0; i < population_num_; ++i)
		{
			parent_population_.push_back(new Individual(dec_num_, obj_num_));
			offspring_population_.push_back(new Individual(dec_num_, obj_num_));
			mixed_population_.push_back(new Individual(dec_num_, obj_num_));
			mixed_population_.push_back(new Individual(dec_num_, obj_num_));
		}
	}

	void Global::DestroyMemory()
	{
		delete[] dec_lower_bound_;
		delete[] dec_upper_bound_;
		dec_lower_bound_ = nullptr;
		dec_upper_bound_ = nullptr;

		int size = parent_population_.size();
		for (int i = 0; i < size; ++i)
		{
			delete parent_population_[i];
			delete offspring_population_[i];
			delete mixed_population_[i];
			delete mixed_population_[i + size];
			parent_population_[i] = nullptr;
			offspring_population_[i] = nullptr;
			mixed_population_[i] = nullptr;
			mixed_population_[i + size] = nullptr;
		}

		delete problem_;
		delete algorithm_;
	}

	//Global *g_GlobalSettingsArray[MAX_THREAD_NUM] = { nullptr };
}