#pragma once
#include "core/individual.h"
#include "problem/problem.h"

namespace emoc {

	class UF1 :public Problem
	{
	public:
		UF1(int dec_num, int obj_num);
		virtual ~UF1();

		void CalObj(Individual *ind);
	};

	class UF2 :public Problem
	{
	public:
		UF2(int dec_num, int obj_num);
		virtual ~UF2();

		void CalObj(Individual *ind);
	};

	class UF3 :public Problem
	{
	public:
		UF3(int dec_num, int obj_num);
		virtual ~UF3();

		void CalObj(Individual *ind);
	};

	class UF4 :public Problem
	{
	public:
		UF4(int dec_num, int obj_num);
		virtual ~UF4();

		void CalObj(Individual *ind);
	};

	class UF5 :public Problem
	{
	public:
		UF5(int dec_num, int obj_num);
		virtual ~UF5();

		void CalObj(Individual *ind);
	};

	class UF6 :public Problem
	{
	public:
		UF6(int dec_num, int obj_num);
		virtual ~UF6();

		void CalObj(Individual *ind);
	};

	class UF7 :public Problem
	{
	public:
		UF7(int dec_num, int obj_num);
		virtual ~UF7();

		void CalObj(Individual *ind);
	};

	class UF8 :public Problem
	{
	public:
		UF8(int dec_num, int obj_num);
		virtual ~UF8();

		void CalObj(Individual *ind);
	};

	class UF9 :public Problem
	{
	public:
		UF9(int dec_num, int obj_num);
		virtual ~UF9();

		void CalObj(Individual *ind);
	};

	class UF10 :public Problem
	{
	public:
		UF10(int dec_num, int obj_num);
		virtual ~UF10();

		void CalObj(Individual *ind);
	};
}