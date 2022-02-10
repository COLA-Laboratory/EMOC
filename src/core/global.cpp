#include "core/global.h"

#include <iostream>
#include <cstdlib>

#include "random/random.h"
#include "core/emoc_manager.h"
#include "problem/problem_head_collect.h"
#include "algorithms/algorithm_head_collect.h"
#include "problem/problem_factory.h"
#include "algorithms/algorithm_factory.h"

namespace emoc {

	Global::Global(const char *algorithn_name, const char *problem_name, int population_num,
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
		dec_lower_bound_(nullptr),
		dec_upper_bound_(nullptr),
		problem_(nullptr),
		algorithm_(nullptr),
		run_id_(run_id),
		thread_id_(thread_id)
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
		// get current emoc mode
		bool is_gui = EMOCManager::Instance()->GetIsGUI();
		bool is_experiment = EMOCManager::Instance()->GetIsExperiment();

		// When test module in gui mode, we need to update the EMOC state.
		if (is_gui &&!is_experiment)
		{
			EMOCManager::Instance()->SetTestPause(false);
			EMOCManager::Instance()->SetTestFinish(false);
		}

		algorithm_->Run();

		if (is_gui && !is_experiment)
			EMOCManager::Instance()->SetTestFinish(true);
	}

	void Global::Init()
	{
		AllocateMemory();

		// Because the initialization of algorithm needs the Global object has been created,
		// we need delay the following function after Global has been created and call it before Start().
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
		problem_ = ProblemFactory::Instance()->CreateProblemObject(problem_name_, dec_num_,obj_num_);
		if (problem_ == nullptr)
		{
			std::cout << "The problem name " << problem_name_ << " is wrong, please check it again" << std::endl;
			exit(1);
		}

		//// modify problem name to lower case
		//std::string problem_name(problem_name_);
		//for (auto &c : problem_name)
		//{
		//	if(c >= '0' && c <= '9') continue;
		//	c = tolower(c);
		//}

		////std::cout << problem_name << std::endl;
		//if (problem_name == "zdt1")
		//	problem_ = new ZDT1(dec_num_, obj_num_);
		//else if (problem_name == "zdt2")
		//	problem_ = new ZDT2(dec_num_, obj_num_);
		//else if (problem_name == "zdt3")
		//	problem_ = new ZDT3(dec_num_, obj_num_);
		//else if (problem_name == "zdt4")
		//	problem_ = new ZDT4(dec_num_, obj_num_);
		//else if (problem_name == "zdt6")
		//	problem_ = new ZDT6(dec_num_, obj_num_);
		//else if (problem_name == "dtlz1")
		//	problem_ = new DTLZ1(dec_num_, obj_num_);
		//else if (problem_name == "dtlz2")
		//	problem_ = new DTLZ2(dec_num_, obj_num_);
		//else if (problem_name == "dtlz3")
		//	problem_ = new DTLZ3(dec_num_, obj_num_);
		//else if (problem_name == "dtlz4")
		//	problem_ = new DTLZ4(dec_num_, obj_num_);
		//else if (problem_name == "dtlz5")
		//	problem_ = new DTLZ5(dec_num_, obj_num_);
		//else if (problem_name == "dtlz6")
		//	problem_ = new DTLZ6(dec_num_, obj_num_);
		//else if (problem_name == "dtlz7")
		//	problem_ = new DTLZ7(dec_num_, obj_num_);
		//else if (problem_name == "minusdtlz1")
		//	problem_ = new MinusDTLZ1(dec_num_, obj_num_);
		//else if (problem_name == "minusdtlz2")
		//	problem_ = new MinusDTLZ2(dec_num_, obj_num_);
		//else if (problem_name == "minusdtlz3")
		//	problem_ = new MinusDTLZ3(dec_num_, obj_num_);
		//else if (problem_name == "minusdtlz4")
		//	problem_ = new MinusDTLZ4(dec_num_, obj_num_);
		//else if (problem_name == "mdtlz1")
		//	problem_ = new MDTLZ1(dec_num_, obj_num_);
		//else if (problem_name == "mdtlz2")
		//	problem_ = new MDTLZ2(dec_num_, obj_num_);
		//else if (problem_name == "mdtlz3")
		//	problem_ = new MDTLZ3(dec_num_, obj_num_);
		//else if (problem_name == "mdtlz4")
		//	problem_ = new MDTLZ4(dec_num_, obj_num_);
		//else if (problem_name == "moeadde_f1")
		//	problem_ = new MOEADDE_F1(dec_num_, obj_num_);
		//else if (problem_name == "moeadde_f2")
		//	problem_ = new MOEADDE_F2(dec_num_, obj_num_);
		//else if (problem_name == "moeadde_f3")
		//	problem_ = new MOEADDE_F3(dec_num_, obj_num_);
		//else if (problem_name == "moeadde_f4")
		//	problem_ = new MOEADDE_F4(dec_num_, obj_num_);
		//else if (problem_name == "moeadde_f5")
		//	problem_ = new MOEADDE_F5(dec_num_, obj_num_);
		//else if (problem_name == "moeadde_f6")
		//	problem_ = new MOEADDE_F6(dec_num_, obj_num_);
		//else if (problem_name == "moeadde_f7")
		//	problem_ = new MOEADDE_F7(dec_num_, obj_num_);
		//else if (problem_name == "moeadde_f8")
		//	problem_ = new MOEADDE_F8(dec_num_, obj_num_);
		//else if (problem_name == "moeadde_f9")
		//	problem_ = new MOEADDE_F9(dec_num_, obj_num_);
		//else if (problem_name == "immoea_f1")
		//	problem_ = new IMMOEA_F1(dec_num_, obj_num_);
		//else if (problem_name == "immoea_f2")
		//	problem_ = new IMMOEA_F2(dec_num_, obj_num_);
		//else if (problem_name == "immoea_f3")
		//	problem_ = new IMMOEA_F3(dec_num_, obj_num_);
		//else if (problem_name == "immoea_f4")
		//	problem_ = new IMMOEA_F4(dec_num_, obj_num_);
		//else if (problem_name == "immoea_f5")
		//	problem_ = new IMMOEA_F5(dec_num_, obj_num_);
		//else if (problem_name == "immoea_f6")
		//	problem_ = new IMMOEA_F6(dec_num_, obj_num_);
		//else if (problem_name == "immoea_f7")
		//	problem_ = new IMMOEA_F7(dec_num_, obj_num_);
		//else if (problem_name == "immoea_f8")
		//	problem_ = new IMMOEA_F8(dec_num_, obj_num_);
		//else if (problem_name == "immoea_f9")
		//	problem_ = new IMMOEA_F9(dec_num_, obj_num_);
		//else if (problem_name == "immoea_f10")
		//	problem_ = new IMMOEA_F10(dec_num_, obj_num_);
		//else if (problem_name == "moeadm2m_f1")
		//	problem_ = new MOEADM2M_F1(dec_num_, obj_num_);
		//else if (problem_name == "moeadm2m_f2")
		//	problem_ = new MOEADM2M_F2(dec_num_, obj_num_);
		//else if (problem_name == "moeadm2m_f3")
		//	problem_ = new MOEADM2M_F3(dec_num_, obj_num_);
		//else if (problem_name == "moeadm2m_f4")
		//	problem_ = new MOEADM2M_F4(dec_num_, obj_num_);
		//else if (problem_name == "moeadm2m_f5")
		//	problem_ = new MOEADM2M_F5(dec_num_, obj_num_);
		//else if (problem_name == "moeadm2m_f6")
		//	problem_ = new MOEADM2M_F6(dec_num_, obj_num_);
		//else if (problem_name == "moeadm2m_f7")
		//	problem_ = new MOEADM2M_F7(dec_num_, obj_num_);
		//else if (problem_name == "uf1")
		//	problem_ = new UF1(dec_num_, obj_num_);
		//else if (problem_name == "uf2")
		//	problem_ = new UF2(dec_num_, obj_num_);
		//else if (problem_name == "uf3")
		//	problem_ = new UF3(dec_num_, obj_num_);
		//else if (problem_name == "uf4")
		//	problem_ = new UF4(dec_num_, obj_num_);
		//else if (problem_name == "uf5")
		//	problem_ = new UF5(dec_num_, obj_num_);
		//else if (problem_name == "uf6")
		//	problem_ = new UF6(dec_num_, obj_num_);
		//else if (problem_name == "uf7")
		//	problem_ = new UF7(dec_num_, obj_num_);
		//else if (problem_name == "uf8")
		//	problem_ = new UF8(dec_num_, obj_num_);
		//else if (problem_name == "uf9")
		//	problem_ = new UF9(dec_num_, obj_num_);
		//else if (problem_name == "uf10")
		//	problem_ = new UF10(dec_num_, obj_num_);
		//else if (problem_name == "wfg1")
		//	problem_ = new WFG1(dec_num_, obj_num_);
		//else if (problem_name == "wfg2")
		//	problem_ = new WFG2(dec_num_, obj_num_);
		//else if (problem_name == "wfg3")
		//	problem_ = new WFG3(dec_num_, obj_num_);
		//else if (problem_name == "wfg4")
		//	problem_ = new WFG4(dec_num_, obj_num_);
		//else if (problem_name == "wfg5")
		//	problem_ = new WFG5(dec_num_, obj_num_);
		//else if (problem_name == "wfg6")
		//	problem_ = new WFG6(dec_num_, obj_num_);
		//else if (problem_name == "wfg7")
		//	problem_ = new WFG7(dec_num_, obj_num_);
		//else if (problem_name == "wfg8")
		//	problem_ = new WFG8(dec_num_, obj_num_);
		//else if (problem_name == "wfg9")
		//	problem_ = new WFG9(dec_num_, obj_num_);
		//else if (problem_name == "lsmop1")
		//	problem_ = new LSMOP1(dec_num_, obj_num_);
		//else if (problem_name == "lsmop2")
		//	problem_ = new LSMOP2(dec_num_, obj_num_);
		//else if (problem_name == "lsmop3")
		//	problem_ = new LSMOP3(dec_num_, obj_num_);
		//else if (problem_name == "lsmop4")
		//	problem_ = new LSMOP4(dec_num_, obj_num_);
		//else if (problem_name == "lsmop5")
		//	problem_ = new LSMOP5(dec_num_, obj_num_);
		//else if (problem_name == "lsmop6")
		//	problem_ = new LSMOP6(dec_num_, obj_num_);
		//else if (problem_name == "lsmop7")
		//	problem_ = new LSMOP7(dec_num_, obj_num_);
		//else if (problem_name == "lsmop8")
		//	problem_ = new LSMOP8(dec_num_, obj_num_);
		//else if (problem_name == "lsmop9")
		//	problem_ = new LSMOP9(dec_num_, obj_num_);
		//else if (problem_name == "bt1")
		//	problem_ = new BT1(dec_num_, obj_num_);
		//else if (problem_name == "bt2")
		//	problem_ = new BT2(dec_num_, obj_num_);
		//else if (problem_name == "bt3")
		//	problem_ = new BT3(dec_num_, obj_num_);
		//else if (problem_name == "bt4")
		//	problem_ = new BT4(dec_num_, obj_num_);
		//else if (problem_name == "bt5")
		//	problem_ = new BT5(dec_num_, obj_num_);
		//else if (problem_name == "bt6")
		//	problem_ = new BT6(dec_num_, obj_num_);
		//else if (problem_name == "bt7")
		//	problem_ = new BT7(dec_num_, obj_num_);
		//else if (problem_name == "bt8")
		//	problem_ = new BT8(dec_num_, obj_num_);
		//else if (problem_name == "bt9")
		//	problem_ = new BT9(dec_num_, obj_num_);
		//else
		//{
		//	// TODO: move these parameter checking things into some specified function 
		//	std::cout <<problem_name<< " The problem name is wrong, please check it again" << std::endl;
		//	exit(1);
		//}
	}

	void Global::InitializeAlgorithm()
	{
		algorithm_ = AlgorithmFactory::Instance()->CreateAlgorithmObject(algorithm_name_, thread_id_);
		if (algorithm_ == nullptr)
		{
			std::cout << "The algorithm name " << algorithm_ << " is wrong, please check it again" << std::endl;
			exit(1);
		}
		//// modify problem name to lower case
		//std::string algorithm_name(algorithm_name_);
		//for (auto &c : algorithm_name)
		//{
		//	if (c >= '0' && c <= '9') continue;
		//	c = tolower(c);
		//}
		////std::cout << algorithm_name << std::endl;
		//if (algorithm_name == "nsga2")
		//	algorithm_ = new NSGA2(thread_id_);
		//else if (algorithm_name == "spea2")
		//	algorithm_ = new SPEA2(thread_id_);
		//else if (algorithm_name == "moead")
		//	algorithm_ = new MOEAD(thread_id_);
		//else if (algorithm_name == "moeadde")
		//	algorithm_ = new MOEADDE(thread_id_);
		//else if (algorithm_name == "moeaddra")
		//	algorithm_ = new MOEADDRA(thread_id_);
		//else if (algorithm_name == "moeadfrrmab")
		//	algorithm_ = new MOEADFRRMAB( thread_id_);
		//else if (algorithm_name == "ibea")
		//	algorithm_ = new IBEA(thread_id_);
		//else if (algorithm_name == "smsemoa")
		//	algorithm_ = new SMSEMOA(thread_id_);
		//else if (algorithm_name == "hype")
		//	algorithm_ = new HypE(thread_id_);
		//else if (algorithm_name == "ensmoead")
		//	algorithm_ = new ENSMOEAD(thread_id_);
		//else if (algorithm_name == "moeadgra")
		//	algorithm_ = new MOEADGRA(thread_id_);
		//else if (algorithm_name == "moeadira")
		//	algorithm_ = new MOEADIRA(thread_id_);
		//else if (algorithm_name == "moeaddyts")
		//	algorithm_ = new MOEADDYTS(thread_id_);
		//else if (algorithm_name == "moeadswts")
		//	algorithm_ = new MOEADSWTS(thread_id_);
		//else if (algorithm_name == "moeadcdts")
		//	algorithm_ = new MOEADCDTS(thread_id_);
		//else if (algorithm_name == "moeaddts")
		//	algorithm_ = new MOEADDTS(thread_id_);
		//else if (algorithm_name == "moeadcde")
		//	algorithm_ = new MOEADCDE(thread_id_);
		//else if (algorithm_name == "moeadstm")
		//	algorithm_ = new MOEADSTM(thread_id_);
		//else if (algorithm_name == "rvea")
		//	algorithm_ = new RVEA(thread_id_);
		//else if (algorithm_name == "moeadpas")
		//	algorithm_ = new MOEADPAS(thread_id_);
		//else if (algorithm_name == "moeadm2m")
		//	algorithm_ = new MOEADM2M(thread_id_);
		//else if (algorithm_name == "spea2sde")
		//	algorithm_ = new SPEA2SDE(thread_id_);
		//else if (algorithm_name == "tdea")
		//	algorithm_ = new tDEA(thread_id_);
		//else if (algorithm_name == "moeadd")
		//	algorithm_ = new MOEADD(thread_id_);
		//else
		//{
		//	// TODO: move these parameter checking things into some specified function 
		//	std::cout << "The algorithm name is wrong, please check it again" << std::endl;			
		//	exit(1);
		//}
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