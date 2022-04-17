#include "problem/kp/knapsack.h"

#include <iostream>
#include <fstream>
#include <cmath>

#include "core/macro.h"
#include "core/global.h"
#include "random/random.h"
#include "core/file.h"

#if defined(_WIN32)
#include <direct.h>
#include <io.h>
#elif defined(__linux) || defined(linux) || defined(__APPLE__)
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
#include <unistd.h>
#endif

namespace emoc {

	Knapsack::Knapsack(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		Init();
		encoding_ = BINARY;
	}

	Knapsack::~Knapsack()
	{

	}

	void Knapsack::CalObj(Individual* ind)
	{
		double sum = 0.0;
		double total_profit = 0.0;
		for (int i = 0; i < dec_num_; i++)
		{
			sum += profit_[i];
			total_profit += ind->dec_[i] * profit_[i];
		}

		// For minimization, we reverse the profit.
		ind->obj_[0] = sum - total_profit;
	}

	void Knapsack::CalCon(Individual* ind)
	{
		double sum = 0.0;
		double total_weight = 0.0;
		for (int i = 0; i < dec_num_; i++)
		{
			sum += weight_[i];
			total_weight += ind->dec_[i] * weight_[i];
		}
		ind->con_[0] = total_weight - sum / 2.0;
	}

	void Knapsack::Init()
	{
		profit_.resize(dec_num_, 0);
		weight_.resize(dec_num_, 0);

		int is_exist = 0;
		std::string path = "./output/problem_config/KP_" + std::to_string(dec_num_) + "D.txt";

#if defined(_WIN32)
		is_exist = _access(path.data(), 0);
#elif defined(__linux) || defined(linux)
		is_exist = access(path.data(), F_OK);
#elif defined(__APPLE__)
		is_exist = access(path.data(), F_OK);
#endif
		if (is_exist == -1)
		{
			// create random weight and profit
			for (int i = 0; i < dec_num_; i++)
			{
				profit_[i] = rnd(10, 100);
				weight_[i] = rnd(10, 100);
			}

			// save
			CreateDirectory(path);
			std::fstream data_file(path, std::ios::out);
			if (!data_file)
			{
				std::cerr << path << " open file failed!\n";
			}
			else
			{
				for (int i = 0; i < dec_num_; i++)
					data_file << profit_[i] << " ";
				data_file << "\n";
				for (int i = 0; i < dec_num_; i++)
					data_file << weight_[i] << " ";
				data_file << "\n";
			}
			data_file.close();
		}
		else
		{
			std::fstream data_file(path);
			if (!data_file)
			{
				std::cerr << path << " file doesn't exist!\n";
			}
			else
			{
				for (int i = 0; i < dec_num_; i++)
					data_file >> profit_[i];
				for (int i = 0; i < dec_num_; i++)
					data_file >> weight_[i];
			}
			data_file.close();
		}

		// debug
		//std::cout << "profit: ";
		//for (int i = 0; i < dec_num_; i++)
		//{
		//	std::cout << profit_[i] << " ";
		//}
		//std::cout << " \n";
		//std::cout << "weight: ";
		//for (int i = 0; i < dec_num_; i++)
		//{
		//	std::cout << weight_[i] << " ";
		//}
		//std::cout << " \n";
	}

}