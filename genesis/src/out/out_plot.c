static char rcsid[] = "$Id: out_plot.c,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $";

/*
** $Log: out_plot.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1997/05/29 08:49:18  dhb
** Update from Antwerp GENESIS version 21e
**
 * EDS21a revison: EDS BBF-UIA 96/04/12
 * Corrected CallElement syntax
 *
** Revision 1.1  1992/12/11  19:03:25  dhb
** Initial revision
**
*/

#include "sim_ext.h"

do_simplot (argc, argv)
int     argc;
char   *argv[];
{
char    fname[100];
char    label[100];
char    plotname[100];
int	cellnum;
char	*graph;
short	plotnum;
FILE	*fp;
int 	header_size;
int	cell;
int	i;
int	nxtarg;
int	xmax, ymax;
int	datatype;
int	j;
float	dt;
float	start_time;
float	time;
float	val;
double 	dval;
int 	ival;
float 	fval;
float	gain;
float	*data;
short	skip;
int	count;
int	datasize;
short	new_format = 1;
short 	dnum;
short	overlay = 0;
float	offset;
static char *color = "Black";

    if (argc < 4) {
	printf ("usage: %s graph [-c cell][-g gain][-old][-overlay #][-offset #]file\n",
	argv[0]);
	return(0);
    }
    graph = argv[1];
    gain = 1;
    skip = 0;
    fp = NULL;
    offset = 0;
    plotnum = 0;

    nxtarg = 1;
    while( argc > ++nxtarg){
	if (strcmp (argv[nxtarg], "-c") == 0){
	    cell = atoi(argv[++nxtarg]);
	} else
	if (strcmp (argv[nxtarg], "-g") == 0){
	    gain = Atof(argv[++nxtarg]);
	} else
	if (strcmp (argv[nxtarg], "-skip") == 0){
	    skip = atoi(argv[++nxtarg]);
	} else
	if (strcmp (argv[nxtarg], "-color") == 0){
	    color = argv[++nxtarg];
	} else
	if (strcmp (argv[nxtarg], "-overlay") == 0){
	    plotnum = atoi(argv[++nxtarg]);
	} else
	if (strcmp (argv[nxtarg], "-offset") == 0){
	    offset = Atof(argv[++nxtarg]);
	} else
	if (strcmp (argv[nxtarg], "-old") == 0){
	    new_format = 0;
	} else
	if(argv[nxtarg][0] != '-'){
	    if((fp = fopen (argv[nxtarg],"r")) != NULL){
		strcpy(fname,argv[nxtarg]);
	    } else {
		fprintf(stderr,"\nfile not found : %s\n",argv[nxtarg]);
		return(0);
	    }
	} else{
	    printf ("usage: %s [-c cell][-color color][-g gain][-old][-overlay #] file\n",
	    argv[0]);
	    printf("unrecognized option\n");
	}
    }
    if(fp == NULL){
	printf("a data file must be specified\n");
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
    data = (float *)malloc(cellnum*datasize);
    count=0;
    fseek (fp, (long) ((cell * datasize) + header_size),0); 
    time = start_time;
    if(!overlay){
	/*
	** clear the graph
	*/
	CallElement(GetElement(graph),GetAction("RESET"),0);
    }
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
	val += plotnum*offset;
	if(dnum <=0) break;
	if (feof(fp)) break;
	/*
	** add the points to the graph
	*/
	if((skip==0) || (count%skip == 0)) {
	    sprintf(plotname,"%d %s",plotnum,fname);
	    axis_graph_add_pts(graph,plotname,color,time,val);
	}
	time += dt;
	count++;
	fseek (fp, (long)( (cellnum-1)*datasize), 1);
    }
    fclose(fp);
}
