static char rcsid[] = "$Id: sim_save.c,v 1.2 2005/09/29 21:45:56 ghood Exp $";

/*
** $Log: sim_save.c,v $
** Revision 1.2  2005/09/29 21:45:56  ghood
** changed memusage to support Cray XT3
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.12  2001/04/25 17:17:03  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.10  2000/04/20 06:50:38  mhucka
** Removed unused variable progname.
**
** Revision 1.9  1997/11/07 21:08:39  dhb
** ELF Linux sets up process beginning at 0x8000000.  Changed memusage()
** to base usage from edata on Linux systems (like it does for mips).
**
** Revision 1.8  1997/08/08 18:49:43  dhb
** Changed return type for sbrk() to void*
**
** Revision 1.7  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.6  1996/07/24 21:46:21  dhb
** do_restoreparms() wasn't detecting EOF and ended up calling the
** last element in the SAVE2 file twice.  This is normally harmless
** since the SAVE2 action code would catch the EOF, return a random
** value for unread values and fail in attempting to read past the
** EOF.
**
 * Revision 1.5  1995/04/12  22:07:47  dhb
 * Removed all image save and restore code for the save and restore
 * commands.
 *
 * Revision 1.4  1995/03/23  18:15:15  dhb
 * Added number of values handling for save file format.  do_restoreparms()
 * has been updated to skip data values in various error cases.  Note: the
 * RESTORE2 action has been updated to return 0 on success and the number
 * of values left unread on an error.  do_restoreparms() uses this value
 * to skip additional unread values.  The prefered method for reading the
 * data for an element is to read the number of values stored and check
 * for consistency in the RESTORE2 action.  If the number of values does
 * not match what the element wants, it should read no values and return
 * the number of values as read from the file.
 *
 * Revision 1.3  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.2  1993/02/22  19:41:20  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_save (saveimage), do_restore (restoreimage), do_saveparms (save)
 * 	and do_retoreparms (restore) changed to use GENESIS getopt routines.
 *
 * Revision 1.1  1992/10/27  20:27:04  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

/* Changed restore function, E De Schutter Caltech 11/91 */

extern int debug;


size_t memusage()
{
size_t memsize;
#if defined(mips) || (defined(Linux) && !defined(XT3))
extern int edata;
    memsize = (ADDR)sbrk(0) - (ADDR)(&edata);
#else
    memsize = (ADDR)sbrk(0);
#endif
    return memsize;
}

int CheckType2Header(file)
char *file;
{
FILE *fp;
int	size = 80;
char	header[80];

    /*
    ** check the header
    */
    if((fp = fopen(file,"r")) == NULL){
	printf("unable to read file '%s'\n",file);
	return(0);
    }
    fgets(header,size,fp);
    fclose(fp);
    if(strncmp(header,"TYPE2",5) != 0){
	printf("incompatible save formats 'TYPE2' and '%s'\n",header);
	return(0);
    }
    return(1);
}

int do_saveparms(argc,argv)
int argc;
char **argv;
{
FILE 	*fp;
char 	*mode;
char 	*file;
char 	*path;
PFI	func;
Action	action;
Element	*element;
ElementList	*list;
int	i;
int	status;

    mode = "w";

    initopt(argc, argv, "path file-name -append");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-append") == 0)
	    mode = "a";
      }

    if(status < 0){
	printoptusage(argc, argv);
	return(0);
    }

    path = optargv[1];
    file = optargv[2];

    if(strcmp(mode,"a") == 0){
	if(!CheckType2Header(file)){
	    return(0);
	}
    } 
    if((fp = fopen(file,mode)) == NULL){
	printf("unable to write to file '%s'\n",file);
	return(0);
    }
    if(strcmp(mode,"w") == 0){
	/*
	** write out the header
	*/
	fprintf(fp,"TYPE2\n");
    }
    list = WildcardGetElement(path,1);
    action.data = (char *)fp;
    action.type = SAVE2;
    action.name = "SAVE2";
    for(i=0;i<list->nelements;i++){
	element = list->element[i];
	/*
	** call the element function with the desired action
	*/
	if((func = GetActionFunc(element->object,SAVE2,NULL,NULL)) != NULL){
	    fprintf(fp,"%s\n",Pathname(element));
	    CallActionFunc(element, &action);
	} else {
	    printf("'%s' does not support the SAVE2 action\n",
	    Pathname(element));
	}
    }
    fclose(fp);
    FreeElementList(list);
    return(1);
}

int do_restoreparms(argc,argv)
/* E De Schutter: restore now continues if a stored element is not present
** in the simulation */
int argc;
char **argv;
{
FILE 	*fp;
char 	*file;
char 	path[300];
int	size = 300;
PFI	func;
Action	action;
char	restoredata[1000];
Element	*element;
char	*ptr;
int	n;

    initopt(argc, argv, "file-name");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0;
      }

    file = optargv[1];
    if(!CheckType2Header(file)){
	return(0);
    }
    if((fp = fopen(file,"r")) == NULL){
	printf("unable to read from file '%s'\n",file);
	return(0);
    }
    action.data = (char *)fp;
    action.type = RESTORE2;
    action.name = "RESTORE2";
    /* skip past the header */
    fgets(path,size,fp);
    fgets(path,size,fp);
    while(!feof(fp)){
		/* strip the CR */
		for(ptr=path;*ptr != '\n' && *ptr != '\0';ptr++);
		*ptr = '\0';
		if((element = GetElement(path)) == NULL){
		    printf("cannot find element '%s'\n",path);
		    fread(&n,sizeof(int),1,fp);
		    while (n-- > 0)
			fread(restoredata,sizeof(double),1,fp);
		} else {
			/* call the element function with the desired action */
			if((func = GetActionFunc(element->object,RESTORE2,NULL,NULL)) != NULL){
				n = CallActionFunc(element, &action);
				while (n-- > 0)
				    fread(restoredata,sizeof(double),1,fp);
			} else {
				printf("'%s' does not support the RESTORE2 action\n",
					Pathname(element));
				fread(&n,sizeof(int),1,fp);
				while (n-- > 0)
				    fread(restoredata,sizeof(double),1,fp);
			}
		}
		fgets(path,size,fp); /* get next path or EOF */
    }
    fclose(fp);
    return(1);
}
