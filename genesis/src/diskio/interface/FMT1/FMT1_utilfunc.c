#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include "FMT1_ext.h"

/* This is defined in accordance with the defined base interface class
        data types (i.e. enum Abs_Type), and thus provides a one-one mapping */

fmt1_type FMT1_type[14] = {
	FMT1_INVALID,
	FMT1_INVALID,
        FMT1_INVALID,
        FMT1_SHORT,
        FMT1_INVALID,
        FMT1_INT,
        FMT1_INVALID,
        FMT1_LONG,
        FMT1_FLOAT,
        FMT1_DOUBLE,
        FMT1_INVALID,
        FMT1_CHAR,
        FMT1_INVALID,
        FMT1_INVALID
};


fmt1_type Abs_Type_To_fmt1_type(type) 
 Abs_Type type;
{
 return FMT1_type[(int)type];
}

Abs_Type fmt1_type_To_Abs_Type(ftype) 
	fmt1_type ftype;
{
 switch(ftype) {

	case FMT1_SHORT:
		return abs_int16;
		/* NOTREACHED */
		break;

	case FMT1_INT:
		return abs_int32;
		/* NOTREACHED */
		break;
		
	case FMT1_LONG:
		return abs_int64;
		/* NOTREACHED */
		break;
	
	case FMT1_FLOAT:
		return abs_float32;
		/* NOTREACHED */
		break;

	case FMT1_DOUBLE:
		return abs_float64;
		/* NOTREACHED */
		break;

	default:
		return (Abs_Type)-1;
		/* NOTREACHED */
		break;
 }
}

void *FMT1_Convert_To_fmt1_type(vals, type, size)
 void *vals;
 Abs_Type type;
 int size;
{
	fmt1_type ftype;
	void *retvals;

	int i, sz;

	if(vals == NULL) return NULL;

	ftype = Abs_Type_To_fmt1_type(type);
	sz = (size == 0)? 1: size;

	switch (ftype){

		case FMT1_INVALID:
			retvals = NULL;
			break;

		default:
			retvals = vals;
			break;
	}

	return retvals;
}


 /* Since the notion of attribute/variable names does not exist in FMT1
    we desist from implementing this as a method for the FMT1_diskio object
    However in order to create the appropriate number of indexed children
    we recreate names for every autoSAVE attr and variable in the 
	netcdf-type encoding i.e., autoSAVE....-_<num> with the following
	2 utility functions.

 	UGLY_SIGNPOST:
		FMT1 is very fixed in its organization and sufficently 
		inflexible in its format to deter users from using 
		it for anything other that reasons of BC with genesis.
		It is therefore assumed that the only metadata/variable 
		elements that exist in it come from the genesis SAVE messages.
 */

 /* attrname should be a sufficiently  allocated char array */
 void FMT1_FillIndexedAttrName(attrname, attrnum)
	char *attrname;
	int attrnum;
{
 char locbuf[10];

 if(attrnum < 0) return; 
 
 strcpy(attrname, "auto3-DPosSAVE_diskio");
 if(attrnum != 0) {
        strcat(attrname, "-_");

        sprintf(locbuf, "%d", attrnum);
        strcat(attrname, locbuf);
 }
}

 void FMT1_FillIndexedVarName(varname, varnum)
	char *varname;
	int varnum;
{
 char locbuf[10];


 if(varnum < 0) return; 
 
 strcpy(varname, "autoSAVE_diskio");
 if(varnum != 0) {
        strcat(varname, "-_");

        sprintf(locbuf, "%d", varnum);
        strcat(varname, locbuf);
 }

}
