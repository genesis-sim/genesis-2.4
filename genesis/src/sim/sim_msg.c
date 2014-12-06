static char rcsid[] = "$Id: sim_msg.c,v 1.4 2005/07/01 10:03:09 svitak Exp $";

/*
** $Log: sim_msg.c,v $
** Revision 1.4  2005/07/01 10:03:09  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.3  2005/06/29 16:51:47  svitak
** Init srcelem to NULL to quiet compiler. Changed name of srcelm to theelem
** for better maintainability (difficult to discern from srcelem).
**
** Revision 1.2  2005/06/27 19:01:08  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.34  2001/04/25 17:17:02  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.32  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.31  1997/05/29 19:46:20  dhb
** Added DeleteMsgReferringToData() which will remove any outbound
** messages which reference a given blobk of data.
**
** Revision 1.30  1997/05/23 00:00:06  dhb
** Added RemapMsgData() which remaps messages slot pointers
** which point within an old data block to point to the corresponding
** place in a new data block.
**
** Revision 1.29  1996/07/18 23:43:23  dhb
** Added "-slot msg-number slot-number" option to do_getmsg(); returns
** data in given slot.
**
 * Revision 1.28  1996/05/16  22:14:17  dhb
 * Return to linked list implementation of messages.
 * Merged MsgOut and MsgIn structures into a single Msg structure
 * with links for both inbound and outbound message lists.  Element
 * supports message list tail pointer for faster insertions.
 * GetMsgOutByMsgIn() and GetMsgInByMsgOut() have been replaced with
 * macros which just return the Msg pointer.
 *
 * Messages are allocated individually.  Will eventually support
 * message allocation pools for various message sizes.
 *
 * Revision 1.27  1995/06/16  06:00:30  dhb
 * FreeBSD compatibility.
 *
 * Revision 1.26  1995/06/12  21:26:29  dhb
 * Removed all the old code under the #ifdef COMMENT preprocessor
 * statement.  This was dead code anyway.
 *
 * Revision 1.25  1995/06/12  21:18:05  venkat
 * The Linux compiler was complaining (Dieter's machine) about the uncommented
 * comments within the #ifdef COMMENT preprocessor statements. dhb decided that those comments can be removed.
 *
 * Revision 1.24  1995/01/24  19:41:56  dhb
 * do_getmsg() would return NULL for the destination of an incoming
 * message.  Fixed to return the correct destination.
 *
 * Revision 1.23  1995/01/09  19:32:41  dhb
 * Changed MsgListAdd() so that it doesn't allocate memory for
 * slotnames then there are no slots for the message.
 *
 * Revision 1.22  1995/01/09  19:06:44  dhb
 * Added DeleteAllMsgsByType() which will delete all MsgIns on an element
 * of a given message type.
 *
 * Revision 1.21  1994/12/20  00:25:51  dhb
 * Added DeleteAllMsgs() code to delete all messages (in and out) from
 * an element.
 *
 * Added DeleteMsg() which deletes a message (e.g. the MsgOut from the
 * source element and the MsgIn from the destination element).
 *
 * Changed the lastmsg MsgIn field to msgflags to support multiple
 * message flags.  Now have both a last message flag and a forwarded
 * message flag.
 *
 * Changed do_deletemsg() to not allow deleting of forwarded messages.
 *
 * Moved calling of actions for deleting messages out of DeleteMsgIn()
 * and DeleteMsgOut().  This means that the calling routines are now
 * responsible for calling the actions.  But now the actions see a
 * consistent messages data structure when they are called.
 *
 * Added AddForwMsg() and DeleteForwMsg() as interface routines for
 * message forwarding.
 *
 * Revision 1.20  1994/12/15  17:47:49  dhb
 * Added MSGINDELETED and MSGOUTDELETED actions which are called AFTER
 * a MsgIn or MsgOut is deleted.  This is in addition to DELETEMSGIN and
 * DELETEMSGOUT which are called prior to deleting MsgIn or MsgOut.
 *
 * Revision 1.19  1994/11/10  21:31:48  dhb
 * Moved CopyMsgs() from sim_copy.c to here where it belongs.
 *
 * CopyMsgs() wasn't setting up the msgout->msgin fields of the copies.
 *
 * ADDMSGIN and ADDMSGOUT actions were being called in the AddMsgIn() and
 * AddMsgOut() functions.  This would cause the consistency of the messaging
 * to be inconsistent in the action functions.  Moved the action calls out
 * into the AddMsg() and  ForwardMsg()
 * oops, the do_forw_msg() functions after both the MsgIn and MsgOut data
 * is added.  One side affect of this change is that ADDMSGIN and ADDMSGOUT
 * are no longer called for the messages copied when an element is copied.
 * But I don't think it was right to call the actions in this situation
 * anyways.
 *
 * Made the AddMsgIn() and AddMsgOut() functions static to prevent calls
 * which would not support the message adding semantics.
 *
 * Revision 1.18  1994/08/31  02:49:56  dhb
 * Changed order of DeleteMsg...() calls in do_delete_msg().
 *
 * Revision 1.17  1994/08/08  22:06:19  dhb
 * Merge of 1.15.1.1 changes.
 *
<<<<<<< 1.16
 * Revision 1.16  1994/06/15  00:28:31  dhb
 * Fixes to ReallocateMsgIn() and AddMsg() related to maintaining the
 * msgin field of MsgOut structures.
 *
=======
 * Revision 1.15.1.1  1994/08/08  22:01:55  dhb
 * Changes from Upi.
 *
 * Revision 1.16  1994/06/13  22:32:11  bhalla
 * Added GetSlotType function,
 * put in check in get_msg to avoid error messages
 *
>>>>>>> 1.15.1.1
 * Revision 1.15  1994/05/28  00:13:02  dhb
 * Missing variable decl in RemapMsgOutPtrsToMsgIn().
 *
 * Revision 1.14  1994/05/27  22:59:11  dhb
 * Merged in 1.10.1.1 changes.
 *
<<<<<<< 1.13
 * Revision 1.13  1994/04/11  21:01:40  dhb
 * Missed the merge conflict on the first line.
 *
 * Revision 1.12  1994/04/08  23:50:27  dhb
 * Merged in Upi update in version 1.8.1.1.
 *
<<<<<<< 1.11
 * Revision 1.11  1994/03/28  15:53:47  dhb
 * Fixed bug in PreallocateMsgs(): missing arg to realloc().
 *
=======
 * Revision 1.10.1.1  1994/05/27  22:47:56  dhb
 * Changes to extended field references: moved ExtFieldMessageData() to
 * sim_extfields.c and have GetExtFieldAdr() return data access function
 * in additional arg.
 *
 * The MsgIn pointer in MsgOut structures have been put back to improve
 * performance.  This required changing these pointers whenever MsgIn
 * structure move in memory.
 *
 * Added two functions GetMsgInNumber() and GetMsgOutNumber() to return
 * the message number for a given MsgIn or MsgOut.
 *
 * Implemented message forwarding.  do_forw_msg() and do_del_forw_msg()
 * implement the addforwmsg and deleteforwmsg commands.  The MsgIn from
 * the forwarding element is basically copied, changing the message type
 * number.  The messages for the forwarding and receiving elements must
 * have the same name and number of slots.
 *
>>>>>>> 1.10.1.1
 * Revision 1.10  1994/03/22  17:42:22  dhb
 * Added Upi's GetStringMsgData() function.  The MSGPTR() macro does the
 * same function, so GetStringMsgData() will evetually be removed.
 *
 * Revision 1.9  1994/03/18  19:34:32  dhb
 * Changed MsgListAdd() to zero the flags field.
 *
=======
 * Revision 1.8.1.1  1994/04/08  23:38:16  dhb
 * Update from Upi
 *
 * Revision 1.4  1994/04/04  17:37:47  bhalla
 * Cleaned up GetStringMsgData to return NULL if the slot does not
 * contain a string
 *
 * Revision 1.3  1994/03/21  21:47:24  bhalla
 * Added new option to the deletemsg and getmsg commands:
 * 	-find srcelem msgtype
 * which allows the message to be identified by the src and dest and type.
 * In the process found and fixed a bug in deletemsg (DeleteMsgIn) func.
 *
 * Revision 1.2  1994/03/21  16:54:16  bhalla
 * Added func GetStringMsgData to get string values in msgs
 *
 * Revision 1.1  1994/01/13  16:55:44  bhalla
 * Initial revision
 *
>>>>>>> 1.8.1.1
 * Revision 1.8  1993/12/23  02:29:47  dhb
 * Extended Objects (mostly complete)
 *
 * Revision 1.7  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.6  1993/03/18  17:15:55  dhb
 * Element field protection.
 *
 * Revision 1.5  1993/03/11  18:11:41  dhb
 * Made messages work for extended fields.
 *
 * Revision 1.4  1993/02/17  18:38:39  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_add_msg (sendmsg), do_delete_msg (deletemsg), do_show_msg (showmsg)
 * 	and do_getmsg (getmsg) changed to use GENESIS getopt routines.
 *
 * 	All changed to use command options -incoming and -outgoing instead of
 * 	a command argument IN or OUT.
 *
 * 	do_getmsg moved type of field to get from command arguments to command
 * 	options which take the message number as argument (e.g. getmsg elm IN
 * 	0 type ==> getmsg elm -incoming -type 0).
 *
 * Revision 1.3  1993/02/03  23:16:10  dhb
 * Changed PreallocateMsgs() in two ways:
 *
 * Only allocate memory for non-zero number of in or out bound msgs
 *
 * The code allows for a destination with already existing messages.
 * In this case, the old code reallocated enough memory for the larger
 * of the source and destination elements.  Changed to allocate enough
 * memory for all message in the source and destination, since the caller
 * will be adding the source element message to the destination rather
 * than replacing them.
 *
 * Revision 1.2  1992/12/18  21:12:23  dhb
 * Compact messages
 *
 * Revision 1.1  1992/10/27  20:19:31  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"


#define NSLOTS		20



/*
** MsgListEqual
**
** FUNCTION
**	Determine if two MsgList lists are the same.
**
** ARGUMENTS
**	MsgList* list1	- The two MsgLists to compare.
**	MsgList* list2
**
** RETURN VALUE
**	int		- 1 if the lists are the same, 0 if not.
**
** NOTES
**	This routine assumes the two MsgLists are from objects with a
**	common base object!  This will be true for its use in the object
**	cache code in sim_extobject.c which requires that two objects must
**	have the same super object before checking MsgList equality.
**
** AUTHOR
**	David Bilitch
*/

int MsgListEqual(list1, list2)

MsgList*	list1;
MsgList*	list2;

{	/* MsgListEqual --- */

	MsgList*	m1;
	MsgList*	m2;

	m1 = list1;
	m2 = list2;
	while (m1 != NULL && m2 != NULL)
	  {
	    m1 = m1->next;
	    m2 = m2->next;
	  }

	if (m1 != m2)
	    return 0;

	for (m1 = list1; m1 != NULL; m1 = m1->next)
	  {
	    for (m2 = list2; m2 != NULL; m2 = m2->next)
	      {
		if (m1->slots == m2->slots && strcmp(m1->name, m2->name) == 0)
		    break;
	      }

	    if (m2 == NULL)
		return 0;
	  }

	return 1;

}	/* MsgListEqual */



/*
** msgs must be evaluated for validity after changing the
** enabled status of structure containing out messages
*/

MsgList *GetMsgListByName(object,name)
GenesisObject	*object;
char	*name;
{
MsgList	*list;
    
    /*
    ** look up the name in the object msg list
    */
    for(list=object->valid_msgs;list;list=list->next){
	if(strcmp(list->name,name) == 0){
	    return(list);
	}
    }
    return(NULL);
}

double DoubleMessageData(slot)
Slot *slot;
{
	return(*((double *)(slot->data)));
}

double FloatMessageData(slot)
Slot *slot;
{
	return((double)(*((float *)(slot->data))));
}

double IntMessageData(slot)
Slot *slot;
{
	return((double)(*((int *)(slot->data))));
}

double ShortMessageData(slot)
Slot *slot;
{
	return((double)(*((short *)(slot->data))));
}

double StringMessageData(slot)
Slot *slot;
{
	return((double)(Atof(slot->data)));
}

double VoidMessageData(slot)
Slot *slot;
{
	return(0.0);
}

/* Upi addition - needed for Xodus. This is now also used in 
** sim_dump as well. Slot checking has been added.
**
** This function will go away as there is the MSGPTR(msg_in,slotno) macro
** which does the same thing. (sez Dave. I dont think that 
** macro does everything done by this func  -- Upi).
*/
char *GetStringMsgData(msg_in,slotno)
	MsgIn	*msg_in;
	int		slotno;
{
Slot *slot;

    if (msg_in == NULL)
	return(NULL);

	/* check slotno - need to update */
	if (slotno < 0 || slotno > 10)
		return(NULL);

    slot = (Slot *) MSGSLOT(msg_in);
	/* do type checking here */
	if ((ADDR)slot[slotno].func != (ADDR)StringMessageData)
		return(NULL);
	return(slot[slotno].data);
}

/* Another Upi addition, used in olf/table.c. This function returns
** an int identifying the slot function so we can find out
** what the type of the source field was.
*/
 
int GetSlotType(slot,slotno)
    Slot    *slot;
    int     slotno;
{
    ADDR func = (ADDR)slot[slotno].func;
    if (func ==  (ADDR)DoubleMessageData) {
         return(DOUBLE);
   } else if (func == (ADDR)FloatMessageData) {
         return(FLOAT);
   } else if (func == (ADDR)IntMessageData) {
         return(INT);
   } else if (func == (ADDR)ShortMessageData) {
         return(SHORT);
   } else {
         return(INVALID);
    }
}

/*
** DisplayMsgIn is supposed to print the source element field name
** on the left of the equal sign, which was the 1.4 behaviour.  In
** removing the source field name from the slot data structure the
** meaning of this field was misconstrued as the slot name.  GENESIS
** 2.0 prints the slot name rather than the source field name.  This
** needs to be fixed.
*/

void DisplayMsgIn(msg_in, slotnames)
Msg *msg_in;
char **slotnames;
{
Slot *slot;
int i;
PFD func;

    if (msg_in == NULL)
	return;

	 slot = (Slot *) MSGSLOT(msg_in);
    for(i=0;i<msg_in->nslots;i++){
	if(slotnames[i]){
	    printf("< %s = ", slotnames[i]);
	    func = slot[i].func;
	    if(func == DoubleMessageData){
		printf("%g",*((double *)(slot[i].data)));
	    } else
	    if(func == FloatMessageData){
		printf("%g",*((float *)(slot[i].data)));
	    } else
	    if(func == IntMessageData){
		printf("%d",*((int *)(slot[i].data)));
	    } else
	    if(func == ShortMessageData){
		printf("%d",*((short *)(slot[i].data)));
	    } else 
	    if(func == StringMessageData){
		printf("%s",slot[i].data);
	    } else /* externally defined slot type, just call the conversion
		    * function and print as a double
		    */
		printf("%g", func(slot+i));
	    printf(" > ");
	} else 
	break;
    }
}

MsgList *GetMsgListByType(object,type)
GenesisObject	*object;
int	type;
{
MsgList	*list;
	
    for(list=object->valid_msgs;list;list=list->next){
	if(list->type == type){
	    return(list);
	}
    }
    return(NULL);
}

void ShowMsgIn(msg, object, nmsgs)

Msg		*msg;
GenesisObject	*object;
unsigned int	nmsgs;

{
    int		count;
    MsgList	*ml;

    if(msg == NULL){
	return;
    }

    for(count = 0; count < nmsgs; count++){
	if (VISIBLE(msg->src)){
	    ml=GetMsgListByType(object,msg->type);
	    if (ml)
		    printf("MSG %d from '%s' %s type [%d] '%s' ", 
			    count,Pathname(msg->src),MSGINFORW(msg) ? "(forwarded)" : "",msg->type, ml->name);
	    else
		    printf("MSG %d from '%s' %s type [%d] '%s' ", 
			    count,Pathname(msg->src),MSGINFORW(msg) ? "(forwarded)" : "",msg->type, "nil");
	    DisplayMsgIn(msg, ml->slotname);
	    printf("\n");
	}

	msg = MSGINNEXT(msg);
    }
    printf("\n");
}

void ShowMsgOut(msgin, nmsgs)
Msg *msgin;
unsigned int nmsgs;
{
int count;
MsgList	*ml;

    if(msgin == NULL){
	return;
    }

    for(count = 0;count < nmsgs; count++){
	if (VISIBLE(msgin->dst)){
	    ml=GetMsgListByType(msgin->dst->object, msgin->type);
	    if (ml)
		    printf("MSG %d to '%s' type [%d] '%s' ", 
		    count, Pathname(msgin->dst),
		    msgin->type, ml->name);
	    else
		    printf("MSG %d to '%s' type [%d] '%s' ", 
		    count, Pathname(msgin->dst),
		    msgin->type, "nil");
	    DisplayMsgIn(msgin, ml->slotname);
	    printf("\n");
	}

	msgin = MSGOUTNEXT(msgin);
    }
    printf("\n");
}



int UnlinkMsgIn(msg)

Msg	*msg;

{
    Element	*element;
    int		count;
    Msg		*ptr;
    Msg		*prev;
    Slot*	slot;
    int		i;

    if(msg == NULL)
	return 0;

    element = msg->dst;
    ptr = element->msgin;
    prev = NULL;
    for (count = 0; count < element->nmsgin; count++)
      {
	if (ptr == msg)
	  {
	    /*
	    ** Free the address structure for extended fields
	    */

	    slot = MSGSLOT(msg);
	    for (i = 0; i < msg->nslots; i++)
		FreeExtFieldAdr(slot[i].data, slot[i].func);

	    /*
	    ** remove the msgin by unlinking the message from the msgin
	    ** list.
	    */

	    if (prev == NULL)
		element->msgin = msg->next_in;
	    else
		prev->next_in = msg->next_in;

	    if (msg == element->msgintail)
		element->msgintail = prev;

	    element->nmsgin--;
	    break;
	  }

	prev = ptr;
	ptr = MSGINNEXT(ptr);
      }

    return(1);
}



int UnlinkMsgOut(msgout)

Msg	*msgout;

{
    Element	*element;
    Msg		*ptr;
    Msg		*prev;
    int		count;

    if(msgout == NULL)
	return(0);

    /*
    ** find the msgout on the src that references it 
    */

    element = msgout->src;
    ptr = element->msgout;
    prev = NULL;
    for (count = 0; count < element->nmsgout; count++)
      {
	if (ptr == msgout)
	  {
	    /*
	    ** take the msgout out by unlinking it from the msgout
	    ** list.
	    */

	    if (prev == NULL)
		element->msgout = msgout->next_out;
	    else
		prev->next_out = msgout->next_out;

	    if (msgout == element->msgouttail)
		element->msgouttail = prev;

	    element->nmsgout--;
	    break;
	  }

	prev = ptr;
	ptr = MSGOUTNEXT(ptr);
    }

    return(1);
}


int GetMsgOutNumber(element, msgout)

Element*	element;
Msg*		msgout;

{	/* GetMsgOutNumber --- Return the number of the message out */

	Msg*	curmsgout;
	int	msgNo;

	curmsgout = element->msgout;
	for (msgNo = 0; msgNo < element->nmsgout; msgNo++)
	  {
	    if (curmsgout == msgout)
		return msgNo;

	    curmsgout = MSGOUTNEXT(curmsgout);
	  }

	return -1;

}	/* GetMsgOutNumber */


Msg *GetMsgOutByNumber(element,msgnum)

Element	*element;
int msgnum;

{
    Msg	*msgout;
    int	count;

    if(element == NULL || msgnum < 0 || msgnum >= element->nmsgout)
	return(NULL);

    /*
    ** find the msgout on the src that references it 
    */

    msgout = element->msgout;
    for (count = 0; count < msgnum; count++)
	msgout = MSGOUTNEXT(msgout);

    return msgout;
}


int GetMsgInNumber(element, msgin)

Element*	element;
Msg*		msgin;

{	/* GetMsgInNumber --- Return the message number of the message */

	Msg*	curmsgin;
	int	msgNo;

	curmsgin = element->msgin;
	for (msgNo = 0; msgNo < element->nmsgin; msgNo++)
	  {
	    if (curmsgin == msgin)
		return msgNo;

	    curmsgin = MSGINNEXT(curmsgin);
	  }

	return -1;

}	/* GetMsgInNumber */


Msg *GetMsgInByNumber(element,msgnum)
Element	*element;
int msgnum;
{
    Msg	*msgin;
    int	count;

    if(element == NULL || msgnum < 0 || msgnum >= element->nmsgin)
	return(NULL);

    /*
    ** find the msgout on the src that references it 
    */

    msgin = element->msgin;
    for (count = 0; count < msgnum; count++)
	msgin = MSGINNEXT(msgin);

    return(msgin);
}



int MsgInMemReq(element)

Element*	element;

{
	Msg*	msgin;
	int	nbytes;
	int	count;

	msgin = element->msgin;
	if (msgin == NULL)
	    return 0;

	nbytes = 0;
	for (count = 0; count < element->nmsgin; count++)
	  {
	    nbytes += sizeof(Msg) + msgin->nslots*sizeof(Slot);
	    msgin = MSGINNEXT(msgin);
	  }

	return nbytes;
}

/*
** This will eventually handle several pools of msgins by number of
** slots available.
*/

Msg* AllocMsg(dst, src, type, slots, nslots)

Element*	dst;
Element*	src;
int		type;
Slot*		slots;
int		nslots;

{
	Msg*	msgin;

	msgin = (Msg*) malloc(sizeof(Msg) + sizeof(Slot)*nslots);
	if (msgin != NULL)
	  {
	    msgin->src = src;
	    msgin->dst = dst;
	    msgin->type = type;
	    msgin->msgflags = 0;
	    msgin->next_in = NULL;
	    msgin->next_out = NULL;
	    msgin->nslots = nslots;
	    if (slots != NULL)
		BCOPY(slots, (Slot *) MSGSLOT(msgin), nslots*sizeof(Slot));
	  }

	return msgin;
}


void FreeMsg(msg)

Msg*	msg;

{
	if (msg != NULL)
	    free(msg);
}


/*
 * RemapMsgData
 *
 * FUNCTION
 *     When data that message slots access is moved (e.g. due to a 
 *     realloc from one block to another) this function has to be 
 *     called so that the message slots point to the correct data 
 *     entries.  This function assumes that the new data block size
 *     is at least as big as the old one (which will be the case
 *     in a realloc).
 *     
 * ARGUMENTS
 *     element          -- pointer to element
 *     olddatablock     -- pointer to old data block
 *     olddatablocksize -- size of old data block
 *     newdatablock     -- pointer to new data block
 *
 *     
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */


void RemapMsgData( element, olddatablock, olddatablocksize, newdatablock )
  Element *element;
  char    *olddatablock;
  int      olddatablocksize;
  char    *newdatablock;
{
  int   i;
  Msg  *msg;
  Slot *slot;


  /*
   * Iterate through all the outgoing messages of the element.
   */

  MSGOUTLOOP( element, msg )
    {
      slot = MSGSLOT( msg );


      /*
       * Do for all slots in the message.
       */

      for ( i = 0; i < msg->nslots; i++ )
	{
	  /*
	   * If the slot->data field is in olddatablock
	   * then remap it to the newdatablock.
	   */

	  char *end_of_olddatablock = olddatablock + olddatablocksize;

	  if ( ( slot->data >= olddatablock ) &&
	       ( slot->data < end_of_olddatablock ) )
	    {
	      /* The field is inside the old data block, so remap it. */

	      slot->data = newdatablock + ( slot->data - olddatablock );
	    }

	  slot++;
	}
    }
}


/*
 * DeleteMsgReferringToData
 *
 * FUNCTION
 *     When data which might be refered to in an outgoing message
 *     is deleted, this function must be called to remove the
 *     affected outbound messages.  Removes all output messages
 *     with data slot references to the given memory block.
 *     
 * ARGUMENTS
 *     element          -- pointer to element
 *     datablock        -- pointer to the invalidated data block
 *     datablocksize    -- size of the invalidated data block
 *
 *     
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *   Dave Bilitch (based on code for RemapMsgData by Mike Vanier)
 */


void DeleteMsgReferringToData(element, datablock, datablocksize)

Element *element;
char    *datablock;
int      datablocksize;

{
  int   i;
  Msg  *msg;
  Msg  *delete_me;
  Slot *slot;


  /*
   * Iterate through all the outgoing messages of the element.
   */

  delete_me = NULL;
  MSGOUTLOOP(element, msg)
    {
      if (delete_me != NULL)
	{
	  DeleteMsg(delete_me);
	  delete_me = NULL;
	}

      slot = MSGSLOT(msg);


      /*
       * Do for all slots in the message.
       */

      for (i = 0; i < msg->nslots; i++)
	{
	  /*
	   * If the slot->data field points into the datablock
	   * then remove the message.
	   */

	  char *end_of_datablock = datablock + datablocksize;

	  if (slot->data >= datablock && slot->data < end_of_datablock)
	    {
	      /*
	      ** we postpone deleting the message until we are through
	      ** using it to link to the next message.  If we don't
	      ** delay the delete, we'll have to access the freed
	      ** memory for the link.
	      */

	      delete_me = msg;
	      break;
	    }

	  slot++;
	}
    }

  if (delete_me != NULL)
      DeleteMsg(delete_me);
}



int LinkMsgIn(msgin)

Msg*		msgin;

{
	Element*	dst;

	if (msgin == NULL || msgin->dst == NULL)
	    return 0;

	dst = msgin->dst;
	msgin->next_in = NULL;
	if (dst->msgin == NULL)
	    dst->msgin = msgin;
	else
	    dst->msgintail->next_in = msgin;
	dst->msgintail = msgin;

	dst->nmsgin++;

	return 1;
}


int LinkMsgOut(msgin)

Msg*		msgin;

{
	Element*	src;

	if (msgin == NULL || msgin->src == NULL)
	    return 0;

	src = msgin->src;
	msgin->next_out = NULL;
	if (src->msgout == NULL)
	    src->msgout = msgin;
	else
	    src->msgouttail->next_out = msgin;
	src->msgouttail = msgin;
	src->nmsgout++;

	return 1;
}


void CopyMsgs(orig,copy)
Element		*orig;
Element		*copy;
{
struct equiv_type {
    Element	*orig;
    Element	*copy;
} *equiv_table;
int 		count=0;
int 		i,j;
int 		size;
ElementStack 		*stk0,*stk1;
Element 	*orig_ptr;
Element 	*copy_ptr;
Msg		*msg_in;
Msg		*copy_msg_in;
Msg		*msg_out;
Element		*orig_msg_dst;
Element		*copy_msg_dst;
int		offset;
int		objsize;

    size = CountChildren(2,orig,0) + 1;
    equiv_table = (struct equiv_type *)
    malloc(size * sizeof(struct equiv_type));

    /*
    ** the trees should be identical or strange things could happen
    */
    stk0 = NewPutElementStack(orig);
    stk1 = NewPutElementStack(copy);
    /*
    ** check the first element 
    */
    equiv_table[0].orig = orig;
    equiv_table[0].copy = copy;
    count = 1;
    /*
    ** construct the element equivalence table
    ** by adding all elements in the tree
    */
    while((orig_ptr = NewFastNextElement(2,stk0)) != NULL){
	copy_ptr = NewFastNextElement(2,stk1);
	equiv_table[count].orig = orig_ptr;
	equiv_table[count].copy = copy_ptr;
	count++;
    }
    NewFreeElementStack(stk0);
    NewFreeElementStack(stk1);
    /*
    ** go through all of the elements in the table and map the msgs
    */
    for(i=0;i<count;i++){
	int	nmsgout;

	/*
	** get the next original projection
	*/
	orig_ptr = equiv_table[i].orig;
	/*
	** start the copy without any connections 
	*/
	copy_ptr = equiv_table[i].copy;
	/*
	** and go through all of the original msgs
	*/
	objsize = Size(orig_ptr);
	nmsgout = orig_ptr->nmsgout;
	for(msg_out=orig_ptr->msgout;nmsgout-- > 0; msg_out = MSGOUTNEXT(msg_out)){
	    /*
	    ** get the destination element
	    */
	    orig_msg_dst = msg_out->dst;
	    msg_in = msg_out;
	    /*
	    ** get the equivalent destination element from the copy
	    ** by searching the equivalence table
	    */
	    for(j=0;j<count;j++){
		if(equiv_table[j].orig == orig_msg_dst)
		    break;
	    }
	    if(j >= count){
		/*
		** could not find the destination element within the 
		** table indicating a non-local msg
		** so forget it
		*/
		continue;
	    }
	    /*
	    ** get the equivalent copy destination element  
	    */
	    copy_msg_dst = equiv_table[j].copy;

	    /*
	    ** add the msgs to the copy elements
	    */

	    copy_msg_in = AllocMsg(copy_msg_dst, copy_ptr, msg_in->type,
				   MSGSLOT(msg_in), msg_in->nslots);
	    if (copy_msg_in == NULL) {
		Error();
		printf("could not add message to '%s'\n",
		Pathname(copy_msg_dst));
		free(equiv_table);
		return;
	    }
	    LinkMsgIn(copy_msg_in);
	    LinkMsgOut(copy_msg_in);

	    /*
	    ** go through each slot and convert the data addresses to the
	    ** copy
	    ** LATER check the msg copying to handle non-contiguous
	    ** addresses like dynamic arrays etc.
	    */

	    for(j=0;j<(int)msg_in->nslots;j++){
                offset = (MSGSLOT(msg_in)[j].data - (char *)orig_ptr);
                if(offset < objsize && offset >= 0){
                    MSGSLOT(copy_msg_in)[j].data = (char *)copy_ptr + offset;
                } else {
		    char*	extfldname;

		    /*
		    ** Could be an extended field
		    */

		    extfldname =
			GetExtFieldNameByAdr(orig_ptr, MSGSLOT(msg_in)[j].data);

		    if (extfldname != NULL)
			MSGSLOT(copy_msg_in)[j].data =
			    (char*) GetExtFieldAdr(copy_ptr, extfldname, NULL);
		    else
			MSGSLOT(copy_msg_in)[j].data = MSGSLOT(msg_in)[j].data;
                }
	    }
	}
    }
    free(equiv_table);
}

void do_show_msg(argc,argv)
int argc;
char **argv;
{
Element 	*bptr;
char 		*elementname;

	/* Since this function does not use any other options, we
	** wont put the -find option in either, though that would
	** not be too hard */
    initopt(argc, argv, "element");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    elementname = optargv[1];

    if((bptr = GetElement(elementname)) == NULL){
	TraceScript();
	printf("%s:  cannot find element '%s'\n",
	optargv[0],
	elementname);
	return;
    }

    printf("\nINCOMING MESSAGES\n");
    ShowMsgIn(bptr->msgin,bptr->object, bptr->nmsgin);
    printf("\nOUTGOING MESSAGES\n");
    ShowMsgOut(bptr->msgout, bptr->nmsgout);
}


/*
** DeleteAllMsgsByType
**
**	Deletes all incoming messages of a given message type.
*/

int DeleteAllMsgsByType(element, type)

Element*	element;
int		type;

{	/* DeleteAllMsgsByType --- Delete MsgIns matching type */

	Msg*	msgin;
	int	count;

	msgin = element->msgin;
	for (count = element->nmsgin; count > 0; count--)
	  {
	    Msg*	nextmsg = MSGINNEXT(msgin);

	    if (msgin->type == type)
		DeleteMsg(msgin);

	    msgin = nextmsg;
	  }

	return 1;

}	/* DeleteAllMsgsByType */


/*
** DeleteAllMsgs
**
**	Function to delete all messages in and out from the element.  Tries
**	to avoid calling redundant actions on element.
*/

void DeleteAllMsgs(element)

Element*	element;

{	/* DeleteAllMsgs --- Delete all incoming and outgoing messages */

    Msg*	msgin;
    Msg*	msgout;
    int		count;

    /* keep deleting first MsgIn until all gone */
    for (count = element->nmsgin; count > 0; count--) {
	msgin = element->msgin;
	if (MSGINFORW(msgin))
	    DeleteForwMsg(element, msgin);
	else {
	    DeleteMsg(msgin);
	}
    }

    /* Keep deleting first msgout until all gone */
    for (count = element->nmsgout; count > 0; count--) {
	msgout = element->msgout;
	DeleteMsg(msgout);
    }

}	/* DeleteAllMsgs */


int DeleteMsg(msgin)

Msg*	msgin;

{	/* DeleteMsg --- Delete the message given by msgin */

	Action		action;
	Element*	src;
	Element*	dst;

	if (msgin == NULL)
	    return 0;

	src = msgin->src;
	dst = msgin->dst;

	action.name = "DELETEMSGIN";
	action.type = DELETEMSGIN;
	action.data = (char*) msgin;
	action.argc = 0;
	action.argv = NULL;
	CallActionFunc(dst, &action);

	action.name = "DELETEMSGOUT";
	action.type = DELETEMSGOUT;
	action.data = (char*) msgin;
	action.argc = 0;
	action.argv = NULL;
	CallActionFunc(src, &action);

	UnlinkMsgOut(msgin);
	UnlinkMsgIn(msgin);
	FreeMsg(msgin);

	action.name = "MSGINDELETED";
	action.type = MSGINDELETED;
	action.data = NULL;
	action.argc = 0;
	action.argv = NULL;
	CallActionFunc(dst, &action);

	action.name = "MSGOUTDELETED";
	action.type = MSGOUTDELETED;
	action.data = NULL;
	action.argc = 0;
	action.argv = NULL;
	CallActionFunc(src, &action);

	return 1;

}	/* DeleteMsg */


int do_delete_msg(argc,argv)
int argc;
char **argv;
{
Element 	*element;
char 		*elementname;
Msg		*msgin;
int		msgnum;
int		incoming;
int		status;
int		do_find = 0;
char	*msgtype = NULL;
char	*srcelem = NULL;

    incoming = 1;
    initopt(argc, argv, "element message-number -incoming -outgoing -find srcelem type");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-incoming") == 0)
	    incoming = 1;
	else if (strcmp(G_optopt, "-outgoing") == 0)
	    incoming = 0;
	else if (strcmp(G_optopt, "-find") == 0) {
	    incoming = 1;
		do_find = 1;
		srcelem = optargv[1];
		msgtype = optargv[2];
	  }
      }

    if(status < 0) {
	printoptusage(argc, argv);
	return(0);
    }

    elementname = optargv[1];
    if((element = GetElement(elementname)) == NULL){
	Error();
	printf("%s:  cannot find element '%s'\n",
	optargv[0],
	elementname);
	return(0);
    }
    msgnum = atoi(optargv[2]);
	if (do_find) {
		Element *srcelm = GetElement(srcelem);
		if (!srcelm) {
			InvalidPath(optargv[0],srcelem);
			return(0);
   		}
		/* assume we only want to delete the first message found */
		if ((msgnum = GetMsgNumBySrcDest(srcelm,element,msgtype)) < 0){
			Warning();
			printf("Cannot find %s msg from %s to %s\n",
				msgtype,Pathname(srcelm), Pathname(element));
			return(0);
		}
	}

    if(incoming){
	if((msgin = GetMsgInByNumber(element,msgnum)) == NULL){
	    Error();
	    printf("could not get IN msg #%d from '%s'\n",
	    msgnum,
	    Pathname(element));
	    return(0);
	}
	if (MSGINFORW(msgin)) {
	    Error();
	    printf("cannot delete forwarded messages using %s\n", argv[0]);
	    return(0);
	}
    } else /* !incoming ==> outgoing */ {
	if((msgin = GetMsgOutByNumber(element,msgnum)) == NULL){
	    Error();
	    printf("could not get OUT msg #%d from '%s'\n",
	    msgnum,
	    Pathname(element));
	    return(0);
	}
	if (MSGINFORW(msgin)) {
	    Error();
	    printf("cannot delete forwarded messages using %s\n", argv[0]);
	    return(0);
	}
    }

    DeleteMsg(msgin);
    OK();
    return(1);
}

void AssignSlotFunc(slot,slot_type)
Slot	*slot;
int	slot_type;
{
		switch(slot_type){
		case DOUBLE:
			slot->func = DoubleMessageData;
		break;
		case FLOAT:
			slot->func = FloatMessageData;
		break;
		case INT:
			slot->func = IntMessageData;
		break;
		case SHORT:
			slot->func = ShortMessageData;
		break;
		case STRING:
			slot->func = StringMessageData;
		break;
		case CHAR:
			slot->func = VoidMessageData;
		break;
		default:
		    fprintf(stderr,
		    "Unknown data type %d in a message argument\n",slot_type);
		    slot->func = VoidMessageData;
		break;
		}
}


/*
** MsgListMaxType
**
** FUNCTION
**	Returns the highest message type in the object valid message list.
**
** ARGUMENTS
**	GenesisObject* object	- Object with message list to scan.
**
** RETURN VALUE
**	int			- Highest type in the message list
**
** AUTHOR
**	David Bilitch
*/

int MsgListMaxType(object)

GenesisObject*	object;

{	/* MsgListMaxType --- */

	MsgList*	ml;
	int		max;

	if (object->valid_msgs == NULL)
	    return 0;

	max = object->valid_msgs->type;
	for (ml = object->valid_msgs->next; ml != NULL; ml = ml->next)
	    if (ml->type > max)
		max = ml->type;

	return max;

}	/* MsgListMaxType */


/*
** MsgListAdd
**
** FUNCTION
**	Add a message to the valid message list on the object.
**
** ARGUMENTS
**	GenesisObject* object	- object to add the message to
**	char* msgName		- name of the new message
**	int msgType		- Type to assign the new message
**	char** slotNames	- Names of message slots
**	int slots		- Number of slots in the message
**
** RETURN VALUE
**	int			- 1 on success, 0 on memory allocation
**				  error
**
** AUTHOR
**	David Bilitch
*/

int MsgListAdd(object, msgName, msgType, slotNames, slots)

GenesisObject*	object;
char*		msgName;
int		msgType;
char**		slotNames;
int		slots;

{	/* MsgListAdd --- */

	MsgList*	new_msg;
	int		i;

	new_msg = (MsgList *)malloc(sizeof(MsgList));
	if (new_msg == NULL)
	  {
	    perror("MsgListAdd");
	    return 0;
	  }

	new_msg->type = msgType;
	new_msg->name = CopyString(msgName);
	new_msg->flags = 0;
	new_msg->slots = slots;
	if (slots > 0)
	    new_msg->slotname = (char **)malloc(slots*sizeof(char *));
	else
	    new_msg->slotname = (char **)NULL;

	if (new_msg->name == NULL || (slots > 0 && new_msg->slotname == NULL))
	  {
	    perror("MsgListAdd");
	    free(new_msg);
	    return 0;
	  }

	for(i = 0; i < slots; i++){
	    new_msg->slotname[i] = CopyString(slotNames[i]);
	    if (new_msg->slotname[i] == NULL)
	      {
		perror("MsgListAdd");
		for (i--; i >= 0; i--)
		    free(new_msg->slotname[i]);
		free(new_msg->name);
		free(new_msg->slotname);
		free(new_msg);

		return 0;
	      }
	}

	new_msg->next = object->valid_msgs;
	object->valid_msgs = new_msg;

	return 1;

}	/* MsgListAdd */


/*
** MsgListDelete
**
** FUNCTION
**	Delete a message from the list of valid messages on an element.
**
** ARGUMENTS
**	GenesisObject* object	- object from which to delete the message
**	char* msgName		- name of the message to delete
**
** RETURN VALUE
**	int			- 1 on success, 0 if the message is not in
**				  the list
**
** NOTES
**	Because the message list is shared between objects, we must copy
**	MessageList structures up to the one we want to delete in order
**	to preserve the list for other objects.
**
**	The message list is a list of what messages are defined for an
**	object and information about the messages.  This is not a list of
**	actual messages sent between elements.
**
** AUTHOR
**	David Bilitch
*/

int MsgListDelete(object, msgName)

GenesisObject*	object;
char*		msgName;

{	/* MsgListDelete --- Delete a message from the object message list */

	MsgList*	cur;
	MsgList*	copy;
	MsgList*	curcopy = NULL;

	copy = NULL;
	for (cur = object->valid_msgs; cur != NULL; cur = cur->next)
	  {
	    MsgList*	new;

	    if (strcmp(cur->name, msgName) == 0)
		break;

	    new = (MsgList*) malloc(sizeof(MsgList));
	    if (new == NULL)
	      {
		perror("MsgListDelete");
		return 0;
	      }

	    *new = *cur;
	    new->next = NULL;

	    if (copy == NULL)
		copy = new;
	    else
		curcopy->next = new;

	    curcopy = new;
	  }

	if (cur == NULL)
	  {
	    while (copy != NULL)
	      {
		cur = copy;
		copy = cur->next;
		free(cur);
	      }

	    return 0;
	  }

	if (copy == NULL)
	    copy = cur->next;
	else
            if (curcopy)
	        curcopy->next = cur->next;

	object->valid_msgs = copy;

	return 1;

}	/* MsgListDelete */


/*
** MsgListMakePermanent
**
** FUNCTION
**	Sets the MSGLIST_PERMANENT flag for all the new messages in the
**	valis_msgs list.
**
** ARGUMENTS
**	GenesisObject* object	- object with message list to make permanent
**
** RETURN VALUE
**	None.
**
** AUTHOR
**	David Bilitch
*/

void MsgListMakePermanent(object)

GenesisObject*	object;

{	/* MsgListMakePermanent --- Make object's messages permanent */

	MsgList*	cur;

	cur = object->valid_msgs;
	while (cur != NULL && (cur->flags&MSGLIST_PERMANENT) != MSGLIST_PERMANENT)
	  {
	    cur->flags |= MSGLIST_PERMANENT;
	    cur = cur->next;
	  }

}	/* MsgListMakePermanent */


/*
** do_add_msglist
**
** FUNCTION
**	The addmsg command adds a message to the list of allowable messages
**	to an element.
**
** ARGUMENTS
**	int argc	- Number of command arguments
**	char** argv	- Command arguments
**
** RETURN VALUE
**	int		- 1 on success, 0 if the element doesn't exist or the
**			  message cannot be added for some reason.
**
** SIDE EFFECTS
**	Adding to the message list changes the element's object.  On the first
**	change to an object this results in derivation from the original
**	object of the element.  Subsequent changes to the object continue to
**	have the original object as its super object.
**
** AUTHOR
**	David Bilitch
*/

int do_add_msglist(argc, argv)

int	argc;
char**	argv;

{	/* do_add_msglist --- */

	GenesisObject*	object;
	Element*	elm;
	char*		typeArg;
	char*		msgName;
	int		msgType;
	int		status;

	typeArg = NULL;
	initopt(argc, argv, "element message-name [message-arguments...] -type message-type");
	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    if (strcmp(G_optopt, "-type") == 0)
		typeArg = optargv[1];
	  }

	if (status < 0)
	  {
	    printoptusage(argc, argv);
	    return 0;
	  }

	elm = GetElement(optargv[1]);
	if (elm == NULL)
	  {
	    InvalidPath(optargv[0], optargv[1]);
	    return 0;
	  }

	msgName = optargv[2];
	if (GetMsgListByName(elm->object, msgName) != NULL)
	  {
	    fprintf(stderr, "%s: Message '%s' already exists on element '%s'\n",
		    optargv[0], msgName, Pathname(elm));
	    return 0;
	  }

	if (typeArg != NULL)
	  {
	    msgType = atoi(typeArg);
	    if (GetMsgListByType(elm->object, msgType) != NULL)
	      {
		fprintf(stderr, "%s: Message with type %d already exists on element '%s'\n",
			optargv[0], msgType, Pathname(elm));
		return 0;
	      }
	  }
	else /* determine message type dynamically */;
	    msgType = MsgListMaxType(elm->object) + 1;

	object = ObjectModify(elm->object);
	if (object == NULL)
	  {
	    fprintf(stderr, "%s: Cannot modify object; cannot add message '%s' to element '%s'\n",
		optargv[0], msgName, Pathname(elm));
	    return 0;
	  }

	if (!MsgListAdd(object, msgName, msgType, optargv+3, optargc - 3))
	  {
	    ObjectFree(object);
	    return 0;
	  }

	elm->object = ObjectCacheCheck(object);

	OK();
	return 1;

}	/* do_add_msglist */


/*
** do_delete_msglist
**
** FUNCTION
**	The deletemsg command  deletes a message from an element message list.
**
** ARGUMENTS
**	int argc	- number of command arguments
**	char** argv	- command arguments
**
** RETURN VALUE
**	int		- 1 on success, 0 if element cannot be found, the
**			  message doesn't exist or is permanent.
**
** SIDE EFFECTS
**	Deleting a message from an element's message list results in a new
**	object for the element.
**
** AUTHOR
**	David Bilitch
*/

int do_delete_msglist(argc, argv)

int	argc;
char**	argv;

{	/* do_delete_msglist --- Delete a message from the message list */

	GenesisObject*	object;
	Element*	elm;
	MsgList*	ml;
	char*		msgName;

	initopt(argc, argv, "element msg-name");
	if (G_getopt(argc, argv) != 0)
	  {
	    printoptusage(argc, argv);
	    return 0;
	  }

	elm = GetElement(optargv[1]);
	if (elm == NULL)
	  {
	    InvalidPath(optargv[0], optargv[1]);
	    return 0;
	  }

	msgName = optargv[2];
	ml = GetMsgListByName(elm->object, msgName);
	if (ml == NULL)
	  {
	    fprintf(stderr, "%s: Message '%s' does not exist on element '%s'\n",
		    optargv[0], msgName, Pathname(elm));
	    return 0;
	  }

	if (ml->flags&MSGLIST_PERMANENT)
	  {
	    fprintf(stderr, "%s: cannot delete permanent message '%s' from the message list of element '%s'\n",
		    optargv[0], msgName, Pathname(elm));
	    return 0;
	  }

	object = ObjectModify(elm->object);
	if (object == NULL)
	  {
	    fprintf(stderr, "%s: Cannot modify object; cannot delete message '%s' from element '%s'\n",
		optargv[0], msgName, Pathname(elm));
	    return 0;
	  }

	if (!MsgListDelete(object, msgName))
	  {
	    ObjectFree(object);
	    return 0;
	  }
	elm->object = ObjectCacheCheck(object);

	OK();
	return 1;

}	/* do_delete_msglist */


/*
** A forwarded message is a duplicate of some other incoming message.
** The duplicate does not appear in the outbound message list.
*/

Msg* AddForwMsg(dstElm, srcElm, type, slots, nslots)

Element*	dstElm;
Element*	srcElm;
short		type;
Slot*		slots;
int		nslots;

{	/* AddForwMsg --- Add the given Msg as forwarded to the dstElm */

	Action	action;
	Msg*	new_msgin;

	new_msgin = AllocMsg(dstElm, srcElm, type, slots, nslots);
	if (new_msgin != NULL)
	  {
	    new_msgin->msgflags |= MSGFLAGS_FORW;
	    LinkMsgIn(new_msgin);

	    action.name = "ADDMSGIN";
	    action.type = ADDMSGIN;
	    action.data = (char*) new_msgin;
	    action.argc = 0;
	    action.argv = NULL;
	    CallActionFunc(dstElm, &action);
	  }

	return(new_msgin);

}	/* AddForwMsg */


/*
** do_forw_msg
**
** FUNCTION
**	Forward a message from one element to another.  The destination
**	element must accept a message with the same name and number of
**	slots.  The forwarded message is identical to the message received
**	originally, except that the message type may change.
**
** ARGUMENTS
**	int argc	- number of command arguments
**	char** argv	- command arguments
**
** RETURN VALUE
**	int		- 1 on success, 0 if element cannot be found, the
**			  message doesn't exist, the message type doesn't
**			  exist on the destination or the number of slots
**			  don't match
**
** AUTHOR
**	David Bilitch
*/

int do_forw_msg(argc, argv)

int	argc;
char**	argv;

{	/* do_forw_msg --- Command to forward a message to another element */

	char*		srcName;
	Element*	srcElm;
	char*		dstName;
	Element*	dstElm;
	int		msgNo;

	Msg*		msgin;
	Msg*		new_msgin;
	MsgList*	srcML;
	MsgList*	dstML;


	char*		msgName;

	initopt(argc, argv, "source message-number destination");
	if (G_getopt(argc, argv) != 0 || sscanf(optargv[2], "%d", &msgNo) != 1)
	  {
	    printoptusage(argc, argv);
	    return 0;
	  }

	srcName = optargv[1];
	dstName = optargv[3];

	srcElm = GetElement(srcName);
	if (srcElm == NULL)
	  {
	    InvalidPath(argv[0], srcName);
	    return 0;
	  }

	dstElm = GetElement(dstName);
	if (dstElm == NULL)
	  {
	    InvalidPath(argv[0], dstName);
	    return 0;
	  }

	msgin = GetMsgInByNumber(srcElm, msgNo);
	if (msgin == NULL)
	  {
	    Error();
	    printf("%s: could not find message #%d on element %s\n", argv[0],
		    msgNo, Pathname(srcElm));
	    return 0;
	  }

	srcML = GetMsgListByType(srcElm->object, msgin->type);
	msgName = srcML->name;

	dstML = GetMsgListByName(dstElm->object, msgName);
	if (dstML == NULL)
	  {
	    Error();
	    printf("%s: message %s not defined on element %s\n", argv[0],
		    msgName, Pathname(dstElm));
	    return 0;
	  }

	if (srcML->slots != dstML->slots)
	  {
	    Error();
	    printf("%s: number of slots in the %s message for elements\n",
		    argv[0], msgName);
	    printf("  %s and %s do not match\n", Pathname(srcElm),
						 Pathname(dstElm));
	    return 0;
	  }

	new_msgin = AddForwMsg(dstElm, msgin->src, dstML->type, MSGSLOT(msgin),
					    msgin->nslots);
	if (new_msgin == NULL)
	  {
	    Error();
	    printf("%s: could not add forwarded message %s to element %s\n",
			argv[0], msgName, Pathname(dstElm));
	    return 0;
	  }

	OK();
	return 1;

}	/* do_forw_msg */


void DeleteForwMsg(elm, msgin)

Element*	elm;
Msg*		msgin;

{	/* DeleteForwMsg --- Delete The given forwarded message from elm */

	Action	action;

	if (elm == NULL || msgin == NULL || !MSGINFORW(msgin))
	    return;

	action.name = "DELETEMSGIN";
	action.type = DELETEMSGIN;
	action.data = (char*) msgin;
	action.argc = 0;
	action.argv = NULL;
	CallActionFunc(elm, &action);

	UnlinkMsgIn(msgin);
	FreeMsg(msgin);

	action.name = "MSGINDELETED";
	action.type = MSGINDELETED;
	action.data = NULL;
	action.argc = 0;
	action.argv = NULL;
	CallActionFunc(elm, &action);

}	/* DeleteForwMsg */


/*
** do_del_forw_msg
**
** FUNCTION
**	Removes a forwarded message.
**
** ARGUMENTS
**	int argc	- number of command arguments
**	char** argv	- command arguments
**
** RETURN VALUE
**	int		- 1 on success, 0 if element cannot be found, the
**			  message doesn't exist, etc.
**
** AUTHOR
**	David Bilitch
*/

int do_del_forw_msg(argc, argv)

int	argc;
char**	argv;

{	/* do_del_forw_msg --- Remove a forwarded message */

	char*		srcName;
	Element*	srcElm;
	char*		dstName;
	Element*	dstElm;
	int		msgNo;

	Msg*		srcmsgin;
	Msg*		dstmsgin;
	MsgList*	srcML;
	MsgList*	dstML;

	char*		msgName;
	int		i;

	initopt(argc, argv, "source message-number destination");
	if (G_getopt(argc, argv) != 0 || sscanf(optargv[2], "%d", &msgNo) != 1)
	  {
	    printoptusage(argc, argv);
	    return 0;
	  }

	srcName = optargv[1];
	dstName = optargv[3];

	srcElm = GetElement(srcName);
	if (srcElm == NULL)
	  {
	    InvalidPath(argv[0], srcName);
	    return 0;
	  }

	dstElm = GetElement(dstName);
	if (dstElm == NULL)
	  {
	    InvalidPath(argv[0], dstName);
	    return 0;
	  }

	srcmsgin = GetMsgInByNumber(srcElm, msgNo);
	if (srcmsgin == NULL)
	  {
	    Error();
	    printf("%s: could not find message #%d on element %s\n", argv[0],
		    msgNo, Pathname(srcElm));
	    return 0;
	  }

	srcML = GetMsgListByType(srcElm->object, srcmsgin->type);
	msgName = srcML->name;

	dstML = GetMsgListByName(dstElm->object, msgName);
	if (dstML == NULL)
	  {
	    Error();
	    printf("%s: message %s not defined on element %s\n", argv[0],
		    msgName, Pathname(dstElm));
	    return 0;
	  }

	if (srcML->slots != dstML->slots)
	  {
	    Error();
	    printf("%s: number of slots in the %s message for elements\n",
		    argv[0], msgName);
	    printf("  %s and %s do not match\n", Pathname(srcElm),
						 Pathname(dstElm));
	    return 0;
	  }

	dstmsgin = dstElm->msgin;
	for (i = 0; i < dstElm->nmsgin; i++)
	  {
	    if (dstmsgin->type == dstML->type && dstmsgin != srcmsgin &&
		BCMP(MSGSLOT(dstmsgin), MSGSLOT(srcmsgin),
					dstmsgin->nslots*sizeof(Slot)) == 0)
	      {
		DeleteForwMsg(dstElm, dstmsgin);

		OK();
		return 1;
	      }
	    dstmsgin = MSGINNEXT(dstmsgin);
	  }

	Error();
	printf("%s: could not find forwarded %s message between elements\n",
			argv[0], msgName);
	printf("  %s and %s\n", Pathname(srcElm), Pathname(dstElm));
	return 0;

}	/* do_del_forw_msg */


int do_add_msg(argc,argv)
int 		argc;
char 		**argv;
{
char 		*dst_path;
char 		*src_path;
char		*typename;
int		type;
ElementList	*src_list;
ElementList	*dst_list;
Element 	*src_element;
Element 	*dst_element;
short 		slot;
int		nxtarg;
Slot		sarray[NSLOTS];
char		*slotname[NSLOTS];
struct mlist_type	*msgspec;
int		reqslots;
int		i,j,k;
double		number_value[100];
char		*str_value[100];
short		slot_type;

    /*
    ** check the syntax
    */
    initopt(argc, argv, "source-element dest-delement msg-type [msg-fields]");
    if(G_getopt(argc, argv) != 0){
	printoptusage(argc, argv);
	return 0;
    }

    src_path = optargv[1];
    dst_path = optargv[2];
    typename = optargv[3];

    src_list = WildcardGetElement(src_path,0);
    if(src_list->nelements == 0){
	InvalidPath(optargv[0],src_path);
	FreeElementList(src_list);
	return 0;
    }
    dst_list = WildcardGetElement(dst_path,0);
    if(dst_list->nelements == 0){
	InvalidPath(optargv[0],dst_path);
	FreeElementList(src_list);
	FreeElementList(dst_list);
	return 0;
    }

    slot = 0;
    nxtarg = 3;
    /*
    ** get the message specification information from the argument list
    */
    while(++nxtarg < optargc){
	if(optargv[nxtarg][0] == '*'){
	    if(slot >= NSLOTS){
		Error();
		printf("exceeded the maximum number of data slots\n");
	    } else { 
		str_value[slot] = optargv[nxtarg]+1;
		AssignSlotFunc(sarray+slot,STRING);
		slotname[slot] = "_string";
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
		str_value[slot] = optargv[nxtarg]+1;
		AssignSlotFunc(sarray+slot,STRING);
		slotname[slot] = "_string";
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
		slotname[slot] = optargv[nxtarg];
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
		slotname[slot] = "_number";
		slot++;
	    }
	} else {
	    Error();
	    printoptusage(argc, argv);
	    return 0;
	}
    }
    for(k=0;k<dst_list->nelements;k++){
	dst_element = dst_list->element[k];
	/* 
	** get the msg type 
	*/
	if((msgspec = GetMsgListByName(dst_element->object,typename)) == NULL){
	    Error();
	    printf("%s is not a valid msg type for '%s'\n",
	    typename,
	    Pathname(dst_element));
	    return 0;
	}

	type = msgspec->type;
	reqslots = msgspec->slots;

	/*
	** check the number of slots filled against the number of
	** slots required
	*/
	if(slot != reqslots){
	    Error();
	    printf("msg type '%s' requires %d field%s.\n",
	    typename,reqslots, reqslots == 1 ? "" : "s");
	    return 0;
	}
	for(j=0;j<src_list->nelements;j++){
	    src_element = src_list->element[j];
	    /*
	    ** assign the element specific information to the message
	    */
	    for(i=0;i<reqslots;i++){
		if(slotname[i] && (strcmp(slotname[i],"_string") == 0)){
		    sarray[i].data = CopyString(str_value[i]);
		} else 
		if(slotname[i] && (strcmp(slotname[i],"_number") == 0)){
		    sarray[i].data = (char *)malloc(sizeof(double));
		    *((double *)(sarray[i].data)) = number_value[i];
		} else {
		    int	prot;

		    prot = GetFieldListProt(src_element->object, slotname[i]);
		    if (prot == FIELD_HIDDEN)
		      {
			Error();
			printf("%s: could not find field '%s' on '%s'\n",
				optargv[0], slotname[i], Pathname(src_element));
			return 0;
		      }

		    if((
		    sarray[i].data = 
		    GetFieldAdr(src_element,slotname[i],&slot_type)
		    ) == NULL){
			if((sarray[i].data = (char *) GetExtFieldAdr(
						    src_element,
						    slotname[i],
						    &sarray[i].func)) == NULL){
			    Error();
			    printf("%s: could not find field '%s' on '%s'\n",
			    optargv[0],
			    slotname[i],
			    Pathname(src_element));
			    return 0;
			}
		    }
		    else
			AssignSlotFunc(sarray+i,slot_type);
		}
	    }
	    AddMsg(src_element,dst_element,type,slot,sarray);
	}
    }

    FreeElementList(src_list);
    FreeElementList(dst_list);
    OK();
    return 1;
}

void AddMsg(src_element,dst_element,type,nslots,sarray)
Element		*src_element;
Element		*dst_element;
int		type;
int		nslots;
Slot		*sarray;
{
    Msg*	msg;
    Action	action;

    msg = AllocMsg(dst_element, src_element, type, sarray, nslots);
    if (msg == NULL) {
	Error();
	printf("could not add message to '%s'\n",
	Pathname(dst_element));
	return;
    }

    LinkMsgIn(msg);
    LinkMsgOut(msg);

    action.name = "ADDMSGIN";
    action.type = ADDMSGIN;
    action.data = (char*) msg;
    action.argc = 0;
    action.argv = NULL;
    CallActionFunc(dst_element, &action);

    action.name = "ADDMSGOUT";
    action.type = ADDMSGOUT;
    action.data = (char*) msg;
    action.argc = 0;
    action.argv = NULL;
    CallActionFunc(src_element, &action);
}

char *do_getmsg(argc,argv)
int argc;
char **argv;
{
Element *element;
int msgnum=0;
int slotnum=0;
Msg	*msgin = NULL;
int	incoming;
int	action;
int	status;
char *srcelem = NULL;
char *msgtype = NULL;
char *strmsgvalue;

    incoming = 1;
    action = -1;
    initopt(argc, argv, "element -incoming -outgoing -slot msg-number slot-number -count -type msg-number -destination msg-number -source msg-number -find srcelem type");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-count") == 0)
	    action = 0;
	else if (strcmp(G_optopt, "-slot") == 0)
	  {
	    action = 5;
	    msgnum = atoi(optargv[1]);
	    slotnum = atoi(optargv[2]);
	  }
	else if (strcmp(G_optopt, "-type") == 0)
	  {
	    action = 1;
	    msgnum = atoi(optargv[1]);
	  }
	else if (strcmp(G_optopt, "-destination") == 0)
	  {
	    action = 2;
	    msgnum = atoi(optargv[1]);
	  }
	else if (strcmp(G_optopt, "-source") == 0)
	  {
	    action = 3;
	    msgnum = atoi(optargv[1]);
	  }
	/* new option added by Upi */
	else if (strcmp(G_optopt, "-find") == 0) {
		action = 4;
		srcelem = optargv[1];
		msgtype = optargv[2];
	}
	else if (strcmp(G_optopt, "-outgoing") == 0)
	    incoming = 0;
	else if (strcmp(G_optopt, "-incoming") == 0)
	    incoming = 1;
      }

    if (status < 0 || action < 0)
      {
	printoptusage(argc, argv);
	printf("\t[One of -slot, -count, -type, -destination, -source or -find must be given]\n");
	return NULL;
      }

    if((element = GetElement(optargv[1])) == NULL){
	InvalidPath(optargv[0],optargv[1]);
	return(NULL);
    }

    if (action == 0) /* -count */
      {
	if(incoming)
	    return(itoa(element->nmsgin));
	else /* !incoming ==> outgoing */
	    return(itoa(element->nmsgout));
      }

/* This protective check prevents stupid error messages with
** the -find option -- Upi */
  if (action != 4) {
    if(incoming){
	if((msgin = GetMsgInByNumber(element,msgnum)) == NULL){
	    Error();
	    printf("could not get IN msg #%d from '%s'\n",
	    msgnum,
	    Pathname(element));
	    return(NULL);
	}
    } else /* !incoming ==> outgoing */ {
	if((msgin = GetMsgOutByNumber(element,msgnum)) == NULL){
	    Error();
	    printf("could not get OUT msg #%d from '%s'\n",
	    msgnum,
	    Pathname(element));
	    return(NULL);
	}
    }
  }

    /*
    ** check the field to return
    */
    switch (action)
      {
      case 1: /* -type */
	if(msgin && msgin->dst)
	    return(
	    CopyString(GetMsgListByType(msgin->dst->object,msgin->type)->name));
	break;
      case 2: /* -destination */
	if(msgin->dst)
	    return(CopyString(Pathname(msgin->dst)));
	break;
      case 3: /* -source */
	if(msgin)
	    return(CopyString(Pathname(msgin->src)));
	break;
	  case 4: /* -find */ {
		Element *theelem = GetElement(srcelem);
		if (!theelem) {
			InvalidPath(optargv[0],optargv[1]);
			return(NULL);
   		}
		return(itoa(GetMsgNumBySrcDest(theelem,element,msgtype)));
	  }
      case 5: /* -slot */
	if (slotnum < 0 || msgin->nslots <= slotnum)
	  {
	    Error();
	    printf("%s: slot number is out of range\n", argv[0]);
	    return(NULL);
	  }

	strmsgvalue = GetStringMsgData(msgin, slotnum);
	if (strmsgvalue != NULL)
	    return CopyString(strmsgvalue);
	else
	  {
	    char	buf[50];

	    sprintf(buf, "%g", MSGVALUE(msgin, slotnum));
	    return CopyString(buf);
	  }
	/* can't get here!!! */
    }

    return(NULL);
}

/* Returns -1 if not found, otherwise the message number */
/* The argument 'msgtype' is a string with the message type. If
** it is 'all' then the any message will do. */ 
int GetMsgNumBySrcDest(src,dest,msgtype)
	Element *src;
	Element *dest;
	char	*msgtype;
{
	MsgList *msgspec;
	int mtype;
	Msg	*msg = dest->msgin;
	int foundit = 0;
	int	msgcount;

	if (strcmp(msgtype,"all") == 0) {
		for(msgcount = 0; msgcount < dest->nmsgin; msgcount++) {
			if (msg->src == src) {
				foundit = 1;
				break;
			}
			msg = MSGINNEXT(msg);
		}
	} else if ((msgspec = GetMsgListByName(dest->object,msgtype))
		== NULL) {
		Error();
		printf("'%s' not a valid msgtype for %s\n",
			msgtype,Pathname(dest));
	} else {
		mtype = msgspec->type;
		for(msgcount = 0; msgcount < dest->nmsgin; msgcount++) {
			if (msg->src == src && mtype == msg->type) {
				foundit = 1;
				break;
			}
			msg = MSGINNEXT(msg);
		}
	}
	if (foundit) return(msgcount);
	return(-1);
}
