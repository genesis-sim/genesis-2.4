/*********************************************************************

** This library uses the netcdf - version 3.4 which is
** Copyright 1993-1997 University Corporation for Atmospheric Research/Unidata

** The netcdf library is provided as per the terms of the
** UCAR/Unidata license.

** See interface/netcdf-3.4/copyright.html for the full notice.

********************************************************************/

/* $Id: gen_variable.c,v 1.2 2005/07/01 10:03:02 svitak Exp $ */
/*
 * $Log: gen_variable.c,v $
 * Revision 1.2  2005/07/01 10:03:02  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:29  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.10  2001/06/29 21:13:32  mhucka
 * Added extra parens inside conditionals and { } groupings to quiet certain
 * compiler warnings.
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
 * Revision 1.4  1998/01/14 00:39:39  venkat
 * Changes to accomodate support for FMT1 files.
 *
 * Revision 1.3  1997/08/04  04:18:19  dhb
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


int Gen_Variable(var, action)
 struct Gen_Variable_Type *var;
 Action *action;
{
   
 static char fileaccessmode;

 Abs_Varshape *shape; 
 Abs_Type type;
 char *varname;


 long start_index[1];
 long count[1];
 register int i,j;
 char *field;
 char *value;
 char **showvals;
 char *values_field, *values_value;

 long ret_index[1];
 static char buf[BUFSIZ];
 char locbuf[BUFSIZ];
 int childsize, nvals;
 char *newargv[50];
 Action newaction;
	MsgIn *msg;
	char *timevals[1];
	char *vals[MAX_CHARS_IN_STRING];
	double currenttime;
 	float fval;
	short inputmsgs;

 static int nname2type;
#if 0
 static int coordvar; /* Flag that determines if this element is a coordinate variable */

 static int coordindex = 0; /* At the momemt this is just a running variable that allocates
			each member of the coordinate_variable interface array that is 
			maintained by the abstract diskio object */
#endif

 /* Local variables for efficient indirection into structure members */
 Abs_Diskio *adi; 
 Abs_Variable *avi;
 char *coordname;
 Abs_Variable *coordi; /* The interface to the coordinate variable */

 struct Gen_Diskio_Type *parentdiskio = (struct Gen_Diskio_Type *)var->parent;

 char **av = action->argv;
 int ac = action->argc;

 newaction.argv = newargv;


 SELECT_ACTION(action){

  case CREATE:
	/* 
		Disallow creation of arbitrary variables for FMT1. 
		We must make this more general later since there may be 
		other formats that need similar treatment.
	*/
	if ((parentdiskio == NULL) || strcmp(BaseObject(parentdiskio)->name, "diskio") != 0) {
		ffError("Immediate parent should be a 'diskio' element\n"); 	
		return 0;
	}	
	if ( strcmp(parentdiskio->fileformat, "FMT1") == 0 &&
		strcmp(var->name, "autoSAVE_diskio") != 0 ) {
			fprintf(stderr, "Fileformat '%s' does not allow for creation of arbitrary variable '%s'\n", parentdiskio->fileformat, var->name);
			return 0;
	}

	adi = (Abs_Diskio*) ParentDiskio_Interface(var); 

	/* 
	This is a static variable that is used in a bunch of actions
	   So we set it once here in the CREATE action 
	*/
 	nname2type = FF_Number(name2type);


	{ 
	/*
		########################################################################
			BEGIN-SECTION
		This section parses the create command-line args and sets local variables
		from them or defaults
		########################################################################
	*/
		ac--; av++; /*object type*/
		ac--; av++; /*path */
			

		/* 
		We go ahead and hardcode the default allocation for the shape 
		struct. Note that we do not currently provide any flexbility in
		terms of the user choosing which dimension the variable is associated with.
		Providing a -domain as an option is conceivable but I think it is best
		at this stage to keep the concept of dimensions transparent to the user.
		Another ugly assumption is that a variable can only be associated with 
		one dimension at most (vector) or none at all (scalar).

		For the sake of expediency (for 2.1 - Oct 96) the "SimulationTime" for analog traces 
		is taken as the default dimension and the default variable is a 1-dimensional
		non-coordinate analog trace with an unlimited number of double-precision floating point values; 
		This should allow people to be able to run simulations easily without having to 
		learn the various options supported by this object.
		*/
		/* UGLY_SIGNPOST */ 

		shape = Absff_ConstructDefaultShape("SimulationTime");
		type = abs_float64;

		varname = adi->CreateIndexedName(adi, var->name, var->index); 

		/* Process all options to 'create' as fieldname/value pairs*/
		values_field = NULL;
		values_value = NULL;
		for(i=0; i < ac; i+=2){
			if(strcmp(av[i]+1, "size") == 0){
				if(strcasecmp(av[i+1], "unlimited")==0)
					shape->domaintype[0]=DYNAMICSIZECOMPONENT;
				else{
					shape->domaintype[0]=STATICSIZECOMPONENT;
					shape->domainsize[0]=Absff_Atol(av[i+1]);
				}
			}	
			if(strcmp(av[i]+1, "datatype") == 0){
				for(j=0; j < nname2type ; j++){
					if(strcmp(name2type[j].typename, av[i+1])==0){
						type = name2type[j].type;
						break;
					} 
				}
			}
			if(strcmp(av[i]+1, "valuetype") == 0){
				if(strcmp(av[i+1], "scalar") == 0)
					shape->ndomains = 0; /* This disallows any attempt on creation of dimensions in the 
								interface code */
			}	
			/*
			if(strcmp(av[i]+1, "category") == 0){
				if(strcmp(av[i+1], "coordinate") == 0){
					coordvar = 1;
					if(shape->domainname[0] != NULL) FreeString(shape->domainname[0]);
					shape->domainname[0]=CopyString(varname);
				}
			}	
			*/
			if(strcmp(av[i]+1, "domain") == 0){
				if(shape->domainname[0] != NULL) FreeString(shape->domainname[0]);
				shape->domainname[0]=CopyString(av[i+1]);
			}	

			if(strstr(av[i]+1, "value") != NULL){
			/* Dealing with the special case of string value 
			  specifications */
				/* UGLY_SIGNPOST */
				values_field = av[i]+1;
				values_value = av[i+1];
				if((type == abs_char8 || type == abs_uchar8) &&
				 (shape->ndomains !=0) && (shape->domaintype[0] != DYNAMICSIZECOMPONENT))
					shape->domainsize[0] = strlen(values_value) +1;
							
			}
		}

	/*
	######################################################################
		END_SECTION
	#####################################################################
	*/

	}

	/* Construct the interface to the variable */
	var->avi = (char *) adi->ConstructVariable_Interface(adi, varname, type, shape);	
	avi = (Abs_Variable *) var->avi;
      
	/* Free the local shape variable and its members */
	Absff_DestructDefaultShape(&shape);



	{	

	/*
		########################################################################
			BEGIN-SECTION
		This section checks for a valid handle to the variable interface and 
		replicates the hierarchy of local attribute children if the mode of access
		of the file is read-only or append provided it is not a coordinate variable. Note that if the file is to be overwritten with
		genesis defaults, the variable attributes are determined in the ADDMSG action 
		and then created. Additionally it also constructs its interface to its own
		coordinate variable.
		########################################################################
	*/
		
		if(avi != NULL) {
			int nlmd;
			if(adi->IsCoordVariable(adi, varname)){
			/* CREATE failed  - Do not create coord variable in hierarchy*/
			/* DELETE action will be called by base code and the destruction of the 
			 interface happens there */
				free(varname);
				return 0; 
			}	
			free(varname);
			fileaccessmode = ((struct Gen_Diskio_Type *)var->parent)->accessmode;
			switch(fileaccessmode){
				case 'r':
				case 'a':
					newaction.type = CREATE;
					newaction.name = "CREATE";
					newaction.argc = 0;
					if(avi->nlocalmetadata > 0){
						char *mdname;
						int mdindex;
						nlmd = avi->nlocalmetadata;
						for(i=0; i<nlmd; i++){
							mdname = adi->RecreateNameAndIndex(adi, avi->localmetadatanames[i], &mdindex);
							Create("metadata", mdname, var, &newaction, mdindex);
							FreeString(mdname);
						}
				  	}
				break;
			}
			/* 
	   		Construct the interface to the coord variable 
			*/
			shape = avi->GetShape(avi);
			if(shape->ndomains > 0){
				coordname = shape->domainname[0];
				var->coordi = (char*) adi->ConstructCoord_Interface(adi, coordname);
			}
			if(values_field != NULL && values_value != NULL){
				SetElement((Element *)var, values_field, values_value);
			}
			return 1;
		}
		else{
			if(fileaccessmode == 'r') 
				ffError("File accessmode '%c' disallows variable '%s' creation", 
					fileaccessmode, var->name);
		}

	/*
		#######################################################################
			END-SECTION
		#######################################################################
	*/
	
	}
	return 0;
	/* NOTREACHED */
  break;

  case ADDMSGIN:
#ifndef HOLD_METADATA 
	newaction.type = CREATE;
	newaction.name = "CREATE";

	/* Create the default genesis attribute children only if the fileaccessmode is overwrite */

	switch(fileaccessmode){
		case 'w':
			
			MSGLOOP(var, msg){
				/****
				  SAVE
				***/
				case 0:
		
					/* FIRST -  ######################################### */

					j=0;
					newargv[j] = "metadata"; j++;
					newargv[j] = "SourceElementName"; j++;

					newargv[j] = "-values"; j++;
					newargv[j] = msg->src->name; j++;

					newargv[j] = "-type"; j++;
					newargv[j] = "char8"; j++;

					newargv[j] = "-size"; j++;
					childsize = strlen(msg->src->name)+1;
					sprintf(buf, "%d", childsize);	
					newargv[j] = buf; j++;

					newaction.argc = j;
					Create("metadata", "SourceElementName", var, &newaction, 0);
			
					/* ############################################### */

					/* SECOND -  ######################################### */

					j=0;
					newargv[j] = "metadata"; j++;
					newargv[j] = "SourceElementIndex"; j++;

					newargv[j] = "-values"; j++;
					sprintf(buf, "%d", msg->src->index); 	
					newargv[j] = buf; j++;
					

					newargv[j] = "-type"; j++;
					newargv[j] = "int32"; j++;

					newargv[j] = "-size"; j++;
					childsize = 1; /* 1 index per element */
					sprintf(locbuf, "%d", childsize);	
					newargv[j] = locbuf; j++;

					newaction.argc = j;
					Create("metadata", "SourceElementIndex", var, &newaction, 0);
			
					/* ############################################### */

					/* THIRD - ##################################### */

					j=0;
					newargv[j] = "metadata"; j++;
					newargv[j] = "SourceElementType"; j++;

					newargv[j] = "-values"; j++;
					newargv[j] = BaseObject(msg->src)->name; j++;
					
					newargv[j] = "-type"; j++;
					newargv[j] = "char8"; j++;

					newargv[j] = "-size"; j++;
					childsize = strlen(BaseObject(msg->src)->name)+1;
					sprintf(buf, "%d", childsize);	
					newargv[j] = buf; j++;

					newaction.argc = j;
					Create("metadata", "SourceElementType", var, &newaction, 0);
			
					/* ############################################### */

					/* FOURTH - ##################################### */

					j=0;
					newargv[j] = "metadata"; j++;
					newargv[j] = "SourceElementLocation"; j++;

					newargv[j] = "-values"; j++;
					sprintf(locbuf, "%f", msg->src-x);
					strcpy(buf, locbuf);

					sprintf(locbuf, "%f", msg->src->y);
					strcat(buf, " ");
					strcat(buf, locbuf);

					sprintf(locbuf, "%f", msg->src->z);
					strcat(buf, " ");
					strcat(buf, locbuf);
					newargv[j] = buf; j++;

					newargv[j] = "-type"; j++;
					newargv[j] = "float32"; j++;

					newargv[j] = "-size"; j++;
					childsize = 3;
					sprintf(locbuf, "%d", childsize);	
					newargv[j] = locbuf; j++;

					newaction.argc = j;
					Create("metadata", "SourceElementLocation", var, &newaction, 0);
			
					/* ############################################### */
						

				break;
			}
			
		break;
	}
#endif
  break;

#if 0
  case DELETEMSGIN:
	/* Create a child metadata element that flags this variable on file to be 
	   inactive and forward this message to it 
	i=0;
	newargv[i] = "metadata"; i++;
	newargv[i] = "autoinactiveSAVE_diskio"; i++;
	newargv[i] = "-size"; i++;
	newargv[i] = "1"; i++;
	newargv[i] = "-datatype"; i++;
	newargv[i] = "int16"; i++;
	
	newaction.argc = i;
	Create("metadata", "autoinactiveSAVE_diskio", var, &newaction, 0);
	*/

	/* Merely renaming the variable on file to a name that 
	   flags this message as inactive will make things considerably cleaner */
	avi = (Abs_Variable *) var->avi;
	avi->Rename(avi, "autoinactiveSAVE_diskio");
  break;
#endif

  case SET:

	/* In each case disallow setting of field by base code in case of error */
	
	avi = (Abs_Variable *) var->avi;

	if(ac == 2) {

		field = av[0];
	  	value = av[1];

	 	if (strcmp(field,  "datatype") == 0){
			for(j=0; j < nname2type; j++){
				if(strcmp(name2type[j].typename, value)==0){
					type = name2type[j].type;
					break;
				}
			}
	 		if(avi != NULL && avi->SetType(avi, type) == -1){
				ffError("Format '%s' or file accessmode '%c' does not allow setting type for variable '%s'", 
					parentdiskio->fileformat, parentdiskio->accessmode, var->name);	
				return 1;
			}
       	    	}
	
	 	if (strstr(field,  "value") != NULL){
			if (avi != NULL)
				type = avi->GetType(avi);
			/* UGLY_SIGNPOST */
			if(type == abs_char8 || type == abs_uchar8){
				count[0] = strlen(value) + 1;
				for(i=0; i<count[0]; i++){
					sprintf(buf, "%c", value[i]);
					vals[i] = CopyString(buf);
				}
			}
			else {
				count[0] = ffParseWhiteSpacedString(value, vals);
			}
			start_index[0] = ffParseIndexedField(field);	
			if(start_index[0] == -1) start_index[0] = 0;
	 		if(avi->SetValuesFromString(avi, vals, start_index, count) == -1){
				ffError("Probably a wrong index ('%d') or number of values to set ('%d') provided\n\
Or format '%s' or file accessmode '%c' does not allow setting values for variable '%s'",
						 start_index[0], count[0], parentdiskio->fileformat, parentdiskio->accessmode, var->name);	
				for(i=0; i<count[0]; i++)
					FreeString(vals[i]);
				return 1;
			}
			else{
				for(i=0; i<count[0]; i++)
					FreeString(vals[i]);
				return 1;
			}
       	    	}
		
		if(strcmp(field, "size") == 0){

		 /* 
			Again we assume only 1-D variables or scalars 
			So the setting of the size is done only for the
			1 domain - UGLY_SIGNPOST
 		*/

			Abs_Varshape *varshape;
			int ndims, status;

			varshape = avi->GetShape(avi);	
			if(varshape == NULL) return 1;

			shape = (Abs_Varshape *) calloc(1, sizeof(Abs_Varshape));
			shape->ndomains = varshape->ndomains;
			if((ndims = shape->ndomains) > 0){
				shape->domainname = (char **) malloc(ndims*sizeof(char*));
				shape->domaintype = (byte *) malloc(ndims*sizeof(byte));
				shape->domainsize = (long *) malloc(ndims*sizeof(long));
				for(i=0; i<ndims; i++){
					shape->domainname[i] = CopyString(varshape->domainname[i]);
					shape->domaintype[i] = varshape->domaintype[i];
					shape->domainsize[i] = varshape->domainsize[i]; 
				}
				shape->domainsize[0] = Absff_Atol(value);	
			}

			status = avi->SetShape(avi, shape);

			/* Free the local shape variable and its members */
			if(ndims > 0){
				free(shape->domaintype);
				free(shape->domainsize);
				for(i=0; i < ndims; i++)
					FreeString(shape->domainname[i]);
				free(shape->domainname);
			}
			free(shape);

			if(status == -1){
				ffError("Format '%s' or file accessmode '%c' does not allow setting size for variable '%s'",
						parentdiskio->fileformat, parentdiskio->accessmode, var->name);	
				return 1;
			}
	   	}
	 }
	 return 0;
	/* NOTREACHED */
  break;

  case SHOW:
	field = action->data;
	coordi = (Abs_Variable*) var->coordi;
	avi = (Abs_Variable *) var->avi;
	/* showobject works on the objects defaults - so we dont want query the interface in such a case */
	if(avi != NULL) { 
		if(strcmp(field, "datatype") == 0){
			type = avi->GetType(avi);
			for(j=0; j<nname2type; j++)
				if(name2type[j].type == type) break;
			strcpy(buf, name2type[j].typename);
			action->passback = buf;
			return 1;
		}
		else if(strcmp(field, "size") == 0){	
			shape = avi->GetShape(avi);
			if(shape == NULL) return 0;
			if(shape->ndomains == 0)
				var->size = 1;
			else
				var->size = shape->domainsize[0];
		}
		else if(strstr(field, "value") != NULL){
		/* Deal with the indexed 'value' field */
			type = avi->GetType(avi);

		/* Special treatment for string values - UGLY_SIGNPOST*/

		/** *******************************************************

		  Please note also that this index business gets kind of sticky 
		  if the capability to deal with multiple dimensions is provided
		  to these objects.

		  The count and start_index arrays may need to be multi-dimensional
		  as well.

		    ********************************************************/

		
			if(type == abs_char8 || type == abs_uchar8) {
				shape = avi->GetShape(avi);
				if((shape->ndomains == 0) ||
					((start_index[0] = ffParseIndexedField(field)) != -1))
					count[0] = 1;
				else{
					if(start_index[0] == -1) start_index[0] = 0;
					count[0] = shape->domainsize[0];
				}
			}
			else{
				start_index[0] = ffParseIndexedField(field);
				if(start_index[0] == -1) start_index[0] = 0;
				count[0] = 1;
			}
			showvals = (char **) malloc (count[0] * sizeof(char*));
			strcpy(buf, "");
			if(avi->GetValuesAsString(avi, showvals, start_index, count) == -1)
				ffError("Wrong index ('%d') or number of values ('%d')", start_index[0], count[0]);
			else {
				for(i=0; i<count[0]; i++){
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
		else if(strcmp(field, "output") == 0){
			count[0] = 1;
			ret_index[0] = -1;
			sprintf(locbuf, "%g", var->input);
			if (coordi != NULL) {
				ret_index[0] = coordi->GetIndexForInput(coordi, locbuf);	
				avi->GetValuesAsString(avi, vals, ret_index, count);	
				var->output = Atod(vals[0]);
				FreeString(vals[0]);
			}
		}
	}
	return 0;
	/* NOTREACHED */
  break;

  case UPDATE:
  case FLUSH:
	avi = (Abs_Variable *) var->avi;
	if(avi != NULL) {
		Element *child, *nextchild;
		int stat;
		for(child = var->child; child != NULL; child=nextchild) {
			nextchild = child->next;
			CallActionFunc(child, action);
		}
		stat = avi->Flush(avi);
		if (stat != -1) return 1;
	}	
	return 0;
	/* NOTREACHED */
  break;

  case DELETE:
	coordi = (Abs_Variable*) var->coordi;
	avi = (Abs_Variable *) var->avi;
	adi = (Abs_Diskio *) ParentDiskio_Interface(var);
	if(avi != NULL){ 
		avi->Destruct(avi);
		adi->FreeChildInterface(adi, avi);
	}
	if(coordi != NULL) {
		coordi->Destruct(coordi);
		free(coordi);
	}
  break;
	
#if 0
  case CLOSE:
	{
	/* Arrange to call DELETE on all metadata children */
	}
	avi->Delete(avi);
	avi->Destruct(avi);
	free(avi);
	if(var->values != NULL) FreeString(var->values);
  break;

  case RENAME:
	/*
	- var->avi->Rename(var->avi);
	- Self-destruct by calling the DELETE action
	- Recreate with new name (???)
	*/
  break;
#endif
 
  case RESET:
	if (1){
		float varrate = Clockrate(var);
		float diskiorate = Clockrate(parentdiskio);
		if(varrate != diskiorate){
			ffError("Clock rate (%e) of variable '%s[%d]' does not match that (%e) of parent diskio", 
					varrate, var->name, var->index, diskiorate);
			fprintf(stderr, "Data written to or retrieved from this variable may be inaccurate.\n");
		}
		if (!parentdiskio->append)
			var->start_time_index[0] = 0;
		coordi = (Abs_Variable *) var->coordi;
		if (coordi != NULL)
			coordi->ResetFastIndexForInput(coordi);
	}
  break;

  case PROCESS: 
	coordi = (Abs_Variable*) var->coordi;
	avi = (Abs_Variable *) var->avi;
	adi = (Abs_Diskio*) ParentDiskio_Interface(var);
	count[0] = 1;

	currenttime = SimulationTime();	
	var->input = currenttime; /* Set the input field every time step */

	inputmsgs = 0;
	MSGLOOP(var, msg){
		case 0:
			inputmsgs = 1;
			/* 
			 ** SAVE **
			*/
 			fval = MsgValue(msg, float, 0);
#ifndef HOLD_METADATA
			/*
			- Later: Get the "start" and "stop" attribute children values and 
			conditionally do a var->avi->SetValues(var->avi, &fval, var->start_time_index, count);				
			*/
#endif
			sprintf(buf, "%g", fval);
			vals[0] = buf;	
	
			if(((struct Gen_Diskio_Type *)var->parent)->is_writable && avi != NULL)
				avi->SetValuesFromString(avi, vals, var->start_time_index, count); 	
			var->output = fval;
			
		break;
	}

	/*
	 var->output is the value that is set from the input message if there are any,
	  Otherwise we will need to get the  appropriate value for the current time step
	*/ 

	/* 
	   The if conditions here optimize the performance a little bypassing the 
	   very inefficient searching of variable values (GetFastIndexForInput method)
	   only if there are no incoming messages and at least 1 outgoing message

  	   Also BC demands that any outgoing messages from the parent diskio will 
	   translate to retrieving values from the implicitly created variables and
	   and plopping them into its vals array. Hence the optimization condition
  	   checks for the presence of outgoing messages in the parent as well.
         */

	if(!inputmsgs && (var->nmsgout || parentdiskio->nmsgout)){
		sprintf(buf, "%g", currenttime);	
		ret_index[0] = -1;
		if (coordi != NULL) {
			ret_index[0] = coordi->FastGetIndexForInput(coordi, buf); 
			avi->GetValuesAsString(avi, vals, ret_index, count);
			var->output = Atod(vals[0]);
			FreeString(vals[0]);
		}
	}	

#if 0
	var->start_time_index[0]+=round(Clockrate(var)/
					(Clockrate(parentdiskio)/ClockValue(0)));
#endif

	var->start_time_index[0]++;

  break;

 }
 return 0;
}
 
