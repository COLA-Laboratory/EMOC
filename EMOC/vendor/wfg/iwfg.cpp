/*
 * iwfg.c:
 *  This is a slight modification of the original main function of iwfg algorithm that is used to identify the least
 *  HV contribution within a population of solutions. We download this file from http://www.wfg.csse.uwa.edu.au/hypervolume/
 *
 * Authors:
 *  Renzhi Chen <rxc332@cs.bham.ac.uk>
 *  Ke Li <k.li@exeter.ac.uk>
 *
 * Copyright (c) 2017 Renzhi Chen, Ke Li
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

# include <stdio.h>
# include <stdbool.h>
# include <math.h>
# include <time.h>
//# include <sys/resource.h>
# include "iwfg.h"
# include "core/global.h"

int i_fr = 0;   // current depth
int i_n;     // the number of objectives
POINT i_ref; // the reference point
POINT i_dirs;// records the directions of the objectives

FRONT *i_fs;    // memory management stuff

int i_maxm; // maxmimum number of points
int i_maxn; // maximum number of objectives
int i_safe;
int objective_num;

double* partial; //partial exclusive hypervolumes
int* heap; //heap-based priority queue
int heapsize; //number of points in queue
SLICE **stacks; //set of slices per point per slicing depth
int *stacksize; //current slicing depth per point

int* gorder; //objective order used by comparison functions
int** torder; //order of objectives per point
int** tcompare;
FRONT* fsorted; //front sorted in each objective


/* sort point indexes into groups of last objective */
int i_sorter(const void *a, const void *b)
{
	int i = *(int*)a;
	int j = *(int*)b;
	if (torder[i][i_n - 1] == torder[j][i_n - 1])
		return tcompare[j][torder[j][i_n - 1]] - tcompare[i][torder[i][i_n - 1]];
	else
		return torder[i][i_n - 1] - torder[j][i_n - 1];
}

/* this sorts points worsening in the last objective */
int i_greater(const void *v1, const void *v2)
{
	int i;
	POINT p = *(POINT*)v1;
	POINT q = *(POINT*)v2;
	for (i = i_n - 1; i >= 0; i--)
	{
		if BEATS(p.objectives[i], q.objectives[i])
			return -1;
		else if BEATS(q.objectives[i], p.objectives[i])
			return  1;
	}

	return 0;
}

/* this sorts points worsening in the last objective for a certain objective ordering */
int i_same(const void *v1, const void *v2)
{
	int i;
	POINT p = *(POINT*)v1;
	POINT q = *(POINT*)v2;
	for (i = i_n - 1; i >= 0; i--)
	{
		if (p.objectives[i] != q.objectives[i])
			return 0;
	}

	return 1;
}

/* this sorts points worsening in the last objective for a certain objective ordering */
int i_greaterorder(const void *v1, const void *v2)
{
	int i;
	POINT p = *(POINT*)v1;
	POINT q = *(POINT*)v2;
	for (i = i_n - 1; i >= 0; i--)
	{
		if BEATS(p.objectives[gorder[i]], q.objectives[gorder[i]])
			return -1;
		else if BEATS(q.objectives[gorder[i]], p.objectives[gorder[i]])
			return  1;
	}

	return 0;
}

/* this sorts points worsening in the penultimate objective */
int i_greaterabbrev(const void *v1, const void *v2)
{
	int i;
	POINT p = *(POINT*)v1;
	POINT q = *(POINT*)v2;
	for (i = i_n - 2; i >= 0; i--)
	{
		if BEATS(p.objectives[i], q.objectives[i])
			return -1;
		else if BEATS(q.objectives[i], p.objectives[i])
			return  1;
	}

	return 0;
}

/* this sorts points worsening in the penultimate objective for a certain objective ordering */
int i_greaterabbrevorder(const void *v1, const void *v2)
{
	int i;
	POINT p = *(POINT*)v1;
	POINT q = *(POINT*)v2;
	for (i = i_n - 2; i >= 0; i--)
	{
		if BEATS(p.objectives[gorder[i]], q.objectives[gorder[i]])
			return -1;
		else if BEATS(q.objectives[gorder[i]], p.objectives[gorder[i]])
			return  1;
	}

	return 0;
}

/* returns -1 if p dominates q, 1 if q dominates p, 2 if p == q, 0 otherwise k is the highest index inspected */
int i_dominates2way(POINT p, POINT q, int k)
{
	int i, j;
	for (i = k; i >= 0; i--)
	{
		if BEATS(p.objectives[i], q.objectives[i])
		{
			for (j = i - 1; j >= 0; j--)
			{
				if BEATS(q.objectives[j], p.objectives[j])
					return 0;
			}
			return -1;
		}
		else if BEATS(q.objectives[i], p.objectives[i])
		{
			for (j = i - 1; j >= 0; j--)
			{
				if BEATS(p.objectives[j], q.objectives[j])
					return 0;
			}
			return  1;
		}
	}

	return 2;
}

/* returns true if p dominates q or p == q, false otherwise the assumption is that q doesn't dominate p
 * k is the highest index inspected */
int i_dominates1way(POINT p, POINT q, int k)
{
	int i;
	for (i = k; i >= 0; i--)
		if BEATS(q.objectives[i], p.objectives[i])
			return 0;

	return 1;
}

/* returns true if p dominates q or p == q, false otherwise the assumption is that q doesn't dominate p
 * k is the highest index inspected */
int i_dominates1wayOrder(POINT p, POINT q, int k, int* order)
{
	int i;
	for (i = k; i >= 0; i--)
		if BEATS(q.objectives[order[i]], p.objectives[order[i]])
			return 0;

	return 1;
}

/* points below l are all equal in the last objective; points above l are all worse points below l can dominate each
 * other, and we don't need to compare the last objective points above l cannot dominate points that start below l,
 * and we don't need to compare the last objective */
void i_removeDominated(int l, int limit)
{
	int i, j, k;
	POINT t;
	i_fs[i_fr].nPoints = 1;
	for (i = 1; i < l; i++)
	{
		j = 0;
		while (j < i_fs[i_fr].nPoints) {
			switch (i_dominates2way(i_fs[i_fr].points[i], i_fs[i_fr].points[j], i_n - 2))
			{
			case  0:
				j++;
				break;
			case -1: // AT THIS POINT WE KNOW THAT i CANNOT BE DOMINATED BY ANY OTHER PROMOTED POINT j
				// SWAP i INTO j, AND 1-WAY DOM FOR THE REST OF THE js
				t = i_fs[i_fr].points[j];
				i_fs[i_fr].points[j] = i_fs[i_fr].points[i];
				i_fs[i_fr].points[i] = t;
				while (j < i_fs[i_fr].nPoints - 1 && i_dominates1way(i_fs[i_fr].points[j], i_fs[i_fr].points[i_fs[i_fr].nPoints - 1], i_n - 1))
					i_fs[i_fr].nPoints--;
				k = j + 1;
				while (k < i_fs[i_fr].nPoints)
				{
					if (i_dominates1way(i_fs[i_fr].points[j], i_fs[i_fr].points[k], i_n - 2))
					{
						t = i_fs[i_fr].points[k];
						i_fs[i_fr].nPoints--;
						i_fs[i_fr].points[k] = i_fs[i_fr].points[i_fs[i_fr].nPoints];
						i_fs[i_fr].points[i_fs[i_fr].nPoints] = t;
					}
					else
						k++;
				}
			default:
				j = i_fs[i_fr].nPoints + 1;
			}
		}
		if (j == i_fs[i_fr].nPoints)
		{
			t = i_fs[i_fr].points[i_fs[i_fr].nPoints];
			i_fs[i_fr].points[i_fs[i_fr].nPoints] = i_fs[i_fr].points[i];
			i_fs[i_fr].points[i] = t;
			i_fs[i_fr].nPoints++;
		}
	}
	i_safe = WORSE(l, i_fs[i_fr].nPoints);
	for (i = l; i < limit; i++)
	{
		j = 0;
		while (j < i_safe)
		{
			if (i_dominates1way(i_fs[i_fr].points[j], i_fs[i_fr].points[i], i_n - 2))
				j = i_fs[i_fr].nPoints + 1;
			else
				j++;
		}
		while (j < i_fs[i_fr].nPoints)
		{
			switch (i_dominates2way(i_fs[i_fr].points[i], i_fs[i_fr].points[j], i_n - 1))
			{
			case  0:
				j++;
				break;
			case -1: // AT THIS POINT WE KNOW THAT i CANNOT BE DOMINATED BY ANY OTHER PROMOTED POINT j
				// SWAP i INTO j, AND 1-WAY DOM FOR THE REST OF THE js
				t = i_fs[i_fr].points[j];
				i_fs[i_fr].points[j] = i_fs[i_fr].points[i];
				i_fs[i_fr].points[i] = t;
				while (j < i_fs[i_fr].nPoints - 1 && i_dominates1way(i_fs[i_fr].points[j], i_fs[i_fr].points[i_fs[i_fr].nPoints - 1], i_n - 1))
					i_fs[i_fr].nPoints--;
				k = j + 1;
				while (k < i_fs[i_fr].nPoints)
				{
					if (i_dominates1way(i_fs[i_fr].points[j], i_fs[i_fr].points[k], i_n - 1))
					{
						t = i_fs[i_fr].points[k];
						i_fs[i_fr].nPoints--;
						i_fs[i_fr].points[k] = i_fs[i_fr].points[i_fs[i_fr].nPoints];
						i_fs[i_fr].points[i_fs[i_fr].nPoints] = t;
					}
					else
						k++;
				}
			default:
				j = i_fs[i_fr].nPoints + 1;
			}
		}
		if (j == i_fs[i_fr].nPoints)
		{
			t = i_fs[i_fr].points[i_fs[i_fr].nPoints];
			i_fs[i_fr].points[i_fs[i_fr].nPoints] = i_fs[i_fr].points[i];
			i_fs[i_fr].points[i] = t;
			i_fs[i_fr].nPoints++;
		}
	}
	i_fr++;
}

/* creates the front ps[0 .. p-1] in i_fs[i_fr], with each point bounded by ps[p] and dominated points removed */
void i_makeDominatedBit(FRONT ps, int p)
{
	int i, j;
	int l = 0;
	int u = p - 1;
	for (i = p - 1; i >= 0; i--)
	{
		if (BEATS(ps.points[p].objectives[i_n - 1], ps.points[i].objectives[i_n - 1]))
		{
			i_fs[i_fr].points[u].objectives[i_n - 1] = ps.points[i].objectives[i_n - 1];
			for (j = 0; j < i_n - 1; j++)
				i_fs[i_fr].points[u].objectives[j] = WORSE(ps.points[p].objectives[j], ps.points[i].objectives[j]);
			u--;
		}
		else
		{
			i_fs[i_fr].points[l].objectives[i_n - 1] = ps.points[p].objectives[i_n - 1];
			for (j = 0; j < i_n - 1; j++)
				i_fs[i_fr].points[l].objectives[j] = WORSE(ps.points[p].objectives[j], ps.points[i].objectives[j]);
			l++;
		}
	}
	i_removeDominated(l, p);
}


/* returns the hypervolume of ps[0 .. k-1] in 2D assumes that ps is sorted improving */
double i_hv2(FRONT ps, int k)
{
	int i;
	double volume = ps.points[0].objectives[0] * ps.points[0].objectives[1];
	for (i = 1; i < k; i++)
		volume += ps.points[i].objectives[1] * (ps.points[i].objectives[0] - ps.points[i - 1].objectives[0]);

	return volume;
}


/* returns the inclusive hypervolume of p */
double i_inclhv(POINT p)
{
	int i;
	double volume = 1;
	for (i = 0; i < i_n; i++)
		volume *= p.objectives[i];

	return volume;
}

/* returns the inclusive hypervolume of p */
double i_inclhvOrder(POINT p, int* order)
{
	int i;
	double volume = 1;
	for (i = 0; i < i_n; i++)
		volume *= p.objectives[order[i]];

	return volume;
}

/* returns the hypervolume of {p, q} */
double i_inclhv2(POINT p, POINT q)
{

	//    printf("%f  %f\n",q.objectives[0],q.objectives[1]);
	int i;
	double vp = 1;
	double vq = 1;
	double vpq = 1;
	for (i = 0; i < i_n; i++)
	{
		vp *= p.objectives[i];
		vq *= q.objectives[i];
		vpq *= WORSE(p.objectives[i], q.objectives[i]);
	}

	return vp + vq - vpq;
}

/* returns the hypervolume of {p, q, r} */
double i_inclhv3(POINT p, POINT q, POINT r)
{
	int i;
	double vp = 1;
	double vq = 1;
	double vr = 1;
	double vpq = 1;
	double vpr = 1;
	double vqr = 1;
	double vpqr = 1;
	for (i = 0; i < i_n; i++)
	{
		vp *= p.objectives[i];
		vq *= q.objectives[i];
		vr *= r.objectives[i];
		if (BEATS(p.objectives[i], q.objectives[i]))
		{
			if (BEATS(q.objectives[i], r.objectives[i]))
			{
				vpq *= q.objectives[i];
				vpr *= r.objectives[i];
				vqr *= r.objectives[i];
				vpqr *= r.objectives[i];
			}
			else
			{
				vpq *= q.objectives[i];
				vpr *= WORSE(p.objectives[i], r.objectives[i]);
				vqr *= q.objectives[i];
				vpqr *= q.objectives[i];
			}
		}
		else if (BEATS(p.objectives[i], r.objectives[i]))
		{
			vpq *= p.objectives[i];
			vpr *= r.objectives[i];
			vqr *= r.objectives[i];
			vpqr *= r.objectives[i];
		}
		else
		{
			vpq *= p.objectives[i];
			vpr *= p.objectives[i];
			vqr *= WORSE(q.objectives[i], r.objectives[i]);
			vpqr *= p.objectives[i];
		}

	}
	return vp + vq + vr - vpq - vpr - vqr + vpqr;
}

/* returns the hypervolume of {p, q, r, s} */
double i_inclhv4(POINT p, POINT q, POINT r, POINT s)
{
	int i;
	double vp = 1;
	double vq = 1;
	double vr = 1;
	double vs = 1;
	double vpq = 1;
	double vpr = 1;
	double vps = 1;
	double vqr = 1;
	double vqs = 1;
	double vrs = 1;
	double vpqr = 1;
	double vpqs = 1;
	double vprs = 1;
	double vqrs = 1;
	double vpqrs = 1;
	for (i = 0; i < i_n; i++)
	{
		vp *= p.objectives[i];
		vq *= q.objectives[i];
		vr *= r.objectives[i];
		vs *= s.objectives[i];
		if (BEATS(p.objectives[i], q.objectives[i]))
		{
			if (BEATS(q.objectives[i], r.objectives[i]))
			{
				if (BEATS(r.objectives[i], s.objectives[i]))
				{
					vpq *= q.objectives[i];
					vpr *= r.objectives[i];
					vps *= s.objectives[i];
					vqr *= r.objectives[i];
					vqs *= s.objectives[i];
					vrs *= s.objectives[i];
					vpqr *= r.objectives[i];
					vpqs *= s.objectives[i];
					vprs *= s.objectives[i];
					vqrs *= s.objectives[i];
					vpqrs *= s.objectives[i];
				}
				else
				{
					OBJECTIVE z1 = WORSE(q.objectives[i], s.objectives[i]);
					vpq *= q.objectives[i];
					vpr *= r.objectives[i];
					vps *= WORSE(p.objectives[i], s.objectives[i]);
					vqr *= r.objectives[i];
					vqs *= z1;
					vrs *= r.objectives[i];
					vpqr *= r.objectives[i];
					vpqs *= z1;
					vprs *= r.objectives[i];
					vqrs *= r.objectives[i];
					vpqrs *= r.objectives[i];
				}
			}
			else if (BEATS(q.objectives[i], s.objectives[i]))
			{
				vpq *= q.objectives[i];
				vpr *= WORSE(p.objectives[i], r.objectives[i]);
				vps *= s.objectives[i];
				vqr *= q.objectives[i];
				vqs *= s.objectives[i];
				vrs *= s.objectives[i];
				vpqr *= q.objectives[i];
				vpqs *= s.objectives[i];
				vprs *= s.objectives[i];
				vqrs *= s.objectives[i];
				vpqrs *= s.objectives[i];
			}
			else
			{
				OBJECTIVE z1 = WORSE(p.objectives[i], r.objectives[i]);
				vpq *= q.objectives[i];
				vpr *= z1;
				vps *= WORSE(p.objectives[i], s.objectives[i]);
				vqr *= q.objectives[i];
				vqs *= q.objectives[i];
				vrs *= WORSE(r.objectives[i], s.objectives[i]);
				vpqr *= q.objectives[i];
				vpqs *= q.objectives[i];
				vprs *= WORSE(z1, s.objectives[i]);
				vqrs *= q.objectives[i];
				vpqrs *= q.objectives[i];
			}
		}
		else if (BEATS(q.objectives[i], r.objectives[i]))
		{
			if (BEATS(p.objectives[i], s.objectives[i]))
			{
				OBJECTIVE z1 = WORSE(p.objectives[i], r.objectives[i]);
				OBJECTIVE z2 = WORSE(r.objectives[i], s.objectives[i]);
				vpq *= p.objectives[i];
				vpr *= z1;
				vps *= s.objectives[i];
				vqr *= r.objectives[i];
				vqs *= s.objectives[i];
				vrs *= z2;
				vpqr *= z1;
				vpqs *= s.objectives[i];
				vprs *= z2;
				vqrs *= z2;
				vpqrs *= z2;
			}
			else
			{
				OBJECTIVE z1 = WORSE(p.objectives[i], r.objectives[i]);
				OBJECTIVE z2 = WORSE(r.objectives[i], s.objectives[i]);
				vpq *= p.objectives[i];
				vpr *= z1;
				vps *= p.objectives[i];
				vqr *= r.objectives[i];
				vqs *= WORSE(q.objectives[i], s.objectives[i]);
				vrs *= z2;
				vpqr *= z1;
				vpqs *= p.objectives[i];
				vprs *= z1;
				vqrs *= z2;
				vpqrs *= z1;
			}
		}
		else if (BEATS(p.objectives[i], s.objectives[i]))
		{
			vpq *= p.objectives[i];
			vpr *= p.objectives[i];
			vps *= s.objectives[i];
			vqr *= q.objectives[i];
			vqs *= s.objectives[i];
			vrs *= s.objectives[i];
			vpqr *= p.objectives[i];
			vpqs *= s.objectives[i];
			vprs *= s.objectives[i];
			vqrs *= s.objectives[i];
			vpqrs *= s.objectives[i];
		}
		else
		{
			OBJECTIVE z1 = WORSE(q.objectives[i], s.objectives[i]);
			vpq *= p.objectives[i];
			vpr *= p.objectives[i];
			vps *= p.objectives[i];
			vqr *= q.objectives[i];
			vqs *= z1;
			vrs *= WORSE(r.objectives[i], s.objectives[i]);
			vpqr *= p.objectives[i];
			vpqs *= p.objectives[i];
			vprs *= p.objectives[i];
			vqrs *= z1;
			vpqrs *= p.objectives[i];
		}
	}
	return vp + vq + vr + vs - vpq - vpr - vps - vqr - vqs - vrs + vpqr + vpqs + vprs + vqrs - vpqrs;
}

/* returns the hypervolume of {p, q, r, s, t} */
double i_inclhv5(POINT p, POINT q, POINT r, POINT s, POINT t)
{
	int i;
	double vp = 1;
	double vq = 1;
	double vr = 1;
	double vs = 1;
	double vt = 1;

	double vpq = 1;
	double vpr = 1;
	double vps = 1;
	double vpt = 1;
	double vqr = 1;
	double vqs = 1;
	double vqt = 1;
	double vrs = 1;
	double vrt = 1;
	double vst = 1;

	double vpqr = 1;
	double vpqs = 1;
	double vpqt = 1;
	double vprs = 1;
	double vprt = 1;
	double vpst = 1;
	double vqrs = 1;
	double vqrt = 1;
	double vqst = 1;
	double vrst = 1;

	double vpqrs = 1;
	double vpqrt = 1;
	double vpqst = 1;
	double vprst = 1;
	double vqrst = 1;

	double vpqrst = 1;

	for (i = 0; i < i_n; i++) {
		vp *= p.objectives[i];
		vq *= q.objectives[i];
		vr *= r.objectives[i];
		vs *= s.objectives[i];
		vt *= t.objectives[i];
		vpq *= WORSE(p.objectives[i], q.objectives[i]);
		vpr *= WORSE(p.objectives[i], r.objectives[i]);
		vps *= WORSE(p.objectives[i], s.objectives[i]);
		vpt *= WORSE(p.objectives[i], t.objectives[i]);
		vqr *= WORSE(q.objectives[i], r.objectives[i]);
		vqs *= WORSE(q.objectives[i], s.objectives[i]);
		vqt *= WORSE(q.objectives[i], t.objectives[i]);
		vrs *= WORSE(r.objectives[i], s.objectives[i]);
		vrt *= WORSE(r.objectives[i], t.objectives[i]);
		vst *= WORSE(s.objectives[i], t.objectives[i]);
		vpqr *= WORSE(p.objectives[i],
			WORSE(q.objectives[i], r.objectives[i]));
		vpqs *= WORSE(p.objectives[i],
			WORSE(q.objectives[i], s.objectives[i]));
		vpqt *= WORSE(p.objectives[i],
			WORSE(q.objectives[i], t.objectives[i]));
		vprs *= WORSE(p.objectives[i],
			WORSE(r.objectives[i], s.objectives[i]));
		vprt *= WORSE(p.objectives[i],
			WORSE(r.objectives[i], t.objectives[i]));
		vpst *= WORSE(p.objectives[i],
			WORSE(s.objectives[i], t.objectives[i]));
		vqrs *= WORSE(q.objectives[i],
			WORSE(r.objectives[i], s.objectives[i]));
		vqrt *= WORSE(q.objectives[i],
			WORSE(r.objectives[i], t.objectives[i]));
		vqst *= WORSE(q.objectives[i],
			WORSE(s.objectives[i], t.objectives[i]));
		vrst *= WORSE(r.objectives[i],
			WORSE(s.objectives[i], t.objectives[i]));
		vpqrs *= WORSE(WORSE(p.objectives[i], q.objectives[i]),
			WORSE(r.objectives[i], s.objectives[i]));
		vpqrt *= WORSE(WORSE(p.objectives[i], q.objectives[i]),
			WORSE(r.objectives[i], t.objectives[i]));
		vpqst *= WORSE(WORSE(p.objectives[i], q.objectives[i]),
			WORSE(s.objectives[i], t.objectives[i]));
		vprst *= WORSE(WORSE(p.objectives[i], r.objectives[i]),
			WORSE(s.objectives[i], t.objectives[i]));
		vqrst *= WORSE(WORSE(q.objectives[i], r.objectives[i]),
			WORSE(s.objectives[i], t.objectives[i]));
		vpqrst *= WORSE(WORSE(p.objectives[i],
			WORSE(q.objectives[i], r.objectives[i])),
			WORSE(s.objectives[i], t.objectives[i]));
	}
	return vp + vq + vr + vs + vt
		- vpq - vpr - vps - vpt - vqr - vqs - vqt - vrs - vrt - vst
		+ vpqr + vpqs + vpqt + vprs + vprt + vpst + vqrs + vqrt + vqst + vrst
		- vpqrs - vpqrt - vpqst - vprst - vqrst
		+ vpqrst;
}

/* returns the exclusive hypervolume of ps[p] relative to ps[0 .. p-1] */
double i_exclhv(FRONT ps, int p)
{
	double volume;
	i_makeDominatedBit(ps, p);
	volume = i_inclhv(ps.points[p]) - i_hv(i_fs[i_fr - 1]);
	i_fr--;

	return volume;
}

double i_hv_contribution(FRONT ps, int id, double whole_hv)
{
	int i;
	for (i = 0; i < ps.n; i++)
		ps.points[id].objectives[i] = 0;

	whole_hv = whole_hv - i_hv(ps);

	return  whole_hv;
}

/* returns the hypervolume of ps[0 ..] */
double i_hv(FRONT ps)
{
	int i;
	double volume;
	// process small fronts with the IEA
	switch (ps.nPoints)
	{
	case 1:
		return i_inclhv(ps.points[0]);
	case 2:
		return i_inclhv2(ps.points[0], ps.points[1]);
	case 3:
		return i_inclhv3(ps.points[0], ps.points[1], ps.points[2]);
	case 4:
		return i_inclhv4(ps.points[0], ps.points[1], ps.points[2], ps.points[3]);
	}

	// these points need sorting
	qsort(&ps.points[i_safe], ps.nPoints - i_safe, sizeof(POINT), i_greater);
	// n = 2 implies that safe = 0
	if (i_n == 2)
		return i_hv2(ps, ps.nPoints);

	// these points don't NEED sorting, but it helps
	qsort(ps.points, i_safe, sizeof(POINT), i_greaterabbrev);

	if (i_n == 3 && i_safe > 0)
	{
		volume = ps.points[0].objectives[2] * i_hv2(ps, i_safe);
		i_n--;
		for (i = i_safe; i < ps.nPoints; i++)
		{
			// we can ditch dominated points here, but they will be ditched anyway in makeDominatedBit
			volume += ps.points[i].objectives[i_n] * i_exclhv(ps, i);
		}
		i_n++;
		return volume;
	}
	else
	{
		double volume = i_inclhv4(ps.points[0], ps.points[1], ps.points[2], ps.points[3]);
		i_n--;
		for (i = 4; i < ps.nPoints; i++)
		{
			// we can ditch dominated points here, but they will be ditched anyway in makeDominatedBit
			volume += ps.points[i].objectives[i_n] * i_exclhv(ps, i);
		}
		i_n++;
		return volume;
	}
}

/* creates the front ps in i_fs[i_fr], with each point bounded by p and dominated points removed */
void i_makeDominatedBitPoint(FRONT ps, POINT p, int* order)
{
	//printf("i_n(c)%d\n",i_n);
	int i, j;
	int l = 0;
	int u = ps.nPoints - 1;
	for (i = ps.nPoints - 1; i >= 0; i--)
	{
		if (BEATS(p.objectives[order[i_n - 1]], ps.points[i].objectives[order[i_n - 1]]))
		{
			i_fs[i_fr].points[u].objectives[i_n - 1] = ps.points[i].objectives[order[i_n - 1]];
			for (j = 0; j < i_n - 1; j++)
				i_fs[i_fr].points[u].objectives[j] = WORSE(p.objectives[order[j]], ps.points[i].objectives[order[j]]);
			u--;
		}
		else {
			i_fs[i_fr].points[l].objectives[i_n - 1] = p.objectives[order[i_n - 1]];
			for (j = 0; j < i_n - 1; j++)
				i_fs[i_fr].points[l].objectives[j] = WORSE(p.objectives[order[j]], ps.points[i].objectives[order[j]]);
			l++;
		}
	}
	i_removeDominated(l, ps.nPoints);
}

/* returns the exclusive hypervolume of p relative to ps */
double i_exclhvPoint(FRONT ps, POINT p, int* order)
{

	double volume;
	i_makeDominatedBitPoint(ps, p, order);
	//printf("i_n(c)%d\n",i_n);
	volume = i_inclhvOrder(p, order) - i_hv(i_fs[i_fr - 1]);
	//printf("i_n(c)%d\n",i_n);
	i_fr--;
	return volume;
}

/* restores heap property starting at location and working downwards to place index in heap */
void i_heapify(int location, int index)
{
	bool left, right;
	while (2 * location + 2 < heapsize)
	{
		left = false;
		right = false;
		if (partial[heap[2 * location + 1]] < partial[index])
			left = true;
		if (partial[heap[2 * location + 2]] < partial[index])
			right = true;
		if (left)
		{
			if (right && partial[heap[2 * location + 2]] < partial[heap[2 * location + 1]])
			{
				heap[location] = heap[2 * location + 2];
				location = 2 * location + 2;
			}
			else
			{
				heap[location] = heap[2 * location + 1];
				location = 2 * location + 1;
			}
		}
		else if (right)
		{
			heap[location] = heap[2 * location + 2];
			location = 2 * location + 2;
		}
		else
			break;
	}
	if (2 * location + 1 < heapsize && partial[heap[2 * location + 1]] < partial[index])
	{
		heap[location] = heap[2 * location + 1];
		location = 2 * location + 1;
	}
	heap[location] = index;
}

int i_peekFromHeap(void)
{
	return heap[0];
}

/* creates the heap with the indexes 0..(capacity-1)  */
void i_initialiseHeap(int capacity)
{
	int i;
	heapsize = capacity;
	for (i = heapsize - 1; i >= 0; i--)
		i_heapify(i, i);
}

/* inserts p into pl with the result in stacks[i][j] */
void i_insert(POINT p, int k, FRONT pl, int i, int j, int *order)
{
	int place = 0;
	int placeNext;
	while (place < pl.nPoints && pl.points[place].objectives[order[k]] > p.objectives[order[k]])
	{
		stacks[i][j].front.points[place] = pl.points[place];
		place++;
	}
	POINT pp = pl.points[place];
	stacks[i][j].front.points[place] = p;
	placeNext = place + 1;
	POINT ppn = pl.points[place + 1];
	while (place < pl.nPoints)
	{
		if (!i_dominates1wayOrder(p, pp, k, order))
		{
			stacks[i][j].front.points[placeNext] = pp;
			placeNext++;
		}
		place++;
		pp = ppn;
		ppn = pl.points[place + 1];
	}
	stacks[i][j].front.nPoints = placeNext;
}

/* slice using a separate objective ordering per point */
void i_sliceOrder(int nPoints)
{
	int i, j, p, k, l;
	int seen;
	int pos, start, end;

	int *sorder = new int[nPoints];
	for (i = 0; i < nPoints; i++)
		sorder[i] = i;
	qsort(sorder, nPoints, sizeof(int), i_sorter);
	seen = 0;
	for (p = 0; p < nPoints; p++)
	{
		i = sorder[p];
		if (p == 0 || torder[i][i_n - 1] != torder[sorder[p - 1]][i_n - 1])
		{
			seen = 0;
			stacks[i][1].front.nPoints = 0;
		}
		else
		{
			for (j = 0; j < stacks[sorder[p - 1]][1].front.nPoints; j++)
				stacks[i][1].front.points[j] = stacks[sorder[p - 1]][1].front.points[j];
			stacks[i][1].front.nPoints = stacks[sorder[p - 1]][1].front.nPoints;
		}
		pos = nPoints - 1 - tcompare[i][torder[i][i_n - 1]];
		for (j = seen; j < pos; j++)
			stacks[i][1].front.points[stacks[i][1].front.nPoints + j - seen] = stacks[i][0].front.points[j];
		start = stacks[i][1].front.nPoints;
		end = stacks[i][1].front.nPoints + pos - seen;
		seen = pos;
		POINT temp;
		for (j = start; j < end; j++)
		{
			k = 0;
			while (k < stacks[i][1].front.nPoints)
			{
				if (i_dominates1wayOrder(stacks[i][1].front.points[j], stacks[i][1].front.points[k], i_n - 2, torder[i]))
				{
					temp = stacks[i][1].front.points[k];
					stacks[i][1].front.points[k] = stacks[i][1].front.points[j];
					stacks[i][1].front.points[j] = temp;
					while (k < stacks[i][1].front.nPoints - 1 &&
						i_dominates1wayOrder(stacks[i][1].front.points[k], stacks[i][1].front.points[stacks[i][1].front.nPoints - 1], i_n - 2, torder[i]))
					{
						stacks[i][1].front.nPoints--;
					}
					l = k + 1;
					while (l < stacks[i][1].front.nPoints)
					{
						if (i_dominates1wayOrder(stacks[i][1].front.points[k], stacks[i][1].front.points[l], i_n - 2, torder[i]))
						{
							temp = stacks[i][1].front.points[l];
							stacks[i][1].front.nPoints--;
							stacks[i][1].front.points[l] = stacks[i][1].front.points[stacks[i][1].front.nPoints];
							stacks[i][1].front.points[stacks[i][1].front.nPoints] = temp;
						}
						else
							l++;
					}
					k = stacks[i][1].front.nPoints + 1;
				}
				else {
					k++;
				}
			}
			if (k == stacks[i][1].front.nPoints)
			{
				temp = stacks[i][1].front.points[stacks[i][1].front.nPoints];
				stacks[i][1].front.points[stacks[i][1].front.nPoints] = stacks[i][1].front.points[j];
				stacks[i][1].front.points[j] = temp;
				stacks[i][1].front.nPoints++;
			}
		}
		stacks[i][1].index = pos + 1;
		if (pos < nPoints - 1)
			stacks[i][1].width = fabs(stacks[i][0].front.points[pos].objectives[torder[i][i_n - 1]] -
				stacks[i][0].front.points[pos + 1].objectives[torder[i][i_n - 1]]);
		else
			stacks[i][1].width = stacks[i][0].front.points[pos].objectives[torder[i][i_n - 1]];
	}
	for (i = 0; i < nPoints; i++)
	{
		gorder = torder[i];
		qsort(stacks[i][1].front.points, stacks[i][1].front.nPoints, sizeof(POINT), i_greaterabbrevorder);
	}
	delete[] sorder;
}

/* slice in the last objective */
void i_slice(FRONT pl)
{
	int i;

	stacks[0][1].front.nPoints = 0;
	for (i = 0; i < pl.nPoints - 1; i++)
	{
		stacks[i][1].width = fabs(pl.points[i].objectives[i_n - 1] - pl.points[i + 1].objectives[i_n - 1]);
		stacks[i][1].index = i + 1;
		i_insert(pl.points[i], i_n - 2, stacks[i][1].front, i + 1, 1, torder[i]);
	}
	stacks[pl.nPoints - 1][1].width = pl.points[pl.nPoints - 1].objectives[i_n - 1];
	stacks[pl.nPoints - 1][1].index = pl.nPoints;
}

int i_binarySearch(POINT p, int d)
{
	//int i, j;
	int min, mid, max;

	min = 0;
	max = fsorted[d].nPoints - 1;

	gorder = torder[d];
	while (min <= max) {
		mid = (max + min) / 2;
		if (i_same(&p, &(fsorted[d].points[mid])))
			return mid;
		else if (i_greaterorder(&p, &fsorted[d].points[mid]) == -1)
			max = mid - 1;
		else
			min = mid + 1;
	}
	//print_error (1, 1, "EE: Cannot find the point in binary search");
	//printf("p:");
	//for (i = 0; i < emoc::g_GlobalSettings->obj_num_; i++)
	//	printf("%lf ", p.objectives[i]);
	//printf("\n");

	//printf("ps:");
	//for (j = 0; j < fsorted[d].nPoints; j++)
	//{
	//	for (i = 0; i < emoc::g_GlobalSettings->obj_num_; i++)
	//		printf("%lf ", fsorted[d].points[j].objectives[i]);
	//	printf("\n");
	//}

	return -1;
}

void i_runHeuristic(FRONT ps)
{
	int i, j, k;
	for (i = 0; i < i_n - 1; i++)
	{
		torder[i][i_n - 1] = i;
		torder[i][i] = i_n - 1;
	}
	for (i = i_n - 1; i >= 0; i--)
	{
		for (j = 0; j < ps.nPoints; j++)
		{
			fsorted[i].points[j] = ps.points[j];
			tcompare[j][i] = 0;
		}
		fsorted[i].nPoints = ps.nPoints;
		gorder = torder[i];
		qsort(fsorted[i].points, ps.nPoints, sizeof(POINT), i_greaterorder);
	}
	for (i = 0; i < ps.nPoints; i++)
		for (k = 0; k < i_n; k++)
			tcompare[i][k] = ps.nPoints - 1 - i_binarySearch(ps.points[i], k);

	for (i = 0; i < ps.nPoints; i++)
	{
		for (j = 1; j < i_n; j++)
		{
			int x = torder[i][j];
			k = j;
			while (k > 0 && tcompare[i][x] < tcompare[i][torder[i][k - 1]])
			{
				torder[i][k] = torder[i][k - 1];
				k--;
			}
			torder[i][k] = x;
		}
	}
}

int i_slicingDepth(int d)
{
	if (d <= 5) return 1;
	if (d <= 7) return 2;
	if (d <= 12) return 3;
	return 4;
}

/* returns the minimum exclusive hypervolume of points in ps for the 2D case */
void i_ihv2(FRONT ps, double *min)
{
	int k;
	int sm;
	double kvol;
	double vol;

	qsort(ps.points, ps.nPoints, sizeof(POINT), i_greater);
	vol = ps.points[0].objectives[0] * (ps.points[0].objectives[1] - ps.points[1].objectives[1]);
	sm = 0;
	for (k = 1; k < ps.nPoints - 1; k++)
	{
		kvol = (ps.points[k].objectives[0] - ps.points[k - 1].objectives[0]) * (ps.points[k].objectives[1] - ps.points[k + 1].objectives[1]);
		if (kvol <= 0)
		{
			min[0] = ps.points[k].objectives[0];
			min[1] = ps.points[k].objectives[1];
			min[2] = 0;

			return;
		}
		else
			if (kvol < vol)
			{
				vol = kvol;
				sm = k;
			}
	}
	kvol = (ps.points[ps.nPoints - 1].objectives[0] - ps.points[ps.nPoints - 2].objectives[0]) * ps.points[ps.nPoints - 1].objectives[1];
	if (kvol < vol)
	{
		vol = kvol;
		sm = ps.nPoints - 1;
	}
	min[0] = ps.points[sm].objectives[0];
	min[1] = ps.points[sm].objectives[1];
	min[2] = vol;

}
//
//i_fs = (FRONT*)malloc(sizeof(FRONT) * max_depth);
//partial = (double*)malloc(sizeof(double) * i_maxm);
//heap = (int*)malloc(sizeof(int) * i_maxm);
//stacksize = (int*)malloc(sizeof(int) * i_maxm);
//stacks = (SLICE**)malloc(sizeof(SLICE*) * i_maxm);
//fsorted = (FRONT*)malloc(sizeof(FRONT) * i_maxn);
//torder = (int**)malloc(sizeof(int *) * MAX(i_maxm, i_maxn));
//tcompare = (int**)malloc(sizeof(int *) * i_maxm);
//int max_stacksize = MIN(i_maxn - 2, i_slicingDepth(i_maxn)) + 1;

/* returns the minimum exclusive hypervolume of points in ps */
void i_ihv(FRONT ps, double *min)
{
	int i, j, k, z;

	int maxStackSize = MIN(i_slicingDepth(i_n), i_n - 2) + 1;   // ****
	for (i = 0; i < MAX(ps.nPoints, i_n); i++)
		for (j = 0; j < i_n; j++)
			torder[i][j] = j;
	i_runHeuristic(ps);  // ****
	for (i = 0; i < ps.nPoints; i++)
	{
		stacks[i][0].front = fsorted[torder[i][i_n - 1]];
		stacks[i][0].width = 1;
		stacksize[i] = 2;
	}
	//printf("i_n(a)%d\n",i_n);
	i_sliceOrder(ps.nPoints); // ****
	i_n--;
	//printf("i_n(b)%d\n",i_n);
	for (i = 0; i < ps.nPoints; i++)
	{

		SLICE top = stacks[i][stacksize[i] - 1];
		while (stacksize[i] < maxStackSize && top.front.nPoints > SLICELIMIT)
		{
			stacks[i][stacksize[i]].front.nPoints = 0;
			int index = 0;
			while (index < top.front.nPoints && ps.points[i].objectives[torder[i][i_n - 1]] < top.front.points[index].objectives[torder[i][i_n - 1]])
			{
				i_insert(top.front.points[index], i_n - 2, stacks[i][stacksize[i]].front, i, stacksize[i], torder[i]);
				index++;
			}
			if (index < top.front.nPoints)
				stacks[i][stacksize[i]].width = ps.points[i].objectives[torder[i][i_n - 1]] - top.front.points[index].objectives[torder[i][i_n - 1]];
			else
				stacks[i][stacksize[i]].width = ps.points[i].objectives[torder[i][i_n - 1]];
			stacks[i][stacksize[i]].index = index;
			top = stacks[i][stacksize[i]];
			stacksize[i]++;
			i_n--;

		}

		double width = 1;
		for (j = 0; j < stacksize[i]; j++)
			width *= stacks[i][j].width;

		if (top.front.nPoints == 0)
			partial[i] = width * i_inclhvOrder(ps.points[i], torder[i]); // ****
		else
			partial[i] = width * i_exclhvPoint(top.front, ps.points[i], torder[i]);
		//printf("i_n(d)%d\n",i_n);
		i_n += stacksize[i] - 2;
		while (stacksize[i] > 1 && (top.index == stacks[i][stacksize[i] - 2].front.nPoints ||
			i_dominates1wayOrder(stacks[i][stacksize[i] - 2].front.points[top.index], ps.points[i], i_n - stacksize[i] + 1, torder[i])))
		{
			stacksize[i]--;
			top = stacks[i][stacksize[i] - 1];
		}
	}

	i_initialiseHeap(ps.nPoints);

	maxStackSize = 2;
	while (true)
	{
		// int i = removeFromHeap();
		i = i_peekFromHeap();
		if (stacksize[i] <= 1)
		{
			for (z = 0; z < ps.n; z++)
				min[z] = ps.points[i].objectives[z];
			min[ps.n] = partial[i];
			break;
		}
		i_n -= stacksize[i] - 2;
		j = stacks[i][stacksize[i] - 1].index;
		if (j < stacks[i][stacksize[i] - 2].front.nPoints - 1)
			stacks[i][stacksize[i] - 1].width = stacks[i][stacksize[i] - 2].front.points[j].objectives[torder[i][i_n]] -
			stacks[i][stacksize[i] - 2].front.points[j + 1].objectives[torder[i][i_n]];
		else
			stacks[i][stacksize[i] - 1].width = stacks[i][stacksize[i] - 2].front.points[j].objectives[torder[i][i_n]];
		i_insert(stacks[i][stacksize[i] - 2].front.points[j], i_n - 1, stacks[i][stacksize[i] - 1].front, i, stacksize[i] - 1, torder[i]);
		stacks[i][stacksize[i] - 1].index = j + 1;
		SLICE top = stacks[i][stacksize[i] - 1];

		double width = 1;
		for (k = 0; k < stacksize[i]; k++)
			width *= stacks[i][k].width;
		if (top.front.nPoints == 0)
			partial[i] += width * i_inclhvOrder(ps.points[i], torder[i]);
		else
			partial[i] += width * i_exclhvPoint(top.front, ps.points[i], torder[i]);
		i_n += stacksize[i] - 2;
		while (stacksize[i] > 1 && (top.index == stacks[i][stacksize[i] - 2].front.nPoints ||
			i_dominates1wayOrder(stacks[i][stacksize[i] - 2].front.points[top.index], ps.points[i], i_n - stacksize[i] + 1, torder[i])))
		{
			stacksize[i]--;
			top = stacks[i][stacksize[i] - 1];
		}

		i_heapify(0, i);
	}
	i_n++;
}

void iwfg_read_data(FILECONTENTS *fc, emoc::Individual **pop_table, double *nadir_point, int pop_size, int obj_num)
{
	int i, j, k;
	int front, point, objective;

	// init the struct
	fc->nFronts = 0;
	fc->fronts = NULL;
	front = fc->nFronts;
	fc->nFronts++;
	fc->fronts = (FRONT*)realloc(fc->fronts, sizeof(FRONT) * fc->nFronts);
	fc->fronts[front].nPoints = 0;
	fc->fronts[front].points = NULL;

	// read the data
	for (i = 0; i < pop_size; i++)
	{
		FRONT *f = &fc->fronts[front];
		point = f->nPoints;
		f->nPoints++;
		f->points = (POINT*)realloc(f->points, sizeof(POINT) * f->nPoints);
		f->n = 0;
		f->points[point].objectives = NULL;

		for (j = 0; j < obj_num; j++)
		{
			POINT *p = &f->points[point];
			objective = f->n;
			f->n++;
			p->objectives = (OBJECTIVE*)realloc(p->objectives, sizeof(OBJECTIVE) * f->n);
			p->objectives[objective] = pop_table[i]->obj_[j];
		}
	}

	/*normalize*/
	for (i = 0; i < fc->nFronts; i++)
	{
		//        printf("here!,%d\n",fc->nFronts);
		for (j = 0; j < fc->fronts[i].nPoints; j++)
		{
			for (k = 0; k < fc->fronts[i].n; k++)
			{
				fc->fronts[i].points[j].objectives[k] = nadir_point[k] - fc->fronts[i].points[j].objectives[k];
				if (fc->fronts[i].points[j].objectives[k] < 0)
					fc->fronts[i].points[j].objectives[k] = 0;
			}
		}
	}
}

