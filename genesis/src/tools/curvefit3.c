static char rcsid[] = "$Id: curvefit3.c,v 1.3 2005/07/20 20:02:04 svitak Exp $";

/*
** $Log: curvefit3.c,v $
** Revision 1.3  2005/07/20 20:02:04  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:10  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  1997/07/18 03:02:36  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/24 16:54:48  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:05:50  dhb
** Initial revision
**
*/


#include <math.h>
#include "sim_ext.h"

#define MAXFITPTS 5001
#define MAXNA 20
#define ENDT 1000.0

/*
** This file has the sum of exp form for the nonlin curvefit routine
** mrqmin to use.
*/

#ifdef OLD
void expsum(x,a,y,dyda,na)
	float	x,a[],*y,dyda[];
	int na;
{
	int i;
	float e1,e2;

	e1 = exp(-x * a[3]);
	e2 = exp(-x * a[5]);

	*y = a[1] + a[2] * e1 + a[4] * e2;
	dyda[1] = 1;
	dyda[2] = e1;
	dyda[3] = -a[2] * x * e1;
	dyda[4] = e2;
	dyda[5] = -a[4] * x * e2;
}

void fitsum(x,a,y,dyda,na)
	float	x,a[],*y,dyda[];
	int na;
{
	int i;
	float e;

	dyda[1] = 1;
	*y = a[1];
	for (i=2;i<na;i += 2){
		e = exp(-x * a[i+1]);
		*y += a[i] * e;
		dyda[i] = e;
		dyda[i+1]= -a[i] * x * e;
	}
	if ((na%2) == 0) {
		*y += a[na] * x;
		dyda[na]=x;
	}
}
#endif

void fitsum(x,a,y,dyda,na)
	float	x,a[],*y,dyda[];
	int na;
{
	int i;
	float e,e0;
	float sig;

	dyda[1] = 1;
	*y = a[1];
	e0 = exp(-x*a[3]);
	dyda[2] = e0;

	sig = a[2];

	for(i=4;i<na;i+=2) {
		/* getting the sum for tau0 */
		sig -= a[i];
		/* the diff of the Vi's */
		e=exp(-x*a[i+1]);
		dyda[i]=e-e0;
		/* calculating the tau-i term */
		e *= a[i];
		*y += e;
		/* the diff of the tau-i's */
		dyda[i+1]= -x*e;
	}
	/* calculating the tau0 term */
	e0 *= sig;
	*y += e0;
	/* getting the diff for tau0 */
	dyda[3]= -x*e0;
}

void spikeparmoutput(allvars,vardevs,varnames,nvars,filename,filemode,notes)
	float **allvars;
	float **vardevs;
	char **varnames;
	int	nvars;
	char	*filename;
	char	*filemode;
	char	*notes;
{
	int i;
	static char	parmfile[200];
	FILE	*fp,*fopen();

	if (strcmp(filemode,"multifile") == 0) {
		for (i = 0 ; i < nvars ; i++) {
			sprintf(parmfile,"%s.%s",filename,varnames[i]);
			fp = fopen(parmfile,"a");
			if (vardevs) {
				if (notes)
					fprintf(fp,"%s	%g	%g\n",notes,*(allvars[i]),
						*(vardevs[i]));
				else
					fprintf(fp,"%g	%g\n",*(allvars[i]),*(vardevs[i]));
			} else {
				if (notes)
					fprintf(fp,"%s	%g\n",notes,*(allvars[i]));
				else
					fprintf(fp,"%g\n",*(allvars[i]));
			}
			fclose(fp);
		}
	}
	if (strcmp(filemode,"onefile") == 0) {
		fp = fopen(filename,"w");
		if (notes)
			fprintf(fp,"\n%s\n",notes);
		for (i = 0 ; i < nvars ; i++) {
			if (vardevs) {
				fprintf(fp,"%-12s	%-12g	+/-	%-12g	(%1.4f %%)\n",
					varnames[i],*(allvars[i]), *(vardevs[i]),
					*(vardevs[i]) * 100.0/ *(allvars[i]));
			} else {
				fprintf(fp,"%s	%g	",varnames[i],*(allvars[i]));
				if ((i%3) == 2)
					fprintf(fp,"\n");
			}
		}	
		fprintf(fp,"\n");
		fclose(fp);
	}
	if (strcmp(filemode,"onefileappend") == 0) {
		fp = fopen(filename,"a");
		if (notes)
			fprintf(fp,"\n%s\n",notes);
		for (i = 0 ; i < nvars ; i++) {
			fprintf(fp,"%s	%g	",varnames[i],*(allvars[i]));
			if ((i%3) == 2)
				fprintf(fp,"\n");
		}	
		fprintf(fp,"\n");
		fclose(fp);
	}
}

char *do_expfit(argc,argv)
	int		argc;
	char	**argv;
{
	float	x[MAXFITPTS],y[MAXFITPTS],sig[MAXFITPTS];
	float	a[MAXNA],dummyda[MAXNA];
	int		lista[MAXNA];
	int		na,mfit;
	float	**covar,**alpha;
	float	chisq;
	float	lastchisq;
	float	alamda;
	int		plotno = 0;
	float	startt = 0,endt = ENDT,dt = 0,t=0;
	int		niterations=0,nexpterms=2;
	float	Echarge,Erest;
	float	Vm0;
	char	*outputfile,*filemode;
	char	*expfile,*diffile;
	char	*notes;
	int		npts;
	int		i = 0;
	static char ret[200];
	float	**matrix();
	FILE	*fp,*fopen();
	float	temp;

	float	*allvars[50];
	char	*varnames[50];
	int	nvars;
	int	status;

	ret[0] = '\0';
	expfile = NULL;
	diffile = NULL;
	notes=NULL;
	outputfile=NULL;
	filemode=NULL;

	initopt(argc, argv, "file-name Erest Echarge -plot plot-number -start time -end time -iterations n -expterms n -file output-file-name file-mode -notes notes -diffile file-name -expfile file-name");
	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    if (strcmp(G_optopt,"-plot") == 0) {
		    plotno = atoi(optargv[1]);
	    }
	    else if (strcmp(G_optopt,"-start") == 0) {
		    startt = Atof(optargv[1]);
	    }
	    else if (strcmp(G_optopt,"-end") == 0) {
		    endt = Atof(optargv[1]);
	    }
	    else if (strcmp(G_optopt,"-iterations") == 0) {
		    niterations = atoi(optargv[1]);
	    }
	    else if (strcmp(G_optopt,"-expterms") == 0) {
		    nexpterms = atoi(optargv[1]);
		    if (nexpterms < 1)
			    nexpterms = 1;
	    }
	    else if (strcmp(G_optopt,"-file") == 0) {
		    if (argc < (i + 2)) {
			    printf("syntax : -file outputfile mode\n");
			    return NULL;
		    }
		    outputfile = optargv[1];
		    filemode = optargv[2];
	    }
	    else if (strcmp(G_optopt,"-notes") == 0) {
		    notes = optargv[1];
	    }
	    else if (strcmp(G_optopt,"-expfile") == 0) {
		    expfile = optargv[1];
	    }
	    else if (strcmp(G_optopt,"-diffile") == 0) {
		    diffile = optargv[1];
	    }
	  }

	if (status < 0) {
		printoptusage(argc, argv);
		printf("\n");
		printf("-start and -end specify the range of points in the plot that will be fitted\n");
		printf("-iterations n specifies the number of iterations : min if +ve, max if -ve\n");
		printf("It forces the routine to print out chisq for each stage\n");
		printf("-expterms n sets the number of exponential terms that the routine must fit\n");
		printf("Valid file-modes : multifile onefile onefileappend\n");
		printf("-diffile writes the error in the curve to diffile\n");
		printf("-expfile writes the fitted curve to expfile\n");
		return NULL;
	}

	Erest = Atof(optargv[2]);
	Echarge = Atof(optargv[3]);

	na = 1 + nexpterms * 2;
	if (na >= MAXNA) {
		printf("Max no of nexpterms is %d. Abandoning.\n",MAXNA/2 - 2);
		return NULL;
	}

	nvars = 0;
	allvars[nvars] = &Erest; varnames[nvars] = "Erest"; nvars++;
	allvars[nvars] = &Echarge; varnames[nvars] = "Echarge"; nvars++;
	allvars[nvars] = &chisq; varnames[nvars] = "chisq"; nvars++;
	allvars[nvars] = &Vm0; varnames[nvars] = "Vm0"; nvars++;
	for (i = 3 ; i < (2 + nexpterms * 2) ; i++) {
		allvars[nvars] = &(a[i]);
		varnames[nvars] = (char *) malloc(8 * sizeof(char *));
		if (i%2)
			sprintf(varnames[nvars],"tau%d",(i-2)/2);
		else
			sprintf(varnames[nvars],"Vm%d",(i-2)/2);
		nvars++;
	}

	if (!read_plot2(optargv[1],plotno,&dt,y,MAXFITPTS - 1,&npts,startt,endt)) {
		printf("Read plot failed\n");
		return("failed");
	}
	for (i=0,t=startt;i<npts;i++,t+=dt)
		x[i]=t;
	endt = t-dt;


/* INITIALISING VARIABLES */

	alpha = (float **)malloc((na + 1) * sizeof(float *));
	for (i = 0 ; i <= na ; i++)
		alpha[i] = (float *)malloc((na+1) * sizeof(float));
	covar = (float **)malloc((na+1) * sizeof(float *));
	for (i = 0 ; i <= na ; i++)
		covar[i] = (float *)malloc((na+1) * sizeof(float));

/* Since we have the solver in the straighforward 
**	A + (B-sigVi)exp(-t/tau0) + v1 exp(-t/tau1) + ....
** form, we need to rearrange variables to deal with decay time courses
*/
	a[1]=Erest+Echarge;
	a[2]= -Echarge;
	a[3]=1.0/0.020;
	a[4]=0.002;
	a[5]=1.0/0.002;
	for (i=6;i<=na;i++){
		if (i%2) /* odds i.e taus */
			a[i]= 5 * i * i;
		else
			a[i]= -0.04/((float)i);
	}

	mfit = na-2;
	for (i=0;i<na;i++)
		lista[i] = i+2;

	for (i = npts ; i > 0 ; i--) {
		x[i] = x[i - 1];
		y[i] = y[i - 1];
		sig[i] = 0.0002; /* assume 0.2 mV error */
		/* moving the values up because of their stupid
		** convention of starting at 1 */
	}
	
/* INITIALISING THE FUNCTION */
	alamda = -1;
	if (mrqmin(x,y,sig,npts,a,na,lista,mfit,covar,alpha,&chisq,
		fitsum,&alamda) == 0) /* error */
		goto cleanup;
	lastchisq = chisq * 1.01 + 1;

/* LOOPING TILL CONVERGENCE */
	if (niterations < 0) {
		niterations = -niterations;
		for (i = 1; i <= niterations; i++) {
			lastchisq = chisq;
			if (mrqmin(x,y,sig,npts,a,na,lista,mfit,covar,alpha,&chisq,
				fitsum,&alamda) == 0)
				goto cleanup;
			printf("Iteration %d, chisq = %f, lamda = %f\n",
				i,chisq,alamda);
		}
	} else {
		for (i=1;((lastchisq - chisq)/1000.0) > 0.001 ||
			lastchisq <=  chisq || i <= niterations; i++) {
			lastchisq = chisq;
			if (mrqmin(x,y,sig,npts,a,na,lista,mfit,covar,alpha,&chisq,
				fitsum,&alamda) == 0)
				goto cleanup;
			printf("Iteration %d, chisq = %f, lamda = %f\n",
				i,chisq,alamda);
		}
	}

/* WRITING THE ERROR FILE */
	if (diffile) {
		if (!(fp = fopen(diffile,"w"))) {
			printf ("could not open diffile '%s'\n",diffile);
		} else {
			for (i = 1 ; i <= npts ; i++) {
				fitsum(x[i],a,&temp,dummyda,na);
				fprintf(fp,"%f	%f\n",x[i],y[i]-temp);
			}
			fclose(fp);
		}
	}
	if (expfile) {
		if (!(fp = fopen(expfile,"w"))) {
			printf ("could not open expfile '%s'\n",expfile);
		} else {
			for (i = 1 ; i <= npts ; i++) {
				fitsum(x[i],a,&temp,dummyda,na);
				fprintf(fp,"%f	%f\n",x[i],temp);
			}
			fclose(fp);
		}
	}
/* EXTRACTING THE VARIABLES */
	alamda = 0;
	if (mrqmin(x,y,sig,npts,a,na,lista,mfit,covar,alpha,&chisq,
		fitsum,&alamda) == 0)
		goto cleanup;
	
	Vm0=Echarge;
	for(i=4;i<na;i+=2)
		Vm0 += a[i];


/*
	free_matrix(alpha,1,na,1,1);
	free_matrix(covar,1,na,1,1);
*/

/* INVERTING THE TAUS */
	for (i=3;i<=na;i+=2)
		if (a[i] > 0.000001)
			a[i]=1.0/a[i];

/* FLIPPING Vm SIGNS */
	for (i=4;i<=na;i+=2)
		a[i]= -a[i];

/* DUMPING OUTPUT */
    if (notes)
        printf("\n%s\n",notes);
    for (i = 0 ; i < nvars ; i++) {
        printf("%s  %g  ",varnames[i],*(allvars[i]));
        if ((i%3) == 2)
            printf("\n");
    }
    printf("\n");

	if (outputfile && strlen(outputfile) > 0) {
		spikeparmoutput(allvars,NULL,varnames,nvars,outputfile,filemode,notes);
	}
	if (na > 4)
		sprintf(ret,"%g %g %g %g %g %g %g",a[1],a[2],chisq,Vm0,
			a[3],a[4],a[5]);
	else
		sprintf(ret,"%g %g %g %g %g",a[1],a[2],chisq,Vm0,a[3]);
	
/* CLEANING UP */
cleanup:
    for (i = 0 ; i <= na ; i++)
        free(alpha[i]);
    free(alpha);
    for (i = 0 ; i <= na ; i++)
        free(covar[i]);
    free(covar);

	if (ret[0] != '\0')
		return(ret);

	return NULL;
}


int read_plot(fname,plotno,x,y,maxpts,actualpts,timestep)
	char	*fname;
	int	plotno;
	float	*x,*y;
	int	maxpts;
	int	*actualpts;
	float	timestep;
{
char	label[100];
int	npts = 0;
int	cellnum;
FILE	*fp;
int 	header_size;
int	xmax, ymax;
int	datatype;
float	dt;
float	start_time;
float	time;
float	val = 0.0;
double 	dval;
int 	ival;
float 	fval;
float	gain;
float	*data;
short	skip;
int	count;
int	datasize = 0;
short	new_format = 1;
short 	dnum = 0;

    gain = 1;
    skip = 0;
    fp = NULL;

	if((fp = fopen (fname,"r")) == NULL) {
		fprintf(stderr,"\nfile not found : %s\n",fname);
		return(0);
	    }

    if(new_format){
	fread(label,sizeof(char),80,fp);
	fread(&start_time,sizeof(float),1,fp);
	fread(&dt,sizeof(float),1,fp);
	fread(&cellnum,sizeof(int),1,fp);
	fread(&datatype,sizeof(int),1,fp);
	header_size = 2*sizeof(int) + 2*sizeof (float) + 80 +
	3*cellnum*sizeof(float);
    } else {
	fread(&xmax,sizeof(int),1,fp);
	fread(&ymax,sizeof(int),1,fp);
	cellnum= (xmax+1)*(ymax+1);
	fread(&dt,sizeof(float),1,fp);
	fread(&datatype,sizeof(int),1,fp);
	header_size = 3*sizeof(int) + sizeof (float);
    }
    if(cellnum <= 0) {
	printf("file is empty\n");
	fclose(fp);
	return(0);
    }
    switch(datatype){
	case INT :
	datasize = sizeof(int);
	break;
	case SHORT :
	datasize = sizeof(short);
	break;
	case FLOAT :
	datasize = sizeof(float);
	break;
	case DOUBLE :
	datasize = sizeof(double);
	break;
    }

    skip = timestep/dt - 0.5;
    data = (float *)malloc(cellnum*datasize);
    count=0;
    fseek (fp, (long) ((plotno * datasize) + header_size),0); 
    time = start_time;
    while (!feof(fp)){
	switch(datatype){
	case FLOAT :
	    dnum = fread (&fval, datasize,1,fp);
	    val = fval;
	    break;
	case DOUBLE :
	    dnum = fread (&dval, datasize,1,fp);
	    val = dval;
	    break;
	case INT :
	    dnum = fread (&ival, datasize,1,fp);
	    val = ival;
	    break;
	}
	if(dnum <=0) break;
	if (feof(fp)) break;
	/*
	** add the points to the array
	*/
	if((skip <= 0) || (count%skip == 0)) {
		x[npts] = time;
		y[npts] = val;
		npts++;
		if (npts >= maxpts)
			break;
	}
	time += dt;
	count++;
	fseek (fp, (long)( (cellnum-1)*datasize), 1);
    }
    fclose(fp);
    *actualpts = npts;

	return(npts);
}
#undef MAXFITPTS

