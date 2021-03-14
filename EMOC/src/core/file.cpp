#define _CRT_SECURE_NO_WARNINGS

#include "core/file.h"

#include <direct.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>

namespace emoc {

	void PrintObjective(const char *filename, int obj_num, Individual **pop_table, int pop_num)
	{
		FILE *fpt = fopen(filename, "w");

		for (int i = 0; i < pop_num; ++i)
		{
			Individual *ind = pop_table[i];
			for (int j = 0; j < obj_num; ++j)
				fprintf(fpt, "%lf\t", ind->obj_[j]);
			fprintf(fpt, "\n");
		}

		fclose(fpt);
	}

	void RecordPop(int run_index, int generation, Global *para)
	{
		char output_dir_level0[MAX_BUFFSIZE];
		char output_dir_level1[MAX_BUFFSIZE];    // upper level directory
		char output_dir_level2[MAX_BUFFSIZE];    // lower level directory
		char output_file[MAX_BUFFSIZE];
		int n = 0;
		// set the output directory


		sprintf(output_dir_level0, "./src/output/%s_M%d_D%d",
			para->problem_name_.c_str(),
			para->obj_num_,
			para->dec_num_

		);
		sprintf(output_dir_level1, "./src/output/%s_M%d_D%d/%s/",
			para->problem_name_.c_str(),
			para->obj_num_,
			para->dec_num_,
			para->algorithm_name_.c_str()
		);
		sprintf(output_dir_level2, "./src/output/%s_M%d_D%d/%s/%d/",
			para->problem_name_.c_str(),
			para->obj_num_,
			para->dec_num_,
			para->algorithm_name_.c_str(),
			run_index
		);

		_mkdir(output_dir_level0);
		_mkdir(output_dir_level1);
		_mkdir(output_dir_level2);

		sprintf(output_file, "%spop_%d.txt", output_dir_level2, generation);
		PrintObjective(output_file, para->obj_num_, para->parent_population_.data(), para->population_num_);
	}

	void ReadParametersFromFile(const char *filename, EMOCParameters *para)
	{
		int i = 0;
		char buff[MAX_BUFFSIZE] = { 0 };
		char line[MAX_BUFFSIZE] = { 0 };

		FILE *config = fopen(filename, "r");
		if (config == nullptr)
		{
			std::cout << "Fail to open the file, please check the path" << std::endl;
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
}