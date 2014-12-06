/*********************************************************************

** This library uses the netcdf - version 3.4 which is
** Copyright 1993-1997 University Corporation for Atmospheric Research/Unidata

** The netcdf library is provided as per the terms of the
** UCAR/Unidata license.

** See interface/netcdf-3.4/copyright.html for the full notice.

********************************************************************/

/* $Id: gen_metadata.c,v 1.2 2005/07/01 10:03:02 svitak Exp $ */
/*
 * $Log: gen_metadata.c,v $
 * Revision 1.2  2005/07/01 10:03:02  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:29  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.10  2000/06/12 06:09:31  mhucka
 * Botched original attempt at removing nested comments.  Trying again :).
 *
 * Revision 1.9  2000/06/12 05:07:02  mhucka
 * Removed nested comments; added NOTREACHED comments where appropriate.
 *
 * Revision 1.8  1999/10/16 21:52:46  mhucka
 * Merged Upi's changes for supporting netCDF 3.4.  The changes in most files
 * only involve the copyright notice.  I also fixed up the comments slightly
 * to reflect the location of the actual netCDF copyright file.
 *
 * Revision 1.7  1999/10/16 21:23:48  mhucka
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
 * Revision 1.4  1998/01/14 00:40:28  venkat
 * Changes to accomodate support for FMT1 files.
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
#include <sys/types.h>

#include "absff_ext.h"
#include "diskio_ext.h"


static Name2Type_Table name2type[] = {
	{"uint8", abs_uint8},
	{"int8", abs_int8},
	{"uint16", abs_uint16},
	{"int16", abs_int16},
	{"uint32", abs_uint32},
	{"int32", abs_int32},
	{"uint64", abs_uint64},
	{"int64", abs_int64},
	{"float32", abs_float32},
	{"float64", abs_float64},
	{"uchar8", abs_uchar8},
	{"char8", abs_char8},
	{"char16", abs_char16},
	{"string", abs_string}
};

int Gen_Metadata(md, action)
 struct Gen_Metadata_Type *md;
 Action *action;
{
  Abs_Type type;
  long nvals;
  char *mdname;
  long start_index, count;

  char *field, *value;
  char *vals[MAX_CHARS_IN_STRING];
  static char buf[BUFSIZ];

  char *values_field, *values_value;
  char **showvals;
  register int i, j;

  static Abs_Diskio *adi;
  static Abs_Variable *avi;
  Abs_Metadata *amdi;

  static int numn2t;

  MsgIn *msg;

  int ac = action->argc;
  char **av = action->argv;

  struct Gen_Diskio_Type *parentdiskio;
 
  if(strcmp(BaseObject(md->parent)->name, "diskio") == 0)
	parentdiskio = (struct Gen_Diskio_Type *)md->parent;
  else
	parentdiskio = (struct Gen_Diskio_Type *)md->parent->parent;


  SELECT_ACTION(action){
	case CREATE:
  		if(parentdiskio == NULL || !(strcmp(BaseObject(parentdiskio)->name, "diskio") != 0 || strcmp(BaseObject(parentdiskio)->name, "variable") != 0)) { 
			ffError("At least 1 ancestor must be of type diskio");
			return 0;
		}
		if ( strcmp(parentdiskio->fileformat, "FMT1") == 0 &&
			strcmp(md->name, "auto3-DPosSAVE_diskio") != 0 )
			{
				fprintf(stderr, "Fileformat '%s' does not allow for creation of arbitrary metadata '%s'.\n", parentdiskio->fileformat, md->name);
				return 0;
		}

		adi = (Abs_Diskio*) ParentDiskio_Interface(md);
		avi = (Abs_Variable*) ParentVariable_Interface(md);

		/* 
			** FF_Number is a macro similar to XtNumber - 
			** returns the number of objects 
			** in a statically-sized array

			numn2t is a static variable used in this function
			in a number of actions. So we set it once here in
			the CREATE code.
		*/
  		numn2t = FF_Number(name2type); 
		{
		/*
		  ###################################################################
				BEGIN-SECTION
		  This section parses the create command-line args and sets local variables
		  to be passed to the interface Constructor.
			
		  ##################################################################
		*/
		
			ac--; av++; /*object type */
			ac--; av++; /* path */
		
		 	/*	 
		   	We start by specifying default values for the various fields. The 
		   	default metadata is an 8-bit blank char value 
		 	*/ 
			values_field = NULL;
			values_value = NULL;
			type = (Abs_Type)abs_char8;
			nvals = 1;

			mdname = adi->CreateIndexedName(adi, md->name, md->index);
				
		 	/* Parse CREATE arguments as fieldname/value pairs */
			for(i=0; i<ac; i+=2){
		  		if(strcmp(av[i]+1, "size")==0)
					nvals = Absff_Atol(av[i+1]); 
				if(strcmp(av[i]+1, "datatype")==0){
					for(j=0; j<numn2t; j++){
						if(strcmp(name2type[j].typename, av[i+1])==0){
							type = 	name2type[j].type;
							break;
						}
					}
				}
				if(strstr(av[i]+1, "value")!= NULL){
					values_field = av[i]+1;	
					values_value = av[i+1];
				}
			}
		
		/*
		  ######################################################################
				END_SECTION
		  #####################################################################
		*/  

		}

		if(avi == NULL)
			md->amdi = (char*) adi->ConstructMetadata_Interface(adi, mdname, type, nvals);
		else
			md->amdi = (char*) avi->ConstructMetadata_Interface(avi, mdname, type, nvals);

		free(mdname);

		amdi = (Abs_Metadata *) md->amdi;

		{
		/* 
		  ###################################################################
				BEGIN-SECTION
		  This section checks for a valid metadata interface handle and returns 
		  a success status to the base code. The SET action is also called prior to that
	  	  if -value had been specfied in the create command
		  #################################################################
		*/

			if(amdi != NULL){
				if(values_field != NULL && values_value != NULL){
					SetElement((Element *)md, values_field, values_value);	
				}
				return 1;	
			}	
			else {
				struct Gen_Diskio_Type *parentdiskio;
				if(avi == NULL)
					parentdiskio = (struct Gen_Diskio_Type *)md->parent;
				else
					parentdiskio = (struct Gen_Diskio_Type *)md->parent->parent;
				if(parentdiskio->accessmode == 'r')	
					ffError("File accessmode '%c' does not allow creating metadata '%s'", 
						parentdiskio->accessmode, md->name);	
			}

		/*
		  ###################################################################
				END-SECTION
		  ##################################################################
		*/
		}

		return 0;
		/* NOTREACHED */
	break;

	case ADDMSGIN:
		amdi = (Abs_Metadata *) md->amdi;
		msg = (MsgIn*) (action->data);

		if(! msg->type == 0) /* SAVE */
			return 0;
	
		if(strcmp(md->name, "auto3-DPosSAVE_diskio") == 0) {
			float posvals[3];

			posvals[0] = msg->src->x;
			posvals[1] = msg->src->y;
			posvals[2] = msg->src->z;	
			start_index = (long) 0;
			count = (long) 3;		
			if (amdi != NULL && amdi->SetValues(amdi, posvals, start_index, count) == -1)
				return 0;
		}
	break;
#if 0
	case DELETEMSGIN:
		/* Signal that the variable described or the SAVE message 
		 associated with this is inactive by just renaming it in the
		 file. */
		amdi = (Abs_Metadata *) md->amdi;
		amdi->Rename(amdi, "autoinactiveSAVE_diskio");
	break;
#endif
	case SET:
		amdi = (Abs_Metadata *) md->amdi;
		if(ac == 2) {

			field = av[0];
			value = av[1];
		

			if (strcmp(field, "datatype") == 0){
				for(j=0; j < numn2t; j++){
					if(strcmp(name2type[j].typename, av[1])==0){
						type = name2type[j].type;
						break; 
					}
				}
				if (amdi != NULL && amdi->SetType(amdi, type) == -1){
					struct Gen_Diskio_Type *parentdiskio;
					if(avi == NULL)
						parentdiskio = (struct Gen_Diskio_Type *)md->parent;
					else
						parentdiskio = (struct Gen_Diskio_Type *)md->parent->parent;
					ffError("Format '%s' or file accessmode '%c' does not allow setting type for metadata '%s'", 
						parentdiskio->fileformat, parentdiskio->accessmode, md->name);	
					return 1;
				}
			}
			
			if(strcmp(field, "size")==0){
				nvals = Absff_Atol(value);
				if(amdi != NULL && amdi->SetSize(amdi, nvals) == -1){
					struct Gen_Diskio_Type *parentdiskio;
					if(avi == NULL)
						parentdiskio = (struct Gen_Diskio_Type *)md->parent;
					else
						parentdiskio = (struct Gen_Diskio_Type *)md->parent->parent;
					ffError("Format '%s' or file accessmode '%c' does not allow setting size for metadata '%s'", 
						parentdiskio->fileformat, parentdiskio->accessmode, md->name);	
					return 1;
				}
			}
			/* character types need to be dealt with in 
				a special way. Currently the scheme 
				is to store each char in a string of a fixed-size 
				array  before 
				using the interface to set it in file. This is why the 
				limit is enforced on the number of characters in a string value.
				We could determine this dynamically but it is more 
				efficient this way. Another problem is if someone attempts to
				set a bunch of values or a string after creating a default attribute,
				without setting the size.
				Other types are passed one-by-one as strings in
				argv/argc and so the scheme in that situation is
				to just set one string in the array to the value
			*/

			if(strstr(field, "value") != NULL){
				if (amdi != NULL)
					type = amdi->GetType(amdi);
				/* BIG HACK - UGLY_SIGNPOST */
				if(type == abs_char8 || type == abs_uchar8){
					count = strlen(value) + 1;
					if (amdi != NULL)
						amdi->SetSize(amdi, count);
					for(i=0; i<count; i++){
						sprintf(buf, "%c", value[i]);
						vals[i] = CopyString(buf);
					}
				}
				else{
					count = ffParseWhiteSpacedString(value, vals);
				}	
				start_index = ffParseIndexedField(field);
				if(start_index == -1) start_index = 0;
				if(amdi != NULL && amdi->SetValuesFromString(amdi, vals, start_index, count) == -1){
					struct Gen_Diskio_Type *parentdiskio; 
					if(avi == NULL)
						parentdiskio = (struct Gen_Diskio_Type *)md->parent;
					else
						parentdiskio = (struct Gen_Diskio_Type *)md->parent->parent;
					ffError("Probably a wrong index '%d' or number of values to set'%d' provided\n\
Or format '%s' or file accessmode '%c' does not allow setting values for metadata '%s'",
							start_index, count, parentdiskio->fileformat, parentdiskio->accessmode, md->name);	
					for(i=0; i<count; i++)
						FreeString(vals[i]);
					return 1;
				}
				else{
					for(i=0; i<count; i++)
						FreeString(vals[i]);
					return 1;	
				}
			}	
		}		
		return 0;
		/* NOTREACHED */
	break;
#if 0
	case RENAME:
		/*
		- md->amdi->Rename(md->amdi);	
		- Self-Destruct by calling the DELETE action
		- Recreate with new name (???)
		*/	
	break;
#endif
	
	case SHOW:
		field = action->data;
		amdi = (Abs_Metadata *) md->amdi;
		if(amdi != NULL) {
			if(strcmp(field, "datatype") == 0){
				type = amdi->GetType(amdi);		
				for(j=0; j<numn2t; j++)
					if(name2type[j].type == type) break;
				strcpy(buf, name2type[j].typename);
				action->passback = buf;
				return 1;
			}
			else if(strcmp(field, "size") == 0)
				md->size= amdi->GetSize(amdi);
			else if(strstr(field, "value") != NULL){
				type = amdi->GetType(amdi);
				/* Deal specially with string values - UGLY_SIGNPOST */
				if(type == abs_char8 || type == abs_uchar8){
					start_index=ffParseIndexedField(field);
					if(start_index == -1) {
						start_index = 0;
						count = amdi->GetSize(amdi);
					}
					else
						count = 1;
				}
				else{
					start_index=ffParseIndexedField(field);
					if(start_index == -1) start_index = 0;
					count = 1;
				}
				showvals = (char **) malloc(count * sizeof(char*));
				strcpy(buf, "");
				if(amdi->GetValuesAsString(amdi, showvals, start_index, count) == -1)
					ffError("Wrong index ('%d') or number of values ('%d')", start_index, count);
				else {
					for(i=0; i<count; i++){
				 		strcat(buf, showvals[i]);
						if(! (type == abs_char8 || type == abs_uchar8))
							strcat(buf, " ");
				 		FreeString(showvals[i]);
					}
				}
				free(showvals);
				action->passback = buf;
				return 1;
			}
		}
		return 0;
		/* NOTREACHED */
	break;

	case UPDATE:
	case FLUSH:
		amdi = (Abs_Metadata *) md->amdi;
		if(amdi != NULL) {
			int stat;
			stat = amdi->Flush(amdi);			
			if(stat != -1) return 1;
		}
		return 0;
		/* NOTREACHED */
	break;

	case DELETE:
		adi = (Abs_Diskio *) ParentDiskio_Interface(md);
		avi = (Abs_Variable *) ParentVariable_Interface(md);
		amdi = (Abs_Metadata *) md->amdi;
		if(amdi != NULL) {
			amdi->Destruct(amdi);	
			if (avi != NULL)
				avi->FreeChildInterface(avi, amdi);
			else
				adi->FreeChildInterface(adi, amdi);
		}
	break;
#if 0
	case CLOSE:
		amdi->Delete(amdi);
		amdi->Destruct(amdi);
		free(amdi);
		if(md->values != NULL) FreeString(md->values);
		break;
#endif

	case PROCESS:

	break;

	case RESET:
	
	break;

  }
  return 0;	
}
