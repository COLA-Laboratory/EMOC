#include "core/global.h"

#include <iostream>
#include <cstdlib>

#include "random/random.h"
#include "core/emoc_manager.h"
#include "problem/problem_head_collect.h"
#include "algorithm/algorithm_head_collect.h"
#include "problem/problem_factory.h"
#include "algorithm/algorithm_factory.h"

namespace emoc {

	Global::Global(const char* algorithn_name, const char* problem_name, int population_num,
		int dec_num, int obj_num, int max_evaluation, int thread_id, int output_interval, int run_id) :
		dec_num_(dec_num),
		obj_num_(obj_num),
		population_num_(population_num),
		iteration_num_(0),
		current_evaluation_(0),
		max_evaluation_(max_evaluation),
		output_interval_(output_interval),
		algorithm_name_(algorithn_name),
		problem_name_(problem_name),
		dec_lower_bound_(std::vector<double>(dec_num)),
		dec_upper_bound_(std::vector<double>(dec_num)),
		problem_(nullptr),
		algorithm_(nullptr),
		run_id_(run_id),
		thread_id_(thread_id),
		is_customized_init_pop_(false),
		is_customized_problem_(false)
	{
		// reserve population space
		parent_population_.reserve(population_num);
		offspring_population_.reserve(population_num);
		mixed_population_.reserve(population_num * 2);

		// set default operator parameters
		sbx_parameter_.crossover_pro = 1.0;
		sbx_parameter_.eta_c = 20.0;
		de_parameter_.crossover_pro = 1.0;
		de_parameter_.F = 0.5;
		de_parameter_.K = 0.5;
		pm_parameter_.muatation_pro = 1.0 / (double)dec_num;
		pm_parameter_.eta_m = 20.0;

		// allocate memory for all population
		AllocateMemory();
	}

	Global::~Global()
	{
		DestroyMemory();
	}

	void Global::InitializePopulation(Individual** pop, int pop_num)
	{
		// When the initial population is set before algorithm starts, return directly.
		if (iteration_num_ == 0 && is_customized_init_pop_)
			return;

		for (int i = 0; i < pop_num; ++i)
		{
			InitializeIndividual(pop[i]);
		}
	}

	void Global::InitializeIndividual(Individual* ind)
	{
		if (problem_->encoding_ == Problem::REAL)
		{
			for (int i = 0; i < dec_num_; ++i)
				ind->dec_[i] = rndreal(dec_lower_bound_[i], dec_upper_bound_[i]);
		}
		else if (problem_->encoding_ == Problem::BINARY)
		{
			for (int i = 0; i < dec_num_; ++i)
				ind->dec_[i] = rnd(0,1);
		}
		else if (problem_->encoding_ == Problem::PERMUTATION)
		{
			std::vector<int> perm(dec_num_);
			random_permutation(perm.data(), perm.size());
			for (int i = 0; i < dec_num_; ++i)
				ind->dec_[i] = perm[i];
		}
	}

	void Global::SetCustomProblem(Problem* problem)
	{
		problem_ = problem;
		is_customized_problem_ = true;
	}

	void Global::SetCustomInitialPop(std::vector<std::vector<double>>& initial_pop)
	{
		int initial_pop_num = initial_pop.size();
		int initial_dec_dim = initial_pop[0].size();
		if (initial_pop_num > population_num_) throw std::runtime_error("initial population number is larger than the setted parameter!\n");
		if (initial_dec_dim != dec_num_) throw std::runtime_error("initial population decision dimensions is not equal to the setted parameter!\n");

		for (int i = 0; i < initial_pop_num; i++)
			for (int j = 0; j < initial_dec_dim; j++)
				parent_population_[i]->dec_[j] = initial_pop[i][j];

		is_customized_init_pop_ = true;
	}

	void Global::Start()
	{
		// get current emoc mode
		bool is_gui = EMOCManager::Instance()->GetIsGUI();
		bool is_experiment = EMOCManager::Instance()->GetIsExperiment();

		// When test module in gui mode, we need to update the EMOC state.
		if (is_gui && !is_experiment)
		{
			EMOCManager::Instance()->SetTestPause(false);
			EMOCManager::Instance()->SetTestFinish(false);
		}

		algorithm_->Solve();

		if (is_gui && !is_experiment)
			EMOCManager::Instance()->SetTestFinish(true);
	}

	void Global::Init()
	{
		// Because the initialization of algorithm needs the Global object has been created,
		// we need delay the following function after Global has been created and call it before Start().
		if (!is_customized_problem_)
			InitializeProblem();
		InitializeAlgorithm();

		// set decision boundary
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
		problem_ = ProblemFactory::Instance()->CreateProblemObject(problem_name_, dec_num_, obj_num_);
		if (problem_ == nullptr)
		{
			std::cout << "The problem name " << problem_name_ << " is wrong, please check it again" << std::endl;
			exit(1);
		}
	}

	void Global::InitializeAlgorithm()
	{
		algorithm_ = AlgorithmFactory::Instance()->CreateAlgorithmObject(algorithm_name_, thread_id_);
		if (algorithm_ == nullptr)
		{
			std::cout << "The algorithm name " << algorithm_ << " is wrong, please check it again" << std::endl;
			exit(1);
		}
	}

	void Global::AllocateMemory()
	{
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

		if(!is_customized_problem_)
			delete problem_;
		delete algorithm_;
	}

}