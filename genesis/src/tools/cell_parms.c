static char rcsid[] = "$Id: cell_parms.c,v 1.2 2005/07/01 10:03:10 svitak Exp $";

/*
** $Log: cell_parms.c,v $
** Revision 1.2  2005/07/01 10:03:10  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1993/03/11 23:22:54  dhb
** Changed use of environment variables to use extended fields.
** NOTE: while previously one was able to use any environment variable name
** for sendmsg env vars, you must now use sendmsg# where # must start at one
** and # must increase by one for each sendmsg# variable.
**
** Revision 1.1  1992/12/11  19:05:46  dhb
** Initial revision
**
*/

#include <stdio.h>
#include <math.h>
#include "sim_ext.h"
#include "tools.h"
#include "seg_struct.h"
#include "hh_struct.h"
#include "olf_struct.h"

float calc_len(elm,RM,RA)
	struct compartment_type	*elm;
	float RM,RA;
{
	float len;
	Element	*parent,*find_parent_dend();
	char*	shape;
/* test whether marked as spherical */
	shape = GetExtField(elm, "Shape");
	if (shape != NULL &&
		(strncmp(shape,"sphere",6) == 0)) {
		/* spherical, return zero length */
		len = 0.0;
	} else {
		parent = find_parent_dend(elm);
		if (parent) {
			len = sqrt((elm->x - parent->x) * (elm->x - parent->x) +
				(elm->y - parent->y) * (elm->y - parent->y) +
				(elm->z - parent->z) * (elm->z - parent->z));
		} else {
			if (elm->dia > 0.0) {
				len = RM / (elm->Rm * elm->dia * PI);
			} else {
				len = RM * RM * elm->Ra / (4 * PI * RA * elm->Rm * elm->Rm);
				len = exp((log(len))/3.0);
			}
		}
	}
	return(len);
}

float calc_dia(elm,RM,RA)
	struct compartment_type	*elm;
	float RM,RA;
{
	float dia,len;
	float calc_len();

	if (elm->dia > 0.0) {
		return(elm->dia);
	}
	len = calc_len(elm,RM,RA);
	dia = RM / (elm->Rm * len * PI);
	return(dia);
}

float calc_cond(elm,chan,RM,RA,syn_dens_flag)
	struct compartment_type *elm;
	Element	*chan;
	float RM,RA;
	int syn_dens_flag;
{
	float dia,len;
	float cond;
	float calc_len();

	len = calc_len(elm,RM,RA);
	dia = RM / (elm->Rm * len * PI);

	if (strcmp(chan->object->name,"hh_channel") == 0) {
		cond = ((struct hh_channel_type *)chan)->Gbar;
	} else if (strcmp(chan->object->name,"vdep_channel") == 0) {
		cond = ((struct vdep_channel_type *)chan)->gbar;
	} else if (strcmp(chan->object->name,"channelC2") == 0) {
		cond = ((struct channelC2_type *)chan)->gmax;
		if (!syn_dens_flag)
			cond *= len * dia * PI;
	} else if (strcmp(chan->object->name,"receptor2") == 0) {
		cond = ((struct olf_receptor2_type *)chan)->gmax;
		if (!syn_dens_flag)
			cond *= len * dia * PI;
	} else if (strcmp(chan->object->name,"channelC") == 0) {
		cond = ((struct channelC_type *)chan)->gmax;
		if (!syn_dens_flag)
			cond *= len * dia * PI;
	} else {
		return(-1.0);
	}
	return(cond/(len * dia * PI));
}
