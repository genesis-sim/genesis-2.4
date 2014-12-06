/*********************************************************************

** This library uses the netcdf - version 3.4 which is
** Copyright 1993-1997 University Corporation for Atmospheric Research/Unidata

** The netcdf library is provided as per the terms of the
** UCAR/Unidata license.

** See interface/netcdf-3.4/copyright.html for the full notice.

********************************************************************/

/* $Id: diskio_utilfunc.c,v 1.2 2005/06/17 17:37:06 svitak Exp $ */
/*
 * $Log: diskio_utilfunc.c,v $
 * Revision 1.2  2005/06/17 17:37:06  svitak
 * Deprecated varargs.h replaced by stdarg.h. Old-style argument lists updated.
 * These changes were part of varargs-genesis.patch. Systems without stdarg.h
 * will no longer compile.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:29  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.9  2000/06/12 05:07:01  mhucka
 * Removed nested comments; added NOTREACHED comments where appropriate.
 *
 * Revision 1.8  1999/10/16 21:52:46  mhucka
 * Merged Upi's changes for supporting netCDF 3.4.  The changes in most files
 * only involve the copyright notice.  I also fixed up the comments slightly
 * to reflect the location of the actual netCDF copyright file.
 *
 * Revision 1.7  1999/10/16 21:23:47  mhucka
 * Venkat had a version of the diskio code in his home directory that he
 * appears to have been working on, and that did not make it into the
 * source tree that I had originally used as the starting point of my
 * GENESIS 2.2.2 DR reorganization effort.  In order to merge Venkat's
 * changes into the distribution, I'm reverting to his version, then will
 * update his version to incorporate recent changes by Upi for netCDF 3.4.
 *
 * Revision 1.5  1998/01/15 02:19:50  venkat
 * Inclusion-of-interface-headers-in-subdirectory-changed-to-be-not-hardcoded.-Now-this-is-taken-care-through-the-use-of-the-'-I'-flag-during-compilation-in-the-Makefile
 *
 * Revision 1.4  1998/01/14 00:35:21  venkat
 * Defined utility functions that encompass the mechanism of setting field
 * protections for variable and metadata children. Needed when the supported
 * format is not self-describing. e.g: FMT1
 *
 * Revision 1.3  1997/08/04 04:18:19  dhb
 * include of strings.h does not work under Solaris.  Include
 * of string.h should suffice.
 *
 * Revision 1.2  1997/07/26 00:12:00  venkat
 * Added
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>

#include "absff_ext.h"
#include "diskio_ext.h"


void ffError(char *format, ...)
{
	char error[256];
	va_list args;

	va_start(args, format);
	vsprintf(error, format, args);
	va_end(args);	 

	fprintf(stderr, "Error: %s\n", error);
}

char * ParentDiskio_Interface(elm)
 Element *elm; 
{
  Element *parentofelm;

  if (elm == NULL || (parentofelm = elm->parent) == NULL)
		return NULL;
	
  if(strcmp(BaseObject(parentofelm)->name, "variable") == 0) 
	return ((struct Gen_Diskio_Type *)(parentofelm->parent))->adi;
  else if(strcmp(BaseObject(parentofelm)->name, "diskio") == 0)
	return ((struct Gen_Diskio_Type *)(parentofelm))->adi;
  else
	return (NULL);
}

char *ParentVariable_Interface(elm)
 Element *elm;
{
 Element *parentofelm;

  if (elm == NULL || (parentofelm = elm->parent) == NULL)
		return NULL;

 if(strcmp(BaseObject(parentofelm)->name, "variable") == 0) 
	return ((struct Gen_Variable_Type *)(parentofelm))->avi;
 else
	return (NULL);
}

char *SelfVariable_Interface(elm)
 Element *elm;
{
  if (elm == NULL)
	return NULL;

 if(strcmp(BaseObject(elm)->name, "variable") == 0) 
	return ((struct Gen_Variable_Type *)(elm))->avi;
 else
	return (NULL);
}

char *SelfMetadata_Interface(elm)
 Element *elm;
{
  if (elm == NULL)
	return NULL;
 if(strcmp(BaseObject(elm)->name, "metadata") == 0) 
	return ((struct Gen_Metadata_Type *)(elm))->amdi;
 else
	return (NULL);
}

/* 
   At present, the fields that need to be restricted are fixed  and known 
   both in the case of variables and metadata. Of course, we can
   imagine this function to evolve into one with more arguments that 
   specify the fields and their protections
*/
int ffProtectMetadataFields(elm)
 Element *elm;
{
 
 GenesisObject *obj;

 if(elm == NULL || strcmp(BaseObject(elm)->name, "metadata") != 0)
	return 0;

 obj = ObjectModify(elm->object);

 SetFieldListProt(obj, "size", FIELD_READONLY);
 SetFieldListProt(obj, "datatype", FIELD_READONLY);
 SetFieldListProt(obj, "value", FIELD_READONLY);

 FieldListMakePermanent(obj);
 elm->object = ObjectCacheCheck(obj);

 return 1;

}

/* 
   At present, the fields that need to be restricted are fixed  and known 
   both in the case of variables and metadata. Of course, we can
   imagine this function to evolve into one with more arguments that 
   specify the fields and their protections
*/
int ffProtectVariableFields(elm)
 Element *elm;
{
 
 GenesisObject *obj;

 if(elm == NULL || strcmp(BaseObject(elm)->name, "variable") != 0)
	return 0;

 obj = ObjectModify(elm->object);

 SetFieldListProt(obj, "size", FIELD_READONLY);
 SetFieldListProt(obj, "datatype", FIELD_READONLY);
 SetFieldListProt(obj, "value", FIELD_READONLY);
 SetFieldListProt(obj, "input", FIELD_HIDDEN);
 SetFieldListProt(obj, "output", FIELD_HIDDEN);

 FieldListMakePermanent(obj);
 elm->object = ObjectCacheCheck(obj);

 return 1;

}

char * ffCreateName(genesis_name, genesis_index)
 char *genesis_name;
 int genesis_index;
{
 char *buf;
 char locbuf[10];
 int namesize;

 if(genesis_name == NULL) return NULL;
  
 namesize = strlen(genesis_name) + 1; 
 buf = (char *) malloc(sizeof(char) * namesize);
 strcpy(buf, genesis_name);
 if(genesis_index != 0) {
	namesize += (strlen("-_")+1);
	buf = (char *) realloc(buf, namesize); 
 	strcat(buf, "-_");

 	sprintf(locbuf, "%d", genesis_index);
	namesize += (strlen(locbuf)+1);
	buf = (char *) realloc(buf, namesize);
 	strcat(buf, locbuf);
 }
 return buf;
}

char * ffRecreateName(nameinfile, index_val)
 char *nameinfile;
 int *index_val;
{
  char* genesis_name;
  char index_part[10];
  char *ptr;

  if(nameinfile == NULL || index_val == NULL) return NULL;

  ptr = strstr(nameinfile,  "-_");
  if(ptr != NULL){
	*ptr = '\0'; genesis_name = CopyString(nameinfile);
	ptr+=2; /* go past the "-_" */
	if(ptr != NULL) {
		strcpy(index_part, ptr);	
		*index_val = atoi(index_part);
	}
	else
		*index_val = 0;
  }
  else{
	genesis_name = CopyString(nameinfile);
	*index_val = 0;
  }
  return genesis_name;
} 

long ffParseIndexedField(field)
 char *field;
{
 char *indexpart, *ptr;
 long indexval;
 char buf[BUFSIZ];
 char val[2];

 indexpart = strchr(field, '[');

 if(indexpart == NULL) return -1;
  
 val[1] = '\0';
 strcpy(buf, "");
 for(ptr=indexpart+1; ptr != NULL && *ptr != '\0' && *ptr != ']'; ptr++) {
        val[0] = *ptr;
        strcat(buf, val);
 }
 
 indexval = Absff_Atol(buf);
 return indexval;
}


long ffParseWhiteSpacedString(string, eachstr)
 char *string;
 char **eachstr;
{
  long nstrings;
  char *ptr;

  if(string == NULL || eachstr == NULL) 
	return 0;

  nstrings=0; 

  for(ptr=string; ptr != NULL; ptr++) {
	if(*ptr == ' '){
		string++;
		continue;
	}
	ptr = strchr(string, ' ');	
	if(ptr == NULL){
		eachstr[nstrings] = CopyString(string);
		nstrings++;
		break;
	}
	else{
		*ptr = '\0';
		eachstr[nstrings] = CopyString(string);
		nstrings++;
		string+=(int)(ptr-string+1);
	}
  }
	
  return nstrings;
} 
  

void Expand_And_Copy_String(copybuf, origbuf)
 char **copybuf, *origbuf;
{
  int spacesize;
  int newsize;

  if(copybuf == NULL || *copybuf == NULL || origbuf == NULL) return;

  spacesize = 1;
  newsize = spacesize;

  *copybuf = (char *) realloc(*copybuf, newsize * sizeof(char));
  strcat(*copybuf, " ");

  newsize = strlen(origbuf) + 1 ;
  *copybuf = (char *) realloc(*copybuf, newsize*sizeof(char)); 
  strcat(*copybuf, origbuf); 
}
