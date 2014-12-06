static char rcsid[] = "$Id: shell_convert.c,v 1.4 2005/07/20 20:02:03 svitak Exp $";

/*
** $Log: shell_convert.c,v $
** Revision 1.4  2005/07/20 20:02:03  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.3  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.2  2005/06/18 19:09:04  svitak
** Include __APPLE__ on the list of architectures that has tolower().
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.9  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.8  1997/07/18 02:55:46  dhb
** Fix to getopt problem.  getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.7  1996/06/06 20:46:50  dhb
** merged in 1.5.1.1 changes.
**
** Revision 1.6  1996/05/23  23:15:45  dhb
** t3d/e port
**
** Revision 1.5.1.1  1996/06/06  20:05:49  dhb
** Paragon port.
**
** Revision 1.5  1995/07/14  01:57:16  dhb
** so_set_float_format() no longer allows the %d format to be given
** as none of the code using the float format knows to cast to int
** before doing a %d format.
**
** Revision 1.4  1995/06/16  05:59:42  dhb
** FreeBSD compatibility.
**
** Revision 1.3  1995/02/22  19:18:19  dhb
** Linux support.
**
** Revision 1.2  1993/02/23  23:56:13  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:04:23  dhb
** Initial revision
**
*/

#include <math.h>
#include "shell_ext.h"
#include "sim_func_ext.h"
#include "ss_func_ext.h"

float Atof(s)
char *s;
{
float f;
    if(s != NULL && sscanf(s,"%f",&f) >0){
	return(f);
    } else
	return(0.0);
}

double Atod(s)
char *s;
{
double d;
    if(s != NULL && sscanf(s,"%lf",&d) >0){
	return(d);
    } else
	return(0.0);
}

#if !defined(mips) && !defined(Linux) && !defined(__FreeBSD__) && !defined(__APPLE__)
char tolower(c) 
char	c;
{
	if((c >= 'A') && (c <= 'Z')) return((c) | 0x20);
	else return(c);
}
#endif

#ifndef i860
char *itoa(val)
int val;
{
char string[80];
char *val_str;

    sprintf(string,"%d",val);
    val_str = CopyString(string);
    return(val_str);
}
#endif

char *ftoa(val)
float val;
{
char string[80];
char *val_str;

    if(fabs(val) < 1e-5){
	sprintf(string,"%e",val);
    } else {
	sprintf(string,"%f",val);
    }
    val_str = (char *)malloc(strlen(string)+1);
    strcpy(val_str,string);
    return(val_str);
}


void do_set_float_format(argc,argv)
	int argc;
	char **argv;
{
        char * strrchr();	/* for CRAY and paragon */
	char *format;
	char formtype;

	initopt(argc, argv, "format");
	if (G_getopt(argc, argv) != 0)
	  {
	    printoptusage(argc, argv);
	    printf("\texample : %s %%0.5g\n",optargv[0]);
	    printf("\tonly f and g formats are allowed\n");
	    return;
	  }

	format = optargv[1];
	formtype = format[strlen(format)-1];

	if (format[0] != '%' || strrchr(format, '%') != format ||
	    (formtype != 'f' && formtype != 'g')) {
		printoptusage(argc, argv);
		printf("\texample : %s %%0.5g\n",optargv[0]);
		printf("\tonly f and g formats are allowed\n");
		return;
	}

	set_float_format(format);
	sim_set_float_format(format);
}
