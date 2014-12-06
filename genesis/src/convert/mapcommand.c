static char rcsid[] = "$Id: mapcommand.c,v 1.2 2005/06/27 19:00:33 svitak Exp $";

/*
** $Log: mapcommand.c,v $
** Revision 1.2  2005/06/27 19:00:33  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.10  2001/06/29 21:13:11  mhucka
** Added extra parens inside conditionals and { } groupings to quiet certain
** compiler warnings.
**
** Revision 1.9  2000/06/12 04:33:00  mhucka
** Added a type specifier where one was missing.
**
** Revision 1.8  1995/08/09 17:13:40  dhb
** Changed mapping of xaddpts to use the ADDPTS action on the xplot
** rather than the xaddpts X1compat script function.  The -color option
** is mapped to a setfield command on the xplot fg field.
**
 * Revision 1.7  1995/08/03  01:04:16  dhb
 * xinit commands will now be removed in converted scripts.
 *
 * Revision 1.6  1995/07/18  19:36:44  dhb
 * Mapping of xodus create command for shapes no longer
 * combines multiple arguments to the -coords option.
 *
 * Revision 1.5  1995/05/19  01:03:15  dhb
 * Fixed a bug in MapSet() where an sprintf() call was missing its
 * initial argument.  This resulted in erroneous code for "set -env"
 * commands to be generated.
 *
 * Revision 1.4  1995/04/12  00:52:41  dhb
 * Fixes and updates to set and get command mapping of -env option.
 *
 * 	Will warn that -env is being mapped to extended fields
 *
 * 	Will add code to add the field if it doesn't already exist.
 *
 * 	Set will now correctly pairs fields and value when an element
 * 	path is not present (e.g. setting field on current element).
 *
 * Revision 1.3  1995/04/11  23:54:41  dhb
 * Removed include of search.h which doesn't appear to be necessary
 * and is not present on some Linux systems.
 *
 * Revision 1.2  1995/02/01  23:01:03  dhb
 * Added support for toggling use of X1compat library using
 * -noX1compat switch.
 *
 * Revision 1.1  1995/01/13  01:09:48  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include <string.h>
#include "output.h"	/* InsertItemInArgList(), AddItemToArgList() */

/*
** mapcommand.c
**
**	Map commands from GENESIS 1.4.1 to GENESIS 2.0 command names and
**	options.
*/

#define arg_is(str)	(strcmp(argv[nxtarg], str) == 0)

extern char* Combine();

typedef struct _mapfunction
  {
    char*	cmd;
    char*	newcmd;
    int		(*mapfunc)();
    int		retstr;
  } MapFunction;

static int	UsingX1compat = 1;

/*
** SetX1compat
**
**	Function for setting use of X1compat
*/

void SetX1compat(newval)

int	newval;

{
	UsingX1compat = newval;
}


/*
** CountArgs
**
**	Return a count of the number of args in the NULL terminated argv.
*/

static int CountArgs(argv)

char**	argv;

{
	int	count;

	count = 0;
	while (argv[count] != NULL)
	    count++;

	return count;

}


/*
** All commands which are startup related come here.  The user is warned
** that startup scripts cannot be converted with convert and returns -1
** which will cause the command to be commented out of the script file.
*/

int StartupCmd(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	fprintf(stderr, "WARNING: The command '%s' is only used at startup\n",
								*pcmd);
	fprintf(stderr, "         and cannot be converted to GENESIS 2.0\n");
	fprintf(stderr, "         by convert.\n");

	return -1;
}

/*
** Any command which is to be removed can call RemoveCmd() which returns
** zero (e.g. command is not output).
*/

int RemoveCmd(pcmd, argv)

char**	pcmd;
char*	argv[];


{
	return 0;
}



int MapPosition(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	nxtarg;

	/*
	** In GENESIS 1.x the position command handled the ignoring of the
	** z component using a "-" argument instead of "I" as for x and y.
	** 2.0 uses "I".  NOTE: since we don't know the number of args in
	** the command we have to scan for the z arg (argv[3]).
	*/

	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	    if (nxtarg == 3)
	      {
		if (argv[3][0] == '-' && argv[3][1] == '\0')
		    argv[3] = "I";
		break;
	      }

	return 1;
}


int MapCreateMap(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	char            *wdx,*wdy;
	char            *wxmin,*wymin;
	char            *elementname;
	char            *parentname;
	int             nxtarg;
	int             outarg;

	wdx = NULL;
	wdy = NULL;
	wxmin = NULL;
	wymin = NULL;
	elementname = NULL;
	parentname = NULL;

	outarg = 0;
	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++){
	    if(elementname == NULL){
		if(strcmp(argv[nxtarg],"of") == 0) continue;
		elementname = argv[nxtarg];
		argv[outarg++] = elementname;
		continue;
	    } else
	    if(parentname == NULL){
		if(strcmp(argv[nxtarg],"on") == 0) continue;
		parentname = argv[nxtarg];
		argv[outarg++] = parentname;
		continue;
	    } else
	    break;
	}

	/*
	** set the map parameters
	*/
	argv[outarg++] = argv[nxtarg++]; /* nx */
	argv[outarg++] = argv[nxtarg++]; /* ny */

	if(argv[nxtarg] != NULL && argv[nxtarg+1] != NULL){
	    wdx = argv[nxtarg++];
	    wdy = argv[nxtarg++];
	}
	if(argv[nxtarg] != NULL && argv[nxtarg+1] != NULL){
	    wxmin = argv[nxtarg++];
	    wymin = argv[nxtarg++];
	}

	if (wdx != NULL)
	  {
	    argv[outarg++] = "-delta";
	    argv[outarg++] = wdx;
	    argv[outarg++] = wdy;
	  }

	if (wxmin != NULL)
	  {
	    argv[outarg++] = "-origin";
	    argv[outarg++] = wxmin;
	    argv[outarg++] = wymin;
	  }

	argv[outarg] = NULL;

	return 1;
}


int MapMove(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	short           nxtarg;
	char            *src;
	char            *dst;

	src = NULL;
	dst = NULL;
     
	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++){
	    if(src == NULL){
		src = argv[nxtarg];
		continue;
	    } else
	    if(dst == NULL){
		if(strcmp(argv[nxtarg],"to") == 0){
		    continue;
		}
		dst = argv[nxtarg];
		continue;
	    }
	}

	argv[1] = dst;
	argv[2] = NULL;

	return 1;
}



void SepGeom(geom, xpart, ypart, wpart, hpart)

char*	geom;
char**	xpart;
char**	ypart;
char**	wpart;
char**	hpart;

{
	char*	comp;

	comp = strtok(geom, ",");
	if (*xpart == NULL)
	  {
	    if (comp != NULL)
		comp++; /* strip the leading [ */
	    *xpart = comp;
	  }
	comp = strtok(NULL, ",");
	if (*ypart == NULL)
	    *ypart = comp;
	comp = strtok(NULL, ",");
	if (*wpart == NULL)
	    *wpart = comp;
	comp = strtok(NULL, ",");
	if (*hpart == NULL)
	  {
	    if (comp != NULL)
		comp[strlen(comp) - 1] = '\0'; /* strip trailing ] */
	    *hpart = comp;
	  }
}


void AddGeom(geom, part, defval, widg)

char*	geom;
char*	part;
char*	defval;
char*	widg;

{
	if (part == NULL || part[0] == '\0')
	    part = defval;

	strcat(geom, part);
	if (widg != NULL && widg[0] != '\0' && strcmp(widg, "NULL") != 0)
	  {
	    strcat(geom, ":");
	    strcat(geom, widg);
	  }
}


char* TransformGeom(ingeom, xwidg, ywidg, wwidg, hwidg)

char*	ingeom;
char*	xwidg;
char*	ywidg;
char*	wwidg;
char*	hwidg;

{
	static char	geom[1000];

	char		oldgeom[100];
	char*		xgeom = NULL;
	char*		ygeom = NULL;
	char*		wgeom = NULL;
	char*		hgeom = NULL;

	strcpy(oldgeom, ingeom);
	SepGeom(oldgeom, &xgeom, &ygeom, &wgeom, &hgeom);

	strcpy(geom, "[");

	AddGeom(geom, xgeom, "20", xwidg);
	strcat(geom, ",");

	AddGeom(geom, ygeom, "20", ywidg);
	strcat(geom, ",");

	AddGeom(geom, wgeom, "200", wwidg);
	strcat(geom, ",");

	AddGeom(geom, hgeom, "30", hwidg);

	strcat(geom, "]");

	return geom;
}


char* GetBracketedArgs(argv, pnxtarg)

char*	argv[];
int*	pnxtarg;

{	/* GetBracketedArgs --- Collect arguments delimited by square brackets*/

	extern char*	strsave();
	static char	buf[1000];

	char*		mem;
	int		nxtarg;

	nxtarg = *pnxtarg;

	strcpy(buf, argv[nxtarg]);
	while (argv[nxtarg+1] != NULL)
	  {
	    int	len;

	    len = strlen(argv[nxtarg]);
	    if (len > 0 && argv[nxtarg][len-1] == ']')
		break;

	    nxtarg++;
	    strcat(buf, argv[nxtarg]);
	  }

	*pnxtarg = nxtarg;
	mem = strsave(buf);
	return mem;

}	/* GetBracketedArgs */


int MapCreate(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	static char	coords[50000];
	static char	newtype[100];
	char		*coordoption;
	int             nxtarg;
	int             outarg;
	char            *type;
	char            *target;
	char		*geom;
	char		*widgets;
	char		*xwidget;
	char		*ywidget;
	char		*wwidget;
	char		*hwidget;
	int		is_xodus;
	int		is_pix;

	type = NULL;
	target = NULL;
	geom = NULL;
	widgets = NULL;
	xwidget = NULL;
	ywidget = NULL;
	wwidget = NULL;
	hwidget = NULL;
	coords[0] = '\0';
	coordoption = "-coords";
	is_xodus = 0;
	is_pix = 0;

	outarg = 0;
	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++){
	    if(type == NULL){
		if(strcmp(argv[nxtarg],"a")==0 || strcmp(argv[nxtarg],"an")==0){
		    continue;
		}
		type = argv[nxtarg];
		is_xodus = type[0] == 'x';
		is_pix = strcmp(type, "xshape") == 0;
		if (is_xodus && UsingX1compat)
		  {
		    sprintf(newtype, "x1%s", type+1);
		    argv[outarg++] = newtype;
		  }
		else
		    argv[outarg++] = type;
		continue;
	    } else
	    if(target == NULL){
		if(strcmp(argv[nxtarg],"called")==0){
		    continue;
		}
		target = argv[nxtarg];
		argv[outarg++] = target;
		continue;
	    } else
		if (is_xodus)
		  {
		    if (strcmp(argv[nxtarg], "-widgets") ==  0 &&
						    argv[nxtarg+1] != NULL)
		      {
			nxtarg++;
			widgets = GetBracketedArgs(argv, &nxtarg);
		      }
		    else if (strcmp(argv[nxtarg], "-xwidget") ==  0 &&
						    argv[nxtarg+1] != NULL)
			xwidget = argv[++nxtarg];
		    else if (strcmp(argv[nxtarg], "-ywidget") ==  0 &&
						    argv[nxtarg+1] != NULL)
			ywidget = argv[++nxtarg];
		    else if (strcmp(argv[nxtarg], "-wwidget") ==  0 &&
						    argv[nxtarg+1] != NULL)
			wwidget = argv[++nxtarg];
		    else if (strcmp(argv[nxtarg], "-hwidget") ==  0 &&
						    argv[nxtarg+1] != NULL)
			hwidget = argv[++nxtarg];
		    else if (strcmp(argv[nxtarg], "-coords") ==  0)
			argv[outarg++] = argv[nxtarg];
		    else if (strcmp(argv[nxtarg], "-range") ==  0)
		      {
			coordoption = argv[nxtarg];
			strcpy(coords, argv[++nxtarg]);
		      }
		    else if (argv[nxtarg][0] == '[')
		      {
			if (is_pix)
			    argv[outarg++] = argv[nxtarg];
			else
			    geom = GetBracketedArgs(argv, &nxtarg);
		      }
		    else
			argv[outarg++] = argv[nxtarg];
		  }
		else
		    argv[outarg++] = argv[nxtarg];
	}

	if (widgets != NULL)
	    SepGeom(widgets, &xwidget, &ywidget, &wwidget, &hwidget);
	    
	if (xwidget != NULL || ywidget != NULL ||
	    wwidget != NULL || hwidget != NULL) {
	    if (geom != NULL && geom[0] == '[')
		geom = TransformGeom(geom,xwidget,ywidget,wwidget,hwidget);
	    else
		geom = TransformGeom("[20,20,200,30]",xwidget,ywidget,wwidget,hwidget);
	}

	if (coords[0] != '\0')
	  {
	    argv[outarg++] = coordoption;
	    argv[outarg++] = coords;
	  }
	argv[outarg] = NULL;

	if (geom != NULL)
	    InsertItemInArgList(argv, 2, geom, 100);

	return 1;
}


int MapCopy(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int             nxtarg;
	char            *src;
	char            *dst;
	char            *repeat;
     
	repeat = NULL;
	src = NULL;
	dst = NULL;
     
	/*
	** locate the src/dst and the multiplier
	*/
	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++){
	    if(strcmp(argv[nxtarg],"-repeat") == 0){
		repeat = argv[++nxtarg];
		continue;
	    } else
	    if(src == NULL){
		src = argv[nxtarg];
		continue;
	    } else
	    if(dst == NULL){
		if(strcmp(argv[nxtarg],"to") == 0){
		    continue;
		}
		dst = argv[nxtarg];
		continue;
	    }
	}
 
	nxtarg = 0;
	argv[nxtarg++] = src;
	argv[nxtarg++] = dst;
	if (repeat != NULL)
	  {
	    argv[nxtarg++] = "-repeat";
	    argv[nxtarg++] = repeat;
	  }
	argv[nxtarg] = NULL;

	return 1;
}


int MapLe(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	char            *element;
	int             nxtarg;
	short           recursive = 0;
	short           showtype = 0;
     
	element = "";
	nxtarg = 0;
	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++){
	    /*
	    ** check for a recursive listing
	    */
	    if(arg_is("-R")){
		recursive = 1;
	    } else
	    if(arg_is("-t")){
		showtype = 1;
	    } else
	    if(argv[nxtarg][0] != '-') {
		element = argv[nxtarg];
	    }
	}

	nxtarg = 0;
	argv[nxtarg++] = element;

	if (recursive)
	    argv[nxtarg++] = "-recursive";

	if (showtype)
	    argv[nxtarg++] = "-types";

	argv[nxtarg] = NULL;

	return 1;
}


int MapStatus(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	if (argv[0] != NULL && (strcmp(argv[0], "-process") == 0 ||
				strcmp(argv[0], "-element") == 0))
	    *pcmd = "showstat";

	return 1;
}


int MapDate(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	if (argv[0] != NULL)
	    *pcmd = "date";

	return 1;
}


int MapStep(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	mode;
	int	verbose;
	int	bg;
	char*	duration;
	int	nxtarg;

	duration = "1";
	verbose = 0;
	mode = 0;
	bg = 0;

	nxtarg = 0;
	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	  {
	    if(strcmp(argv[nxtarg],"-t") == 0)
	      {
		if (argv[++nxtarg] == NULL)
		    break;
		mode = 1;
		duration = argv[nxtarg];
	      }
	    else if(strcmp(argv[nxtarg],"-v") == 0)
		verbose = 1;
	    else if(strcmp(argv[nxtarg],"&") == 0)
		bg = 1;
	    else if(argv[nxtarg][0] != '-')
	      {
		mode = 0;
		duration = argv[nxtarg];
	      }
	  }

	nxtarg = 0;
	argv[nxtarg++] = duration;
	if (mode == 1)
	    argv[nxtarg++] = "-time";

	if (verbose)
	    argv[nxtarg++] = "-verbose";

	if (bg)
	    argv[nxtarg++] = "-background";

	argv[nxtarg] = NULL;

	return 1;
}


int MapBgStep(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	AddItemToArgList(argv, "&", 100);

	return MapStep(pcmd, argv);
}


int MapRandomField(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	if (argv[0] == NULL)
	    return 1;

	if (strcmp(argv[2], "-exp") == 0)
	    argv[2] = "-exponential";

	return 1;
}


int MapRandomSeed(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	if (argv[0] == NULL)
	    return 1;

	if (strcmp(argv[0], "-new") == 0)
	    argv[0] = "";

	return 1;
}


static void WarnAboutEnvVars()

{
	static int warned_already = 0;

	if (!warned_already)
	  {
	    fprintf(stderr, "WARNING: use of 1.4 environment variables being\n");
	    fprintf(stderr, "changed to use extended fields.\n");
	    warned_already = 1;

	  }
}


int MapGet(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	arg;

	if (argv[0] == NULL)
	    return 1;

	arg = 0;
	if (strcmp(argv[0], "-env") == 0)
	  {
	    WarnAboutEnvVars();
	    argv[0] = "";
	    arg++;
	  }

	if (strchr(argv[arg], ':') != NULL)
	    *pcmd = "getconn";

	return 1;
}

int MapSet(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	fld;
	int	argc;
	int	addfields;
	int	arg;
	char*	elm;

	if (argv[0] == NULL)
	    return 1;

	addfields = 0;
	argc = CountArgs(argv);

	arg = 0;
	if (strcmp(argv[0], "-env") == 0)
	  {
	    WarnAboutEnvVars();
	    addfields = 1;

	    argv[0] = "";
	    arg++;
	    argc--;
	  }

	if (argc%2 == 1) /* set command with element path */
	  {
	    if (strchr(argv[arg], ':') != NULL)
		*pcmd = "setconn";

	    elm = argv[arg];
	    arg++;
	  }
	else
	    elm = ".";

	/*
	** Combine field/value argument pairs.  This will cause the
	** arguments to be paired on output, regardless of line continuation.
	*/

	for (fld = arg; argv[fld] != NULL; fld += 2)
	  {
	    /*
	    ** an environment variable was being set. Cause the field to be
	    ** assed to the element if it doesn't already exist.
	    */

	    if (addfields)
	      {
		char	line[500];

		OutputIndentation();
		sprintf(line, "if (!{exists %s %s})\n", elm, argv[fld]);
		Output(line, NULL);

		    OutputIndentation();
		    sprintf(line, "    addfield %s %s\n", elm, argv[fld]);
		    Output(line, NULL);

		OutputIndentation();
		Output("end\n", NULL);
	      }
	    argv[arg] = Combine(argv[fld], " ", argv[fld+1], NULL);

	    arg++;
	    if (argv[fld+1] == NULL)  /* missing value for the field */
		break;
	  }
	argv[arg] = NULL;

	return 1;
}

int MapShow(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	if (argv[0] == NULL)
	    return 1;

	if (strchr(argv[0], ':') != NULL)
	    *pcmd = "showconn";

	return 1;
}


int MapClean(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	nxtarg;

	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	    if (arg_is("-f"))
		argv[nxtarg] = "-force";

	return 1;
}


int MapHelp(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	if (argv[0] != NULL && argv[1] != NULL)
	    InsertItemInArgList(argv, 1, "-helpdir", 100);

	return 1;
}


int MapDeleteMsg(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	if (argv[0] == NULL || argv[1] == NULL)
	    return -1;

	if (strcmp(argv[1], "IN") == 0)
	    AddItemToArgList(argv, "-incoming", 100);
	else if (strcmp(argv[1], "OUT") == 0)
	    AddItemToArgList(argv, "-outgoing", 100);
	argv[1] = "";

	return 1;
}


int MapGetMsg(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	char*	mode;
	char*	index;
	char*	option;

	if (argv[0] == NULL || argv[1] == NULL || argv[2] == NULL)
	    return -1;

	if (strcmp(argv[1], "IN") == 0)
	    mode = "-incoming";
	else if (strcmp(argv[1], "OUT") == 0)
	    mode = "-outgoing";
	else
	    return -1;

	if (strcmp(argv[2], "-count") == 0)
	  {
	    argv[1] = mode;
	    return 1;
	  }

	index = argv[2];
	option = argv[3];
	if (option == NULL)
	    return -1;

	argv[1] = mode;
	if (strcmp(option, "type") == 0)
	    argv[2] = "-type";
	else if (strcmp(option, "src") == 0)
	    argv[2] = "-source";
	else if (strcmp(option, "dst") == 0)
	    argv[2] = "-destination";

	argv[3] = index;
	argv[4] = NULL;

	return 1;
}


int MapReadFile(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	linemode;
	int	nxtarg;

	linemode = 0;
	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	  {
	    if (strcmp(argv[nxtarg], "-l") == 0)
	      {
		linemode = 1;
		argv[nxtarg] = "";
	      }
	  }

	if (linemode)
	    AddItemToArgList(argv, "-linemode", 100);

	return 1;
}


int MapWriteFile(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	nonewline;
	char*	format;
	int	nxtarg;
	int	outarg;

	nonewline = 0;
	format = NULL;

	outarg = 0;
	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	  {
	    if (arg_is("-n"))
		nonewline = 1;
	    else if (arg_is("-format"))
	      {
		format = argv[nxtarg+1];
		if (format == NULL)
		    format = "";
		else
		    nxtarg++;
	      }
	    else
		argv[outarg++] = argv[nxtarg];
	  }
	
	if (nonewline)
	    argv[outarg++] = "-nonewline";

	if (format != NULL)
	  {
	    argv[outarg++] = "-format";
	    argv[outarg++] = format;
	  }

	argv[outarg] = NULL;

	return 1;
}


int MapAddEscape(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	nxtarg;

	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	    if (arg_is("-exec"))
		argv[nxtarg] = "-execute";

	return 1;
}


int MapGetArg(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	if (argv[0] == NULL)
	    return -1;

	if (strcmp(argv[0], "-count") == 0)
	    AddItemToArgList(argv, "-count", 100);
	else
	  {
	    AddItemToArgList(argv, "-arg", 100);
	    AddItemToArgList(argv, argv[0], 100);
	  }
	argv[0] = "";

	return 1;
}


int MapDebug(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	if (argv[0] != NULL && strcmp(argv[0], "-value") == 0)
	    argv[0] = "";

	return 1;
}


int MapDebugFunc(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	if (argv[0] != NULL && argv[1] != NULL &&
				    strcmp(argv[1], "-value") == 0)
	    argv[1] = "";

	return 1;
}


int MapWriteCell(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	nxtarg;

	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	    if (arg_is("cartesian"))
		argv[nxtarg] = "-cartesian";
	    else if (arg_is("polar"))
		argv[nxtarg] = "-polar";
	    else if (arg_is("relative"))
		argv[nxtarg] = "-relative";
	    else if (arg_is("absolute"))
		argv[nxtarg] = "-absolute";
	    else if (arg_is("syn_dens"))
		argv[nxtarg] = "-syndens";
	    else if (arg_is("syn_cond"))
		argv[nxtarg] = "-syncond";

	return 1;
}


int MapCellSheet(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	nxtarg;
	int	i;

	if (argv[0] == NULL || argv[1] == NULL || argv[2] == NULL)
	    return -1;

	for (nxtarg = 3; argv[nxtarg] != NULL; nxtarg++)
	    if (argv[nxtarg][0] == 'o')
		argv[nxtarg] = "-orient";
	    else if (argv[nxtarg][0] == 'c')
		argv[nxtarg] = "-curve";
	    else if (argv[nxtarg][0] == 'u')
		argv[nxtarg] = "-useprototype";
	    else if (argv[nxtarg][0] == 'p')
		argv[nxtarg] = "-parallel";
	    else if (argv[nxtarg][0] == 't')
	      {
		argv[nxtarg] = "-twirl";
		if (argv[nxtarg+1] != NULL)
		    nxtarg++;
	      }
	    else if (argv[nxtarg][0] == 'r')
	      {
		argv[nxtarg] = "-randomize";
		for (i = 0; i < 3; i++)
		  {
		    if (argv[nxtarg+1] == NULL)
			break;

		    nxtarg++;
		  }
	      }

	return 1;
}


int MapSpikeParms(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	nxtarg;

	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	    if (arg_is("-restV"))
		argv[nxtarg] = "-rest";
	    else if (arg_is("-abs_exp"))
		argv[nxtarg] = "-absexp";
	return 1;
}


int MapEgg(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	nxtarg;

	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	    if (arg_is("-n"))
		argv[nxtarg] = "-normal";

	return 1;
}


int MapPlane(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	nxtarg;

	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	    if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'n')
		argv[nxtarg] = "-normal";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'c')
		argv[nxtarg] = "-center";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'j')
		argv[nxtarg] = "-jitter";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'e')
		argv[nxtarg] = "-ellipse";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'h')
		argv[nxtarg] = "-hexagonal";

	return 1;
}


int MapRandComp(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	nxtarg;
	int	outarg;

	outarg = 0;
	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	    if (arg_is("-p"))
	      {
		AddItemToArgList(argv, "-probability", 100);
		AddItemToArgList(argv, argv[++nxtarg], 100);
	      }
	    else
		argv[outarg++] = argv[nxtarg];
	argv[outarg] = NULL;

	return 1;
}


int MapCurveMatch(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	nxtarg;

	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	    if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'a')
		argv[nxtarg] = "-amplitude";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 't')
		argv[nxtarg] = "-time";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 's')
		argv[nxtarg] = "-shape";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'p')
		argv[nxtarg] = "-peaktopeak";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'w')
		argv[nxtarg] = "-weight";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'P')
		argv[nxtarg] = "-plot";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'v')
		argv[nxtarg] = "-verbose";

	return 1;
}


int MapNewMatch(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	nxtarg;

	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	    if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'W')
		argv[nxtarg] = "-weight";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'R')
		argv[nxtarg] = "-result";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'l')
		argv[nxtarg] = "-low";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'L')
		argv[nxtarg] = "-toolow";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'h')
		argv[nxtarg] = "-high";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'H')
		argv[nxtarg] = "-toohigh";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'p')
		argv[nxtarg] = "-peaktopeak";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'P')
		argv[nxtarg] = "-toosmallpeaktopeak";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 's')
		argv[nxtarg] = "-shape";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 't')
		argv[nxtarg] = "-time";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'w')
		argv[nxtarg] = "-width";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'd')
		argv[nxtarg] = "-display";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'v')
		argv[nxtarg] = "-verbose";

	return 1;
}


int MapNMin(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	nxtarg;

	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	    if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'e')
		argv[nxtarg] = "-exponential";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'i')
		argv[nxtarg] = "-initialize";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 't')
		argv[nxtarg] = "-tolerance";

	return 1;
}


int MapSpliNMin(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	nxtarg;

	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	    if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'e')
		argv[nxtarg] = "-exponential";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'i')
		argv[nxtarg] = "-initialize";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 't')
		argv[nxtarg] = "-tolerance";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'c')
		argv[nxtarg] = "-cspline";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'd')
		argv[nxtarg] = "-dx";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'r')
		argv[nxtarg] = "-ratio";
	    else if (argv[nxtarg][0] == '-' && argv[nxtarg][1] == 'n')
		argv[nxtarg] = "-niterations";

	return 1;
}


int MapSetupAlpha(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	char*	size;
	char*	min;
	char*	max;
	int	nxtarg;

	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	    /* count the args */;

	if (nxtarg == 15)
	  {
	    size = argv[12];
	    min = argv[13];
	    max = argv[14];

	    argv[12] = "-size";
	    argv[13] = size;
	    argv[14] = "-range";
	    argv[15] = min;
	    argv[16] = max;
	    argv[17] = NULL;
	  }

	return 1;
}


int MapSetupGate(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	char*	size;
	char*	min;
	char*	max;
	char*	noalloc;
	int	nxtarg;
	int	i;

	noalloc = NULL;
	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	    /* count the args */;

	if (nxtarg < 10)
	    return -1;

	if (nxtarg == 11 && argv[10][0] == 'n')
	    noalloc = "-noalloc";

	size = argv[2];
	min = argv[3];
	max = argv[4];
	for (i = 0; i < 5; i++)
	    argv[i+2] = argv[i+5];

	argv[7] = "-size";
	argv[8] = size;
	argv[9] = "-range";
	argv[10] = min;
	argv[11] = max;
	argv[12] = noalloc;
	argv[13] = NULL;

	return 1;
}


int MapTweakAlpha(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	argv[2] = NULL; /* removes xdivs xmin xmax args if present */
	return 1;
}


int MapScaleTabchan(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	nxtarg;

	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	    /* count the args */;

	if (nxtarg == 8) {
	    if (argv[7][0] == 'd')
		argv[7] = "-duplicate";
	    else
		argv[7] = NULL;
	}

	return 1;
}


int MapDupTable(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	argv[2] = NULL; /* truncate extraneous args */
	return 1;
}


int MapXAddPts(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	static char	plotpath[1000];

	int	nxtarg;
	int	outarg;
	char	line[1000];
	char*	color;
	char*	plotname;
	char*	graphname;

	plotname = NULL;
	color = NULL;

	outarg = 0;
	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	    if (strcmp(argv[nxtarg], "-plotname") == 0 && argv[nxtarg] != NULL)
		plotname = argv[++nxtarg];
	    else if (strcmp(argv[nxtarg], "-color") == 0 && argv[nxtarg] != NULL)
		color = argv[++nxtarg];
	    else
		argv[outarg++] = argv[nxtarg];
	argv[outarg] = NULL;

	graphname = argv[0];
	if (plotname == NULL)
	    plotname = "default";
	sprintf(plotpath, "%s/%s", graphname, plotname);

	OutputIndentation();
	sprintf(line, "if (!{exists %s})\n", plotpath);
	Output(line, NULL);

	OutputIndentation();
	sprintf(line, "    create xplot %s\n", plotpath);
	Output(line, NULL);

	OutputIndentation();
	Output("end\n\n", NULL);

	if (color != NULL)
	  {
	    OutputIndentation();
	    sprintf(line, "setfield %s fg %s\n", plotpath, color);
	    Output(line, NULL);
	  }

	/* call <plotpath> ADDPTS <x> <y> ... */
	*pcmd = "call";
	argv[0] = plotpath;
	InsertItemInArgList(argv, 1, "ADDPTS", 100);
	return 1;
}


int MapXUpdatePix(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	static char	combo[1000];

	if (argv[0] == NULL || argv[1] == NULL)
	    return 1;

	sprintf(combo, "%s/%s", argv[0], argv[1]);
	argv[0] = combo;
	argv[1] = "PROCESS";
	argv[2] = NULL;

	return 1;
}


int MapSetMethod(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	static char	methodStr[20];
	int	methodNumber;

	if (argv[1] == NULL || sscanf(argv[1], "%d", &methodNumber) != 1)
	  {
	    fprintf(stderr, "Unable to map setmethod command.  Beware, method numbers have changed\n");
	    fprintf(stderr, "and some methods have been removed.\n");
	  }
	else
	  {
	    switch (methodNumber)
	      {

	      /* Adams-Bashforth methods 2 - 5 step renumbered 2 - 5 */
	      case 5: /* AB4 */
	      case 6: /* AB5 */
		methodNumber--;
		break;

	      /* broken methods removed from GENESIS 2.0 */
	      case 1: /* Gear */
	      case 4: /* Trapizoidal */
	      case 7: /* Runge-Kutta */
		methodNumber = -100;
		break;
	      }

	    sprintf(methodStr, "%d", methodNumber);
	    argv[1] = methodStr;
	  }

	return 1;
}


int MapEcho(pcmd, argv)

char**	pcmd;
char*	argv[];

{
	int	nxtarg;
	int	curarg;
	int	nocr;
	char*	format;

	nocr = 0;
	format = NULL;

	curarg = 0;
	for (nxtarg = 0; argv[nxtarg] != NULL; nxtarg++)
	  {
	    if (strcmp(argv[nxtarg], "-n") == 0)
		nocr = 1;
	    else if (strcmp(argv[nxtarg], "-format") == 0)
	      {
		format = argv[++nxtarg];
		if (format == NULL) /* oops, no argument to format option! */
		    break;
	      }
	    else
		argv[curarg++] = argv[nxtarg];
	  }

	if (nocr)
	    argv[curarg++] = "-nonewline";
	if (format != NULL)
	  {
	    argv[curarg++] = "-format";
	    argv[curarg++] = format;
	  }
	argv[curarg] = NULL;

	return 1;
}


MapFunction MapFuncs[] =
  {
    /*
    ** This table must be maintained in alphabetical order on the
    ** old command names.
    */

    /* 							command returns */
    /* old command	new command	map function	char* or char** */
    { "EL",		"el",		NULL,		1 },
    { "Error",		"error",	NULL,		0 },
    { "INPUT",		"input",	NULL,		1 },
    { "VERSION",	"version",	NULL,		0 },
    { "addescape",	NULL,		MapAddEscape,	0 },
    { "addfunc",	NULL,		StartupCmd,	0 },
    { "addschedule",	"addtask",	NULL,		0 },
    { "alias",		"addalias",	NULL,		0 },
    { "arglist",	NULL,		NULL,		1 },
    { "argv",		NULL,		NULL,		1 },
    { "bgstep",		"step",		MapBgStep,	0 },
    { "calc_Cm",	"calcCm",	NULL,		0 },
    { "calc_Rm",	"calcRm",	NULL,		0 },
    { "call",		NULL,		NULL,		1 },
    { "cell_sheet",	"cellsheet",	MapCellSheet,	0 },
    { "chr",		NULL,		NULL,		1 },
    { "clean",		"deleteall",	MapClean,	0 },
    { "clear_buffer",	"clearbuffer",	NULL,		0 },
    { "clearschedule",	"deletetasks",	NULL,		0 },
    { "clocks",		"showclocks",	NULL,		0 },
    { "copy",		NULL,		MapCopy,	0 },
    { "cpu",		"showcpu",	NULL,		0 },
    { "create",		NULL,		MapCreate,	0 },
    { "createmap",	NULL,		MapCreateMap,	0 },
    { "curvematch",	NULL,		MapCurveMatch,	0 },
    { "date",		"getdate",	MapDate,	1 },
    { "dd_3d_msg",	"dd3dmsg",	NULL,		0 },
    { "debug",		NULL,		MapDebug,	0 },
    { "debugfunc",	NULL,		MapDebugFunc,	0 },
    { "deletemsg",	NULL,		MapDeleteMsg,	0 },
    { "duplicate_table","duplicatetable", MapDupTable,	0 },
    { "echo",		NULL,		MapEcho,	0 },
    { "egg",		NULL,		MapEgg,		0 },
    { "element_list",	"el",		NULL,		1 },
    { "float_format",	"floatformat",	NULL,		0 },
    { "gen_3d_msg",	"gen3dmsg",	NULL,		0 },
    { "get",		"getfield",	MapGet,		1 },
    { "getarg",		NULL,		MapGetArg,	1 },
    { "getenv",		NULL,		NULL,		1 },
    { "getfields",	"getfieldnames",NULL,		1 },
    { "getmsg",		NULL,		MapGetMsg,	1 },
/*  { "getobjenv",	"getdefault",	NULL,		1 },*/
    { "help",		NULL,		MapHelp,	0 },
    { "jobs",		"showjobs",	NULL,		0 },
    { "killjob",	"deletejob",	NULL,		0 },
    { "le",		NULL,		MapLe,		0 },
    { "listescape",	"showescapes",	NULL,		0 },
    { "listfiles",	"showfiles",	NULL,		0 },
    { "listglobals",	"showglobals",	NULL,		0 },
    { "move",		NULL,		MapMove,	0 },
    { "newclass",	"addclass",	NULL,		0 },
    { "newmatch",	NULL,		MapNewMatch,	0 },
    { "nmin",		NULL,		MapNMin,	0 },
    { "object",		NULL,		StartupCmd,	0 },
	/* parmin and splinmin options are identical */
    { "parmin",		NULL,		MapSpliNMin,	0 },
    { "paste_channel",	"pastechannel",	NULL,		0 },
    { "pathname",	"getpath",	NULL,		1 },
    { "plane",		NULL,		MapPlane,	0 },
    { "pop",		"pope",		NULL,		0 },
    { "position",	NULL,		MapPosition,	0 },
    { "priority",	"setpriority",	NULL,		0 },
    { "prompt",		"setprompt",	NULL,		0 },
    { "push",		"pushe",	NULL,		0 },
    { "rall_calc_Rm",	"rallcalcRm",	NULL,		0 },
    { "rancoord",	"randcoord",	NULL,		0 },
    { "rand_comp",	"randcomp",	MapRandComp,	1 },
    { "randomfield",	"setrandfield",	MapRandomField,	0 },
    { "randomseed",	"randseed",	MapRandomSeed,	0 },
    { "read_cell",	"readcell",	NULL,		0 },
    { "readfile",	NULL,		MapReadFile,	1 },
    { "rel_position",	"relposition",	NULL,		0 },
    { "reschedule",	"resched",	NULL,		0 },
    { "scale_parms",	"scaleparms",	NULL,		0 },
    { "scale_tabchan",	"scaletabchan",	MapScaleTabchan,0 },
    { "sched",		"showsched",	NULL,		0 },
    { "sendmsg",	"addmsg",	NULL,		0 },
    { "set",		"setfield",	MapSet,		0 },
    { "set_postscript",	"xps",		NULL,		0 },
    { "setmethod",	NULL,		MapSetMethod,	0 },
/*  { "setobjenv",	"setdefault",	NULL,		0 },*/
    { "setpostscript",	"xps",		NULL,		0 },
    { "setup_alpha",	"setupalpha",	MapSetupAlpha,	0 },
    { "setup_gate",	"setupgate",	MapSetupGate,	0 },
	/* options to setupalpha and setuptau are identical */
    { "setup_tau",	"setuptau",	MapSetupAlpha,	0 },
	/* options to newmatch and shapematch are identical */
    { "shapematch",	NULL,		MapNewMatch,	0 },
    { "show",		"showfield",	MapShow,	0 },
    { "showbinding",	"showalias",	NULL,		0 },
    { "spikeparms",	NULL,		MapSpikeParms,	0 },
    { "splinmin",	NULL,		MapSpliNMin,	0 },
    { "status",		"getstat",	MapStatus,	1 },
    { "step",		NULL,		MapStep,	0 },
    { "strcat",		NULL,		NULL,		1 },
    { "substring",	NULL,		NULL,		1 },
    { "tweak_alpha",	"tweakalpha",	MapTweakAlpha,	0 },
	/* options to tweakalpha and tweaktau are identical */
    { "tweak_tau",	"tweaktau",	MapTweakAlpha,	0 },
    { "write_cell",	"writecell",	MapWriteCell,	0 },
    { "writefile",	NULL,		MapWriteFile,	0 },
    { "xaddpts",	NULL,		MapXAddPts,	0 },
    { "xinit",		NULL,		RemoveCmd,	0 },
    { "xkill",		"delete",	NULL,		0 },
    { "xon",		NULL,		RemoveCmd,	0 },
    { "xshow_on_top",	"xshowontop",	NULL,		0 },
    { "xupdatepix",	"call",		MapXUpdatePix,	0 }
  };

#define NMAPFUNCS	(sizeof(MapFuncs)/sizeof(MapFunction))

int mapcmp(mf1, mf2)

MapFunction*	mf1;
MapFunction*	mf2;

{
	return strcmp(mf1->cmd, mf2->cmd);
}


/*
** MapCommand
**
**	Maps a given command to its 2.0 equivalent.  Returns 1 on success
**	with argv changed to the new command, 0 if it has outputed the
**	replacement code manually and doesn't want the caller to do anything
**	more and -1 if there was an error in mapping the command in which
**	case cmdname and argv should be left as it was on entering MapCommand.
*/

int MapCommand(cmdname, argv, retstr)

char**	cmdname;
char*	argv[];
int*	retstr;

{	/* MapCommand --- Map a command from 1.4.1 to 2.0 usage */

	extern char*	bsearch();
	MapFunction	key;
	MapFunction*	mf;

	if (retstr != NULL)
	    *retstr = 0;

	key.cmd = *cmdname;
	mf = (MapFunction*) bsearch(&key, MapFuncs, NMAPFUNCS,
						sizeof(MapFunction), mapcmp);
	if (mf != NULL)
	  {
	    if (retstr != NULL)
		*retstr = mf->retstr;
	    if (mf->newcmd != NULL)
		*cmdname = mf->newcmd;

	    if (mf->mapfunc != NULL)
		return mf->mapfunc(cmdname, argv);
	  }

	return 1;

}	/* MapCommand */
