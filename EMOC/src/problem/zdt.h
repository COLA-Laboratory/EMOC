#pragma once
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

	class ZDT2 :public Problem
	{
	public:
		ZDT2(int dec_num, int obj_num);
		virtual ~ZDT2();

		void CalObj(Individual *ind);
	};

	class ZDT3 :public Problem
	{
	public:
		ZDT3(int dec_num, int obj_num);
		virtual ~ZDT3();

		void CalObj(Individual *ind);
	};

	class ZDT4 :public Problem
	{
	public:
		ZDT4(int dec_num, int obj_num);
		virtual ~ZDT4();

		void CalObj(Individual *ind);
	};

	class ZDT6 :public Problem
	{
	public:
		ZDT6(int dec_num, int obj_num);
		virtual ~ZDT6();

		void CalObj(Individual *ind);
	};

}