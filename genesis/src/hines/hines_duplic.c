static char rcsid[] = "$Id: hines_duplic.c,v 1.2 2005/07/01 10:03:04 svitak Exp $";

/* Version EDS22c 97/12/05, Erik De Schutter, BBF-UIA 12/95-12/97 */

/*
** $Log: hines_duplic.c,v $
** Revision 1.2  2005/07/01 10:03:04  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.7  2000/09/19 18:52:04  mhucka
** Patch from Hugo for error in DUPLICATE operation.
**
** Revision 1.6  2000/09/07 05:13:49  mhucka
** Fix from Hugo Cornelis.  Allows to do an hsolve DUPLICATE from any
** working element if you use an absolute pathname as the first 'call'
** argument.
**
** Revision 1.5  2000/06/12 04:43:44  mhucka
** Fixed trivial errors in some printf format strings.
**
** Revision 1.4  2000/05/26 08:00:09  mhucka
** Bug fix from Erik De Schutter.
**
** Revision 1.3  1999/10/17 21:39:37  mhucka
** New version of Hines code from Erik De Schutter, dated 30 March 1999
** and including an update to hines.c from Erik from June 1999.  This version
** does not include changes to support Red Hat 6.0 and the Cray T3E, which
** will need to be incorporated in a separate revision update.
**
 * EDS22c revison: EDS BBF-UIA 97/11/28-97/12/05
 * Deleted use of values array
 *
** Revision 1.1  1997/05/28 23:11:50  dhb
** Initial revision
**
*/

#include "hines_ext.h"

/* Contains routine that duplicates an existing hines solver, so that an
**  additional cell can be simulated.  Tries to reuse as many tables as
**  possible. */
/* TO DO: - remove all variables from ops array so that it does not need to
**          be replicated.
**        - edit h_funcs_init sp that ravals can be filled without changing 
**          funcs. */


int do_duplicate(origsolve,newname,path)
	Hsolve	*origsolve;
	char	*newname,*path;
{
	char    *argv[10];
	Hsolve	*hsolve;
	ElementList *list;
	int 	ncompts;
	Element	**compts;
	int 	i,no_elminfo;
	int		*elmnum;
	char    *CopyString();
	double  *chip,*origchip;
	int     *ops,*origops;
	int     nchip,nop;
	Element *saveelm;

	if (!(newname) || !(path)) {
	    printf("Usage: call solve DUPLICATE duplic_name duplic_path\n");
	    return(ERR);
	}
	if (!(origsolve->path) || !(origsolve->ncompts)) {
	    Error();
	    printf(" original must be SETUP first: DUPLICATE %s failed.\n",Pathname(origsolve));
	    return(ERR);
	}
	hsolve = (Hsolve *)GetElement(newname);
	if (!hsolve) {
	    /* copy the origsolve to make new solve */
	    saveelm = WorkingElement();
	    SetWorkingElement(origsolve->parent);
	    argv[0] = "c_do_copy";
	    argv[1] = origsolve->name;
	    argv[2] = newname;
	    do_copy(3,argv);
	    hsolve = (Hsolve *)GetElement(newname);
	    SetWorkingElement(saveelm);
	    if (!hsolve) {
		Error();
		printf(" copy of %s failed: DUPLICATE failed.\n",Pathname(origsolve));
		return(ERR);
	    }
	}


	hsolve->path=CopyString(path);
	hsolve->readflag=HDUPLICATE_T;
	hsolve->origsolve=origsolve;

 	/*- get wildcard expansion relative to new solver */
 
 	saveelm = WorkingElement();
 	SetWorkingElement(hsolve);
  	list = WildcardGetElement(path,0);
 	SetWorkingElement(saveelm);

	if (hsolve->ncompts != list->nelements)
		return(ERR);
	compts=hsolve->compts = list->element;
	ncompts = hsolve->ncompts;
	elmnum = hsolve->elmnum;
	for (i=0;i<ncompts;i++) HsolveBlock(*compts++);

	no_elminfo=hsolve->no_elminfo;
	hsolve->no_elminfo=1;	/* don't recreate this info */
	hsolve->ravals=0;	/* we need to make and fill out a new one */
	switch (hsolve->chanmode) {
	    case 0 :
		if (h_hh_init(hsolve,0)) {
			h_failed(hsolve);
			return(ERR);
		}
		break;

	    case 2 :
	    case 3 :
	    case 4 :
 	    case 5 :
		hsolve->ininfo=NULL;
		hsolve->outinfo=NULL;
		if (h2_init(hsolve)) {
		    h_failed(hsolve);
		    return(ERR);
		}
		if (do_hget_children(hsolve)) {
		    h_failed(hsolve);
		    return(ERR);
		}
		/* h_hh_chip_init routines */
		nop=hsolve->nops;
		nchip=hsolve->nchips;
		chip = hsolve->chip=(double *)calloc(nchip,sizeof(double));
		ops = hsolve->ops=(int *)calloc(nop,sizeof(int));
		if (hsolve->storemode) hsolve->itotal=(double *)calloc(hsolve->nelm_names,sizeof(double));
		origops=origsolve->ops;
		for (i=0; i<nop; i++) *ops++=*origops++;
		origchip=origsolve->chip;
		for (i=0; i<nchip; i++) *chip++=*origchip++;
		if (hsolve->nconcs) {
		    if (h_init_conc_chip(hsolve)) {
			h_failed(hsolve);
			return(ERR);
		    }
		    if (h_init_conc_solve(hsolve)) {
			h_failed(hsolve);
			return(ERR);
		    }
		}
		break;

	    default :
		if (h_hh_init(hsolve)) {
		    h_failed(hsolve);
		    return(ERR);
		}
		break;
	}
	hsolve->no_elminfo=no_elminfo;	/* restore original setting */
	return(0);
}
