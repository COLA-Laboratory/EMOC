#pragma once
#include "core/individual.h"
#include "problem/problem.h"

namespace emoc {

	class WFG1 :public Problem
	{
	public:
		WFG1(int dec_num, int obj_num);
		virtual ~WFG1();

		void CalObj(Individual *ind);
	};

	class WFG2 :public Problem
	{
	public:
		WFG2(int dec_num, int obj_num);
		virtual ~WFG2();

		void CalObj(Individual *ind);
	};

	class WFG3 :public Problem
	{
	public:
		WFG3(int dec_num, int obj_num);
		virtual ~WFG3();

		void CalObj(Individual *ind);
	};

	class WFG4 :public Problem
	{
	public:
		WFG4(int dec_num, int obj_num);
		virtual ~WFG4();

		void CalObj(Individual *ind);
	};

	class WFG5 :public Problem
	{
	public:
		WFG5(int dec_num, int obj_num);
		virtual ~WFG5();

		void CalObj(Individual *ind);
	};

	class WFG6 :public Problem
	{
	public:
		WFG6(int dec_num, int obj_num);
		virtual ~WFG6();

		void CalObj(Individual *ind);
	};

	class WFG7 :public Problem
	{
	public:
		WFG7(int dec_num, int obj_num);
		virtual ~WFG7();

		void CalObj(Individual *ind);
	};

	class WFG8 :public Problem
	{
	public:
		WFG8(int dec_num, int obj_num);
		virtual ~WFG8();

		void CalObj(Individual *ind);
	};

	class WFG9 :public Problem
	{
	public:
		WFG9(int dec_num, int obj_num);
		virtual ~WFG9();

		void CalObj(Individual *ind);
	};
}