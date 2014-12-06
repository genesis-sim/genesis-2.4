/* $Id: xform.c,v 1.4 2006/02/03 21:21:59 svitak Exp $ */
/*
 * $Log: xform.c,v $
 * Revision 1.4  2006/02/03 21:21:59  svitak
 * Fix for newer window managers which won't allow windows to be raised
 * to the top of the stack under the guise of "focus stealing prevention".
 *
 * Revision 1.3  2005/07/01 10:02:59  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.2  2005/06/18 18:50:29  svitak
 * Typecasts to quiet compiler warnings. From OSX Panther patch.
 *
 * Revision 1.1  2005/06/17 21:23:51  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.31  2001/04/18 22:39:35  mhucka
 * Miscellaneous portability fixes, mainly for SGI IRIX.
 *
 * Revision 1.30  2000/10/10 16:23:26  mhucka
 * Removed needless definition of "XtNhappy".
 *
 * Revision 1.29  2000/06/12 04:21:52  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.28  2000/05/19 18:35:59  mhucka
 * Added a missing type cast to quiet the compiler.
 *
 * Revision 1.27  2000/05/02 06:18:44  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.26  1999/10/16 22:50:22  mhucka
 * Merged in changes from Venkat for supporting "xsimplot".
 *
 * Revision 1.22.1.4  1998/01/16 22:33:16  venkat
 * Added check for form->widget member being not NULL before
 * passing it off to XtParent(). Was causing a core dump
 * if the DISPLAY could not be opened.
 *
 * Revision 1.22.1.3  1997/06/28  00:04:36  venkat
 * Embedded form implementation with BC. They need to be created
 * explicitly with -nested option and if they are not to be mapped
 * by default, the -noautomap option needs to  be specified.
 *
 * Revision 1.22.1.2  1997/06/16 18:26:19  venkat
 * Changes to set the WMShell XtNtitle resource when the title field is set.
 * Windows are all created with the name 'genesis' which makes resource settings
 * from Xdefaults file easy, but can then be set/unset as necessary using the
 * title field of the xform.
 *
 * Revision 1.22.1.1  1995/09/26  23:28:20  venkat
 * Changed XtVaSetValues() call to XoXtVaSetValues()
 *
 * Revision 1.22  1995/07/29  00:14:36  venkat
 * Fixed memery leak involving form->title
 *
 * Revision 1.21  1995/07/08  01:34:58  venkat
 * Changes-to-allocate-and-deallocate-string-member-resources-in-the-SET-and-DELETE-actions
 *
 * Revision 1.20  1995/06/26  23:14:41  dhb
 * Merged in 1.19.1.3
 *
 * Revision 1.19.1.3  1995/05/19  22:24:23  venkat
 * Some more color specific debugging statements for finding
 * solution to the XProtocol errors on the SGI
 *
 * Revision 1.19.1.2  1995/05/17  23:35:32  venkat
 * Added debugging code to check widget and window color based resources
 * and attributes.
 *
 * Revision 1.19.1.1  1995/05/15  18:28:29  venkat
 * Changed the code format of checking the existence of the shell. It now checks
 * explicitly for being non-null
 *
 * Revision 1.19  1995/05/08  21:51:09  venkat
 * SHOW action attempts to get the shell widgets' geometry resources
 * only if the shell exists. This is because the 'showobject' command
 * also invokes this action in addition to the 'xshow' command.
 *
 * Revision 1.18  1995/05/02  22:01:59  venkat
 * The CREATE action  first checks if the DISPLAY env variable
 * is set and returns after printing an error message, if it is not.
 *
 * Revision 1.17  1995/04/12  18:18:07  venkat
 * The Window manager positioning hack is now shifted to the Xo directory
 * The CREATE and SET actions have been changed to this effect.
 *
 * Revision 1.16  1995/04/04  01:02:59  venkat
 * Made changes to pre-2.0.9 form geometry specification
 * behaviour
 *
 * Revision 1.15  1995/02/22  20:31:34  dhb
 * Changed SHOW action to get width and height resources from the
 * form widget rather than the shell widget.
 *
 * Revision 1.14  1995/02/07  01:46:36  venkat
 * Made sure that form widget was realized before parsing
 * create args such as -bg etc.,
 *
 * Revision 1.13  1995/02/04  01:01:57  venkat
 * Added case for SHOW action to update the
 * geometry fields when showfield/getfield is done
 *
 * Revision 1.12  1995/01/31  18:43:09  venkat
 * CREATE action modified to parse all args including geometry
 * followed by processing of xform specific options
 *
 * Revision 1.11  1995/01/20  20:04:01  dhb
 * Workaround for the form placement problem.  The TopLevelShell
 * widget would require the user to place the window if a geometry
 * field was set while the form was unmapped.  Used the same code
 * as the CREATE action to force window manager to place the form
 * without user interaction.
 *
 * Revision 1.10  1995/01/18  02:30:09  dhb
 * Added -nolabel and -notitle options which allows form without
 * a title label.  Changed handling of title as form name slightly
 * so that other options don't cause the title not to appear.
 * (e.g. could be create xform aForm -xgeom 10).
 *
 * Revision 1.9  1995/01/18  00:44:06  venkat
 * Changed the name of the label widget used to display
 * the title of the form to an inconspicous #@label@#.
 * To avoid the geometry problem when referencing a
 * created label as 'label'.
 *
 * Revision 1.8  1995/01/11  18:09:51  venkat
 * Added code in XForm() "CREATE" action to have form
 * display name if title field not set in command-line
 *
 * Revision 1.7  1994/04/07  13:32:12  bhalla
 * Turned off the auto_resize flag
 *
 * Revision 1.6  1994/03/22  15:06:20  bhalla
 * Added position hints so that twm can handle placement of widgets.
 *
 * Revision 1.5  1994/03/16  16:57:03  bhalla
 * Changed width and height defaults to 300, 375
 *
 * Revision 1.4  1994/02/08  22:27:13  bhalla
 * using xoFullName for widget name during create, added xoGetGeom
 *
 * Revision 1.3  1994/02/08  17:36:42  bhalla
 * added auto_resize field.
 * Added large height and width defaults
 * Put in xoParseCreateArgs
 * Cleaned up the -title option
 *
 * Revision 1.2  1994/02/02  20:30:34  bhalla
 * Added callbacks to form
 * Put in stuff for destroying forms
 * */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Shell.h>
#include <Widg/Form.h>
#include <Widg/Framed.h>
#include <Widg/Label.h>
#include "widg_ext.h"
#include "Xo/Xo_ext.h"
#ifdef XOCOLOR_DEBUG
#include <Xo/XoDefs.h>
#endif


#ifdef XOCOLOR_DEBUG
	Colormap cmap;
	int depth;	
 	Visual *vis;
	VisualID vid;
	XVisualInfo *visuallist;
	XVisualInfo vtemplate;
	int visualsmatched;
	static char  *visualname[] = {
  "StaticGray",
  "GrayScale",
  "StaticColor",
  "PseudoColor",
  "TrueColor",
  "DirectColor"
	};
	XWindowAttributes attrib;
#endif
static Gen2Xo GXconvert[] = {
	{"auto_resize",		XtNauto_resize},
	{"label", 		XtNlabel},
};

int XForm (form, action)
     struct xform_type *form;
     Action            *action;
{
	XWMHints	wm_hints;
	static Window app_group = None;
	int ac=action->argc;
	char** av=action->argv;
	int argc = ac;
	char **argv = av;
    Widget parentW, fW = NULL, xoFindParentForm(), XoGetFramedChild();
    Position xpos,ypos;
    Dimension wdim,hdim;
    static char value[100];
	int add_title;
  	char *cur_arg;
	int i;

   Boolean xoIsNestedForm();
   int noautomap;
  
  if (Debug(0) > 1)
    ActionHeader("XForm", form, action);

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
    break;
  case RESET:
    break;


  case CREATE:
  
    if(!XgDisplay()){
	fprintf(stderr, "Xodus cannot be supported in this display\n");
	fprintf(stderr, "Check DISPLAY env variable and restart genesis\n");
	return(0);
    }

    /* arguments are: object_type name [field value] ... */
	parentW = NULL;
	form->auto_resize = 0;

    /* Any one of these flags should be a part of the form since a number of
        the xcommands implemented in xo/xo_cmds.c map the widget by
        determining the right ancestor of the passed framed child. Since
        the determination is done by a nested XtParent() call, it was easier
        to fold in the nested flag */

	/* Default is a non-nested automapping form */
	form->nested = 0;
	noautomap = 0;	

/** Look for and process the arguments for xform specific
    options -title, -label, -nolabel and -notitle **/

    add_title = 0;
    for(i=0; i < argc; i++) {
	cur_arg = (argv[i][0] == '-') ? argv[i]+1 : argv[i];
        if (strcmp(cur_arg,"title") == 0 ||
            strcmp(cur_arg,"label") == 0) 
			add_title = 1;
	else if (strcmp(cur_arg,"notitle") == 0 ||
		   strcmp(cur_arg,"nolabel") == 0)
			add_title = 0;
	else if (strcmp(cur_arg,"nested") == 0)
			form->nested = 1;	
	else if (strcmp(cur_arg,"noautomap") == 0)
			noautomap = 1;
	else continue;
    }
 
#ifdef XOCOLOR_DEBUG
    	form->shell = (char *)XgCreateTopLevelShell(form->name);
	XtVaGetValues((Widget)form->shell,XtNvisual,&vis,XtNcolormap,&cmap,XtNdepth,
				&depth, NULL);
	fprintf(stderr, "The colormap shell resource is %d\n",(int)cmap);
	fprintf(stderr, "The visual shell resource is %s\n",visualname[vis->class]);
	vid = XVisualIDFromVisual(vis);	
	fprintf(stderr, "The visual id is 0x%x\n", vid);
	vtemplate.visualid = vid;
	visuallist = XGetVisualInfo(XgDisplay(),VisualIDMask,&vtemplate,&visualsmatched);
	fprintf(stderr, "The depth shell resource is %d\n", depth);
	if(visuallist != NULL){
	 fprintf(stderr,"The depth of the visual shell resource is %d\n",visuallist[0].depth);
	 if(visuallist[0].depth != depth)
	 fprintf(stderr, "The depths %d and %d do not match-BadMatch error\n",
		 visuallist[0].depth,depth);
	}
	XtRealizeWidget ((Widget)form->shell);
	XGetWindowAttributes(XgDisplay(),XtWindow((Widget)form->shell),&attrib);
	fprintf(stderr, "The colormap shell window attribute is %d\n",
		(int)attrib.colormap);
	fprintf(stderr, "The visual shell window attribute is %s\n",
		visualname[attrib.visual->class]);
	fprintf(stderr, "The depth shell window attribute is %d\n", attrib.depth);
	XtDestroyWidget((Widget)form->shell);
    	form->shell = (char *)XgCreateTopLevelShell(form->name);
    	form->outerframe = (char *)
      	XtVaCreateManagedWidget ("frame", frameWidgetClass, (Widget) form->shell,
				 XtNelevation, XoFlatFrame,
				 NULL);
	XtVaGetValues((Widget)form->outerframe,XtNvisual,&vis,XtNcolormap,&cmap,XtNdepth,
			&depth, NULL);
	fprintf(stderr, "The colormap outerframe resource is %d\n",(int)cmap);
	fprintf(stderr, "The visual outerframe resource is %s\n",visualname[vis->class]);
	fprintf(stderr, "The depth outerframe resource is %d\n", depth);
	XtRealizeWidget ((Widget)form->shell);
	XGetWindowAttributes(XgDisplay(),XtWindow((Widget)form->outerframe),&attrib);
	fprintf(stderr, "The colormap outerframe window attribute is %d\n",
		(int)attrib.colormap);
	fprintf(stderr, "The visual outerframe window attribute is %s\n",
		visualname[attrib.visual->class]);
	fprintf(stderr, "The depth outerframe window attribute is %d\n", attrib.depth);
	XtDestroyWidget((Widget)form->shell);
#endif

 if (! (xoIsNestedForm(form) && form->nested)) {
    form->shell = (char *)XgCreateTopLevelShell(form->name);

    form->outerframe = (char *)
      XtVaCreateManagedWidget ("frame", frameWidgetClass, (Widget) form->shell,
				 XtNelevation, XoFlatFrame,
				 NULL);
    form->middleframe = (char *) XtVaCreateManagedWidget (
		"form", frameWidgetClass, (Widget) form->outerframe,
				 XtNelevation, XoInsetFrame,
				 NULL);
    form->innerframe = (char *) XtVaCreateManagedWidget (
		"inner", frameWidgetClass, (Widget) form->middleframe,
				 XtNelevation, XoFlatFrame,
				 XtNborderWidth, 4,
				 NULL);
    form->widget = (char *) XtVaCreateManagedWidget (
		xoFullName(form), formWidgetClass, (Widget) form->innerframe,
				 XtNauto_resize, 0,
				 XtNwidth, 300,
				 XtNheight, 375,
				 XtNlabel, form->name, 
				 NULL);
	XtRealizeWidget ((Widget)form->shell);

	/*
	** If this is the first shell created, save the window id. Put this and
	** all subsequent shells in the same application group. This is needed
	** for newer WMs which break legacy apps under the guise of 
	** "focus stealing prevention".
	*/
	if (app_group == None && XtWindow((Widget)form->shell) != 0) {
		app_group = XtWindow((Widget)form->shell);
	}
	wm_hints.flags = WindowGroupHint;
	wm_hints.window_group = app_group;
	XSetWMHints(XgDisplay(), XtWindow((Widget)form->shell), &wm_hints);


 } else {
	parentW  = xoFindParentForm(form);
        fW =  XtVaCreateManagedWidget(
        	xoFullName(form), framedWidgetClass, parentW,
        	XtNchildClass, formWidgetClass,
                XtNmappedWhenManaged, False,
                XtNauto_resize, 0,
                XtNlabel, form->name,
                NULL);
        form->widget = (char *) XoGetFramedChild(fW);
 }
#ifdef XOCOLOR_DEBUG
	XtVaGetValues((Widget)form->widget,XtNvisual,&vis,XtNcolormap,&cmap,XtNdepth,
				&depth, NULL);
	fprintf(stderr, "The colormap form resource is %d\n",(int)cmap);
	fprintf(stderr, "The visual form resource is %s\n",visualname[vis->class]);
	fprintf(stderr, "The depth form resource is %d\n", depth);
#endif
	XtAddCallback((Widget) form->widget, XtNcallback,xoCallbackFn,
		(XtPointer)form);
    ac--, av++;			/* object type */
    ac--, av++;			/* path */


	form->label_w = NULL;
	form->title = NULL;
	form->auto_resize = 1;

#ifdef XOCOLOR_DEBUG
	XGetWindowAttributes(XgDisplay(),XtWindow((Widget)form->widget),&attrib);
	fprintf(stderr, "The colormap form window attribute is %d\n",
		(int)attrib.colormap);
	fprintf(stderr, "The visual form window attribute is %s\n",
		visualname[attrib.visual->class]);
	fprintf(stderr, "The depth form window attribute is %d\n", attrib.depth);
#endif
	xoParseCreateArgs(form,ac,av);


         /*  Create a label widget with the title of the form in it 
	   only if -title or -label is present  and -nolabel and
	-notitle are  not present.
   The weird widget name is to avoid the geometry conflict
    if another child widget references this widget as "label"
    in its geometry specification */

    if (add_title && form->title != NULL && form->label_w==NULL)
	form->label_w = (char *)
	XtVaCreateManagedWidget("#@label@#",labelWidgetClass,
		(Widget)(form->widget),
		XtNlabel,form->title,
		NULL);

	xoGetGeom(form); /* Get the geometry into the geom fields */

	/* form->title will be NULL here if there was no -title/-label option */
	/* The SET action will take care of setting the XtNtitle resource if -title/-label were specified */
	
	if(form->title == NULL)
		form->title = CopyString(form->name);

	if (!(xoIsNestedForm(form) && form->nested)) {
		XoXtVaSetValues((Widget)form->shell, XtNtitle, form->title, NULL);
		XSetIconName((Display *)XgDisplay(),XtWindow((Widget) form->shell),form->name);
		/* This ugly stuff is here for archaic window managers like twm */
		XoWMPositionHack((Widget)form->shell,(Position)atoi(form->xgeom),(Position)atoi(form->ygeom));
	}

	if(parentW != NULL && !noautomap)
		XtMapWidget(fW);
    	return 1;
	/* NOTREACHED */
	break;
  case SET:
	if (ac == 2) {
        if (strcmp(av[0],"title") == 0 ||
            strcmp(av[0],"label") == 0) {
			if (form->label_w != NULL) 
		/* set the string in the label widget */
				XoXtVaSetValues((Widget)form->label_w,
					XtNlabel,av[1],
					NULL);
		/* Or create one if it doesn't exist */
			else
		       	        form->label_w = (char *)
		       	XtVaCreateManagedWidget("#@label@#",labelWidgetClass,
			(Widget)(form->widget),
			XtNlabel,av[1],
			NULL);

			if(form->title != NULL) 
				FreeString(form->title);
			DirectSetElement(form,"title",av[1]);
			if (!(xoIsNestedForm(form) && form->nested))
				XoXtVaSetValues((Widget)form->shell, XtNtitle, av[1], NULL);
			return(1);
		}
	}
    	if (ac) {			/* need to set fields */
		gx_convert(form,GXconvert,XtNumber(GXconvert),ac,av);

		/* setting geometry fields cause the top level shell
		   widget to require user pacement of the form.  This
		   code is a work around (same as in CREATE action)
		   to force the window manager to place the form at
		   the given location.
		*/
		xoGetGeom(form);
		if (! (xoIsNestedForm(form) && form->nested)) 
			XoWMPositionHack((Widget)form->shell,(Position)atoi(form->xgeom),(Position)atoi(form->ygeom));

		return(0);
	}
	break;
  case SHOW:

 	/** Handles updation of element geometry fields 
	    when window is moved by window manager, not resized */
      if(form->shell != NULL && ac > 0){
	if(strcmp(action->data, "xgeom")==0){
	XtVaGetValues((Widget)form->shell,XtNx,&xpos,NULL);
 	 sprintf(value,"%d",xpos);
	 DirectSetElement(form,"xgeom",value);
	}
	if(strcmp(action->data, "ygeom")==0){
	XtVaGetValues((Widget)form->shell,XtNy,&ypos,NULL);
 	sprintf(value,"%d",ypos);
	DirectSetElement(form,"ygeom",value);
	}
	if(strcmp(action->data, "wgeom")==0){
	XtVaGetValues((Widget)form->shell,XtNwidth,&wdim,NULL);
 	sprintf(value,"%d",wdim);
	DirectSetElement(form,"wgeom",value);
	}
	if(strcmp(action->data, "hgeom")==0){
	XtVaGetValues((Widget)form->shell,XtNheight,&hdim,NULL);
 	sprintf(value,"%d",hdim);
	DirectSetElement(form,"hgeom",value);
	}
      }
	return(0);
	/* NOTREACHED */
	break;	
  case DELETE:
	if (!(xoIsNestedForm(form) && form->nested) && form->shell != NULL) 
		XtDestroyWidget((Widget)form->shell);
	else 
		if (form->widget != NULL)
			XtDestroyWidget((Widget)XtParent((Widget)form->widget));

	if (form->title != NULL) free(form->title);	
	break;
  case XUPDATE : /* update label fields due to changes in widget */
		if(ac > 0)
		xg_convert(form,GXconvert,XtNumber(GXconvert),ac,av);
	break;
  }
	return(0);
}
