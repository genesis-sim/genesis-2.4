/*
** rms.c 
** By Upinder S. Bhalla, May 1992, Caltech.
**
** This program calculates the rms difference between two
** curves. It does linear interpolation to handle differing 
** timescales.
*/

#include <stdio.h>
#include <math.h>

#define MAXPTS 10000

main(argc,argv)
	int	argc;
	char	**argv;
{
	FILE	*f1,*f2,*fopen();
	float 	t1[MAXPTS],t2[MAXPTS],v1[MAXPTS],v2[MAXPTS];
	double	sqrt();
	int		i,j,k;
	float	t,st,interp1,interp2;
	float	temp,min=1e10,max=-1e10;
	float	dt,maxt;
	int		npts1,npts2;
	float	scale,sumsq,nsumsq;
	FILE	*out1,*out2;

	if (argc<3) {
		printf("usage : %s file1 file2 [-out]\n",argv[0]);
		return;
	}

	f1=fopen(argv[1],"r");
	if (!f1) {
		printf("Could not open file %s\n",argv[1]);
		return;
	}
	f2=fopen(argv[2],"r");
	if (!f2) {
		printf("Could not open file %s\n",argv[2]);
		return;
	}

	if (argc==4 && strcmp(argv[3],"-out")==0) {
		out1=fopen("out1","w");
		out2=fopen("out2","w");
		if (!out1 || !out2) {
			printf("Could not open files out1 and out2\n",argv[1]);
			return;
		}
	} else {
		out1=out2=NULL;
	}

	for(i=0;fscanf(f1,"%f %f",&t1[i],&temp) == 2;i++) {
		v1[i]=temp;
		if (min>temp) min=temp;
		else if (max<temp) max=temp;
	}
	npts1=i;
	dt=(t1[npts1-1]-t1[0])/(float)(npts1-1);
	for(i=0;fscanf(f2,"%f %f",&t2[i],&temp) == 2;i++) {
		v2[i]=temp;
		if (min>temp) min=temp;
		else if (max<temp) max=temp;
	}
	npts2=i;
	maxt=(t2[npts2-1]<t1[npts1-1]) ? t2[npts2-1] : t1[npts1-1];


	/* Comparing the spike shape. We need to scale each spike in 
	** time to do this. We will use linear interpolation. */
	j=1;
	k=1;
	for(t=0.0;t<maxt;t+=dt) {
		for(;t>t1[j] && j < npts1;j++);
		interp1=v1[j-1]+(v1[j]-v1[j-1])*
			(t-t1[j-1])/(t1[j]-t1[j-1]);
		st=t;
		for(;st>t2[k] && k < npts2;k++);
		interp2=v2[k-1]+(v2[k]-v2[k-1])*
			(st-t2[k-1])/(t2[k]-t2[k-1]);
		if (out1) {
			fprintf(out1,"%f	%f\n",t,interp1);
			fprintf(out2,"%f	%f\n",t,interp2);
		}
		/* Get the difference */
		temp=interp1-interp2;
		sumsq+=temp*temp;
		nsumsq+=1.0;
	}
	printf("absolute rms diff=%e; range=%e; \n npts=%d		normalized rms diff=%e\n",
		sqrt(sumsq/nsumsq),
		max-min,
		(int)nsumsq,
		sqrt(sumsq/nsumsq)/(max-min));
	fclose(f1);
	fclose(f2);
	if (out1) {
		fclose(out1);
		fclose(out2);
	}

	exit(0);
}
