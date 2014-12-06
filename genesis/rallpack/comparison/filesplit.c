#include <stdio.h>
/* This fhile takes an output data file in mv and msec in the formt
** time val0 valx
** and generates 2 output files in V and sec in the form
** time val
*/


main(argc,argv)
	int argc;
	char	**argv;
{
	FILE	*fp,*fout0,*foutx,*fopen();
	double	t,v0,vx;
	char	fname[200];
	int		i=0;

	if (argc<2) {
		printf("usage : %s datafile\n",argv[0]);
		return;
	}
	if (!(fp=fopen(argv[1],"r"))) {
		printf("could not open file %s for reading\n");
		return;
	}
	sprintf(fname,"%s0",argv[1]);

	if (!(fout0=fopen(fname,"w"))) {
		printf("could not open file %s for writing\n");
		return;
	}

	sprintf(fname,"%sx",argv[1]);
	if (!(foutx=fopen(fname,"w"))) {
		printf("could not open file %s for writing\n");
		return;
	}

	while(fscanf(fp,"%lf%lf%lf",&t,&v0,&vx)==3) {
		i++;
		fprintf(fout0,"%g	%g\n",t/1000.0,v0/1000.0);
		fprintf(foutx,"%g	%g\n",t/1000.0,vx/1000.0);
	}
	printf("%d points found\n",i);
}
