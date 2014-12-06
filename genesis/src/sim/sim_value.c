static char rcsid[] = "$Id: sim_value.c,v 1.2 2005/06/27 19:01:12 svitak Exp $";

/*
** $Log: sim_value.c,v $
** Revision 1.2  2005/06/27 19:01:12  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2001/04/25 17:17:03  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.3  1994/06/03 22:40:02  dhb
** Fixes to GetValue() for checking for NULL pointers to strings and
** guarding against buffer overruns on long strings (from Upi Bhalla).
**
 * Revision 1.2  1993/07/21  21:32:47  dhb
 * fixed rcsid variable type
 *
 * Revision 1.1  1992/10/27  20:38:35  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

static char float_format[10];
#define MAX_VALUE_STRING_LEN 400

char *GetValue(adr,info)
char *adr;
Info *info;
{
char *name;
static char	string[MAX_VALUE_STRING_LEN];
float fval;
double dval;

    string[0] = '\0';
    if(adr == NULL || info == NULL){
	return("");
    }
    if(info->function_type){
	name = GetFuncName(*((PFI *)adr));
	if(name != NULL)
	    sprintf(string,"%s",name);
	else
	    sprintf(string,"&%d",*((int *)adr));
    } else
    if(strcmp(info->type,"char") ==0){
	if(info->field_indirection == 1){
	    /* Upi fix for attempts to access null strings */
	    /* I am surprised that this has not been caught already */
	    if ((*((char **)adr)) == NULL) return("nil");
     
	    /* Yet another problem here - long strings croak */
	    if ((int)strlen(*((char**)adr)) > MAX_VALUE_STRING_LEN - 10) {
		strncpy(string,*((char**)adr),MAX_VALUE_STRING_LEN - 10);
		string[MAX_VALUE_STRING_LEN - 10] = '\0';
		strcat(string,"....");
	    } else {
		sprintf(string,"%s",*((char **)adr));
	    }
	} else
	if(info->field_indirection == 0){
	    sprintf(string,"%c",*((char *)adr));
	}
    } else 
    if(info->field_indirection >= 1){
	sprintf(string,"&%d",*((int *)adr));
    } else 
    if(strcmp(info->type,"int") ==0){
	    sprintf(string,"%d",*((int *)adr));
    } else
    if(strcmp(info->type,"float") ==0){
	fval = *((float *)(adr));
/*
	if((fabs(fval) < 1e-6 || fabs(fval) > 1e6)
	 && fval != 0.0)
	    sprintf(string,"%e",fval);
	else
	    sprintf(string,"%f",fval);
	sprintf(string,"%0.20g",fval);
*/
	sprintf(string,float_format,fval);
    } else
    if(strcmp(info->type,"double") ==0){
	dval = *((double *)(adr));
/*
	if((fabs(dval) < 1e-6 || fabs(dval) > 1e6)
	 && dval != 0.0)
	    sprintf(string,"%e",dval);
	else
	    sprintf(string,"%f",dval);
	    sprintf(string,"%0.20g",dval);
*/
	sprintf(string,float_format,dval);
    } else
    if(strcmp(info->type,"short") ==0){
	sprintf(string,"%d",*((short *)adr));
    } else
    if(strcmp(info->type,"unsigned char") ==0){
	sprintf(string,"%d",*((unsigned char *)adr));
    } else 
    if(strcmp(info->type,"unsigned short") ==0){
	sprintf(string,"%d",*((unsigned short *)adr));
    } else {
	return("");
    }
    return(string);
}

int PutValue(adr,info,value)
char *adr;
Info *info;
char *value;
{
char *name;
PFI func;

    if(adr == NULL || value == NULL) return(0);
    if(info->function_type){
	if((func = (PFI)GetFuncAddress(value))== NULL){
	    printf("could not find function '%s'\n",value);
	    return(0);
	} else {
	    *((PFI *)adr) = func;
	}
    } else
    if(strcmp(info->type,"int") ==0){
	*((int *)adr) = atoi(value);
    } else
    if(strcmp(info->type,"float") ==0){
	*((float *)adr) = Atof(value);
    } else
    if(strcmp(info->type,"double") ==0){
	*((double *)adr) = Atod(value);
    } else
    if(strcmp(info->type,"short") ==0){
	*((short *)adr) = atoi(value);
    } else
    if(strcmp(info->type,"unsigned char") ==0){
	*((unsigned char *)adr) = atoi(value);
    } else
    if(strcmp(info->type,"unsigned short") ==0){
	*((unsigned short *)adr) = (unsigned short)(atoi(value));
    } else
    if(strcmp(info->type,"char") ==0){
	if(info->field_indirection == 1){
	    name = (char *)malloc(strlen(value)+1);
	    strcpy(name,value);
	    *((char **)adr) = name;
	} else
	if(info->field_indirection == 0){
	    *((char *)adr) = atoi(value);
	}
    } else {
	printf("unable to assign to type '%s'\n",info->type);
	return(0);
    }
    return(1);
}

void sim_set_float_format(format)
	char	*format;
{
	strcpy(float_format,format);
}
