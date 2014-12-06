/* $id$ */
/*
 * $Log: xo_callb.c,v $
 * Revision 1.2  2005/07/01 10:02:59  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1  2005/06/17 21:23:52  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.14  2001/06/29 21:12:40  mhucka
 * Added extra parens inside conditionals and { } groupings to quiet certain
 * compiler warnings.
 *
 * Revision 1.13  2001/04/25 17:16:58  mhucka
 * Misc. small changes to improve portability and address compiler warnings.
 *
 * Revision 1.12  2000/09/21 19:30:32  mhucka
 * An sprintf format arg in xoCallbackFn() didn't specify "long" as the
 * type of the arg.
 *
 * Revision 1.11  2000/06/12 04:28:22  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.10  1998/07/15 06:29:11  dhb
 * Upi update
 *
 * Revision 1.9  1997/07/16 21:26:43  dhb
 * Changed getopt() name to avoid conflicts with unix version
 *
 * Revision 1.8  1995/07/14 20:17:23  venkat
 * Replaced the lastdata change with a lastinfo structure. The intention is to
 * retain the selected pix information from the drag window. So even if the
 * info pointer were changed across the DRAG-DROP the `wasdropped` action should
 * still get the information from the drag-window. So the correct way should be
 * to make a static XoEVentInfo structure and locally copy the drag info.
 *
 * Revision 1.7  1995/07/08  01:21:01  venkat
 * Fix in xoExecuteFunction() to index the correct selected element in
 * the WASDROPPED action. This was causing a core dump when a selected pix
 * was dragged and dropped at another window.
 *
 * Revision 1.6  1994/06/30  21:56:51  bhalla
 * Added code to handle the keypress events and return suitable args.
 *
 * Revision 1.5  1994/03/16  16:53:28  bhalla
 * Fixed xoExectuteFunction so that after doing a XOWASDROPPED recurse,
 * it changes back to the XODROP action.
 * Added the do_callfunc function which allows one to call a function whose
 * name is specified by a string.
 *
 * Revision 1.4  1994/02/02  20:26:05  bhalla
 * Changed script calling syntax to use options in angle brackets.
 * Added stuff to handle drags between subparts of a complex pix
 * Added angle bracket options for values of draggee and droppee
 *
 * Revision 1.4  1994/02/02  20:26:05  bhalla
 * Changed script calling syntax to use options in angle brackets.
 * Added stuff to handle drags between subparts of a complex pix
 * Added angle bracket options for values of draggee and droppee
 *
 * Revision 1.3  1994/01/31  17:35:22  bhalla
 * This version is Caltech current version
 * */

#include <string.h>
#include "sim_ext.h"
#include "shell_func_ext.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "xo_ext.h"
#include "../Xo/XoDefs.h"

#define XONONE 0

char *xoFindNextScript();

/* This function sets up the callbacks from widgets
** to elements in a stereotyped manner.
*/
void xoCallbackFn(w,client_data,call_data)
	Widget w;
	XtPointer client_data,call_data;
{
	Element	*elm = (Element *) client_data;
	Action		action;
	XoEventInfo	*info = (XoEventInfo *) call_data;
	int 		i;
	char		*targs[10];
	int			atype = 0,btype = 0;
	int			but1=0,but2=0,but3=0,anybut=0;
	static int	lastevent=0;
	static long	lasttime=0;
	static char	targs_event[20];
	static char	targs_x[20];
	static char	targs_y[20];
	static char	targs_z[20];

	/*
	printf("In xoCallbackFn with event %d\n",info->event);
	*/
	/* converting the Xo events to xo actions */
	if (info->event & XoBut1 && !(info->event & (XoBut2 | XoBut3)))
		but1 = 1;
	if (info->event & XoBut2 && !(info->event & (XoBut1 | XoBut3)))
		but2 = 1;
	if (info->event & XoBut3 && !(info->event & (XoBut2 | XoBut1)))
		but3 = 1;
	if (info->event & (XoBut1 | XoBut2 | XoBut3))
		anybut = 1;

/* Assume that the remaining events are exclusive */
	if (info->event & XoPress) {
		atype=(but1) ? B1DOWN : ((but2) ? B2DOWN : 
			((but3) ? B3DOWN : XONONE));
		btype= anybut ? ANYBDOWN : XONONE;
	}
	if (info->event & XoRelease) {
		atype=(but1) ? B1UP : ((but2) ? B2UP : 
			((but3) ? B3UP : XONONE));
		btype= anybut ? ANYBUP : XONONE;
	}
	if (info->event & XoDouble) {
		atype=(but1) ? B1DOUBLE : ((but2) ? B2DOUBLE : 
			((but3) ? B3DOUBLE : XONONE));
		btype= anybut ? ANYBDOUBLE : XONONE;
	}
	if (info->event & XoMove) {
		atype=(but1) ? B1MOVE : ((but2) ? B2MOVE : 
			((but3) ? B3MOVE : NOBMOVE));
		btype= anybut ? ANYBMOVE : XONONE;
	}

	if (info->event & XoEnter) {
		if (lastevent == XoDrag && lasttime == info->key)
			atype = XODROP;
		else 
			atype=(but1) ? B1ENTER : ((but2) ? B2ENTER : 
				((but3) ? B3ENTER : NOBENTER));
		btype= anybut ? ANYBENTER : XONONE;
	}
	if (info->event & XoLeave) {
		atype=(but1) ? B1LEAVE : ((but2) ? B2LEAVE : 
			((but3) ? B3LEAVE : NOBLEAVE));
		btype= anybut ? ANYBLEAVE : XONONE;
	}
	if (info->event & XoKeyPress) {
		atype=KEYPRESS;
		btype=XONONE;
	}
	if (info->event & XoUpdateFields) {
		atype=UPDATEFIELDS;
		btype=XONONE;
	}
	if (info->event & XoDrag) {
		atype = XODRAG;
		btype = XONONE;
		lastevent = info->event;
		lasttime = info->key;
	}
	if (info->event & XoDrop) {
		if (lastevent == XoDrag && lasttime == info->key)
			atype = XODROP;
		btype= XONONE;
	}
	/* Sometimes a leave event intervenes and messes things up */
	if (!(info->event & XoLeave))
		lastevent = info->event;

/* Using pre_allocated strings to avoid memory leaks */
/* Note that the responsibility for the allocation of info->ret is
** with the widget that passed it in */
	if (atype || btype) {
		action.data = (char *)info;
		i=0;
		targs[i] = info->ret; i++;
		if (atype == KEYPRESS)
			sprintf(targs_event,"%ld",info->key);
		else 
			sprintf(targs_event,"%d",info->event);
		targs[i] = targs_event;
		i++;
		if (!(info->x == 0.0 && info->y == 0.0 && info->z == 0.0)) {
			sprintf(targs_x,"%f",info->x); targs[i] = targs_x; i++;
			sprintf(targs_y,"%f",info->y); targs[i] = targs_y; i++;
			sprintf(targs_z,"%f",info->z); targs[i] = targs_z; i++;
		}
		action.argc=i;
		action.argv=targs;
	}


/*
	if (atype || btype) {
		i=0;
		targs[i]=CopyString(info->ret); i++;
		if (atype == KEYPRESS)
			targs[i] = itoa(info->key);
		else 
			targs[i]=itoa(info->event);
		i++;
		if (!(info->x == 0.0 && info->y == 0.0 && info->z == 0.0)) {
			targs[i]=ftoa(info->x); i++;
			targs[i]=ftoa(info->y); i++;
			targs[i]=ftoa(info->z); i++;
		}
		action.argc=i;
		action.argv=targs;
	}
*/

	if (atype != XONONE) {
		action.type=atype;
		CallActionFunc(elm,&action);
	}

	if (btype != XONONE) {
		action.type=btype;
		CallActionFunc(elm,&action);
	}
}


/*
** This function invokes one or several script functions from
** an xodus event. Script argument lists are terminated by a ';'
** (semicolon). Any script function name with a .xxx suffix is 
** executed only when the appropriate X action occurs. The absence
** of any suffix is equivalent to .u or .d (any button). 
** The event to name translations are:
** b1,b2,b3 : button down events
** u1,u2,u3 : button up events
** d1,d2,d3 : double click events
** m1,m2,m3 : motion events (these are restricted to a few widgets)
** k[a-z,etc]: keyboard character events
** drag,drop : Drag and Drop events. -- need some thought
** e1,e2,e3 : Enter events
** l1,l2,l3 : Enter events
** ###		: Direct mapping to event number. Should be used sparingly.
**
** Every script function has the following arguments:
** func(x,x,....,x,self,value,special)
** where x is a value defined in the arg list for the script,
** self is the name of the widget,
** value is the value being returned by the xodus object. If there
** 						is none, this field contains a blank.
** The special arg values depend on the event.
**				key : key character
**				button/motion : x,y,z of click point in space
**				Drag: None
**				Drop: path of drag (src) widget
**
** 	Drag and drop events work like this.
**	1. The drag and drop functs are checked for in the src and dest
**	widgets.
** 	2. The drag function is called for the source widget.
**	3. The drop function is called for the dest
**	widget with extra arg being the path of the src widget.
**
**	Special arg options are selected as follows:
**		Identifier	Meaning
**		w			widget: as before, full path of calling widget
**		n			name: alias for widget, ie path of calling widget
**		v			value: value passed from calling widget
**		s			src: source widget for drag-drops
**		d			dst: destination widget for drag-drops
**		x			x coordinate of event
**		y			y coordinate of event
**		z			z coordinate of event
*/

void xoExecuteFunction(elm,action,script,value)
	Element *elm;
	Action	*action;
	char	*script;
	char	*value;
{
	char	*name;
	char	*tpathname;
	char	sname = '\0';
	char	scriptname[80];
	static char lastpath[200];
	static char lastval[200];
	static XoEventInfo lastinfo; /* holds a shallow copy of the info structure to retain the drag-data for 'wasdropped' events */ 
	char	*xcoord = "0"; /* x coord passed in for some events */
	char	*ycoord = "0";
	char	*zcoord = "0";
	char	*src="";	/* src widget passed in for drag-drop events*/
	char	*dest="";	/* dest widget passed in for drag-drop events*/
	char	*srcval="";	/* src value passed in for drag-drop events*/
	char	*destval=""; /* dest value passed in for drag-drop events*/
	char	tscript[1000];
	char*	temp = tscript;
	char*	temp2;
	char*	ret[10];
	int		nret = 0;
	int		argc = 0;
	char	*argv[50];
	int		targc = 0;
	char	*targv[50];
	int		len;
	int		i,j;
	int		doit;
	Element	*tempelm;
	char	*val;
	char	*key = "";
	if (script == NULL) {
		if (action->type == XODRAG) {
			strcpy(lastpath,Pathname(elm)); /* set it up for drop */
			if ((val = FieldValue(elm,elm->object,"value")))
				strcpy(lastval,val);
			else
				lastval[0] = '\0';
			lastinfo = *((XoEventInfo*)(action->data));
		}
	} else {
		for(i=0;i<8;i++) ret[i] = NULL;

		tpathname= CopyString(Pathname(elm));
	
		/* Setting up the name string to search for */
		switch (action->type) {
			case B1DOWN : name = "d1"; 
				break;
			case B2DOWN : name = "d2"; 
				break;
			case B3DOWN : name = "d3"; 
				break;
			case B1UP : name = "u1"; 
				break;
			case B2UP : name = "u2"; 
				break;
			case B3UP : name = "u3"; 
				break;
			case B1DOUBLE : name = "D1"; 
				break;
			case B2DOUBLE : name = "D2"; 
				break;
			case B3DOUBLE : name = "D3"; 
				break;
			case B1MOVE : name = "m1"; 
				break;
			case B2MOVE : name = "m2"; 
				break;
			case B3MOVE : name = "m3"; 
				break;
			case B1ENTER : name = "e1"; 
				break;
			case B2ENTER : name = "e2"; 
				break;
			case B3ENTER : name = "e3"; 
				break;
			case B1LEAVE : name = "l1"; 
				break;
			case B2LEAVE : name = "l2"; 
				break;
			case B3LEAVE : name = "l3"; 
				break;
			case KEYPRESS :
				sname = 'k';
				key = action->argv[1];
				name = "key";
				break;
			case XUPDATE : name = "x";
			/* this is identical to UPDATEFIELDS */
				break;
			case XODRAG : name = "drag";
				sname = 'y';
				strcpy(lastpath,Pathname(elm)); /* set it up for drop */
				if ((val = FieldValue(elm,elm->object,"value")))
					strcpy(lastval,val);
				else
					lastval[0] = '\0';
				src = lastpath;
				dest = lastpath;
				srcval = destval = lastval;
				lastinfo = *((XoEventInfo*)(action->data));
				break;
			case XODROP : name = "drop";
				sname = 'p';
				src = lastpath;
				dest = tpathname;
				srcval = lastval;
				if (!(destval = FieldValue(elm,elm->object,"value")))
					destval = "";
				break;
			case XOWASDROPPED: name = "wasdropped";
				src = tpathname;
				dest = lastpath;
				/*
				** Note that the lastval is the same as for DROP:
				** it still refers to the src, and we have not
				** messed with it. However, the
				** destval needs to look up the element on
				** which the drop event took place, which is
				** now identified in 'dest'.
				*/
				srcval = lastval;
				tempelm = GetElement(dest);
				if (!(destval =
					FieldValue(tempelm,tempelm->object,"value")))
					destval = "";
				break;
			case XOCOMMAND : name = "command";
				sname = 'c';
				for(i = 0 ; i < action->argc ; i++) {
					ret[nret] = action->argv[i]; nret++;
				}
				break;
			default:
				return;
		}
		if (strcmp(name,"drag") != 0 && strcmp(name,"drop") != 0)
			sname = name[0];
	
		/* Set up x,y,z return values for Press,Release,Move,Double events*/
		if (action->argc >= 5 && 
		(sname == 'd' || sname == 'u' || sname == 'm' || sname == 'p' || sname == 'w')) {
			xcoord = action->argv[2];
			ycoord = action->argv[3];
			zcoord = action->argv[4];
		}
	
		strcpy(tscript,script);
	
		while (temp){
			temp = xoFindNextScript(temp,&argc,argv,scriptname);
			doit = 0;
			/* The first possibility is that the script has no
			** suffix, in which case it will only run if it is a button
			** press event. Otherwise it needs to have the specified
			** string as a suffix */
			/* Default: exec script on any button down/up event */
			if ((temp2 = strchr(scriptname,'.')) == NULL && 
				(sname == 'd' || sname == 'u')) {
				doit =1;
			} else if (temp2 != NULL) {
				/* Check if the full event description is given */
				if ((int)strlen(temp2+1) > 1) {
					if (strcmp(temp2+1,name) == 0) doit = 1;
				/* Otherwise use the abbreviated single_letter version */
				} else {
					if (temp2[1] == sname) doit = 1;
				}
			}
	
			if (doit) {
				int k;
				/* for(j = 0; j < targc; j++) 
					targv[j] = argv[j]; */
	
				/* check for special args */
				for(j = 1; j < argc; j++) {
					/* check for places where we need to insert
					** special argument values, indicated by '<name>' */
					if (argv[j] && (k =strlen(argv[j])) > 2  &&
						argv[j][0] == '<' && argv[j][k-1]  == '>') {
						switch (argv[j][1]) {
						case 'w': /* insert pathname here*/
						case 'n':
					/* need to be careful about Pathnames since they
					** are statics and other funcs may change them */
							argv[j] = tpathname;
							break;

						case 'v': /* insert value here */
							if (value != NULL && (int)strlen(value) > 0)
								argv[j] = value;
							else
								argv[j] = "";
							break;
						case 's': /* insert src name here */
							argv[j] = src;
							break;
						case 'd': /* insert dest name here */
							argv[j] = dest;
							break;
						case 'S': /* insert src value here */
							argv[j] = srcval;
							break;
						case 'D': /* insert dest value here */
							argv[j] = destval;
							break;
						case 'x':	/* insert x coord here */
							argv[j] = xcoord;
							break;
						case 'y':	/* insert y coord here */
							argv[j] = ycoord;
							break;
						case 'z':	/* insert z coord here */
							argv[j] = zcoord;
							break;
						case 'k':	/* insert key here */
							argv[j] = key;
							break;
						}
					}
				}
				for(j = 0 ; j < nret ; j++) {
					argv[argc] = ret[j];
					argc++;
				}
				ExecuteFunction(argc,argv);
			}
		}
		free(tpathname);
	}
	/* A last recursive call if there was a drop: we need to
	** call the dropee's XOWASDROPPED action */
	/* This might be risky if elm is moved or deleted during
	** the function */
	if (action->type == XODROP) {
		Element *lastelm = elm;
		char *tempdata = action->data;
		/* swap over to the WASDROPPED action values */
		action->type = XOWASDROPPED;
		action->data = (char*) &lastinfo;
		name = Pathname(elm);
		elm = GetElement(lastpath);
		strcpy(lastpath,name);
		/* Note that we do NOT swap around the lastval, since
		** it might apply to the current pix if it is an xtree or
		** xcell. This is sorted out in the XOWASDROPPED case
		*/
		CallActionFunc(elm,action);
		/* restore the action values in case there are further
		** calls using this action */
		action->type = XODROP;
		action->data = tempdata;
		name = Pathname(elm);
		elm = lastelm;
		strcpy(lastpath,name);
	}
}


char *xoFindNextScript(str,argc,argv,scriptname)
	char *str;
	int	 *argc;
	char **argv;
	char *scriptname;
{
	int len =strlen(str);
	char	*endpt;
	char	*sc;
	int		i;

	*argc = 0;

	/* Eliminate leading spaces */
	while (*str == ' ')
		str++;

	/* Find end of function by looking for semicolon or terminator */
	endpt = strchr(str,';');
	if (endpt != NULL)
		*endpt = '\0';

	/* Chop up str into args */
	len = strlen(str);
	for(i=0;i<len;i++)
		if (str[i] == ' ' || str[i] == '	')
			str[i] = '\0';
	argv[0] = str; *argc=1;
	for(i=1;i<len;i++) {
		if (str[i-1] == '\0' && str[i] != '\0') {
			argv[*argc] = str+i;
			(*argc)++;
		}
	}

	/* copy out the entire script name */
	strcpy(scriptname,argv[0]);

	/* get rid of the '.' if present, in the script name */
	if ((sc = strchr(argv[0],'.')) != NULL)
		*sc = '\0';
	
	if (endpt == NULL)
		return(NULL);
	else
		return(endpt + 1);
}

void do_callfunc(argc,argv)
	int argc;
	char **argv;
{
	initopt(argc,argv,"funcname ...");
	if (G_getopt(argc,argv) != 0) {
		printoptusage(argc,argv);
		return;
	}
	/* get rid of the first arg, and call the func named by the second*/
	if (argc > 1)
		ExecuteFunction(argc-1,argv+1);
}

#undef XONONE
