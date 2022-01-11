#pragma once
#include "core/individual.h"
#include "problem/problem.h"

namespace emoc {

	class IMMOEA_F1 :public Problem
	{
	public:
		IMMOEA_F1(int dec_num, int obj_num);
		virtual ~IMMOEA_F1();

		void CalObj(Individual* ind);
	};

	class IMMOEA_F2 :public Problem
	{
	public:
		IMMOEA_F2(int dec_num, int obj_num);
		virtual ~IMMOEA_F2();

		void CalObj(Individual* ind);
	};

	class IMMOEA_F3 :public Problem
	{
	public:
		IMMOEA_F3(int dec_num, int obj_num);
		virtual ~IMMOEA_F3();

		void CalObj(Individual* ind);
	};

	class IMMOEA_F4 :public Problem
	{
	public:
		IMMOEA_F4(int dec_num, int obj_num);
		virtual ~IMMOEA_F4();

		void CalObj(Individual* ind);
	};

	class IMMOEA_F5 :public Problem
	{
	public:
		IMMOEA_F5(int dec_num, int obj_num);
		virtual ~IMMOEA_F5();

		void CalObj(Individual* ind);
	};

	class IMMOEA_F6 :public Problem
	{
	public:
		IMMOEA_F6(int dec_num, int obj_num);
		virtual ~IMMOEA_F6();

		void CalObj(Individual* ind);
	};

	class IMMOEA_F7 :public Problem
	{
	public:
		IMMOEA_F7(int dec_num, int obj_num);
		virtual ~IMMOEA_F7();

		void CalObj(Individual* ind);
	};

	class IMMOEA_F8 :public Problem
	{
	public:
		IMMOEA_F8(int dec_num, int obj_num);
		virtual ~IMMOEA_F8();

		void CalObj(Individual* ind);
	};

	class IMMOEA_F9 :public Problem
	{
	public:
		IMMOEA_F9(int dec_num, int obj_num);
		virtual ~IMMOEA_F9();

		void CalObj(Individual* ind);
	};

	class IMMOEA_F10 :public Problem
	{
	public:
		IMMOEA_F10(int dec_num, int obj_num);
		virtual ~IMMOEA_F10();

		void CalObj(Individual* ind);
	};
}