#include "random/random.h"

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>

// #include "print.h"

static double seed;
static double oldrand[55];
static int jrand;

/* Get seed number for random and start it up */
void randomize()
{
	int j1;
	int random;
	for (j1 = 0; j1 <= 54; j1++)
	{
		oldrand[j1] = 0.0;
	}
	jrand = 0;

	// initialize a random seed
	srand((unsigned)time(NULL));
	random = rand() % 1000;
	seed = (double)random / 1000.0;
	// print_error(seed <= 0.0 || seed >= 1.0, 1, "Entered seed value is wrong, seed value must be in (0,1)!");

	warmup_random(seed);

	return;
}

/* Get randomize off and running */
void warmup_random(double seed)
{
	int j1, ii;
	double new_random, prev_random;

	oldrand[54] = seed;
	new_random = 0.000000001;
	prev_random = seed;
	for (j1 = 1; j1 <= 54; j1++)
	{
		ii = (21 * j1) % 54;
		oldrand[ii] = new_random;
		new_random = prev_random - new_random;
		if (new_random < 0.0)
		{
			new_random += 1.0;
		}
		prev_random = oldrand[ii];
	}
	advance_random();
	advance_random();
	advance_random();
	jrand = 0;

	return;
}

/* Create next batch of 55 random numbers */
void advance_random()
{
	int j1;
	double new_random;
	for (j1 = 0; j1 < 24; j1++)
	{
		new_random = oldrand[j1] - oldrand[j1 + 31];
		if (new_random < 0.0)
		{
			new_random = new_random + 1.0;
		}
		oldrand[j1] = new_random;
	}
	for (j1 = 24; j1 < 55; j1++)
	{
		new_random = oldrand[j1] - oldrand[j1 - 24];
		if (new_random < 0.0)
		{
			new_random = new_random + 1.0;
		}
		oldrand[j1] = new_random;
	}
}

/* Fetch a single random number between 0.0 and 1.0 */
double randomperc()
{
	jrand++;
	if (jrand >= 55)
	{
		jrand = 1;
		advance_random();
	}
	return((double)oldrand[jrand]);
}

int rnd(int low, int high)
{
	int res;
	if (low >= high)
	{
		res = low;
	}
	else
	{
		//res = low + (int)(randomperc() * (high - low));
		res = low + (unsigned int)(randomperc() * (high - low + 1));
		if (res > high)
		{
			res = high;
		}
		else if (res < low)
		{
			res = low;
		}
	}
	return (res);
}

void random_permutation(int *perm, int size)
{
	int i, num, start;
	int *index, *flag;

	index = (int *)malloc(size * sizeof(int));
	flag = (int *)malloc(size * sizeof(int));
	for (i = 0; i < size; i++)
	{
		index[i] = i;
		flag[i] = 1;
	}

	num = 0;
	while (num < size)
	{
		start = rnd(0, size - 1);
		while (1)
		{
			if (flag[start])
			{
				perm[num] = index[start];
				flag[start] = 0;
				num++;
				break;
			}
			if (start == (size - 1))
				start = 0;
			else
				start++;
		}
	}

	free(index);
	free(flag);
	return;
}

/* Fetch a single random real number between low and high including the bounds */
double rndreal(double low, double high)
{
	return (low + (high - low) * randomperc());
}

double g_rand(double mi, double sigma) {
	double v1, v2, s, result;
	v1 = ((double)rand() / RAND_MAX) * 2 - 1;
	//printf("%d, %d\n",rand(),RAND_MAX);
	v2 = ((double)rand() / RAND_MAX) * 2 - 1;
	s = v1 * v1 + v2 * v2;

	if (s == 0 || s >= 1)
		return g_rand(mi, sigma);
	result = v1 * sqrt(-2.0*log(s) / s);

	return (mi + result * sigma);
}