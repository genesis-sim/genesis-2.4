static char rcsid[] = "$Id: pc_thresh.c,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $";

/*
** $Log: pc_thresh.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  1997/07/18 03:10:49  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/25 18:45:51  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:04:09  dhb
** Initial revision
**
*/

#include "seg_ext.h"
#ifdef LATER
do_get_thresh(argc,argv) 
int 	argc;
char 	**argv;
{
Element 	*element;
Element 	*parent;
Buffer		*buffer;
int 		nxtarg;
char		*buffer_name;

    initopt(argc, argv, "parent buffer mode ...");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    /*
    ** parent element
    */
    parent = GetElement(optargv[1]);
    buffer_name = optargv[2];

    if (trace >0){
	printf ("GetThresh %d\n",optargv[1]);
    }

    nxtarg = 3;
    if(arg_is("DEFINE ALL")){
	for(element=parent->child;element;element=element->next){
	    if(buffer = GetBuffer(element,buffer_name)){ 
		if( buffer->type && strcmp(buffer->type,"spike_type") == 0){
		    Specific(buffer,spike_type,thresh) = farg();
		} else {
		    Warning();
		    printf("buffer must be a 'spike_type' to set thresholds\n");
		}
	    }
	}
    } else
    /*
    * generate a random distribution of thresholds?
    */
    if(arg_is("RANDOM")){
	if(nxtarg + 1 >= argc){
	    Error();
	    printf ("Error in threshold specification");
	    return(0);
	}
	/*
	* normally distributed?
	*/
	nxtarg++;
	if(arg_is("NORMAL")){
	    float u,s;
	    u = farg();
	    s = farg();
	    for(element=parent->child;element;element=element->next){
		if(buffer = GetBuffer(element,buffer_name)){
		    if( buffer->type && strcmp(buffer->type,"spike_type") == 0){
			Specific(buffer,spike_type,thresh) = rangauss(u,s);
		    } else {
			Warning();
			printf("buffer must be a 'spike_type' to set thresholds\n");
		    }
		}
	    }
	} else
	/*
	* or uniformly distributed
	*/
	if(arg_is("UNIFORM")){
	    float	upper_thr;
	    float	lower_thr;
	    upper_thr = farg();
	    lower_thr = farg();
	    for(element=parent->child;element;element=element->next){
		if(buffer = GetBuffer(element,buffer_name)){
		    if( buffer->type && strcmp(buffer->type,"spike_type") == 0){
			Specific(buffer,spike_type,thresh) = 
			frandom(lower_thr,upper_thr);
		    } else {
			Warning();
			printf("buffer must be a 'spike_type' to set thresholds\n");
		    }
		}
	    }
	} else {
	    Error();
	    printf ("Error in threshold specification");
	    return(0);
	}
    } else {
	Error();
	printf ("Error in threshold specification");
	return(0);
    }
}

#endif
