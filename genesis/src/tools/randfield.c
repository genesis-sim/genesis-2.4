static char rcsid[] = "$Id: randfield.c,v 1.3 2005/07/20 20:02:04 svitak Exp $";

/*
** $Log: randfield.c,v $
** Revision 1.3  2005/07/20 20:02:04  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:10  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  1999/10/16 23:53:54  mhucka
** Added fixed from Dave Bilitch and Dave Beeman for problems involving
** numerical quantities in CreateMap.
**
** Revision 1.4  1998/07/22 00:06:56  dhb
** Changed do_random_field() -exponential option to work as
** for planardelay.
**
** Revision 1.3  1997/07/18 03:11:14  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/24 23:46:34  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:03:40  dhb
** Initial revision
**
*/

#include <math.h>
#include "tool_ext.h"
#include "shell_func_ext.h"

/* 1/89 Matt Wilson */
void do_random_field(argc,argv)
int 	argc;
char 	**argv;
{
char 		val[80];
float 		lower = 0.0;
float 		upper = 0.0;
float 		mean = 0.0;
float 		sd;
float		mid = 0.0;
float		max = 0.0;
float 		var = 0.0;
double		value;
char 		*field;
char		*path;
ElementList	*list;
int		i;
int		mode;
int		status;

    mode = -1;
    initopt(argc, argv, "path field -uniform low high -gaussian mean sd -exponential mid max");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	/*
	** check the distribution options
	*/
	if(strcmp(G_optopt,"-uniform") == 0){
	    mode = 0;
	    lower = Atof(optargv[1]);
	    upper = Atof(optargv[2]);
	} else if(strcmp(G_optopt,"-gaussian") == 0){
	    mode = 1;
	    mean = Atof(optargv[1]);
	    sd = Atof(optargv[2]);
	    var = sd*sd;
	} else if(strcmp(G_optopt,"-exponential") == 0){
	    mode = 2;
	    mid = Atof(optargv[1]);
	    max = Atof(optargv[2]);
	}
      }

    if(status < 0 || mode == -1){
	printoptusage(argc, argv);
	printf("\n");
	printf("One of the command options must be given.\n");
	return;
    }

    path = optargv[1];
    field = optargv[2];

    /*
    ** make the element list
    */
    if((list = WildcardGetElement(path,1)) == NULL){
	InvalidPath(optargv[0],path);
	return;
    }
    /*
    ** set the fields
    */
    switch(mode){
    case 0:			/* uniform */
	for(i=0;i<list->nelements;i++){
	    sprintf(val,"%e",frandom(lower,upper));
	    SetElement(list->element[i],field,val);
	}
	break;
    case 1:			/* gaussian */
	for(i=0;i<list->nelements;i++){
	    sprintf(val,"%e",rangauss(mean,var));
	    SetElement(list->element[i],field,val);
	}
	break;
    case 2:			/* exponential */
	for(i=0;i<list->nelements;i++){
	    do
	      {
		double	x,y;

		do
		  x = urandom();  /* a random number between 0 and 1 */
		while (x == 0.0); /* don't want to take the log of 0 */
		y = -log(x);

		/*
		 * y is exponentially distributed between 0 and infinity,
		 * with the 1/e point at 1.0.
		 */

		value = mid * y;  /* now the 1/e point is at arg1 */
	      }
	    while (value > max);

	    sprintf(val,"%e",value);
	    SetElement(list->element[i],field,val);
	}
	break;
    }
    FreeElementList(list);
    OK();
}
