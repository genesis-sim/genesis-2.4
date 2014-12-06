static char rcsid[] = "$Id: sim_fields.c,v 1.2 2005/06/27 19:01:04 svitak Exp $";

/*
** $Log: sim_fields.c,v $
** Revision 1.2  2005/06/27 19:01:04  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.11  2001/06/29 21:33:43  mhucka
** Fixes from Hugo C.
**
** Revision 1.10  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.9  2001/04/01 19:31:21  mhucka
** Small cleanup fixes involving variable inits.
**
** Revision 1.8  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.7  1997/05/28 23:56:21  dhb
** Changes from Antwerp GENESIS 21e
**
 *
 * EDS20j-l revison: EDS BBF-UIA 95/07/25-95/12/25
 * Increased DisplayFields, do_getfields and FormatStruct string sizes
 *
** Revision 1.6  1994/09/23 16:36:30  dhb
** Changes to DisplayFields() to support connections.  Was calling action
** function as if it were an element, but it may be a connection object.
** Also, changed field protection check to use object passed to function
** rather than element->object since a connection object doesn't have an
** object field.
**
 * Revision 1.5  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.4  1993/03/18  17:15:55  dhb
 * Element field protection.
 *
 * Revision 1.3  1993/03/10  23:11:35  dhb
 * Extended element fields
 *
 * Revision 1.2  1993/02/23  19:10:32  dhb
 * 1.4 to 2.0 command argument changes.  *
 * 	do_getfields changed to use GENESIS getopt routines.
 *
 * Revision 1.1  1992/10/27  20:03:13  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

static Action show_action = { "SHOW", SHOW, NULL, 0, NULL, NULL };

int GetDatatype(type)
char *type;
{
    if(type == NULL){
	return(INVALID);
    }
    if(strcmp(type,"char") == 0){
	return(CHAR);
    } else
    if(strcmp(type,"short") == 0){
	return(SHORT);
    } else
    if(strcmp(type,"int") == 0){
	return(INT);
    } else
    if(strcmp(type,"float") == 0){
	return(FLOAT);
    } else
    if(strcmp(type,"double") == 0){
	return(DOUBLE);
    } else
    return(INVALID);
}


void DisplayOneField(adr,type,field)
char *adr;
char *type;
char *field;
{
Info 		info;
int 		i;
int 		j;
char		pr[100];
char 		ival[20];
int 		offset;
int 		total;
int 		mod;
char 		*dstring;
char		name[100];

    /*
    ** construct the variable specifier of the form:
    ** datatype.fieldname
    */
    strcpy(name,type);
    strcat(name,".");
    strcat(name,field);
    /*
    ** get information on the variable specifier
    */
    if(GetInfo(name,&info)){
	/*
	** get the value of the variable in generic string form
	*/
	dstring = GetValue(adr + info.offset,&info);
	/*
	** if it fails then try to recursively display
	** the fields of the field
	** this is assuming that the unknown field is
	** a structure and has other fields in it
	*/
	if(strlen(dstring) == 0){
#ifdef RECURSIVE
	    if(info.struct_type == 1){
		printf("(%s)\n",field);
		DisplayFields(adr+info.offset,info.type);
	    } else
#endif
		/*
		** use the string ??? to indicate an unknown value
		*/
		FieldFormat(field,"???");
	} else {
	    /*
	    ** is this variable an array?
	    */
	    if(info.dimensions == 0){
		/*
		** if not then just print the value
		*/
		FieldFormat(field,dstring);
	    } else {
		/*
		** if it is an array then
		** construct a string which contains the field
		** name along with the array dimensions
		*/
		strcpy(pr,field);
		for(i=0;i<info.dimensions;i++){
		    sprintf(ival,"[%d]",info.dimension_size[i]);
		    strcat(pr,ival);
		}
		/*
		** and display it
		*/
		printf("%-20s = ",pr);
		total = 1;
		for(i=0;i<info.dimensions;i++){
		    total *= info.dimension_size[i];
		}
		printf("{ ");
		offset = info.offset;
		for(i=0;i<total;i++){
		    dstring = GetValue(adr + offset,&info);
		    printf("%s , ",dstring);
		    mod = 1;
		    for(j=info.dimensions-1;j>=0;j--){
			mod *= info.dimension_size[j];
			if(i < total -1){
			    if((i +1)%mod == 0) printf("},{");
			} else
			    printf("}");
		    }
		    if(info.field_indirection == 0){
			offset += info.type_size;
		    } else {
			offset += sizeof(char *);
		    }
		}
		printf("\n");
	    }
	}
    } else {
	printf("could not find field %s\n",field);
    }
}

void DisplayFields(element,object,adr,type,start_field,after)
Element	*element;
GenesisObject 	*object;
char 	*adr;
char	*type;
char	*start_field;
int	after;
{
char 		*ptr1;
char 		*ptr2;
char 		*tmp;
char		field[100];
/* char 		string[1500]; */
int		reached = 0;
extern char 	*FieldHashFindAndCopy();
int		already_shown;
PFI		func;
int		prot;

    if(adr == NULL) return;
    /*
    ** get the string containing all of the fields for given data structure
    */
    ptr1 = ptr2 = FieldHashFindAndCopy(type);
    if(ptr2 == NULL) return;
    /*
    ** copy the field list into a safe place
    */
    /* strcpy(string,ptr2); */
    /* ptr2 = string; */
    while(*ptr2 != '\0'){
	/*
	** find the next field in the list by searching for a CR
	*/
	tmp = ptr2;
	while(*tmp != '\n') tmp++;
	*tmp = '\0';
	/*
	** get the field name
	*/
	strcpy(field,ptr2);
	/*
	** advance the ptr beyond the field name
	*/
	ptr2 = tmp+1;
	/*
	** if a starting field has been specified then check
	** to see if it has been reached
	*/
	if(start_field && !reached){
	    if(strcmp(field,start_field) != 0){
		/*
		** if it hasnt then continue
		*/
		continue;
	    }
	    /*
	    ** otherwise indicate that the field has been found
	    */
	    reached = 1;
	    /*
	    ** if the non-inclusive flag is set then start after
	    ** this field
	    */
	    if(after) continue;
	}

	prot = GetFieldListProt(object, field);
	if (prot == -1 || prot != FIELD_HIDDEN)
	  {
	    /*
	    ** if the object has the SHOW action then give it the
	    ** first shot
	    */
	    already_shown = 0;
	    if(object && (func = GetActionFunc(object,SHOW,NULL,NULL))){
		Element	tmp;
		/*
		** if it does then call the function with the action
		*/
		show_action.data = field;

		/*
		** Can only use CallActionFunc() with an element.  Call the
		** action function directly for connections.  This should be
		** safe since a connection can never be an extended object
		** and so can not have a script action function.
		**
		** CheckClass() takes an element to check against so I have
		** to use the tmp element and set the object field to my
		** object (which may be a connection structure).
		*/
		tmp.object = object;
		if (CheckClass(&tmp, ClassID("connection")))
		    already_shown = func(element, &show_action);
		else
		    already_shown = CallActionFunc(element, &show_action);

		if (already_shown)
		    FieldFormat(field,show_action.passback);
	    } 
	    if(!already_shown){
		DisplayOneField(adr,type,field);
	    }
	  }
    }

    free(ptr1);
}

void FieldFormat(field,value)
char *field;
char *value;
{
    printf("%-20s = %s\n", field,value);
}

char *FieldValue(adr,object,field)
char	*adr;
GenesisObject	*object;
char	*field;
{
char *value = NULL;
int	status;
int	already_shown;
PFI	func;

    if(adr == NULL || object == NULL || field == NULL){
	return(NULL);
    }
    already_shown = 0;
    if ((func = GetActionFunc(object,SHOW,NULL,NULL))) {
	Element*	elm = (Element*) adr;
	Element		tmp;
	/*
	** if it does then call the function with the action
	*/
	show_action.data = field;

	/*
	** Can only use CallActionFunc() with elements.  Check to see if
	** the object refers to a connection.  If so, call the action
	** function directly.  This should be save, since the action
	** function will never be a script function for a connection.
	**
	** CheckClass() needs an element to work.  I set the object field
	** of the tmp element to the object of interest.
	*/
	tmp.object = object;
	if (CheckClass(&tmp, ClassID("connection")))
	    already_shown = func(elm, &show_action);
	else
	    already_shown = CallActionFunc(elm, &show_action, elm->object);
	value = show_action.passback;
    } 
    if(!already_shown){
	value = GetFieldStr(adr,object->type,field,&status);
	if(!status){
	    /*
	    Error();
	    printf("could not find field '%s'\n",field);
	    */
	    return(NULL);
	}
    }
    return(value);
}

void DisplayField(adr,object,field)
char 	*adr;
GenesisObject 	*object;
char 	*field;
{
char	*value;

    if(adr == NULL || object == NULL) return;

    if((value = FieldValue(adr,object,field)) != NULL){
	FieldFormat(field,value);
    }
}

char *GetFieldStr(adr,type,field,status)
char 	*adr;
char 	*type;
char 	*field;
int	*status;
{
char *address;
Info info;

    if((address = CalculateAddress(adr,type,field,&info)) != NULL){
	*status = 1;
	/*
	** if it is a structure then return the entire contents of the
	** structure
	*/
	return(GetValue(address,&info));
    } else {
	*status = 0;
	return(NULL);
    }
}

char *GetFieldAdr(element,field,datatype)
Element 	*element;
char 		*field;
short 		*datatype;
{
char 	*address;
Info 	info;

    if((address = CalculateAddress(element,Type(element),field,&info)) != NULL){
	*datatype = GetDatatype(info.type);
	return(address);
    } else
	return(NULL);
}

char **do_getfields(argc,argv)
int argc;
char **argv;
{
extern char 	*FieldHashFindAndCopy();
int fargc;
char **fargv;
/* char string[1500]; */
char *ptr;
Element *element;

    initopt(argc, argv, "element");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return(NULLArgv());
      }

    element = GetElement(optargv[1]);
    if (element == NULL)
	return NULLArgv();

    /*
    ** get the field string
    */
    ptr = FieldHashFindAndCopy(Type(element));
    if(ptr == NULL) return(NULLArgv());
    /* strcpy(string,ptr); */
    AddExtFieldsToString(element, ptr);
    StringToArgList(ptr,&fargc,&fargv,0);
    free(ptr);
    return(fargv);
}

/* char *FormatStruct(s) */
/* char *s; */
/* { */
/* static char string[1500]; */
/* char *ptr; */

/*     */
/*     ** get the string containing all of the fields for given data structure */
/*     * */
/*     if(s == NULL) return(0); */
/*     ptr = string; */
/*     for(;*s != '\0';s++){ */
/* 	if(*s == '\n'){ */
/* 	    *ptr = '\n'; */
/* 	    ptr++; */
/* 	    *ptr = '\t'; */
/* 	    ptr++; */
/* 	    *ptr = '\t'; */
/* 	    ptr++; */
/* 	    *ptr = '\t'; */
/* 	} else { */
/* 	    *ptr = *s; */
/* 	} */
/* 	ptr++; */
/*     } */
/*     *ptr = '\0'; */
/*     return(string); */
/* } */

