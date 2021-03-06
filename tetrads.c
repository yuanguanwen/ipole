
/*

all model-independent functions related to creation and 
manipulation of tetrads

*/

#include "decs.h"

/* input and vectors are contravariant (index up) */
void coordinate_to_tetrad(double Ecov[NDIM][NDIM], double K[NDIM],
			  double K_tetrad[NDIM])
{
    int k;

    for (k = 0; k < 4; k++) {
	K_tetrad[k] = Ecov[k][0] * K[0] 
	            + Ecov[k][1] * K[1] 
	            + Ecov[k][2] * K[2] 
	            + Ecov[k][3] * K[3];
    }
}

/* input and vectors are contravariant (index up) */
void tetrad_to_coordinate(double Econ[NDIM][NDIM], double K_tetrad[NDIM],
			  double K[NDIM])
{
    int l;

    for (l = 0; l < 4; l++) {
	K[l] = Econ[0][l] * K_tetrad[0] 
	     + Econ[1][l] * K_tetrad[1] 
	     + Econ[2][l] * K_tetrad[2] 
	     + Econ[3][l] * K_tetrad[3];
    }

    return;
}

#define SMALL_VECTOR	1.e-30

/*
    Kronecker delta
*/
double delta(int i, int j)
{
    if (i == j)
	return (1.);
    else
	return (0.);
}

void lower(double *ucon, double Gcov[NDIM][NDIM], double *ucov)
{

    ucov[0] = Gcov[0][0] * ucon[0]
	+ Gcov[0][1] * ucon[1]
	+ Gcov[0][2] * ucon[2]
	+ Gcov[0][3] * ucon[3];
    ucov[1] = Gcov[1][0] * ucon[0]
	+ Gcov[1][1] * ucon[1]
	+ Gcov[1][2] * ucon[2]
	+ Gcov[1][3] * ucon[3];
    ucov[2] = Gcov[2][0] * ucon[0]
	+ Gcov[2][1] * ucon[1]
	+ Gcov[2][2] * ucon[2]
	+ Gcov[2][3] * ucon[3];
    ucov[3] = Gcov[3][0] * ucon[0]
	+ Gcov[3][1] * ucon[1]
	+ Gcov[3][2] * ucon[2]
	+ Gcov[3][3] * ucon[3];

    return;
}

/* 

    normalize input vector (and overwrite)
    so that |v . v| = 1

*/

void normalize(double *vcon, double Gcov[4][4])
{
    int k, l;
    double norm;

    norm = 0.;
    for (k = 0; k < 4; k++)
	for (l = 0; l < 4; l++)
	    norm += vcon[k] * vcon[l] * Gcov[k][l];

    norm = sqrt(fabs(norm));
    for (k = 0; k < 4; k++)
	vcon[k] /= norm;

    return;
}

/*

    project out vconb from vcona

    both arguments are index up (contravariant)

    covariant metric is third argument.

    overwrite the first argument on return

*/

void project_out(double *vcona, double *vconb, double Gcov[4][4])
{

    double adotb, vconb_sq;
    int k, l;

    vconb_sq = 0.;
    for (k = 0; k < 4; k++)
	for (l = 0; l < 4; l++)
	    vconb_sq += vconb[k] * vconb[l] * Gcov[k][l];

    adotb = 0.;
    for (k = 0; k < 4; k++)
	for (l = 0; l < 4; l++)
	    adotb += vcona[k] * vconb[l] * Gcov[k][l];

    for (k = 0; k < 4; k++)
	vcona[k] -= vconb[k] * adotb / vconb_sq;

    return;
}

/* 

   copy the trial vector into a tetrad basis vector,
   checking to see if it is null, and if it is null
   setting to some default value 

*/
void set_Econ_from_trial(double Econ[4], int defdir, double trial[4])
{
    double norm = 0.;
    int k;

    for (k = 0; k < 4; k++)
	norm += fabs(trial[k]);
    for (k = 0; k < 4; k++)	/* trial vector */
	if (norm <= SMALL_VECTOR)	/* bad trial vector; default to radial direction */
	    Econ[k] = delta(k, defdir);
	else
	    Econ[k] = trial[k];

    return;
}

/* 
    check the handedness of a tetrad basis.

    basis is assumed to be in form e^\mu_{(a)} = Econ[a][mu]

    levi_(ijkl) e0^i e1^j e2^k e3^l will be +1 if spatial
    	components are right-handed, -1 if left-handed.

    experience suggests that roundoff produces errors of
    	order 10^{-12} in the result.

*/

double check_handedness(double Econ[NDIM][NDIM], double Gcov[NDIM][NDIM])
{
    int i, j, k, l;
    static int firstc = 1;
    void set_levi_civita(double levi_civita[NDIM][NDIM][NDIM][NDIM]);
    static double levi_civita[NDIM][NDIM][NDIM][NDIM];

    if (firstc) {
	firstc = 0;
	set_levi_civita(levi_civita);
    }

    double g = gdet_func(Gcov);

    /* check handedness */
    double dot = 0.;
    for (i = 0; i < 4; i++)
	for (j = 0; j < 4; j++)
	    for (l = 0; l < 4; l++)
		for (k = 0; k < 4; k++) {
		    dot += g * levi_civita[i][j][k][l] *
			Econ[0][i] * Econ[1][j] * Econ[2][k] * Econ[3][l];
		}

    return (dot);
}

