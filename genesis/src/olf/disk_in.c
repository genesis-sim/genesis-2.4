static char rcsid[] = "$Id: disk_in.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: disk_in.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2000/06/12 05:07:03  mhucka
** Removed nested comments; added NOTREACHED comments where appropriate.
**
** Revision 1.5  1997/06/12 23:15:02  dhb
** Fixed CheckFMT1Header() to rewind the file if the file is not
** FMT1.  This was resulting in skipping the first 80 bytes of
** ascii files when leave_open was set.
**
** Revision 1.4  1997/05/29 07:57:50  dhb
** Update from Antwerp GENESIS 21e
**
 * EDS20j revison: EDS BBF-UIA 95/08/03
 * Updated ShowInterpol function call
 *
 * Revision 1.3  1995/05/23  20:34:06  dhb
 * Fixed erroneous free() call in ReadFMT1Header() which was passing
 * a structure value rather than the pointer.
 *
 * Revision 1.2  1995/05/13  01:35:12  dhb
 * Changes to support xfileview like functionality using disk_in
 * and xview elements.
 *
 * Revision 1.1  1992/11/14  00:37:19  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "olf_ext.h"
/* By Upi Bhalla Caltech */
/* Bugfixes March 1992 Upi Bhalla Caltech */

#define ASCII 0 
#define FMT1 1
int	DEBUG_DiskIn = 0;
int CheckFMT1Header();

typedef struct {
	float x,y,z;
} XYZCoord;

/*
** DiskIn : reads in data from an ascii file on disk to a 2d array
** of variable size. Array is allocated on reset.
*/
/* Jan 90 Upinder S. Bhalla */
int DiskIn(disk_in,action)
register struct disk_in_type *disk_in;
Action 	*action;
{
	register float	**val;
	char	cbuf[201];
	char	*temp = NULL;
	register int	k,nnum;
	float	x[16];
	int		i,j;
	int		read;
	char	*field;
	int		setval;
	FILE	*fp,*fopen();

    if(Debug(DEBUG_DiskIn) > 1){
		ActionHeader("DiskIn",disk_in,action);
    }
    SELECT_ACTION(action){
    case INIT:
		break;
    case PROCESS:
		if (!disk_in->allocated)
			return 0;
		val = disk_in->val;
		if (disk_in->is_open) {
			fp = disk_in->fp;
		} else {
			if (!(fp = fopen(disk_in->filename,"r"))) {
				printf("cannot open filename %s\n",disk_in->filename);
				return 0;
			}
			disk_in->fp = fp;
			disk_in->is_open = 1;
		}
		switch (disk_in->fileformat) {
		case ASCII:
			if (!disk_in->leave_open)
				fseek(fp,disk_in->lastpos,0);
			i = j = 0;
			read = 0;
			/* This outer loop is to make sure that the whole val
			** array gets filled even if the file ends */
			while(!read) {
				/* This inner loop continues till eof or val filled */
				while(!read && (temp = fgets(cbuf,200,fp)) != NULL) {
					/* Assuming no one will be compulsive enough to put > 16 
					** numbers per line */
					nnum = sscanf(cbuf,"%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f",
						&x[0],&x[1],&x[2],&x[3],&x[4],&x[5],&x[6],&x[7],&x[8],
						&x[9],&x[10],&x[11],&x[12],&x[13],&x[14],&x[15]);
					for (k = 0 ; k < nnum ; k++) {
						val[i][j] = x[k];
						i++;
						if (i >= disk_in->nx) {
							i = 0 ; j++;
							if (j >= disk_in->ny) {
								read = 1;
								break;
							}
						}
					}
				}
				if (disk_in->loop && temp == NULL && !read) {
					fseek(fp,0,0);
				} else {
					read = 1;
				}
			}
			break;
		case FMT1 :
			UpisGetValuesRoutine(fp,disk_in);
			break;
		default :
			break;
		}
		if (!(disk_in->leave_open)) {
			disk_in->is_open = 0;
			disk_in->lastpos=ftell(fp);
			fclose(fp);
		}

		break;
    case RESET:
		if (disk_in->is_open) {
			fclose(disk_in->fp);
		}
		if (!(fp = fopen(disk_in->filename,"r"))) {
			printf("Error ... cannot open filename %s\n",
				disk_in->filename);
			return 0;
		}

		/* Now it checks the file format automatically */
		disk_in->fileformat = CheckFMT1Header(fp,disk_in);
		if (!disk_in->allocated) {
			switch (disk_in->fileformat) {
			case ASCII:
				if (disk_in->nx <= 0 || disk_in->ny <= 0)
					return 0;
				break;
			case FMT1:
				if (ReadFMT1Header(fp,disk_in) == 0)
					return 0;
				break;
			default :
				return 0;
			}
			val = disk_in->val =
				(float **) calloc(disk_in->nx,sizeof(float *));
			for (i = 0 ; i < disk_in->nx ; i++)
				val[i] = (float *)calloc(disk_in->ny,sizeof(float));
			disk_in->allocated = 1;
		}
		if (disk_in->leave_open) {
			disk_in->fp = fp;
			disk_in->is_open = 1;
		} else {
			fclose(fp);
		}
		break;

	case SET :
		if (action->argc != 2)
			return(0); /* do the normal set */
		if (SetTable(disk_in,2,action->argv,action->data,
			"xpts ypts zpts"))
			return(1);
		field = action->argv[0];
		if (disk_in->fileformat == FMT1 && disk_in->allocated && 
			strcmp(field,"filename") == 0) {
		/* cleaning out the old file info */
			free(disk_in->tempdata);
			free(disk_in->val[0]);
			free(disk_in->val);
			disk_in->allocated = 0;
			return(0);
		}
		if (strcmp(field,"nx") != 0 && strcmp(field,"ny") != 0)
			return(0); /* do the normal set */
		if (disk_in->fileformat == FMT1) {
			printf("The size of the array is determined by the FMT1 data file at reset.\n");
			return(0);
		}
		setval = atoi(action->argv[1]);

		if (setval < 0) {
			printf("Cannot assign a negative size to array");
			return(1);
		}
		/* Checking if change is necessary */
		if (strcmp(field,"nx") == 0)
			if (setval == disk_in->nx)
				return(1);
		if (strcmp(field,"ny") == 0)
			if (setval == disk_in->ny)
				return(1);

		/* cleaning up the existing array */
		if (disk_in->allocated) {
			val = disk_in->val;
			for (i = 0 ; i < disk_in->nx ; i++)
				free(val[i]);
			free(val);
			disk_in->allocated = 0;
		}

		/* Assigning new array */
		if (strcmp(field,"nx") == 0)
			disk_in->nx = setval;
		if (strcmp(field,"ny") == 0)
			disk_in->ny = setval;
		if (!disk_in->allocated) {
			if (disk_in->nx <= 0 || disk_in->ny <= 0)
				return(1);
			val = disk_in->val =
				(float **) calloc(disk_in->nx,sizeof(float *));
			for (i = 0 ; i < disk_in->nx ; i++)
				val[i] = (float *) calloc(disk_in->ny,sizeof(float));
			disk_in->allocated = 1;
		}
		return(1);
    case RECALC:
		break;
	case SHOW:
		if (ShowInterpol(action,"xpts",disk_in->xpts,disk_in->allocated,50)) 
			return(1);
		if (ShowInterpol(action,"ypts",disk_in->ypts,disk_in->allocated,50)) 
			return(1);
		return(ShowInterpol(action,"zpts",disk_in->zpts,disk_in->allocated,50));
    case CHECK:
		break;
	}
	return 0;
}

Interpol *realloc_interpol(ip,n)
	Interpol *ip;
	int n;
{
	Interpol	*create_interpol();
	if (ip &&
		ip->allocated && ip->table && ip->xdivs >= n)
		return ip;

	if (ip) {
		if (ip->table && ip->allocated)
			free(ip->table);
		free(ip);
	}
	return(create_interpol(n,(float)0.0,(float)n));
}

int CheckFMT1Header(fp,disk_in)
	FILE	*fp;
	struct	disk_in_type	*disk_in;
{
	char	label[100];
	int	nread;

    nread = fread(label,sizeof(char),80,fp);
    /* check the label */
    if(nread < 80 || strncmp("FMT1",label,4) != 0){
		rewind(fp);
		return(0);
    }
	return(1);
}

int ReadFMT1Header(fp,disk_in)
	FILE	*fp;
	struct	disk_in_type	*disk_in;
{
	char	label[100];
	int	header_size;
	int		datasize = 0;
	Interpol	*create_interpol();
	XYZCoord	*coord;
	int		i;

	rewind(fp);
    fread(label,sizeof(char),80,fp);
    /*
    ** check the label
    */
    if(strncmp("FMT1",label,4) != 0){
	Error();
	fclose(fp);
	return(0);
    }
    fread(&(disk_in->start_time),sizeof(float),1,fp);
    fread(&(disk_in->dt),sizeof(float),1,fp);
    fread(&(disk_in->ny),sizeof(int),1,fp);
	disk_in->nx = 1;
    if(fread(&(disk_in->datatype),sizeof(int),1,fp) == 0){
		disk_in->datatype = -1;
		fclose(fp);
		return(0);
    }
    /*
    ** get the header size
    */
    header_size = 2*sizeof(int) + 2*sizeof (float) + 80 +
    disk_in->ny*sizeof(XYZCoord);

	disk_in->header_size = header_size;
    /*
    ** allocate the actual data array
    */

    switch(disk_in->datatype) {
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
	case -1 :
		return(0);
		/* NOTREACHED */
		break;
	default :
		break;
    }
    disk_in->tempdata = (char *)malloc(disk_in->ny * datasize);

	/* allocate the interpols, if necessary */
	disk_in->xpts = realloc_interpol(disk_in->xpts, disk_in->ny);
	disk_in->ypts = realloc_interpol(disk_in->ypts, disk_in->ny);
	disk_in->zpts = realloc_interpol(disk_in->zpts, disk_in->ny);

	/* Fill up the interpols with the coords */
	coord = (XYZCoord *)malloc(disk_in->ny*sizeof(XYZCoord));
	if(fseek (fp, (long) 2*sizeof(int) + 2*sizeof(float) + 80,0) == -1){
		fclose(fp);
		return(0);
    }
	if(fread(coord,disk_in->ny*sizeof(XYZCoord),1,fp) == 0){
		fclose(fp);
		return(0);
	}
	for (i = 0; i < disk_in->ny; i++) {
		disk_in->xpts->table[i] = coord[i].x;
		disk_in->ypts->table[i] = coord[i].y;
		disk_in->zpts->table[i] = coord[i].z;
	}

	free(coord);
	return(1);
}

int UpisGetValuesRoutine(fp,disk_in)
FILE 	*fp;
	struct	disk_in_type	*disk_in;
{
	float	*data;
	char	*tmp;
	int		datasize = 0;
	int	 i;

    switch(disk_in->datatype) {
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
	case -1 :
		return(0);
	default :
		break;
    }

    if(fseek(fp,(long)((int)((simulation_time - disk_in->time_offset) /
		disk_in->dt) * disk_in->ny*datasize + disk_in->header_size),0) == -1){
		return(0);
    }
    /*
    ** read in the data
    */
	tmp = disk_in->tempdata;
    if(fread(tmp,disk_in->ny*datasize,1,fp) == 0){
		return(0);
    }

    /*
    ** reformat the data into float form
    */
	data = disk_in->val[0];
    for (i = 0; i < disk_in->ny; i++) {
	switch(disk_in->datatype){
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
    return(1);
}

#undef ASCII
#undef FMT1
