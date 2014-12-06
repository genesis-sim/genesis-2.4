#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "absff_ext.h"

#ifndef NSUPPORTED_FORMATS
#define NSUPPORTED_FORMATS 2

 Abs_Diskio* ((*Absff_ConstructDiskio_Interface[NSUPPORTED_FORMATS])(/* char *filename, char mode */)) = {
#if defined(FMT1)
        FMT1_ConstructFile,
#else
	Absff_ConstructDiskio,
#endif
#if defined(netcdf)
        Netcdf_ConstructFile
#else
	Absff_ConstructDiskio
#endif
 };

int ((*Absff_Format_IsA[NSUPPORTED_FORMATS])(/* char *filename, char mode */)) = {
#if defined(FMT1)
        FMT1_IsA,
#else
	Absff_IsA,
#endif
#if defined(netcdf)
        Netcdf_IsA
#else
	Absff_IsA
#endif
};
 

typedef struct {
        char *name;
        int number;
} FormatName2Number_Table;

#define FF_Number(arr) ((int) (sizeof(arr) / sizeof(arr[0])))

static FormatName2Number_Table name2number[] = {
        {"FMT1", 0},
        {"netcdf", 1}
};

int Absff_MapToConcreteFormat(formatname)
 char *formatname;
{
#define INVALID_FORMAT -1

  register int i;
  int format;
  int nname2number = FF_Number(name2number);

  if (formatname == NULL)
	return INVALID_FORMAT;

  for(i=0; i<nname2number; i++) {
	if(strcasecmp(formatname, name2number[i].name) == 0) {
		format = name2number[i].number;
		break;
	}
  }
  if(i == nname2number) {
	format = INVALID_FORMAT;
	fprintf(stderr, "The format '%s' is not supported by the interface \n",
				formatname);
  }

  return format;

#undef INVALID_FORMAT
}

 /** 
     This is the wrapper that runs thro the list 
     of subclass isA() methods to identify the format from the file. 
 **/

int Absff_DeduceFormatInterface(filename) 
 char *filename;
{
  register int i;

  int nname2number = FF_Number(name2number);
  int formatid = -1;

  fprintf(stderr, "Attempting to deduce format from file '%s' ...\n", filename);
  for (i=0; i<nname2number; i++) {
	if (Absff_Format_IsA[i](filename)) 
		break;
  }

  formatid = i;

  if (formatid == nname2number)
	return -1;
 
  return formatid;
}

Abs_Diskio * Absff_CreateDiskio_Interface(filename, accessmode, formatname)
 char *filename;
 char accessmode;
 char **formatname;
{
 Abs_Diskio *adi = NULL;
 int formatnumber = -1;

  if (formatname == NULL)  {
	fprintf(stderr, "The interface creation method requires a reference \
		to valid allocated array of pointer to char \n");
	return NULL;
  }

 formatnumber = Absff_MapToConcreteFormat(*formatname);

 if (formatnumber != -1) {
	adi = Absff_ConstructDiskio_Interface[formatnumber](filename, accessmode);
	if (adi == NULL) {
		formatnumber = Absff_DeduceFormatInterface(filename);	
		if (formatnumber == -1) {
			fprintf(stderr, "Failed to open file '%s' in any of the supported formats \n", filename);
			return NULL;
		}
		adi = Absff_ConstructDiskio_Interface[formatnumber](filename, accessmode);
	}
 } else {
	switch(accessmode) {
		case 'r':
		case 'a':
			formatnumber = Absff_DeduceFormatInterface(filename);
			if (formatnumber == -1) {
				fprintf(stderr, "Failed to open file '%s' in any of the supported formats \n", filename);
				return NULL;
			}
			adi = Absff_ConstructDiskio_Interface[formatnumber](filename, accessmode);
		break;
		
		case 'w':
			formatnumber = 0;
			fprintf(stderr, "Defaulting to open file '%s' as '%s' in mode '%c'\n", filename, name2number[formatnumber].name, accessmode);
			adi = Absff_ConstructDiskio_Interface[formatnumber](filename, accessmode);
		break;

		default:
			/* Unknown mode */
			break;
 	}
 }

 Absff_FreeString(*formatname);	
 *formatname = Absff_CopyString(name2number[formatnumber].name);

 return adi;
}

/* static (reflection) IsA method: Returns 0 (false) for the abstract case */
int Absff_IsA(filename)
 char *filename;
{
 fprintf(stderr, "Abstract diskio reflection method called\n");
 fprintf(stderr, "May need to check that appropriate file format library for '%s' \n is compiled in.\n", filename);
 return 0;
}

Abs_Diskio * Absff_ConstructDiskio(filename, accessmode)
 char *filename;
 char accessmode;
{
/* Abstract factory will not instantiate */
 fprintf(stderr, "Abstract diskio factory method called\n");
 fprintf(stderr, "May need to check that appropriate file format library for '%s' \n is compiled in.\n", filename);
 return NULL;
}

int Absff_DestructDiskio(adi)
 Abs_Diskio *adi;
{
 register int iattr, ivar, i;

 if(adi->nglobmetadata > 0){
	for(iattr=0; iattr<adi->nglobmetadata; iattr++)
		if(adi->globmetadatanames[iattr] != NULL) free(adi->globmetadatanames[iattr]);
	free(adi->globmetadatanames);
	adi->globmetadatanames = NULL;
	adi->nglobmetadata = 0;
 }

 if(adi->nvariables > 0){
	for(ivar=0; ivar<adi->nvariables; ivar++)
		if(adi->variablenames[ivar] != NULL) free(adi->variablenames[ivar]);
	free(adi->variablenames);
	adi->variablenames = NULL;
	adi->nvariables = 0;
 }


 Absff_FreeString(adi->filename);
 adi->filename = NULL;
}

#undef NSUPPORTED_FORMATS
#endif
