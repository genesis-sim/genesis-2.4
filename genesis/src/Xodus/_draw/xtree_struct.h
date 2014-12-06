/* $Id: xtree_struct.h,v 1.1 2005/06/17 21:23:51 svitak Exp $ */
/*
 * $Log: xtree_struct.h,v $
 * Revision 1.1  2005/06/17 21:23:51  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.8  2000/06/12 04:28:21  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.7  1994/06/13 23:03:45  bhalla
 * Added orientation field, got rid of treefile
 *
 * Revision 1.6  1994/03/30  17:23:00  bhalla
 * Added an object field for the coord_hash table
 *
 * Revision 1.5  1994/03/29  17:18:45  bhalla
 * converted all individual arrays of item properties
 * such as coords, shapeindex, shapeflags etc into a single struct.
 *
 * Revision 1.4  1994/03/22  15:44:01  bhalla
 * Added MOVECUSTOM action definition
 * Added fliparrow field
 *
 * Revision 1.3  1994/02/02  20:04:53  bhalla
 * Added code to handle drops between components of xtree. This also
 * involves a lot of code from CoreDraw.c and xo_callb.c
 * Extended the MsgArrow operations to allow functions to be called
 * specific to drag operations between identified msg types
 * */
#ifndef _xtree_struct_h
#define _xtree_struct_h
#include "../_widg/widg_defs.h"
#include "../Draw/VarShape.h"
#include "../Draw/TreeLeaf.h"

#define ADDMSGARROW 213
#define SHOWMSGARROW 214
#define MOVECUSTOM 215
#define TRUNCATE 216

struct xtree_msgarrow_type {
  char		*msgtype;	/* string with allowed msg types,
						** special cases: all,none = blank = default */
  char		*msgdesttype;	/* dest element type for each msgtype.
							** note that it has to be dest since
							** we require msgtype + desttype to 
							** uniquely specify a msgtype.
							** Special case: all */
  char		*msgsrc;	/* wildcard path of src. Special cases:
						** all = all elms, select = selected elms */
  char		*msgdest;	/* wildcard path of dest. Special cases:
						** all = all elms, select = selected elms */
  char		*msgfgstr;	/* string value for arrow  color */
  int		msgfg;		/* Pixel value for arrow in each msgtype.
						** automatically converted from the string
						** by the addmsgarrow func.*/
  int		fliparrow;	/* flag for direction of arrow */
  char		*script;	/* string for script op to do in all cases when
						** the src, dest and type criteria are met*/
						/* special cases: */
  char		*script_add;	/* string for script op to do 
						** if no arrow exists between specified pair */
  char		*script_drop;	/* string for script if already arrow */
};

#if 0
Defined in Tree.h
struct xtree_leaf_type {
	float xpts,ypts,zpts; /* coords of leaf */
	char	*ptnames;	/* name of leaf */
	int		shapeindex;	/* identifies which shape it belongs to */
  	int		shapeflags;	/* 2 highlights, truncation, selectability,
						** children, visibility, etc */ 
	long	fg;			/* specific color for leaf */
	long	textfg;		/* specific text color for leaf */
	char	*elm;		/* id of elm represented by leaf */
};
#endif

struct xtree_type {
  XGADGET_TYPE
  char		**shapes;	/* shape 0 is the default. */
  char		**shapevals;	/* an array of values of the icons */
  int		nshapes;
  char		*arrowmode;
  		/* msg, custom (user adds arrows),none */

  char		*treemode;	/* tree, geometry, custom (like kkit) */
  char		*namemode;	/* auto, custom (set ptnames directly), none */
  double	sizescale;
  struct xtree_leaf_type *ptlist;
  char		*_ptnames;	/* hidden pointer holding all the ptnames */
  char		*_currptname;
  int		_ptnamesize;
  int		ncoords;
  char		*path;		/* Path for looking up coords and values */
  char		*iconfunc;	/* optional script func for finding icon,
						** when passed the element path. Returns
						** a string which is matched to the icon
						** value field. If null, the element's
						** object name is used */
  char		*treefg;	/* color of arrow lines in tree */
  char		*msgpathmode; /* blank = "all", select, single elm path */
  char		*highlight1;		/* color of highlight 1 */
  char		*highlight2;		/* color of highlight 2 */
  char		*hlmode;	/* Highlight mode. none, one or multi */
  char		*orientation; /* one of: up, down, left or right */

  ElementList *select_elist;
  /* private values */
  ElementList *coord_elist;
  char		*coord_hash; /* hash table for coord_elist */
  int		*fromlist, *tolist, *arrowflags;
  int		nlist;
  int		max_nlist;
  /* the following msg specification arrays are handled using the
  ** utility functions ADDMSGARROW, DELETEMSGARROW */
  struct xtree_msgarrow_type *msgarrows;
  int		nmsgtypes;	/* number of assigned msgtypes */
  int		max_nmsgtypes;	/* number of allowed msgtypes */
};

#endif
