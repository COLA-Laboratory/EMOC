#include "core/utility.h"

#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "emoc_app.h"
#include "ui/plot_manager.h"
#include "core/macro.h"
#include "metric/metric_head_collect.h"
#include "random/random.h"
#include "alglib/src/statistics.h"

namespace emoc {

	double* SetWeight(const std::string& weightstring)
	{
		int size = count(weightstring.begin(),weightstring.end(),',')+1;
        if(!size)
        {
            std::cout<<"\nthe dimensionality of weight is 0, hence exits."<<std::endl;
            exit(1);
        }
        double *m_w = nullptr;
        m_w = new double[size];
        int i = 0;
        //std::string::size_type -- unsigned long long
        std::string::size_type start = 0, end;
        while((end = weightstring.find(',',start)) != std::string::npos)
        {
            m_w[i++] = atof(weightstring.substr(start,end-start).c_str());
            start = end + 1;
        }
        m_w[i] = atof(weightstring.substr(start).c_str());
        return m_w;
	}

	double* SetWeight(int obj_num_, std::string problem_name)
	{
		double *m_w = new double[obj_num_];
		if(problem_name == "ZDT1" || problem_name == "ZDT4")
		{
			m_w[0] = 0.3;
			m_w[1] = 0.4;
		}
		else if(problem_name == "ZDT2")
		{
			m_w[0] = 0.2;
			m_w[1] = 0.8;
		}
		else if(problem_name == "ZDT3")
		{
			m_w[0] = 0.15;
			m_w[1] = 0.4;
		}
		else if(problem_name == "ZDT6")
		{
			m_w[0] = 0.9;
			m_w[1] = 0.3;
		}
		else if(problem_name == "DTLZ1")
		{
			switch (obj_num_)
			{
			case 3:
				m_w[0] = 0.3;
				m_w[1] = 0.3;
				m_w[2] = 0.2;
				break;
			case 5:
				m_w[0] = 0.2;
				m_w[1] = 0.1;
				m_w[2] = 0.1;
				m_w[3] = 0.3;
				m_w[4] = 0.4;
				break;
			case 8:
				m_w[0] = 0.1;
				m_w[1] = 0.2;
				m_w[2] = 0.1;
				m_w[3] = 0.4;
				m_w[4] = 0.4;
				m_w[5] = 0.1;
				m_w[6] = 0.3;
				m_w[7] = 0.1;
				break;
			case 10:
				m_w[0] = 0.05;
				m_w[1] = 0.1;
				m_w[2] = 0.1;
				m_w[3] = 0.05;
				m_w[4] = 0.1;
				m_w[5] = 0.2;
				m_w[6] = 0.08;
				m_w[7] = 0.03;
				m_w[8] = 0.3;
				m_w[9] = 0.1;
				break;
			
			default:
				break;
			}
		}
		else if(problem_name == "DTLZ2" || problem_name == "DTLZ3" || problem_name == "DTLZ4")
		{
			switch (obj_num_)
			{
			case 3:
				m_w[0] = 0.7;
				m_w[1] = 0.8;
				m_w[2] = 0.5;
				break;
			case 5:
				m_w[0] = 0.7;
				m_w[1] = 0.6;
				m_w[2] = 0.3;
				m_w[3] = 0.8;
				m_w[4] = 0.5;
				break;
			case 8:
				m_w[0] = 0.6;
				m_w[1] = 0.5;
				m_w[2] = 0.75;
				m_w[3] = 0.2;
				m_w[4] = 0.3;
				m_w[5] = 0.55;
				m_w[6] = 0.7;
				m_w[7] = 0.6;
				break;
			case 10:
				m_w[0] = 0.3;
				m_w[1] = 0.3;
				m_w[2] = 0.3;
				m_w[3] = 0.1;
				m_w[4] = 0.3;
				m_w[5] = 0.55;
				m_w[6] = 0.35;
				m_w[7] = 0.35;
				m_w[8] = 0.25;
				m_w[9] = 0.45;
				break;
			default:
				break;
			}
			
		}
		else if(problem_name == "DTLZ5" || problem_name == "DTLZ6")
		{
			switch (obj_num_)
			{
			case 3:
				m_w[0] = 0.2;
				m_w[1] = 0.3;
				m_w[2] = 0.6;
				break;
			case 5:
				m_w[0] = 0.12;
				m_w[1] = 0.12;
				m_w[2] = 0.17;
				m_w[3] = 0.24;
				m_w[4] = 0.7;
				break;
			case 8:
				m_w[0] = 0.04;
				m_w[1] = 0.04;
				m_w[2] = 0.0566;
				m_w[3] = 0.8;
				m_w[4] = 0.113;
				m_w[5] = 0.16;
				m_w[6] = 0.2263;
				m_w[7] = 0.68;
				break;
			case 10:
				m_w[0] = 0;
				m_w[1] = 0;
				m_w[2] = 0;
				m_w[3] = 0;
				m_w[4] = 0.0096;
				m_w[5] = 0.027;
				m_w[6] = 0.082;
				m_w[7] = 0.25;
				m_w[8] = 0.75;
				m_w[9] = 0.08;
				break;
			default:
				break;
			}
		}
		return m_w;
	}

	int CheckDominance(Individual* ind1, Individual* ind2, int obj_num)
	{
		int flag1 = 0, flag2 = 0;

		for (int i = 0; i < obj_num; ++i)
		{
			if (ind1->obj_[i] < ind2->obj_[i])
				flag1 = 1;
			else
			{
				if (ind1->obj_[i] > ind2->obj_[i])
					flag2 = 1;
			}
		}

		if (flag1 == 1 && flag2 == 0)
			return 1;
		else
		{
			if (flag1 == 0 && flag2 == 1)
				return -1;
			else
				return 0;
		}
	}

	int CheckDominanceWithConstraint(Individual* ind1, Individual* ind2, int obj_num)
	{
		bool is_infeasible1 = false, is_infeasible2 = false;
		double cons1 = 0.0, cons2 = 0.0;
		for (int i = 0; i < ind1->con_.size(); i++)
		{
			cons1 += std::max(0.0, ind1->con_[i]);
			if (ind1->con_[i] > 0)
				is_infeasible1 = true;
		}
		for (int i = 0; i < ind2->con_.size(); i++)
		{
			cons2 += std::max(0.0, ind2->con_[i]);
			if (ind2->con_[i] > 0)
				is_infeasible2 = true;
		}

		if (!is_infeasible1 && !is_infeasible2)
			return CheckDominance(ind1, ind2, obj_num);
		else if (is_infeasible1 && !is_infeasible2)
			return -1;
		else if (!is_infeasible1 && is_infeasible2)
			return 1;
		else if (is_infeasible1 && is_infeasible2)
		{
			if (std::fabs(cons1 - cons2) < EMOC_EPS)
				return 0;
			else if (cons1 < cons2)
				return 1;
			else if (cons1 > cons2)
				return -1;
		}
	}

	int WeaklyDominates(double* point1, double* point2, int obj_num)
	{
		int i = 0, better = 1;
		while (i < obj_num && better)
		{
			better = point1[i] <= point2[i];
			i++;
		}
		return better;
	}

	double CalEuclidianDistance(const double* a, const double* b, int dimension)
	{
		double distance = 0.0;
		for (int i = 0; i < dimension; i++)
			distance += (a[i] - b[i]) * (a[i] - b[i]);
		return sqrt(distance);
	}

	double CalPerpendicularDistance(double* a, double* weight, int dimension)
	{
		double sin = CalculateSin(a, weight, dimension);
		double d2 = CalculateNorm(a, dimension);
		d2 = d2 * sin;

		return d2;
	}

	double CalPerpendicularDistanceNormalization(double* a, double* weight, int dimension, double* ideal, double* nadir)
	{
		double* new_a = new double[dimension];
		for (int i = 0; i < dimension; i++)
			new_a[i] = (a[i] - ideal[i]) / (nadir[i] - ideal[i]);

		double sin = CalculateSin(a, weight, dimension);
		double d2 = CalculateNorm(a, dimension);
		d2 = d2 * sin;
		delete[] new_a;
		return d2;
	}

	int Combination(int n, int k)
	{
		if (n < k)
			return -1;

		double ans = 1;
		for (int i = k + 1; i <= n; i++)
		{
			ans = ans * i;
			ans = ans / (double)(i - k);
		}

		return (int)ans;
	}

	double CalculateDotProduct(double* vector1, double* vector2, int dimension)
	{
		double dot_product = 0;
		for (int i = 0; i < dimension; i++)
			dot_product += vector1[i] * vector2[i];

		return dot_product;
	}

	double CalculateCos(double* a, double* b, int dimension)
	{
		return CalculateDotProduct(a, b, dimension) / (CalculateNorm(a, dimension) * CalculateNorm(b, dimension));
	}

	double CalculateSin(double* a, double* b, int dimension)
	{
		double cos = CalculateCos(a, b, dimension);
		return sqrt(1 - pow(cos, 2.0));
	}

	double CalculateNorm(double* vector, int dimension)
	{
		double norm = 0;
		for (int i = 0; i < dimension; i++)
		{
			norm += (vector[i] * vector[i]);
		}

		return sqrt(norm);
	}

	void UpdateIdealpoint(Individual* ind, double* ideal_point, int obj_num)
	{
		for (int i = 0; i < obj_num; i++)
		{
			if (ind->obj_[i] < ideal_point[i])
				ideal_point[i] = ind->obj_[i];
		}
	}

	void UpdateIdealpoint(Individual** pop, int pop_num, double* ideal_point, int obj_num)
	{
		for (int i = 0; i < obj_num; ++i)
			ideal_point[i] = EMOC_INF;

		for (int i = 0; i < pop_num; i++)
		{
			for (int j = 0; j < obj_num; j++)
			{
				if (pop[i]->obj_[j] < ideal_point[j])
					ideal_point[j] = pop[i]->obj_[j];
			}
		}
	}

	void UpdateNadirpoint(Individual* ind, double* nadir_point, int obj_num)
	{
		for (int i = 0; i < obj_num; i++)
		{
			if (ind->obj_[i] > nadir_point[i])
				nadir_point[i] = ind->obj_[i];
		}
	}

	void UpdateNadirpoint(Individual** pop, int pop_num, double* nadir_point, int obj_num)
	{
		for (int i = 0; i < obj_num; ++i)
			nadir_point[i] = -EMOC_INF;

		for (int i = 0; i < pop_num; i++)
		{
			for (int j = 0; j < obj_num; j++)
			{
				if (pop[i]->obj_[j] > nadir_point[j])
					nadir_point[j] = pop[i]->obj_[j];
			}
		}
	}

	void UpdateNadirpointWithRank(Individual** pop, int pop_num, double* nadir_point, int obj_num, int rank)
	{
		for (int i = 0; i < obj_num; ++i)
			nadir_point[i] = -EMOC_INF;

		for (int i = 0; i < pop_num; i++)
		{
			if (pop[i]->rank_ == rank)
			{
				for (int j = 0; j < obj_num; j++)
				{
					if (pop[i]->obj_[j] > nadir_point[j])
						nadir_point[j] = pop[i]->obj_[j];
				}
			}
		}
	}

	double CalWeightedLpScalarizing(Individual* ind, double* weight_vector, double* ideal_point, double* nadir_point, int obj_num, int p /*= -1*/)
	{
		double fitness = 0, max = -1.0e+20, sum = 0.0;

		if (p == -1)
		{
			for (int i = 0; i < obj_num; ++i)
			{
				double diff = fabs(ind->obj_[i] - ideal_point[i]) / (nadir_point[i] - ideal_point[i]);
				if (weight_vector[i] < EMOC_EPS)
					fitness = diff / 0.000001;
				else
					fitness = diff / weight_vector[i];

				if (fitness > max)
					max = fitness;
			}

			fitness = max;
			ind->fitness_ = fitness;
		}
		else if (p > 0)
		{
			for (int i = 0; i < obj_num; ++i)
			{
				double diff = fabs(ind->obj_[i] - ideal_point[i]) / (nadir_point[i] - ideal_point[i]);
				if (weight_vector[i] < EMOC_EPS)
					sum += pow(diff / 0.000001, (double)p);
				else
					sum += pow(diff / weight_vector[i], (double)p);
			}

			fitness = pow(sum, (1 / (double)p));
			ind->fitness_ = fitness;
		}


		return fitness;
	}

	double CalWeightedSum(Individual* ind, double* weight_vector, double* ideal_point, int obj_num)
	{
		double fitness = 0;
		for (int i = 0; i < obj_num; i++)
		{
			fitness += ind->obj_[i] * weight_vector[i];
		}

		ind->fitness_ = fitness;

		return fitness;
	}

	double CalInverseChebycheff(Individual* ind, double* weight_vector, double* ideal_point, int obj_num)
	{
		double fitness = 0, max = -1.0e+20;

		for (int i = 0; i < obj_num; ++i)
		{
			double diff = fabs(ind->obj_[i] - ideal_point[i]);
			if (weight_vector[i] < EMOC_EPS)
				fitness = diff / 0.000001;
			else
				fitness = diff / weight_vector[i];

			if (fitness > max)
				max = fitness;
		}

		fitness = max;
		ind->fitness_ = fitness;

		return fitness;
	}

	double CalPBI(Individual* ind, double* weight_vector, double* ideal_point, int obj_num, double theta)
	{
		theta == 0.0 ? 5.0 : theta;
		double d1 = 0.0, d2 = 0.0, nl = 0.0;

		for (int i = 0; i < obj_num; ++i)
		{
			d1 += (ind->obj_[i] - ideal_point[i]) * weight_vector[i];
			nl += weight_vector[i] * weight_vector[i];
		}
		nl = sqrt(nl);
		d1 = fabs(d1) / nl;

		for (int i = 0; i < obj_num; ++i)
			d2 += ((ind->obj_[i] - ideal_point[i]) - d1 * (weight_vector[i] / nl)) * ((ind->obj_[i] - ideal_point[i]) - d1 * (weight_vector[i] / nl));
		d2 = sqrt(d2);

		ind->fitness_ = d1 + theta * d2;
		return  (d1 + theta * d2);
	}

	double** LoadPFData(int& pf_size, int obj_num, std::string problem_name)
	{
		// reset pf_size
		pf_size = 0;

		// get problem name without number
		int count = 0;
		for (int i = problem_name.size() - 1; i >= 0; i--)
		{
			if (problem_name[i] >= '0' && problem_name[i] <= '9')
				count++;
			else
				break;
		}

		std::string temp_problemname = problem_name.substr(0, problem_name.size() - count);
		for (auto& c : temp_problemname)
		{
			if ((c >= '0' && c <= '9') || c == '_') continue;
			c = tolower(c);
		}

		for (auto& c : problem_name)
		{
			if ((c >= '0' && c <= '9') || c == '_') continue;
			c = tolower(c);
		}

		// open pf data file
		double** pf_data = nullptr;
		char pf_filename[255] = { 0 };
		sprintf(pf_filename, "pf_data/%s/%s.%dD.pf", temp_problemname.c_str(), problem_name.c_str(), obj_num);
		std::fstream pf_file(pf_filename);

		if (!pf_file)
		{
			std::cerr << pf_filename << " file doesn't exist!\n";
		}
		else
		{
			std::string line;
			while (getline(pf_file, line))
				pf_size++;

			// init memory for pf data
			pf_data = new double* [pf_size];
			for (int i = 0; i < pf_size; ++i)
				pf_data[i] = new double[obj_num];

			// read the pf data
			pf_file.clear();
			pf_file.seekg(0);
			for (int i = 0; i < pf_size; i++)
			{
				for (int j = 0; j < obj_num; j++)
				{
					pf_file >> pf_data[i][j];
				}
			}
		}
		pf_file.close();
		return pf_data;
	}


	double CauchyRandom(double location, double scale)
	{
		double u, y;
		u = randomperc();
		y = location + scale * tan(PI * (u - 0.5));

		return y;
	}

	static double rgama(double a)
	{
		if (a < 1)
		{
			double temp = randomperc();
			return rgama(1.0 + a) * pow(temp, 1.0 / a);
		}
		else
		{
			double d, c, x, v, u;
			d = a - 1. / 3.;
			c = 1. / sqrt(9. * d);
			for (;;) {
				do {
					x = GaussianRandom(0.0, 1.0);
					v = 1. + c * x;
				} while (v <= 0.);
				v = v * v * v;
				u = randomperc();
				if (u < 1. - 0.0331 * (x * x) * (x * x)) {
					return (d * v);
				}
				if (log(u) < 0.5 * x * x + d * (1. - v + log(v))) {
					return (d * v);
				}
			}
		}
	}

	double BetaRandom(double a, double b)
	{
		double x1, x2;
		if ((a <= 1.0) && (b <= 1.0))
		{
			double U, V, X, Y;
			while (1)
			{
				U = randomperc();
				V = randomperc();
				X = pow(U, 1.0 / a);
				Y = pow(V, 1.0 / b);
				if ((X + Y) <= 1.0)
				{
					if ((X + Y) > 0)
					{
						return X / (X + Y);

					}
					else
					{
						double logX = log(U) / a;
						double logY = log(V) / b;
						double logM = logX > logY ? logX : logY;
						logX -= logM;
						logY -= logM;
						return exp(logX - log(exp(logX) + exp(logY)));
					}
				}
			}
		}
		else
		{
			x1 = rgama(a);
			x2 = rgama(b);
			return x1 / (x1 + x2);
		}
	}

	double GaussianRandom(double mean, double stdev)
	{

		static double nextNextGaussian;
		static int haveNextNextGaussian = 0;
		double r;

		if (haveNextNextGaussian)
		{
			haveNextNextGaussian = 0;
			r = nextNextGaussian;
		}
		else
		{
			double v1, v2, s, m;

			do {
				v1 = rndreal(-1.0, 1.0);
				v2 = rndreal(-1.0, 1.0);
				s = v1 * v1 + v2 * v2;
			} while (s >= 1 || s == 0);

			m = sqrt(-2 * log(s) / s);
			nextNextGaussian = v2 * m;
			haveNextNextGaussian = 1;
			r = v1 * m;
		}

		return stdev * r + mean;
	}

	double* GaussianElimination(double** A, double* b, double* x, int obj_num)
	{
		int i, j, p;
		int N, max;
		double alpha, sum, t;
		double* temp;

		N = obj_num;
		for (p = 0; p < N; p++)
		{
			// find pivot row and swap
			max = p;
			for (i = p + 1; i < N; i++)
				if (fabs(A[i][p]) > fabs(A[max][p]))
					max = i;
			temp = A[p];
			A[p] = A[max];
			A[max] = temp;
			t = b[p];
			b[p] = b[max];
			b[max] = t;

			// singular or nearly singular
			if (fabs(A[p][p]) <= EMOC_EPS)
				return nullptr;

			// pivot within A and b
			for (i = p + 1; i < N; i++)
			{
				alpha = A[i][p] / A[p][p];
				b[i] -= alpha * b[p];
				for (j = p; j < N; j++)
					A[i][j] -= alpha * A[p][j];
			}
		}

		// back substitution
		for (i = N - 1; i >= 0; i--)
		{
			sum = 0.0;
			for (j = i + 1; j < N; j++)
				sum += A[i][j] * x[j];
			x[i] = (b[i] - sum) / A[i][i];
		}

		return x;
	}

	int RankSumTest(const std::vector<double>& array1, const std::vector<double>& array2)
	{
		int res = 0;

		alglib::real_1d_array a1, a2;
		a1.setcontent(array1.size(), array1.data());
		a2.setcontent(array2.size(), array2.data());
		double p1, p2, p3;
		alglib::mannwhitneyutest(a1, array1.size(), a2, array2.size(), p1, p2, p3);

		if (p1 > 0.05)
			res = 0;
		else
			res = 1;

		return res;
	}

	int SignRankTest(const std::vector<double>& array1, const std::vector<double>& array2)
	{
		int res = 0;

		alglib::real_1d_array a;
		a.setlength(array1.size());
		for (int i = 0; i < array1.size(); i++)
			a[i] = array1[i] - array2[i];

		double p1, p2, p3;
		alglib::wilcoxonsignedranktest(a, array1.size(), 0, p1, p2, p3);


		if (p1 > 0.05)
			res = 0;
		else
			res = -1;

		return res;
	}

	void CollectSingleThreadResult(int run_id, int thread_id, EMOCParameters para)
	{
		EMOCSingleThreadResult result;
		Global* g = EMOCManager::Instance()->GetGlobalSetting(thread_id);
		int dec_num = g->dec_num_;
		int obj_num = g->obj_num_;
		int pop_num = g->algorithm_->GetRealPopNum();
		std::string problem_name = para.problem_name;

		if (para.objective_num > 1)				// multi-objective
		{
			int pf_size = 0;
			double **pf_data = LoadPFData(pf_size, obj_num, problem_name);
			HVCalculator hv_calculator(obj_num, pop_num);
			double igd = CalculateIGD(g->parent_population_.data(), pop_num, obj_num, pf_data, pf_size);
			double hv = hv_calculator.Calculate(g->parent_population_.data(), pop_num, obj_num, pf_data, pf_size);
			double gd = CalculateGD(g->parent_population_.data(), pop_num, obj_num, pf_data, pf_size);
			double spacing = CalculateSpacing(g->parent_population_.data(), pop_num, obj_num);
			double igdplus = CalculateIGDPlus(g->parent_population_.data(), pop_num, obj_num, pf_data, pf_size);
			double gdplus = CalculateGDPlus(g->parent_population_.data(), pop_num, obj_num, pf_data, pf_size);

			int count = EMOCManager::Instance()->GetSingleThreadResultSize();
			result.para = para;
			result.description = para.algorithm_name + " on " + para.problem_name + " Run" + std::to_string(count);
			result.last_igd = igd;
			result.last_hv = hv;
			result.last_gd = gd;
			result.last_spacing = spacing;
			result.last_igdplus = igdplus;
			result.last_gdplus = gdplus;
			result.best_value = -1;
			result.runtime = g->algorithm_->GetRuntime();
			result.pop_num = g->algorithm_->GetRealPopNum();
			result.max_iteration = g->iteration_num_;
			result.igd_history[result.max_iteration] = igd;
			result.hv_history[result.max_iteration] = hv;
			EMOCManager::Instance()->AddSingleThreadResult(result);
			if (EMOCManager::Instance()->GetIsGUI()) UIPanelManager::Instance()->AddAvailSingleThreadResult(result.description);
			printf("run %d time: %fs   igd: %f \n", run_id, result.runtime, igd);

			// free pf data memory
			for (int i = 0; i < pf_size; ++i)
				delete[] pf_data[i];
			delete[] pf_data;
		}
		else if (para.objective_num == 1)		// single-objective
		{
			int count = EMOCManager::Instance()->GetSingleThreadResultSize();
			result.para = para;
			result.description = para.algorithm_name + " on " + para.problem_name + " Run" + std::to_string(count);
			result.last_igd = -1;
			result.last_hv = -1;
			result.last_gd = -1;
			result.last_spacing = -1;
			result.last_igdplus = -1;
			result.last_gdplus = -1;
			result.runtime = g->algorithm_->GetRuntime();
			result.pop_num = g->algorithm_->GetRealPopNum();
			result.max_iteration = g->iteration_num_;
			result.igd_history[result.max_iteration] = -1;
			result.hv_history[result.max_iteration] = -1;
			result.best_value = g->parent_population_[0]->obj_[0];
			EMOCManager::Instance()->AddSingleThreadResult(result);
			if (EMOCManager::Instance()->GetIsGUI()) UIPanelManager::Instance()->AddAvailSingleThreadResult(result.description);
			printf("run %d time: %fs   best value: %f \n", run_id, result.runtime, result.best_value);
		}

		// set result for python dlls
		EMOCGeneralResult py_res;
		py_res.igd = result.last_igd;
		py_res.hv = result.last_hv;
		py_res.gd = result.last_igd;
		py_res.spacing = result.last_igd;
		py_res.igdplus = result.last_igd;
		py_res.gdplus = result.last_igd;
		py_res.runtime = result.runtime;
		py_res.best_value = result.best_value;
		py_res.pop_num = g->algorithm_->GetRealPopNum();

		// record population information
		py_res.pop_decs.clear();
		py_res.pop_objs.clear();
		for (int j = 0; j < py_res.pop_num; j++)
		{
			py_res.pop_decs.push_back(std::vector<double>(dec_num));
			py_res.pop_objs.push_back(std::vector<double>(obj_num));
			for (int k = 0; k < dec_num; k++)
				py_res.pop_decs[j][k] = g->parent_population_[j]->dec_[k];

			for (int k = 0; k < obj_num; k++)
				py_res.pop_objs[j][k] = g->parent_population_[j]->obj_[k];
		}
		EMOCManager::Instance()->SetPythonResult(py_res);
	}

	void CollectMultiThreadResult(int run_id, int parameter_id, int thread_id)
	{
		Global* g = EMOCManager::Instance()->GetGlobalSetting(thread_id);
		// TODO: differentiate single-obj and multi-obj

		if (g->obj_num_ > 1)				// multi-objective
		{
			std::string problem = g->problem_name_;
			int dec_num = g->dec_num_;
			int obj_num = g->obj_num_;
			int pop_num = g->algorithm_->GetRealPopNum();
			HVCalculator hv_calculator(obj_num, pop_num);
			int pf_size = 0;
			double** pf_data = LoadPFData(pf_size, obj_num, problem);

			double igd = CalculateIGD(g->parent_population_.data(), pop_num, obj_num, pf_data, pf_size);
			double hv = hv_calculator.Calculate(g->parent_population_.data(), pop_num, obj_num, pf_data, pf_size);
			double gd = CalculateGD(g->parent_population_.data(), pop_num, obj_num, pf_data, pf_size);
			double spacing = CalculateSpacing(g->parent_population_.data(), pop_num, obj_num);
			double igdplus = CalculateIGDPlus(g->parent_population_.data(), pop_num, obj_num, pf_data, pf_size);
			double gdplus = CalculateGDPlus(g->parent_population_.data(), pop_num, obj_num, pf_data, pf_size);
			double runtime = g->algorithm_->GetRuntime();

			// In experiment module, we record the result when it is really finished
			if (g->current_evaluation_ >= g->max_evaluation_)
			{
				EMOCMultiThreadResult& res = EMOCManager::Instance()->GetMultiThreadResult(parameter_id);
				res.runtime.metric_history[run_id] = runtime;
				res.igd.metric_history[run_id] = igd;
				res.hv.metric_history[run_id] = hv;
				res.gd.metric_history[run_id] = gd;
				res.spacing.metric_history[run_id] = spacing;
				res.igdplus.metric_history[run_id] = igdplus;
				res.gdplus.metric_history[run_id] = gdplus;
				res.best_value.metric_history[run_id] = -1;
				res.runtime.is_record[run_id] = true;
				res.igd.is_record[run_id] = true;
				res.hv.is_record[run_id] = true;
				res.gd.is_record[run_id] = true;
				res.spacing.is_record[run_id] = true;
				res.igdplus.is_record[run_id] = true;
				res.gdplus.is_record[run_id] = true;
				res.best_value.is_record[run_id] = true;

				// update statistic result only in gui mode
				if (EMOCManager::Instance()->GetIsGUI())
				{
					// update counter, mean, std, etc...
					UpdateExpResult(res, run_id, parameter_id);

					// update hypothesis test data
					UpdateExpStatTest(parameter_id);
				}
			}

			std::string print_problem = problem + "_" + std::to_string(obj_num) + "_" + std::to_string(dec_num);
			printf("current thread id : %d, parameter: %d runs: %d, runtime: %f igd:%f algorithm:%s, problem:%s\n",
				thread_id, parameter_id, run_id, runtime, igd, g->algorithm_name_.c_str(), print_problem.c_str());

			// free pf data memory
			for (int i = 0; i < pf_size; ++i)
				delete[] pf_data[i];
			delete[] pf_data;
		}
		else if (g->obj_num_ == 1)
		{
			std::string problem = g->problem_name_;
			int dec_num = g->dec_num_;
			int obj_num = g->obj_num_;
			int pop_num = g->algorithm_->GetRealPopNum();

			double igd = -1;
			double hv = -1;
			double gd = -1;
			double spacing = -1;
			double igdplus = -1;
			double gdplus = -1;
			double runtime = g->algorithm_->GetRuntime();

			// In experiment module, we record the result when it is really finished
			if (g->current_evaluation_ >= g->max_evaluation_)
			{
				EMOCMultiThreadResult& res = EMOCManager::Instance()->GetMultiThreadResult(parameter_id);
				res.runtime.metric_history[run_id] = runtime;
				res.igd.metric_history[run_id] = igd;
				res.hv.metric_history[run_id] = hv;
				res.gd.metric_history[run_id] = gd;
				res.spacing.metric_history[run_id] = spacing;
				res.igdplus.metric_history[run_id] = igdplus;
				res.gdplus.metric_history[run_id] = gdplus;
				res.best_value.metric_history[run_id] = g->parent_population_[0]->obj_[0];
				res.runtime.is_record[run_id] = true;
				res.igd.is_record[run_id] = true;
				res.hv.is_record[run_id] = true;
				res.gd.is_record[run_id] = true;
				res.spacing.is_record[run_id] = true;
				res.igdplus.is_record[run_id] = true;
				res.gdplus.is_record[run_id] = true;
				res.best_value.is_record[run_id] = true;

				// update statistic result only in gui mode
				if (EMOCManager::Instance()->GetIsGUI())
				{
					// update counter, mean, std, etc...
					UpdateExpResult(res, run_id, parameter_id);

					// update hypothesis test data
					UpdateExpStatTest(parameter_id);
				}
			}

			std::string print_problem = problem + "_" + std::to_string(obj_num) + "_" + std::to_string(dec_num);
			printf("current thread id : %d, parameter: %d runs: %d, runtime: %f best_value:%f algorithm:%s, problem:%s\n",
				thread_id, parameter_id, run_id, runtime, g->parent_population_[0]->obj_[0], g->algorithm_name_.c_str(), print_problem.c_str());
		}
	}

	int GetBestParameterIndex(int start, int end, const std::string& metric, const std::string& format)
	{
		int res = start;
		double best_value1 = 0.0f;
		double best_value2 = 0.0f;

		GetComparedMetric(metric, format, start, best_value1, best_value2);

		bool is_min_better = true;
		if (metric == "HV") is_min_better = false;
		for (int i = start + 1; i < end; i++)
		{
			double current_value1, current_value2;
			GetComparedMetric(metric, format, i, current_value1, current_value2);
			if ((is_min_better && best_value1 > current_value1) || (!is_min_better && best_value1 < current_value1))
			{
				res = i;
				best_value1 = current_value1;
				best_value2 = current_value2;
			}
			else if (std::fabs(best_value1 - current_value1) < EMOC_EPS)
			{
				if (format == "Median")
				{
					if ((is_min_better && best_value2 > current_value2) || (!is_min_better && best_value2 < current_value2))
					{
						res = i;
						best_value1 = current_value1;
						best_value2 = current_value2;
					}
				}
				else if (format == "Mean")  // standard deviation is still the smaller the better
				{
					if (best_value2 > current_value2)
					{
						res = i;
						best_value1 = current_value1;
						best_value2 = current_value2;
					}
				}
			}
		}

		return res;
	}

	void StatisticTestAccordingMetric(EMOCMultiThreadResult& res, EMOCMultiThreadResult& compared_res, const std::string& metric, const std::string& format)
	{
		int index = 2;
		if (format == "Mean") index = 0;
		else if (format == "Median") index = 1;

		if (metric == "Runtime")
		{
			bool is_diff_ranksum = RankSumTest(res.runtime.metric_history, compared_res.runtime.metric_history);
			bool is_diff_signrank = SignRankTest(res.runtime.metric_history, compared_res.runtime.metric_history);
			if (res.runtime.metric_mean_ranksum[index] == -2) res.runtime.metric_mean_ranksum[index] = is_diff_ranksum ? (res.runtime.metric_mean < compared_res.runtime.metric_mean ? 1 : -1) : 0;
			if (res.runtime.metric_median_ranksum[index] == -2)res.runtime.metric_median_ranksum[index] = is_diff_ranksum ? (res.runtime.metric_median < compared_res.runtime.metric_median ? 1 : -1) : 0;
			if (res.runtime.metric_mean_signrank[index] == -2) res.runtime.metric_mean_signrank[index] = is_diff_signrank ? (res.runtime.metric_mean < compared_res.runtime.metric_mean ? 1 : -1) : 0;
			if (res.runtime.metric_median_signrank[index] == -2)res.runtime.metric_median_signrank[index] = is_diff_signrank ? (res.runtime.metric_median < compared_res.runtime.metric_median ? 1 : -1) : 0;
		}
		else if (metric == "IGD")
		{
			bool is_diff_ranksum = RankSumTest(res.igd.metric_history, compared_res.igd.metric_history);
			bool is_diff_signrank = SignRankTest(res.igd.metric_history, compared_res.igd.metric_history);
			if (res.igd.metric_mean_ranksum[index] == -2) res.igd.metric_mean_ranksum[index] = is_diff_ranksum ? (res.igd.metric_mean < compared_res.igd.metric_mean ? 1 : -1) : 0;
			if (res.igd.metric_median_ranksum[index] == -2)res.igd.metric_median_ranksum[index] = is_diff_ranksum ? (res.igd.metric_median < compared_res.igd.metric_median ? 1 : -1) : 0;
			if (res.igd.metric_mean_signrank[index] == -2) res.igd.metric_mean_signrank[index] = is_diff_signrank ? (res.igd.metric_mean < compared_res.igd.metric_mean ? 1 : -1) : 0;
			if (res.igd.metric_median_signrank[index] == -2)res.igd.metric_median_signrank[index] = is_diff_signrank ? (res.igd.metric_median < compared_res.igd.metric_median ? 1 : -1) : 0;
		}
		else if (metric == "HV")
		{
			// note that hv is the bigger the better
			bool is_diff_ranksum = RankSumTest(res.hv.metric_history, compared_res.hv.metric_history);
			bool is_diff_signrank = SignRankTest(res.hv.metric_history, compared_res.hv.metric_history);
			if (res.hv.metric_mean_ranksum[index] == -2) res.hv.metric_mean_ranksum[index] = is_diff_ranksum ? (res.hv.metric_mean > compared_res.hv.metric_mean ? 1 : -1) : 0;
			if (res.hv.metric_median_ranksum[index] == -2)res.hv.metric_median_ranksum[index] = is_diff_ranksum ? (res.hv.metric_median > compared_res.hv.metric_median ? 1 : -1) : 0;
			if (res.hv.metric_mean_signrank[index] == -2) res.hv.metric_mean_signrank[index] = is_diff_signrank ? (res.hv.metric_mean > compared_res.hv.metric_mean ? 1 : -1) : 0;
			if (res.hv.metric_median_signrank[index] == -2)res.hv.metric_median_signrank[index] = is_diff_signrank ? (res.hv.metric_median > compared_res.hv.metric_median ? 1 : -1) : 0;
		}
		else if (metric == "GD")
		{
			bool is_diff_ranksum = RankSumTest(res.gd.metric_history, compared_res.gd.metric_history);
			bool is_diff_signrank = SignRankTest(res.gd.metric_history, compared_res.gd.metric_history);
			if (res.gd.metric_mean_ranksum[index] == -2) res.gd.metric_mean_ranksum[index] = is_diff_ranksum ? (res.gd.metric_mean < compared_res.gd.metric_mean ? 1 : -1) : 0;
			if (res.gd.metric_median_ranksum[index] == -2)res.gd.metric_median_ranksum[index] = is_diff_ranksum ? (res.gd.metric_median < compared_res.gd.metric_median ? 1 : -1) : 0;
			if (res.gd.metric_mean_signrank[index] == -2) res.gd.metric_mean_signrank[index] = is_diff_signrank ? (res.gd.metric_mean < compared_res.gd.metric_mean ? 1 : -1) : 0;
			if (res.gd.metric_median_signrank[index] == -2)res.gd.metric_median_signrank[index] = is_diff_signrank ? (res.gd.metric_median < compared_res.gd.metric_median ? 1 : -1) : 0;
		}
		else if (metric == "Spacing")
		{
			bool is_diff_ranksum = RankSumTest(res.spacing.metric_history, compared_res.spacing.metric_history);
			bool is_diff_signrank = SignRankTest(res.spacing.metric_history, compared_res.spacing.metric_history);
			if (res.spacing.metric_mean_ranksum[index] == -2) res.spacing.metric_mean_ranksum[index] = is_diff_ranksum ? (res.spacing.metric_mean < compared_res.spacing.metric_mean ? 1 : -1) : 0;
			if (res.spacing.metric_median_ranksum[index] == -2)res.spacing.metric_median_ranksum[index] = is_diff_ranksum ? (res.spacing.metric_median < compared_res.spacing.metric_median ? 1 : -1) : 0;
			if (res.spacing.metric_mean_signrank[index] == -2) res.spacing.metric_mean_signrank[index] = is_diff_signrank ? (res.spacing.metric_mean < compared_res.spacing.metric_mean ? 1 : -1) : 0;
			if (res.spacing.metric_median_signrank[index] == -2)res.spacing.metric_median_signrank[index] = is_diff_signrank ? (res.spacing.metric_median < compared_res.spacing.metric_median ? 1 : -1) : 0;
		}
		else if (metric == "IGDPlus")
		{
			bool is_diff_ranksum = RankSumTest(res.igdplus.metric_history, compared_res.igdplus.metric_history);
			bool is_diff_signrank = SignRankTest(res.igdplus.metric_history, compared_res.igdplus.metric_history);
			if (res.igdplus.metric_mean_ranksum[index] == -2) res.igdplus.metric_mean_ranksum[index] = is_diff_ranksum ? (res.igdplus.metric_mean < compared_res.igdplus.metric_mean ? 1 : -1) : 0;
			if (res.igdplus.metric_median_ranksum[index] == -2)res.igdplus.metric_median_ranksum[index] = is_diff_ranksum ? (res.igdplus.metric_median < compared_res.igdplus.metric_median ? 1 : -1) : 0;
			if (res.igdplus.metric_mean_signrank[index] == -2) res.igdplus.metric_mean_signrank[index] = is_diff_signrank ? (res.igdplus.metric_mean < compared_res.igdplus.metric_mean ? 1 : -1) : 0;
			if (res.igdplus.metric_median_signrank[index] == -2)res.igdplus.metric_median_signrank[index] = is_diff_signrank ? (res.igdplus.metric_median < compared_res.igdplus.metric_median ? 1 : -1) : 0;
		}
		else if (metric == "GDPlus")
		{
			bool is_diff_ranksum = RankSumTest(res.gdplus.metric_history, compared_res.gdplus.metric_history);
			bool is_diff_signrank = SignRankTest(res.gdplus.metric_history, compared_res.gdplus.metric_history);
			if (res.gdplus.metric_mean_ranksum[index] == -2) res.gdplus.metric_mean_ranksum[index] = is_diff_ranksum ? (res.gdplus.metric_mean < compared_res.gdplus.metric_mean ? 1 : -1) : 0;
			if (res.gdplus.metric_median_ranksum[index] == -2)res.gdplus.metric_median_ranksum[index] = is_diff_ranksum ? (res.gdplus.metric_median < compared_res.gdplus.metric_median ? 1 : -1) : 0;
			if (res.gdplus.metric_mean_signrank[index] == -2) res.gdplus.metric_mean_signrank[index] = is_diff_signrank ? (res.gdplus.metric_mean < compared_res.gdplus.metric_mean ? 1 : -1) : 0;
			if (res.gdplus.metric_median_signrank[index] == -2)res.gdplus.metric_median_signrank[index] = is_diff_signrank ? (res.gdplus.metric_median < compared_res.gdplus.metric_median ? 1 : -1) : 0;
		}
		else if (metric == "BestValue")
		{
			bool is_diff_ranksum = RankSumTest(res.best_value.metric_history, compared_res.best_value.metric_history);
			bool is_diff_signrank = SignRankTest(res.best_value.metric_history, compared_res.best_value.metric_history);
			if (res.best_value.metric_mean_ranksum[index] == -2) res.best_value.metric_mean_ranksum[index] = is_diff_ranksum ? (res.best_value.metric_mean < compared_res.best_value.metric_mean ? 1 : -1) : 0;
			if (res.best_value.metric_median_ranksum[index] == -2)res.best_value.metric_median_ranksum[index] = is_diff_ranksum ? (res.best_value.metric_median < compared_res.best_value.metric_median ? 1 : -1) : 0;
			if (res.best_value.metric_mean_signrank[index] == -2) res.best_value.metric_mean_signrank[index] = is_diff_signrank ? (res.best_value.metric_mean < compared_res.best_value.metric_mean ? 1 : -1) : 0;
			if (res.best_value.metric_median_signrank[index] == -2)res.best_value.metric_median_signrank[index] = is_diff_signrank ? (res.best_value.metric_median < compared_res.best_value.metric_median ? 1 : -1) : 0;
		}
		else
		{
			// TODO... ADD MORE METRICS

		}
	}

	void UpdateExpStatTest(int parameter_index)
	{
		std::lock_guard<std::mutex> locker(EMOCLock::mutex_pool[parameter_index % EMOCLock::mutex_pool.size()]);
		int range_index = parameter_index / UIPanelManager::Instance()->GetExpAlgorithmNum();
		int range_start = range_index * UIPanelManager::Instance()->GetExpAlgorithmNum(), range_end = (range_index + 1) * UIPanelManager::Instance()->GetExpAlgorithmNum();

		bool is_ready = true;
		for (int i = range_start; i < range_end; i++)
		{
			EMOCMultiThreadResult& res = EMOCManager::Instance()->GetMultiThreadResult(i);
			if (res.valid_res_count < res.runtime.metric_history.size())
				is_ready = false;
		}

		// Only do statistic test the when datas in the same row are ready. 
		if (!is_ready) return;

		// Take the last column algorithm to be the compared object as default.
		int compared_parameter_index = range_end - 1;
		EMOCMultiThreadResult& default_compared_res = EMOCManager::Instance()->GetMultiThreadResult(compared_parameter_index);;
		int igdmean_best_index = GetBestParameterIndex(range_start, range_end, "IGD", "Mean");
		int igdmedian_best_index = GetBestParameterIndex(range_start, range_end, "IGD", "Median");
		int runtimemean_best_index = GetBestParameterIndex(range_start, range_end, "Runtime", "Mean");
		int runtimemedian_best_index = GetBestParameterIndex(range_start, range_end, "Runtime", "Median");
		int hvmean_best_index = GetBestParameterIndex(range_start, range_end, "HV", "Mean");
		int hvmedian_best_index = GetBestParameterIndex(range_start, range_end, "HV", "Median");
		int gdmean_best_index = GetBestParameterIndex(range_start, range_end, "GD", "Mean");
		int gdmedian_best_index = GetBestParameterIndex(range_start, range_end, "GD", "Median");
		int spacingmean_best_index = GetBestParameterIndex(range_start, range_end, "Spacing", "Mean");
		int spacingmedian_best_index = GetBestParameterIndex(range_start, range_end, "Spacing", "Median");
		int igdplusmean_best_index = GetBestParameterIndex(range_start, range_end, "IGDPlus", "Mean");
		int igdplusmedian_best_index = GetBestParameterIndex(range_start, range_end, "IGDPlus", "Median");
		int gdplusmean_best_index = GetBestParameterIndex(range_start, range_end, "GDPlus", "Mean");
		int gdplusmedian_best_index = GetBestParameterIndex(range_start, range_end, "GDPlus", "Median");
		int bestvalue_mean_best_index = GetBestParameterIndex(range_start, range_end, "BestValue", "Mean");
		int bestvalue_median_best_index = GetBestParameterIndex(range_start, range_end, "BestValue", "Median");

		for (int i = range_start; i < range_end; i++)
		{
			EMOCMultiThreadResult& res = EMOCManager::Instance()->GetMultiThreadResult(i);

			// default comparision
			if (i != range_end - 1)
			{
				StatisticTestAccordingMetric(res, default_compared_res, "Runtime", "Default");
				StatisticTestAccordingMetric(res, default_compared_res, "IGD", "Default");
				StatisticTestAccordingMetric(res, default_compared_res, "HV", "Default");
				StatisticTestAccordingMetric(res, default_compared_res, "GD", "Default");
				StatisticTestAccordingMetric(res, default_compared_res, "Spacing", "Default");
				StatisticTestAccordingMetric(res, default_compared_res, "IGDPlus", "Default");
				StatisticTestAccordingMetric(res, default_compared_res, "GDPlus", "Default");
				StatisticTestAccordingMetric(res, default_compared_res, "BestValue", "Default");
			}

			// mean best comparision
			if (i != runtimemean_best_index)
			{
				EMOCMultiThreadResult& meanbest_compared_res = EMOCManager::Instance()->GetMultiThreadResult(runtimemean_best_index);
				StatisticTestAccordingMetric(res, meanbest_compared_res, "Runtime", "Mean");
			}
			if (i != igdmean_best_index)
			{
				EMOCMultiThreadResult& meanbest_compared_res = EMOCManager::Instance()->GetMultiThreadResult(igdmean_best_index);
				StatisticTestAccordingMetric(res, meanbest_compared_res, "IGD", "Mean");
			}
			if (i != hvmean_best_index)
			{
				EMOCMultiThreadResult& meanbest_compared_res = EMOCManager::Instance()->GetMultiThreadResult(hvmean_best_index);
				StatisticTestAccordingMetric(res, meanbest_compared_res, "HV", "Mean");
			}
			if (i != gdmean_best_index)
			{
				EMOCMultiThreadResult& meanbest_compared_res = EMOCManager::Instance()->GetMultiThreadResult(gdmean_best_index);
				StatisticTestAccordingMetric(res, meanbest_compared_res, "GD", "Mean");
			}
			if (i != spacingmean_best_index)
			{
				EMOCMultiThreadResult& meanbest_compared_res = EMOCManager::Instance()->GetMultiThreadResult(spacingmean_best_index);
				StatisticTestAccordingMetric(res, meanbest_compared_res, "Spacing", "Mean");
			}
			if (i != igdplusmean_best_index)
			{
				EMOCMultiThreadResult& meanbest_compared_res = EMOCManager::Instance()->GetMultiThreadResult(igdplusmean_best_index);
				StatisticTestAccordingMetric(res, meanbest_compared_res, "IGDPlus", "Mean");
			}
			if (i != gdplusmean_best_index)
			{
				EMOCMultiThreadResult& meanbest_compared_res = EMOCManager::Instance()->GetMultiThreadResult(gdplusmean_best_index);
				StatisticTestAccordingMetric(res, meanbest_compared_res, "GDPlus", "Mean");
			}
			if (i != bestvalue_mean_best_index)
			{
				EMOCMultiThreadResult& meanbest_compared_res = EMOCManager::Instance()->GetMultiThreadResult(bestvalue_mean_best_index);
				StatisticTestAccordingMetric(res, meanbest_compared_res, "BestValue", "Mean");
			}

			// median best comparision
			if (i != runtimemedian_best_index)
			{
				EMOCMultiThreadResult& medianbest_compared_res = EMOCManager::Instance()->GetMultiThreadResult(runtimemedian_best_index);
				StatisticTestAccordingMetric(res, medianbest_compared_res, "Runtime", "Median");
			}
			if (i != igdmedian_best_index)
			{
				EMOCMultiThreadResult& medianbest_compared_res = EMOCManager::Instance()->GetMultiThreadResult(igdmedian_best_index);
				StatisticTestAccordingMetric(res, medianbest_compared_res, "IGD", "Median");
			}
			if (i != hvmedian_best_index)
			{
				EMOCMultiThreadResult& medianbest_compared_res = EMOCManager::Instance()->GetMultiThreadResult(hvmedian_best_index);
				StatisticTestAccordingMetric(res, medianbest_compared_res, "HV", "Median");
			}
			if (i != gdmedian_best_index)
			{
				EMOCMultiThreadResult& medianbest_compared_res = EMOCManager::Instance()->GetMultiThreadResult(gdmedian_best_index);
				StatisticTestAccordingMetric(res, medianbest_compared_res, "GD", "Median");
			}
			if (i != spacingmedian_best_index)
			{
				EMOCMultiThreadResult& medianbest_compared_res = EMOCManager::Instance()->GetMultiThreadResult(spacingmedian_best_index);
				StatisticTestAccordingMetric(res, medianbest_compared_res, "Spacing", "Median");
			}
			if (i != igdplusmedian_best_index)
			{
				EMOCMultiThreadResult& medianbest_compared_res = EMOCManager::Instance()->GetMultiThreadResult(igdplusmedian_best_index);
				StatisticTestAccordingMetric(res, medianbest_compared_res, "IGDPlus", "Median");
			}
			if (i != gdplusmedian_best_index)
			{
				EMOCMultiThreadResult& medianbest_compared_res = EMOCManager::Instance()->GetMultiThreadResult(gdplusmedian_best_index);
				StatisticTestAccordingMetric(res, medianbest_compared_res, "GDPlus", "Median");
			}
			if (i != bestvalue_median_best_index)
			{
				EMOCMultiThreadResult& medianbest_compared_res = EMOCManager::Instance()->GetMultiThreadResult(bestvalue_median_best_index);
				StatisticTestAccordingMetric(res, medianbest_compared_res, "BestValue", "Median");
			}
		}
	}

	void UpdateExpResult(EMOCMultiThreadResult& res, int new_res_index, int parameter_index)
	{
		std::lock_guard<std::mutex> locker(EMOCLock::mutex_pool[parameter_index % EMOCLock::mutex_pool.size()]);
		res.valid_res_count++;

		UpdateExpMetricStat(res.igd.metric_history, res.igd.is_record, res.igd.metric_mean, res.igd.metric_std, res.igd.metric_median, res.igd.metric_iqr);
		UpdateExpMetricStat(res.hv.metric_history, res.hv.is_record, res.hv.metric_mean, res.hv.metric_std, res.hv.metric_median, res.hv.metric_iqr);
		UpdateExpMetricStat(res.runtime.metric_history, res.runtime.is_record, res.runtime.metric_mean, res.runtime.metric_std, res.runtime.metric_median, res.runtime.metric_iqr);
		UpdateExpMetricStat(res.gd.metric_history, res.gd.is_record, res.gd.metric_mean, res.gd.metric_std, res.gd.metric_median, res.gd.metric_iqr);
		UpdateExpMetricStat(res.spacing.metric_history, res.spacing.is_record, res.spacing.metric_mean, res.spacing.metric_std, res.spacing.metric_median, res.spacing.metric_iqr);
		UpdateExpMetricStat(res.igdplus.metric_history, res.igdplus.is_record, res.igdplus.metric_mean, res.igdplus.metric_std, res.igdplus.metric_median, res.igdplus.metric_iqr);
		UpdateExpMetricStat(res.gdplus.metric_history, res.gdplus.is_record, res.gdplus.metric_mean, res.gdplus.metric_std, res.gdplus.metric_median, res.gdplus.metric_iqr);
		UpdateExpMetricStat(res.best_value.metric_history, res.best_value.is_record, res.best_value.metric_mean, res.best_value.metric_std, res.best_value.metric_median, res.best_value.metric_iqr);
	}

	void UpdateExpMetricStat(std::vector<double>& indicator_history, std::vector<bool>& is_indicator_record, double& mean, double& std, double& median, double& iqr)
	{
		mean = 0.0, std = 0.0;
		int count = 0;
		for (int c = 0; c < indicator_history.size(); c++)
		{
			if (is_indicator_record[c])
			{
				mean += indicator_history[c];
				count++;
			}
		}
		mean = mean / count;
		for (int c = 0; c < indicator_history.size(); c++)
		{
			if (is_indicator_record[c])
				std += (indicator_history[c] - mean) * (indicator_history[c] - mean);
		}
		std = std / count; std = std::sqrt(std);


		std::vector<double> temp_ind;
		for (int c = 0; c < indicator_history.size(); c++)
			if (is_indicator_record[c])
				temp_ind.push_back(indicator_history[c]);
		std::sort(temp_ind.begin(), temp_ind.end());

		// early return when the size of result is not enough
		if (temp_ind.size() == 1)
		{
			median = temp_ind[0];
			iqr = 0.0;
			return;
		}

		// calculate median
		if (temp_ind.size() % 2)
			median = temp_ind[temp_ind.size() / 2];
		else
			median = (temp_ind[temp_ind.size() / 2] + temp_ind[temp_ind.size() / 2 - 1]) / 2.0;

		// calculate iqr
		double Q1 = 0, Q3 = 0;
		int Q1_remain = temp_ind.size() * 25 % 100;
		int Q1_index = temp_ind.size() * 25 / 100;		// be careful with the start index is 0 which is different from the calculation in WiKi
		if (Q1_remain == 0)
			Q1 = (temp_ind[Q1_index] + temp_ind[Q1_index - 1]) / 2.0;
		else
			Q1 = temp_ind[Q1_index];

		int Q3_remain = temp_ind.size() * 75 % 100;
		int Q3_index = temp_ind.size() * 75 / 100;		// be careful with the start index is 0 which is different from the calculation in WiKi
		if (Q3_remain == 0)
			Q3 = (temp_ind[Q3_index] + temp_ind[Q3_index - 1]) / 2.0;
		else
			Q3 = temp_ind[Q3_index];

		iqr = Q3 - Q1;
	}

}