static char rcsid[] = "$Id: code_lib.c,v 1.5 2005/06/27 21:53:17 svitak Exp $";

/*
** $Log: code_lib.c,v $
** Revision 1.5  2005/06/27 21:53:17  svitak
** Removed void from FUNC_ function call.
**
** Revision 1.4  2005/06/27 19:01:15  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.3  2005/06/25 22:04:40  svitak
** Added explicit return types to untyped functions. Fixed return statements.
**
** Revision 1.2  2005/06/20 21:20:15  svitak
** Fixed compiler warnings re: unused variables and functions with no return type.
** Default of int causes compiler to complain about return with no type, so void
** used as return type when none present.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.7  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.6  2000/10/09 23:02:14  mhucka
** Portability fixes.
**
** Revision 1.5  2000/09/21 19:44:31  mhucka
** Added parens around assignments inside if/then conditionals to please gcc.
**
** Revision 1.4  2000/06/12 04:55:20  mhucka
** Fixed trivial errors in some printf format strings.
**
** Revision 1.3  1994/03/19 23:28:07  dhb
** Added LIBRARY initialization code to run startup function generated
** from the library startup script.
**
** Revision 1.2  1993/10/16  00:29:38  dhb
** Last line of code_lib's output was not newline terminated!
**
** Revision 1.1  1992/12/11  19:05:27  dhb
** Initial revision
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main(argc,argv)
     int argc;
     char **argv;
{
  FILE	*outfile;
  FILE	*fp;
  int	nxtarg;
  char	*libname;
  char	name[100];
  char	date[100];
  time_t clock;
  int	i;

  if(argc < 2){
    printf("usage: %s libraryname [-o file]\n", argv[0]);
    return 0;
  }
  libname = argv[1];
  nxtarg = 1;
  outfile = stdout;
  while(++nxtarg < argc){
    if(strcmp(argv[nxtarg],"-o") == 0){
      if((outfile = fopen(argv[++nxtarg],"w")) == NULL){
	printf("unable to open file '%s' for writing\n",argv[nxtarg]);
	return 0;
      }
    }
  }
  fprintf(outfile,"LIBRARY_%s()\n{\n",libname);
  /*
   ** get the current date
   */
  time(&clock);
#if defined(__APPLE__)
  strcpy(date,(const char *)ctime(&clock));
#else
  strcpy(date,ctime(&clock));
#endif
  /*
   ** remove an carriage returns
   */
  for(i=0;i<strlen(date);i++){
    if(date[i] == '\n'){
      date[i] = ' ';
    }
  }
  /*
   ** make the header
   */
  fprintf(outfile,"LibraryHeader(\"%s\",",libname);
  fprintf(outfile,"\"%s\");",date);
  
  /*
   ** is there a data section?
   */
  sprintf(name,"%s_d@.c",libname);
  if((fp = fopen(name,"r"))){
    fprintf(outfile,"DATA_%s();\n",libname);
  }
  
  /*
   ** is there a function section?
   */
  sprintf(name,"%s_f@.c",libname);
  if((fp = fopen(name,"r"))){
    fprintf(outfile,"FUNC_%s();\n",libname);
  }
  
  /*
   ** is there a startup section?
   */
  sprintf(name,"%s_g@.c",libname);
  if((fp = fopen(name,"r"))){
    fprintf(outfile,"STARTUP_%s();\n",libname);
  }
  fprintf(outfile,"}\n");
  return(0);
}
