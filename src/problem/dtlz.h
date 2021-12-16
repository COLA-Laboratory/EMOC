#pragma once
#include "problem/problem.h"

namespace emoc {

	class DTLZ1 :public Problem
	{
	public:
		DTLZ1(int dec_num, int obj_num);
		virtual ~DTLZ1();

		void CalObj(Individual *ind);
	};

	class DTLZ2 :public Problem
	{
	public:
		DTLZ2(int dec_num, int obj_num);
		virtual ~DTLZ2();

		void CalObj(Individual *ind);
	};

	class DTLZ3 :public Problem
	{
	public:
		DTLZ3(int dec_num, int obj_num);
		virtual ~DTLZ3();

		void CalObj(Individual *ind);
	};

	class DTLZ4 :public Problem
	{
	public:
		DTLZ4(int dec_num, int obj_num);
		virtual ~DTLZ4();

		void CalObj(Individual *ind);
	};

	class DTLZ5 :public Problem
	{
	public:
		DTLZ5(int dec_num, int obj_num);
		virtual ~DTLZ5();

		void CalObj(Individual *ind);

	private:
		double *theta;
	};

	class DTLZ6 :public Problem
	{
	public:
		DTLZ6(int dec_num, int obj_num);
		virtual ~DTLZ6();

		void CalObj(Individual *ind);
		
	private:
		double *theta;
	};

	class DTLZ7 :public Problem
	{
	public:
		DTLZ7(int dec_num, int obj_num);
		virtual ~DTLZ7();

		void CalObj(Individual *ind);
	};


	class MDTLZ1 :public Problem
	{
	public:
		MDTLZ1(int dec_num, int obj_num);
		virtual ~MDTLZ1();

		void CalObj(Individual* ind);
	};

	class MDTLZ2 :public Problem
	{
	public:
		MDTLZ2(int dec_num, int obj_num);
		virtual ~MDTLZ2();

		void CalObj(Individual* ind);
	};

	class MDTLZ3 :public Problem
	{
	public:
		MDTLZ3(int dec_num, int obj_num);
		virtual ~MDTLZ3();

		void CalObj(Individual* ind);
	};

	class MDTLZ4 :public Problem
	{
	public:
		MDTLZ4(int dec_num, int obj_num);
		virtual ~MDTLZ4();

		void CalObj(Individual* ind);
	};
}