static char rcsid[] = "$Id: shell_lib.c,v 1.3 2005/10/16 20:48:35 svitak Exp $";

/*
** $Log: shell_lib.c,v $
** Revision 1.3  2005/10/16 20:48:35  svitak
** Changed hard-coded character arrays from 100 to 256 or malloc'd and freed
** memory instead. This was only done where directory paths might have easily
** exceeded 100 characters.
**
** Revision 1.2  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:04:35  dhb
** Initial revision
**
*/

#include "shell_ext.h"

void do_load_lib(argc,argv)
int argc;
char **argv;
{
char	*name;
char 	libname[80];
PFI 	func;

    if(argc < 2){
	printf("usage: %s library_name\n",argv[0]);
	return;
    }
    /* 
    ** get the base name for the library of symbol table components 
    */
    name = argv[1];
    /* 
    ** load in the data structure components of the symbol table 
    */
    sprintf(libname,"LIBRARY_%s",name);
    /* 
    ** look up the address of the function 
    */
    if((func=(PFI)GetFuncAddress(libname))== NULL){
	/*
	** try and find the component libraries
	*/
	sprintf(libname,"DATA_%s",name);
	if((func=(PFI)GetFuncAddress(libname))== NULL){
		if (IsSilent() < 2) {
	    	Warning();
	    	printf("can't find data structure library '%s'\n",name);
		}
	} else {
	    func();
	}
	sprintf(libname,"FUNC_%s",name);
	if((func=(PFI)GetFuncAddress(libname))== NULL){
		if (IsSilent() < 2) {
	    	Warning();
	    	printf("can't find function library '%s'\n",name);
		}
	} else {
	    func();
	}
    } else {
	/*
	** and call the function to fill the symbol table entries
	*/
	func();
    }
}

void LibraryHeader(lib,date)
char *lib;
char *date;
{
char libstr[256];
	if(IsSilent() < 0) {
    	sprintf(libstr,"%s %s",lib,"library");
    	printf("Loading.. %-20s : updated %-8s\n",libstr,date);
	}
}
