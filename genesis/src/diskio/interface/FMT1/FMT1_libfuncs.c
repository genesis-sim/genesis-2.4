#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FMT1.h"

int FMT1_CheckHeader(fp)
 FILE *fp;
{
  char label[100];
  int     nread;

  if ((nread = fread(label,sizeof(char),80,fp)) < 80) {
	clearerr(fp);	
	return 0;
  }
  /* check the label */
  if(strncmp("FMT1",label,4) != 0)
	return(0);

  return(1);
}

int FMT1_GetHeaderSize(fp) 
 FILE *fp;
{
	
 int header_size, numdatapoints, stat;

 if (fp == NULL)
	return -1;
 /* Read the number of datapoints first */
 if (fseek(fp, (long) (2 * sizeof(float)+ 80), 0) == -1) {
	return 0;
 }
	
 if (fread(&numdatapoints,  sizeof(int), 1, fp) == 0) {
	clearerr(fp);
	return -1;
 }


 header_size =  2 * sizeof(int) + 2 * sizeof(float) + 80 +
				numdatapoints * 3 * sizeof(float); 

 return header_size;

}

int FMT1_GetFrameSize(fp)
 FILE *fp;
{
 int numdatapoints, stat;

 if (fp == NULL)
	return -1;
 if (fseek(fp, (long) (2 * sizeof(float)+ 80), 0) == -1) {
	return 0;
 }

 if (fread(&numdatapoints,  sizeof(int), 1, fp) == 0) {
	clearerr(fp);
	return 0;
 }

 return numdatapoints;
}

int FMT1_GetDataType(fp)
 FILE *fp;
{

 int datatype;
 if (fp == NULL)
	return -1;
 if (fseek(fp, (long) (2 * sizeof(float)+ sizeof(int) + 80), 0) == -1) {
	return -1;
 }
 
 if (fread(&datatype, sizeof(int), 1, fp) == 0) {
	clearerr(fp);
	return -1;
 }
	
 return datatype;
} 

int FMT1_inquire(fp, domainname, numvars, numattrs, varsize)
 FILE *fp;
 char *domainname;
 int *numvars;
 int *numattrs;
 int *varsize;
{
 int framesize, nframes;
 long filesize, header_size;

 /* 
	In FMT1, any variables or metadata that exist are a 
	result of incoming SAVE messages to the diskio element
 */

 if (fp == NULL)
	return -1; 


 if (fseek(fp, (long) (80 + 2*sizeof(float)), 0) == -1) {
	if (numvars != NULL) 
		*numvars = 0;
	if (numattrs != NULL)
		*numattrs = 0;	
	if (domainname != NULL)
		strcpy(domainname, "SimulationTime");
	if (varsize != NULL)
		*varsize = 0;
	return 1;
 }

 if (fread(&framesize, sizeof(int), 1, fp) == 0) {
	int stat;
	if (numvars != NULL) 
		*numvars = 0;
	if (numattrs != NULL)
		*numattrs = 0;	
	if (domainname != NULL)
		strcpy(domainname, "SimulationTime");
	if (varsize != NULL)
		*varsize = 0;	
	clearerr(fp);
	return -1;
 }

 if (numvars != NULL)
	*numvars = framesize;
 if (numattrs != NULL)
	*numattrs = framesize;

 /* TO calculate the size of each variable which is equivalent to
    the number of frames, we first seek to eof and calculate based on
    the header size 
 */
 fseek(fp, (long) 0, 2);

 filesize = ftell(fp);
 header_size = (long) FMT1_GetHeaderSize(fp); 

 nframes = (int) ((filesize - header_size)/(long)(framesize * sizeof(float)));
 if (varsize != NULL)
	*varsize = nframes;

 if (domainname != NULL)
	strcpy(domainname, "SimulationTime");
	
 return 1;
}

int FMT1_attinq(fp, attrname, ftype, size)
 FILE *fp;
 char *attrname;
 fmt1_type *ftype;
 int *size;
{
if ( strstr(attrname, "auto3-DPosSAVE_diskio") == NULL )
       	return -1;
	
 if(ftype != NULL) {
	/* Requires mapping from datatype on file to fmt1_type */
	*ftype = FMT1_FLOAT;
 }

 if(size != NULL) {
	/* Since the 3-D Position coords are the only possible attribs
	we return that number
	*/
	*size = 3;
 }
 return 1;
}

int FMT1_attget(fp, attrname, ftype, values)
 FILE *fp;
 char *attrname;
 fmt1_type ftype;
 void *values;
{
	char index_part[10];
  	char *ptr;
	long header_size;
	int attnum = 0;
	

	if(fp == NULL || attrname == NULL ||
		values == NULL)
		return -1;
	
	ptr = strstr(attrname,  "-_");	
	if (ptr != NULL) {
		ptr += 2; /* Go past the "-_" */
		if (ptr != NULL) {
			strcpy(index_part, ptr);	
			attnum = atoi(index_part);
		}
	}

	
	header_size = (long) (2 * sizeof(float) + 2 * sizeof(int) + 80);
	/* We need to do a switch on ftype here */
	if (fseek(fp, (long) (header_size + attnum * 3 * sizeof(float)), 0) == -1) 
		return -1;
	if (fread(values, sizeof(float), 3, fp) == 0) {
		clearerr(fp);	
		return -1;
	}
	else
		return 1;
}

int FMT1_attput(fp, ftype, size, values)
 FILE **fp;
 fmt1_type ftype;
 int size;
 void *values;
{
	int stat;

	if(fp == NULL ||
		values == NULL)
		return -1;


 switch(ftype){
	
	case FMT1_INT:
		stat = fwrite(values, sizeof(int), size, *fp);
		if (stat == size)
			return size*sizeof(int);	
		else 
			clearerr(*fp);
		break;
	case FMT1_CHAR:
		stat =  fwrite(values, sizeof(char), size, *fp);
		if (stat == size)
			return size*sizeof(char);	
		else 
			clearerr(*fp);
		break;
	case FMT1_SHORT:
		stat =  fwrite(values, sizeof(short), size, *fp);
		if (stat == size)
			return size*sizeof(short);	
		else 
			clearerr(*fp);
		break;
	case FMT1_LONG:
		stat =  fwrite(values, sizeof(long), size, *fp);
		if (stat == size)
			return size*sizeof(long);	
		else 
			clearerr(*fp);
		break;
	case FMT1_FLOAT:
		stat =  fwrite(values, sizeof(float), size, *fp);
		if (stat == size)
			return size*sizeof(float);	
		else 
			clearerr(*fp);
		break;
	case FMT1_DOUBLE:
		stat =  fwrite(values, sizeof(double), size, *fp);
		if (stat == size)
			return size*sizeof(double);	
		else 
			clearerr(*fp);
		break;
 }
	
	return stat;
}

 

/* Returns the attrname given the attrnum */
int FMT1_attname(fp, attrnum, attrname)
 FILE *fp;
 int attrnum;
 char *attrname;
{
/* This function is defined as a FMT1 utility function in FMT1_utilfuncs.c */
    
 extern void FMT1_FillIndexedAttrName(/* char *attrname, int attrnum */);

 if (fp == NULL || attrname == NULL || attrnum < 0)
	return -1;  


 FMT1_FillIndexedAttrName(attrname, attrnum); 
 return 1;
}

int FMT1_varinq(fp, varname, ftype, ndims, nmetadata)
 FILE *fp;
 char *varname;
 fmt1_type *ftype;
 int *ndims;
 int *nmetadata;
{
if (strstr(varname, "autoSAVE_diskio") == NULL )
       	return -1;
	
 if(ftype != NULL) {
	/* Requires mapping from datatype on file to fmt1_type */
	*ftype = FMT1_FLOAT;
 }

 if(ndims  != NULL) {
	*ndims = 1;
 }
 if(nmetadata != NULL) {
	*nmetadata = 0;
 }
 return 1;
}

int FMT1_varget(fp, varname,  ftype, start, size, values)
 FILE *fp;
 char *varname;
 fmt1_type ftype;
 int start;
 int size;
 void *values;
{
	char index_part[10];
  	char *ptr;
	long header_size;
	int varnum = 0;
	

	if(fp == NULL || varname == NULL ||
		values == NULL)
		return -1;
	
	ptr = strstr(varname,  "-_");	
	if (ptr != NULL) {
		ptr += 2; /* Go past the "-_" */
		if (ptr != NULL) {
			strcpy(index_part, ptr);	
			varnum = atoi(index_part);
		}
	}

	
	header_size = (long) FMT1_GetHeaderSize(fp);
	/* We need to do a switch on ftype here */
	if (fseek(fp, (long) (header_size + varnum * size * sizeof(float)), 0) == -1) 
		return -1;
	if (fread(values, sizeof(float), size, fp) == 0) {
		clearerr(fp);
		return -1;
	}
	else
		return 1;
}

int FMT1_varput(fp, ftype, size, values)
 FILE **fp;
 fmt1_type ftype;
 int size;
 void *values;
{
  int stat;

  if(fp == NULL ||
	values == NULL)
	return -1;

 switch(ftype){
	
	case FMT1_INT:
		stat = fwrite(values, sizeof(int), size, *fp);
		if (stat == size)
			return size*sizeof(int);	
		break;
	case FMT1_CHAR:
		stat = fwrite(values, sizeof(char), size, *fp);
		if (stat == size)
			return size*sizeof(char);	
		break;
	case FMT1_SHORT:
		stat = fwrite(values, sizeof(short), size, *fp);
		if (stat == size)
			return size*sizeof(short);	
		break;
	case FMT1_LONG:
		stat = fwrite(values, sizeof(long), size, *fp);
		if (stat == size)
			return size*sizeof(long);	
		break;
	case FMT1_FLOAT:
		stat = fwrite(values, sizeof(float), size, *fp);
		if (stat == size)
			return size*sizeof(float);	
		break;
	case FMT1_DOUBLE:
		stat = fwrite(values, sizeof(double), size, *fp);
		if (stat == size)
			return size*sizeof(double);	
		break;
 }
	
	return stat;
}
/* Returns the varname given the varnum */
int FMT1_varname(fp, varnum, varname)
 FILE *fp;
 int varnum;
 char *varname;
{
 extern void FMT1_FillIndexedVarName(/* char *varname, int varnum */);


 if (fp == NULL || varname == NULL || varnum < 0)
	return -1;  


 FMT1_FillIndexedVarName(varname, varnum); 
 return 1;
}


int FMT1_ReadHeader(fp)
 FILE *fp;
{
 char label[100];

 fseek(fp, 0L, 0);
 if (fread(label, sizeof(char), 80, fp) != 80) {
	clearerr(fp);
	return 0;
 }

 if (strncmp("FMT1", label, 4) != 0)  
	return 0;

 return 1;
}
