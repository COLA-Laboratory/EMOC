#include "problem/wfg/wfg.h"

#include <iostream>
#include <cmath>
#include <cstdlib>

#include "core/macro.h"
#include "core/global.h"

namespace emoc {

	int next_int(char *st, int st_len, int pos)
	{
		int i;
		int re = 0;
		int flag = 0;
		for (i = pos; i < st_len; i++)
		{
			if (st[i] > '0' && st[i] < '9')
			{
				flag = 1;
				re = re * 10;
				re += st[i] - '0';
			}
			else if (flag)
				return re;
			else if (st[i] == 0)
				break;
		}
		//print_error (flag == 0, 1, "can't get next int in WFG ini");

		return re;
	}

	void WFG::WFG_ini()
	{
		wfg_K = 18;
		wfg_w = (double *)malloc(sizeof(double) * (decision_num + objective_num));
		for (int i = 0; i < (decision_num + objective_num); i++)
			wfg_w[i] = 0.0;
		
		temp = (double*)malloc(sizeof(double) * objective_num);
		for (int i = 0; i < (objective_num); i++)
			temp[i] = 0.0;

		wfg_temp = (double*)malloc(sizeof(double) * (decision_num + objective_num));
		for (int i = 0; i < (decision_num + objective_num); i++)
			wfg_temp[i] = 0.0;
	}

	void WFG::WFG_free()
	{
		free(wfg_temp);
		free(temp);
		free(wfg_w);
	}

	void WFG::WFG_normalise(double *z, int z_size, double *result)
	{
		int i;
		double bound;

		for (i = 0; i < z_size; i++)
		{
			bound = 2.0 * (i + 1);
			result[i] = z[i] / bound;
		}
	}

	void WFG::calculate_x(double *x, double *result, int size)
	{
		int i;
		double val = x[size - 1];

		if (!Degenerate && val < 1)
			val = 1;

		result[0] = x[0];
		result[size - 1] = result[size - 1];

		for (i = 1; i < size - 1; i++)
			result[i] = (x[i] - 0.5) * val + 0.5;
	}

	void WFG::calculate_f(double D, double x, double *h, int size, double *result)
	{
		int i;
		int S = 0;

		for (i = 0; i < size; i++)
		{
			S = S + 2;
			result[i] = (D * x + S * h[i]);
		}
	}


	/*
	 * Shape Function:
	 * The following functions define the shape of the PF.
	 * */
	double WFG::linear(double *x, int M, int m)
	{
		int i;
		double result = 1.0;

		for (i = 1; i <= M - m; i++)
			result *= x[i - 1];

		if (m != 1)
			result *= 1 - x[M - m];

		if (result > 1) result = 1;
		if (result < 0) result = 0;

		return result;
	}

	double WFG::convex(double *x, int x_size, int m)
	{
		int i;
		double result = 1.0;

		for (i = 1; i <= x_size - m; i++)
			result *= 1.0 - cos(x[i - 1] * PI / 2.0);

		if (m != 1)
			result *= 1.0 - sin(x[x_size - m] * PI / 2.0);

		if (result > 1) result = 1;
		if (result < 0) result = 0;

		return result;
	}

	double WFG::concave(double *x, int x_size, int m)
	{
		int i;
		double result = 1.0;

		for (i = 1; i <= x_size - m; i++)
			result *= sin(x[i - 1] * PI / 2.0);

		if (m != 1)
			result *= cos(x[x_size - m] * PI / 2.0);

		if (result > 1) result = 1;
		if (result < 0) result = 0;

		return result;
	}

	double WFG::mixed(double *x, int A, double alpha)
	{
		double tmp = 2.0 * A * PI;
		double result = pow(1.0 - x[0] - cos(tmp * x[0] + PI / 2.0) / tmp, alpha);

		if (result > 1) result = 1;
		if (result < 0) result = 0;

		return result;
	}

	double WFG::disc(double *x, int A, double alpha, double beta)
	{
		double tmp1 = A * pow(x[0], beta) * PI;
		double result = 1.0 - pow(x[0], alpha) * pow(cos(tmp1), 2.0);
		if (result > 1) result = 1;
		if (result < 0) result = 0;

		return result;
	}

	/*
	 * Transform Fuction:
	 * The following functions defines the functions that control the landscape of the search space.
	 * */
	double WFG::b_poly(double y, double alpha)
	{
		double result = pow(y, alpha);

		if (result > 1) result = 1;
		if (result < 0) result = 0;

		return result;
	}

	double WFG::min(double a, double b)
	{
		if (a > b) return b;

		return a;
	}

	double WFG::b_flat(double y, double A, double B, double C)
	{
		double tmp1 = min(0.0, floor(y - B)) * A * (B - y) / B;
		double tmp2 = min(0.0, floor(C - y)) * (1.0 - A) * (y - C) / (1.0 - C);
		double result = A + tmp1 - tmp2;

		if (result > 1) result = 1;
		if (result < 0) result = 0;

		return result;
	}

	double WFG::b_param(double y, double u, double A, double B, double C)
	{
		double v = A - (1.0 - 2.0 * u) * fabs(floor(0.5 - u) + A);
		double result = pow(y, B + (C - B) * v);

		if (result > 1) result = 1;
		if (result < 0) result = 0;

		return result;
	}

	double WFG::s_linear(double y, double A)
	{
		double result = fabs(y - A) / fabs(floor(A - y) + A);

		if (result > 1) result = 1;
		if (result < 0) result = 0;

		return result;
	}

	double WFG::s_decept(double y, double A, double B, double C)
	{
		double tmp1 = floor(y - A + B) * (1.0 - C + (A - B) / B) / (A - B);
		double tmp2 = floor(A + B - y) * (1.0 - C + (1.0 - A - B) / B) / (1.0 - A - B);
		double result = 1.0 + (fabs(y - A) - B) * (tmp1 + tmp2 + 1.0 / B);

		if (result > 1) result = 1;
		if (result < 0) result = 0;

		return result;
	}

	double WFG::s_multi(double y, int A, double B, double C)
	{
		double tmp1 = fabs(y - C) / (2.0 * (floor(C - y) + C));
		double tmp2 = (4.0 * A + 2.0) * PI * (0.5 - tmp1);
		double result = (1.0 + cos(tmp2) + 4.0 * B * pow(tmp1, 2.0)) / (B + 2.0);

		if (result > 1) result = 1;
		if (result < 0) result = 0;

		return  result;
	}

	double WFG::r_sum(double *y, int y_size, double *w, int w_size)
	{
		int i;
		double result;
		double numerator = 0.0;
		double denominator = 0.0;

		for (i = 0; i < y_size; i++)
		{
			numerator += w[i] * y[i];
			denominator += w[i];
		}

		result = numerator / denominator;
		if (result > 1) result = 1;
		if (result < 0) result = 0;

		return result;
	}

	double WFG::r_nonsep(double *y, int y_size, const int A)
	{
		int i, j;
		double result;
		double numerator = 0.0;
		for (i = 0; i < y_size; i++)
		{
			numerator += y[i];

			for (j = 0; j <= A - 2; j++)
				numerator += fabs(y[i] - y[(i + j + 1) % y_size]);
		}

		const double tmp = ceil(A / 2.0);
		const double denominator = y_size * tmp * (1.0 + 2.0 * A - 2.0 * tmp) / A;

		result = numerator / denominator;
		if (result > 1) result = 1;
		if (result < 0) result = 0;

		return result;
	}

	/*
	 * Transform Functions Set:
	 * This defines the tranform function used by different WFG instances.
	 * */
	int WFG::WFG1_t1(double *y, int y_size, int k, double *result)
	{
		int i;
		for (i = 0; i < k; i++)
			result[i] = y[i];

		for (i = k; i < y_size; i++)
			result[i] = s_linear(y[i], 0.35);

		return y_size;
	}

	int WFG::WFG1_t2(double *y, int y_size, int k, double *result)
	{
		int i;

		for (i = 0; i < k; i++)
			result[i] = y[i];

		for (i = k; i < y_size; i++)
			result[i] = b_flat(y[i], 0.8, 0.75, 0.85);

		return y_size;
	}

	int WFG::WFG1_t3(double* y, int y_size, double *result)
	{
		int i;

		for (i = 0; i < y_size; i++)
			result[i] = b_poly(y[i], 0.02);

		return y_size;
	}

	int WFG::WFG1_t4(double *y, int y_size, int k, int M, double *result)
	{
		int i;
		int head, tail;
		for (i = 1; i <= y_size; i++)
			wfg_w[i - 1] = 2.0 * i;

		for (i = 1; i <= M - 1; i++)
		{
			head = (i - 1) * k / (M - 1);
			tail = i * k / (M - 1);
			temp[i - 1] = r_sum(y + head, tail - head, wfg_w + head, tail - head);
		}

		temp[M - 1] = r_sum(y + k, y_size - k, wfg_w + k, y_size - k);

		for (i = 0; i < M; i++)
			result[i] = temp[i];

		return M;
	}

	int WFG::WFG2_t2(double *y, int y_size, int k, double *result)
	{
		int i;
		const int l = y_size - k;

		for (i = 0; i < k; i++)
			result[i] = y[i];

		for (i = k + 1; i <= k + l / 2; i++)
		{
			const int head = k + 2 * (i - k) - 2;
			const int tail = k + 2 * (i - k);

			result[i] = r_nonsep(y + head, tail - head, 2);
		}

		return k + l / 2 + 1;
	}

	int WFG::WFG2_t3(double *y, int y_size, int k, int M, double *result)
	{
		int i;

		for (i = 1; i <= y_size; i++)
			wfg_w[i - 1] = 1.0;

		for (i = 1; i <= M - 1; i++)
		{
			const int head = (i - 1) * k / (M - 1);
			const int tail = i * k / (M - 1);
			result[i - 1] = r_sum(y + head, tail - head, wfg_w + head, tail - head);
		}

		result[M - 1] = r_sum(y + k, y_size - k, wfg_w + k, y_size - k);

		return M;
	}

	int WFG::WFG4_t1(double *y, int y_size, double * result)
	{
		int i;

		for (i = 0; i < y_size; i++)
			result[i] = s_multi(y[i], 30, 10, 0.35);

		return y_size;
	}


	int WFG::WFG5_t1(double *y, int y_size, double *result)
	{
		int i;

		for (i = 0; i < y_size; i++)
			result[i] = s_decept(y[i], 0.35, 0.001, 0.05);

		return y_size;
	}

	int WFG::WFG6_t2(double *y, int y_size, int k, const int M, double *result)
	{
		int i;

		for (i = 1; i <= M - 1; i++)
		{
			const int head = (i - 1) * k / (M - 1);
			const int tail = i * k / (M - 1);

			result[i - 1] = (r_nonsep(y + head, tail - head, k / (M - 1)));
		}

		result[M - 1] = (r_nonsep(y + k, y_size - k, y_size - k));

		return M;
	}

	int WFG::WFG7_t1(double *y, int y_size, int k, double *result)
	{
		int i;
		double u;

		for (i = 0; i < y_size; i++)
			wfg_w[i] = 1.0;

		for (i = 0; i < k; i++)
		{
			u = r_sum(y + i + 1, y_size - (i + 1), wfg_w + i + 1, y_size - (i + 1));
			result[i] = (b_param(y[i], u, 0.98 / 49.98, 0.02, 50));
		}

		for (i = k; i < y_size; i++)
			result[i] = (y[i]);

		return y_size;
	}

	int WFG::WFG8_t1(double *y, int y_size, int k, double *result)
	{
		int i;
		double u;

		for (i = 0; i < y_size; i++)
			wfg_w[i] = 1.0;

		for (i = 0; i < k; i++)
			result[i] = (y[i]);

		for (i = k; i < y_size; i++)
		{
			u = r_sum(y, i, wfg_w, i);
			result[i] = b_param(y[i], u, 0.98 / 49.98, 0.02, 50);
		}

		return y_size;
	}

	int WFG::WFG9_t1(double *y, int y_size, double *result)
	{
		int i;
		double u;

		for (i = 0; i < y_size; i++)
			wfg_w[i] = 1.0;

		for (i = 0; i < y_size - 1; i++)
		{
			u = r_sum(y + i + 1, y_size - (i + 1), wfg_w, y_size - (i + 1));
			result[i] = b_param(y[i], u, 0.98 / 49.98, 0.02, 50);
		}
		result[y_size - 1] = y[y_size - 1];

		return y_size;
	}

	int WFG::WFG9_t2(double *y, int y_size, int k, double *result)
	{
		int i;

		for (i = 0; i < k; i++)
			result[i] = s_decept(y[i], 0.35, 0.001, 0.05);

		for (i = k; i < y_size; i++)
			result[i] = s_multi(y[i], 30, 95, 0.35);

		return y_size;
	}

	void WFG::WFG1_shape(double *y, int size, double *result)
	{
		int i;

		calculate_x(y, temp, size);

		for (i = 1; i <= size - 1; i++)
			result[i - 1] = convex(temp, size, i);

		result[size - 1] = mixed(temp, 5, 1.0);

		calculate_f(1.0, temp[size - 1], result, size, result);
	}

	void WFG::WFG2_shape(double *y, int size, double *result)
	{
		int i;

		calculate_x(y, temp, size);

		for (i = 1; i <= size - 1; i++)
			result[i - 1] = convex(temp, size, i);

		result[size - 1] = disc(temp, 5, 1.0, 1.0);

		calculate_f(1.0, temp[size - 1], result, size, result);
	}

	void WFG::WFG3_shape(double *y, int y_size, double *result)
	{
		int i;

		calculate_x(y, temp, y_size);

		for (i = 1; i <= y_size; i++)
			result[i - 1] = linear(temp, y_size, i);

		calculate_f(1.0, temp[y_size - 1], result, y_size, result);
	}

	void WFG::WFG4_shape(double *y, int y_size, double *result)
	{
		int i;

		calculate_x(y, temp, y_size);
		for (i = 1; i <= y_size; i++)
			result[i - 1] = concave(temp, y_size, i);

		calculate_f(1.0, temp[y_size - 1], result, y_size, result);
	}

	

	WFG1::WFG1(int dec_num, int obj_num) : WFG(dec_num, obj_num)
	{
		decision_num = dec_num;
		objective_num = obj_num;
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	WFG1::~WFG1()
	{

	}

	void WFG1::CalObj(Individual *ind)
	{
		int size = 0;
		double *xreal = ind->dec_.data();
		double *obj = ind->obj_.data();

		Degenerate = 0;

		WFG_ini();
		size = dec_num_;
		size = WFG1_t1(xreal, size, wfg_K, wfg_temp);
		size = WFG1_t2(wfg_temp, size, wfg_K, wfg_temp);
		size = WFG1_t3(wfg_temp, size, wfg_temp);
		size = WFG1_t4(wfg_temp, size, wfg_K, obj_num_, wfg_temp);
		WFG1_shape(wfg_temp, size, obj);
		WFG_free();
	}

	WFG2::WFG2(int dec_num, int obj_num) : WFG(dec_num, obj_num)
	{
		decision_num = dec_num;
		objective_num = obj_num;
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	WFG2::~WFG2()
	{

	}

	void WFG2::CalObj(Individual *ind)
	{
		int size = 0;
		double *xreal = ind->dec_.data();
		double *obj = ind->obj_.data();

		Degenerate = 0;

		WFG_ini();

		size = dec_num_;
		size = WFG1_t1(xreal, size, wfg_K, wfg_temp);
		size = WFG2_t2(wfg_temp, size, wfg_K, wfg_temp);
		size = WFG2_t3(wfg_temp, size, wfg_K, obj_num_, wfg_temp);
		WFG2_shape(wfg_temp, size, obj);

		//std::cout << obj[0] << " " << obj[1] << " \n";
		WFG_free();
	}

	WFG3::WFG3(int dec_num, int obj_num) : WFG(dec_num, obj_num)
	{
		decision_num = dec_num;
		objective_num = obj_num;
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	WFG3::~WFG3()
	{

	}

	void WFG3::CalObj(Individual *ind)
	{
		int size = 0;
		double *xreal = ind->dec_.data();
		double *obj = ind->obj_.data();

		Degenerate = 1;

		WFG_ini();

		size = dec_num_;
		size = WFG1_t1(xreal, size, wfg_K, wfg_temp);
		size = WFG2_t2(wfg_temp, size, wfg_K, wfg_temp);
		size = WFG2_t3(wfg_temp, size, wfg_K, obj_num_, wfg_temp);
		WFG3_shape(wfg_temp, size, obj);

		WFG_free();
	}

	WFG4::WFG4(int dec_num, int obj_num) : WFG(dec_num, obj_num)
	{
		decision_num = dec_num;
		objective_num = obj_num;
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	WFG4::~WFG4()
	{

	}

	void WFG4::CalObj(Individual *ind)
	{
		int size = 0;
		double *xreal = ind->dec_.data();
		double *obj = ind->obj_.data();

		Degenerate = 0;

		WFG_ini();

		size = dec_num_;
		size = WFG4_t1(xreal, size, wfg_temp);
		size = WFG2_t3(wfg_temp, size, wfg_K, obj_num_, wfg_temp);
		WFG4_shape(wfg_temp, size, obj);

		WFG_free();
	}

	WFG5::WFG5(int dec_num, int obj_num) : WFG(dec_num, obj_num)
	{
		decision_num = dec_num;
		objective_num = obj_num;
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	WFG5::~WFG5()
	{

	}

	void WFG5::CalObj(Individual *ind)
	{
		int size = 0;
		double *xreal = ind->dec_.data();
		double *obj = ind->obj_.data();

		Degenerate = 0;

		WFG_ini();

		size = dec_num_;
		size = WFG5_t1(xreal, size, wfg_temp);
		size = WFG2_t3(wfg_temp, size, wfg_K, obj_num_, wfg_temp);
		WFG4_shape(wfg_temp, size, obj);

		WFG_free();
	}

	WFG6::WFG6(int dec_num, int obj_num) : WFG(dec_num, obj_num)
	{
		decision_num = dec_num;
		objective_num = obj_num;
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	WFG6::~WFG6()
	{

	}

	void WFG6::CalObj(Individual *ind)
	{
		int size = 0;
		double *xreal = ind->dec_.data();
		double *obj = ind->obj_.data();

		Degenerate = 0;

		WFG_ini();

		size = dec_num_;
		size = WFG1_t1(xreal, size, wfg_K, wfg_temp);
		size = WFG6_t2(wfg_temp, size, wfg_K, obj_num_, wfg_temp);
		WFG4_shape(wfg_temp, size, obj);

		WFG_free();
	}

	WFG7::WFG7(int dec_num, int obj_num) : WFG(dec_num, obj_num)
	{
		decision_num = dec_num;
		objective_num = obj_num;
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	WFG7::~WFG7()
	{

	}

	void WFG7::CalObj(Individual *ind)
	{
		int size = 0;
		double *xreal = ind->dec_.data();
		double *obj = ind->obj_.data();

		Degenerate = 0;

		WFG_ini();

		size = dec_num_;
		size = WFG7_t1(xreal, size, wfg_K, wfg_temp);
		size = WFG1_t1(xreal, size, wfg_K, wfg_temp);
		size = WFG2_t3(wfg_temp, size, wfg_K, obj_num_, wfg_temp);
		WFG4_shape(wfg_temp, size, obj);

		WFG_free();
	}

	WFG8::WFG8(int dec_num, int obj_num) : WFG(dec_num, obj_num)
	{
		decision_num = dec_num;
		objective_num = obj_num;
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	WFG8::~WFG8()
	{

	}

	void WFG8::CalObj(Individual *ind)
	{
		int size = 0;
		double *xreal = ind->dec_.data();
		double *obj = ind->obj_.data();

		Degenerate = 0;

		WFG_ini();

		size = dec_num_;
		size = WFG8_t1(xreal, size, wfg_K, wfg_temp);
		size = WFG1_t1(xreal, size, wfg_K, wfg_temp);
		size = WFG2_t3(wfg_temp, size, wfg_K, obj_num_, wfg_temp);
		WFG4_shape(wfg_temp, size, obj);

		WFG_free();
	}

	WFG9::WFG9(int dec_num, int obj_num) : WFG(dec_num, obj_num)
	{
		decision_num = dec_num;
		objective_num = obj_num;
		for (int i = 0; i < dec_num; ++i)
		{
			lower_bound_[i] = 0.0;
			upper_bound_[i] = 1.0;
		}
	}

	WFG9::~WFG9()
	{

	}

	void WFG9::CalObj(Individual *ind)
	{
		int size = 0;
		double *xreal = ind->dec_.data();
		double *obj = ind->obj_.data();

		Degenerate = 0;

		WFG_ini();

		size = dec_num_;
		size = WFG9_t1(xreal, size, wfg_temp);
		size = WFG9_t2(xreal, size, wfg_K, wfg_temp);
		size = WFG6_t2(wfg_temp, size, wfg_K, obj_num_, wfg_temp);
		WFG4_shape(wfg_temp, size, obj);

		WFG_free();
	}
}