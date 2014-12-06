static char rcsid[] = "$Id: conn_3d.c,v 1.4 2005/10/18 06:37:54 svitak Exp $";

/*
** $Log: conn_3d.c,v $
** Revision 1.4  2005/10/18 06:37:54  svitak
** Closed memory leaks in do_gen_3d_msg and do_dd_3d_msg.
**
** Revision 1.3  2005/07/01 10:03:10  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.2  2005/06/20 19:31:25  svitak
** Removed malloc.h include and added stdlib.h where necessary. On many
** systems (e.g. Darwin), stdlib must be preceeded by stdio so stdio is
** always included (sorry about the effect on compile times).
**
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.13  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.12  2001/04/18 22:39:40  mhucka
** Miscellaneous portability fixes, mainly for SGI IRIX.
**
** Revision 1.11  2000/07/12 06:19:28  mhucka
** Added #include of malloc.h.
**
** Revision 1.10  1997/07/18 03:02:36  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.9  1995/05/08 21:39:55  dhb
** Reimplementation of -holerange and -offset options to the
** gen3dmsg command.
**
 * Revision 1.8  1994/09/16  23:34:47  dhb
 * Moved connection related code to oldconn/tools
 *
 * Revision 1.7  1994/08/08  22:11:20  dhb
 * Changes from Upi.
 *
 * Also fixed bug in initopt() string in do_gen_3d_msg().
 *
 * Revision 1.7  1994/06/13  22:38:35  bhalla
 * Replaced the missing options for  specifying a hole in the volume
 *
 * Revision 1.2  1994/05/26  13:55:12  bhalla
 * reintroduced fixes that had been lost in the Gen2 version.
 *
 * Revision 1.1  1994/05/09  20:40:08  bhalla
 * Initial revision
 *
 * Revision 1.6  1994/03/21  02:14:59  dhb
 * Changed volume_connect() options back to 1.4.x style.  The new options
 * may be compiled if the NEW_VOLUMECONNECT_OPTIONS define is defined.
 *
 * Also changed call to GetActionFunc() which was missing extra args
 * which were added for extended objects.
 *
 * Revision 1.5  1993/07/08  20:44:00  dhb
 * Fixed bug in gen_3d_msg(): changed type of slot_type to short!
 *
 * Revision 1.4  1993/03/18  17:21:20  dhb
 * Element field protection.
 *
 * Revision 1.3  1993/02/24  15:41:35  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * Revision 1.2  1992/10/29  19:35:29  dhb
 * Replaced explicit msgin looping to use MSGLOOP macro.
 * Modified message creation code to account for removal of slot names
 *   from slots.
 *
 * Revision 1.1  1992/10/29  19:21:45  dhb
 * Initial revision
 *
*/

/*******************************************************************
**                                                                **
**                          conn_3d.c                             **
**                Written by U.S.Bhalla.                          **
**  Permission to use and modify this software is freely granted, **
**    provided this message and acknowledgement remain intact.    **
**                                                                **
**	Parts of this code are derived from code written by           **
**                     Matt Wilson                                **
**                                                                **
**  I do not take any responsibility for the functionality and    **
**  use of this code, which is provided 'as is'.                  **
**                                                                **
*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sim_ext.h"
#include "shell_func_ext.h"

#ifndef EPSILON
#define EPSILON 1e-60
#endif

#define AXIAL 2
#define NSLOTS 20

struct vol_mask_type {
	short	type;
	short	box;
	float	cx,cy,cz;
	float	rx,ry,rz;
};

struct plane_mask_type {
	short	type;
	float	cx,cy,cz;
	float	nx,ny,nz;
	float	r,h;
};

static Element	*FindParentDend();
struct vol_mask_type *GetVolMaskFromArgv();
extern double ExtFieldMessageData();

/*
** PerpDistBetweenLines returns the perpendicular distance between
** two lines in 3-d space. The returned value is negative if the
** perpendicular lies outside the line segments. The line segments
** are specified by the coords of elements and their parents.
**
** The algebra comes from considering a parametric rep of the two
** lines : R = R1 + n(R2-R2), P = P1 + m(P2-P1). Varying n and m
** we generate the lines. Solve for the condition that R - P is 
** normal to both the line segments. Check that n and m are between
** 0 and 1 to be sure that the perp is within the line segments.
*/

float PerpDistBetweenLines(parent1,elm1,parent2,elm2)
	Element	*parent1,*elm1,*parent2,*elm2;
{
	float r1x,r1y,r1z;
	float x2,y2,z2;
	float	PerpDist2();

	if (!elm1 || !parent1 || !elm2 || !parent2 ) {
		fprintf(stderr,"dd_3d_msg : Elements for vol messages do not exist\n");
		Error();
		return(-1.0);
	}

	r1x = parent2->x; r1y = parent2->y; r1z = parent2->z;
	x2 = elm2->x; y2 = elm2->y; z2 = elm2->z;
	return(PerpDist2(parent1,elm1,r1x,r1y,r1z,x2,y2,z2));
}

float PerpDist2(parent1,elm1,r1x,r1y,r1z,x2,y2,z2)
	Element	*parent1,*elm1;
	float r1x,r1y,r1z,x2,y2,z2;
{
	float p1x,p1y,p1z;
	float dpx,dpy,dpz;
	float dpsqr;
	float px,py,pz;

	float drx,dry,drz;
	float drsqr;
	float rx,ry,rz;

	float m,n;
	float t1,t2;
	float dist;

	p1x = parent1->x; p1y = parent1->y; p1z = parent1->z;
	dpx = elm1->x - p1x; dpy = elm1->y - p1y; dpz = elm1->z - p1z;
	dpsqr = dpx * dpx + dpy * dpy + dpz * dpz;

	drx = x2 - r1x; dry = y2 - r1y; drz = z2 - r1z;
	drsqr = drx * drx + dry * dry + drz * drz;

	t2 = t1 = (dpx * drx + dpy * dry + dpz * drz);
	t1 = t1 * t1 / drsqr - dpsqr;

	if (fabs(t1) < EPSILON) {
	/* The lines are parallel */
		m = 0.5;
	} else {
		t2 *= drx * (p1x - r1x) + dry * (p1y - r1y) + drz * (p1z - r1z);
		t2 /= drsqr;
		t2 = dpx * (p1x - r1x) + dpy * (p1y - r1y) + dpz * (p1z - r1z)
			- t2;
		m = t2 / t1;
	}

	n = drx * (p1x + m * dpx - r1x) + dry * (p1y + m * dpy - r1y) + 
		drz * (p1z + m * dpz - r1z);
	n /= drsqr;

	if (m < 0) {
		px = p1x; py = p1y ; pz = p1z;
	} else if (m > 1) {
		px = p1x + dpx ; py = p1y + dpy ; pz = p1z + dpz ;
	} else {
		px = p1x + m * dpx ; py = p1y + m * dpy ; pz = p1z + m * dpz ;
	}

	if (n < 0) {
		rx = r1x; ry = r1y ; rz = r1z;
	} else if (n > 1) {
		rx = r1x + drx ; ry = r1y + dry ; rz = r1z + drz ;
	} else {
		rx = r1x + n * drx ; ry = r1y + n * dry ; rz = r1z + n * drz ;
	}

	dist = (px - rx) * (px - rx) + (py - ry) * (py - ry) + 
		(pz - rz) * (pz - rz);
	dist = sqrt(dist);

	return(dist);
	/*
	if (n > 0.0 && n < 1.0 && m > 0.0 && m < 1.0) {
		the perp is within the two line segments
		return(dist);
	} else {
		return(-dist);
	}
	*/
}

/*
** dd_sendmsg  sends messges from one element list to another
** if the dends which are parents of the elements on the list 
** are within a given range of each other. 
*/
void dd_sendmsg(source_chan,dest_chan,range,msgtype,slot,sarray,slotname,reqslots,str_value,number_value)
	ElementList	*source_chan;
	ElementList	*dest_chan;
	float	range;
	int		msgtype;
	short 		slot;
	Slot	*sarray;
	char	**slotname;
	int		reqslots;
	double		*number_value;
	char		**str_value;
{
        char *smalloc();
	int i,j,k;
	Element *sel, *del;
	float	dist;
	Element	*psel,*pdel;
	Element	*schan,*dchan;
	short slot_type;
	int sel_flag = 0,del_flag = 0;

	if (source_chan->nelements == 0 || dest_chan->nelements == 0)
		return;
	if (strcmp(source_chan->element[0]->object->name,"compartment")==0)
		sel_flag = 1;
	if (strcmp(dest_chan->element[0]->object->name,"compartment")==0)
		del_flag = 1;

	for (k = 0 ; k < source_chan->nelements ; k++) {
		schan = source_chan->element[k];
		if (sel_flag)
			sel = schan;
		else
			sel = schan->parent;
		psel = FindParentDend(sel);
		for (j = 0 ; j < dest_chan->nelements ; j++) {
			dchan = dest_chan->element[j];
			if (del_flag)
				del = dchan;
			else
				del = dchan->parent;
			pdel = FindParentDend(del);
			dist = PerpDistBetweenLines(psel,sel,pdel,del);
			if (dist >= 0 && dist < range) {
			    /*
			    ** assign the element specific info to the message
			    */
			    for(i=0;i<reqslots;i++){
					if (slotname[i] && (strcmp(slotname[i],"_string")
						== 0)){
				    	sarray[i].data = CopyString(str_value[i]);
					} else if (slotname[i] &&
						(strcmp(slotname[i],"_number") == 0)) {
				    	sarray[i].data = (char *)smalloc(sizeof(double));
				    	*((double *)(sarray[i].data)) = number_value[i];
					} else {
					int	prot;

					prot = GetFieldListProt(schan->object, slotname[i]);
					if (prot == FIELD_HIDDEN)
					  {
					    Error();
					    printf("dd_sendmsg: could not find field '%s' on '%s'\n", slotname[i], Pathname(schan));
					    return;
					  }

				    	if((sarray[i].data = 
				    		GetFieldAdr(schan,slotname[i],
							&slot_type)) == NULL){
					    if((sarray[i].data = (char *) GetExtFieldAdr(schan,slotname[i])) == NULL){
						Error();
						printf("dd_sendmsg: could not find field '%s' on '%s'\n", slotname[i], Pathname(schan));
						return;
					    }
					    else
						sarray[i].func = ExtFieldMessageData;
				    	}
					else
					    AssignSlotFunc(sarray+i,slot_type);
					}
			    }
				AddMsg(schan,dchan,msgtype,slot,sarray);
			}
		}
	}
}

/*
** This routine is largely based on Matt's do_add_msg routine
*/
void do_dd_3d_msg(argc,argv)
int 		argc;
char 		**argv;
{
char 		*dst_path;
char 		*src_path;
char		*typename;
int		type;
ElementList	*src_list;
ElementList	*dst_list;
Element 	*dst_element;
short 		slot;
int		nxtarg;
Slot		sarray[NSLOTS];
char		*slotname[NSLOTS];
struct mlist_type	*msgspec;
int		reqslots;
double		number_value[100];
char		*str_value[100];
float		range;
MsgList	*GetMsgListByName();


    /*
    ** check the syntax
    */
    initopt(argc, argv, "source-list dest-list range msg-type msg-data ...");
    if (G_getopt(argc, argv) != 0)
      {
	/*TraceScript();*/
	printoptusage(argc, argv);
	return;
      }

    src_path = optargv[1];
    dst_path = optargv[2];
    range = Atof(optargv[3]);
    typename = optargv[4];

    src_list = WildcardGetElement(src_path,0);
    if(src_list->nelements == 0){
	InvalidPath(optargv[0],src_path);
	FreeElementList(src_list);
	return;
    }
    dst_list = WildcardGetElement(dst_path,0);
    if(dst_list->nelements == 0){
	InvalidPath(optargv[0],dst_path);
	FreeElementList(src_list);
	FreeElementList(dst_list);
	return;
    }

	dst_element = dst_list->element[0];
	/* 
	** get the msg type 
	*/
	if((msgspec = GetMsgListByName(dst_element->object,typename)) == NULL){
	    Error();
	    printf("%s is not a valid msg type for '%s'\n",
	    	typename, Pathname(dst_element));
	    return;
	}

	type = msgspec->type;
	reqslots = msgspec->slots;

	slot = 0;
	nxtarg = 4;
	/*
	** get the message specification information from the argument list
	*/
	while(++nxtarg < optargc){
	    if(optargv[nxtarg][0] == '*'){
		if(slot >= NSLOTS){
		    Error();
		    printf("exceeded the maximum number of data slots\n");
		} else { 
		    str_value[slot] = CopyString(optargv[nxtarg]+1);
		    AssignSlotFunc(sarray+slot,STRING);
		    slotname[slot] = CopyString("_string");
		    slot++;
		}
	    } else 
	    if(optargv[nxtarg][0] == '\''){
		if(slot >= NSLOTS){
		    Error();
		    printf("exceeded the maximum number of data slots\n");
		} else { 
		    /*
		    ** find the trailing quote if any
		    */
		    if(strchr(optargv[nxtarg]+1,'\'') != NULL){
			/*
			** and remove it
			*/
			*strchr(optargv[nxtarg]+1,'\'') = '\0';
		    }
		    str_value[slot] = CopyString(optargv[nxtarg]+1);
		    AssignSlotFunc(sarray+slot,STRING);
		    slotname[slot] = CopyString("_string");
		    slot++;
		}
	    } else 
	    /* 
	    ** variable 
	    */
	    if(is_alpha(optargv[nxtarg][0])){
		if(slot >= NSLOTS){
		    Error();
		    printf("exceeded the maximum number of data slots\n");
		} else { 
		    slotname[slot] = CopyString(optargv[nxtarg]);
		    str_value[slot] = NULL; /* so it's not freed */
		    slot++;
		}
	    } else
	    /* 
	    ** number 
	    */
	    if(is_num(optargv[nxtarg][0]) ||
	    (optargv[nxtarg][0] == '.' && is_num(optargv[nxtarg][1]))){ 
		if(slot >= NSLOTS){
		    Error();
		    printf("exceeded the maximum number of data slots\n");
		} else { 
		    number_value[slot] = Atof(optargv[nxtarg]);
		    AssignSlotFunc(sarray+slot,DOUBLE);
		    slotname[slot] = CopyString("_number");
		    str_value[slot] = NULL; /* so it's not freed */
		    slot++;
		}
	    } else {
		Error();
		goto cleanup;
	    }
	}
	/*
	** check the number of slots filled against the number of
	** slots required
	*/
	if(slot != reqslots){
	    Error();
	    printf("msg type '%s' requires %d arguments.\n",
	    typename,reqslots);
	    goto cleanup;
	}

	dd_sendmsg(src_list,dst_list,range,type,slot,sarray,slotname,reqslots,
		str_value,number_value);

    /*
    ** CORE LEAK! --- should free up slotname elements which we CopyStringed
    ** earlier.
    **
    ** svitak - gnarly hack, but a quick fix to clean up the memory leak.
    */
cleanup:
    for ( --slot; slot >= 0; slot-- ) {
        FreeString(slotname[slot]);
        FreeString(str_value[slot]);	/* assumes FreeString checks for NULL */
    }

    FreeElementList(src_list);
    FreeElementList(dst_list);
    OK();
}

static Element	*FindParentDend(elm)
	Element	*elm;
{
	MsgIn	*msg;

	MSGLOOP(elm, msg) {
	    case AXIAL:
		{
			return(msg->src);
		}
	}
	return(NULL);
}


/*
** vol_sendmsg  sends messages from one element list to another
** if the elements are within a given range of each other.
*/
void vol_sendmsg(source_chan,dest_chan,range,prob,msgtype,slot,sarray,
	slotname,reqslots,str_value,number_value,
	holerange,ox,oy,oz)
	ElementList	*source_chan;
	ElementList	*dest_chan;
	float	range;
	float	prob;
	int		msgtype;
	short 		slot;
	Slot	*sarray;
	char	**slotname;
	int		reqslots;
	char		**str_value;
	double		*number_value;
	float holerange;
	float ox;
	float oy;
	float oz;
{
        char *smalloc();
	int i,j,k;
	float	dist;
	Element	*schan,*dchan;
	short	slot_type;

	for (k = 0 ; k < source_chan->nelements ; k++) {
		schan = source_chan->element[k];
		for (j = 0 ; j < dest_chan->nelements ; j++) {
			if (prob < 1.0 && urandom() > prob)
				continue;
			dchan = dest_chan->element[j];
			dist = (schan->x + ox - dchan->x) * (schan->x + ox - dchan->x) +
			(schan->y + oy - dchan->y) * (schan->y + oy - dchan->y) +
			(schan->z + oz - dchan->z) * (schan->z + oz - dchan->z) ;
			dist = sqrt(dist);
			if (dist < range && dist >= holerange) {
			    /*
			    ** assign the element specific info to the message
			    */
			    for(i=0;i<reqslots;i++){
					if (slotname[i] && (strcmp(slotname[i],"_string")
						== 0)){
				    	sarray[i].data = CopyString(str_value[i]);
					} else if (slotname[i] &&
						(strcmp(slotname[i],"_number") == 0)) {
				    	sarray[i].data = (char *)smalloc(sizeof(double));
				    	*((double *)(sarray[i].data)) = number_value[i];
					} else {
					int	prot;

					prot = GetFieldListProt(schan->object, slotname[i]);
					if (prot == FIELD_HIDDEN)
					  {
					    Error();
					    printf("vol_sendmsg: could not find field '%s' on '%s'\n", slotname[i], Pathname(schan));
					    return;
					  }

				    	if((sarray[i].data = 
				    		GetFieldAdr(schan,slotname[i],
							&slot_type)) == NULL){
					    if((sarray[i].data = (char *) GetExtFieldAdr(schan,slotname[i])) == NULL){
							Error();
							printf("vol_sendmsg: could not find field '%s' on '%s'\n", slotname[i], Pathname(schan));
							return;
					    }
					    else
						sarray[i].func = ExtFieldMessageData;
				    	}
					else
					    AssignSlotFunc(sarray+i,slot_type);
					}
			    }
				AddMsg(schan,dchan,msgtype,slot,sarray);
			}
		}
	}
}

/*
** This routine is largely based on Matt's do_add_msg routine
**
** Extensions by Upi, Jan 1994:
** extended the syntax to allow for an offsets in the region
** and a hole. This should be adequate for most msg and active msg
** (replacing connections) cases, but a more complex version will
** be needed to replace the conn_3d and region_connect routines.
*/
void do_gen_3d_msg(argc,argv)
int 		argc;
char 		**argv;
{
char 		*dst_path;
char 		*src_path;
char		*typename;
int		type;
ElementList	*src_list;
ElementList	*dst_list;
Element 	*dst_element;
short 		slot;
int		nxtarg;
Slot		sarray[NSLOTS];
char		*slotname[NSLOTS];
struct mlist_type	*msgspec;
int		reqslots;
double		number_value[100];
char		*str_value[100];
float		range,prob;
MsgList	*GetMsgListByName();
float		holerange = 0.0;
float		ox = 0.0; /* offset for connections and hole */
float		oy = 0.0;
float		oz = 0.0;
int			status;

    /*
    ** check the syntax
    */
    initopt(argc, argv, "source-list dest-list range probability msg-type msg-data ... -offset x y z -holerange range");

	while ((status = G_getopt(argc,argv)) == 1) {
		if (strcmp(G_optopt,"-holerange") == 0) {
			holerange = Atof(optargv[1]);
		}
		if (strcmp(G_optopt,"-offset") == 0) {
			ox = Atof(optargv[1]);
			oy = Atof(optargv[2]);
			oz = Atof(optargv[3]);
		}
	}

    if (status < 0)
      {
	/*TraceScript();*/
	printoptusage(argc, argv);
	return;
      }

    src_path = optargv[1];
    dst_path = optargv[2];
    range = Atof(optargv[3]);
    prob = Atof(optargv[4]);
    typename = optargv[5];

    src_list = WildcardGetElement(src_path,0);
    if(src_list->nelements == 0){
	InvalidPath(optargv[0],src_path);
	FreeElementList(src_list);
	return;
    }
    dst_list = WildcardGetElement(dst_path,0);
    if(dst_list->nelements == 0){
	InvalidPath(optargv[0],dst_path);
	FreeElementList(src_list);
	FreeElementList(dst_list);
	return;
    }

	dst_element = dst_list->element[0];
	/* 
	** get the msg type 
	*/
	if((msgspec = GetMsgListByName(dst_element->object,typename)) == NULL){
	    Error();
	    printf("%s is not a valid msg type for '%s'\n",
	    	typename, Pathname(dst_element));
	    return;
	}

	type = msgspec->type;
	reqslots = msgspec->slots;

	slot = 0;
	nxtarg = 5;
	/*
	** get the message specification information from the argument list
	*/
	while(++nxtarg < optargc){
	    if(optargv[nxtarg][0] == '*'){
		if(slot >= NSLOTS){
		    Error();
		    printf("exceeded the maximum number of data slots\n");
		} else { 
		    str_value[slot] = CopyString(optargv[nxtarg]+1);
		    AssignSlotFunc(sarray+slot,STRING);
		    slotname[slot] = CopyString("_string");
		    slot++;
		}
	    } else 
	    if(optargv[nxtarg][0] == '\''){
		if(slot >= NSLOTS){
		    Error();
		    printf("exceeded the maximum number of data slots\n");
		} else { 
		    /*
		    ** find the trailing quote if any
		    */
		    if(strchr(optargv[nxtarg]+1,'\'') != NULL){
			/*
			** and remove it
			*/
			*strchr(optargv[nxtarg]+1,'\'') = '\0';
		    }
		    str_value[slot] = CopyString(optargv[nxtarg]+1);
		    AssignSlotFunc(sarray+slot,STRING);
		    slotname[slot] = CopyString("_string");
		    slot++;
		}
	    } else 
	    /* 
	    ** variable 
	    */
	    if(is_alpha(optargv[nxtarg][0])){
		if(slot >= NSLOTS){
		    Error();
		    printf("exceeded the maximum number of data slots\n");
		} else { 
		    slotname[slot] = CopyString(optargv[nxtarg]);
		    str_value[slot] = NULL; /* so it's not freed */
		    slot++;
		}
	    } else
	    /* 
	    ** number 
	    */
	    if(is_num(optargv[nxtarg][0]) ||
	    (optargv[nxtarg][0] == '.' && is_num(optargv[nxtarg][1]))){ 
		if(slot >= NSLOTS){
		    Error();
		    printf("exceeded the maximum number of data slots\n");
		} else { 
		    number_value[slot] = Atof(optargv[nxtarg]);
		    AssignSlotFunc(sarray+slot,DOUBLE);
		    slotname[slot] = CopyString("_number");
		    str_value[slot] = NULL; /* so it's not freed */
		    slot++;
		}
	    } else {
		Error();
		goto cleanup;
	    }
	}
	/*
	** check the number of slots filled against the number of
	** slots required
	*/
	if(slot != reqslots){
	    Error();
	    printf("msg type '%s' requires %d arguments.\n",
	    typename,reqslots);
	    goto cleanup;
	}

	vol_sendmsg(src_list,dst_list,range,prob,type,slot,sarray,
		slotname,reqslots,str_value,number_value,
		holerange,ox,oy,oz);

    /*
    ** CORE LEAK! --- should free up slotname elements which we CopyStringed
    ** earlier.
    **
    ** svitak - gnarly hack, but a quick fix to clean up the memory leak.
    */
cleanup:
    for ( --slot; slot >= 0; slot-- ) {
        FreeString(slotname[slot]);
        FreeString(str_value[slot]);	/* assumes FreeString checks for NULL */
    }

    FreeElementList(src_list);
    FreeElementList(dst_list);
    OK();
}


