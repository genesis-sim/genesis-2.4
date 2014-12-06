static char rcsid[] = "$Id: shell_exec.c,v 1.4 2006/01/09 16:25:56 svitak Exp $";

/*
** $Log: shell_exec.c,v $
** Revision 1.4  2006/01/09 16:25:56  svitak
** More informative error reporting for missing shell commands.
**
** Revision 1.3  2005/07/01 10:03:08  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.2  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.10  2003/05/29 22:33:01  gen-dbeeman
** Replaced C++ style comments for K&R compatiblility
**
** Revision 1.9  2003/03/28 21:10:46  gen-dbeeman
** Changes from Hugo Cornelis to enable debugging commands gctrace and gftrace
**
** Revision 1.8  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.6  2001/04/01 19:30:47  mhucka
** Small cleanup fixes involving variable inits and printf args.
**
** Revision 1.5  2000/06/19 06:05:33  mhucka
** The Script structure and the handling of scripts had this bogosity in which
** FILE pointers were being cast to char * and back again.  Why on earth
** didn't the authors just keep a separate pointer in the Script structure for
** FILE pointers, instead of shoehorning the pointers into the .ptr char *
** field?  Anyway, for better robustness and portability, I reimplemented this
** part using two separate pointers.
**
** Revision 1.4  1995/04/14 19:10:53  dhb
** Changed use of popen() to G_popen().
**
** Revision 1.3  1995/03/29  21:22:01  dhb
** ExecuteCommand() now check for failure on open/pipe redirection
** and issues an appropriate error message.  The failed command will
** return a zero value.
**
** Revision 1.2  1995/02/24  23:58:39  dhb
** Added handling of double functions to ExecuteCommand().
**
** Revision 1.1  1992/12/11  19:04:28  dhb
** Initial revision
**
*/

#include <unistd.h>
#include "shell_ext.h"
#include "result.h"
#include "ss/ss_func_ext.h"


static int bGCTrace = FALSE;


int GetCommandTraceLevel(void)
{
    return(bGCTrace);
}

int SetCommandTraceLevel(int iLevel)
{
    return bGCTrace = !!iLevel;
}

#ifdef NEW
Result ExecuteCommand(argc,argv)
int 	argc;
char 	**argv;
{
FuncTable 	GetCommand();
FILE 		*SearchForScript();
PFI		func;
PFC		cfunc;
PFF		ffunc;
PFD		dfunc;
FILE		*pfile;
int 		code;
/* FILE		tmpfile; */
short 		redirect_mode = 0;
int 		start = 0;
int 		i;
char 		*mode = "a";
Result		result;
FuncTable	command;

    /* - trace */

    if (bGCTrace)
    {
	printf("Command %s(",argv[0] ? argv[0] : "Undefined");
	for (i = 1 ; i < argc ; i++)
	    printf("{%s}%s",argv[i],i + 1 == argc ? "" : ",");
	printf(")\n");
    }

    result.r_type = IntType();
    result.r.r_int = 0;
    if(argc < 1){
	return(result);
    }
    /*
    ** is this an simulator shell function?
    */
    command = GetCommand(argv[0]);
    if ((func = command.adr)) {
	/*
	** check through the arg list for stdout
	** redirection
	*/
	for(i=0;i<argc;i++){
	    /*
	    ** check for stdout redirection
	    */
	    if(i+1 < argc && ((strcmp(argv[i],"|") == 0) ||
	   (strcmp(argv[i],">") == 0) ||
	   (strcmp(argv[i],">>") == 0))){
		start = i+1;
		if(strcmp(argv[i],"|") == 0){
		    /*
		    ** pipe
		    */
		    redirect_mode = 1;
		    mode = "w";
		}
		if(strcmp(argv[i],">") == 0){
		    /*
		    ** redirect stdout to a file
		    */
		    redirect_mode = 2;
		    mode = "w";
		}
		if(strcmp(argv[i],">>") == 0){
		    /*
		    ** append stdout to a file
		    */
		    redirect_mode = 2;
		    mode = "a";
		}
		break;
	    }
	}
	if(redirect_mode){
	    FILE *fp = NULL;
	    int   savefd;
	    int   stdoutfd = fileno(stdout);

	    normal_tty();
	    /*
	    ** save the stdout FILE structure
	    */
#ifdef OLD
	    fflush(stdout);
	    BCOPY(output,&tmpfile,sizeof(FILE));
#endif
	    switch(redirect_mode){
	    case 1:
		/*
		** open up the pipe
		*/
		fp = G_popen(ArgListToString(argc-start,argv+start),mode);
		break;
	    case 2:
		/*
		** open up the file
		*/
		fp = fopen(argv[start],mode);
		break;
	    }
#ifndef OLD
	    if (fp == NULL){ /* G_popen or fopen failed!!! */
		genesis_tty();
		perror(argv[start]);
		return(result);
	    }
	    /*
	    ** flush the existing data in stdout
	    */
	    fflush(stdout);
	    savefd = dup(stdoutfd);
	    close(stdoutfd);
	    dup(fileno(fp));
#else
	    /*
	    ** redirect stdout by replacing the stdout 
	    ** FILE structure with the new fp
	    */
	    BCOPY(fp,output,sizeof(FILE));
#endif
	    /*
	    ** call the function
	    */
	    func(start-1,argv);
	    /*
	    ** flush the output and close it
	    */
	    fflush(stdout);
#ifndef OLD
	    close(stdoutfd);
#endif
	    switch(redirect_mode){
	    case 1:
		G_pclose(fp);
		break;
	    case 2:
		fclose(fp);
		break;
	    }
	    /*
	    ** restore the stdout file structure
	    */
#ifndef OLD
	    dup(savefd);
	    close(savefd);
#else
	    BCOPY(&tmpfile,output,sizeof(FILE));
#endif
	    genesis_tty();
	} else
	/*
	** call the function
	*/
	if(strcmp(command.type,"int") == 0){
	    result.r_type = IntType();
	    result.r.r_int = func(argc,argv);
	} else
	if(strcmp(command.type,"float") == 0){
	    ffunc = (PFF)func;
	    result.r_type = FloatType();
	    result.r.r_float = ffunc(argc,argv);
	} else
	if(strcmp(command.type,"double") == 0){
	    dfunc = (PFD)func;
	    result.r_type = FloatType();
	    result.r.r_float = dfunc(argc,argv);
	} else
	if(strcmp(command.type,"char*") == 0){
	    cfunc = (PFC)func;
	    result.r_type = StrType();
	    result.r.r_str = cfunc(argc,argv);
	} else
	if(strcmp(command.type,"char**") == 0){
	    cfunc = (PFC)func;
	    result.r_type = ArgListType();
	    result.r.r_str = cfunc(argc,argv);
	} else {
	    func(argc,argv);
	    result.r_type = IntType();
	    result.r.r_int = 0;
	}
	return(result);
    } else 
    /*
    ** is it a simulator shell script?
    */
    if(ValidScript(pfile = SearchForScript(argv[0],"r"))){
	AddScript(NULL, pfile, argc, argv, FILE_TYPE);
    } else {
	/*
	** if the function is not locally defined then
	** check to see if system functions should be
	** tried
	*/
	if(Autoshell()){
	    normal_tty();
#ifdef OLD
	    if((code = system(ArgListToString(argc,argv))) != 0){
		Error();
		printf("code %d\n",code);
	    };
#endif
	    if((code = ExecFork(argc,argv)) != 0){
		Error();
		printf("executing %s: code %d\n", argv[0], code);
	    };
	    genesis_tty();
	    result.r_type = IntType();
	    result.r.r_int = code;
	    return(result);
	} else
	printf("undefined function \"%s\"\n",argv[0]);
    }
    result.r_type = IntType();
    result.r.r_int = 0;
    return(result);
}
#endif
