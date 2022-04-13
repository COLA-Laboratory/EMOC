#include "problem/tsp/tsp.h"

#include <iostream>
#include <fstream>
#include <cmath>

#include "core/macro.h"
#include "core/global.h"
#include "core/file.h"
#include "random/random.h"

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

	TSP::TSP(int dec_num, int obj_num) :Problem(dec_num, obj_num)
	{
		Init();
		encoding_ = PERMUTATION;
	}

	TSP::~TSP()
	{

	}

	void TSP::CalObj(Individual* ind)
	{
		double total_dis = 0.0;
		for (int i = 0; i < dec_num_ - 1; i++)
		{
			int l = (int)(ind->dec_[i]);
			int r = (int)(ind->dec_[i + 1]);
			total_dis += distance_[l][r];
		}

		int end = (int)(ind->dec_[dec_num_ - 1]);
		int start = (int)(ind->dec_[0]);
		total_dis += distance_[end][start];
		ind->obj_[0] = total_dis;
	}

	void TSP::Init()
	{
		position_.resize(dec_num_, std::vector<double>(2, 0));
		distance_.resize(dec_num_, std::vector<double>(dec_num_, 0));

		int is_exist = 0;
		std::string path = "./output/problem_config/TSP_" + std::to_string(dec_num_) + "D.txt";

#if defined(_WIN32)
		is_exist = _access(path.data(), 0);
#elif defined(__linux) || defined(linux)
		is_exist = access(path.data(), F_OK);
#elif defined(__APPLE__)
		is_exist = access(path.data(), F_OK);
#endif
		if (is_exist == -1)
		{
			// create random city position
			for (int i = 0; i < dec_num_; i++)
			{
				position_[i][0] = randomperc();
				position_[i][1] = randomperc();
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
				{
					data_file << position_[i][0] << " " << position_[i][1] << "\n";
				}
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
				{
					data_file >> position_[i][0] >> position_[i][1];
				}
			}
			data_file.close();
		}

		// calculate distance between cities
		for (int i = 0; i < dec_num_; i++)
		{
			for (int j = 0; j < dec_num_; j++)
			{
				if (i == j)
				{
					distance_[i][j] = 0.0;
					continue;
				}

				double dis = (position_[i][0] - position_[j][0]) * (position_[i][0] - position_[j][0]) +
					(position_[i][1] - position_[j][1]) * (position_[i][1] - position_[j][1]);
				distance_[i][j] = std::sqrt(dis);
			}
		}
	}

}