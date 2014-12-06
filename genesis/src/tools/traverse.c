static char rcsid[] = "$Id: traverse.c,v 1.2 2005/07/01 10:03:10 svitak Exp $";

/*
** $Log: traverse.c,v $
** Revision 1.2  2005/07/01 10:03:10  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2000/06/12 04:58:35  mhucka
** Added typecasts where appropriate.
**
** Revision 1.3  1997/05/29 09:13:55  dhb
** Update from Antwerp GENESIS version 21e
**
 * EDS21e revison: EDS BBF-UIA 97/01/28-97/04/22
 *  added Gabs_position functions for GELEMENTs
 *
 * Revision 1.2  1992/10/29  19:02:41  dhb
 * Replaced explicit msgin loops with MSGLOOP macro
 *
 * Revision 1.1  1992/10/29  19:00:10  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include <math.h>
#include "sim_ext.h"
#include "tools.h"
#include "seg_struct.h"
#include "hh_struct.h"

#ifndef EPSILON
#define EPSILON 1.0e-60
#endif
#define	CHANNEL	0
#define	RAXIAL	1
#define	AXIAL	2

void traverse_dends();
void traverse_dends_and_ch();
void traverse_ch();
void find_somas();
void scan_somas();
void ran_traverse();
void Grel_position();
void rel_position();
void rel_position_dends();

void find_all_kids(parent,el)
	Element	*parent;
	ElementList	*el;
{
	el->nelements = 0;
	traverse_ch(parent,el);
}

void find_all_dends(parent,find_ch,el)
	Element	*parent;
	int find_ch;
	ElementList	*el;
{
	ElementList	sel;
	Element	*selms[SMALL_EL_SIZE];
	int i;

	sel.nelements = 0;
	sel.element = selms;
	el->nelements = 0;


	find_somas(parent,&sel);

	for (i = 0 ; i < sel.nelements ; i++) {
		if (find_ch)
			traverse_dends_and_ch(sel.element[i],el);
		else
			traverse_dends(sel.element[i],el);
	}
}

void traverse_dends(parent,el)
	Element	*parent;
	ElementList	*el;
{
	Element *child;
	MsgIn	*msg;

	el->element[el->nelements] = parent;
	(el->nelements)++;

	MSGLOOP(parent, msg) {
	    case RAXIAL:
			child = msg->src;
			traverse_dends(child,el);
			break;
	}
}

void traverse_dends_and_ch(parent,el)
	Element	*parent;
	ElementList	*el;
{
	Element *child;
	MsgIn	*msg;

	traverse_ch(parent,el);

	MSGLOOP(parent, msg) {
	    case RAXIAL:
			child = msg->src;
			traverse_dends_and_ch(child,el);
			break;
	}
}

void traverse_ch(parent,el)
	Element	*parent;
	ElementList	*el;
{
	Element *child;

	el->element[el->nelements] = parent;
	(el->nelements)++;

	for (child = parent->child ; child ; child = child->next) {
		traverse_ch(child,el);
	}
}

void find_somas(parent,el)
	Element	*parent;
	ElementList	*el;
{
	el->nelements = 0;
	if ((strcmp(parent->object->name,"compartment") == 0) ||
		(strcmp(parent->object->name,"symcompartment") == 0)) {
	/* This element itself is a compartment, so proceed */
		el->element[0] = parent;
		el->nelements = 1;
	} else {
	/* Look among children for somas : no AXIAL input messages */
		scan_somas(parent,el);
	}
}

Element *find_parent_dend(elm)
	Element	*elm;
{
	MsgIn	*msg;
	Element	*parent;

	MSGLOOP(elm, msg) {
	    case AXIAL:
		{
			parent = msg->src;
			if ((strcmp(parent->object->name,"compartment") == 0) ||
				(strcmp(parent->object->name,"symcompartment") == 0)) 
				return(parent);
		}
		break;
	}
	return(NULL);
}

void FreeEL(el)
	ElementList	*el;
{
	if (el->nelements > 0)
		free(el->element);
	free(el);
}

void scan_somas(parent,el)
	Element	*parent;
	ElementList	*el;
{
	Element *gramp,*child;
	MsgIn	*msg;

	if ((strcmp(parent->object->name,"compartment") == 0) ||
		(strcmp(parent->object->name,"symcompartment") == 0)) {
		gramp = NULL;
		MSGLOOP(parent, msg) {
		    case AXIAL:
			{
				gramp = msg->src;
				goto found;
			}
		        /* NOTREACHED */
			break;
		}
		found:
		if (!gramp) {
			el->element[el->nelements] = parent;
			el->nelements += 1;
		}
	} else {
		for (child = parent->child; child ; child = child->next) {
			scan_somas(child,el);
		}
	}
}

/*
** rancoord : If the given element is not a compartment, it 
** looks for children which are compartments without incoming AXIAL
** messages : i.e. somas.
** If the given element is a compartment, it starts the traversal there
** itself.
**
** Having found the root elements, it then traverses them by their 
** message trees, relatively positioning the children.
** if the el and ch-flags are set it scales the el props &
** ch densities accordingly.
*/
void rancoord(parent,fx,fy,fz,el_flag,ch_flag)
	Element	*parent;
	float	fx,fy,fz;
	int		el_flag,ch_flag;
{
	ElementList	el;
	Element	*elms[SMALL_EL_SIZE];
	int	i;
	
	el.element = elms;
	find_somas(parent,&el);

	for(i = 0 ; i < el.nelements ; i++)
		ran_traverse(el.element[i],NULL,fx,fy,fz,el_flag,ch_flag);
}

void ran_traverse(parent,gramp,fx,fy,fz,el_flag,ch_flag)
	Element	*parent,*gramp;
	float	fx,fy,fz;
	int		el_flag,ch_flag;
{
	Element *child;
	struct compartment_type *temp;
	float dx,dy,dz;
	float original_l,final_l;
	float ratio;
	MsgIn	*msg;

	if (gramp) {
		dx = parent->x - gramp->x;
		dy = parent->y - gramp->y;
		dz = parent->z - gramp->z;
		original_l = dx * dx + dy * dy + dz * dz;
		dx = frandom(dx,-dx) * fx;
		dy = frandom(dy,-dy) * fy;
		dz = frandom(dz,-dz) * fz;

		rel_position_dends(parent,dx,dy,dz);
		if ((strcmp(parent->object->name,"compartment") == 0) ||
			(strcmp(parent->object->name,"symcompartment") == 0)) {
			final_l = dx * dx + dy * dy + dz * dz;
			ratio = (sqrt(final_l))/(sqrt(original_l));
			if (el_flag) {
				temp = (struct compartment_type *)parent;
				temp->Ra *= ratio;
				temp->Rm /= ratio;
				temp->Cm *= ratio;
			}
			if (ch_flag) {
				MSGLOOP(parent, msg) {
				    case CHANNEL:
					{
						child = msg->src;
						if (strcmp(child->object->type,
							"hh_channel")== 0) {
							((struct hh_channel_type *)child)->Gbar *=
								ratio;
						} else if (strcmp(child->object->type,
							"channelC2")== 0) {
							((struct channelC2_type *)child)->gmax *=
								ratio;
						}
					}
					break;
				}
			}
		}
	}
	MSGLOOP(parent, msg) {
	    case RAXIAL:
		{
			child = msg->src;
			ran_traverse(child,parent,fx,fy,fz,el_flag,ch_flag);
		}
		break;
	}
}

/* 
 * Gabs_position
 *
 * FUNCTION
 *	Procedure to position a Gelement and all its children at
 *      coorinates x0,y0,z0 to x,y,z.  Checks for non-Gelements.
 *      for all other elements.
 *
 * ARGUMENTS
 *     element   -- address of element
 *	x0,y0,z0 -- first coordinate
 *	x,y,z    -- last coordinate
 *
 * RETURN VALUE
 *     void
 *
 * AUTHORS
 *	Erik De Schutter
 */

/*
** Positions the Gelement at x0,y0,z0 to x,y,z, then moves all kids by same 
**  amount, checks for non-Gelements.
*/
void Gabs_position(parent,x0,y0,z0,x,y,z)
	GElement *parent;
	float x0,y0,z0,x,y,z;
{
	float x1,y1,z1,dx,dy,dz;
	Element *kid;

	x1 = parent->x;
	y1 = parent->y;
	z1 = parent->z;
	/* find offsets (assume they are linear, we don't check) */
	dx = x - x1;
	dy = y - y1;
	dz = z - z1;

	/* position parent: do it exactly in case x0,... was not preset */
	if (IsGElement(parent)) {
	    parent->x0=x0;
	    parent->y0=y0;
	    parent->z0=z0;
	}
	parent->x=x;
	parent->y=y;
	parent->z=z;

	/* position all children: assume linear offset is sufficient */
	for (kid = parent->child ; kid; kid = kid->next) {
	    if ((kid->x==x1)&&(kid->y==y1)&&(kid->z==z1)) {
	    /* same coordinates as parent -> copy */
		Gabs_position((GElement *) kid,x0,y0,z0,x,y,z);
	    } else {
	    /* different coordinates than parent -> move */
		Grel_position((GElement *) kid,dx,dy,dz);
	    }
	}
}


/* 
 * Grel_position
 *
 * FUNCTION
 *	Procedure to move a Gelement and all its children by dx,dy,dz.
 *      Checks for non-Gelements.
 *
 * ARGUMENTS
 *      element   -- address of element
 *	dx,dy,dz  -- coordinate offset
 *
 * RETURN VALUE
 *     void
 *
 * AUTHORS
 *	Erik De Schutter
 */

void Grel_position(parent,dx,dy,dz)
	GElement *parent;
	float dx,dy,dz;
{
	Element *kid;

	/* position parent */
	if (IsGElement(parent)) {
	    parent->x0 += dx;
	    parent->y0 += dy;
	    parent->z0 += dz;
	}
	parent->x += dx;
	parent->y += dy;
	parent->z += dz;
	/* position all children */
	for (kid = parent->child ; kid; kid = kid->next) {
	    Grel_position((GElement *) kid,dx,dy,dz);
	}
}


/*
** Positions the element at x,y,z, then moves all kids by same amount
*/
void abs_position(parent,x,y,z)
	Element	*parent;
	float x,y,z;
{
	float dx,dy,dz;
	
	dx = x - parent->x;
	dy = y - parent->y;
	dz = z - parent->z;

	rel_position(parent,dx,dy,dz);
}

/*
** Moves the element and all kids by dx,dy,dz
*/
void rel_position(parent,dx,dy,dz)
	Element	*parent;
	float dx,dy,dz;
{
	Element	*kid;
	
	parent->x += dx;
	parent->y += dy;
	parent->z += dz;
	for(kid = parent->child ; kid; kid = kid->next) {
		rel_position(kid,dx,dy,dz);
	}
}

/*
** moves the element and all its non-compartment kids by dx,dy,dz
*/
void rel_position_noncomps(parent,dx,dy,dz)
	Element	*parent;
	float dx,dy,dz;
{
	Element	*kid;
	
	parent->x += dx;
	parent->y += dy;
	parent->z += dz;
	for(kid = parent->child ; kid; kid = kid->next) {
		if ((strcmp(parent->object->name,"compartment") != 0) &&
			(strcmp(parent->object->name,"symcompartment") != 0)) 
			rel_position_noncomps(kid,dx,dy,dz);
	}
}

/*
** Moves the element and all its distal dendrites (found by traversing
** the message lists) by dx,dy,dz
*/
void rel_position_dends(parent,dx,dy,dz)
    Element *parent;
    float dx,dy,dz;
{
    Element *kid;
	MsgIn	*msg;

    rel_position_noncomps(parent,dx,dy,dz);

    MSGLOOP(parent, msg) {
	case RAXIAL:
        {
            kid = msg->src;
            rel_position_dends(kid,dx,dy,dz);
        }
	break;
    }
}
