static char rcsid[] = "$Id: shell_edit.c,v 1.4 2005/06/27 21:19:30 svitak Exp $";

/*
** $Log: shell_edit.c,v $
** Revision 1.4  2005/06/27 21:19:30  svitak
** Added {} around fildes to allow it to be declared only when needed.
**
** Revision 1.3  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.2  2005/06/18 19:10:03  svitak
** Added error checking and typecast for mktemp call.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2001/06/29 21:23:19  mhucka
** Use the safer mkstemp where available.
**
** Revision 1.2  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.1  1992/12/11 19:04:25  dhb
** Initial revision
**
*/

#include "shell_ext.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DEFAULT_EDITOR "vi"

char *EditString(s,editor)
char *s;
char *editor;
{
char *tmpname = NULL;
FILE *fp;
char template[20];
char com[100];
char *buf;
struct stat stbuf;
off_t fsize;

    if(editor == NULL){
	editor= DEFAULT_EDITOR;
    }
    /*
    ** write out the string to a temporary file
    */
    strcpy(template,"/tmp/genXXXXXX");
#if defined(Linux) || defined(Solaris) || defined(irix)
    /*
     * mktemp() is less secure than mkstemp(), but not all systems
     * have mkstemp(), hence the conditional compilation.  The list of
     * platforms in the #if above is incomplete.
     */
	{
    int fildes; 
    if ((fildes = mkstemp(template)) < 0
	|| (fp = fdopen(fildes, "w")) == NULL) {
	printf("cannot make temporary file\n");
	return(NULL);
    }
    strcpy(tmpname, template);
	}
#else
    if ((tmpname = (char *)mktemp(template)) == NULL
	|| (fp = fopen(tmpname,"w")) == NULL) {
	printf("cannot make temporary file\n");
	return(NULL);
    }
#endif
    if(s){
	fwrite(s,1,strlen(s),fp);
    }
    fclose(fp);
    /*
    ** run the editor on the file
    */
    sprintf(com,"%s %s",editor,tmpname);
    system(com);
    /*
    ** copy the results back into the string
    */
    if((fp = fopen(tmpname,"r")) == NULL){
	printf("temporary file has vanished. No changes made\n");
	return(NULL);
    }
    /*
    ** load the file into the buffer
    */
    fstat(fileno(fp),&stbuf);
    fsize = stbuf.st_size;
    buf = (char *)calloc(fsize+1,1);
    fread(buf,fsize,1,fp);
    fclose(fp);

    if((s != NULL) && (fsize <= (off_t)strlen(s))){
	/*
	** copy the buffer into the original string
	*/
	strcpy(s,buf);
	free(buf);
	return(NULL);
    } else {
	/*
	** if the fsize is greater than the string size
	** then send back the reallocated string
	*/
	return(buf);
    }
}
