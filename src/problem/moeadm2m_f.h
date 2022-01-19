#pragma once
#include "core/individual.h"
#include "problem/problem.h"

namespace emoc {

	class MOEADM2M_F1 :public Problem
	{
	public:
		MOEADM2M_F1(int dec_num, int obj_num);
		virtual ~MOEADM2M_F1();

		void CalObj(Individual* ind);
	};

	class MOEADM2M_F2 :public Problem
	{
	public:
		MOEADM2M_F2(int dec_num, int obj_num);
		virtual ~MOEADM2M_F2();

		void CalObj(Individual* ind);
	};

	class MOEADM2M_F3 :public Problem
	{
	public:
		MOEADM2M_F3(int dec_num, int obj_num);
		virtual ~MOEADM2M_F3();

		void CalObj(Individual* ind);
	};

	class MOEADM2M_F4 :public Problem
	{
	public:
		MOEADM2M_F4(int dec_num, int obj_num);
		virtual ~MOEADM2M_F4();

		void CalObj(Individual* ind);
	};

	class MOEADM2M_F5 :public Problem
	{
	public:
		MOEADM2M_F5(int dec_num, int obj_num);
		virtual ~MOEADM2M_F5();

		void CalObj(Individual* ind);
	};

	class MOEADM2M_F6 :public Problem
	{
	public:
		MOEADM2M_F6(int dec_num, int obj_num);
		virtual ~MOEADM2M_F6();

		void CalObj(Individual* ind);
	};

	class MOEADM2M_F7 :public Problem
	{
	public:
		MOEADM2M_F7(int dec_num, int obj_num);
		virtual ~MOEADM2M_F7();

		void CalObj(Individual* ind);
	};

}