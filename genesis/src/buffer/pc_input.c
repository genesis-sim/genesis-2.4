static char rcsid[] = "$Id: pc_input.c,v 1.3 2005/07/20 20:01:58 svitak Exp $";

/*
** $Log: pc_input.c,v $
** Revision 1.3  2005/07/20 20:01:58  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/06/27 18:59:57  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  1997/07/16 21:35:53  dhb
** Changed getopt() name to avoid conflict with unix version
**
** Revision 1.3  1994/09/20 18:43:44  dhb
** Moved from segment to buffer library.  Changed library extern include
** file.
**
** Also, added clearbuffer error handling.
**
** Revision 1.2  1993/02/25  18:45:51  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:04:08  dhb
** Initial revision
**
*/

#include <math.h>
#include "buf_ext.h"

void do_clear_buffer(argc,argv)
int 	argc;
char 	**argv;
{
char 		*pathname;
Buffer 		*buffer;

    initopt(argc, argv, "buffer-element");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    pathname = argv[1];
    buffer = (Buffer *)GetElement(pathname);
    if (buffer == NULL) {
	InvalidPath(argv[0], argv[1]);
    }
    else {
	if (CheckClass(buffer, BUFFER_ELEMENT))
	    buffer->end = buffer->current = buffer->start;
	else {
	    Error();
	    printf("%s is not a buffer element\n", Pathname(buffer));
	}
    }
}

void do_get_input (argc,argv) 
int	argc;
char	**argv;
{
float		xs,xe,ys,ye;
float		stim_time;
float		stim_magnitude;
float		random_input_p;
float		random_input_amp;
float		dt;
Event	 	event;
Element	 	*element;
Element	 	*parent;
Buffer	 	*buffer;
char 		*buffername;
short		nxtarg;
char		*pathname;
int 		stk;

    initopt(argc, argv, "parent-element ...");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    pathname = GetPathComponent(optargv[1]);
    buffername = GetFieldComponent(optargv[1]);
    parent = GetElement(pathname);
    if(parent == NULL){
	printf("could not find element '%s'\n",optargv[1]);
	return;
    }
    if(parent->child == NULL){
	printf("'%s' not a parent element\n",optargv[1]);
	return;
    }

    if (debug >0){
	printf ("%s %s\n",optargv[0],optargv[1]);
    }

    buffer = (Buffer*)malloc(sizeof(Buffer));

    /*
    * get a string from the parameter file which indicates
    * how to specify the input
    */
    nxtarg = 2;
    if(arg_is("RANDOM")){
	float   curtime,maxtime,starttime;
	xs = farg();
	ys = farg();
	xe = farg();
	ye = farg();
	while(xs >= 0) {
	    random_input_p = farg();
	    random_input_amp = farg();
	    starttime = farg();
	    maxtime = farg();
	    stk = PutElementStack(parent);
	    while((element = NextElement(1,BUFFER_ELEMENT,stk)) != NULL){
		if(element->x >= xs && element->x <= xe &&
		element->y >= ys && element->y <= ye){
		    curtime = starttime;
		    dt = Clockrate(buffer);
		    while(curtime < maxtime) {
			if (frandom (0, 1) <= random_input_p*dt) {
#ifdef FLOATTIME
			    event.time = curtime;
#else
			    event.time = curtime/dt;
#endif
			    event.magnitude = random_input_amp;
			    InsertEvent(buffer,&event,NULL, EXPAND);
			}
			curtime += dt;
		    }
		}
	    }
	    FreeElementStack(stk);
	    xs = farg();
	    ys = farg();
	    xe = farg();
	    ye = farg();
	}
    } else
    if (arg_is("RANDOM MODULATED") ) {
	float   curtime,maxtime,starttime;
	float	mod_p;
	float	min_p,max_p;
	float	period;
	float	phase;

	xs = farg();
	ys = farg();
	xe = farg();
	ye = farg();
	while(xs >= 0) {
	    max_p = farg();
	    min_p = farg();
	    starttime = farg();
	    maxtime = farg();
	    period = farg();
	    phase = farg();
	    stk = PutElementStack(parent);
	    while((element = NextElement(1,BUFFER_ELEMENT,stk)) != NULL){
		if(element->x >= xs && element->x <= xe &&
		element->y >= ys && element->y <= ye){
		    curtime = starttime;
		    dt = Clockrate(buffer);
		    while(curtime < maxtime) {
			/*
			* modulate the input prob as a function of time
			*/
			mod_p = min_p+(max_p - min_p)*
			(sin(2*PI*curtime/period+
			phase*2*PI/360.0)+1)/2;
			if (frandom (0, 1) <= mod_p) {
#ifdef FLOATTIME
			    event.time = curtime;
#else
			    event.time = curtime/dt;
#endif
			    event.magnitude = 1.0;
			    InsertEvent(buffer,&event,NULL, EXPAND);
			}
			curtime += dt;
		    }
		}
	    }
	    FreeElementStack(stk);
	    xs = farg();
	    ys = farg();
	    xe = farg();
	    ye = farg();
	}
    } else
    if (arg_is("DEFINE ALL")) {
	xs = farg();
	ys = farg();
	xe = farg();
	ye = farg();
	while(xs >= 0) {
	    while((stim_time = farg()) >= 0){
		stk = PutElementStack(parent);
		while((element = NextElement(1,BUFFER_ELEMENT,stk)) != NULL){
		    if(element->x >= xs && element->x <= xe &&
		    element->y >= ys && element->y <= ye){
#ifdef FLOATTIME
			event.time = stim_time;
#else
			event.time = stim_time/global_clock[buffer->clock];
#endif
			event.magnitude = 1.0;
			InsertEvent(buffer,&event,NULL, EXPAND);
		    }
		}
		FreeElementStack(stk);
	    }
	    xs = farg();
	    ys = farg();
	    xe = farg();
	    ye = farg();
	}
    } else
    if (arg_is("DEFINE STRENGTH"))  {
	xs = farg();
	ys = farg();
	xe = farg();
	ye = farg();
	while(xs >= 0) {
	    while((stim_time = farg())>= 0){
		stim_magnitude = farg();
		stk = PutElementStack(parent);
		while((element = NextElement(1,BUFFER_ELEMENT,stk)) != NULL){
		    if(element->x >= xs && element->x <= xe &&
		    element->y >= ys && element->y <= ye){
#ifdef FLOATTIME
			event.time = stim_time;
#else
			event.time = stim_time/global_clock[buffer->clock];
#endif
			event.magnitude = stim_magnitude;
			InsertEvent(buffer,&event,NULL, EXPAND);
		    }
		}
		FreeElementStack(stk);
	    }
	    xs = farg();
	    ys = farg();
	    xe = farg();
	    ye = farg();
	}
    } else
    if (arg_is("DEFINE PATTERN") ) {
	struct xy_coord {
	    float x;
	    float y;
	    float amp;
	} *pattern;
	int ncoords;
	int i;

	ncoords = iarg();
	/*
	** instead of a bounding region give a coordinate list
	*/
	pattern = (struct xy_coord *) malloc(ncoords*sizeof(struct xy_coord));
	for(i=0;i<ncoords;i++){
	    pattern[i].x = farg();
	    pattern[i].y = farg();
	    pattern[i].amp = farg();
	}
	while ((stim_time = farg()) >= 0) {
	    stk = PutElementStack(parent);
	    while((element = NextElement(1,BUFFER_ELEMENT,stk)) != NULL){
		for(i=0;i<ncoords;i++){
		    if(element->x == pattern[i].x && 
		    element->y == pattern[i].y){
#ifdef FLOATTIME
			event.time = stim_time;
#else
			event.time = stim_time/global_clock[buffer->clock];
#endif
			event.magnitude = pattern[i].amp;
			InsertEvent(buffer,&event,NULL, EXPAND);
			break;
		    }
		}
	    }
	    FreeElementStack(stk);
	}
	free(pattern);
    }
    else {
	Error();
	printf ("Error in input specification\n");
    }
    free((char*)buffer);
}
