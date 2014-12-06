static char rcsid[] = "$Id: out_view.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: out_view.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:03:28  dhb
** Initial revision
**
*/

#include "out_ext.h"

#ifndef MAX_NELEMENTS
#define MAX_NELEMENTS 100000
#endif

typedef struct {
    float x,y,z;
} XYZCoord;

int MattsGetCoordRoutine(file,coord,ncoords)
char 		*file;
XYZCoord	**coord;
int		*ncoords;
{
char	label[100];
FILE	*fp;
float	start_time;
float	dt;
int	datatype;
int	nelements;

    if(file == NULL) 
	return(0);
    if((fp = fopen (file,"r")) == NULL){
	Error();
	printf("file not found : %s\n",file);
	return(0);
    }
    /*
    ** read in the header
    */
    fread(label,sizeof(char),80,fp);
    /*
    ** check the label
    */
    if(strncmp("FMT1",label,4) != 0){
	Error();
	printf("file '%s' is not a valid FMT1 data file\n",file);
	fclose(fp);
	return(0);
    }
    fread(&start_time,sizeof(float),1,fp);
    fread(&dt,sizeof(float),1,fp);
    fread(&nelements,sizeof(int),1,fp);
	if (nelements > MAX_NELEMENTS) {
		printf("%d elements specified! File may be from different architechture.\n",nelements);
		printf("File not read\n");
		fclose(fp);
		return(0);
	}
    if(fread(&datatype,sizeof(int),1,fp) == 0){
	fclose(fp);
	return(0);
    }
    /*
    ** get the coords
    */
    *coord = (XYZCoord *)malloc(nelements*sizeof(XYZCoord)); 
    if(fseek (fp, (long) 2*sizeof(int) + 2*sizeof (float) + 80,0) == -1) {
	fclose(fp);
	return(0);
    }
    if(fread(*coord,nelements*sizeof(XYZCoord),1,fp) == 0){
	fclose(fp);
	return(0);
    }
    *ncoords = nelements;
    fclose(fp);
    return(1);
} 

int MattsGetValuesRoutine(file,time,data)
char 	*file;
float	time;
float	*data;
{
char	label[100];
FILE	*fp;
float	start_time;
float	dt;
int	datatype;
int	datasize = 0;
int	header_size;
int	nelements;
char	*tmp;
int	i;


    if(file == NULL) 
	return(0);
    if((fp = fopen (file,"r")) == NULL){
	Error();
	printf("file not found : %s\n",file);
	return(0);
    }
    /*
    ** read in the header
    */
    fread(label,sizeof(char),80,fp);
    /*
    ** check the label
    */
    if(strncmp("FMT1",label,4) != 0){
	Error();
	printf("file '%s' is not a valid FMT1 data file\n",file);
	fclose(fp);
	return(0);
    }
    fread(&start_time,sizeof(float),1,fp);
    fread(&dt,sizeof(float),1,fp);
    fread(&nelements,sizeof(int),1,fp);
    if(fread(&datatype,sizeof(int),1,fp) == 0){
	fclose(fp);
	return(0);
    }

    switch(datatype){
	case SHORT :
	datasize = sizeof(short);
	break;
	case INT :
	datasize = sizeof(int);
	break;
	case FLOAT :
	datasize = sizeof(float);
	break;
	case DOUBLE :
	datasize = sizeof(double);
	break;
    }
    /*
    ** get the header size
    */
    header_size = 2*sizeof(int) + 2*sizeof (float) + 80 +
    nelements*sizeof(XYZCoord);
    /*
    ** move to the data section
    */
    if(fseek (fp, (long) ((int)(time / dt) * 
    nelements*datasize + header_size),0) == -1){
	fclose(fp);
	return(0);
    }
    /*
    ** allocate the actual data array
    */
    tmp = (char *)malloc(nelements*datasize);
    /*
    ** read in the data
    */
    if(fread(tmp,nelements*datasize,1,fp) == 0){
	free(tmp);
	fclose(fp);
	return(0);
    }

    /*
    ** reformat the data into float form
    */
    for (i = 0; i < nelements; i++) {
	switch(datatype){
	case FLOAT :
	    data[i] = ((float *)(tmp))[i];
	    break;
	case DOUBLE :
	    data[i] = ((double *)(tmp))[i];
	    break;
	case INT :
	    data[i] = ((int *)(tmp))[i];
	    break;
	case SHORT :
	    data[i] = ((short *)(tmp))[i];
	    break;
	}
    }
    free(tmp);
    fclose(fp);
    return(1);
} 
