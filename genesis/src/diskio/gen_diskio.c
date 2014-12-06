/*********************************************************************

** This library uses the netcdf - version 3.4 which is
** Copyright 1993-1997 University Corporation for Atmospheric Research/Unidata

** The netcdf library is provided as per the terms of the
** UCAR/Unidata license.

** See interface/netcdf-3.4/copyright.html for the full notice.

********************************************************************/

/* $Id: gen_diskio.c,v 1.3 2005/07/20 20:01:58 svitak Exp $ */
/*
 * $Log: gen_diskio.c,v $
 * Revision 1.3  2005/07/20 20:01:58  svitak
 * Added standard header files needed by some architectures.
 *
 * Revision 1.2  2005/07/01 10:03:01  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:29  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.12  2001/06/29 21:13:24  mhucka
 * Added extra parens inside conditionals and { } groupings to quiet certain
 * compiler warnings.
 *
 * Revision 1.11  2001/04/18 22:39:36  mhucka
 * Miscellaneous portability fixes, mainly for SGI IRIX.
 *
 * Revision 1.10  2000/06/12 05:07:01  mhucka
 * Removed nested comments; added NOTREACHED comments where appropriate.
 *
 * Revision 1.9  1999/10/16 21:52:46  mhucka
 * Merged Upi's changes for supporting netCDF 3.4.  The changes in most files
 * only involve the copyright notice.  I also fixed up the comments slightly
 * to reflect the location of the actual netCDF copyright file.
 *
 * Revision 1.8  1999/10/16 21:23:47  mhucka
 * Venkat had a version of the diskio code in his home directory that he
 * appears to have been working on, and that did not make it into the
 * source tree that I had originally used as the starting point of my
 * GENESIS 2.2.2 DR reorganization effort.  In order to merge Venkat's
 * changes into the distribution, I'm reverting to his version, then will
 * update his version to incorporate recent changes by Upi for netCDF 3.4.
 *
 * Revision 1.6  1998/01/15 02:19:50  venkat
 * Inclusion-of-interface-headers-in-subdirectory-changed-to-be-not-hardcoded.-Now-this-is-taken-care-through-the-use-of-the-'-I'-flag-during-compilation-in-the-Makefile
 *
 * Revision 1.5  1998/01/14 00:38:32  venkat
 * Lots of changes to accomodate support for FMT1.
 *
 * Revision 1.4  1997/10/07 22:26:55  venkat
 * RESET action now creates the start_time and dt as global metadata children.
 * These can be used to obtain the header information.
 *
 * Revision 1.3  1997/08/04  04:18:19  dhb
 * include of strings.h does not work under Solaris.  Include
 * of string.h should suffice.
 *
 * Revision 1.2  1997/07/26 00:12:00  venkat
 * Added
 * */

#include <ctype.h>	/* isdigit() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>


#include <sys/time.h>
#include <sys/utsname.h>
#include <time.h>

#include "absff_ext.h"
#include "diskio_ext.h"

#ifndef NSUPPORTED_FORMATS
#define NSUPPORTED_FORMATS 2

#ifndef INVALID_FORMAT
#define INVALID_FORMAT -999

static char * Make_System_Info_String(/* struct utsname * */);
static void ffRecreateHierarchy(/* struct Gen_Diskio* */);
static int ffCreateMetadataAndForwardMsg(/* struct Gen_Diskio_Type*, MsgIn* */);
static int ffCreateVarAndForwardMsg(/* struct Gen_Diskio_Type*, MsgIn* */);
static int ffDetermineNewIndex(/* struct Gen_Diskio_Type *, char *name */);
static int ffAllocateFrame(/*struct Gen_Diskio_Type * */);
static int ffReadFrame(/*struct Gen_Diskio_Type * , long begin_index[]*/);
static int ffGet_autoSAVE_diskio_Framesize(/*struct Gen_Diskio_Type *, int *datasize*/);
static int ffSetUpInterpols(/*struct Gen_Diskio_Type * */);
static int ffCreateDefaultGlobalMetadata(/* struct Gen_Diskio_Type * */);


static FormatName2Number_Table name2number[] = {
	{"FMT1", 1}, 
	{"netcdf", 2}
};

static char * Make_System_Info_String(sysinf)
 struct utsname *sysinf;
{
 char *localbuf;
 int bufsize; 

 if(sysinf == NULL) return NULL; 

 bufsize = strlen(sysinf->sysname)+1;
 localbuf = (char *) malloc(bufsize * sizeof(char));
 strcpy(localbuf, sysinf->sysname);

 bufsize += strlen(sysinf->nodename)+1;
 localbuf = (char *) realloc(localbuf, bufsize);
 strcat(localbuf, sysinf->nodename);
 
 bufsize += strlen(sysinf->release)+1;
 localbuf = (char *) realloc(localbuf, bufsize);
 strcat(localbuf, sysinf->release);
  
 bufsize += strlen(sysinf->version)+1;
 localbuf = (char *) realloc(localbuf, bufsize);
 strcat(localbuf, sysinf->version);

 bufsize += strlen(sysinf->machine)+1;
 localbuf = (char *) realloc(localbuf, bufsize);
 strcat(localbuf, sysinf->machine);

 return localbuf;
}


int Gen_Diskio(diskio, action) 
  register struct Gen_Diskio_Type *diskio;
  Action *action;
{
  char *SelfMetadata_Interface();
  char *filename;
  char *formatname;
  char mode;
  int format, nname2number;
  
  register int i,j;
  int childsize;
  Abs_Type childtype;
  char buf[BUFSIZ];
  char localbuf[BUFSIZ];
  char timebuf[BUFSIZ];
  float currenttime, fval;
  char *vals[1];

  long count[1];
  
  char *field;
  char *value;


  Abs_Diskio *adi;
  Abs_Variable *coordi;

  MsgIn *msg;
  static int messages_changed = 0;
  static int messages_deleted = 0;

  char **av = action->argv;
  int ac = action->argc;



  SELECT_ACTION (action){

	case CREATE:
		{
		/* 
		   #########################################################
			BEGIN-SECTION 
		  This section parses the create command-line args and sets local variables
		   #########################################################
		*/	

			filename = NULL;
			formatname = NULL;
			mode = 'r';
			

			ac--, av++;	/*object type */
			ac--, av++;	/*path */

			for(i=0; i < ac ; i+=2){
				if(strcmp((av[i]+1), "filename") == 0)
					filename = CopyString(av[i+1]);
				if(strcmp((av[i]+1), "fileformat") == 0)
					formatname = CopyString(av[i+1]);
				if(strcmp((av[i]+1), "accessmode") == 0){
					mode = av[i+1][0]; 
					if (mode == 'a') {
						ffError("Mode '%c' is not yet supported \n", mode);
						fprintf(stderr, "Defaulting to read mode \n");
						mode = 'r';
					}
				}
			}
			if(formatname == NULL) formatname = CopyString(" ");
			if(filename == NULL) filename = CopyString(diskio->name);

		/* 
		   ############################################################
			 END-SECTION
		   ###########################################################
		*/

		}


		diskio->adi = (char *) Absff_CreateDiskio_Interface(filename, mode, &formatname); 
		adi = (Abs_Diskio *) diskio->adi;

			
		{
		/* 
		###########################################################
			BEGIN-SECTION
		##########################################################
		This section checks for a valid file format interface handle 
		and then attempts to replicate the hierarchy or overwrite 
		with default metadata depending on the mode with which the file
		is to be opened. 

		UGLY_SIGNPOST
		Additionally, it also constructs and maintains an interface to
		the default coordinate variable viz - 'SimulationTime'. Note that 
		this will not find place in the hierarchy. Individual variables also
		maintain their own coordinate variable interface which may or may 
		not be the default coordinate. However, the interface is maintained 
		seperately by the elements even in the case of an overlap. As far 
		as writing values into the coordinate variable is concerned, only the 	
		diskio element does it in the PROCESS action. 
		*/

		 if(adi != NULL){

#ifndef HOLD_METADATA 
			char *machine_info;
			struct utsname sysinfo;
#endif


			/* First set the object fields from the local variables */
			diskio->filename = CopyString(filename);
			FreeString(filename);
			diskio->fileformat = CopyString(formatname);
			FreeString(formatname);
			diskio->is_open = adi->is_open;
			diskio->is_writable = adi->is_writable;
			diskio->accessmode = mode;
			
				
			switch(mode){

			
				case 'r': /* Read-only or append to an existing file*/
				case 'R':
				case 'a':
				case 'A':
					ffRecreateHierarchy(diskio);
				break;

				case 'w': /* Overwrite the file with genesis data */
				case 'W':
#ifndef HOLD_METADATA 
				/*
				Create default global metadata children one-by-one with 
				the appropriate default arguments 	
				*/

				/** FIRST - ############################################## */

				/** The 'start_time' metadata element **/
				j = 0;	
				newargv[j] = "metadata"; j++;
				newargv[j] = "start_time"; j++;

				newargv[j] = "-values"; j++
				sprintf(timebuf, "%f", SimulationTime());
				newargv[j] = timebuf; j++;

				newargv[j] = "-size"; j++;
				newargv[j] = "1"; j++;

				newargv[j] = "-type"; j++;
				newargv[j] = "float32"; j++;

				newaction.argc = j;
				Create("metadata", "start_time", diskio, &newaction, 0);

				/** ############################################## */

				/** SECOND - ############################################## */

				/** The 'dt' metadata element **/
				j = 0;	
				newargv[j] = "metadata"; j++;
				newargv[j] = "dt"; j++;

				newargv[j] = "-values"; j++
				sprintf(timebuf, "%f", Clockrate(diskio));
				newargv[j] = timebuf; j++;

				newargv[j] = "-size"; j++;
				newargv[j] = "1"; j++;

				newargv[j] = "-type"; j++;
				newargv[j] = "float32"; j++;

				newaction.argc = j;
				Create("metadata", "Sampling_Rate", diskio, &newaction, 0);

				/** ############################################## */

				/** THIRD - ############################################## */

				/** The 'Machine_Info' metadata element **/
				j = 0;	
				newargv[j] = "metadata"; j++;
				newargv[j] = "Machine_Info"; j++;

				newargv[j] = "-values"; j++
				uname(&sysinfo);
				machine_info = Make_System_Info_String(sysinfo);
				newargv[j] = machine_info; j++;

				childsize = strlen(machine_info)+1; /* Only one float value */
				sprintf(buf, "%d", childsize);
				newargv[j] = "-size"; j++;
				newargv[j] = buf; j++;

				newargv[j] = "-type"; j++;
				newargv[j] = "char8"; j++;

				newaction.argc = j;
				Create("metadata", "Machine_Info", diskio, &newaction, 0);

				free(machine_info);

				/** ############################################## */
				
				/** FINAL - ############################################## */

				/** The 'Comment' metadata element **/
				j = 0;	
				newargv[j] = "metadata"; j++;
				newargv[j] = "Comment"; j++;

				newargv[j] = "-size"; j++;
#define COMMENT_SIZE 5000
				childsize = COMMENT_SIZE; 
#undef COMMENT_SIZE
				sprintf(buf, "%d", childsize);
				newargv[j] = buf; j++;

				newargv[j] = "-type"; j++;
				newargv[j] = "char8"; j++;

				newaction.argc = j;

				Create("metadata", "Comment", diskio, &newaction, 0);
				free(machine_info);

				/** ############################################## */
#endif
				break;
			} /* switch (mode) */

		/* Construct the default 'SimulationTime' coord_varible interface */

			diskio->coordi = (char*) adi->ConstructCoord_Interface(adi, "SimulationTime"); 
			return 1;

		} /* adi != NULL */	
		else {
			if(mode == 'r') 
				fprintf(stderr, "\nFile '%s' not found\n", filename);
			else
				fprintf(stderr, "\n\nCheck permissions and/or pathname for '%s'.\nAlso please check that the top-level Makefile macro settings \nfor compilation and linking of the '%s' libraries is uncommented.\n\n", filename, formatname);
			FreeString(filename);
			FreeString(formatname);
		}

		/* 
		   ############################################################
			 END-SECTION
		   ###########################################################
		*/

		}

		return 0;
		/* NOTREACHED */
	break;

	case SET:
		adi = (Abs_Diskio *) diskio->adi;
		coordi = (Abs_Variable *) diskio->coordi;
		
		if(ac == 2) {

			if (SetTable(diskio, 2, action->argv, action->data, "xpts ypts zpts"))
				return 1;
			
			field = av[0];
			value = av[1];

			if((strcmp(field, "filename") == 0) && 
			   (strcmp(value, diskio->filename) != 0)) {
				Element *child, *nextchild;


				/* Destruct the children, interfaces and all */
				for(child = diskio->child; child != NULL; child=nextchild) {
					nextchild = child->next;
					DeleteElement(child);	
				}
				if(coordi != NULL) {
					coordi->Destruct(coordi);
					free(coordi);	
					diskio->coordi = NULL;
				}
				adi->DestructChildInterfaces(adi);
				adi->Destruct(adi);
				free(adi);

				/* Recreate the interfaces with the new filename */
				diskio->adi = (char *)Absff_CreateDiskio_Interface(value, diskio->accessmode, &diskio->fileformat);
				adi = (Abs_Diskio *) diskio->adi;
				if(adi != NULL) { 
					FreeString(diskio->filename); 
					diskio->filename = CopyString(value);

					/* Reconstruct the default "SimulationTime" coordi interface */
					diskio->coordi = (char *) adi->ConstructCoord_Interface(adi, "SimulationTime");

					/* Recreate the new hierarchy */
					switch(diskio->accessmode) {
						case 'r':
						case 'R':
						case 'a':
						case 'A':
							ffRecreateHierarchy(diskio);
						break;
					}
				}
				else {
					ffError("Could not open file '%s' with mode '%c' \n",
							value, diskio->accessmode);
					fprintf(stderr, "Error setting field '%s' on object '%s'\n", 
							field, diskio->name);
				}
				return 1;
			}
			
			if (strcmp(field, "fileformat") == 0) {
				/* 
				   This is currently implemented to preserve 
				   BC with scripts using disk_in. Additional
				   things need to be done the interface side 

				   At this point I just do the setting if it 	
				   is specified and do not do anything to the 
				   file itself.
				*/
				int digi;
				int fmtnum, maxfmtnum;	
				int len = strlen(value);
				maxfmtnum = FF_Number(name2number);
				for(digi=0; digi < len; digi++) {
					if (! isdigit(value[digi])) 
						break;
				}
				if (digi == len){
					/* If it is a numerical format, we map it to the name*/
					fmtnum = atoi(value);
					if(! (fmtnum < 1 && fmtnum > maxfmtnum)) {
						if(diskio->fileformat != NULL) FreeString(diskio->fileformat);
						diskio->fileformat = CopyString((name2number[fmtnum-1]).name);
					}
				} 
				else {
					for(i=0; i<maxfmtnum; i++) {
						if(strcasecmp(value, name2number[i].name) == 0){
							if(diskio->fileformat != NULL) FreeString(diskio->fileformat);
							diskio->fileformat = CopyString(value);
						}
					}
				}	
				return 1;
			}

			if (strcmp(field, "append") == 0) {
  				short appendmode;
				appendmode = atoi(value); 
				adi->SetAppendMode(adi, appendmode);	 
			}
		}
	return 0;
	/* NOTREACHED */
	break;

	case ADDMSGIN:
		/* This action is primarily here to provide backward compatibility with	
		   the disk_out object that is capable of receiving SAVE messages

		   The method adopted here is to simply forward the message to a
		   newly created variable child and a newly created metadata child
		   that holds the position coords (x,y,z) of the msg->src. Since
		   this is in some sense implicit creation from the point of view
		   of the user, the name encoding of these child elements helps
		   in the easy retrieval of values without muddling the user's 
		   perspective .  */	

		/* 
		   The above functionality moved to the RESET action 
		   since this stuff needs to be 
		   done there anyway to accomodate any additions/deletions 	
		   of messages 
		*/

	/* 
			The static variable (to this function) "messages_changed" 
			is updated here and/or if a message is deleted so things can 
			be optimized a bit in the RESET action - The creation of 
			the implicit variables and metadata children takes place
			conditionally 
		*/
		
		/* For FMT1 we cannot add messages if RESET was called already */
		if (strcmp(diskio->fileformat, "FMT1") != 0 || !diskio->already_reset) {
			msg = (MsgIn*) (action->data);
			if(msg->type == 0) /* SAVE */
				messages_changed = 1;
		} else {
			fprintf(stderr, "Messages cannot be added to diskio element '%s' supporting fileformat '%s' after being reset.\n", diskio->name, diskio->fileformat);
		}
		
	break;

	case DELETEMSGIN:
		if (strcmp(diskio->fileformat, "FMT1") != 0 || !diskio->already_reset) {
			msg = (MsgIn*) (action->data);
			if(msg->type == 0) { /* SAVE */
		/* The static "messages_deleted" variable reference counts the 
		   number of deleted messages so the naming of the automatic variables 
		   and metadata can follow a policy of reuse. The deleted message data
		   will still exist in the file but will be flagged as inactive by 
		   a simple renaming of the element 
		*/
				messages_deleted++;	
				messages_changed = 1;
			}
		} else {
		/*
			fprintf(stderr, "Messages cannot be deleted from diskio element '%s' supporting fileformat '%s' after being reset.\n", diskio->name, diskio->fileformat);
		*/
		}
	break;

	case PROCESS: 
		adi = (Abs_Diskio *) diskio->adi;
		coordi = (Abs_Variable *) diskio->coordi;

        	sprintf(buf, "%g", diskio->currenttime);
		switch(diskio->accessmode){
			case 'r':
				if (diskio->nmsgout) {
				/* BC with disk_in - Read frame of values into val array */
					long frameindex[1];
					frameindex[0] = -1;

					if (coordi != NULL)
						frameindex[0] = coordi->FastGetIndexForInput(coordi, buf);
					else  
						/* If the coordi is NULL (e.g FMT1, we calculate the index sequentially based on the simulation time */ 
						frameindex[0] = (long) (int)((simulation_time - diskio->time_offset)/diskio->dt);
					if(ffReadFrame(diskio, frameindex) == -1) {
						ffError("Cannot read values from '%s'\n", diskio->filename);					
					}
				}
			break;

			case 'w':
			case 'a':
				if(diskio->filename == NULL)
					diskio->filename = CopyString(diskio->name);
				count[0] =1L;
				if (adi != NULL) {
					if (strcmp(diskio->fileformat, "FMT1") == 0) {
						int nframe = 0;

						if (!diskio->leave_open) {
							adi->CloseFile(adi); 
							if (adi->OpenFile(adi, diskio->filename, 'a') == -1) {
								diskio->is_open = adi->is_open;
								diskio->is_writable = adi->is_writable;
								return 0;
							
							}
						}
						MSGLOOP(diskio, msg) {
							case 0: /* SAVE */
							fval = MsgValue(msg, float, 0);
							adi->SetVariableValues(adi, nframe, &fval, diskio->start_time_index,  count);
							nframe++;
							break;
						}
					}
				}
			 	/* Write out the coordinate variable values every time step */
        			vals[0] = buf;
        			/* UGLY_SIGNPOST - Need to find out the name of the coord before doing this */
				if (coordi != NULL)
                			coordi->SetValuesFromString(coordi, vals, diskio->start_time_index, count);
				if (diskio->flush && adi != NULL)
					adi->Flush(adi);
			break;
		}
#if 0
		diskio->start_time_index[0]+=round(Clockrate(diskio)/ClockValue(0));
#endif
		diskio->start_time_index[0]++;
        	diskio->currenttime += Clockrate(diskio);
		
	break;

	case RESET:
		adi = (Abs_Diskio*) diskio->adi;
		if (adi == NULL)
			return 0;
		coordi = (Abs_Variable *) diskio->coordi;

		if(diskio->accessmode == 'r') {
		/** 
		    This section of code deals primarily with BC issues with 
		    disk_in. The frames are allocated if necessary and 
		    ready to hold data on file. The nx, ny, vals fields  and the 
		    array to hold the pointers to the autoSAVE_diskio variable
		    interface structures are allocated/initialized in the 
		    ffAllocateFrame() routine. 

		    The interpol fields and the array of metadata interface 
		    pointers is also allocated if necessary to read in the 
		    3-D position coords into the interpols.
		    
		    The index of the coord variable
		    starting from which values will be searched for the 
		    appropriate frame of data to be read in, is also reset.
		**/
			if (coordi != NULL) {
				long start_index[1], count[1];
				char *vals[2];
				start_index[0] = 0L; count[0] = 2L;
				coordi->GetValuesAsString(coordi, vals, start_index, count);
				diskio->start_time = Atof(vals[0]);
				diskio->dt = Atof(vals[1]) - Atof(vals[0]);
				FreeString(vals[0]);
				FreeString(vals[1]);
				/* 
				  Get the datatype also from the SAVEavi variable interface
				  diskio->datatype = ?? 
				*/
				diskio->datatype = FLOAT;
				coordi->ResetFastIndexForInput(coordi);		
			}
			else {
				/* All the FMT1 header info is implemented 
				 now as diskio fields and not metadata */
				diskio->start_time = adi->GetStartTime(adi);
				diskio->dt = adi->GetDt(adi);
				diskio->datatype = adi->GetDataType(adi);
			}



			if (! diskio->valsallocated) {
				int status;
				/* Note: This routine (ffAllocateFrame) sets the nx, ny fields */
				if ((status = ffAllocateFrame(diskio)))
					status = ffSetUpInterpols(diskio);
				if (status)
					diskio->valsallocated = 1;
			}
			diskio->start_time_index[0] = 0;
		} else if(diskio->accessmode == 'w') {
			int framesize;	
			Element *child, *nextchild;

			if (!diskio->append) {
				int deleted;
				int otherchildrenexist = 0;
				char openmode;
				for(child = diskio->child; child != NULL; child=nextchild) {
					deleted = 0;
					nextchild = child->next;
					if(strcmp(BaseObject(child)->name, "metadata")==0) {
						if(strcmp(child->name, "auto3-DPosSAVE_diskio") == 0) {
							DeleteElement(child);	
							deleted = 1;
						} else
							otherchildrenexist = 1;
					}
					if(!deleted && strcmp(BaseObject(child)->name, "variable")==0) {
						if(strcmp(child->name, "autoSAVE_diskio") == 0) {
							DeleteElement(child);	
						} else
							otherchildrenexist = 1;
					}
				}
				adi->DestructChildInterfaces(adi);
				if(coordi != NULL) {
					coordi->Destruct(coordi);
					free(coordi);
				}
				if (diskio->is_open) {
					adi->ResetFile(adi);
					adi->CloseFile(adi);
					diskio->is_open = diskio->is_writable = 0;
				}
				/* 
				 If other children exist : Only possible in 
				 the case of self-describing file formats 
				 such as netcdf. 
				
				 In such a situation we avoid clobbering 
				 up the file by reopening it in 'w' mode.
				 We use 'a' mode instead.
				*/

				if (otherchildrenexist) 
					openmode = 'a';
				else
					openmode = diskio->accessmode;
				if(adi->OpenFile(adi, diskio->filename, openmode) != -1){

					/* Reconstruct the default "SimulationTime" coordi interface */
					diskio->is_open = adi->is_open;
					diskio->is_writable = adi->is_writable;
					diskio->coordi = (char *) adi->ConstructCoord_Interface(adi, "SimulationTime");

					diskio->start_time_index[0] = 0;
					messages_changed = 1;

				}
			}
			diskio->start_time = SimulationTime();
			diskio->dt = Clockrate(diskio);
			diskio->datatype = FLOAT; /* should be user-settable */
			framesize = 0;
			MSGLOOP(diskio, msg) {

				case 0: /* SAVE */
					framesize++;
				break;
			}
			adi->BufferLabel(adi, diskio->fileformat);
			adi->BufferStartTime(adi, diskio->start_time);
			adi->BufferDt(adi, diskio->dt);	
			adi->BufferFrameSize(adi, framesize);
			adi->BufferDatatype(adi, diskio->datatype);


			while(messages_deleted) {
				int newindex, lastindex, deleted;
				Element *child, *nextchild;
				newindex = ffDetermineNewIndex(diskio, "auto3-DPosSAVE_diskio");
				lastindex = newindex - 1;
				for(child=diskio->child; child != NULL; child=nextchild) {
					nextchild=child->next;
					deleted = 0;
					if ((strcmp(BaseObject(child)->name, "metadata")==0) &&
						(strcmp(child->name, "auto3-DPosSAVE_diskio") == 0) &&
							(child->index == lastindex)) {
						Abs_Metadata * SAVEamdi = (Abs_Metadata *) SelfMetadata_Interface(child);
						SAVEamdi->Rename(SAVEamdi, "autoinactiveSAVE_diskio");
						DeleteElement(child);
						deleted = 1;
					}
					if(!deleted && 
						((strcmp(BaseObject(child)->name, "variable")==0) &&
       	                                		(strcmp(child->name, "autoSAVE_diskio") == 0) &&
       	                                        		(child->index == lastindex))) {
						Abs_Variable * SAVEavi = (Abs_Variable *) SelfVariable_Interface(child);
						SAVEavi->Rename(SAVEavi, "autoinactiveSAVE_diskio");
						DeleteElement(child);
					}
					
				}
				messages_deleted--;
			}

			if(messages_changed) {
				Element *child, *nextchild;
				int deleted;
				register int k;
				float coord[3];
				for(child = diskio->child; child != NULL; child=nextchild) {
					nextchild = child->next;
					deleted = 0;
					if(strcmp(BaseObject(child)->name, "metadata")==0) {
						if(strcmp(child->name, "auto3-DPosSAVE_diskio") == 0) {
							DeleteElement(child);	
							deleted = 1;
						}
					}
					if(!deleted && strcmp(BaseObject(child)->name, "variable")==0) {
						if(strcmp(child->name, "autoSAVE_diskio") == 0) {
							DeleteElement(child);	
						}
					}
				}
				framesize = 0;
				MSGLOOP(diskio, msg) {

					case 0: /* SAVE */
						if (! ffCreateMetadataAndForwardMsg(diskio, msg))
							return 0;	
						if (strcmp(diskio->fileformat, "FMT1") == 0) {

							coord[0] = msg->src->x;
							coord[1] = msg->src->y;
							coord[2] = msg->src->z;

							adi->SetMetadataValues(adi, framesize, coord, 0L, (long) 3);

						}
						if (! ffCreateVarAndForwardMsg(diskio, msg))
							return 0;

						framesize++;
					break;
				}
	

				/* 
				   HACK ? The newly created autoSAVE children need to be 
				   rescheduled 
				*/
				Reschedule();

				messages_changed = 0;
			}
		}
		diskio->header_size = adi->GetHeaderSize(adi);

		if (!diskio->already_reset)
			diskio->already_reset = 1;

		diskio->currenttime = 0.0;
	break;

	case SHOW:
		if (ShowInterpol(action, "xpts", diskio->xpts, diskio->valsallocated, 50)) return 1;
		if (ShowInterpol(action, "ypts", diskio->ypts,diskio->valsallocated,  50)) return 1;
		return (ShowInterpol(action, "zpts", diskio->zpts,diskio->valsallocated,  50));
		/* NOTREACHED */
	break;
	
	case QUIT:
		adi = (Abs_Diskio *) diskio->adi;
		if((diskio->accessmode == 'w'  || diskio->accessmode == 'a' )&& adi != NULL) {
			Element *child, *nextchild;
			int stat = -1;
			static Action forwaction = {"UPDATE", UPDATE, NULL, 0, NULL, NULL};
	
			/* We write the variable followed by metadata for 
			 the same reason explained in the RESET action*/
			for(child=diskio->child; child != NULL; child=nextchild) {
				CallActionFunc(child, &forwaction);
				nextchild=child->next;
			}
			if (adi != NULL)
				stat = adi->Flush(adi);
			if(stat != -1) return 1;
		}
		return 0;
		/* NOTREACHED */
	break;

	case UPDATE:
	case FLUSH:
		adi = (Abs_Diskio *) diskio->adi;
		if((diskio->accessmode == 'w' || diskio->accessmode == 'a')&& adi != NULL) {
			Element *child, *nextchild;
			int stat = -1;
			for(child=diskio->child; child != NULL; child=nextchild) {
				CallActionFunc(child, action);
				nextchild=child->next;
			}
			if (adi != NULL)
				stat = adi->Flush(adi);
			if(stat != -1) return 1;
		}
		return 0;
		/* NOTREACHED */
	break;

	case DELETE:
		adi = (Abs_Diskio *) diskio->adi;
		coordi = (Abs_Variable *) diskio->coordi;
		if (adi != NULL)
			adi->Flush(adi);
#ifndef HOLD_METADATA
		{
			struct timezone tz;
			struct timeval tv;

		 if(diskio->accessmode == 'w'){
			newaction.type = CREATE;
			newaction.name = "CREATE";

			/** LAST - ############################################## */

			/** The 'CreationTime' metadata element **/
			j = 0;	
			newargv[j] = "metadata"; j++;
			newargv[j] = "CreationTime"; j++;
			newargv[j] = "-values"; j++
			gettimeofday(&tv, &tz);
			strcpy(timebuf, ctime((time_t*) &tv.tv_sec));	
			newargv[j] = timebuf; j++;
			childsize = strlen(timebuf) + 1; /* Array of chars including '\0' */
			sprintf(buf, "%d", childsize);
			newargv[j] = "-size"; j++;
			newargv[j] = buf; j++;
			newargv[j] = "-type"; j++;
			newargv[j] = "char8"; j++;

			newaction.argc = j;
			Create("metadata", "CreationTime", diskio, &newaction, 0);

				/** ############################################## */
		 }

		}				
#endif		
		if(diskio->filename != NULL)
			FreeString(diskio->filename);
		if(diskio->fileformat != NULL)
			FreeString(diskio->fileformat);
		if(coordi != NULL) {
			coordi->Destruct(coordi);
			free(coordi);
		}
		if(adi != NULL) {
			adi->DestructChildInterfaces(adi);
			adi->Destruct(adi);
			free(adi);
		}
		
		if(diskio->val != NULL) {
			for(i=0; i<diskio->nx; i++) {
				if(diskio->val[i] != NULL)
					free(diskio->val[i]);
			}
			free(diskio->val);
		}

		if(diskio->autoSAVEavi != NULL)
			free(diskio->autoSAVEavi);
		if(diskio->autoSAVEamdi != NULL)
			free(diskio->autoSAVEamdi);

	break;	

  }
  return 0;	
}

static int ffReadFrame(diskio, startindex)
 register struct Gen_Diskio_Type *diskio;
 long startindex[];
{
  int status;
  Abs_Variable **SAVEavi;
  int framesize,i, j;
  long count[1];

  status = 1;

  if(!diskio->valsallocated)
	status = ffAllocateFrame(diskio);
  if(!status) {
	ffError("Cannot allocate frames for '%s'\n", diskio->filename);
	return 0;
  }
 
   SAVEavi = (Abs_Variable **) diskio->autoSAVEavi;
   count[0] = 1;
   framesize = diskio->ny;
   for(i=0; i<diskio->nx; i++) {   
   	for(j=0; j<framesize; j++)
		status = SAVEavi[j]->GetValues(SAVEavi[j], &diskio->val[i][j], startindex, count); 
   }

   return status;
} 

static int ffAllocateFrame(diskio)
 register struct Gen_Diskio_Type *diskio;
{
  register int i;
  int datasize; 

  if(diskio == NULL) return 0;

  diskio->nx = 1; /* Only 1 frame per read */
  diskio->ny = ffGet_autoSAVE_diskio_Framesize(diskio, &datasize);

  if(! diskio->ny)
	return 0;

  if(diskio->val != NULL) {
  	for(i=0; i<diskio->nx; i++){
		if(diskio->val[i] != NULL) 
			free(diskio->val[i]);
		diskio->val[i] = NULL;
	}
	free(diskio->val);
	diskio->val = NULL;
  }
  diskio->val = (float **) calloc(diskio->nx, sizeof(float*));
  for(i=0; i<diskio->nx; i++)
	diskio->val[i] = (float *) calloc(diskio->ny, datasize);

  return 1; 
}

static int ffGet_autoSAVE_diskio_Framesize(diskio, datasize)
 register struct Gen_Diskio_Type *diskio;
 int *datasize;
{
  int framesize;
  Element *child;
  Abs_Variable **SAVEavi = NULL;

  if(diskio->autoSAVEavi != NULL) {
	free(diskio->autoSAVEavi);
	diskio->autoSAVEavi = NULL;
  }

  framesize = 0;
  for(child=diskio->child; child != NULL; child=child->next) {
	if ((strcmp(BaseObject(child)->name, "variable")==0) &&
	    	(strcmp(child->name, "autoSAVE_diskio")==0)) {	

		framesize++;
  		if(framesize == 1)
  			diskio->autoSAVEavi = (char **) calloc(framesize, sizeof(Abs_Variable *));
		else
  			diskio->autoSAVEavi = (char **) realloc(diskio->autoSAVEavi, framesize*sizeof(Abs_Variable *));
		SAVEavi = (Abs_Variable **) diskio->autoSAVEavi;
		SAVEavi[framesize -1] = (Abs_Variable *) SelfVariable_Interface(child);
 
	}
  } 

  if (framesize > 0 )
	*datasize = SAVEavi[framesize - 1]->GetTypeSize(SAVEavi[framesize - 1]);	
  
  return framesize;
}

static int ffCreateVarAndForwardMsg(diskio, msg)
 register struct Gen_Diskio_Type *diskio;
 MsgIn *msg;
{
#define MAX_CREATEARGS 20

	char *newargv[50];
	Action newaction;
	char *targv[MAX_CREATEARGS];
	register int i;

	static char varname[80];
	int varindex;
	char localbuf[BUFSIZ];
	char *ptr;
	
	Element *srcelement;

	Element *varchild;


	if(!(msg->type == 0)) 
			/* SAVE message only */
		return 0;

	/** 
	  The coding of the implicitly created variable name:
		"autoSAVE_diskio"
	  If there is more than 1 SAVE message , the 
	  variable will be indexed appropriately.
	
	Entails the definition of a new function (in the base code) that
	returns the name of the field given its address, which is what 
	the msg slot contains in this case (SAVE).
	**/

	strcpy(varname, "autoSAVE_diskio");
	/*
	if((ptr = strchr(varname, '[')) != NULL)
		*ptr = '\0';
	srcelement = msg->src;
	strcat(varname, "OF");
	strcat(varname, srcelement->name);	
	*/

	/* Set the works for the CREATE action call */

	i=0;
	targv[i] = varname; i++;
	targv[i] = "variable"; i++;
	targv[i] = "-datatype"; i++;
	targv[i] = "float32"; i++;
	newaction.argc = i;
	newaction.argv = targv;
	newaction.name = "CREATE";
	newaction.type = CREATE;
	newaction.data = (char *)diskio;

	/* Determine a unique index */
	varindex = ffDetermineNewIndex(diskio, varname);			

	/* Create the child variable */
	if ((varchild = Create("variable", varname, diskio, &newaction, varindex)) == NULL)
		return 0; 
	
	if (strcmp(diskio->fileformat, "FMT1") == 0) {
		Abs_Diskio *adi = (Abs_Diskio *)diskio->adi;
		Abs_Variable *avi = (Abs_Variable *)SelfVariable_Interface(varchild);
		
		adi->SetVariableInterface(adi, varindex, avi);
	/* If FMT1, make the fields of the element ro and hidden */
		return ffProtectVariableFields(varchild);
	}

	/* Forward the message to the child variable if the 
	   underlying format supports the notion of named variables */
	AddForwMsg(varchild, msg->src, msg->type, MSGSLOT(msg), msg->nslots);

	return 1;

#undef MAX_CREATEARGS 
}

static int ffSetUpInterpols(diskio)
 register struct Gen_Diskio_Type *diskio;
{
        char *SelfMetadata_Interface();
	int framesize;
	Element *child;
	Abs_Metadata **SAVEamdi;
	register int i;

	float posvals[3];
	long start_index, count;
	int ffstatus;
	
	struct Gen_Metadata_Type *md;

	Interpol *realloc_interpol();

	if(diskio->autoSAVEamdi != NULL) {
		free(diskio->autoSAVEamdi);
		diskio->autoSAVEamdi = NULL;
  	}

  	framesize = diskio->ny;
	
	if(framesize < 1) return 0;

  	diskio->autoSAVEamdi = (char **) calloc(framesize, sizeof(Abs_Variable *));
  	SAVEamdi = (Abs_Metadata **) diskio->autoSAVEamdi;
  	for(i=0, child=diskio->child; child != NULL; child=child->next) {
		if ((strcmp(BaseObject(child)->name, "metadata")==0) &&
	    		(strcmp(child->name, "auto3-DPosSAVE_diskio")==0)) {	
  				SAVEamdi[i] = (Abs_Metadata *) SelfMetadata_Interface(child);
				i++;
		} 
	}
 	if(i != framesize) return 0; 

	/* allocate the interpols if necessary */
	diskio->xpts = realloc_interpol(diskio->xpts, framesize);
	diskio->ypts = realloc_interpol(diskio->ypts, framesize);
	diskio->zpts = realloc_interpol(diskio->zpts, framesize);

	/* Fill up the interpols with the position coords one-by-one */
	start_index = (long) 0;
	count = (long) 3;
	for(i=0; i<framesize; i++) {
		ffstatus = SAVEamdi[i]->GetValues(SAVEamdi[i], posvals, start_index, count);	
		if(ffstatus == -1) return 0; 
			
		diskio->xpts->table[i] = posvals[0];	
		diskio->ypts->table[i] = posvals[1];	
		diskio->zpts->table[i] = posvals[2];	
	}

  	return framesize;
}

static int ffCreateDefaultGlobalMetadata(diskio) 
 register struct Gen_Diskio_Type *diskio;	
{
	Action newaction;
	char timebuf[BUFSIZ];
	char *newargv[20];
	int j;
	Element *child;
	int headerexists = 0;

	for (child = diskio->child; child != NULL; child=child->next) {
		if((strcmp(BaseObject(child)->name, "metadata") == 0) &&
			(strcmp(child->name, "start_time") == 0))
			headerexists = 1;
	}

	if (!headerexists) {
		newaction.name = "CREATE";
		newaction.type = CREATE;
		newaction.data = (char *) diskio;
	/*
	Create default global metadata children one-by-one with 
	the appropriate default arguments 	
	*/

	/** FIRST - ############################################## */

	/** The 'start_time' metadata element **/
	j = 0;	
	newargv[j] = "metadata"; j++;
	newargv[j] = "start_time"; j++;

	newargv[j] = "-values"; j++;
	sprintf(timebuf, "%f", SimulationTime());
	newargv[j] = timebuf; j++;

	newargv[j] = "-size"; j++;
	newargv[j] = "1"; j++;

	newargv[j] = "-type"; j++;
	newargv[j] = "float32"; j++;

	newaction.argc = j;
	newaction.argv = newargv;
	if (Create("metadata", "start_time", diskio, &newaction, 0) == NULL)
		return 0;

		/** ############################################## */

		/** SECOND - ############################################## */

		/** The 'dt' metadata element **/
		j = 0;	
		newargv[j] = "metadata"; j++;
		newargv[j] = "dt"; j++;

		newargv[j] = "-values"; j++;
		sprintf(timebuf, "%f", Clockrate(diskio));
		newargv[j] = timebuf; j++;

		newargv[j] = "-size"; j++;
		newargv[j] = "1"; j++;

		newargv[j] = "-type"; j++;
		newargv[j] = "float32"; j++;

		newaction.argc = j;
		if (Create("metadata", "dt", diskio, &newaction, 0) == NULL)
			return 0;
	}
	
	return 1;
}

static int ffCreateMetadataAndForwardMsg(diskio, msg)
 register struct Gen_Diskio_Type *diskio;
 MsgIn *msg;
{
#define MAX_CREATEARGS 20

        char *SelfMetadata_Interface();
	char *newargv[50];
	Action newaction;
	char *targv[MAX_CREATEARGS];
	register int i;

	static char mdname[80];
	int mdindex;
	char localbuf[BUFSIZ];
	char *ptr;
	
	Element *srcelement;

	Element *mdchild;


	if(!(msg->type == 0)) 
			/* SAVE message only */
		return 0;

	/** 
	  The coding of the implicitly created metadata name:
		"auto3-DPosSAVE_diskio"
	  If there is more than 1 SAVE message , the 
	  metadata will be indexed appropriately.
	**/

	strcpy(mdname, "auto3-DPosSAVE_diskio");

	/* Set the works for the CREATE action call */

	i=0;
	targv[i] = mdname; i++;
	targv[i] = "metadata"; i++;
	targv[i] = "-datatype"; i++;
	targv[i] = "float32"; i++;
	targv[i] = "-size"; i++;
	targv[i] = "3"; i++;
	newaction.argc = i;
	newaction.argv = targv;
	newaction.name = "CREATE";
	newaction.type = CREATE;
	newaction.data = (char *)diskio;

	/* Determine a unique index */
	mdindex = ffDetermineNewIndex(diskio, mdname);			

	/* Create the child variable */
	if ((mdchild = Create("metadata", mdname, diskio, &newaction, mdindex)) == NULL)
		return 0; 

	if (strcmp(diskio->fileformat, "FMT1") == 0) {
		Abs_Diskio * adi = (Abs_Diskio *) diskio->adi;		
		Abs_Metadata *amdi = (Abs_Metadata *)SelfMetadata_Interface(mdchild);

		adi->SetMetadataInterface(adi, mdindex, amdi);
	/* If FMT1, make the fields of the element ro */
		return ffProtectMetadataFields(mdchild);
	}
	
	/* 
	Forward the message to the child metadata element
	*/
	AddForwMsg(mdchild, msg->src, msg->type, MSGSLOT(msg), msg->nslots);

	return 1;

#undef MAX_CREATEARGS 
}


static int ffDetermineNewIndex(diskio, childname)
 register struct Gen_Diskio_Type *diskio;
 char *childname;
{
   Element *child;
        int i,j;
        int     childindex[500];
        int nnames; 

	nnames = 0;
        for(child = diskio->child; child != NULL; child = child->next) {
                if (strcmp(child->name,childname) == 0) {
                        childindex[nnames] = child->index;
                        nnames++;
                }
        }
        /* look for an unused index, starting at nnames to keep it
        ** simple */
        for(i = nnames; ; i++) {
                for(j = 0; j < nnames; j++) {
                        if (childindex[j] == i) break;
                }
                if (j >= nnames) return(i);
        }

}

static void ffRecreateHierarchy(diskio)
 register struct Gen_Diskio_Type *diskio;
{
        char *SelfMetadata_Interface();
  	char *newargv[50];
  	Action newaction; 
	int ngmd, nvar;
	register int i;

  	Abs_Diskio * adi = (Abs_Diskio *) diskio->adi;
	if (adi == NULL) return;

  	newaction.argv = newargv;
	/* 
	Please note that the file to be updated may be
	a non-genesis created file in which case we dont
	want to write out the default metadata & variables 
	So append and read have the same semantics as far as 
	this CREATE action goes
	*/
	newaction.type = CREATE;
	newaction.name = "CREATE";
	newaction.argc = 0;
	if(adi->nglobmetadata > 0){
	 	char *mdname;					
		int mdindex;
	  /*
	   Create the global metadata children one-by-one with no arguments
	   The CREATE action in the child (metadata) element will 
	   do the job of querying the file through its interface constructor,
	   and setting its fields subsequently 
	  */
		ngmd = adi->nglobmetadata;
		for(i=0; i<ngmd; i++){
			Element *mdchild;
			mdname = adi->RecreateNameAndIndex(adi, adi->globmetadatanames[i], &mdindex);	
			if(strcmp(mdname, "autoinactiveSAVE_diskio")==0) {
				FreeString(mdname);
				continue;
			}
			mdchild = Create("metadata", mdname, diskio, &newaction, mdindex);	
			if (strcmp(diskio->fileformat, "FMT1") == 0) {
				Abs_Metadata *amdi;
				amdi = (Abs_Metadata *) SelfMetadata_Interface(mdchild);	
				adi->SetMetadataInterface(adi, mdindex, amdi);
				ffProtectMetadataFields(mdchild);
			}
			FreeString(mdname);
		}
	}
	/* 
	   Create the variable children one-by-one with default arguments. 
		
	*/

	if(adi->nvariables > 0){
		char *varname;
		int varindex;
		Abs_Variable *avi;
		nvar = adi->nvariables;
		for(i=0; i<nvar; i++){
			Element *varchild;
			varname = adi->RecreateNameAndIndex(adi, adi->variablenames[i], &varindex);	
			if (strcmp(varname, "autoinactiveSAVE_diskio") == 0) {
				FreeString(varname);
				continue;
			}
			varchild = Create("variable", varname, diskio, &newaction, varindex);	
			if (strcmp(diskio->fileformat, "FMT1") == 0) {
				avi = (Abs_Variable *) SelfVariable_Interface(varchild);	
				adi->SetVariableInterface(adi, varindex, avi);
				ffProtectVariableFields(varchild);
			}
			FreeString(varname);
		}
		adi->FillAllVariableValues(adi);
  	}
}

#undef INVALID_FORMAT
#endif

#undef NSUPPORTED_FORMATS
#endif
