#define _CRT_SECURE_NO_WARNINGS

#include "core/file.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <fstream>
#include <thread>

#if defined(_WIN32)
#include <direct.h>
#include <io.h>
#elif defined(__linux) || defined(linux)
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
#include <unistd.h>
#elif defined(__APPLE__)// macos
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
#include <unistd.h>
#endif

#include "emoc_app.h"
#include "ui/ui_utility.h"
#include "core/macro.h"
#include "core/emoc_manager.h"
#include "core/emoc_utility_structures.h"
#include "cxxopts/cxxopts.hpp"
#include "algorithm/algorithm_factory.h"
#include "problem/problem_factory.h"

namespace emoc {

	static void PrintObjective(const char *filename, int obj_num, Individual **pop_table, int pop_num)
	{
		FILE *fpt = fopen(filename, "w");
		
		if(fpt == nullptr)
		{
			std::cout <<"Can not open "<< filename << " !." << std::endl;
			std::cout << "Press enter to exit" << std::endl;
			std::cout << strerror(errno) << "\n";
			std::cin.get();
			exit(-1);
		}

		for (int i = 0; i < pop_num; ++i)
		{
			Individual *ind = pop_table[i];
			for (int j = 0; j < obj_num; ++j)
				fprintf(fpt, "%lf\t", ind->obj_[j]);
			fprintf(fpt, "\n");
		}

		fflush(fpt);
		fclose(fpt);
	}

	static void PrintDecision(const char* filename, int dec_num, Individual** pop_table, int pop_num)
	{
		FILE* fpt = fopen(filename, "w");

		if (fpt == nullptr)
		{
			std::cout << "Can not open " << filename << " !." << std::endl;
			std::cout << "Press enter to exit" << std::endl;
			std::cout << strerror(errno) << "\n";
			std::cin.get();
			exit(-1);
		}

		for (int i = 0; i < pop_num; ++i)
		{
			Individual* ind = pop_table[i];
			for (int j = 0; j < dec_num; ++j)
				fprintf(fpt, "%lf\t", ind->dec_[j]);
			fprintf(fpt, "\n");
		}

		fflush(fpt);
		fclose(fpt);
	}

	static void PrintCityPosition(const char* filename, int dec_num, Individual** pop_table,
		std::vector<std::vector<double>> positions, int pop_num)
	{
		FILE* fpt = fopen(filename, "w");

		if (fpt == nullptr)
		{
			std::cout << "Can not open " << filename << " !." << std::endl;
			std::cout << "Press enter to exit" << std::endl;
			std::cout << strerror(errno) << "\n";
			std::cin.get();
			exit(-1);
		}

		Individual* ind = pop_table[0];
		for (int i = 0; i < dec_num; ++i)
		{
			int city_id = (int)(ind->dec_[i]);
			for(int j = 0;j < positions[city_id].size();j++)
				fprintf(fpt, "%lf\t", positions[city_id][j]);
			fprintf(fpt, "\n");
		}

		int start_city_id = (int)(ind->dec_[0]);
		for (int j = 0; j < positions[start_city_id].size(); j++)
			fprintf(fpt, "%lf\t", positions[start_city_id][j]);
		

		fflush(fpt);
		fclose(fpt);
	}


	void CopyFile(const char* src, const char* dest )
	{
		FILE* in = fopen(src, "r+");
		FILE* out = fopen(dest, "w+");

		char buff[1024];
		while (int len = fread(buff, 1, sizeof(buff), in))
		{
			fwrite(buff, 1, len, out);
		}

		fclose(in);
		fclose(out);
	}

	std::vector<std::vector<double>> ReadPop(char* filepath, int obj_num)
	{
		// open pf data file
		int pop_num = 0;
		std::vector<std::vector<double>> data;
		std::fstream data_file(filepath);


		if (!data_file)
		{
			std::cerr << filepath << " file doesn't exist!\n";
		}
		else
		{
			std::string line;
			while (getline(data_file, line))
				pop_num++;


			// read the pf data
			data_file.clear();
			data_file.seekg(0);
			for (int i = 0; i < pop_num; i++)
			{
				data.push_back(std::vector<double>(obj_num, 0));
				for (int j = 0; j < obj_num; j++)
				{
					data_file >> data[i][j];
				}
			}
		}
		return data;
	}

	void RecordPop(int run_index, int generation, Global* para, int real_popnum)
	{
		char output_dir[1024];
		char output_file[1024];

		// set the output directory
		std::string problem_name(para->problem_name_);
		std::string algorithm_name(para->algorithm_name_);
		for (auto &c : problem_name)
		{
			if (c >= '0' && c <= '9') continue;
			c = toupper(c);
		}
		for (auto &c : algorithm_name)
		{
			if (c >= '0' && c <= '9') continue;
			c = toupper(c);
		}

		if (EMOCManager::Instance()->GetIsExperiment())
		{
			sprintf(output_dir, "./output/experiment_module/%s_M%d_D%d/%s/%d/",
				problem_name.c_str(),
				para->obj_num_,
				para->dec_num_,
				algorithm_name.c_str(),
				run_index
			);
		}
		else
		{
			sprintf(output_dir, "./output/test_module/run%d/",
				EMOCManager::Instance()->GetSingleThreadResultSize()
			);
		}
		CreateDirectory(output_dir);
		sprintf(output_file, "%spop_%d.txt", output_dir, generation);

		if (para->problem_->encoding_ == Problem::REAL)
			PrintObjective(output_file, para->obj_num_, para->parent_population_.data(), real_popnum);
		else if(para->problem_->encoding_ == Problem::BINARY)
			PrintDecision(output_file, para->dec_num_, para->parent_population_.data(), real_popnum);
		else if (para->problem_->encoding_ == Problem::PERMUTATION)
		{
			if (para->problem_name_ == "TSP")
			{
				static bool is_read = false;
				static std::vector<std::vector<double>> positions(para->dec_num_, std::vector<double>(2, 0));

				// reset the data when a new run starts
				if (para->iteration_num_ == 0)
				{
					is_read = false;
					positions.resize(para->dec_num_, std::vector<double>(2, 0));
				}

				if (!is_read)
				{
					std::string path = "./output/problem_config/TSP_" + std::to_string(para->dec_num_) + "D.txt";
					std::fstream data_file(path);
					if (!data_file)
					{
						std::cerr << path << " file doesn't exist!\n";
					}
					else
					{
						for (int i = 0; i < para->dec_num_; i++)
						{
							data_file >> positions[i][0] >> positions[i][1];
						}
					}
					data_file.close();
					is_read = true;
				}
				PrintCityPosition(output_file, para->dec_num_, para->parent_population_.data(), positions, real_popnum);
			}
			else
				PrintObjective(output_file, para->obj_num_, para->parent_population_.data(), real_popnum);
		}
	}

	void EMOCParamerterParse(int argc, char* argv[], EMOCParameters& para, bool& is_gui)
	{
		try
		{
			cxxopts::Options options(argv[0], "-----Evolutionary Multiobjective Optimization Implementation in C++11-----\n"
				"EMOC can be excute with/without gui by \"-g or --gui\".\nIt is worth noting that when use the gui mode the other parameters will be invalid.\n");
			options
				.set_width(80)
				.allow_unrecognised_options()
				.add_options()
				("h,help", "Print help")
				("g,gui", "Whether use gui mode", cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
				("a,algorithm", "Algorithm name", cxxopts::value<std::string>()->default_value("NSGA2"))
				("p,problem", "Problem name", cxxopts::value<std::string>()->default_value("ZDT1"))
				("N,pop", "Population size", cxxopts::value<int>()->default_value("100"))
				("m,obj", "Problem's objective dimension", cxxopts::value<int>()->default_value("2"))
				("n,dec", "Problem's decision variable dimension", cxxopts::value<int>()->default_value("30"))
				("e,evaluation", "Evaluation number for each run", cxxopts::value<int>()->default_value("25000"))
				("i,interval", "Population save interval", cxxopts::value<int>()->default_value("100"))
				("r,run", "The number of runs", cxxopts::value<int>()->default_value("1"))
				("multithread", "Whether open multi-thread mode", cxxopts::value<bool>()->default_value("false"))
				("t,thread", "Thread number in multi-thread mode", cxxopts::value<int>()->default_value("4"));

			auto result = options.parse(argc, argv);

			if (result.count("help"))
			{
				std::cout << options.help() << std::endl;
				exit(0);
			}

			is_gui = result["gui"].as<bool>();
			para.algorithm_name = result["algorithm"].as<std::string>();
			para.problem_name = result["problem"].as<std::string>();
			para.population_num = result["pop"].as<int>();
			para.decision_num = result["dec"].as<int>();
			para.objective_num = result["obj"].as<int>();
			para.max_evaluation = result["evaluation"].as<int>();
			para.output_interval = result["interval"].as<int>();
			para.runs_num = result["run"].as<int>();
			para.is_open_multithread = result["multithread"].as<bool>();
			para.thread_num = result["thread"].as<int>();
		}
		catch (const cxxopts::OptionException& e)
		{
			std::cout << "error parsing options: " << e.what() << std::endl;
			exit(1);
		}

		if (!is_gui)
		{
			std::string description;
			// parameter validity check
			bool is_valid = CheckEMOCParameter(para, description);
			
			if (!is_valid)
			{
				std::cout << "EMOC WRONG PARAMETER ERROR:\n";
				std::cout << description;
				std::cout << "EMOC EXIT\n";
				exit(1);
			}

			// print EMOC Task
			std::cout << "------------EMOC Task------------\n"
				<< "Algorithm:       " << para.algorithm_name << "\n"
				<< "Problem:         " << para.problem_name << "\n"
				<< "Population:      " << para.population_num << "\n"
				<< "Objective:       " << para.objective_num << "\n"
				<< "Decision:        " << para.decision_num << "\n"
				<< "Evaluation:      " << para.max_evaluation << "\n"
				<< "Save Interval:   " << para.output_interval << "\n"
				<< "Runs:            " << para.runs_num << "\n"
				<< "Multi-thread:    " << para.is_open_multithread << "\n";
			if (para.is_open_multithread)
				std::cout << "Thread Number:   " << para.thread_num << "\n";
			std::cout << "---------------------------------\n\n";
		}
	}

	int CreateDirectory(const std::string& path)
	{
		int len = path.length();
		char tmpDirPath[512] = { 0 };
		for (int i = 0; i < len; i++)
		{
			tmpDirPath[i] = path[i];
			if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/')
			{
#if defined(_WIN32)
				if (_access(tmpDirPath, 0) == -1)
				{
					int ret = _mkdir(tmpDirPath);
#elif defined(__linux) || defined(linux)
				if (access(tmpDirPath, F_OK) == -1)
				{
					int ret = mkdir(tmpDirPath, S_IRWXU | S_IRWXG | S_IRWXO);
#elif defined(__APPLE__)
				if (access(tmpDirPath, F_OK) == -1)
				{
					int ret = mkdir(tmpDirPath, S_IRWXU | S_IRWXG | S_IRWXO);
#endif
				}
			}
		}
		return 1;
	}

	bool CheckEMOCParameter(const EMOCParameters& para, std::string& description)
	{
		bool res = true;
		std::string algorithm = para.algorithm_name;
		std::string problem = para.problem_name;
		int N = para.population_num;
		int D = para.decision_num;
		int M = para.objective_num;
		int evaluation = para.max_evaluation;
		int save_interval = para.output_interval;
		int runs_num = para.runs_num;
		bool is_multithread = para.is_open_multithread;
		int thread_num = para.thread_num;
		int max_threadnum = std::thread::hardware_concurrency();

		// basic checking
		if (thread_num < 0)
		{
			description = "The number of thread cannot be negative integer!\n\n";
			res = false;
		}
		else if (thread_num > max_threadnum)
		{
			description = "We recommend the number of thread should not beyond your cpu cores:" + std::to_string(max_threadnum) + "!\n\n";
			res = false;
		}
		if (runs_num < 0)
		{
			description = "The number of runs cannot be negative integer!\n\n";
			res = false;
		}
		if (save_interval < 0)
		{
			description = "The interval of population save cannot be negative integer!\n\n";
			res = false;
		}
		
		bool is_valid1 = EMOCAlgorithmCheck(algorithm, description);
		bool is_valid2 = EMOCProblemCheck(problem, M, D, N, evaluation, description);

		return res && is_valid1 && is_valid2;
	}

	bool EMOCAlgorithmCheck(const std::string& algorithm, std::string& description)
	{
		bool res = false;

		std::unordered_map<std::string, std::vector<char*>>& IMPLEMENTED_ALGORITHMS = AlgorithmFactory::Instance()->GetImplementedAlgorithmsName();
		for (const auto& e : IMPLEMENTED_ALGORITHMS)
		{
			for (const auto& v : e.second)
			{
				if (algorithm == v)
				{
					res = true;
					break;
				}
			}
			if (res) break;
		}

		if (res == false)
			description = "Algorithm " + algorithm + " is not implemented in EMOC yet, please refer to the document.\n\n";

		return res;
	}

	bool EMOCProblemCheck(const std::string& problem, int M, int D, int N, int Evaluation, std::string& description)
	{
		bool res = false;

		std::unordered_map<std::string, std::vector<char*>>& IMPLEMENTED_PROBLEMS = ProblemFactory::Instance()->GetImplementedProblemsName();
		for (const auto& e : IMPLEMENTED_PROBLEMS)
		{
			for (const auto& v : e.second)
			{
				if (problem == v)
				{
					res = true;
					break;
				}
			}

			if (res) break;
		}

		if (res == false)
			description = "Problem " + problem + " is not implemented in EMOC yet, please refer to the document.\n\n";

		bool is_valid = CheckProblemParameters(problem, D, M, N, Evaluation, description);

		return res && is_valid;
	}

}
