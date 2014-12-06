/*
** inj.c
** By Upinder S. Bhalla, May 92, Caltech.
**
** This file computes the Laplace transform solution for a cable with
** sealed ends and current injection in one end.
*/
#include <stdio.h>
#include <math.h>
#include "nrutil.h"

#define DT 50e-6
#define RUNTIME 0.25
#define TAU0 0.04
#define VREST -0.065
#define X0 0.0
#define X1 0.001			/* 1 mm */
#define LAMBDA 0.001 		/* 1 mm , length constant of cable */	
#define LENGTH 0.001		/* length of cable in metres */
#define DIA 1.0e-6			/* dia of cable in  metres */
#define RA 1.0				/* ohms meter = axial resistivity */
#define INJ 1.0e-10			/* 0.1 nA */
#define NTERMS 10
#define CHECKFLAG 0			/* checks the convergence of the terms,
							** specially sum1. This is very tricky
							** for short cables.
							*/

#define PI 3.1415926535


main(argc,argv)
	int	argc;
	char	**argv;
{
	double t;
	FILE *v0,*vx,*fopen();
	double cablev();
	double atof();
	double tau0=TAU0;
	double runtime=RUNTIME;
	double vrest=VREST;
	double dt=DT;
	double length=LENGTH;
	double ra=RA;
	double x=LENGTH;
	double lambda=LAMBDA;
	double inj=INJ;
	double dia=DIA;
	double v;
	int	nterms=NTERMS;
	int	i;

	for(i=1;i<argc;i++) {
		if (argv[i][0] != '-') {
			do_explanation(argv[0]);
			return;
		}
		switch (argv[i][1]) {
			case 't' : i++; tau0=atof(argv[i]);
				break;
			case 'T' : i++; runtime=atof(argv[i]);
				break;
			case 'D' : i++; dt=atof(argv[i]);
				break;
			case 'v' : i++; vrest=atof(argv[i]);
				break;
			case 'l' : i++; length=atof(argv[i]);
				break;
			case 'd' : i++; dia=atof(argv[i]);
				break;
			case 'x' : i++; x=atof(argv[i]);
				break;
			case 'L' : i++; lambda=atof(argv[i]);
				break;
			case 'i' : i++; inj=atof(argv[i]);
				break;
			case 'r' : i++; ra=atof(argv[i]);
				break;
			case 'n' : i++; nterms=atoi(argv[i]);
				break;
			case 'h' : do_explanation(argv[0]);
				return;
				break;
			default :
				break;
		}
	}
	if (x>length) x=length;

	printf("using : tau0=%f runTime=%f dt=%e vrest=%f RA=%f\n",
		tau0,runtime,dt,vrest,ra);
	printf("lambda=%e x=%e length=%e dia=%e inj=%e nterms=%d\n",
		lambda,x,length,dia,inj,nterms);

	v0=fopen("v0","w");
	vx=fopen("vx","w");
	for (t=0.0;t<=runtime;t+=dt) {
/* The scaling of 2.0 happens because we are stimulating at one end 
** of the cable */
		v=vrest+ra/(dia*dia*PI)*lambda*inj*2.0*
			cablev(X0/lambda,t/tau0,length/lambda,nterms);
		fprintf(v0,"%f	%e\n",t,v);

		v=vrest+ra/(dia*dia*PI)*lambda*inj*2.0*
			cablev(x/lambda,t/tau0,length/lambda,nterms);
		/*
		v=VREST+RA*LAMBDA*INJ/4.0 * 2.0 * cablev(x/LAMBDA,
			t/tau0,length/LAMBDA);
			*/
		fprintf(vx,"%f	%e\n",t,v);
	}
	fclose(v0);
	fclose(vx);
}

do_explanation(name)
	char	*name;
{
	printf("usage : %s [-t tau0] [-T runTime] [-D dt] [-v vrest] [r RA]\n",
		name);
	printf("[-L lambda] [-x distance from inj] [-l length] [-d dia] [-i inj] [-n nterms]\n[-h help]\n");
	printf("Defaults : tau0=%f runTime=%f dt=%e vrest=%f RA=%f\n",
		TAU0,RUNTIME,DT,VREST,RA);
	printf("lambda=%e distance from inj=%e length=%e dia=%e inj=%e nterms=%d\n",
		LAMBDA,LENGTH,LENGTH,DIA,INJ,NTERMS);
	printf("All units in SI == MKSA : metres Kg sec Amps\n");
}

double cablev(x,t,len,nterms)
	double x,t,len;
	int	nterms;
{
	double hx;
	double rt;
	double sum1,sum2;
	float erfc();
	int i;

	if (t <= 0.0)
		return(0.0);

	hx=x/2.0;
	if (t>0.0)
		rt=sqrt(t);
	else
		rt=0.0;
	sum1=sum2=0.0;
	for(i=0;i<nterms;i++) {
		sum1+=exp(-2.0*i*len - x) * erfc((i*len+hx-t)/rt) -
			exp(2.0*i*len+x) * erfc((i*len+hx+t)/rt);
		if (CHECKFLAG && t>6) printf("sum1.%d=%f ",i,sum1);
	}
	if (CHECKFLAG && t>6) printf("\n");
	for(i=1;i<nterms;i++) {
		sum2+=exp(-2.0*i*len + x) * erfc((i*len-hx-t)/rt) -
			exp(2.0*i*len-x) * erfc((i*len-hx+t)/rt);
		if (CHECKFLAG && t>6) printf("sum2.%d=%f ",i,sum2);
	}
	if (CHECKFLAG && t>6) printf("\n");
	return(sum1+sum2);
}
