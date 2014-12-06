static char rcsid[] = "$Id: shell_load.c,v 1.2 2005/06/26 08:25:37 svitak Exp $";

/*
** $Log: shell_load.c,v $
** Revision 1.2  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.4  2000/06/12 04:51:02  mhucka
** The alternate definition of LoadFunction() must be defined with the same
** number of arguments as the real definition, otherwise compilers will complain
** about mismatched arguments.
**
** Revision 1.3  1995/06/16 21:47:44  dhb
** Moved includes for dynamic loading under MASSCOMP and sun3 ifdef.
** Was causing compile problems for HPUX.
**
** Revision 1.2  1993/09/17  17:51:15  dhb
** Solaris compatability.
**
** Revision 1.1  1992/12/11  19:04:36  dhb
** Initial revision
**
*/

#include "shell_ext.h"

static short dynamic_enabled = 0;

int Dynamic()
{
    return (int)(dynamic_enabled);
}

void EnableDynamicLoad()
{
	dynamic_enabled =1;
}

#if (defined(MASSCOMP) || defined(sun3))
#ifndef Solaris
#include <a.out.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <stab.h>

/*
** LoadFunction dynamically loads a function into a running program
** It does this by compiling either .c or .o files and linking them
** into the executable file which is currently running
**
** It is assumed that the most recently linked version of the program
** is in /tmp/tmp.dfl, therefore before calling LoadFunction for the first
** time, the executing program should be copied to tmp.dfl
** after this, all further loads will use tmp.dfl which will be updated
** by each subsequent LoadFunction call
**
** LoadFunction checks the modification dates for the .c and .o files
** and compiles which ever is more up to date
**
** LoadFunction assumes that there is only one function per file and
** that the file has the same name as the function (plus .c or .o)
*/
PFI LoadFunction(func_name,prog_name,options)
char 		*func_name;
char 		*prog_name;
char 		*options;
{
FILE		*fp;
unsigned	text_offset;
unsigned	data_offset;
unsigned	text_size;
unsigned	data_size;
struct exec 	header;
char 		objfile[100];
char 		srcfile[100];
char 		*text_block;
char 		*data_block;
char 		cmd[100];
int 		i;
struct stat 	buf;
time_t 		src_mtime;
time_t 		obj_mtime;

    sprintf(srcfile,"%s.c",prog_name);
    sprintf(objfile,"%s.o",prog_name);
    printf("\n");
    /*
    ** try to get the stats on the source file
    */
    if(stat(srcfile,&buf) != -1){
	src_mtime = buf.st_mtime;	
    } else {
	src_mtime = 0;
	/*
	** if the stat check fails then check to see if the obj
	** file exists
	*/
	if((fp = fopen(objfile,"r")) == NULL){
	    /*
	    ** if the obj file doesnt exist either then forget it
	    */
	    printf("*** cant find source file %s\n",srcfile);
	    printf("*** load function failed\n\n");
	    return(0);
	}
    }
    /*
    ** try to get the stats on the obj file
    */
    if(stat(objfile,&buf) != -1){
	obj_mtime = buf.st_mtime;	
    } else {
	obj_mtime = 0;
    }
    /*
    ** if the src file is more recent than the object file then recompile
    */
    if(src_mtime > obj_mtime){
	/*
	** compile the src file into an object file
	*/
	printf("Compiling %s\n",srcfile);
	sprintf(cmd,"cc -c %s",srcfile);
	system(cmd);
    }
    if((fp = fopen(objfile,"r")) == NULL){
	printf("*** cannot open file %s\n",objfile);
	printf("*** load function failed\n\n");
	return(0);
    }
    /*
    ** get the header info from the object module
    ** VERY MACHINE DEPENDENT
    */
    fread(&header,sizeof(header),1,fp);
    text_offset = N_TXTOFF(header);
    text_size = header.a_text;
    data_size = header.a_data;
    fclose(fp);

    /*
    ** allocate the space for the code and data blocks based
    ** on the object module sizes
    */
    text_block = (char *)malloc(text_size+10);
    data_block = (char *)malloc(data_size+10);
    /*
    ** link the object module into the current executable
    */
    printf("Linking %s\n\n",objfile);
#ifdef MASSCOMP
    sprintf(cmd,"ld -A /tmp/tmp.dfl -T %x -D %x %s -o /tmp/tmp.dfl",
    text_block,data_block,objfile);
#else
    sprintf(cmd,"ld -A /tmp/tmp.dfl -Ttext %x -Tdata %x %s -o /tmp/tmp.dfl %s",
    text_block,data_block,objfile,options);
#endif
    system(cmd);

    /*
    ** open up the newly linked executable
    */
    fp = fopen("/tmp/tmp.dfl","r");
    /*
    ** VERY MACHINE DEPENDENT 
    */
    fread(&header,sizeof(header),1,fp);

    /*
    ** get the code from the newly linked executable
    ** by reading the header and determining its location in the file
    */
    text_offset = N_TXTOFF(header);
    text_size = header.a_text;
    fseek(fp,text_offset,0);
    fread(text_block,text_size,1,fp);
    /*
    ** get the data block from the newly linked executable
    */
    data_offset = text_offset + header.a_text;
    data_size = header.a_data;
    fseek(fp,data_offset,0);
    fread(data_block,data_size,1,fp);

    fclose(fp);

    /*
    ** return the address of the new code block
    */
    return((PFI)text_block);
}
#else
PFI LoadFunction(func_name,prog_name,options)
char 		*func_name;
char 		*prog_name;
char 		*options;
{
    printf("** Dynamic loading is not supported on this machine\n");
    return(NULL);
}
#endif

void do_load_func(argc,argv)
int		argc;
char	**argv;
{
PFI	func;
char *progname;
char *funcname;
char *options;

    if(argc < 2){
	printf("usage: %s funcname [progname [options]]\n",argv[0]);
    }
    funcname = argv[1];

    if(argc > 3){
	options = argv[3];
    } else
	options = "";
    if(argc > 2){
	progname = argv[2];
    } else {
	progname = argv[1];
    }
    /*
    ** load the function
    */
    func = LoadFunction(funcname,progname,options);
    PutAuxFunc(funcname,func);
}

void LoadFuncInit(prog)
char *prog;
{
char loadstr[80];

    /*
    ** copy the current program into a temporary file
    */
    sprintf(loadstr,"cp %s /tmp/tmp.dfl",prog);
    system(loadstr);
}

#ifdef MAIN
/*
* test program to dynamically load functions
*
*/
main(argc,argv)
int argc;
char **argv;
{
PFI func;
int i,val;
short cnt;

    LoadFuncInit(argv[0]);

    cnt = 0;
    while(++cnt < argc){
	if(func = LoadFunction(argv[cnt])){
	    printf("executing function %s\n",argv[cnt]);
	    func();
	}
    }
}
#endif
