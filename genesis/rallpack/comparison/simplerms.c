/*
** rms.c
** By Upinder S. Bhalla, May 1992, Caltech.
**
** This program calculates the rms difference between two
** curves. It assumes identical time steps.
*/

#include <stdio.h>
#include <math.h>

main(argc,argv)
	int	argc;
	char	**argv;
{
	FILE	*f1,*f2,*fopen();
	float 	t1,t2,v1,v2;
	float	sumsq=0.0,npts=0.0;
	float	temp,min=1e10,max=-1e10;
	double	sqrt();

	if (argc<3) {
		printf("usage : %s file1 file2\n",argv[0]);
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

	while(fscanf(f1,"%f %f",&t1,&v1) == 2 &&
		fscanf(f2,"%f %f",&t2,&v2) == 2){
		if (t1!=t2) {
			printf("Error : file time scales are different\n");
			return;
		}
		sumsq+=(v1-v2)*(v1-v2);
		npts+=1.0;
		temp=(v1+v2)/2.0;
		if (min>temp) min=temp;
		else if (max<temp) max=temp;
	}

	printf("absolute rms diff=%e; range=%e;\n normalized rms diff=%e; npts=%d\n",
		sqrt(sumsq/npts),
		max-min,
		sqrt(sumsq/npts)/(max-min),
		(int)npts);
}
