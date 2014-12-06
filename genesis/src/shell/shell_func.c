static char rcsid[] = "$Id: shell_func.c,v 1.6 2005/10/20 22:02:44 svitak Exp $";

/*
** $Log: shell_func.c,v $
** Revision 1.6  2005/10/20 22:02:44  svitak
** Made Nlist static since it's module-private.
**
** Revision 1.5  2005/10/16 20:48:35  svitak
** Changed hard-coded character arrays from 100 to 256 or malloc'd and freed
** memory instead. This was only done where directory paths might have easily
** exceeded 100 characters.
**
** Revision 1.4  2005/07/29 16:05:00  svitak
** Removed '-w' option from grep as not all architectures support it.
**
** Revision 1.3  2005/07/20 20:02:03  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.11  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.9  2000/09/21 19:38:49  mhucka
** Added long specifiers where needed in printf format strings.
**
** Revision 1.8  1997/07/18 02:55:46  dhb
** Fix to getopt problem.  getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.7  1996/06/06 20:05:49  dhb
** Paragon port.
**
** Revision 1.6  1995/07/18  22:10:39  dhb
** Changed preprocessor conditional for Sun 386i to use sun386i rather
** than i386.  The latter was an unfortunate choice as it gets defined
** by some compilers on Intel x86 architectures.
**
** Revision 1.5  1995/04/14  19:10:53  dhb
** Changed use of popen() to G_popen().
**
** Revision 1.4  1994/10/21  00:56:12  dhb
** GetFuncName() was leaking the return value from itoa() which is
** allocated memory.
**
** Revision 1.3  1994/04/14  16:06:31  dhb
** Merged 1.1.1.1 changes
**
<<<<<<< 1.2
** Revision 1.2  1993/02/26  22:09:38  dhb
** 1.4 to 2.0 command argument changes.
**
=======
** Revision 1.1.1.1  1994/04/14  15:59:20  dhb
** Changes from ngoddard@psc.edu to use the correct return type for
** GetFuncAddress() (e.g. PFI instead of int).
**
>>>>>>> 1.1.1.1
** Revision 1.1  1992/12/11  19:04:29  dhb
** Initial revision
**
*/

#include <stdlib.h>	/* malloc */
#include "shell_ext.h"

#ifdef NLIST_BUG
struct  nlist {
    char    *n_name;		/* symbol name */
    unsigned long n_value;	/* symbol address */
};
#else
#include <nlist.h>
#endif

int	DEBUG_GetFuncAddress;

#define MAX_FUNCS	1000
static int 		nfuncs = 0;
static FuncTable 	aux_func_table[MAX_FUNCS];
static int		nauxfuncs = 0;
static FuncTable	func_table[MAX_FUNCS];


static void Nlist(progname,nl)
char 	*progname;
struct nlist 	*nl;
{
char 	*target;
FILE		*nfp;
char		*com;

    target = nl[0].n_name;
	/*
	** We want the output to be the same on all archtectures so the
	** TestSuite results will be comparable.
	*/
	if (target[0]=='_')
    	printf("looking up '%s'\n",&target[1]);
	else
    	printf("looking up '%s'\n",target);

    /*
    ** To get around a bug in the sun386i OS.
    ** Instead of using the nlist system call 
    ** run nm explicitly and grep for the target
    ** which is much slower but at least it works
    */
#ifndef __PARAGON__
	com = (char *)malloc(strlen(progname) + strlen(target) + 15);
    sprintf(com,"nm %s | grep %s",progname,target);
    nfp = G_popen(com,"r");
    if(fscanf(nfp,"%lx",&(nl[0].n_value)) != 1){
	nl[0].n_value = 0;
    }
    G_pclose(nfp);
	free(com);
#else
	nl[0].n_value = 0;
#endif
}

void ShowFuncNames(argc, argv)

int	argc;
char**	argv;

{
int i;
int col = 0;
char	**namelist;
extern int Strcmp();

    initopt(argc, argv, "");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    if(nfuncs < 1) return;
    namelist = (char **)malloc(nfuncs*sizeof(char *));
    for(i=0;i<nfuncs;i++){
	namelist[i] = func_table[i].name;
    }
    qsort(namelist,nfuncs,sizeof(char *),Strcmp);
    printf("\nAvailable commands:");
    for(i=0;i<nfuncs;i++){
	if(col %80 == 0) {
	    printf("\n");
	}
	col += 20;
	printf("%-20s",namelist[i]);
    }
    printf("\n\n");
    free(namelist);
}

void AddFunc(name,adr,type)
char 	*name;
PFI	adr;
char	*type;
{
int	index;

    index = FuncIndex(name);

	if (index >= MAX_FUNCS) {
		printf("AddFunc: exceeded max functions: can not add %s\n", name);
		return;
	}

    func_table[index].name = CopyString(name);
    func_table[index].adr = (PFI)adr;
    func_table[index].type = CopyString(type);
    if(index == nfuncs)
	nfuncs++;
}

int FuncIndex(name)
char *name;
{
int		i;

    for(i=0;i<nfuncs;i++){
	if(strcmp(name,func_table[i].name) ==0){
	    return(i);
	}
    }
    return(nfuncs);
}

void PutAuxFunc(name,adr)
char *name;
PFI adr;
{
int index;
    /*
    ** find the index of the function in the aux function table
    */
    index = AuxFuncIndex(name);

	if (index >= MAX_FUNCS) {
		printf("PutAuxFunc: exceeded max functions: can not add %s\n", name);
		return;
	}

    /*
    ** put the new function in the table
    */
    aux_func_table[index].name = CopyString(name);
    aux_func_table[index].adr = adr;
    /*
    ** if it is a new function then increment the function count
    */
    if(index == nauxfuncs)
	    nauxfuncs++;
}

int AuxFuncIndex(name)
char *name;
{
int		i;

    for(i=0;i<nauxfuncs;i++){
	if(strcmp(name,aux_func_table[i].name) ==0){
	    return(i);
	}
    }
    return(nauxfuncs);
}

FuncTable GetCommand(name)
char *name;
{
int		i;
FuncTable	command;
char	*temp;
/* So commands can be executed on remote nodes - Upi Bhalla 1992 */
	for(temp=name;*temp;temp++) {
		if (*temp=='@') {
			name="rcommand";
			break;
		}
	}

    for(i=0;i<nfuncs;i++){
	if(strcmp(name,func_table[i].name) ==0){
	    return(func_table[i]);
	}
    }
    command.adr = NULL;
    return(command);
}

int IsCommand(name)
char *name;
{
int		i;

    for(i=0;i<nfuncs;i++){
	if(strcmp(name,func_table[i].name) ==0){
	    return(1);
	}
    }
    return(0);
}

int GetInfo(name,info)
char *name;
Info *info;
{
Info *hash_info;
Info *InfoHashFind();

    if((hash_info = InfoHashFind(name))){
	    BCOPY(hash_info,info,sizeof(Info));
	    return(1);
    }
    return(0);
}

/*
** GetFuncAddress takes a function name and returns the address associated
** with it
*/
PFI GetFuncAddress(name)
char *name;
{
struct nlist 	nl[2];
FuncTable 	*func;
FuncTable 	*FuncNameHashFind();
char 		target[100];
int		i;

    /*
    ** check the auxilliary function table
    */
    for(i=0;i<nauxfuncs;i++){
	if(strcmp(name,aux_func_table[i].name) == 0){
	    return(aux_func_table[i].adr);
	}
    }
    if ((func = FuncNameHashFind(name))) {
	/*
	** see if it has already been defined
	*/
	return(func->adr);
    } else{
	if(Debug(DEBUG_GetFuncAddress) > 0){
	    printf("GetFuncAddress: looking up %s from nlist\n",name);
	}
	/*
	** try and find it using the nlist
	*/
#if defined(sun386i) || defined(mips)  || defined(irix)
	sprintf(target,"%s",name);
#else
	sprintf(target,"_%s",name);
#endif
	nl[0].n_name = target;
	nl[1].n_name = NULL;
#ifdef NLIST_BUG
	Nlist(progname,nl);
#else
	nlist(progname,nl);
#endif
	if(nl[0].n_value > 0){
	    /*
	    ** add it to the function list
	    */
	    HashFunc(name,nl[0].n_value,"void"); 
	    return((PFI) nl[0].n_value);
	} else
	    return(NULL);
    }
}


/*
** GetFuncType takes a function name and returns the type associated
** with its return value
*/
char *GetFuncType(name)
char *name;
{
struct nlist 	nl[2];
FuncTable 	*func;
FuncTable 	*FuncNameHashFind();
char 		target[100];
int		i;

    /*
    ** check the auxilliary function table
    */
    for(i=0;i<nauxfuncs;i++){
	if(strcmp(name,aux_func_table[i].name) == 0){
	    return(aux_func_table[i].type);
	}
    }
    if ((func = FuncNameHashFind(name))) {
	/*
	** see if it has already been defined
	*/
	return(func->type);
    } else{
	if(Debug(DEBUG_GetFuncAddress) > 0){
	    printf("GetFuncType: looking up %s from nlist\n",name);
	}
	/*
	** try and find it using the nlist
	*/
#if defined(sun386i) || defined(mips) || defined(IRIS)
	sprintf(target,"%s",name);
#else
	sprintf(target,"_%s",name);
#endif
	nl[0].n_name = target;
	nl[1].n_name = NULL;
#ifdef NLIST_BUG
	Nlist(progname,nl);
#else
	/*
	** here is the call
	*/
	nlist(progname,nl);
#endif
	if(nl[0].n_value > 0){
	    /*
	    ** add it to the function list
	    */
	    HashFunc(name,nl[0].n_value,"void");
	    return("void");
	} else
	    return(NULL);
    }
}

/*
** GetFuncName takes an address and returns the symbol name associated
** with it
*/
char *GetFuncName(adr)
PFI adr;
{
int	i;
char *sadr;
FuncTable *func;
FuncTable *FuncAdrHashFind();

    if(adr == NULL) return(NULL);
    /*
    ** check the auxilliary function table
    */
    for(i=0;i<nauxfuncs;i++){
	if(adr == aux_func_table[i].adr){
	    return(aux_func_table[i].name);
	}
    }
    sadr = itoa(adr);
    if ((func = FuncAdrHashFind(sadr))) {
	free(sadr);
	return(func->name);
    } else {
	free(sadr);
	return("???");
    }
}

void do_add_func(argc,argv)
int		argc;
char	**argv;
{
PFI	adr;
char 	*type;

    initopt(argc, argv, "name function [type]");
    if (G_getopt(argc, argv) != 0 || optargc > 4)
      {
	printoptusage(argc, argv);
	return;
      }

    if((adr = GetFuncAddress(optargv[2])) != NULL){
	if(argc > 3){
	    type = optargv[3];
	} else {
	    type = GetFuncType(optargv[2]);
	}
	AddFunc(optargv[1],adr,type);
    } else {
	printf("could not find function %s\n",optargv[2]);
    }
}

void do_execute(argc,argv)
int argc;
char **argv;
{
PFI func;
char **func_argv;
int func_argc;

    if(argc < 2){
	printf("usage: %s func arg1 .. \n",argv[0]);
	return;
    }

    if((func = (PFI)GetFuncAddress(argv[1])) == NULL){
	printf("cant find function '%s'\n",argv[1]);
	return;
    }
    func_argv = argv + 1;
    func_argc = argc - 1;
    func(func_argc,func_argv);
}

