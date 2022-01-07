#pragma once
#include "core/individual.h"
#include "problem/problem.h"

namespace emoc {

	class MOEADDE_F1 :public Problem
	{
	public:
		MOEADDE_F1(int dec_num, int obj_num);
		virtual ~MOEADDE_F1();

		void CalObj(Individual* ind);
	};

	class MOEADDE_F2 :public Problem
	{
	public:
		MOEADDE_F2(int dec_num, int obj_num);
		virtual ~MOEADDE_F2();

		void CalObj(Individual* ind);
	};

	class MOEADDE_F3 :public Problem
	{
	public:
		MOEADDE_F3(int dec_num, int obj_num);
		virtual ~MOEADDE_F3();

		void CalObj(Individual* ind);
	};

	class MOEADDE_F4 :public Problem
	{
	public:
		MOEADDE_F4(int dec_num, int obj_num);
		virtual ~MOEADDE_F4();

		void CalObj(Individual* ind);
	};

	class MOEADDE_F5 :public Problem
	{
	public:
		MOEADDE_F5(int dec_num, int obj_num);
		virtual ~MOEADDE_F5();

		void CalObj(Individual* ind);
	};

	class MOEADDE_F6 :public Problem
	{
	public:
		MOEADDE_F6(int dec_num, int obj_num);
		virtual ~MOEADDE_F6();

		void CalObj(Individual* ind);
	};

	class MOEADDE_F7 :public Problem
	{
	public:
		MOEADDE_F7(int dec_num, int obj_num);
		virtual ~MOEADDE_F7();

		void CalObj(Individual* ind);
	};

	class MOEADDE_F8 :public Problem
	{
	public:
		MOEADDE_F8(int dec_num, int obj_num);
		virtual ~MOEADDE_F8();

		void CalObj(Individual* ind);
	};

	class MOEADDE_F9 :public Problem
	{
	public:
		MOEADDE_F9(int dec_num, int obj_num);
		virtual ~MOEADDE_F9();

		void CalObj(Individual* ind);
	};

}