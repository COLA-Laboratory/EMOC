#pragma once
#include "core/individual.h"
#include "problem/problem.h"

namespace emoc {

	class BT1 :public Problem
	{
	public:
		BT1(int dec_num, int obj_num);
		virtual ~BT1();

		void CalObj(Individual* ind);
	};

	class BT2 :public Problem
	{
	public:
		BT2(int dec_num, int obj_num);
		virtual ~BT2();

		void CalObj(Individual* ind);
	};

	class BT3 :public Problem
	{
	public:
		BT3(int dec_num, int obj_num);
		virtual ~BT3();

		void CalObj(Individual* ind);
	};

	class BT4 :public Problem
	{
	public:
		BT4(int dec_num, int obj_num);
		virtual ~BT4();

		void CalObj(Individual* ind);
	};

	class BT5 :public Problem
	{
	public:
		BT5(int dec_num, int obj_num);
		virtual ~BT5();

		void CalObj(Individual* ind);
	};

	class BT6 :public Problem
	{
	public:
		BT6(int dec_num, int obj_num);
		virtual ~BT6();

		void CalObj(Individual* ind);
	};

	class BT7 :public Problem
	{
	public:
		BT7(int dec_num, int obj_num);
		virtual ~BT7();

		void CalObj(Individual* ind);
	};

	class BT8 :public Problem
	{
	public:
		BT8(int dec_num, int obj_num);
		virtual ~BT8();

		void CalObj(Individual* ind);
	};

	class BT9 :public Problem
	{
	public:
		BT9(int dec_num, int obj_num);
		virtual ~BT9();

		void CalObj(Individual* ind);
	};
}