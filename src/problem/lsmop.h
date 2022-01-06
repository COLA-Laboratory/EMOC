#pragma once
#include "core/individual.h"
#include "problem/problem.h"

namespace emoc {

	class LSMOP :public Problem
	{
	public:
		LSMOP(int dec_num, int obj_num);
		virtual ~LSMOP();

		void Init();
		double sphere(int start, int end);
		double schwefel_yita2(int start, int end);
		double ronsenbrock_yita3(int start, int end);
		double rastrigin_yita4(int start, int end);
		double griewank_yita5(int start, int end);
		double ackley_yita6(int start, int end);
		double CalLSMOP1g(int index);
		double CalLSMOP2g(int index);
		double CalLSMOP3g(int index);
		double CalLSMOP4g(int index);
		double CalLSMOP5g(int index);
		double CalLSMOP6g(int index);
		double CalLSMOP7g(int index);
		double CalLSMOP8g(int index);
		double CalLSMOP9g(int index);

	public:
		double nk = 5.0;
		double* c = nullptr;
		double* g = nullptr;
		int *sublen = nullptr;
		int *len = nullptr;
		double *variable = nullptr;
		double* lsmop9_sum = nullptr;
	};

	class LSMOP1 :public LSMOP
	{
	public:
		LSMOP1(int dec_num, int obj_num);
		virtual ~LSMOP1();

		void CalObj(Individual* ind);
	};

	class LSMOP2 :public LSMOP
	{
	public:
		LSMOP2(int dec_num, int obj_num);
		virtual ~LSMOP2();

		void CalObj(Individual* ind);
	};

	class LSMOP3 :public LSMOP
	{
	public:
		LSMOP3(int dec_num, int obj_num);
		virtual ~LSMOP3();

		void CalObj(Individual* ind);
	};

	class LSMOP4 :public LSMOP
	{
	public:
		LSMOP4(int dec_num, int obj_num);
		virtual ~LSMOP4();

		void CalObj(Individual* ind);
	};

	class LSMOP5 :public LSMOP
	{
	public:
		LSMOP5(int dec_num, int obj_num);
		virtual ~LSMOP5();

		void CalObj(Individual* ind);
	};

	class LSMOP6 :public LSMOP
	{
	public:
		LSMOP6(int dec_num, int obj_num);
		virtual ~LSMOP6();

		void CalObj(Individual* ind);
	};

	class LSMOP7 :public LSMOP
	{
	public:
		LSMOP7(int dec_num, int obj_num);
		virtual ~LSMOP7();

		void CalObj(Individual* ind);
	};

	class LSMOP8 :public LSMOP
	{
	public:
		LSMOP8(int dec_num, int obj_num);
		virtual ~LSMOP8();

		void CalObj(Individual* ind);
	};

	class LSMOP9 :public LSMOP
	{
	public:
		LSMOP9(int dec_num, int obj_num);
		virtual ~LSMOP9();

		void CalObj(Individual* ind);
	};
}