static char rcsid[] = "$Id: msg_clone.c,v 1.2 2005/07/01 10:03:04 svitak Exp $";

/*
** $Log: msg_clone.c,v $
** Revision 1.2  2005/07/01 10:03:04  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  2003/05/30 22:25:25  gen-dbeeman
** New version of kinetics library from Upi Bhalla, April 2003
**
*
*/

#include "sim_ext.h"

/* This file is for cloning all msgs between a path Orig
** to a domain D. There is a new path C identical to Orig, and
** all the messages between D and Orig,
** in either direction, are replicated between D and C.
*/

extern Msg* AllocMsg();
extern ElementList* WildcardGetElement();


/* This function duplicates the outgoing messages from the orig
** list to domain */
int CloneOutMsgs(ElementList* orig, ElementList* copy,
	ElementList* domain) {
	Msg* msg_out;
	Msg* msg_in;
	Msg* copy_msg_in;
	int i, j;
	int ret = 0;

    for(i = 0 ; i < orig->nelements ; i++) {
		int	nmsgout;
		Element *oe = orig->element[i];
		Element *ce = copy->element[i];
		Element *orig_msg_dst;

		/* go through all of the original msgs */
		nmsgout = oe->nmsgout;
		for (msg_out = oe->msgout; nmsgout-- > 0;
			msg_out = MSGOUTNEXT(msg_out)) {
	    	orig_msg_dst = msg_out->dst;
			if (orig_msg_dst == NULL) continue;

			/* Check that the orig_msg_dest is in the domain */
			for (j = 0; j < domain->nelements; j++) {
				if (domain->element[j] == orig_msg_dst)
					break;
			}
			if (j >= domain->nelements) /* Couldn't find it */
				continue;

	    	msg_in = msg_out;

	    	/*
	    	** add the msgs from copied elements to domain
	    	*/
	    	copy_msg_in = AllocMsg(orig_msg_dst, ce, msg_in->type,
				   MSGSLOT(msg_in), msg_in->nslots);
	    	if (copy_msg_in == NULL) {
				Error();
				printf("could not add message to '%s'\n",
				Pathname(orig_msg_dst));
				return 0;
	    	}
			MsgConvert(msg_in, copy_msg_in, oe, ce);
			ret++;
		}
	}
	return ret;
}

/* This function duplicates the incoming messages from the domain
** to the orig list */
int CloneInMsgs(ElementList* orig, ElementList* copy,
	ElementList* domain) {
	Element* orig_msg_src;
	Msg* copy_msg_in;

	int i, j, k;
	int ret = 0;

    for(i = 0 ; i < orig->nelements ; i++) {
		Element *oe = orig->element[i];
		Element *ce = copy->element[i];
		Msg* msg_in;

		/* go through all of the original msgs */
		for (msg_in = oe->msgin, k = 0; k < oe->nmsgin;
			msg_in = MSGINNEXT(msg_in), k++) {
	    	orig_msg_src = msg_in->src;
			if (orig_msg_src == NULL) continue;

			/* Check that the orig_msg_src is in the domain */
			for (j = 0; j < domain->nelements; j++) {
				if (domain->element[j] == orig_msg_src)
					break;
			}
			if (j >= domain->nelements) /* Couldn't find it */
				continue;

	    	/*
	    	** add the msgs from domain to copied elements.
	    	*/
	    	copy_msg_in = AllocMsg(ce, orig_msg_src, msg_in->type,
				   MSGSLOT(msg_in), msg_in->nslots);
	    	if (copy_msg_in == NULL) {
				Error();
				printf("could not add message to '%s'\n",
				Pathname(orig_msg_src));
				return 0;
	    	}
			MsgConvert(msg_in, copy_msg_in, orig_msg_src, orig_msg_src);
			ret++;
		}
	}
	return ret;
}

/*
** go through each slot and convert the data addresses to the
** copy
*/
int MsgConvert(orig_msg_in, copy_msg_in, orig_ptr, copy_ptr)
	Msg *orig_msg_in;
	Msg *copy_msg_in;
	Element* orig_ptr;
	Element* copy_ptr;
{
	int j;
	int offset;
	int objsize = Size(orig_ptr);

   	LinkMsgIn(copy_msg_in);
   	LinkMsgOut(copy_msg_in);
    for(j = 0; j < (int)orig_msg_in->nslots; j++) {
		offset = (MSGSLOT(orig_msg_in)[j].data - (char *)orig_ptr);
        if(offset < objsize && offset >= 0){
			MSGSLOT(copy_msg_in)[j].data = (char *)copy_ptr + offset;
		} else {
		    char*	extfldname;

		    /*
		    ** Could be an extended field
		    */

		    extfldname =
			GetExtFieldNameByAdr(orig_ptr, MSGSLOT(orig_msg_in)[j].data);

		    if (extfldname != NULL)
			MSGSLOT(copy_msg_in)[j].data =
			    (char*) GetExtFieldAdr(copy_ptr, extfldname, NULL);
		    else
			MSGSLOT(copy_msg_in)[j].data = MSGSLOT(orig_msg_in)[j].data;
		}
	}
	return 1;
}


/* This function does the cloning of the messages */
int CloneMsgs(orig, copy, domain)
	ElementList		*orig;
	ElementList		*copy;
	ElementList		*domain;
{
	int i, j;

	/* Check that the orig and copy are isomorphic */
	if (orig->nelements != copy->nelements) return 0;
	for (i = 0; i < orig->nelements; i++) {
		Element *oe = orig->element[i];
		Element *ce = copy->element[i];
		if (oe->object != ce->object) return 0;
		if (oe->index != ce->index) return 0;
		if (strcmp(oe->name, ce->name) != 0) return 0;
	}
	/* Go through the domain and eliminate all overlap with orig. */
	for (i = 0; i < orig->nelements; i++) {
		Element *oe = orig->element[i];
		for (j = 0; j < domain->nelements; j++) {
			if (oe == domain->element[j])
				domain->element[j] = NULL;
		}
	}

	i = CloneOutMsgs(orig, copy, domain);
	i += CloneInMsgs(orig, copy, domain);
	return i;
}

void do_clone_msgs(argc, argv)
	int argc;
	char **argv;
{
	ElementList *orig;
	ElementList *copy;
	ElementList *domain;
	initopt(argc, argv, "orig-path copy-path domain-path");
	if (G_getopt(argc, argv) < 0) {
		printoptusage(argc,argv);
		return;
	}
	orig = WildcardGetElement(optargv[1], 0);
	copy = WildcardGetElement(optargv[2], 0);
	domain = WildcardGetElement(optargv[3], 0);
	printf("%d messages cloned\n",
		CloneMsgs(orig, copy, domain));
	FreeElementList(orig);
	FreeElementList(copy);
	FreeElementList(domain);
}
