/* $Id: xo_cvt.c,v 1.4 2005/07/07 19:21:02 svitak Exp $ */
/*
 * $Log: xo_cvt.c,v $
 * Revision 1.4  2005/07/07 19:21:02  svitak
 * Fixed a bug which may only appear on certain architectures where data on
 * the call stack was getting corrupted. This occurred in calls to XtSetValue
 * where the third (value) argument was a function call which modified one of
 * it's arguments and returned it to the caller. The function call was moved
 * in front of the XtSetValue call.
 *
 * Revision 1.3  2005/07/01 10:03:00  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.2  2005/06/18 18:50:29  svitak
 * Typecasts to quiet compiler warnings. From OSX Panther patch.
 *
 * Revision 1.1  2005/06/17 21:23:52  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.24  2001/06/29 21:12:55  mhucka
 * Added extra parens inside conditionals and { } groupings to quiet certain
 * compiler warnings.
 *
 * Revision 1.23  2000/06/12 04:11:25  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added missing type casts.
 *
 * Revision 1.22  2000/05/02 06:20:02  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.21  1999/10/16 22:50:22  mhucka
 * Merged in changes from Venkat for supporting "xsimplot".
 *
 * Revision 1.17.1.5  1997/07/16  00:21:02  venkat
 * Fixed bug in xoSpecialSetArgs() that was not setting fields for
 * embedded forms.
 *
 * Revision 1.17.1.4  1997/06/28 00:10:52  venkat
 * Changed xoSpecialSetArgs() to deal with embedded and top-level forms.
 *
 * Revision 1.17.1.3  1996/07/23  22:52:50  venkat
 * xg_convert now checks if the number of arguments is atleast 1; returns
 * otherwise
 *
 * Revision 1.17.1.2  1996/07/02  18:42:39  venkat
 * Defintion of xomemzero. This interface zeros out the passed number of
 * bytes starting with the passed address. This is a pointer to a char and
 * so can be used as a general purpose zeroing/intialization routine.
 *
 * Revision 1.17.1.1  1995/09/26  23:01:10  venkat
 * Changed all XtVaSetValues() calls to XoXtVaSetValues() to solve
 * the alpha FPE's.
 *
 * Revision 1.17  1995/08/05  02:31:03  venkat
 * gx_convert_all() does not do gx_convert() if the field value happens to
 * be the string "nil"
 *
 * Revision 1.16  1995/07/21  20:13:57  venkat
 * Commented call to flush the XEvent buffer in xoGetGeom(). This was calling
 * overlapping widget callbacks and strange behaviours
 *
 * Revision 1.15  1995/05/17  23:37:16  venkat
 * The geom args are initialized in xoSpecialSetArg() not by callocing and freeing
 * an XoGeometryPtr by simply by setting fields of a static XoGeometryRec structure.
 * thereby freeing is not an issue
 *
 * Revision 1.14  1995/05/02  21:58:51  venkat
 * Modified xoSpecialSetArg() to free the initialized XoGeometryPtr structure
 * after processing the appropriate geom arg.
 *
 * Revision 1.13  1995/04/04  01:03:53  venkat
 * Made changes to revert to pre-2.0.9 form geom
 * spec behaviour. The form's width and height are 16
 * pixels shorter than the ones specified.
 *
 * Revision 1.12  1995/03/06  20:17:04  venkat
 * Changed xoSpecialSetArg() to process options for
 * fg and bg in terms of the common resources specified
 * in the base classes XoCore and XoComposite.
 *
 * Revision 1.11  1995/02/10  23:48:25  venkat
 * Merged in 1.9.1.1 and 1.10 changes
 *
<<<<<<< 1.10
 * Revision 1.10  1995/02/08  00:49:10  dhb
 * Merged in 1.8.1.1 changes.
 *
<<<<<<< 1.9
=======
 * Revision 1.9.1.1  1995/02/10  23:26:55  venkat
 * Added local functions GeomSyntax() and ParseGeometryString() to
 * deal with create command-line geom syntax. Also changed gx_convert()
 * to guard against internal geometry string buffer overflow.
 *
>>>>>>> 1.9.1.1
 * Revision 1.9  1995/02/07  01:44:20  venkat
 * Changed xoSpecialSetArgs() to handle create line -bg
 * and -fg (not needed) for a form widget.
 *
=======
 * Revision 1.8.1.1  1995/02/08  00:39:12  dhb
 * Changes to set wgeom and hgeom from form rather than
 * shell widget values.
 *
>>>>>>> 1.8.1.1
 * Revision 1.8  1995/01/31  18:45:24  venkat
 * Changed gx_convert to parse geometry args with spaces and return
 * the count of spaces. Also modified xoParseCreateArgs to parse the
 * cmd-line args one at a time instead of a field-value pair.
 * xoSpecialSetArgs() modified to initialize XoGeometryPtr structure
 * after calloc to default values for the appropriate field xgeom, ygeom
 * wgeom and hgeom.
 *
 * Revision 1.7  1994/04/25  18:04:10  bhalla
 * Implemented Dave Bilitch's fix for the assignment of xgeom ygeom etc
 * to targv
 *
 * Revision 1.6  1994/03/16  16:51:07  bhalla
 * Fixed problem of geometry conversion barfing if there were spaces
 * in the [x,y,w,h] form of geom specification. Turned out that the
 * parser was treating them as separate args in that case.
 * Also fixed problem of retrieving geom values from widgets if the
 * geom had not been explicitly set. Did this by retrieving pixel coords
 * for xywh as a fallback.
 *
 * Revision 1.5  1994/02/08  22:24:44  bhalla
 * Fixed xoGetGeom
 *
 * Revision 1.4  1994/02/08  17:34:58  bhalla
 * Added function xoGetGeom used in updating geometry fields on
 * widget creation.
 *
 * Revision 1.3  1994/02/02  20:26:05  bhalla
 * Eliminated angle bracket conversion for <widget> in script calls.
 * This is now handled along with many other options in xo_callb.c
 * */

#include <string.h>
#include <ctype.h>
#include "sim_ext.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Xo/Geometry.h"
#include "Xo/Xo_ext.h"
#include "Widg/Framed.h"
#include "Widg/Form.h"
#include "Widg/XoCore.h"
#include "xo_ext.h"
#include "_widg/widg_struct.h"

#define streq(a,b) (strcmp( (a), (b) ) == 0)
#define strbegins(a,b) (strncmp ((a), (b), strlen (a)) == 0)

/* These two functions put fields from widgets to
** elements and vice versa.
*/

static int xoConvertGeom(value,min,max,deflt)
	char	*value;
	int		min;
	int		max;
	int		deflt;
{
	int ret;
	char	*sc;

	if (value == NULL) return(deflt);
	if ((int)strlen(value) < 1) return(deflt);
	for(sc=value;*sc;sc++){
	 if(isalpha(*sc)){
          printf("Cannot convert string '%s' to geometry specification\n",value);
	 
	 printf("Using default for the field \n");
	  return(deflt);
	 }	
  	}
	sc = strchr(value,':');
	if (sc) {
		/* only references to other forms are allowed */
		*sc = '\0';
		printf("References not allowed in form geometry spec\n");
		printf("Using default for the field \n");
		return (deflt);
	}
	if ((sc=strchr(value,','))) {
	 printf("Cannot convert string %s to geometry specification\n",value);
         *sc = '\0';
	 printf("Using default for the field \n");
	 return(deflt);
	} 
	if (value[strlen(value) -1] == '%')
		ret = (int)((atoi(value)*max)/100);
	else
		ret = atoi(value);
	if (ret < min)
	    return(min);
	if (ret > max)
	    return(max);

	return(ret);
}

/** Local functions Geomsyntax(), parseTo(), and ParseGeomrtyString()
  to deal with create command-line
 geometry specification errors and parsing **/

static void Geomsyntax(str)
char *str;
{
 printf("Missing '%s' in geometry specification \n", str);
 printf("Using default geometry \n");
}

/*
** input string is modififed!!
*/
static Boolean parseTo( delim, mungedStr, returnStr, defaultStr )
char delim;
char ** mungedStr;
char ** returnStr;
char * defaultStr;
{
	int i;
	char *strsave();

	/* find delim */
	for(i=0; (*mungedStr)[i] && (*mungedStr)[i] != delim;i++);

	if((*mungedStr)[i]=='\0'||(*mungedStr)[i]!=delim){
		char s[2];
		s[0] = delim; s[1] = '\0';
 		Geomsyntax(s);
 		return False;
	}

	(*mungedStr)[i]='\0';
	/* If the string is empty, use default */
	if(strlen((*mungedStr)) == 0) {
		*returnStr = strsave(defaultStr);
	} else {
		*returnStr = strsave(*mungedStr);
	}

	(*mungedStr) += i + 1;

	return True;
}


/**
  Returns True to reflect that the geometry specification has
  bad syntax
**/ 
static Boolean ParseGeometryString(geom,xpart,ypart,wpart,hpart)
char *geom;
char **xpart;
char **ypart;
char **wpart;
char **hpart; 
{
	char *strsave(), *tempstrptr, *tempstr;

	/* nothing in string? -> bad */
	if (!geom) return(True);

	/* Get rid of leading spaces on value */
	for(;*geom && (*geom == ' ' || *geom == '	'); geom++);

	if (strlen(geom) < 1) return(True);

	/* skip opening '[' */
	tempstrptr = tempstr = strsave(geom+1);

	if (parseTo(',', &tempstr, xpart, "") &&
		parseTo(',', &tempstr, ypart, "") &&
		parseTo(',', &tempstr, wpart, "") &&
		parseTo(']', &tempstr, hpart, "")) {
			free(tempstrptr);
			return False;
	}

	free(tempstrptr);
	return True;
}

/* Note that the widget being passed in here is actually
** the child widget of the Framed widget that is actually
** handled by the constraint. So many of these args need
** to be handled with this in mind.
*/
static Boolean xoSpecialSetArg (object, field, value)
     Widget object;
     char *field;
     char *value;
{
  XoGeometryRec geometryRec;
  Widget	fW = XtParent(object);
  Widget 	parentW = XtParent(fW);
  Arg		args[20];
  int n = 0;

	/* check if the old form for geometry is being used */
 if(strbegins (field, "oldgeom")){ 
   char *xval = NULL,*yval = NULL,*wval = NULL,*hval = NULL;

   /** Process individual geometry args after making sure of 
	** syntax. Bad syntax (True return value) makes default geometry spec for
	** the Xodus element
	**/
	if( !ParseGeometryString(value,&xval,&yval,&wval,&hval)) {
	    	xoSpecialSetArg(object,"xgeom",xval);
	    	xoSpecialSetArg(object,"ygeom",yval);
	    	xoSpecialSetArg(object,"wgeom",wval);
	    	xoSpecialSetArg(object,"hgeom",hval);
	}
	if (xval) free(xval);
	if (yval) free(yval);
	if (wval) free(wval);
	if (hval) free(hval);
  }

	if (XtIsSubclass(object,formWidgetClass) && XtIsSubclass(parentW, frameWidgetClass)) {
		/* We handle form widgets specially */
		fW = XtParent(XtParent(XtParent(XtParent(object))));
  		if ( strbegins (field, "xgeom") ) {
			n=xoConvertGeom(value,0,XDisplayWidth((Display *)XgDisplay(),0),1);
			/* ugly hack to head off a problem I havent figured out */
			/*if (n == 0) n = 1;*/
			if (n>=0) {
				XoXtVaSetValues(fW,XtNx,n,NULL);
				return(True);
			}
			n = 0;
		}
  		if ( strbegins (field, "ygeom") ) {
			n=xoConvertGeom(value,0,XDisplayHeight((Display *)XgDisplay(),0),1);
			/* ugly hack to head off a problem I havent figured out */
			/*if (n == 0) n = 1;*/
			if (n>=0) {
				XoXtVaSetValues(fW,XtNy,n,NULL);
				return(True);
			}
			n = 0;
		}
  		if ( strbegins (field, "wgeom") ) {
			n=xoConvertGeom(value,1,XDisplayWidth((Display *)XgDisplay(),0),200);
			if (n>1) {
				XoXtVaSetValues(/*object*/fW,XtNwidth,n,NULL);
				return(True);
			}
			n = 0;
		}
  		if ( strbegins (field, "hgeom") ) {
			n=xoConvertGeom(value,1,XDisplayHeight((Display *)XgDisplay(),0),200);
			if (n>1) {
				XoXtVaSetValues(/*object*/fW,XtNheight,n,NULL);
				return(True);
			}
			n = 0;
		}
  		if ( strbegins (field, "bg") ) {
			XoXtVaSetValues(object, XtVaTypedArg,
				XtNbg,
				XtRString,value,strlen(value)+1,
				NULL);
			return(True);
	  	}

		return(False);
	}
/* printf("xoGeometrySetArg :3\n");  */
  if ( strbegins (field, "xgeom") ) {
/* printf("xoGeometrySetArg :5\n");  */
  /** Initialize geometryRec to layout defaults for the field **/
    geometryRec.request=0;
    geometryRec.requestType=XoAbsoluteRequest;
    geometryRec.reference = XoReferToParent;
    geometryRec.referenceEdge = XoReferToDefault;
	XoGeometryFromString(fW, value, &geometryRec);
	XtSetArg(args[n],XtNxGeometry, &geometryRec); n++;
  }
/* printf("xoGeometrySetArg :11\n"); */
  if ( strbegins (field, "ygeom") ) {
/* printf("xoGeometrySetArg :13\n");  */
  /** Initialize geometryRec to layout defaults for the field **/
    geometryRec.request=0;
    geometryRec.requestType=XoAbsoluteRequest;
    geometryRec.reference = XoReferToLastChild;
    geometryRec.referenceEdge = XoReferToDefault;
	XoGeometryFromString(fW, value, &geometryRec);
	XtSetArg(args[n],XtNyGeometry, &geometryRec); n++;
  }
/* printf("xoGeometrySetArg :19\n"); */
  if ( strbegins (field, "wgeom") ) {
/* printf("xoGeometrySetArg :21\n"); */
  /** Initialize geometryRec to layout defaults for the field **/
    geometryRec.request=100;
    geometryRec.requestType=XoRelativeRequest;
    geometryRec.reference = XoReferToParent;
    geometryRec.referenceEdge = XoReferToDefault;
	XoGeometryFromString(fW, value, &geometryRec);
	XtSetArg(args[n],XtNwGeometry, &geometryRec); n++;
  }
/* printf("xoGeometrySetArg :27\n"); */
  if ( strbegins (field, "hgeom") ) {
/* printf("xoGeometrySetArg :29\n"); */
  /** Initialize geometryRec to layout defaults for the field **/
    geometryRec.request=XoNaturalGeometry;
    geometryRec.requestType=XoAbsoluteRequest;
    geometryRec.reference = XoReferToParent;
    geometryRec.referenceEdge = XoReferToDefault;
	XoGeometryFromString(fW, value, &geometryRec);
	XtSetArg(args[n],XtNhGeometry, &geometryRec); n++;
  }

/* printf("xoGeometrySetArg :36\n"); */
  if ( strbegins (field, "border_width") ) {
/* printf("xoGeometrySetArg :38\n"); */
	XtSetArg(args[n],XtNborderWidth,atoi(value)); n++;
  }
  if ( strbegins (field, "border_color") ) {
	XtSetArg(args[n],XtNborderColor,XoPixelFromString(fW,value));n++;
  }
  if (n>0) {
  	XtSetValues (fW, args, n);
	return(True);
  }
  
  if ( strbegins (field, "fg") ) {
	XoXtVaSetValues(object, XtVaTypedArg,
			XtNfg,
			XtRString,value,strlen(value)+1,
			NULL);
	return(True);
  }
  if ( strbegins (field, "bg") ) {
	XoXtVaSetValues(object, XtVaTypedArg,
			XtNbg,
			XtRString,value,strlen(value)+1,
			NULL);
	return(True);
  }
  return False;
}

/* Put genesis element fields into xodus widgets  */
int gx_convert(elm, g2x,ng2x, ac,av)
	struct xwidg_type	*elm;
	Gen2Xo	*g2x;	/* table with genesis and xodus field names */
	int		ng2x;	/* size of g2x list */
	int		ac;		/* Number of arguments to convert */
	char	**av;	/* array of argument names and values */
{
	/* the args are used to set the special values that need
	** xodus converters */
	char	*cur_arg;
	int i, ret=0;
	int is_gadget = 0;

	/*
	printf("Converting fields for elm %s, ng2x=%d\n",Pathname(elm),
		ng2x);
	for(i=0;i<ac;i++) printf("%s\n",av[i]);
		*/
	if (CheckClass(elm,ClassID("gadget")) == 1) /* is a gadget */
		is_gadget = 1;
		
	/* Scan through all fields to be set. Though we are scanning
	** upwards in av, we are counting down in ac.*/
    for (; ac > 0; ac-=2, av+=2) {
	  cur_arg = (*av[0] == '-') ? av[0]+1 : av[0];
	  if (cur_arg[0] == '[') {
		char *geomstr=NULL;
		Boolean LastStringAdded = False;
		Boolean firstalloc=True;

	  /* tell the special set stuff to expect an old-form geometry */
	  	cur_arg = "oldgeom";

	  /* Scan through the args in case the geometry was split into 
	  ** several args by spaces, till we get to a closing bracket 
	  ** If there is no closing bracket check if the following arg is
	  ** an option or a negative geom spec and squish it into the
	  ** the geometry if necessary. The return value reflects the
	  ** the number of spaces in the geometry spec */

		for(; av[0][strlen(av[0]) -1] != ']'; av++){
			if((ac > 1) && (*av[1]== '-')
			      && (strlen(av[1])>1)
			      && (isalpha(*(av[1]+1)))){
			 LastStringAdded=False; break;
			} else{
			 if(firstalloc){
			  if((geomstr = (char*)calloc(strlen(av[0])+1,
							 sizeof(char)))==NULL){
			   printf("Insufficient memory for geometry string \n");
			   printf("Using default geometry \n");  
			  }
			  firstalloc = False;
			 }else{
			  if(geomstr!=NULL)
			   geomstr=(char*)realloc((void*)geomstr,(size_t)
							(strlen(geomstr)+1+
							strlen(av[0])+1));
			   if(geomstr==NULL){
			   printf("Insufficient memory for geometry string \n");
			   printf("Using default geometry \n");  
			   }	
			 }
			if(geomstr!=NULL)
			 strcat(geomstr,av[0]);
			 ac--;
			 ret++;
			}
			if(ac==0){
			 LastStringAdded=True; break;
			}
		}
		/* put in the last string too */
		if(!LastStringAdded){
		 if(geomstr==NULL){ 
			  if((geomstr = (char*)calloc(strlen(av[0])+1,
							 sizeof(char)))==NULL){
			   printf("Insufficient memory for geometry string \n");
			   printf("Using default geometry \n");
			  }
		 }
		 else{
			   geomstr=(char*)realloc((void*)geomstr,(size_t)
							(strlen(geomstr)+1+
							strlen(av[0])+1));
			    if(geomstr==NULL){
			   printf("Insufficient memory for geometry string \n");
			   printf("Using default geometry \n");
			   }	
		
		 }
		 
		if(geomstr!=NULL)
		 strcat(geomstr, av[0]);
	       }
    	if (!is_gadget && xoSpecialSetArg(elm->widget,cur_arg,geomstr))
		printf("Failed to convert geom field %s\n", geomstr);
		av++;
		cur_arg = av[0]+1;	
		ac--;
		ret++;
		if(geomstr!=NULL) free(geomstr);	
	  }
	  if (ac < 2) return (ret); /* At this point we need to have at least
							** 2 args 
				    **Otherwise return the number of 
				  ** spaces encountered 									*/
	/* This routine sets the args if they are special */
    	if (!is_gadget && xoSpecialSetArg(elm->widget,cur_arg, av[1])) {
		/* This is handled by the routine */
		} else {
		/* Check through the list of registered field names */
	  		for(i=0;i<ng2x;i++) {
				if (strcmp(g2x[i].g,cur_arg)==0)  {
				/* just set the values as is */
					XoXtVaSetValues(elm->widget, XtVaTypedArg,
						g2x[i].x,
						XtRString,av[1],strlen(av[1])+1,
						NULL);
					/*
					printf("normal args : %s,%s,%s,%d\n",
					Pathname(elm),av[1],g2x[i].x,
						strlen(av[1])+1); */
				}
			}
		}
	}
       return (ret);
}

/* This function converts all values on the g2x list into widget
** form. Useful for copies */
void gx_convert_all(elm, g2x,ng2x)
	struct xwidg_type	*elm;
	Gen2Xo	*g2x;	/* table with genesis and xodus field names */
	int		ng2x;	/* size of g2x list */
{
	char	*av[2];
	char	*FieldValue();
	int i;

	for (i = 0; i < ng2x;i++) {
		/* Get ascii value for field */
		av[0] = g2x[i].g;
		av[1] = FieldValue(elm,elm->object,av[0]);
		if (av[1] && (int)strlen(av[1]) > 0 && strcmp(av[1],"(null)") != 0 &&
			strcmp(av[1],"nil") != 0 && 
			strcmp(av[1],"???") != 0)
			gx_convert(elm, g2x,ng2x, 2,av);
	}
}


/* Again, the geometry resources are not being successfully
** retrieved. I wont puzzle over it for now, since this version
** seems mostly harmless */

/*
** Get x and y from gw and width and height from w.  If w is NULL
** use gw.

   Mon Apr  3 11:40:43 PDT 1995 venkat
  
   Get all geom fields from the shell
*/
static void xoGetPixelGeom(elm,gw,w,field)
	Element *elm;
	Widget gw;
	Widget w;
	char *field;
{
	static char	retvalue[100];
	Position pos;
	Dimension dim;

	if (w == NULL)
	    w = gw;

		if (strstr(field,"xgeom") != NULL) { /* have to convert xgeom */
			XtVaGetValues(gw, XtNx,&pos, NULL);
			sprintf(retvalue,"%d",pos);
			DirectSetElement(elm,"xgeom",retvalue);
		}
		if (strstr(field,"ygeom") != NULL) { /* have to convert xgeom */
			XtVaGetValues(gw, XtNy,&pos, NULL);
			sprintf(retvalue,"%d",pos);
			DirectSetElement(elm,"ygeom",retvalue);
		}
		if (strstr(field,"wgeom") != NULL) { /* have to convert xgeom */
			XtVaGetValues(gw, XtNwidth,&dim, NULL);
			sprintf(retvalue,"%d",dim);
			DirectSetElement(elm,"wgeom",retvalue);
		}
		if (strstr(field,"hgeom") != NULL) { /* have to convert xgeom */
			XtVaGetValues(gw, XtNheight,&dim, NULL);
			sprintf(retvalue,"%d",dim);
			DirectSetElement(elm,"hgeom",retvalue);
		}
}


/* Put xodus widget changes into genesis elements  */
void xg_convert(elm, g2x, ng2x,ac,av)
	struct xwidg_type	*elm;
	Gen2Xo	*g2x;	/* table with genesis and xodus field names */
	int		ng2x;	/* size of g2x list */
	int		ac;		/* Number of arguments to convert */
	char	**av;	/* array of argument names and values */
{
	static char	retvalue[100];
	char	*XoStringFromGeometry();
	Boolean xoIsNestedForm(), xoIsNestedOptionSet();
	int i;
	Position pos;
	Dimension dim;
	XoGeometryPtr geometryPtr;
	Widget w = (Widget)elm->widget; /* actual widget */
	Widget gw; /* shell (geom) widget */

	/** Handle the error condition of no args **/
	if(ac < 1) return;

	if (XtIsSubclass(w,formWidgetClass) && !(xoIsNestedForm(elm) && xoIsNestedOptionSet(elm))) {

		gw = XtParent(XtParent(XtParent(XtParent(w))));
		xoGetPixelGeom((Element *) elm,gw,w,av[0]);
#if 0
		if (strstr(av[0],"xgeom") != NULL) { /* have to convert xgeom */
			XtVaGetValues(gw, XtNx,&pos, NULL);
			sprintf(retvalue,"%d",pos);
			DirectSetElement(elm,"xgeom",retvalue);
		}
		if (strstr(av[0],"ygeom") != NULL) { /* have to convert xgeom */
			XtVaGetValues(gw, XtNy,&pos, NULL);
			sprintf(retvalue,"%d",pos);
			DirectSetElement(elm,"ygeom",retvalue);
		}
		if (strstr(av[0],"wgeom") != NULL) { /* have to convert xgeom */
			XtVaGetValues(gw, XtNwidth,&dim, NULL);
			sprintf(retvalue,"%d",dim);
			DirectSetElement(elm,"wgeom",retvalue);
		}
		if (strstr(av[0],"hgeom") != NULL) { /* have to convert xgeom */
			XtVaGetValues(gw, XtNheight,&dim, NULL);
			sprintf(retvalue,"%d",dim);
			DirectSetElement(elm,"hgeom",retvalue);
		}
#endif
	} else {
		char *xg;
		char *yg;
		char *wg;
		char *hg;

		gw = XtParent(w);
		if (XtIsManaged(gw)) {
		geometryPtr = (XoGeometryPtr) calloc (1,sizeof (XoGeometryRec));

		if (strstr(av[0],"xgeom") != NULL) { /* have to convert xgeom */
			XtVaGetValues(gw, XtNxGeometry,geometryPtr,
				NULL);
			xg = XoStringFromGeometry(geometryPtr);
			if (xg == NULL) {
				XtVaGetValues(gw, XtNx,&pos, NULL);
				sprintf(retvalue,"%d",pos);
				DirectSetElement(elm,"xgeom",retvalue);
			} else {
				DirectSetElement(elm,"xgeom",xg);
			}
		}
		if (strstr(av[0],"ygeom") != NULL) { /* have to convert ygeom */
			XtVaGetValues(gw, XtNyGeometry,geometryPtr,
				NULL);
			yg = XoStringFromGeometry(geometryPtr);
			if (yg == NULL) {
				XtVaGetValues(gw, XtNy,&pos, NULL);
				sprintf(retvalue,"%d",pos);
				DirectSetElement(elm,"ygeom",retvalue);
			} else {
				DirectSetElement(elm,"ygeom",yg);
			}
		}
		if (strstr(av[0],"wgeom") != NULL) { /* have to convert wgeom */
			XtVaGetValues(gw, XtNwGeometry,geometryPtr,
				NULL);
			wg = XoStringFromGeometry(geometryPtr);
			if (wg == NULL) {
				XtVaGetValues(gw, XtNwidth,&dim, NULL);
				sprintf(retvalue,"%d",dim);
				DirectSetElement(elm,"wgeom",retvalue);
			} else {
				DirectSetElement(elm,"wgeom",wg);
			}
		}
		if (strstr(av[0],"hgeom") != NULL) { /* have to convert hgeom */
			XtVaGetValues(gw, XtNhGeometry,geometryPtr,
				NULL);
			hg = XoStringFromGeometry(geometryPtr);
			if (hg == NULL) {
				XtVaGetValues(gw, XtNheight,&dim, NULL);
				sprintf(retvalue,"%d",dim);
				DirectSetElement(elm,"hgeom",retvalue);
			} else {
				DirectSetElement(elm,"hgeom",hg);
			}
		}
		free(geometryPtr);
		}
	}
	/* At this point I do not expect to need to convert any special
	** fields like fg back into genesis, so we will just use
	** the conversion table fields */
	for(i=0;i<ng2x;i++) {
		/* Check if the fieldname is a repeated one: only the first
		** exists in Genesis */
		if (i>0 && strcmp(g2x[i].x,g2x[i-1].x) == 0) continue;

		/* Check if we are using the return value to specify changes */
		if (ac > 0 && (int)strlen(av[0]) > 0 &&
			strstr(av[0],g2x[i].x) == NULL)
			/* skip this assignment, as it has not changed */
			continue; 

		/* Do the assignment */
		XtVaGetValues((Widget) elm->widget, XtVaTypedArg,g2x[i].x,
			XtRString,retvalue,99,NULL);
		DirectSetElement(elm,g2x[i].g,retvalue);
	}
}

#if 0
/* This version ought to work but for some reason the resource
** converter cannot find the geometry resources
*/
/* Put xodus widget changes into genesis elements  */
xg_convert(elm, g2x, ng2x,ac,av)
	struct xwidg_type	*elm;
	Gen2Xo	*g2x;	/* table with genesis and xodus field names */
	int		ng2x;	/* size of g2x list */
	int		ac;		/* Number of arguments to convert */
	char	**av;	/* array of argument names and values */
{
	static char	retvalue[100];
	char	*field,*xfield;
	int i;

	if (strstr(av[0],"xgeom") != NULL) { /* have to convert xgeom */
		XtVaGetValues(elm->widget, XtVaTypedArg,XtNxGeometry,
			XtRString,retvalue,99,NULL);
		DirectSetElement(elm,"xgeom",retvalue);
	}
	if (strstr(av[0],"ygeom") != NULL) { /* have to convert ygeom */
		XtVaGetValues(elm->widget, XtVaTypedArg,XtNyGeometry,
			XtRString,retvalue,99,NULL);
		DirectSetElement(elm,"ygeom",retvalue);
	}
	if (strstr(av[0],"wgeom") != NULL) { /* have to convert wgeom */
		XtVaGetValues(elm->widget, XtVaTypedArg,XtNwGeometry,
			XtRString,retvalue,99,NULL);
		DirectSetElement(elm,"wgeom",retvalue);
	}
	if (strstr(av[0],"hgeom") != NULL) { /* have to convert hgeom */
		XtVaGetValues(elm->widget, XtVaTypedArg,XtNhGeometry,
			XtRString,retvalue,99,NULL);
		DirectSetElement(elm,"hgeom",retvalue);
	}
	/* At this point I do not expect to need to convert any special
	** fields like fg back into genesis, so we will just use
	** the conversion table fields */
	for(i=0;i<ng2x;i++) {
		/* Check if the fieldname is a repeated one: only the first
		** exists in Genesis */
		if (i>0 && strcmp(g2x[i].x,g2x[i-1].x) == 0) continue;

		/* Check if we are using the return value to specify changes */
		if (ac > 0 && strlen(av[0]) > 0 &&
			strstr(av[0],g2x[i].x) == NULL)
			/* skip this assignment, as it has not changed */
			continue; 

		/* Do the assignment */
		XtVaGetValues(elm->widget, XtVaTypedArg,g2x[i].x,
			XtRString,retvalue,99,NULL);
		DirectSetElement(elm,g2x[i].g,retvalue);
	}
}
#endif


#if 0

	/* check if the old form for geometry is being used */
    for (tac = ac, tav = av; tac > 0; tac-=1, tav+=1) {
	  if (tav[0][0] = '[') {
		strcpy(tempstr,tav[0]+1);
		/* find the comma */
		for(i=0;tempstr[i] && tempstr[i] != ',';i++);
		/* set the comma to a stop */
		tempstr[i]='\0';
		xoSpecialSetArg(elm->widget,"xgeom",tempstr);
		/* repeat for ygeom */
		tempstr += i + 1;
		for(i=0;tempstr[i] && tempstr[i] != ',';i++);
		tempstr[i]='\0';
		xoSpecialSetArg(elm->widget,"ygeom",tempstr);
		/* repeat for wgeom */
		tempstr += i + 1;
		for(i=0;tempstr[i] && tempstr[i] != ',';i++);
		tempstr[i]='\0';
		xoSpecialSetArg(elm->widget,"wgeom",tempstr);
		/* repeat for wgeom */
		tempstr += i + 1;
		/* This time it terminates with a closing bracket */
		for(i=0;tempstr[i] && tempstr[i] != ']';i++);
		tempstr[i]='\0';
		xoSpecialSetArg(elm->widget,"wgeom",tempstr);
	  }
	}
#endif

/*
** Utility func for handling args that we may pass into the create
** command, doing the proper SET actions and all
*/
void xoParseCreateArgs(elm,ac,av)
	Element *elm;
	int	ac;
	char	**av;
{
	int i,argc=ac;
	int dargc=1;
	char	*field;
	char	*value = NULL;

	for (i=0;i<ac;i++) {
		value = NULL;
		if (av[i][0] == '-') {
			field = av[i]+1;
			argc--;
			if (strcmp(field,"widgets") != 0 && argc > 0)
			    if (av[i+1][0] != '-' || isdigit(av[i+1][1]))
			      {
				i++;
				argc--;
				value = av[i];
				SetElement(elm,field,value);
			      }

			continue;
		} else if (av[i][0] == '[') {
			/* Another stupid special case, this time for create-line
			** geometry specification 
   gx_convert returns the number of spaces encountered when parsing
   the geometry specification. If there are no spaces, the retrun value
   is 1*/
			 dargc = gx_convert((struct xwidg_type *) elm, NULL, 0, argc, av+i);
			 i+=(dargc-1);
			 argc-=dargc;
			continue;
		}
	}
}

/* This function gets the initial geometry once the widget
** is created */
void xoGetGeom(elm)
	Element	*elm;
{
	char *targv[1];

	targv[0] = "xgeom ygeom wgeom hgeom";

	/* Start out by flushing X so that the geometry is settled */
 	/* XgEventLoop();  */

	/* call the conversion routines with the geometry args */
	xg_convert((struct xwidg_type *) elm, NULL, 0, 1,targv);
}

void xomemzero(memloc, nbytes)
 char *memloc;
 int  nbytes;
{
 register int i;

 for (i=0; i<nbytes; i++) memloc[i] = '\0';
}

#undef streq
#undef strbegins
