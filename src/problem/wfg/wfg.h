#pragma once
#include "core/individual.h"
#include "problem/problem.h"



namespace emoc {

	class WFG : public Problem
	{
	public:
		WFG(int dec_num, int obj_num) : Problem(dec_num, obj_num) {}
		virtual ~WFG() {};

		void WFG_ini();
		void WFG_free();
		void WFG_normalise(double* z, int z_size, double* result);
		void calculate_x(double* x, double* result, int size);
		void calculate_f(double D, double x, double* h, int size, double* result);
		double linear(double* x, int M, int m);
		double convex(double* x, int x_size, int m);
		double concave(double* x, int x_size, int m);
		double mixed(double* x, int A, double alpha);
		double disc(double* x, int A, double alpha, double beta);
		double b_poly(double y, double alpha);
		double min(double a, double b);
		double b_flat(double y, double A, double B, double C);
		double b_param(double y, double u, double A, double B, double C);
		double s_linear(double y, double A);
		double s_decept(double y, double A, double B, double C);
		double s_multi(double y, int A, double B, double C);
		double r_sum(double* y, int y_size, double* w, int w_size);
		double r_nonsep(double* y, int y_size, const int A);
		int WFG1_t1(double* y, int y_size, int k, double* result);
		int WFG1_t2(double* y, int y_size, int k, double* result);
		int WFG1_t3(double* y, int y_size, double* result);
		int WFG1_t4(double* y, int y_size, int k, int M, double* result);
		int WFG2_t2(double* y, int y_size, int k, double* result);
		int WFG2_t3(double* y, int y_size, int k, int M, double* result);
		int WFG4_t1(double* y, int y_size, double* result);
		int WFG5_t1(double* y, int y_size, double* result);
		int WFG6_t2(double* y, int y_size, int k, const int M, double* result);
		int WFG7_t1(double* y, int y_size, int k, double* result);
		int WFG8_t1(double* y, int y_size, int k, double* result);
		int WFG9_t1(double* y, int y_size, double* result);
		int WFG9_t2(double* y, int y_size, int k, double* result);
		void WFG1_shape(double* y, int size, double* result);
		void WFG2_shape(double* y, int size, double* result);
		void WFG3_shape(double* y, int size, double* result);
		void WFG4_shape(double* y, int size, double* result);

	public:
		int Degenerate;
		double* wfg_temp;
		double* temp;
		double* wfg_w;
		int wfg_K; // position parameter
		int decision_num;
		int objective_num;
	};

	class WFG1 :public WFG
	{
	public:
		WFG1(int dec_num, int obj_num);
		virtual ~WFG1();

		void CalObj(Individual *ind);
	};

	class WFG2 :public WFG
	{
	public:
		WFG2(int dec_num, int obj_num);
		virtual ~WFG2();

		void CalObj(Individual *ind);
	};

	class WFG3 :public WFG
	{
	public:
		WFG3(int dec_num, int obj_num);
		virtual ~WFG3();

		void CalObj(Individual *ind);
	};

	class WFG4 :public WFG
	{
	public:
		WFG4(int dec_num, int obj_num);
		virtual ~WFG4();

		void CalObj(Individual *ind);
	};

	class WFG5 :public WFG
	{
	public:
		WFG5(int dec_num, int obj_num);
		virtual ~WFG5();

		void CalObj(Individual *ind);
	};

	class WFG6 :public WFG
	{
	public:
		WFG6(int dec_num, int obj_num);
		virtual ~WFG6();

		void CalObj(Individual *ind);
	};

	class WFG7 :public WFG
	{
	public:
		WFG7(int dec_num, int obj_num);
		virtual ~WFG7();

		void CalObj(Individual *ind);
	};

	class WFG8 :public WFG
	{
	public:
		WFG8(int dec_num, int obj_num);
		virtual ~WFG8();

		void CalObj(Individual *ind);
	};

	class WFG9 :public WFG
	{
	public:
		WFG9(int dec_num, int obj_num);
		virtual ~WFG9();

		void CalObj(Individual *ind);
	};
}