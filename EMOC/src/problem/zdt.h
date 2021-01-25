#pragma once
#include <cmath>

#include "core/individual.h"
#include "problem/problem.h"

namespace emoc {

	class ZDT1:public Problem
	{
	public:
		ZDT1(int dec_num, int obj_num);
		virtual ~ZDT1();

		void CalObj(Individual *ind);
	};

// 	class ZDT1
// 	{
// 	public:
// 		ZDT1(int dec_num, int obj_num) :
// 			dec_num_(dec_num), obj_num_(obj_num), lower_bound_(new double[dec_num]), upper_bound_(new double[dec_num])
// 		{
// 			for (int i = 0; i < dec_num; ++i)
// 			{
// 				lower_bound_[i] = 0.0;
// 				upper_bound_[i] = 1.0;
// 			}
// 		}
// 
// 		void CalObj(Individual *ind)
// 		{
// 			double f1 = 0, f2 = 0, sigema = 0;
// 
// 			f1 = ind->dec_[0];
// 			for (int i = 1; i < dec_num_; i++)
// 			{
// 				sigema += ind->dec_[i];
// 			}
// 
// 			double g = 1 + sigema * 9.0 / (dec_num_ - 1);
// 			double h = 1 - sqrt((double)(f1 / g));
// 			f2 = g * h;
// 
// 			ind->obj_[0] = f1;
// 			ind->obj_[1] = f2;
// 		}
// 
// 		virtual ~ZDT1()
// 		{
// 			std::cout << "ZDT1 deconstructor\n";
// 			delete[] lower_bound_;
// 			delete[] upper_bound_;
// 		}
// 
// 	public:
// 		int dec_num_;
// 		int obj_num_;
// 		double *lower_bound_;
// 		double *upper_bound_;
// 	};
}