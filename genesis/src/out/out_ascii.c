static char rcsid[] = "$Id: out_ascii.c,v 1.3 2005/10/16 20:48:35 svitak Exp $";

/*
** $Log: out_ascii.c,v $
** Revision 1.3  2005/10/16 20:48:35  svitak
** Changed hard-coded character arrays from 100 to 256 or malloc'd and freed
** memory instead. This was only done where directory paths might have easily
** exceeded 100 characters.
**
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  1997/07/18 03:06:15  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/26 18:03:19  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:03:20  dhb
** Initial revision
**
*/

#include "sim_ext.h"

#define NOSELECTION	0
#define CELL		1
#define TIME		2
#define HEADER		3

void do_ascii_convert(argc, argv)
int     argc;
char   *argv[];
{
char    label[100];
char    fname[256];
int	cellnum;
FILE	*fp;
int 	header_size;
int	cell = 0;
int	i;
int	xmax, ymax;
int	datatype;
float	dt;
float	start_time = 0;
float	time;
float	val = 0.0;
double 	dval;
int 	ival;
short 	sval;
float 	fval;
float	gain;
char	*data;
short	skip;
int	count;
int	datasize;
short	new_format = 1;
short 	dnum = 0;
short	xy;
int	mode;
float	selecttime = 0.0;
int	status;

    gain = 1;
    skip = 0;
    fp = NULL;
    xy = 0;
    mode = NOSELECTION;

    initopt(argc, argv, "file-name -time t -cell cell -gain g -old -xy -skip n -header");
    while((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp (G_optopt, "-cell") == 0){
	    cell = atoi(optargv[1]);
	    mode = CELL;
	} else if (strcmp (G_optopt, "-time") == 0){
	    selecttime = Atof(optargv[1]);
	    mode = TIME;
	} else if (strcmp (G_optopt, "-gain") == 0){
	    gain = Atof(optargv[1]);
	} else if (strcmp (G_optopt, "-skip") == 0){
	    skip = atoi(optargv[1]);
	} else if (strcmp (G_optopt, "-xy") == 0){
	    xy = 1;
	} else if (strcmp (G_optopt, "-header") == 0){
	    mode = HEADER;
	} else if (strcmp (G_optopt, "-old") == 0){
	    new_format = 0;
	}
      }

    if (status < 0 || mode == NOSELECTION) {
	printoptusage(argc, argv);
	printf("\n");
	printf("One of options -cell, -time or -header must be given.\n");
	return;
    }

    if((fp = fopen (optargv[1],"r")) != NULL){
	strcpy(fname,optargv[1]);
    } else {
	Error();
	fprintf(stderr,"file not found : %s\n",optargv[1]);
	return;
    }

    if(new_format){
	fread(label,sizeof(char),80,fp);
	fread(&start_time,sizeof(float),1,fp);
	fread(&dt,sizeof(float),1,fp);
	fread(&cellnum,sizeof(int),1,fp);
	fread(&datatype,sizeof(int),1,fp);
	header_size = 2*sizeof(int) + 2*sizeof (float) + 80*sizeof(char) +
	3*cellnum*sizeof(float);
    } else {
	fread(&xmax,sizeof(int),1,fp);
	fread(&ymax,sizeof(int),1,fp);
	cellnum= (xmax+1)*(ymax+1);
	fread(&dt,sizeof(float),1,fp);
	fread(&datatype,sizeof(int),1,fp);
	header_size = 3*sizeof(int) + sizeof (float);
    }
    datasize = DataSize(datatype);
    if(mode == HEADER){
	if(new_format){
	    printf("%s\n",label);
	    printf("%-20s = %e\n","start time",start_time);
	    printf("%-20s = %e\n","time step",dt);
	    printf("%-20s = %d\n","cellnum",cellnum);
	    printf("%-20s = %d\n","data type",datatype);
	    printf("%-20s = %d\n","data size",datasize);
	    printf("\n");
	} else {
	    printf("%-20s = %d\n","xmax",xmax);
	    printf("%-20s = %d\n","ymax",ymax);
	    printf("%-20s = %e\n","time step",dt);
	    printf("%-20s = %d\n","data type",datatype);
	    printf("%-20s = %d\n","data size",datasize);
	    printf("\n");
	}
	return;
    }
    if(cellnum <= 0) {
	fprintf(stderr,"file is empty\n");
	fclose(fp);
	return;
    }
    if(mode == CELL){
	count=0;
	fseek (fp, (long) ((cell * datasize) + header_size),0); 
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
	    case SHORT :
		dnum = fread (&sval, datasize,1,fp);
		val = sval;
		break;
	    }
	    if(dnum <=0) break;
	    if (feof(fp)) break;
	    /*
	    ** print the value
	    */
	    if((skip==0) || (count%skip == 0)) {
		if(xy){
		    printf("%e\t%e\n",time,val);
		} else {
		    printf("%e\n",val);
		}
	    }
	    time += dt;
	    count++;
	    fseek (fp, (long)( (cellnum-1)*datasize), 1);
	}
    } else
    if(mode == TIME){
	data = (char *)malloc(cellnum*datasize);
	fseek (fp, (long) ((cellnum*datasize*(selecttime-start_time)/dt) + header_size),0); 
	dnum = fread (data, datasize,cellnum,fp);
	if(dnum != cellnum){
	    fprintf(stderr,"unable to read data at time %g. read %d of %d\n",
	    selecttime,dnum,cellnum);
	    return;
	}
	for(i=0;i<cellnum;i++){
	    if(xy){
		printf("%d\t",i);
	    }
	    switch(datatype){
	    case FLOAT :
		printf("%g\n",((float *)(data))[i]);
		break;
	    case DOUBLE :
		printf("%g\n",((double *)(data))[i]);
		break;
	    case INT :
		printf("%d\n",((int *)(data))[i]);
		break;
	    case SHORT :
		printf("%d\n",((short *)(data))[i]);
		break;
	    case CHAR :
		printf("%d\n",((char *)(data))[i]);
		break;
	    }
	}
    }
    fclose(fp);
}
