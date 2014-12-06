static char rcsid[] = "$Id: sim_address.c,v 1.2 2005/06/27 19:00:57 svitak Exp $";

/*
** $Log: sim_address.c,v $
** Revision 1.2  2005/06/27 19:00:57  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.9  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.8  2001/04/18 22:39:38  mhucka
** Miscellaneous portability fixes, mainly for SGI IRIX.
**
** Revision 1.7  2000/06/12 04:51:35  mhucka
** Added a missing type specifier and a missing type cast.
**
** Revision 1.6  2000/03/27 10:34:44  mhucka
** Commented out statusline functionality, because it caused GENESIS to
** print garbage control characters upon exit, screwing up the user's
** terminal.  Also added return type declaractions for various things.
**
** Revision 1.5  1995/07/14 01:19:53  dhb
** Added additional checks to parsing of array indexing.  This
** avoids calls to strchr() with a null pointer and undefined
** array indexes when bad or missing indexes are given or the
** closing square bracket is missing.
**
 * Revision 1.4  1995/05/17  21:32:10  dhb
 * Fixed bug in check for indexing of allocated arrays (pointer values)
 * which basically disallowed any indexing of these fields.
 *
 * Revision 1.3  1993/11/08  22:41:36  dhb
 * Changed CalculateAddress() to consider a negative array index as an error.
 *
 * Revision 1.2  1993/07/21  21:31:57  dhb
 * fixed rcsid variable type
 *
 * Revision 1.1  1992/10/27  19:28:53  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "sim_ext.h"
#include "shell_func_ext.h"

#define MAX_DIMENSIONS 4

/*
** calculates the linear offset of an array element from the 
** beginning of a multi-dimensional array
*/
int IndexOffset(i,index,dim,maxd)
int i;
int index;
int dim[MAX_DIMENSIONS];
int maxd;
{
int j;
int offset;

    offset = index;
    for(j=i+1;j<maxd;j++){
	offset *= dim[j];
    }
    return(offset);
}

/*
** calculate the offset of a field from the beginning of a structure
*/
int CalculateOffset(type,field,returninfo)
char *type;
char *field;
Info *returninfo;
{
char fullname[100];
Info info;
char *end;
char *iptr;
char tmp[100];
int offset;
int field_offset;
int index[MAX_DIMENSIONS];
int dim;
int i;

    if(type == NULL || field == NULL){
	return(-1);
    }
    strcpy(fullname,type);
    strcat(fullname,".");
    /*
    ** check for any type of indirection in the field
    ** if so then an offset calculation cannot be made and the
    ** return value of -2 indicates that CalculateAddress should
    ** be used instead
    */
    if(strchr(field,'*') != NULL) return(-2);
    if(strchr(field,'-') != NULL) return(-2);
    /*
    ** any array indices?
    */
    dim  = 0;
    iptr = field;
    if ((end = strchr(iptr,'['))) {
	/*
	** get the name
	*/
	strncpy(tmp,field,(WORD)end-(WORD)field);
	tmp[end-field] = '\0';
	strcat(fullname,tmp);
	/*
	** extract the index components
	*/
	while ((end = strchr(iptr,'['))) {
	    sscanf(end+1,"%d",index+dim);
	    iptr = strchr(end,']');
	    dim++;
	}
    } else
    /*
    ** are there multiple fields
    */
    if ((end = strchr(field,'.'))) {
	/*
	** if there are more  fields then just do the first
	*/
	strncpy(tmp,field,(WORD)end-(WORD)field);
	tmp[end-field] = '\0';
	strcat(fullname,tmp);
    } else {
	strcat(fullname,field);
    }
    /*
    ** get the info on the field
    */
    if(GetInfo(fullname,&info)){
	/*
	** get its offset
	*/
	/*
	** if there is array indexing then  calculate the array offset
	** as well
	*/
	if(dim != info.dimensions){
	    if(info.field_indirection != dim){
		return(-1);
	    } else {
		return(-2);
	    }
	}
	/*
	** this bit of code calculates the offset of an array indexing 
	** sequence given in index
	*/
	if(dim == 0){
	    field_offset = info.offset;
	} else{
	    field_offset = info.offset;
	    for(i=0;i<dim;i++){
		if(info.field_indirection == 0){
		    field_offset += 
		    IndexOffset(i,index[i],info.dimension_size,dim)*
		    info.type_size;
		} else {
		    field_offset += 
		    IndexOffset(i,index[i],info.dimension_size,dim)*
		    sizeof(char *);
		}
	    }
	}
	/*
	** check for more fields
	*/
	if ((end = strchr(field,'.'))) {
	    /*
	    ** if there are more then calculate their offsets
	    ** using the newly aquired type info and the next field
	    */
	    if((offset = CalculateOffset(info.type,strchr(field,'.')+1,&info))
	    == -1) return(-1);
	    if(offset == -2) return(-2);
	} else {
	    offset = 0;
	}
	/*
	** return the offset info
	*/
	BCOPY(&info,returninfo,sizeof(Info));
	return(offset + field_offset);
    } else {
	return(-1);
    }
}

/*
** return the size of a structure
*/
int StructSize(type)
char *type;
{
Info info;
static char *ptype = NULL;
static int size;

    if(type == NULL){
	return(0);
    }
    if(type == ptype) return(size);
    if(GetInfo(type,&info)){
	size = (int) info.type_size;		
	return(size);
    } else {
	return(0);
    }
}

int ComputeMode(field)
char *field;
{
char *iptr;
int mode;

    mode = 0;	/* no operator */
    for(iptr=field;iptr && *iptr!= '\0';iptr++){
	switch(*iptr){
	case '.': mode = 1; break;	/* structure */
	case '-': mode = 2; break;	/* structure indirection */
	case '[': mode = 3; break;	/* array */
	}
	/*
	** end the search after finding an operator
	*/
	if(mode > 0) break;
    }
    return(mode);
}

int ComputeIndirection(field)
char *field;
{
char *iptr;
int mode;
int	indirection=0;

    mode = 0;	/* no operator */
    for(iptr=field;iptr && *iptr!= '\0';iptr++){
	switch(*iptr){
	case '.': mode = 1; break;	/* structure */
	case '-': mode = 2; break;	/* structure indirection */
	case '[': mode = 3; break;	/* array */
	case '*': indirection++; break;	/* indirection */
	}
	/*
	** end the search after finding an operator
	*/
	if(mode > 0) break;
    }
    return(indirection);
}

/*
** calculate the address of a field 
*/
char *CalculateAddress(adr,type,field,returninfo)
char *adr;
char *type;
char *field;
Info *returninfo;
{
char fullname[100];
Info info;
char *end;
char *iptr;
char tmp[100];
int field_offset;
int index[MAX_DIMENSIONS];
int dim;
int i;
int indirection = 0;
int mode;

    if(adr == NULL || type == NULL || field == NULL){
	return(NULL);
    }
    strcpy(fullname,type);
    strcat(fullname,".");
    /*
    ** find out what type of operator (if any) exists for the
    ** first field
    */
    mode = ComputeMode(field);
    indirection = ComputeIndirection(field);

    /*
    ** build the full type.field name
    */
    dim  = 0;
    iptr = field;
    if(indirection > 0){
	i=0;
	while(i< indirection){
	    if(*field == '*') i++;
	    field++;
	}
    }
    switch(mode){
    case 0: 		/* plain field name */
	/*
	** append it to the full type string
	*/
	strcat(fullname,field);
	break;
    case 1:		/* structure reference */
        end =strchr(field,'.');
	/*
	** copy the first component (up to the .) into tmp
	*/
	strncpy(tmp,field,(WORD)end-(WORD)field);
	tmp[end-field] = '\0';
	/*
	** and append it to the full type string
	*/
	strcat(fullname,tmp);
	break;
    case 2:		/* structure indirection */
        end =strchr(field,'-');
	/*
	** copy the first component (up to the -) into tmp
	*/
	strncpy(tmp,field,(WORD)end-(WORD)field);
	tmp[end-field] = '\0';
	/*
	** and append it to the full type string
	*/
	strcat(fullname,tmp);
	break;
    case 3:		/* array reference */
        end =strchr(iptr,'[');
	/*
	** copy the field name (up to the [) into tmp
	*/
	strncpy(tmp,field,(WORD)end-(WORD)field);
	tmp[end-field] = '\0';
	/*
	** add the field name to the full type string
	*/
	strcat(fullname,tmp);
	/*
	** get the index values
	** this needs to be fixed (doesnt handle multiple array fields
	** correctly) b[x].c[y]
	*/
	while ((end = strchr(iptr,'['))) {
	    if (sscanf(end+1,"%d",index+dim) != 1 ||
		    (iptr = strchr(end,']')) == NULL ||
		    index[dim] < 0) {
		Error();
		printf("invalid array indexing\n");
		return(NULL);
	    }
	    dim++;
	}
	break;
    }

    /*
    ** get the info on the type.field and compute its address
    */
    if(GetInfo(fullname,&info)){
	/*
	** if there is array indexing then  calculate the array offset
	** as well
	*/
	if(dim == info.dimensions){
	    /*
	    ** this bit of code calculates the offset of an array indexing 
	    ** sequence given in index
	    */
	    if(dim == 0){
		field_offset = info.offset;
	    } else{
		field_offset = info.offset;
		for(i=0;i<dim;i++){
		    if(info.field_indirection == 0){
			field_offset += 
			IndexOffset(i,index[i],info.dimension_size,dim)
			*info.type_size;
		    } else {
			field_offset += 
			IndexOffset(i,index[i],info.dimension_size,dim)
			*sizeof(char *);
		    }
		}
	    }
	    adr += field_offset;
	} else
	/*
	** is this a dynamically allocated array?
	*/
	if(dim <= info.field_indirection){
	    /*
	    ** this bit of code calculates the offset of an array indexing 
	    ** sequence 
	    */
	    if(dim == 0){
		adr += info.offset;
	    } else{
		field_offset = info.offset;
		for(i=0;i<dim;i++){
		    adr  = (char *)(*((char **)(adr + field_offset)));
		    if(--info.field_indirection > 0){
			field_offset = index[i] *sizeof(char *);
		    } else {
			field_offset = index[i] *info.type_size;
		    }
		}
		adr  += field_offset;
	    }
	} else {
	    Error();
	    printf("invalid array indexing\n");
	    return(NULL);
	}
	/*
	** if there are more fields then recursively calculate their 
	** addresses using the newly aquired type info and the next field
	*/
	if(mode == 3){
	    /*
	    ** get the index values
	    ** this needs to be fixed (doesnt handle multiple array fields
	    ** correctly) b[x].c[y]
	    */
	    while ((end = strchr(field,']'))) {
		field = end+1;
	    }
	    mode = ComputeMode(field);
	}
	switch(mode){
	case 1:
	    end = strchr(field,'.');
	    /*
	    ** nested structure field
	    */
	    if((adr = 
	    CalculateAddress(adr,info.type,strchr(field,'.')+1,&info)) == NULL) 
	    return(NULL);
	    break;
	case 2:
	    end = strchr(field,'-');
	    /*
	    ** pointer indirection field
	    */
	    if((adr = 
	    CalculateAddress((char *)(*((char **)(adr))),
	    info.type,strchr(field,'>')+1,&info)) == NULL) 
	    return(NULL);
	    break;
	}
	/*
	** if there is indirection then determine if it is appropriate
	** and then apply it
	*/
	if(info.field_indirection >= indirection){
	    for(i=0;i<indirection;i++){
		info.field_indirection--;
		adr = (char *)(*((char **)adr));
	    }
	}
	/*
	** get the field info and return its address 
	*/
	BCOPY(&info,returninfo,sizeof(Info));
	return(adr);
    } else {
	return(NULL);
    }
}
