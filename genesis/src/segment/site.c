static char rcsid[] = "$Id: site.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: site.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:04:11  dhb
** Initial revision
**
*/

#include "seg_ext.h"

int	DEBUG_Site = 0;

/*
** Simple synaptic transformation which maps input to current
*/
/* 7/88 Matt Wilson */
void Site(site,action)
struct site_type	*site;
Action			*action;
{

    if(Debug(DEBUG_Site) > 1){
	ActionHeader("Site",site,action);
    }
    switch(action->type){
    case INIT:
	site->activation = 0;
	break;
    case PROCESS:
	/*
	** set the current equal to the activation level
	*/
	site->Ik = site->activation;
	break;
    case RESET:
	site->activation = 0;
	break;
    }
}
