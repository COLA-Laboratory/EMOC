#define _CRT_SECURE_NO_WARNINGS

#include "core/file.h"

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <fstream>


#if defined(_WIN32)
#include <direct.h>
#include <io.h>
#elif defined(__linux) || defined(linux)
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
#endif

#include "core/emoc_manager.h"

namespace emoc {

	static void SetParameter(char *parameter_name, char *value, EMOCParameters *para)
	{
		if (*parameter_name != '-')
		{
			std::cout << "The parameter name should begin with '-'." << std::endl;
			std::cout << "Press enter to exit" << std::endl;
			std::cin.get();
			exit(-1);
		}

		parameter_name++;
		if (!strcmp(parameter_name, "algorithm"))
		{
			para->algorithm_name = value;
		}
		else if (!strcmp(parameter_name, "problem"))
		{
			para->problem_name = value;
		}
		else if (!strcmp(parameter_name, "D"))
		{
			para->decision_num = atoi(value);
		}
		else if (!strcmp(parameter_name, "M"))
		{
			para->objective_num = atoi(value);
		}
		else if (!strcmp(parameter_name, "N"))
		{
			para->population_num = atoi(value);
		}
		else if (!strcmp(parameter_name, "evaluation"))
		{
			para->max_evaluation = atoi(value);
		}
		else if (!strcmp(parameter_name, "save"))
		{
			para->output_interval = atoi(value);
		}
		else if (!strcmp(parameter_name, "run"))
		{
			para->runs_num = atoi(value);
		}
		else if (!strcmp(parameter_name, "thread"))
		{
			para->is_open_multithread = 1;
			para->thread_num = atoi(value);
		}
		else
		{
			std::cout << "Set a nonexistent parameter name." << std::endl;
			std::cout << "The parameters should be set like '-parameter_name value', e.g. '-algorithm nsga2'." << std::endl;
			std::cout << "Press enter to exit" << std::endl;
			std::cin.get();
			exit(-1);
		}
	}

	void PrintObjective(const char *filename, int obj_num, Individual **pop_table, int pop_num)
	{
		FILE *fpt = fopen(filename, "w");
		
		if(fpt == nullptr)
		{
			std::cout <<filename<< " doesn't exist." << std::endl;
			std::cout << "Press enter to exit" << std::endl;
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

		fclose(fpt);
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

		PrintObjective(output_file, para->obj_num_, para->parent_population_.data(), real_popnum);
	}

	void ReadParametersFromFile(const char *filename, EMOCParameters *para)
	{
		int i = 0;
		char buff[MAX_BUFFSIZE] = { 0 };
		char line[MAX_BUFFSIZE] = { 0 };

		FILE *config = fopen(filename, "r");
		if (config == nullptr)
		{
			std::cout << "Fail to open the config file, please check the file path." << std::endl;
			std::cout << "Press enter to exit" << std::endl;
			std::cin.get();
			exit(-1);
		}

		while (!feof(config))
		{

			fgets(buff, MAX_BUFFSIZE, config);
			FormalizeStr(buff);
			for (i = 0; i < strlen(buff); i++)
			{
				if (buff[i] == ':')
				{
					buff[i] = 0;
					break;
				}
			}

			i++;
			if (!strcmp(buff, "algorithm_name"))
			{
				para->algorithm_name = buff + i;
			}
			else if (!strcmp(buff, "problem_name"))
			{
				para->problem_name = buff + i;
			}
			else if (!strcmp(buff, "variable_number"))
			{
				para->decision_num = atoi(buff + i);
			}
			else if (!strcmp(buff, "objective_number"))
			{
				para->objective_num = atoi(buff + i);
			}
			else if (!strcmp(buff, "population_number"))
			{
				para->population_num = atoi(buff + i);
			}
			else if (!strcmp(buff, "max_evaluation"))
			{
				para->max_evaluation = atoi(buff + i);
			}
			else if (!strcmp(buff, "output_interval"))
			{
				para->output_interval = atoi(buff + i);
			}
			else if (!strcmp(buff, "runs_number"))
			{
				para->runs_num = atoi(buff + i);
			}
			else if (!strcmp(buff, "open_multithread"))
			{
				para->is_open_multithread = atoi(buff + i);
			}
			else if (!strcmp(buff, "thread_number"))
			{
				para->thread_num = atoi(buff + i);
			}
			else
			{
				std::cout << "Input a wrong parameter, please check the parameter name" << std::endl;
				std::cout << "Press enter to exit" << std::endl;
				std::cin.get();
				exit(-1);
			}

		}

		fclose(config);
	}

	void FormalizeStr(char *buff)
	{
		int i = 0, j = 0, len = 0;

		if (NULL == buff)
		{
			return;
		}

		len = strlen(buff);

		for (i = 0; i < len; i++)
		{
			switch (buff[i])
			{
			case ' ':
			case '\n':
				for (j = i; j < len; j++)
				{
					buff[j] = buff[j + 1];
				}
				break;
			case '\r':
				buff[i] = 0;
				break;
			default:
				break;
			}
		}
	}

	void ParseParamerters(int argc, char *argv[], EMOCParameters *para)
	{
		
		// defalut value
		para->algorithm_name = "SPEA2";
		para->problem_name = "ZDT1";
		para->is_plot = false;
		para->population_num = 100;
		para->decision_num = 30;
		para->objective_num = 2;
		para->max_evaluation = 25000;
		para->output_interval = 1000000;	// no output except the first and last gerneration
		para->runs_num = 20;
		para->is_open_multithread = 0;
		para->thread_num = 8;

		// parse parameter from command line
		if (argc == 1)
			return;

		if (argc % 2 == 0)
		{
			std::cout << "The number of the parameter name is not matching the number of value." << std::endl;
			std::cout << "The parameters should be set like '-parameter_name value', e.g. '-algorithm nsga2'." << std::endl;
			std::cout << "Press enter to exit" << std::endl;
			std::cin.get();
			exit(-1);
		}
		else
		{
			char *first_parameter_name = argv[1] + 1;

			// read from file
			if (!strcmp(first_parameter_name, "file"))
			{
				ReadParametersFromFile(argv[2], para);
			}
			else
			{
				for (int i = 0; i < argc / 2; ++i)
				{
					char *parameter_name = argv[2 * i + 1];
					char *value = argv[2 * i + 2];
					SetParameter(parameter_name, value, para);
				}
			}
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
#endif

					//if (ret == -1)
					//{
					//	std::cerr << "CREATE SAVE DIRECTORY FAIL!\n";
					//	return 0;
					//}
				}
			}
		}
		return 1;
	}
	
}