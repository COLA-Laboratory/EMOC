#pragma once
#include "core/individual.h"
#include "problem/problem.h"

namespace emoc {

	class DC1DTLZ1:public Problem
	{
	public:
		DC1DTLZ1(int dec_num, int obj_num);
		virtual ~DC1DTLZ1();

		void CalObj(Individual* ind);
		void CalCon(Individual* ind);
	};

	class DC1DTLZ3 :public Problem
	{
	public:
		DC1DTLZ3(int dec_num, int obj_num);
		virtual ~DC1DTLZ3();

		void CalObj(Individual* ind);
		void CalCon(Individual* ind);
	};

	class DC2DTLZ1 :public Problem
	{
	public:
		DC2DTLZ1(int dec_num, int obj_num);
		virtual ~DC2DTLZ1();

		void CalObj(Individual* ind);
		void CalCon(Individual* ind);
	};

	class DC2DTLZ3 :public Problem
	{
	public:
		DC2DTLZ3(int dec_num, int obj_num);
		virtual ~DC2DTLZ3();

		void CalObj(Individual* ind);
		void CalCon(Individual* ind);
	};

	class DC3DTLZ1 :public Problem
	{
	public:
		DC3DTLZ1(int dec_num, int obj_num);
		virtual ~DC3DTLZ1();

		void CalObj(Individual* ind);
		void CalCon(Individual* ind);
	};

	class DC3DTLZ3 :public Problem
	{
	public:
		DC3DTLZ3(int dec_num, int obj_num);
		virtual ~DC3DTLZ3();

		void CalObj(Individual* ind);
		void CalCon(Individual* ind);
	};

}