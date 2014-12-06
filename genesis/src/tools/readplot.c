static char rcsid[] = "$Id: readplot.c,v 1.2 2005/07/01 10:03:10 svitak Exp $";

/*
** $Log: readplot.c,v $
** Revision 1.2  2005/07/01 10:03:10  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:06:06  dhb
** Initial revision
**
*/

#include "sim_ext.h"

int read_plot2(fname,plotno,timestep,y,maxpts,actualpts,startt,endt)
        char    *fname;
        int     plotno;
        float   *timestep;
        float   *y;
        int     maxpts;
        int     *actualpts;
        float   startt,endt;
{
char    label[100];
int     npts = 0;
int     cellnum;
FILE    *fp;
int      header_size;
int     xmax, ymax;
int     datatype;
float   dt;
float   start_time;
float   time;
float   val = 0.0;
double  dval;
int     ival;
float   fval;
float   gain;
int     count;
int     datasize = 0;
short   new_format = 1;
short   dnum = 0;

    gain = 1;
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
        if (time > endt) break;
        /*
        ** add the points to the array
        */
        if(time >= startt) {
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
	*timestep = dt;
    *actualpts = npts;

        return(npts);
}
