static char rcsid[] = "$Id: shell_script.c,v 1.3 2005/07/07 19:03:54 svitak Exp $";

/*
** $Log: shell_script.c,v $
** Revision 1.3  2005/07/07 19:03:54  svitak
** Eliminated traces of old TraceScript function so as not to confuse it
** with the newer, less usable TraceScript found in ss/eval.c.
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
** Revision 1.10  2000/06/19 06:06:08  mhucka
** The Script structure and the handling of scripts had this bogosity in which
** FILE pointers were being cast to char * and back again.  Why on earth
** didn't the authors just keep a separate pointer in the Script structure for
** FILE pointers, instead of shoehorning the pointers into the .ptr char *
** field?  Anyway, for better robustness and portability, I reimplemented this
** part using two separate pointers.
**
** Revision 1.9  2000/06/12 05:07:04  mhucka
** Removed nested comments; added NOTREACHED comments where appropriate.
**
** Revision 1.8  1997/07/18 02:55:46  dhb
** Fix to getopt problem.  getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.7  1995/07/14 00:30:01  dhb
** SetScript() wasn't pushing argv as local variables in the
** script.  As a result, scripts entered on the command line
** with script arguments were not getting the arguments in the
** script.
**
** Revision 1.6  1995/04/06  21:00:58  dhb
** Changed SearchFroScript(), SearchForExecutable() and
** SearchForNonScript() to open directly and file with a
** patch.  Previous versions were doing this only for
** absolute paths; added relative paths.
**
** Fixed potential bug in SearchForExecutable() calling fclose with
** a NULL file pointer.
**
** Revision 1.5  1994/11/10  21:02:33  dhb
** Added TopLevelScriptFileName() function to return the top level
** script file name.
**
** Revision 1.4  1994/10/18  16:55:01  dhb
** NextScript() and NextScriptFp() now free the script argv.
** Removed duplicate fclose() in NextScriptFp().
**
** Revision 1.3  1994/03/22  17:49:50  dhb
** Added FindFileOnPath() function which finds a file in the directories
** on the path returning the full pathname.
**
** Revision 1.2  1993/02/26  22:09:38  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:04:42  dhb
** Initial revision
**
*/

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ss_func_ext.h"
#include "shell_ext.h"

/* mds3 changes */
/*
** Argument to 'fclose()' cast to (FILE*)
*/

#define MAXSCRIPTS	20
#define SIZE 80

short	script_ptr = -1;
Script	script[MAXSCRIPTS];

extern char **CopyArgv();

int ValidScript(fp)
FILE *fp;
{
char line[80];
int i;

    if(fp == NULL) return(0);
    /*
    ** check for the magic word at the beginning of the file
    */
    line[0] = '\0';
    while(EmptyLine(line) && !feof(fp)){
	fgets(line,SIZE,fp);
    }
    /*
    ** skip white space
    */
    i = 0;
    while(line[i] == ' ' && i < SIZE-1) i++;
    if((i < SIZE-2) && (strncmp(line+i,"//",2) == 0)){
	i += 2;
	/*
	** skip more white space
	*/
	while(line[i] == ' ' && i < SIZE-1) i++;
	if((i <= SIZE - strlen("genesis")) && 
	(strncmp(line+i,"genesis", strlen("genesis")) ==  0)) {
	    fseek(fp,0L,0);
	    return(1);
	}
    }
    fclose(fp);
    return(0);
}
#undef SIZE
/*
** file script utilities (used by the 'source' command)
*/
void SetScript(ptr, fp, argc, argv, type)
char  *ptr;
FILE  *fp;
int    argc;
char **argv;
short  type;
{
    if ((type == FILE_TYPE && fp == NULL)
	|| (type == STR_TYPE && ptr == NULL)) {
	return;
    }

    ClearScriptStack();
    script_ptr = 0;
    script[script_ptr].ptr = ptr;
    script[script_ptr].file = fp;
    script[script_ptr].current = ptr;
    script[script_ptr].type = type;
    script[script_ptr].argc = argc;
    /*
    ** make a copy of the argv list
    */
    script[script_ptr].argv = CopyArgv(argc, argv);
    script[script_ptr].line = 0;
#ifdef NEW
    if (argc > 0)
	PushLocalVars(argc,argv,NULL);
#endif
}

void AddScript(ptr, fp, argc, argv, type)
char  *ptr;
FILE  *fp;
int    argc;
char **argv;
short  type;
{
    if ((type == FILE_TYPE && fp == NULL)
	|| (type == STR_TYPE && ptr == NULL)) {
	return;
    }

    if (script_ptr+1 < MAXSCRIPTS) {
	script_ptr++;
	script[script_ptr].ptr = ptr;
	script[script_ptr].file = fp;
	script[script_ptr].current = ptr;
	script[script_ptr].type = type;
	script[script_ptr].argc = argc;
	script[script_ptr].argv = CopyArgv(argc,argv);
	script[script_ptr].line = 0;
    } else {
	Error();
	printf("file script stack overflow\n");
    }
#ifdef NEW
    if(argc > 0)
	PushLocalVars(argc,argv,NULL);
#endif
}

char* TopLevelNamedScriptFileName()
{
    int i;

    for (i = 0; i <= script_ptr; i++) {
	if (script[i].type == FILE_TYPE && script[i].file != stdin) {
	    return script[i].argv[0];
	}
    }
    return NULL;
}

FILE *NextScriptFp()
{
    if (script_ptr > 0) {
	/*
	** close the current file
	*/
	if (script[script_ptr].type == FILE_TYPE) {
	    fclose(script[script_ptr].file);
	} else {
	    script[script_ptr].current = script[script_ptr].ptr;
	}
#ifdef NEW
	if (script[script_ptr].argc > 0) {
	    PopLocalVars();
	}
#endif
	FreeArgv(script[script_ptr].argc, script[script_ptr].argv);
	return(script[--script_ptr].file);
    } else {
	return(NULL);
    }
}

Script *NextScript()
{
    if (script_ptr > 0) {
	/*
	** close the current file
	*/
	if (script[script_ptr].type == FILE_TYPE){
	    fclose(script[script_ptr].file);
	} else {
	    script[script_ptr].current = script[script_ptr].ptr;
	}
#ifdef NEW
	if (script[script_ptr].argc > 0) {
	    PopLocalVars();
	}
#endif
	FreeArgv(script[script_ptr].argc, script[script_ptr].argv);
	return(&(script[--script_ptr]));
    } else {
	return(NULL);
    }
}

void ClearScriptStack()
{
    while(script_ptr > 0){
	NextScript();
    }
}

int ScriptDepth()
{
    return(int)(script_ptr);
}

Script *CurrentScript()
{
    if(script_ptr >= 0)
	return(&(script[script_ptr]));
    else
	return(NULL);
}

FILE *CurrentScriptFp()
{
    if (script_ptr >= 0)
	return(script[script_ptr].file);
    else
	return(NULL);
}

char *CurrentScriptName()
{
    if(script_ptr >= 0 && script[script_ptr].argc > 0)
	return(script[script_ptr].argv[0]);
    else
	return(NULL);
}

int CurrentScriptLine()
{
    if(script_ptr >= 0)
	return(script[script_ptr].line);
    else
	return(-1);
}

int ScriptEnded()
{
    if (script_ptr >= 0) {
	switch(script[script_ptr].type) {
	case FILE_TYPE:
#ifdef NEW
	    if (script[script_ptr].argc > 0) {
		PopLocalVars();
	    }
#endif
	    return(feof(script[script_ptr].file));
	case STR_TYPE:
#ifdef NEW
	    if (script[script_ptr].argc > 0) {
		PopLocalVars();
	    }
#endif
	    return(*(script[script_ptr].current) == '\0');
	}
    }
    return(0);
}

int ScriptArgc()
{
    if(script_ptr >= 0)
	return(script[script_ptr].argc);

    return(0);
}

char *ScriptArgv(arg)
int arg;
{
    if((script_ptr >= 0) && (arg < script[script_ptr].argc)){
	return(script[script_ptr].argv[arg]);
    } else {
	return("");
    }
}

void EndScript()
{
    NextScript();
}

int IsFile(fp)
FILE *fp;
{
struct stat buf;
    if(!fp) return(0);
    if(fstat(fileno(fp),&buf) == -1 || buf.st_mode & S_IFDIR){
	return(0);
    }
    return(1);
}

FILE *OpenScriptFile(name,mode)
char *name;
char *mode;
{
FILE *fp;
char extname[300];
size_t len;

    /*
    ** try to open the file.  Append script file extension if not present.
    */

    len = strlen(name);
    if (len < 2 || name[len-1] != 'g' || name[len-2] != '.')
      {
	sprintf(extname, "%s.g", name);
	name = extname;
      }

    if((fp = fopen(name,mode)) != NULL){
	/*
	** is it a real file?
	*/
	if(!IsFile(fp)){
	    return(NULL);
	}
    }
    return(fp);
}

/*
** search the paths given in SIMPATH environment variable
** if the SIMPATH variable is not found then just search the
** current directory
*/
FILE *SearchForScript(name,mode)
char *name;
char *mode;
{
char prefix[300];
char *prefixptr;
char *path;
FILE *fp;
char *getenv();

    if(name == NULL) return(NULL);
    path = getenv("SIMPATH");
    fp = NULL;
    /*
    ** if there is no search path defined or it is an absolute
    ** or relative path then dont search
    */
    if((path == NULL) || (strchr(name, '/') != NULL)){
	/*
	** just look it up in current directory
	*/
	fp = OpenScriptFile(name,mode);
    } else {
	/*
	** follow the paths in order trying to find the file
	*/
	while(fp == NULL && path != NULL && *path != '\0'){
	    /*
	    ** skip white space
	    */
	    while(IsWhiteSpace(*path) && *path != '\0') path++;
	    /*
	    ** copy the path up to a space
	    */
	    prefixptr = prefix;
	    while(!IsWhiteSpace(*path) && *path != '\0') {
		*prefixptr++ = *path++;
	    }
	    *prefixptr = '\0';
	    /*
	    ** now try to open the file
	    */
	    if(prefix[0] != '\0'){
		strcat(prefix,"/");
		strcat(prefix,name);
		fp = OpenScriptFile(prefix,mode);
	    }
	}
    }
    return(fp);
}

void do_where(argc,argv)
int argc;
char **argv;
{
char prefix[300];
char tmp[300];
char *prefixptr;
char *path;
char *env;
FILE *fp;
char *getenv();
char *name;

    initopt(argc, argv, "script");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    name = optargv[1];
    env = path = getenv("SIMPATH");
    fp = NULL;
    if(path == NULL){
	/*
	** just look it up in current directory
	*/
	fp = fopen(name,"r");
	if(fp != NULL){
	    fclose(fp);
	    printf("'%s' found in current directory \n",name);
	    return;
	} else {
	    printf("could not find '%s' in current directory\n",name);
	    return;
	}
    } else {
	/*
	** follow the paths in order trying to find the file
	*/
	while(fp == NULL && path != NULL && *path != '\0'){
	    /*
	    ** skip white space
	    */
	    while(IsWhiteSpace(*path) && *path != '\0') path++;
	    /*
	    ** copy the path up to a space
	    */
	    prefixptr = prefix;
	    while(!IsWhiteSpace(*path) && *path != '\0') {
		*prefixptr++ = *path++;
	    }
	    *prefixptr = '\0';
	    if(prefix[0] != '\0'){
		strcpy(tmp,prefix);
		strcat(tmp,"/");
		strcat(tmp,name);
		fp = fopen(tmp,"r");
	    }
	    if(fp != NULL){
		printf("'%s' found in %s\n",name,prefix);
		fclose(fp);
		return;
	    }
	}
    }
    printf("could not find '%s' in %s\n",name,env);
}

void do_source(argc,argv)
int argc;
char **argv;
{
FILE	*pfile;

    if(argc < 2){
	printf("usage: %s script [script-arguments]\n",argv[0]);
	return;
    }
    /*
    ** try to open the named script
    */
    if((pfile = SearchForScript(argv[1],"r")) != NULL){
	if(debug > 0){
	    printf("source %s\n",argv[1]);
	}
	/*
	** activate the script and pass it the remaining
	** arguments on the command line
	*/
	AddScript((char *)NULL, pfile, argc - 1, argv + 1, FILE_TYPE);
    } else {
	Error();
	printf("unable to open file %s\n",
	argv[1]);
    }
}

int IncludeScript(argc,argv)
int 	argc;
char	**argv;
{
FILE	*pfile;

    /*
    ** try to open it as a script
    */
    if((pfile = SearchForScript(argv[0],"r")) != NULL){
	AddScript((char *)NULL, pfile, argc, argv, FILE_TYPE);
	return(1);
    }
    return(0);
}

int IsInclude(s)
char	*s;
{
FILE	*pfile;

    /*
    ** try to open it as a script
    */
    if((pfile = SearchForScript(s,"r")) != NULL){
	fclose(pfile);
	return(1);
    }
    return(0);
}


/*
** search the paths given in PATH environment variable
** if the PATH variable is not found then just search the
** current directory
*/
FILE *SearchForExecutable(name,mode,pathname)
char *name;
char *mode;
char **pathname;
{
char prefix[300];
char *prefixptr;
char *path;
FILE *fp;
char *getenv();

    if(name == NULL) return(NULL);
    path = getenv("PATH");
    fp = NULL;
    /*
    ** if there is no search path defined or it is an absolute
    ** or relative path then dont search
    */
    if((path == NULL) || (strchr(name, '/') != NULL)){
	/*
	** just look it up in current directory
	*/
	if(!IsFile(fp = fopen(name,mode))){
	    if (fp != NULL){
		fclose(fp);
		fp = NULL;
	    }
	} else {
	    *pathname = CopyString(name);
	}
    } else {
	/*
	** follow the paths in order trying to find the file
	*/
	while(fp == NULL && path != NULL && *path != '\0'){
	    /*
	    ** skip white space
	    */
	    while(IsWhiteSpace(*path) && *path != '\0') path++;
	    /*
	    ** copy the path up to a space
	    */
	    prefixptr = prefix;
	    while(!IsWhiteSpace(*path) && *path != '\0' &&
	    *path !=':') {
		*prefixptr++ = *path++;
	    }
	    if(*path == ':'){
		path++;
	    }
	    *prefixptr = '\0';
	    /*
	    ** now try to open the file
	    */
	    if(prefix[0] != '\0'){
		strcat(prefix,"/");
	    }
	    strcat(prefix,name);
	    if(!IsFile(fp = fopen(prefix,mode))){
		if(fp != NULL){
		    fclose(fp);
		    fp = NULL;
		}
	    } else {
		*pathname = CopyString(prefix);
	    }
	}
    }
    return(fp);
}

/*
** Searching for non-script files like the colorscales.
** search the paths given in SIMPATH environment variable
** if the SIMPATH variable is not found then just search the
** current directory
*/
FILE *SearchForNonScript(name,mode)
char *name;
char *mode;
{
char prefix[300];
char *prefixptr;
char *path;
FILE *fp;
char *getenv();

    if(name == NULL) return(NULL);
    path = getenv("SIMPATH");
    fp = NULL;
    /*
    ** if there is no search path defined or it is an absolute
    ** or relative path then dont search
    */
    if((path == NULL) || (strchr(name, '/') != NULL)){
	/*
	** just look it up in current directory
	*/
	fp = fopen(name,mode);
    } else {
	/*
	** follow the paths in order trying to find the file
	*/
	while(fp == NULL && path != NULL && *path != '\0'){
	    /*
	    ** skip white space
	    */
	    while(IsWhiteSpace(*path) && *path != '\0') path++;
	    /*
	    ** copy the path up to a space
	    */
	    prefixptr = prefix;
	    while(!IsWhiteSpace(*path) && *path != '\0') {
		*prefixptr++ = *path++;
	    }
	    *prefixptr = '\0';
	    /*
	    ** now try to open the file
	    */
	    if(prefix[0] != '\0'){
		strcat(prefix,"/");
		strcat(prefix,name);
		fp = fopen(prefix,mode);
	    }
	}
    }
    return(fp);
}


/*
** Searching for a file on the genesis path, returns the
** full pathname of the file
*/
char *FindFileOnPath(name)
char *name;
{
static char prefix[300];
char *prefixptr;
char *path;
FILE *fp;
char *getenv();
 
    if(name == NULL) return(NULL);
    path = getenv("SIMPATH");
    fp = NULL;
    /*
    ** if there is no search path defined or it is an absolute
    ** path then dont search
    */
    if((path == NULL) || (name[0] == '/')){
	/*
	** just look it up in current directory
	*/
	fp = fopen(name,"r");
	if (fp) {
	    /* This is stupid, opening a file and then closing it,
	    ** but it is easy */
	    fclose(fp);
	    return(name);
	}
    } else {
	/*
	** follow the paths in order trying to find the file
	*/
	while(fp == NULL && path != NULL && *path != '\0'){
	    /*
	    ** skip white space
	    */
	    while(IsWhiteSpace(*path) && *path != '\0') path++;
	    /*
	    ** copy the path up to a space
	    */
	    prefixptr = prefix;
	    while(!IsWhiteSpace(*path) && *path != '\0') {
	    *prefixptr++ = *path++;
	    }
	    *prefixptr = '\0';
	    /*
	    ** now try to open the file
	    */
	    if(prefix[0] != '\0'){
		strcat(prefix,"/");
		strcat(prefix,name);
		fp = fopen(prefix,"r");
		if (fp) {
		    fclose(fp);
		    return(prefix);
		}
	    }
	}
    }
    return(NULL);
}
