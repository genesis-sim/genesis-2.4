/* $Id: xtree.c,v 1.3 2005/07/20 20:01:57 svitak Exp $ */
/*
 * $Log: xtree.c,v $
 * Revision 1.3  2005/07/20 20:01:57  svitak
 * Added standard header files needed by some architectures.
 *
 * Revision 1.2  2005/07/01 10:02:59  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1  2005/06/17 21:23:51  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.35  2001/06/29 21:11:58  mhucka
 * Added extra parens inside conditionals and { } groupings to quiet certain
 * compiler warnings.
 *
 * Revision 1.34  2001/04/18 22:39:35  mhucka
 * Miscellaneous portability fixes, mainly for SGI IRIX.
 *
 * Revision 1.33  2000/06/12 04:25:42  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.32  2000/05/02 06:18:31  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.31  1998/07/15 06:23:58  dhb
 * Upi update
 *
 * Revision 1.30  1997/07/16 21:33:09  dhb
 * Change getopt() calls to avoid conflict with unix version.
 *
 * Revision 1.29  1996/07/02 18:19:36  venkat
 * Excess reallocated memory is initialized with the xomemzero() call where
 * appropriate
 *
 * Revision 1.28  1995/10/16  19:55:53  dhb
 * Merged in 1.26.1.1 fixes.
 *
<<<<<<< 1.27
 * Revision 1.27  1995/09/27  00:09:32  venkat
 * XtVaSetValues()-calls-changed-to-XoXtVaSetValues()-to-avoid-alpha-FPE's
 *
=======
 * Revision 1.26.1.1  1995/10/16  19:53:55  dhb
 * Fix from Upi Bhalla: avoids call to create_hash() with zero
 * table size.
 *
>>>>>>> 1.26.1.1
 * Revision 1.26  1995/08/05  16:54:27  dhb
 * Patch from Upi Bhalla.
 *
 *
 * Upi Bhalla Mt Sinai Aug 5 1995. Fix for ADDMSGARROW problem with
 * script_add and script_drop functions getting the wrong 'tree->value'
 *
 * Revision 1.25  1995/06/26  23:36:51  dhb
 * Merged in 1.24.1.2
 *
 * Revision 1.24.1.2  1995/06/21  23:28:37  venkat
 * Changed ReallocArrows() also calloc the fromlist, tolist and the arrowflags
 * if they had not been alloced earlier. Also tree->max_nlist is initialized
 * in the CREATE action. The message which warns the user to set the path field
 * as absolute path names now appears only if the path has been set as a relatiive path name
 *
 * Revision 1.24.1.1  1995/06/15  20:28:14  venkat
 *  WildcardGetElement() always returns a valid pointer to the head of a locally
 * created element list. So checking this for null is not reasonable but checking whether the no of elements found in the path is. Also the warning message
 * now makes a note of using absolute pathnames for setting the path field.
 *
 * Revision 1.24  1995/04/04  00:58:12  venkat
 * Fixed bug involving ElementIsA() affecting drag-drop
 * operation.
 *
 * Revision 1.23  1995/03/28  21:12:01  venkat
 * COPY
 *
 * Revision 1.22  1995/03/22  00:21:02  venkat
 * Changed occurances of BaseObject() to ElementIsA()
 * This may not have been necessary but for an inadvertant loss
 * of a version during the course of merging upi's changes.
 *
 * Revision 1.21  1995/03/17  20:54:51  venkat
 * Upi inclusions and changes
 *
 * Revision 1.20  1995/01/23  03:27:49  bhalla
 * 1. Got rid of unnecessary definition of create_interpol
 * 2. put in check for tree->ncoords to be greater than zero before
 * attempting to create hash table.
 *
 * Revision 1.19  1994/11/07  22:06:03  bhalla
 * Fix at Dave Bilitch's behest: use calloc rather than realloc
 * for initial allocation, since some UNIX systems die when realloc
 * is used on a NULL pointer.
 *
 * Also: removed the check for colors when undumping the msgarrow list.
 *
 * Revision 1.18  1994/06/13  23:00:00  bhalla
 * Added pixflags c for child wigdets
 * enhanced namemode to allow exclusion of certain objects
 * assorted bugfixes
 *
 * Revision 1.17  1994/06/06  16:00:59  bhalla
 * Changed DUMP and UNDUMP so that it now saves the full path of each
 * element whose coords are being saved (for the 'custom' layout). This
 * allows it to reload over existing and changed layouts and paths.
 *
 * Revision 1.16  1994/05/25  13:38:57  bhalla
 * Added orientation options for tree
 *
 * Revision 1.15  1994/05/09  20:10:16  bhalla
 * Corrected bug where truncating in the custom mode failed to get rid of
 * the msgarrows, and eventually led to core dumps.
 *
 * Revision 1.14  1994/04/18  19:21:04  bhalla
 * Added the value field to the list of read-only fields
 *
 * Revision 1.13  1994/04/11  21:15:21  bhalla
 * Added facility to look up extended fields xtree_shape_fg,
 * xtree_fg_req and xtree_textfg_req to directly set these fields in
 * the ptlist
 *
 * Revision 1.12  1994/04/07  13:31:02  bhalla
 * Corrected problem with loading in coords from a dump file.
 *
 * Revision 1.11  1994/03/30  17:20:51  bhalla
 * Added an object field for the coord_hash which is used so often that
 * it is stupid to rebuild it in every function
 * Added new action TRUNCATE to handle truncation in custom and other
 * treemodes.
 *
 * Revision 1.10  1994/03/29  17:16:16  bhalla
 * Exceedingly painful conversion of all the individual arrays of item
 * properties, such as coords, shapeindex, shapeflags etc into a single
 * struct.
 * Also changed the way in which ptnames are allocated, using a single large
 * array for all the ptnames rather than having multiple mallocs. This was
 * really the cause of much of the difficulty. But it should greatly ease
 * memory leaks and fragmentation.
 *
 * Revision 1.9  1994/03/22  15:09:39  bhalla
 * Cleaned up DUMP and UNDUMP actions, added check for duplication of
 * old arrows (IsArrowDuplicated) during undump.
 *
 * Revision 1.8  1994/03/16  17:02:16  bhalla
 * Sorry, too many changes accumulated before this update:
 * In no particular order:
 * 1. extended AddMsgArrow to handle the fliparrow option
 * 2. for obj_grid and obj_custom, use the name of the object as the
 * 	value of the tree when clicked.
 * 3. Added check for last_tree when looking for last item to be clicked
 * 	on. Since these fields are static, there was a bug when multiple
 * 	trees were used in the same simulation
 * 4. Added option to skip an arrow if its color is "none"
 * 5. Added a lot of stuff to handle display of objects in obj_grid/obj_custom
 * 6. Fixed bug with losing ptnames when new items were added.
 *
 * Revision 1.7  1994/03/01  14:40:25  bhalla
 * Added DUMP and UNDUMP actions
 * Added MOVECUSTOM action
 * Assorted cleanups
 *
 * Revision 1.6  1994/02/08  22:33:37  bhalla
 * ci bug - I made no changes here
 *
 * Revision 1.5  1994/02/02  20:04:53  bhalla
 * Added code to handle drops between components of xtree. This also
 * involves a lot of code from CoreDraw.c and xo_callb.c
 * Extended the MsgArrow operations to allow functions to be called
 * specific to drag operations between identified msg types
 *
 * Revision 1.5  1994/02/02  20:04:53  bhalla
 * Added code to handle drops between components of xtree. This also
 * involves a lot of code from CoreDraw.c and xo_callb.c
 * Extended the MsgArrow operations to allow functions to be called
 * specific to drag operations between identified msg types
 *
 * Revision 1.4  1994/01/31  23:11:44  bhalla
 * Updated to Genesis 2; Fixed bug by creation of shapevals array.
 *
 * Revision 1.3  1994/01/10  23:47:22  bhalla
 * Added tree truncation by double-click
 *
 * Revision 1.2  1994/01/08  15:48:15  bhalla
 * Initial functioning version, can do tree and geometry modes only, no msgs
 *
 * Revision 1.1  1994/01/01  19:20:08  bhalla
 * Initial revision
 * */

#include <math.h>	/* sqrt() */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Widg/Framed.h"
#include "Draw/Pix.h"
#include "Draw/Tree.h"
#include "draw_ext.h"
#include "_widg/xform_struct.h"
#include "Xo/XoDefs.h"
#include "Xo/Xo_ext.h"
#include "hash.h"

#define XO_TREE_MAXSHAPES 100
#define XO_TREE_MAXARROWS 1000
#define XO_ELM_NOT_FOUND -1
extern ElementList *CreateElementList();

static Gen2Xo GXconvert[] = {
	{"tx",		XtNpixX},
	{"ty",		XtNpixY},
	{"tz",		XtNpixZ},
	{"highlight1",		XtNhighlight1},
	{"highlight2",		XtNhighlight2},
	{"hlmode",		XtNhlmode},
};

struct tree_struct {
	int next;
	int child;
};
		
static int ScanShapes();
static int BuildTreeList();
static int BuildTreeCoords();
static int BuildTreeNames();
static int FindShapeIndex();
static void AssignTreeCoords();
static int TreeLayout();
static void CustomLayout();
static int GridLayout();
static int ReallocArrows();
static HASH *FindMsgSrc();
static ElementList *FindMsgDest();
static void AddMsgArrow();
static void FindMsgArrows();
static int OnTreePath();
static int IsAncestor();
static int IsArrowDuplicated();
static int AddPtname();
static int FindElistIndex();
static int TruncateKids();
static void RecurseTruncate();

int XTree (tree, action)
     register struct xtree_type *tree;
     Action                     *action;
{
	int ac=action->argc;
	char** av=action->argv;
    Widget parentW, xoFindParentDraw();
	TreeObject tW;
	int  i;
	struct 	xtree_type *copy_tree;
	struct	xshape_type	*xshape;
	Action	newaction;
	char	*newargv[15];
	static	char	lasttree[100];
	static int		lastsrc;
	Element	*elm;
  
  if (Debug(0) > 1)
    ActionHeader("XTree", tree, action);

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
	XoUpdateTree((Widget)tree->widget);
    break;
  case RESET:
  /* This first step is inherited from the xvar. It scans the list of
	of children to find the applicable shapes, and fills in
	the lists */
	if (ScanShapes(tree) > 0) {

/* Here we fill in the values of the xtree */
	/* allocate tables */
		BuildTreeList(tree,0);
	/* assign coords for shape icons */
		BuildTreeCoords(tree);
	/* assign names to each icon */
		BuildTreeNames(tree);

	/* Set up the message arrows */
		FindMsgArrows(tree);

	/* Setting all the values and tables */
		if (tree->widget) 
			XoXtVaSetValues((Widget)tree->widget,
				XtNshapes, (XtPointer)tree->shapes,
				XtNnshapes, tree->nshapes,
				XtNshapechange, (int)1,
				XtNptlist,(XtPointer)tree->ptlist,
				XtNncoords,tree->ncoords,
				XtNfromlist,tree->fromlist,
				XtNtolist,tree->tolist,
				XtNarrowflags,tree->arrowflags,
				XtNnlist,tree->nlist,
				NULL);
		XoProjectTree((Widget)tree->widget);
	}
    break;
  case CREATE:
	/* Must always create a child shape for the default cases */
	/* If special cases are needed then they can be added */
    /* arguments are: object_type name [field value] ... */
	if ((parentW = xoFindParentDraw(tree)) == NULL) return(0);

	/* assign memory for the value field */
	tree->value = (char *)calloc(200,sizeof(char));


	/* Make the arrays for the coords */
	/* Make the array for the values */
	/* set up some defaults */
	tree->ncoords = 0;
	tree->sizescale = 1.0;
	tree->treemode = "tree";
	tree->namemode = "auto";
	tree->fg = "black";
	tree->highlight1 = "red";
	tree->highlight2 = "gray";
	tree->hlmode = "none";
	tree->orientation = "up";
	tree->max_nlist = 100;
	/* Create an element list with space for ten elements. This will
	** usually be enough, if not we will expand it later */
	tree->select_elist = CreateElementList(10);



	tW = (TreeObject)XtCreateManagedWidget(
		av[1], treeObjectClass, parentW, NULL,0);
    tree->widget = (char *)tW;
	/* copying over the table pointers and defaults */
	XoXtVaSetValues(tW,
		XtNcolorscale, (XtPointer)XoColorscale(),
		XtNncols,(int)XoNcolors(),
		NULL);
	XtAddCallback((Widget) tree->widget, XtNcallback,xoCallbackFn,
		(XtPointer)tree);
	AddMsgArrow(tree,NULL,NULL,NULL,NULL,NULL,0,NULL,NULL,NULL,NULL);

    ac--, av++;			/* object type */
    ac--, av++;			/* path */
	xoParseCreateArgs(tree,ac,av);
	newaction.type = CREATE;
	newaction.name = "CREATE";
	newaction.data = (char *)tree;
	newaction.argc = 12;
	newaction.argv = newargv;
	newaction.argv[0] = "xshape";
	newaction.argv[1] = "shape[0]";
	newaction.argv[2] = "-fg";
	newaction.argv[3] = "white";
	newaction.argv[4] = "-coords";
	newaction.argv[5] = "[-0.5,-0.5,0][-0.5,0.5,0][0.5,0.5,0][0.5,-0.5,0]";
	newaction.argv[6] = "-drawmode";
	newaction.argv[7] = "FillPoly";
	newaction.argv[8] = "-pixflags";
	newaction.argv[9] = "v";
	newaction.argv[10] = "-pixflags";
	newaction.argv[11] = "c";
	elm = Create("xshape","shape",tree,&newaction,0);
	return(1);
	/* NOTREACHED */
	break;
  case COPY: /* currently incomplete */
	/* Note that the 'tree' pointer refers to the original,
	** and the data contains the pointer to the copy */
	copy_tree = (struct xtree_type *)action->data;
	if ((parentW = xoFindParentDraw(copy_tree)) == NULL) return(0);

	/* We need to duplicate the shapes */
	/* we need to duplicate the value */
	/* creating a new widget */
		tW = (TreeObject)XtCreateManagedWidget(
			copy_tree->name, treeObjectClass, parentW, NULL,0);
    	copy_tree->widget = (char *)tW;
	/* copying over the table pointers */
/*
		XoXtVaSetValues(tW,
			XtNxpts,(XtPointer)((Interpol *)copy_tree->xpts)->table,
			XtNypts,(XtPointer)((Interpol *)copy_tree->ypts)->table,
			XtNzpts,(XtPointer)((Interpol *)copy_tree->zpts)->table,
			XtNpixflags,copy_tree->pixflags,
			NULL);
*/
		
	/* adding the callback */
		XtAddCallback((Widget) copy_tree->widget, XtNcallback,xoCallbackFn,
			(XtPointer)copy_tree);
	/* Converting the rest of the values */
		gx_convert_all(copy_tree,GXconvert,XtNumber(GXconvert));
		return(1);
		/* NOTREACHED */
	break;
  case SET:
	if (xoSetPixFlags(tree,ac,av)) return(1);
    if (ac) {			/* need to set fields */
		/* a useful function for messing with the display size */
		if (strcmp(av[0],"sizescale") == 0) {
			for (i = 0; i < XO_TREE_MAXSHAPES; i++) {
				xshape = (struct xshape_type *)
					GetChildElement(tree,"shape",i);
				if (!xshape) break;
				scale_table(xshape->xpts,"sy",av[1]);
				scale_table(xshape->ypts,"sy",av[1]);
				scale_table(xshape->zpts,"sy",av[1]);
			}
			if (i > 0) {
				XoProjectTree((Widget)tree->widget);
				return(0);
			}
		}
		if (
			strcmp(av[0],"max_nlist") == 0 ||
			strcmp(av[0],"ncoords") == 0 ||
			strcmp(av[0],"xpts") == 0 ||
			strcmp(av[0],"ypts") == 0 ||
			strcmp(av[0],"zpts") == 0 ||
			strcmp(av[0],"shapes") == 0 ||
			strcmp(av[0],"nshapes") == 0 ||
			strcmp(av[0],"fromlist") == 0 ||
			strcmp(av[0],"tolist") == 0 ||
			strcmp(av[0],"arrowflags") == 0 ||
			strcmp(av[0],"coord_elist") == 0 ||
			strcmp(av[0],"value") == 0
			) {
			printf("Warning: %s is a readonly field\n",av[0]);
			return(1);
		}
		/* Do special things here for backwards compatibility */
		if (strcmp(av[0],"treemode") == 0) {
			if (strcmp(av[1],"tree") == 0) {
			}
		}
		gx_convert(tree,GXconvert,XtNumber(GXconvert),ac,av);
		return(0);
	}
	break;
  case DELETE:
	if (tree->value) free(tree->value);	
	if (tree->coord_elist) FreeElementList(tree->coord_elist);
	if (tree->ncoords > 0 && tree->ptlist != NULL) {
		if (tree->_ptnames)
			free(tree->_ptnames);
		free(tree->ptlist);
	}
	if (tree->max_nlist > 0) {
		free(tree->fromlist);
		free(tree->tolist);
		free(tree->arrowflags);
	}
	/* get rid of shapes here */
	if (tree->shapes && tree->nshapes > 0)
		free(tree->shapes);
	if (!(tree->widget)) return(0);
	XtDestroyWidget((Widget)tree->widget);
	break;
  case DUMP:
	if (ac  == 1) {
		char *path = Pathname(tree);
		/* We do not want to do any pre-dump operations */
		if (strcmp(av[0],"pre") == 0) {
			action->passback = (char *)0;
			return(0);
		}
		if (strcmp(av[0],"post") == 0) {
			int i;
			FILE *fp;
			if (!(fp = (FILE *)action->data))
				return(0);
			/* here we undump the msgarrows. However, these are
			** done using the existing ADDMSGARROW call */
			for(i = 0; i < tree->nmsgtypes; i++) {
				char *s0 = tree->msgarrows[i].script;
				char *s1 = tree->msgarrows[i].script_add;
				char *s2 = tree->msgarrows[i].script_drop;
				fprintf(fp,"call %s ADDMSGARROW \\\n",path);
				if (!s0) s0 = "";
				if (!s1) s1 = "";
				if (!s2) s2 = "";
				fprintf(fp,
					"%19s %19s %9s %9s %7s %d %d \\\n",
					tree->msgarrows[i].msgsrc,
					tree->msgarrows[i].msgdest,
					tree->msgarrows[i].msgtype,
					tree->msgarrows[i].msgdesttype,
					tree->msgarrows[i].msgfgstr,
					-XO_TREE_MAXARROWS - i,
					tree->msgarrows[i].fliparrow);
				fprintf(fp,
					"  \"%s\" \"%s\" \"%s\"\n",
					s0,s1,s2);
			}

			/* here we undump the coords. We only have to
			** do this for custom mode, so check that first */
			if (strcmp(tree->treemode, "custom") == 0) {
				/* Each dump line has the full path of the dumped
				** element as well as its coords. This lets us
				** maintain coherence between dump and undump,
				** though it is expensive in space. In particular,
				** we can undump onto an existing tree element and
				** it will handle missing items cleanly and not
				** be confused by changes in element order 
				*/
				fprintf(fp,"call %s UNDUMP %d",
					path,tree->ncoords);
				for(i = 0; i < tree->ncoords; i++) {
					fprintf(fp," \\\n \"%s\" %g %g %g",
						Pathname(tree->coord_elist->element[i]),
						tree->ptlist[i].xpts,
						tree->ptlist[i].ypts,
						tree->ptlist[i].zpts);
				}
				/* clean off the last line */
				fprintf(fp,"\n");
			}
			return(0);
		}
	}
  break;
  case UNDUMP:
	/* I will not use initopt here since the number of args is
	** slightly colossal */
	if (ac > 0) {
		int i;
		int j;
		Element *elm;

		BuildTreeList(tree,0);

		/*
		** Here we load in the coords. The rule followed is that 
		** we load in all coords for specified elements that are
		** found in the tree.
		*/
		for(j = 1; j < (ac - 3); j += 4) {
			elm = GetElement(av[j]);
			if (elm) {
				i = FindElistIndex(tree,elm);
				if (i != XO_ELM_NOT_FOUND) {
					tree->ptlist[i].xpts = Atof(av[j+1]);
					tree->ptlist[i].ypts = Atof(av[j+2]);
					tree->ptlist[i].zpts = Atof(av[j+3]);
				}
			}
		}
	}
  break;
  case ADDMSGARROW:
	{
	int status;
	char *s0,*s1,*s2;
	int index = tree->nmsgtypes;
	s0 = s1 = s2 = NULL;

	/* shuffle so that we have the calling funcname too */
	ac++;
	av--;

	initopt(ac,av,"msgsrc msgdest msgtype msgdesttype fg index fliparrow script script_add script_drop");
	if ((status = G_getopt(ac,av)) != 0) {
		printoptusage(ac,av);
		break;
	} else {
		if ((index = atoi(optargv[6])) < 0) {
		/* This case is to avoid reassignment of pre-existing indices */
			if (IsArrowDuplicated(index,tree,optargv))
				break;
			else
				index = tree->nmsgtypes;
		}
		AddMsgArrow(tree,optargv[1],optargv[2],  optargv[3],
			optargv[4], optargv[5],index,optargv[7],optargv[8],
			optargv[9],optargv[10]);
	}

	}
	break;
  case SHOWMSGARROW: { /* this case is for debugging mainly */
	char *s0;
	char *s1;
	char *s2;

	printf("nmsgtypes = %d, max_nmsgtypes = %d\n",
		tree->nmsgtypes,tree->max_nmsgtypes);
	printf("%7s%20s%20s%10s%10s%8s f\n",
		"msgno","msgsrc","msgdest","msgtype","desttype","color");
	for(i = 0 ; i < tree->nmsgtypes; i++) {
		s0 = tree->msgarrows[i].script;
		s1 = tree->msgarrows[i].script_add;
		s2 = tree->msgarrows[i].script_drop;
		if (!s0) s0 = "";
		if (!s1) s1 = "";
		if (!s2) s2 = "";
		printf("%7d %19s %19s %9s %9s %7s %1d\n	script='%s'\n	add='%s'\ndrop='%s'\n",
			i,
			tree->msgarrows[i].msgsrc,
			tree->msgarrows[i].msgdest,
			tree->msgarrows[i].msgtype,
			tree->msgarrows[i].msgdesttype,
			tree->msgarrows[i].msgfgstr,
			tree->msgarrows[i].fliparrow,
			s0,s1,s2);
	}
	}
    break;
  case MOVECUSTOM: /* elmpath x y z recurse_flag */
	/* this case is for handling xyz moves in custom mode. 
	** It involves identifying the 'moved' element,
	** setting the xyz coords for it in the xtree geometry, and
	** recursing down its children to move them by the same offset.
	** Following all this it updates the xtree display
	*/
	/* shuffle so that we have the calling funcname too */
	ac++;
	av--;

	action->passback = "0";
	initopt(ac,av,"elmpath x y z recurse_flag");
	if (G_getopt(ac,av) != 0) {
		printoptusage(ac,av);
		return(0);
	}
	if ((elm = GetElement(optargv[1]))) {
		int i;
		float x, y, z;
		float inx = Atof(optargv[2]);
		float iny = Atof(optargv[3]);
		float inz = Atof(optargv[4]);
		int movekids = atoi(optargv[5]);
		Element	**elmarray = tree->coord_elist->element;

		if ((i = FindElistIndex(tree,elm)) == XO_ELM_NOT_FOUND)
			return(0);
		x = inx - tree->ptlist[i].xpts;
		y = iny - tree->ptlist[i].ypts;
		z = inz - tree->ptlist[i].zpts;
		if (movekids == 1) { /* apply move to kids */
			int j;
			for(j = 0; j < tree->ncoords; j++) {
				if (IsAncestor(elm,elmarray[j])) {
					tree->ptlist[j].xpts += x;
					tree->ptlist[j].ypts += y;
					tree->ptlist[j].zpts += z;
				}
			}
		}
		/* In any case apply move to self */
		tree->ptlist[i].xpts += x;
		tree->ptlist[i].ypts += y;
		tree->ptlist[i].zpts += z;
		XoProjectTree((Widget)tree->widget);
		return(1);
	}
	return(0);
	/* NOTREACHED */
	break;
  case TRUNCATE : /* elmname [mode] */
	/* shuffle so that we have the calling funcname too */
	ac++;
	av--;

	action->passback = "0";
	initopt(ac,av,"elmname [mode]");
	if (G_getopt(ac,av) != 0) {
		printoptusage(ac,av);
		return(0);
	}
	if (ac == 3) /* mode is specified */
		TruncateKids(tree,optargv[1],atoi(optargv[2]));
	else
		TruncateKids(tree,optargv[1],-1);

	/* Redo all the arrows */
	ReallocArrows(tree,0);
	FindMsgArrows(tree);

	/* Let the widget know about the arrow changes */
	if (tree->widget) 
		XoXtVaSetValues((Widget)tree->widget,
			XtNptlist,(XtPointer)tree->ptlist,
			XtNncoords,tree->ncoords,
			XtNfromlist,tree->fromlist,
			XtNtolist,tree->tolist,
			XtNarrowflags,tree->arrowflags,
			XtNnlist,tree->nlist,
			NULL);

	/* Update the tree */
	XoProjectTree((Widget)tree->widget);
	break;
  case XUPDATE : /* update tree fields due to changes in widget */
		xg_convert(tree,GXconvert,XtNumber(GXconvert),ac,av);
	break;
  default:
  /* This case is complicated because we want to do special things
  ** for the tree pix. 
  ** First specialization : double clicks toggle truncation of tree
  ** Second specialization: drops need to be checked for msgarrow
  **						related operations
  ** Third specialization:  clicks need to be checked for changes
  **						in the select_elist.
  */
  /* the info->ret contains the index of the element clicked on */
	i = -1;
	if (action->data)
		i = *((int *)(((XoEventInfo *)(action->data))->ret));
	if (tree->coord_elist && tree->ncoords > 0 &&
		i >= 0 && i < tree->coord_elist->nelements) {
		if (strcmp(tree->treemode,"obj_grid") == 0 ||
			strcmp(tree->treemode,"obj_custom") == 0) {
			strcpy(tree->value,
				tree->coord_elist->element[i]->object->name);
		} else {
			strcpy(tree->value,Pathname(tree->coord_elist->element[i]));
		}

		/*  This is where we do the tree truncation */
		if (action->type == B1DOUBLE &&
			strcmp(tree->treemode,"tree") ==0) {
			/* special case for doing tree truncate etc. May need to
			** have another flag to check in this case */
			/* Do an exclusive or. This flips only the TRUNCATE bit */
			tree->ptlist[i].shapeflags =
				XO_TREE_TRUNCATE ^ tree->ptlist[i].shapeflags;
			/* Propagate the change into the display */
			BuildTreeCoords(tree);
			/* Set up the message arrows */
				FindMsgArrows(tree);
		
			/* Setting all the values and tables */
			if (tree->widget) {
				XoXtVaSetValues((Widget)tree->widget,
					XtNncoords,tree->ncoords,
					XtNfromlist,tree->fromlist,
					XtNtolist,tree->tolist,
					XtNarrowflags,tree->arrowflags,
					XtNnlist,tree->nlist,
					NULL);
				XoProjectTree((Widget)tree->widget);
				xoUpdateParentDraw(tree);
			}
  		}

		/* set up the lasttree and lastsrc for future drops */
		if (action->type == XODRAG) {
			strcpy(lasttree,Pathname(tree));
			lastsrc = i;
		}

		/* Handle drops from one part of the tree to another */
		if (action->type == XODROP &&
			strcmp(lasttree,Pathname(tree)) == 0) {
			int dest = i;
			Element *destelm = tree->coord_elist->element[dest];
			Element *srcelm = tree->coord_elist->element[lastsrc];
			int j;
			int foundit = 0;
			struct xtree_msgarrow_type *arrow;
			MsgIn	*msg;
			int		mtype;
			char	*tempvalue;
#ifdef VERSION2
			int msgcount;
#endif
	
		tempvalue = (char *)calloc(200,sizeof(char));
		strcpy(tempvalue,tree->value);

		/* check if the src and dest match with a msgarrow spec */
			for(j = 0;j < tree->nmsgtypes; j++) {
				arrow = &(tree->msgarrows[j]);
				if (arrow->script == NULL && arrow->script_add == NULL
					&& arrow->script_drop == NULL)
					continue;
				if (OnTreePath(tree,j,0,lastsrc) &&
					OnTreePath(tree,j,1,dest)) {
		/* call the msgarrow script */
					xoExecuteFunction(tree,action,arrow->script,
						tree->value);
		/* Restore the old tree->value from tempvalue, since it
		** may be changed by the XOWASDROPPED recursive call here */
			strcpy (tree->value,tempvalue);
		/* check if there is a valid msg of the appropriate type */
				if (arrow->msgtype) {
					if (strcmp(arrow->msgtype,"all") == 0) {
						mtype = -1;
					} else {
						MsgList		*msgspec,*GetMsgListByName();
						msgspec = GetMsgListByName(destelm->object,
							arrow->msgtype);
						if (msgspec)
							mtype = msgspec->type;
						else /* go on to the next arrow */
							continue;
					}
				} else { /* go on to the next arrow */
					continue;
				}
#ifdef VERSION2
			/* look for a msg on destelm of type mtype whose
			** src is the same as srcelm */
					msg = destelm->msgin;
					for(msgcount = 0; msgcount < destelm->nmsgin;
						msgcount++) {
						if (msg->src == srcelm && 
							(mtype == -1 || msg->type == mtype)) {
							foundit = 1;
							break;
						}
						msg = MSGINNEXT(msg);
					}
#endif
		/* call either the msgarrow script_add or script_drop */
		/* cf tempvalue: we do NOT want to restore tree->value here,
		** because we should leave it as it was for the last
		** action called, ie XOWASDROPPED */
					if (foundit) {
						xoExecuteFunction(tree,action,
							tree->msgarrows[j].script_drop,tree->value);
					} else {
						xoExecuteFunction(tree,action,
							tree->msgarrows[j].script_add,tree->value);
					}
				}
			}
			free (tempvalue);
		}


/* Here we do the third specialization: filling the select_elist */
		if (action->type == B1DOWN ||
			action->type == B2DOWN ||
			action->type == B3DOWN) {
			int j;
			if (strcmp(tree->hlmode,"one") == 0) {
				tree->select_elist->element[0] =
					tree->coord_elist->element[i];
				tree->select_elist->nelements = 1;
			} else if (strcmp(tree->hlmode,"multi") == 0) {
				tree->select_elist->nelements = 0;
				for(j = 0; j < tree->ncoords; j++) {
					if (tree->ptlist[j].shapeflags == XO_TREE_HIGHLIGHT2)
						AddElementToList(tree->select_elist,
							tree->coord_elist->element[j]);
				}
			} else { /* hlmode is "none" */
				tree->select_elist->nelements = 0;
			}
		}
	} else {
		tree->value[0] = '\0';
	}
	xoExecuteFunction(tree,action,tree->script,tree->value);
	break;
  }
  return 0;
}

static int FindShapeIndex(tree,shapeval)
	struct xtree_type	*tree;
	char				*shapeval;
{
	char *id;
	static char *last_id = NULL;
	static int last_index = 0;
	static struct xtree_type *last_tree = NULL;
	int i;

	if (!shapeval || !(tree->treemode)) return 0;
	if (tree->iconfunc) {
		/* do iconfunc stuff here */
		return(0);
	}

	/* something to speed it up a bit */
	if (last_tree == tree && last_id &&
		strcmp(last_id,shapeval) == 0)
		return(last_index);

	for(i = 0; i < tree->nshapes; i++) {
		id = tree->shapevals[i];
		if (id && strcmp(id,shapeval) == 0) {
			last_id = id;
			last_index = i;
			last_tree=tree;
			return(i);
		}
	}
	return(0);
}

static int TreeLayout(tree)
	struct xtree_type	*tree;
{
	int		nelms;
	Element	*elm;
	Element	*parent;

	struct tree_struct root;
	struct tree_struct *orig;
	int		child, x;
	int		i;
	float	temp;


	if (!tree->coord_elist) return(0);
	if ((nelms = tree->coord_elist->nelements) <= 0) return 0;

	/* set up the from-to lists */
	ReallocArrows(tree,nelms);

	root.child = -1;
	orig = (struct tree_struct *)
		calloc(nelms,sizeof(struct tree_struct));
	for(i = 0 ; i < nelms; i++) {
		elm = tree->coord_elist->element[i];
		orig[i].child = orig[i].next = -1;
		if ((parent = elm->parent)) { /* root element */
			int elmno = FindElistIndex(tree,parent);
			if (elmno == XO_ELM_NOT_FOUND) {
				orig[i].next = root.child;
				root.child = i;
			} else {
				orig[i].next = orig[elmno].child;
				orig[elmno].child = i;
			}
		} else {
			orig[i].next = root.child;
			root.child = i;
		}
	}

	/* recursively set up tree coords */
	x = 0;
	tree->nlist = 0;
	for(child = root.child; child != -1; child = orig[child].next) {
		AssignTreeCoords(tree,child,orig,&x,0);
		x++;
	}
	free(orig);

	/* Figure out how tree is meant to be oriented */
	switch(tree->orientation[0]) {
		case 'd' : /* flip the y coordinate */
			for(i = 0; i < tree->ncoords; i++)
				tree->ptlist[i].ypts = -tree->ptlist[i].ypts;
			break;
		case 'l' : /* Flip the y coordinate and fall through to 'r' */
			for(i = 0; i < tree->ncoords; i++)
				tree->ptlist[i].ypts = -tree->ptlist[i].ypts;
		case 'r' : /* swap the x and y coordinates */
			for(i = 0; i < tree->ncoords; i++) {
				temp = tree->ptlist[i].ypts;
				tree->ptlist[i].ypts = tree->ptlist[i].xpts;
				tree->ptlist[i].xpts = temp;
			}
			break;
		/* by default the tree is oriented upwards */
	}
}


static void AssignTreeCoords(tree,curr,orig,x,y)
	struct xtree_type	*tree;
	int		curr;
	struct tree_struct	*orig;
	int					*x;
	int					y;
{
	int child;
	int	trunc = (tree->ptlist[curr].shapeflags &
		(XO_TREE_TRUNCATE | XO_TREE_TRUNCATED));

	tree->ptlist[curr].xpts = *x;
	tree->ptlist[curr].ypts = y;
	/* get rid of attempts to truncate a childless branch */
	if ((tree->ptlist[curr].shapeflags & XO_TREE_TRUNCATE) &&
		orig[curr].child == -1) {
		tree->ptlist[curr].shapeflags &= ~XO_TREE_TRUNCATE;
		trunc = 0;
	}
	
	for(child = orig[curr].child;child != -1;child = orig[child].next) {
		if (trunc)
			tree->ptlist[child].shapeflags |= XO_TREE_TRUNCATED;
		else
			tree->ptlist[child].shapeflags &= ~XO_TREE_TRUNCATED;

		AssignTreeCoords(tree,child,orig,x,y+1);
		if (!trunc) {
			tree->fromlist[tree->nlist] = curr;
			tree->tolist[tree->nlist] = child;
			if (child == orig[curr].child)
				tree->arrowflags[tree->nlist] = XO_TREE_CHILD_ARROW1;
			else
				/* A hack for the remaining children. We need to have
				** the original child as a third coord to make the
				** tree branch, so we use the negative to indicate that
				** it is this kind of arrow and the value to specify the
				** orig child */
				tree->arrowflags[tree->nlist] = -orig[curr].child - 1 ;
			tree->nlist++;
		}
	}
	/* do not add to x if this leaf is hidden */
	if ((!trunc) && orig[curr].child == -1)
		(*x)++;
	/* but do add if it is the parent of the hidden region, but
	** is itself visible */
	else if ((trunc & XO_TREE_TRUNCATE) && !(trunc & XO_TREE_TRUNCATED))
		(*x)++;
}

static int ReallocArrows(tree,nlist)
	struct xtree_type	*tree;
	int		nlist;
{
	tree->nlist = nlist;
 		if(tree->fromlist == NULL)
			tree->fromlist = (int*) calloc(tree->max_nlist, sizeof(int));
		
 		if(tree->tolist == NULL)
			tree->tolist = (int*) calloc(tree->max_nlist, sizeof(int));
		if(tree->arrowflags == NULL)
			tree->arrowflags = (int *)calloc(tree->max_nlist,sizeof(int));
	if (nlist >= tree->max_nlist) {
		int ap = nlist + 100;

		tree->fromlist =
		(int *)realloc(tree->fromlist,ap*sizeof(int));
		xomemzero((char*) &tree->fromlist[tree->max_nlist], (ap-tree->max_nlist)*sizeof(int));

		tree->tolist =
		(int *)realloc(tree->tolist,ap*sizeof(int));
		xomemzero((char*) &tree->tolist[tree->max_nlist], (ap-tree->max_nlist)*sizeof(int));

		tree->arrowflags = 
		(int *)realloc(tree->arrowflags,ap*sizeof(int));
		xomemzero((char*) &tree->arrowflags[tree->max_nlist], (ap-tree->max_nlist)*sizeof(int));

		tree->max_nlist = ap;
		return(tree->max_nlist);
	}
	return(0);
}

static void CustomLayout(tree)
	struct xtree_type	*tree;
{
	/* set up the from-to lists */
	ReallocArrows(tree,0);
	/* Do nothing here */
}

static int GridLayout(tree)
	struct xtree_type	*tree;
{
	int		nelms;
	int		side;
	int		i,j,k;
	struct xtree_leaf_type *leaf;
	if (!tree->coord_elist) return(0);
	if ((nelms = tree->coord_elist->nelements) <= 0) return 0;

	/* set up the from-to lists */
	ReallocArrows(tree,0);

	/* Depending on the orientation, decide how to do the tree */
	/* The options are: up: a grid with the long axis vertical 
	** left: grid with the long axis horizontal
	** down: single column
	** right: single row
	*/
	switch(tree->orientation[0]) {
		case 'd' : /* single column */
			for(i = 0, i=0 ; i < nelms; i++) {
				leaf = tree->ptlist + i;
				leaf->xpts = 0;
				leaf->ypts = i;
				leaf->zpts = 0;
			}
			break;
		case 'r' : /* single row */
			for(i = 0, i=0 ; i < nelms; i++) {
				leaf = tree->ptlist + i;
				leaf->xpts = i;
				leaf->ypts = 0;
				leaf->zpts = 0;
			}
			break;
		case 'l' : /* grid with horizontal long axis */
			side = 1 + (int)sqrt((double)(nelms - 0.5));
			for(i = 0, k=0 ; k < nelms; i++) {
				for(j = 0; j < side && k < nelms ; j++,k++) {
					leaf = tree->ptlist + k;
					leaf->xpts = j;
					leaf->ypts = i;
					leaf->zpts = 0;
				}
			}
			break;
		case 'u' : /* grid with vertical long axis */
			side = 1 + (int)sqrt((double)(nelms - 0.5));
			for(i = 0, k=0 ; k < nelms; i++) {
				for(j = 0; j < side && k < nelms ; j++,k++) {
					leaf = tree->ptlist + k;
					leaf->xpts = i;
					leaf->ypts = j;
					leaf->zpts = 0;
				}
			}
			break;
		case 'L' : /* grid with horizontal long axis, staggered */
			side = 1 + (int)sqrt((double)(nelms - 0.5));
			for(i = 0, k=0 ; k < nelms; i++) {
				for(j = 0; j < side && k < nelms ; j++,k++) {
					leaf = tree->ptlist + k;
					leaf->xpts = j;
					leaf->ypts = i + ((float)(j % 2))/2.0 - 0.25;
					leaf->zpts = 0;
				}
			}
			break;
		case 'R' :
		case 's' : /* single row, with pts staggered so text
					** doesnt overlap */
			for(i = 0, i=0 ; i < nelms; i++) {
				leaf = tree->ptlist + i;
				leaf->xpts = i;
				leaf->ypts = ((float)(i % 2))/2.0 - 0.25;
				leaf->zpts = 0;
			}
			break;
		case 'm' : /* multiple rows with n entries each */
			sscanf (tree->orientation +1, "%d", &side);
			if (side > 0) {
				for(i = 0, k=0 ; k < nelms; i++) {
					for(j = 0; j < side && k < nelms ; j++,k++) {
						leaf = tree->ptlist + k;
						leaf->xpts = j;
						leaf->ypts = i;
						leaf->zpts = 0;
					}
				}
			}
			break;
		case 'M' : /* multiple rows with n entries each, staggered */
			sscanf (tree->orientation +1, "%d", &side);
			if (side > 0) {
				for(i = 0, k=0 ; k < nelms; i++) {
					for(j = 0; j < side && k < nelms ; j++,k++) {
						leaf = tree->ptlist + k;
						leaf->xpts = j;
						leaf->ypts =
							(float)i + ((float)(j % 2))/2.0 -0.25;
						leaf->zpts = 0;
					}
				}
			}
			break;
	}
	return 0;
}

static void FindMsgArrows(tree)
	struct xtree_type	*tree;
{
	int i;
	int ncoords;
	int msgtypeno;
	char	*msgtype;
	HASH	*hash_create();
	ENTRY	*xo_hash_find(), *xo_hash_enter();
	ENTRY	entry;
	ENTRY	*tempentry;
	HASH	*hash_table; /* for the coord_elist */
	HASH	*msgsrc_hash_table; /* for the msgsrc_elist */
	GenesisObject	*lastobject = NULL;
	MsgList		*msgspec,*GetMsgListByName();
	int		mtype = 0;
	MsgIn	*msg;
#ifdef VERSION2
	int		msgcount;
#endif
	/* note that we will be iterating over this list, so we do not 
	** need a hash table for it */
	ElementList	*msgdest_elist;

	if (!tree->coord_elist) return;
	if ((ncoords = tree->coord_elist->nelements) <= 0) return;
	if (tree->nmsgtypes <= 0 || tree->msgarrows == NULL) return;

/* Set up hash table for finding element index by pointer */
/* It would be a lot easier to have a temporary field on the element */
/* In this table we drop out elements which are truncated or invisible*/

	hash_table = hash_create(ncoords * 2);
	for(i = 0; i < ncoords; i++) {
		if (!(tree->ptlist[i].shapeflags &
			(XO_TREE_TRUNCATED | XO_TREE_INVISIBLE))) {
			entry.data = (char *)i;
			entry.key = (char *)tree->coord_elist->element[i];
			xo_hash_enter(&entry,hash_table);
		}
	}

	for(msgtypeno = 0; msgtypeno < tree->nmsgtypes; msgtypeno++) {
		int fliparrow = tree->msgarrows[msgtypeno].fliparrow;
		/* skip this arrow if the msgcolor is 'none' */
		if (strcmp(tree->msgarrows[msgtypeno].msgfgstr,"none") == 0)
			continue;
		/* Setting up the msg type criteria */
		msgtype = tree->msgarrows[msgtypeno].msgtype; /* NULL if all */
		if (strcmp(msgtype,"all") == 0) /* speed up compares a bit */
			msgtype = NULL;

		/* setting up the msgsrc list */
		if (!(msgsrc_hash_table =
			FindMsgSrc(tree,msgtypeno,hash_table)))
			continue;

	/* Setting up the msgdest list. This also checks the msgdesttype
	** criteria, if any, and looks for truncation /invisible elements */
		if (!(msgdest_elist =
			FindMsgDest(tree,msgtypeno,hash_table)))
			continue;
		
	/* scanning the msgdest list for all msgs that meet criteria */
		lastobject = NULL;
		for(i = 0 ; i < msgdest_elist->nelements; i++) {
			ADDR elmindex;
			Element *elm = msgdest_elist->element[i];
			if (!elm) continue;

			/* find elm index */
			tempentry = xo_hash_find(elm,hash_table);
			if (!tempentry) continue;
			elmindex = (ADDR)tempentry->data;

			/* identify msg type */
			/* Skip extra lookups if object type is same as last */
			if (elm->object != lastobject) {
				lastobject = elm->object;
				mtype = -1;
				if (msgtype) {
					msgspec = GetMsgListByName(lastobject,msgtype);
					if (msgspec) {
						mtype = msgspec->type;
					}
				}
			}
/* Doing the msgins */
#ifdef VERSION2
			msg = elm->msgin;
			for(msgcount = 0; msgcount < elm->nmsgin; msgcount++) {
				if (mtype == -1 || msg->type == mtype) {
					tempentry =
						xo_hash_find(msg->src,msgsrc_hash_table);
					if (tempentry) {
						if (fliparrow) {
							tree->fromlist[tree->nlist] = elmindex;
							tree->tolist[tree->nlist] =
								(int)tempentry->data;
						} else {
							tree->tolist[tree->nlist] = elmindex;
							tree->fromlist[tree->nlist] =
								(int)tempentry->data;
						}
						tree->arrowflags[tree->nlist] =
							XO_TREE_MSGARROW_OFFSET + 
							tree->msgarrows[msgtypeno].msgfg;
						tree->nlist++;
						ReallocArrows(tree,tree->nlist);
					}
				}
				msg = MSGINNEXT(msg);
			}
#else
			for(msg = elm->msg_in; msg; msg=msg->next) {
				if (mtype == -1 || msg->type == mtype) {
					tempentry =
						xo_hash_find(msg->src,msgsrc_hash_table);
					if (tempentry) {
						if (fliparrow) {
							tree->fromlist[tree->nlist] = elmindex;
							tree->tolist[tree->nlist] =
								(int)tempentry->data;
						} else {
							tree->tolist[tree->nlist] = elmindex;
							tree->fromlist[tree->nlist] =
								(int)tempentry->data;
						}
						tree->arrowflags[tree->nlist] =
							XO_TREE_MSGARROW_OFFSET + 
							tree->msgarrows[msgtypeno].msgfg;
						tree->nlist++;
						ReallocArrows(tree,tree->nlist);
					}
				}
			}
#endif
		}
	/* cleaning up */
		if (msgdest_elist != tree->coord_elist) 
			FreeElementList(msgdest_elist);
		if (hash_table) {
			if(msgsrc_hash_table && msgsrc_hash_table != hash_table) {
				if (msgsrc_hash_table->size > 0)
					free(msgsrc_hash_table->entry);
				free(msgsrc_hash_table);
			}
		}
	}
	/* cleaning up */
	if (hash_table) {
		if (hash_table->size > 0)
			free(hash_table->entry);
		free(hash_table);
	}
}

/* we need to set msgtype[i] to either the msgtype name or to "".
** if "" then any msg will do, represented by mtype = -1 */
/* We need a utility function (probably in sim) for matching 
** dest object types specially if they have been subclassed */
/* At the specification time msgfg has to be converted to 
** pixel values */

static HASH *FindMsgSrc(tree,msgtypeno,hash_table)
	struct xtree_type	*tree;
	int		msgtypeno;
	HASH	*hash_table;
{
	HASH	*msgsrc_hash_table = NULL;
	HASH	*hash_create();
	ENTRY	*xo_hash_find(), *xo_hash_enter();
	ENTRY	entry;
	ENTRY	*tempentry;
	int		i;
	int		ncoords = tree->coord_elist->nelements;

/* Setting up the msgsrc list */
	/* Checking for the special case of msgsrc == all */
	if (strcmp(tree->msgarrows[msgtypeno].msgsrc,"all") == 0) {
		msgsrc_hash_table = hash_table;
	} else if (strcmp(tree->msgarrows[msgtypeno].msgsrc,"select") == 0){
	/* checking for the special case where the selected elements
	** in the widget are to be used as the msgsrc */
		int nselect = 0;
		for(i = 0; i < ncoords; i++) {
			if (tree->ptlist[i].shapeindex & XO_TREE_SELECTED)
				nselect++;
		}
		if (nselect == 0) return(NULL);
		msgsrc_hash_table = hash_create(nselect * 2);
		for(i = 0; i < ncoords; i++) {
			if (tree->ptlist[i].shapeindex & XO_TREE_SELECTED) {
				entry.data = (char *)i;
				entry.key = (char *)tree->coord_elist->element[i];
				xo_hash_enter(&entry,msgsrc_hash_table);
			}
		}
	} else {
	/* Setting up the general case with a msgsrc path */
		ElementList	*msgsrc_elist =
			WildcardGetElement(tree->msgarrows[msgtypeno].msgsrc,0);
		int	nsrcpath = 0;
		int	nselect = 0;
		Element	*elm;

		if (!msgsrc_elist) return(NULL);
		if ((nsrcpath = msgsrc_elist->nelements) == 0) return(NULL);

		/* Checking for elements which are also present on the
		** coord_elist path */
		for(i = 0; i < nsrcpath; i++) {
			elm = msgsrc_elist->element[i];
			if ((tempentry = xo_hash_find(elm,hash_table)) == NULL){
				msgsrc_elist->element[i] = NULL;
			} else {
				nselect++;
			}
		}
		if (nselect > 0) {
			msgsrc_hash_table = hash_create(nselect * 2);
			for(i = 0; i < nsrcpath; i++) {
				elm = msgsrc_elist->element[i];
				if (elm) {
					if ((tempentry = xo_hash_find(elm,hash_table))
						!= NULL){
						entry.data = tempentry->data;
						entry.key = (char *)elm;
						xo_hash_enter(&entry,msgsrc_hash_table);
					}
				}
			}
		}
		FreeElementList(msgsrc_elist);
	}
	return(msgsrc_hash_table);
}


static ElementList *FindMsgDest(tree,msgtypeno,hash_table)
	struct xtree_type	*tree;
	int		msgtypeno;
	HASH	*hash_table;
{
	ElementList *msgdest_elist, *CreateElementList();
	char	*msgdesttype = tree->msgarrows[msgtypeno].msgdesttype;
	int		i;
	int		ncoords = tree->coord_elist->nelements;
	ENTRY	*xo_hash_find();
	ENTRY	*tempentry;

/* First, do a scan to find the elements on msgdest_elist */

	if (strcmp(tree->msgarrows[msgtypeno].msgdest,"all") == 0) {
		msgdest_elist = tree->coord_elist;
	} else if (strcmp(tree->msgarrows[msgtypeno].msgdest,"select") ==0){
	/* checking for the special case where the selected elements
	** in the widget are to be used as the msgdest */
		int nselect = 0;
		/* count the number of selected elements */
		for(i = 0; i < ncoords; i++) {
			if (tree->ptlist[i].shapeindex & XO_TREE_SELECTED) {
				nselect++;
			}
		}
		if (nselect == 0) return(NULL);
		msgdest_elist = CreateElementList(nselect);
		nselect=0;
		/* Fill up the element list */
		for(i = 0; i < ncoords; i++) {
			if (tree->ptlist[i].shapeindex & XO_TREE_SELECTED) {
				msgdest_elist->element[nselect] =
					tree->coord_elist->element[i];
				nselect++;
			}
		}
	} else {
	/* Setting up the general case with a msgdest path */
		int	nsrcpath = 0;
		Element	*elm;

		msgdest_elist =
			WildcardGetElement(tree->msgarrows[msgtypeno].msgdest,0);

		if (!msgdest_elist) return(NULL);
		if ((nsrcpath = msgdest_elist->nelements) == 0) return(NULL);

		/* Getting rid of elements which are not present on the
		** coord_elist path */
		for(i = 0; i < nsrcpath; i++) {
			elm = msgdest_elist->element[i];
			if ((tempentry = xo_hash_find(elm,hash_table)) == NULL) {
				msgdest_elist->element[i] = NULL;
			} 
		}
	}

/* Then, do a scan to find the elements which match msgdesttype
** and are on the hash table */
	if (strcmp(msgdesttype,"all") == 0) {
		return(msgdest_elist);
	} else {
		ElementList *temp_elist;
		int nselect = 0;
		/* count the number of elements of the correct type */
		for(i = 0; i < msgdest_elist->nelements; i++) {
			Element *elm = msgdest_elist->element[i];
			if (!elm) continue;
			if ((tempentry = xo_hash_find(elm,hash_table)) == NULL)
				continue;
			if (ElementIsA(elm,msgdesttype))
				nselect++;
		}
		if (nselect == 0) return(NULL);
		if (nselect == msgdest_elist->nelements) return(msgdest_elist);
		temp_elist = msgdest_elist;
		msgdest_elist = CreateElementList(nselect);
		nselect=0;
		/* Fill up the element list */
		for(i = 0; i < temp_elist->nelements; i++) {
			Element *elm = temp_elist->element[i];
			if (!elm) continue;
			/* a bit messy, but it will do for now */
			if ((tempentry = xo_hash_find(elm,hash_table)) == NULL)
				continue;
			if (ElementIsA(elm,msgdesttype)){
				msgdest_elist->element[nselect] =temp_elist->element[i];
				nselect++;
			}
		}
		msgdest_elist->nelements = nselect;
		/* Get rid of old elist */
		FreeElementList(temp_elist);
	}

	return(msgdest_elist);
}

static void AddMsgArrow(tree,msgsrc,msgdest,msgtype,msgdesttype,fg,index,
	fliparrow,
	s0,s1,s2)
	struct xtree_type *tree;
	char	*msgsrc,*msgdest,*msgtype,*msgdesttype,*fg;
	int		index;
	char	*fliparrow;
	char	*s0,*s1,*s2;
{
	if (index > tree->nmsgtypes || index < 0) {
		printf("error: in AddMsgArrow %s: index %d > nmsgtypes %d\n",
			tree->name,index,tree->nmsgtypes);
		return;
	}

	/* Deal with allocation of the msgarrows struct */
	if (index >= tree->max_nmsgtypes) {
		/* fix for moronic UNIX systems which do not adhere to the
		** definition of realloc for NULL pointers */
		if (tree->msgarrows == NULL)
			tree->msgarrows = (struct xtree_msgarrow_type *)
				calloc(10,sizeof(struct xtree_msgarrow_type));
		else 
			tree->msgarrows = (struct xtree_msgarrow_type *)
				realloc((char *)tree->msgarrows,
					(index + 10) * sizeof(struct xtree_msgarrow_type));
		tree->max_nmsgtypes = index + 10;
	}
	if (index == tree->nmsgtypes)
		tree->nmsgtypes += 1;


	if (msgsrc == NULL || (int)strlen(msgsrc) < 1) /* assign default */
		msgsrc = "select";
	if (msgdest == NULL || (int)strlen(msgdest) < 1) /* use default */
		msgdest = "all";
	if (msgtype == NULL || (int)strlen(msgtype) < 1) /* use default */
		msgtype = "all";
	if (msgdesttype == NULL || (int)strlen(msgdesttype) < 1)
		/* assign default */
		msgdesttype = "all";
	if (fg == NULL || (int)strlen(fg) < 1) /* assign default */
		fg = "blue";
	if (fliparrow == NULL || (int)strlen(fliparrow) < 1)
		tree->msgarrows[index].fliparrow = 0;
	else
		tree->msgarrows[index].fliparrow = atoi(fliparrow);

	tree->msgarrows[index].msgsrc = CopyString(msgsrc);
	tree->msgarrows[index].msgdest = CopyString(msgdest);
	tree->msgarrows[index].msgtype = CopyString(msgtype);
	tree->msgarrows[index].msgdesttype = CopyString(msgdesttype);
	tree->msgarrows[index].msgfgstr = CopyString(fg);
	if (strcmp(fg,"none") == 0)
		tree->msgarrows[index].msgfg =
			XoGetPixelFromString(tree->widget,"blue");
	else
		tree->msgarrows[index].msgfg =
			XoGetPixelFromString(tree->widget,fg);
	if (s0 && (int)strlen(s0) > 0)
		tree->msgarrows[index].script = CopyString(s0);
	else
		tree->msgarrows[index].script = NULL;
	if (s1 && (int)strlen(s1) > 0)
		tree->msgarrows[index].script_add = CopyString(s1);
	else
		tree->msgarrows[index].script_add = NULL;
	if (s2 && (int)strlen(s1) > 0)
		tree->msgarrows[index].script_drop = CopyString(s2);
	else
		tree->msgarrows[index].script_drop = NULL;
}

static int ScanShapes(tree)
	struct xtree_type *tree;
{
	int i;
	Element	*elms[XO_TREE_MAXSHAPES];
	struct	xshape_type	*xshape;

  /* This first step is inherited from the xvar. It scans the list of
	of children to find the applicable shapes, and fills in
	the lists */
	for (i = 0; i < XO_TREE_MAXSHAPES; i++) {
		elms[i] = GetChildElement(tree,"shape",i);
		if (!elms[i]) break;
	}
	if (i > 0) {
		if (i != tree->nshapes) {
			if (tree->nshapes > 0)
				free(tree->shapes);
			tree->shapes = (char **)calloc(i,sizeof(char *));
			tree->shapevals = (char **)calloc(i,sizeof(char *));
			tree->nshapes = i;
		}
		for(i = 0; i < tree->nshapes; i++) {
			xshape = (struct xshape_type *)(elms[i]);
			tree->shapes[i] = xshape->widget;
			tree->shapevals[i] = xshape->value;
		}
	}
	return(tree->nshapes);
}

/* This func redoes the coord_elists and handles allocation */
static int BuildTreeList(tree,ncoords)
	struct xtree_type *tree;
	int	ncoords;
{
/* Here we fill in the values of the xtree */
	int i;
	int old_ncoords = tree->ncoords;
	HASH	*coord_hash,*hash_create();
	ENTRY	*xo_hash_find(), *xo_hash_enter();
	ENTRY	entry;
	struct xtree_leaf_type *new_ptlist;


	if (ncoords == 0) { /* calculate ncoords from elist */
		/* Get rid of the old elist and hash table */
		if (tree->coord_elist && tree->ncoords > 0)
			FreeElementList(tree->coord_elist);
			tree->coord_elist = NULL;
		if ((coord_hash = (HASH *)tree->coord_hash)) {
			if (coord_hash->size > 0)
				free(coord_hash->entry);
			free(tree->coord_hash);
			tree->coord_hash = NULL;
		}
	/* check the paths */
		if (tree->path && strcmp(tree->path,"none") != 0 &&
			(int)strlen(tree->path) > 0) {
			if (strcmp(tree->treemode,"obj_grid") == 0 ||
				strcmp(tree->treemode,"obj_custom") == 0) {
		/* set up an object list from the path */
				int i;
				int nobj = 0;
				extern HASH *object_hash_table;
				char	*name;
				GenesisObject	*obj;

				if (strcmp(tree->path,"all") == 0) {
					/* count the objects */
					for(i = 0; i < object_hash_table->size; i++)
					        if ((name = object_hash_table->entry[i].key))
							nobj++;
					tree->coord_elist = (ElementList *)
						calloc(1,sizeof(ElementList));
					tree->coord_elist->nelements = nobj;
					tree->coord_elist->element = (Element **)
						calloc(nobj,sizeof(Element *));
					nobj = 0;
					for(i = 0; i < object_hash_table->size; i++) {
						if ((name = object_hash_table->entry[i].key)) {
							obj = (GenesisObject *)
								object_hash_table->entry[i].data;
							tree->coord_elist->element[nobj] =
								obj->defaults;
							nobj++;
						}
					}
					tree->ncoords = nobj;
				} else {
					int argc;
					char **argv;

					StringToArgList(tree->path,&argc,&argv,0);
					for(i = 0; i < argc; i++) {
					        if ((obj=GetObject(argv[i])))
							nobj++;
					}
					tree->coord_elist = (ElementList *)
						calloc(1,sizeof(ElementList));
					tree->coord_elist->nelements = nobj;
					tree->coord_elist->size = nobj;
					tree->coord_elist->element = (Element **)
						calloc(nobj,sizeof(Element *));
					nobj = 0;
					for(i = 0; i < argc; i++) {
						if ((obj=GetObject(argv[i]))) {
							tree->coord_elist->element[nobj] =
								obj->defaults;
							nobj++;
						}
					}
					tree->ncoords = nobj;
					/* Clean up */
					for(i = 0 ; i < argc; i++)
						if (argv[i])
							free(argv[i]);
					free(argv);
				}
			} else {
		/* Set up the elists */
				tree->coord_elist = WildcardGetElement(tree->path,0);
				if (tree->coord_elist->nelements==0) {
				/*
					printf("Warning: No elements found on path %s\n", tree->path);
					*/
				if (tree->path[0] != '/')
					printf("Note: Use absolute path names for the path field\n");
					tree->ncoords = 0;
				} else {
					tree->ncoords = tree->coord_elist->nelements;
				}
			}
		}
/* Set up the coord_hash hash-table from the elist */
		if (tree->ncoords > 0) {
			coord_hash = hash_create(tree->ncoords * 2);
			tree->coord_hash = (char *)coord_hash;
			for(i = 0; i < tree->ncoords; i++) {
				entry.data = (char *)i;
				entry.key = (char *)tree->coord_elist->element[i];
				xo_hash_enter(&entry,coord_hash);

			}
		}
	} else if (ncoords > 0) {
		tree->ncoords = ncoords;
	} else {
		return(0);
	}
	if (tree->ncoords == 0) {
		/* Free everything */
		free(tree->ptlist);
		free(tree->_ptnames);
		tree->_ptnames = NULL;
		tree->ptlist = NULL;
		tree->_ptnamesize = 0;
		return(0);
	}

	

/* Set up the ptlist in the same order as the elist,
** keeping all old values associated with elements already on the
** list */
	new_ptlist = (struct xtree_leaf_type *)
		calloc(tree->ncoords,sizeof(struct xtree_leaf_type));
/* Set up the new _ptnames array. If the old one was big enough,
** leave it in place */
	if (tree->ncoords * 8 > tree->_ptnamesize) {
		if (tree->_ptnamesize > 0 && tree->_ptnames)
			free(tree->_ptnames);
		tree->_currptname = tree->_ptnames = (char *)
			calloc(tree->ncoords * 8,sizeof(char));
		tree->_ptnamesize = tree->ncoords * 8;
	}

	/* Scan the old ptlist to get rid of elements not on the elist,
	** and put the existing old elements in their new locations */
	for (i = 0; i < old_ncoords; i++) {
		struct xtree_leaf_type *pl = tree->ptlist + i;
		int new_location =
			FindElistIndex(tree,(Element *)tree->ptlist[i].elm);
		if (new_location != XO_ELM_NOT_FOUND) {
			new_ptlist[new_location] = *pl;
		}
	}

	/* Get rid of the old ptlist */
	if (tree->ptlist && old_ncoords > 0)
		free(tree->ptlist);
	tree->ptlist = new_ptlist;

	/* scan the new ptlist to fill in the remaining elm ids */
	for (i = 0; i < tree->ncoords; i++) {
		tree->ptlist[i].elm = (char *)tree->coord_elist->element[i];
	}

	return(tree->ncoords);
}

static int BuildTreeCoords(tree)
	struct xtree_type *tree;
{
	int i;
	Element *elm;
	char	*reqval;
/*
	long	defaultfg = XoGetPixelFromString(tree->widget,tree->fg);
*/

	/* Set up the shapeindex table. This identifies each coord
		with a specific shape */
		for(i = 0; i < tree->ncoords; i++) {
			elm = tree->coord_elist->element[i];

			/* check if the element has an extfield by the name of
			** xtree_shape_req */
			/*reqval = FieldValue(elm,elm->object,"xtree_shape_req"); */
			reqval = GetExtField(elm,"xtree_shape_req");
			if (!reqval || strlen(reqval) < 1)
				reqval = elm->object->name;

			tree->ptlist[i].shapeindex =
				FindShapeIndex(tree,reqval);

	
			/* Check if the element has a field by the name of
			** xtree_fg_req */
			reqval = GetExtField(elm,"xtree_fg_req");
			tree->ptlist[i].fg = (reqval && *reqval != '\0') ?
					XoGetPixelFromString(tree->widget,reqval) :
					-1;
	
			/* Check if the element has a field by the name of
			** xtree_textfg_req */
			reqval = GetExtField(elm,"xtree_textfg_req");
			tree->ptlist[i].textfg = (reqval && *reqval != '\0') ?
				XoGetPixelFromString(tree->widget,reqval): 
				-1;
		}

	/* Set up the coords. */
		/* reset the number of list elements */
		tree->nlist = 0;
		if (strcmp(tree->treemode,"tree") == 0 ||
			strcmp(tree->treemode,"treenotrunc") == 0) {
			TreeLayout(tree);
		} else if (strcmp(tree->treemode,"custom") == 0) {
			CustomLayout(tree);
		} else if (strcmp(tree->treemode,"grid") == 0) {
			GridLayout(tree);
		} else if (strcmp(tree->treemode,"geometry") == 0) {
			ReallocArrows(tree,0);
			for(i = 0; i < tree->ncoords; i++) {
				tree->ptlist[i].xpts = tree->coord_elist->element[i]->x;
				tree->ptlist[i].ypts = tree->coord_elist->element[i]->y;
				tree->ptlist[i].zpts = tree->coord_elist->element[i]->z;
			}
		} else if (strcmp(tree->treemode,"obj_grid") == 0) {
			GridLayout(tree);
		} else if (strcmp(tree->treemode,"obj_custom") == 0) {
			CustomLayout(tree);
		}
	return(0);
}

static int BuildTreeNames(tree)
	struct xtree_type *tree;
{
	int i;
	char elmname[80];
	/* set up the names of the elms */
		if (strcmp(tree->namemode,"auto") == 0) { /* Do all names */
			Element *elm;
			if (strcmp(tree->treemode,"obj_grid") == 0 ||
				strcmp(tree->treemode,"obj_custom") == 0) {
				for(i = 0; i < tree->ncoords; i++) {
					elm = tree->coord_elist->element[i];
					AddPtname(tree,i,elm->object->name);
				}
			} else {
				for(i = 0; i < tree->ncoords; i++) {
					elm = tree->coord_elist->element[i];
					if (elm->index > 0)
						sprintf(elmname,"%s[%d]",elm->name,elm->index);
					else
						sprintf(elmname,"%s",elm->name);
					AddPtname(tree,i,elmname);
				}
			}
		} else { /* Do only the names of specified objects */
			Element *elm;
			if (strcmp(tree->treemode,"obj_grid") == 0 ||
				strcmp(tree->treemode,"obj_custom") == 0) {
				for(i = 0; i < tree->ncoords; i++) {
					elm = tree->coord_elist->element[i];
					if (strstr(tree->namemode,elm->object->name))
						AddPtname(tree,i,elm->object->name);
					else
						AddPtname(tree,i,"");
				}
			} else {
				for(i = 0; i < tree->ncoords; i++) {
					elm = tree->coord_elist->element[i];
					if (strstr(tree->namemode,elm->object->name)) {
						if (elm->index > 0)
							sprintf(elmname,"%s[%d]",
								elm->name,elm->index);
						else
							sprintf(elmname,"%s",elm->name);
						AddPtname(tree,i,elmname);
					} else {
						AddPtname(tree,i,"");
					}
				}
			}
		}
	return(0);
}

/* The idea here is to avoid wasting a lot of room doing mallocs */
static int AddPtname(tree,i,name)
	struct xtree_type *tree;
	int i;
	char *name;
{
	int len = strlen(name) + 1;
	if (tree->_currptname + len >= tree->_ptnames + tree->_ptnamesize) {
		int namesize = tree->_currptname - tree->_ptnames;
		int j;
		char *oldptnames = tree->_ptnames;
		tree->_ptnames = (char *)realloc(tree->_ptnames,
			(200 + tree->_ptnamesize) * sizeof(char));
		if (!tree->_ptnames)
			return(0);
		tree->_currptname = tree->_ptnames +  namesize;
		tree->_ptnamesize += 200;
		/* Relocate all the pointers that were based on the previous
		** ptnames array */
		if (tree->_ptnames != oldptnames) {
			int nameshift = tree->_ptnames - oldptnames;
			for(j = 0; j < i; j++)
				tree->ptlist[j].ptname += nameshift;
		}
	}
	tree->ptlist[i].ptname = tree->_currptname;
	strcpy(tree->ptlist[i].ptname,name);
	tree->_currptname += len;
	return(1);
}



static int OnTreePath(tree,msgno,is_dest,elmno)
	struct xtree_type *tree;
	int msgno;
	int is_dest;
	int	elmno;
{
	struct xtree_msgarrow_type *arrow = &(tree->msgarrows[msgno]);
	ElementList *elist, *WildcardGetElement();
	Element *elm = tree->coord_elist->element[elmno];
	int do_clean_elist = 0;
	int	do_use_select = 0;
	int i;

	if (is_dest) {
		if (!arrow->msgdest)
			return(0);

/* This may break in VERSION2 */
		if (strcmp(arrow->msgdesttype,"all") != 0 &&
			 !(ElementIsA(elm,arrow->msgdesttype)))
			return(0);

		if (strcmp(arrow->msgdest,"all") == 0) {
			elist = tree->coord_elist;
		} else if (strcmp(arrow->msgdest,"select") == 0) {
			elist = tree->coord_elist;
			do_use_select = 1;
		} else  {
			elist = WildcardGetElement(arrow->msgdest,0);
			do_clean_elist = 1;
		}
	} else {
		if (!arrow->msgsrc)
			return(0);
		if (strcmp(arrow->msgsrc,"all") == 0) {
			elist = tree->coord_elist;
		} else if (strcmp(arrow->msgsrc,"select") == 0) {
			elist = tree->coord_elist;
			do_use_select = 1;
		} else  {
			elist = WildcardGetElement(arrow->msgsrc,0);
			do_clean_elist = 1;
		}
	}

	for(i = 0; i < elist->nelements; i++) {
		if (elm == elist->element[i]) {
			if (!do_use_select || 
				tree->ptlist[elmno].shapeindex == XO_TREE_SELECTED) {
				if (do_clean_elist) {
					FreeElementList(elist);
				}
				return(1);
			}
			/* to speed things up: each elm occurs once on an elist */
			return(0);
		}
	}
	return(0);
}

static int IsAncestor(elm,child)
	Element	*elm;
	Element	*child;
{
	for(child = child->parent; child; child = child->parent)
		if (child == elm)
			return(1);
	return(0);
}

static int IsArrowDuplicated(index,tree)
	int index;
	struct xtree_type *tree;
{
	int i;

	if (index > -XO_TREE_MAXARROWS)
	/* add this arrow without bothering to check */
		return(0);
	for(i = 0; i < tree->nmsgtypes; i++) {
		if (strcmp(tree->msgarrows[i].msgsrc,optargv[1]) == 0 &&
			strcmp(tree->msgarrows[i].msgdest,optargv[2]) == 0 &&
			strcmp(tree->msgarrows[i].msgtype,optargv[3]) == 0 &&
			strcmp(tree->msgarrows[i].msgdesttype,optargv[4]) == 0)
			return(1);
			/* We do not want to check for the functions in case
			** there are funny spacing differences */
			/* We do not want to check for the colors since these
			** may lead to unexpected duplication of entries
			strcmp(tree->msgarrows[i].msgfgstr,optargv[5]) == 0 
			*/
	}
	return(0);
}

static int TruncateKids(tree,elmname,mode)
	struct xtree_type *tree;
	char	*elmname;
	int		mode;
{
	Element *elm = GetElement(elmname);
	int		elmno;

	if (!elm) {
		Error();
		printf("truncating tree: Could not find element %s\n",
			elmname);
		return(0);
	}
	elmno = FindElistIndex(tree,elm);
	if (elmno == XO_ELM_NOT_FOUND) {
		Error();
		printf("truncating tree: element %s is not on tree\n",
			elmname);
		return(0);
	}
	if (mode == 0) { /* set truncation off */
		tree->ptlist[elmno].shapeflags &= ~XO_TREE_TRUNCATE;
	} else if (mode == 1) { /* Turn truncation on */
		tree->ptlist[elmno].shapeflags |= XO_TREE_TRUNCATE;
	} else {	/* Flip truncation */
		tree->ptlist[elmno].shapeflags ^= XO_TREE_TRUNCATE;
		if (tree->ptlist[elmno].shapeflags & XO_TREE_TRUNCATE)
			mode = 1;
		else
			mode = 0;
	}
	RecurseTruncate(tree,elm,mode);
	return(1);
}

static void RecurseTruncate(tree,elm,mode)
	struct xtree_type *tree;
	Element *elm;
	int mode;
{
	Element *child;
	int		elmno;

	for (child = elm->child; child; child = child->next) {
		if ((elmno = FindElistIndex(tree,child)) != XO_ELM_NOT_FOUND)  {
			if (mode)
				tree->ptlist[elmno].shapeflags |= XO_TREE_TRUNCATED;
			else
				tree->ptlist[elmno].shapeflags &= ~XO_TREE_TRUNCATED;
		}
		RecurseTruncate(tree,child,mode);
	}
}

static int FindElistIndex(tree,elm)
	struct xtree_type *tree;
	Element *elm;
{	
	ENTRY	*tempentry;
	ENTRY	*xo_hash_find();

	tempentry = xo_hash_find((char *)elm,tree->coord_hash);
	if (tempentry) {
		return((int)tempentry->data);
	}
	return(XO_ELM_NOT_FOUND);
}

#undef XO_TREE_MAXSHAPES /* 50 */
#undef XO_TREE_MAXARROWS /* 1000 */
#undef XO_ELM_NOT_FOUND /* -1 */
