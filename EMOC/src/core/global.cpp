#include "core/global.h"

#include <iostream>
#include <cstdlib>

#include "random/random.h"
#include "problem/problem_head_collect.h"
#include "algorithms/algorithm_head_collect.h"

namespace emoc {

	Global::Global(const char *algorithn_name, const char *problem_name, int population_num, int dec_num, int obj_num,int max_evaluation):
		algorithm_name_(algorithn_name),
		problem_name_(problem_name),
		population_num_(population_num),
		dec_num_(dec_num),
		obj_num_(obj_num),
		max_evaluation_(max_evaluation)
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

		Init();
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
		InitializeProblem();
		InitializeAlgorithm();
		algorithm_->Run();
	}

	bool Global::IsTermination()
	{
		return current_evaluation_ >= max_evaluation_;
	}


	void Global::Init()
	{
		AllocateMemory();
		SetDecBound();
	}

	void Global::SetDecBound()
	{
		for (int i = 0; i < dec_num_; ++i)
		{
			dec_lower_bound_[i] = 0.0; // TODO: 这里其实可以改一改接口，创建具体的问题的时候，让问题的构造函数来赋值
			dec_upper_bound_[i] = 1.0;
		}

		if (problem_name_ == "zdt4")
		{
			for (int i = 1; i < dec_num_; ++i)
			{
				dec_lower_bound_[i] = -5.0;
				dec_upper_bound_[i] = 5.0;
			}
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

		std::cout << problem_name << std::endl;
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
		else
		{
			std::cout << "the problem name is wrong, please check it again" << std::endl;
			std::cout << "press enter to exit" << std::endl;
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
		std::cout << algorithm_name << std::endl;
		if (algorithm_name == "nsga2")
			algorithm_ = new NSGA2(problem_);
		if (algorithm_name == "spea2")
			algorithm_ = new SPEA2(problem_);
		else if (algorithm_name == "moead")
			algorithm_ = new MOEAD(problem_);
		else if (algorithm_name == "moeadde")
			algorithm_ = new MOEADDE(problem_);
		else if (algorithm_name == "moeaddra")
			algorithm_ = new MOEADDRA(problem_);
		else if (algorithm_name == "moeadfrrmab")
			algorithm_ = new MOEADFRRMAB(problem_);
		else if (algorithm_name == "ibea")
			algorithm_ = new IBEA(problem_);
		else if (algorithm_name == "smsemoa")
			algorithm_ = new SMSEMOA(problem_);
		else if (algorithm_name == "hype")
			algorithm_ = new HypE(problem_);
		else
		{
			std::cout << "the algorithm name is wrong, please check it again" << std::endl;			
			std::cout << "press enter to exit" << std::endl;
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

	Global *g_GlobalSettings = nullptr;
}