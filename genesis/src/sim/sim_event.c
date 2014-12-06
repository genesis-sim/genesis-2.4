static char rcsid[] = "$Id: sim_event.c,v 1.3 2005/07/20 20:02:03 svitak Exp $";

/*
** $Log: sim_event.c,v $
** Revision 1.3  2005/07/20 20:02:03  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/06/27 19:00:59  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.7  2002/01/16 03:30:43  mvanier
** Some code cleanup which was done while investigating a possible bug
** (which doesn't appear to actually be a bug after all).
**
** Revision 1.6  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.4  2000/06/12 04:52:44  mhucka
** Added alternate printf statement for when FLOATTIME is defined.
**
** Revision 1.3  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/22 23:53:47  dhb
** 1.4 to 2.0 command argument changes.
**
** 	do_insert_event (putevent) changed to use GENESIS getopt routines.
**
 * Revision 1.1  1992/10/27  18:57:23  dhb
 * Initial revision
 *
*/

#include <math.h>
#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

int	DEBUG_ExpandBuffer = 0;
int	DEBUG_InsertEvent = 0;
int	DEBUG_AppendEvent = 0;
int	DEBUG_PutEvent = 0;
int	DEBUG_IncrementEnd = 0;
int	DEBUG_GetEventList = 0;

void ClearBuffer(buffer)
Buffer		*buffer;
{
	buffer->start = buffer->end = buffer->current = 0;
}

void ResetBuffer(buffer)
Buffer		*buffer;
{
    buffer->current = buffer->start;
}

int AddEventBuffer(buffer,size)
Buffer 		*buffer;
int		size;
{
Event 		*new_event;
int 		event_size;

    ClearBuffer(buffer);
    /*
    ** allocate the new event buffer
    */
    event_size = buffer->event_size;
    if((new_event = (Event *)calloc(size,event_size)) == NULL){
	Error();
	printf(" unable to create a new event buffer for '%s'\n",
	Pathname(buffer));
	buffer->size = 0;
	return(0);
    }
    /*
    ** assign the new buffer
    */
    buffer->event = new_event;
    buffer->size = size;
    return(1);
}

/*
** copy an event from the src to the dst
*/
void ReplaceEvent(buffer,src,dst)
Buffer 		*buffer;
Event 		*src;
Event 		*dst;
{
    BCOPY(src,dst, buffer->event_size);
}

/*
** Increase the size of a buffer preserving its previous contents.
*/

int ExpandBuffer(buffer, buffer_increase)
  Buffer *buffer;
  int     buffer_increase;
{
    Event *new_buffer;
    Event *old_buffer;
    int    old_buffer_ptr;
    int    new_buffer_ptr;
    int    current_offset;

    /*
    ** Allocate the new event buffer.
    */

    new_buffer = (Event *) calloc((buffer->size + buffer_increase),
				  buffer->event_size);

    /*
    ** Did the allocation succeed?
    */

    if (new_buffer != NULL)
    {
	old_buffer = buffer->event;

	/*
	** If the old buffer is not empty then copy its contents into
	** the new buffer.
	*/

	old_buffer_ptr = buffer->start;
	new_buffer_ptr = 0;

	if (old_buffer != NULL)
	{
	    /*
	    ** Copy all events between the start and end locations
	    ** of the old buffer to the new buffer.
	    */

	    while (old_buffer_ptr != buffer->end)
	    {
		/*
		** Copy one event.
		*/

		BCOPY(&(old_buffer[old_buffer_ptr]),
		      &(new_buffer[new_buffer_ptr]),
		      buffer->event_size);

		/*
		** Increment the buffer indices.
		*/

		old_buffer_ptr = (old_buffer_ptr + 1) % buffer->size;
		new_buffer_ptr++;
	    }

	    /*
	    ** Copy the buffer->end event.
	    */

	    BCOPY(&(old_buffer[old_buffer_ptr]),
		  &(new_buffer[new_buffer_ptr]),
		  buffer->event_size);

	    /*
	    ** free the old buffer
	    */

	    if (buffer->event != NULL)
	    {
		free(buffer->event);
	    }
	}

	/*
	** calculate the offset of the current location from the
	** starting location in the old buffer
	*/

	if (buffer->current >= buffer->start)
	{
	    current_offset = buffer->current - buffer->start;
	} 
	else 
	{
	    current_offset = buffer->size - (buffer->start - buffer->current);
	}

	/*
	** Set the new buffer pointers.
	*/

	buffer->end      = new_buffer_ptr;
	buffer->current  = current_offset;
	buffer->start    = 0;
	buffer->size    += buffer_increase;

	/*
	** Set up the new buffer.
	*/

	buffer->event = new_buffer;

	if (Debug(DEBUG_ExpandBuffer) > 1) 
	{
	    printf("buffer expansion successful.\n");
	}

	return 1;
    } 
    else 
    {
	if (Debug(DEBUG_ExpandBuffer) > 1) 
	{
	    printf("buffer expansion failed.\n");
	}

	return 0;
    }
}

void IncrementEnd(buffer,mode)
  Buffer *buffer;
  short   mode;
{
    int new_end;

    /*
    ** Increment the buffer end pointer.
    */

    if (buffer->size == 0)
    {
	/*
	** Attempt to expand it.
	*/

	if (!ExpandBuffer(buffer, BUFFER_INCREASE_HINT))
	{
	    /*
	    ** If for some reason the buffer cannot
	    ** be expanded then just return.
	    */

	    Error();
	    printf("overflow in buffer %s\n", Pathname(buffer));
	    return;
	}
    }

    new_end = (buffer->end + 1) % buffer->size;

    /*
    ** If the end == the start then the buffer is full.
    */

    if (new_end == buffer->start)
    {
	/*
	** Buffer either needs to be expanded (mode=EXPAND)
	** or the starting location must be increased.
	** the oldest event must be overwritten (mode=WRAP)
	** buffer needs to be expanded
	*/

	if (mode == EXPAND)
	{
	    if (Debug(DEBUG_IncrementEnd) > 0)
	    {
		Warning();
		printf("buffer '%s' is full\n",
		Pathname(buffer));
		printf("Attempting to expand buffer\n");
	    }

	    /*
	    ** Attempt to expand it.
	    */

	    if (!ExpandBuffer(buffer, BUFFER_INCREASE_HINT))
	    {
		/*
		** If for some reason the buffer cannot
		** be expanded then just return.
		*/

		Error();
		printf("overflow in buffer %s\n",
		Pathname(buffer));
		return;
	    }

	    /*
	    ** Set the new end using the new buffer pointers
	    ** changed by expansion.
	    */

	    new_end = (buffer->end + 1) % buffer->size;
	} 
	else if(mode == WRAP)
	{
	    /*
	    ** Just move up the start pointer and thereby lose
	    ** the oldest event.
	    */

	    /*
	    ** should check tmin to determine whether this
	    ** event could still be accessed which would
	    ** indicate an error
	    */
#ifdef FLOATTIME
	    if ((SimulationTime() 
		 - buffer->event[(buffer->start+1) 
				% buffer->size].time) 
		< buffer->tmin + CORRECTION)
	    {
#else
	    if ((round(SimulationTime() / Clockrate(buffer)) 
		 - buffer->event[(buffer->start+1) 
				% buffer->size].time)
		< buffer->tmin)
	    {
#endif
		/*
		** Try to expand instead.
		*/

		if (Debug(DEBUG_IncrementEnd) > 0)
		{
		    Warning();
		    printf("buffer '%s' is full. ", 
		    Pathname(buffer));
		    printf("Attempting to expand...\n");
		}

		/*
		** Attempt to expand it.
		*/

		if (!ExpandBuffer(buffer, BUFFER_INCREASE_HINT))
		{
		    /*
		    ** if for some reason the buffer cannot
		    ** be expanded then just do the wrap
		    */
		    Error();
		    printf("overflow in buffer '%s'\n", 
		    Pathname(buffer));
		}

		/*
		** Set the new end using the new buffer pointers
		** changed by expansion.
		*/

		new_end = (buffer->end + 1) % buffer->size;
	    }

	    buffer->start = (buffer->start + 1) % buffer->size;
	}
    }

    buffer->end = new_end;
}


/*
** inserts events into a buffer according to its time stamp
**
** the mode affects how collisions of the current buffer location
** and the buffer starting location are handled. The two modes
** are:
**   WRAP     which maintains the same buffer size and simply
**            increases the starting location to make room for
**            the new event and therefore overwrites the oldest
**            event in the buffer.
**   EXPAND   which will not overwrite any events and instead
**            expands the buffer by some amount to make room
**            for the new event
**
** normally InsertEvent would be called with the mode of EXPAND
** since this routine would normally be used to setup a buffer and
** therefore dynamically establish the buffer size without
** destroying previous insertions
*/
int InsertEvent(buffer,event,mode)
Buffer 		*buffer;
Event		*event;
short 		mode;
{
int 		insert_point;
Event 		*bufptr;
int		bufsize;
#ifdef FLOATTIME
float		event_time;
float		error_correction;
#else
int		event_time;
#endif
int		start;
int		current;
int 		end;
int 		offset;
int 		to_index;
int 		from_index;

    if(buffer == NULL){
	return(0);
    }
    if(Debug(DEBUG_InsertEvent) > 2){
	printf("InsertEvent buffer '%s' mode %d\n", Pathname(buffer), mode);
    }
#ifdef FLOATTIME
    error_correction = CORRECTION;
#endif
    /*
    ** the only way that start == end is if the buffer is empty
    ** if that is the case then bump the end pointer and put
    ** in the event
    */
    if(buffer->start == buffer->end){
	IncrementEnd(buffer,mode);
	ReplaceEvent(buffer,event,&(buffer->event[buffer->end]));
	return(1);
    }
    event_time = event->time;
    /*
    ** set bufptr to the event list of the buffer
    */
    bufptr = buffer->event;
    /*
    ** end points to the last event
    */
    end = buffer->end;
    current = buffer->current;
    bufsize = buffer->size;
    /*
    ** start points to one location before the first event
    */
    start = buffer->start;
    /*
    ** start the search for the insertion point at the beginning
    */
    insert_point = (start + 1) % bufsize;
    /* 
    ** skip to the insert location 
    ** by finding the first buffer event which has a time 
    ** greater than or equal to the event being inserted
    */
    /*
    ** check the time of the event to be inserted against
    ** the time of the event currently pointed to in the
    ** event buffer.
    **
    ** check to see that the search does not go
    ** past the last defined buffer event
    **
    ** Since events are stored in increasing time order
    ** if the time of the event to be inserted is less than
    ** or equal to the current buffer event time then
    ** the event belongs either just before (if less than)
    ** or on top of (if equal) the current buffer event
    */
#ifdef FLOATTIME
    while((insert_point != end) 
    && (event_time > bufptr[insert_point].time + error_correction)){
#else
    while((insert_point != end) 
    && (event_time > bufptr[insert_point].time)){
#endif
	/*
	** while the appropriate slot has not been found
	*/
	/*
	** increment the buffer pointer
	** checking for wraparound
	*/
	insert_point = (insert_point + 1) % bufsize;
    }
    /*
    ** if the time stamp of the new event and the current buffer
    ** event are the same then simply replace the buffer event with
    ** the new event
    **
    ** the buffer pointers do not need to be changed
    */
#ifdef FLOATTIME
    if(fabs(event_time - bufptr[insert_point].time) < error_correction){
#else
    if(event_time == bufptr[insert_point].time){
#endif
	ReplaceEvent(buffer,event,&(bufptr[insert_point]));
	return(1);
    }
    /*
    ** if the time stamp of the new event is less than the current buffer
    ** event then the new event must be inserted before the buffer event
    **
    ** the buffer pointers need to be updated
    */
#ifdef FLOATTIME
    if(event_time < bufptr[insert_point].time - error_correction){
#else
    if(event_time < bufptr[insert_point].time){
#endif
	/*
	** calculate the offset of the insert point from the end
	*/
	if(insert_point <= end){
	    offset = end - insert_point;
	} else {
	    offset = end + bufsize - insert_point;
	}
	/*
	** increment the end pointer with the appropriate mode
	*/
	IncrementEnd(buffer,mode);
	/*
	** reset the buffer pointers as they may have been changed
	** by buffer resizing in IncrementEnd
	*/
	bufptr = buffer->event;
	start = buffer->start;
	end = buffer->end;
	current = buffer->current;
	bufsize = buffer->size;
	/*
	** calculate the new insertion point by subtracting
	** the offset from the new end (taking the increment
	** into account) using modulo addition with bufsize 
	** to perform the subtraction
	*/
	insert_point = (end + bufsize - offset - 1) % bufsize;
	/*
	** the current location must be compared with the insert
	** point to determine whether it needs to be modified or
	** not.
	** inserting an event before the current location is
	** quite peculiar and will be flagged
	*/
	if(current <= end){
	    /*
	    ** if the insertion point offset is greater than
	    ** the current offset then flag it
	    */
	    if(offset >= end - current){
		Warning();
		printf("event insertion prior to current taking place\n");
		buffer->current = (buffer->current + 1) % buffer->size;
	    }
	} else {
	    if(offset >= end + bufsize - current){
		Warning();
		printf("event insertion prior to current taking place\n");
		buffer->current = (buffer->current + 1) % buffer->size;
	    }
	}
	/*
	** open up a buffer location by shifting existing
	** events over one slot leaving a hole at the
	** insertion point
	*/
	to_index = end;
	while(to_index != insert_point){
	    /*
	    ** decrement the pointer
	    */
	    if((from_index = to_index - 1) < 0 ) from_index = bufsize - 1;
	    /*
	    ** shift the event
	    */
	    BCOPY(&(bufptr[from_index]),&(bufptr[to_index]),
	    buffer->event_size);
	    to_index = from_index;
	}
	/* 
	** and insert the new event
	*/
	ReplaceEvent(buffer,event,&(bufptr[insert_point]));

	/*
	** return successfully
	*/
	return(1);
    }
    /*
    ** if the time stamp of the new event is greater than the current buffer
    ** event then the new event must be inserted after the buffer event
    **
    ** the buffer pointers need to be updated
    */
#ifdef FLOATTIME
    if(event_time > bufptr[insert_point].time + error_correction){
#else
    if(event_time > bufptr[insert_point].time){
#endif
	/*
	** increment the end pointer with the appropriate mode
	*/
	IncrementEnd(buffer,mode);
	/* 
	** and append the new event
	*/
	ReplaceEvent(buffer,event,&(buffer->event[buffer->end]));
    }
    return(1);
}

/* 
** appends an event to the buffer at buffer->end
*/
int AppendEvent(buffer,event,mode)
Buffer 		*buffer;
Event 		*event;
short		mode;
{
#ifdef FLOATTIME
float		delta_t;
float		error_correction;
#else
int		delta_t;
#endif

    if(Debug(DEBUG_AppendEvent) > 2){
	printf("AppendEvent buffer '%s' mode ",
	Pathname(buffer));
	if(mode == WRAP)
		printf("WRAP\n");
	else
	if(mode == EXPAND)
		printf("EXPAND\n");
    }
    if(buffer->size == 0 && mode == WRAP){
	printf("zero sized buffer '%s'\n",
	Pathname(buffer));
	return 0;
    }
    /*
    ** the only way that start == end is if the buffer is empty
    */
    if(buffer->start == buffer->end){
	/*
	** if that is the case then bump the end pointer and
	** put in the event
	*/
	IncrementEnd(buffer,mode);
	ReplaceEvent(buffer,event,&(buffer->event[buffer->end]));
	return(1);
    }
    delta_t = event->time - buffer->event[buffer->end].time;
    /*
    ** if delta_t is greater than zero then add the new
    ** event after the last buffer event
    */
#ifdef FLOATTIME
    error_correction = CORRECTION;
    if(delta_t > error_correction){
#else
    if(delta_t > 0){
#endif
	IncrementEnd(buffer,mode);
    }
    /*
    ** if delta t is less than zero then the event cannot be
    ** appended
    */
#ifdef FLOATTIME
    if(delta_t < -error_correction){
#else
    if(delta_t < 0){
#endif
	Error();
	printf("cannot append event to buffer '%s'\n",
	Pathname(buffer));
	return(0);
    }

    /*
    ** if it is equal then just replace it
    */
    ReplaceEvent(buffer,event,&(buffer->event[buffer->end]));
    return(1);
}

/* 
** adds an event to the buffer at buffer->current
*/
int PutEvent(buffer,magnitude,mode)
Buffer 		*buffer;
double 		magnitude;
short		mode;
{
#ifdef FLOATTIME
float		delta_t;
float		error_correction;
#else
int		delta_t;
#endif
Event 		event;

    if(buffer == NULL) {
	printf("PutEvent : NULL buffer\n");
	return(0);
    }
    if(Debug(DEBUG_PutEvent) > 2){
	printf("PutEvent buffer '%s' mode ",
	Pathname(buffer));
	if(mode == WRAP)
	    printf("WRAP\n");
	else
	if(mode == EXPAND)
	    printf("EXPAND\n");
    }
    if(buffer->size == 0 && mode == WRAP){
	if(Debug(DEBUG_PutEvent) > 0){
	    printf("PutEvent : expanding zero sized buffer '%s'\n",
	    Pathname(buffer));
	}
	/*
	** and attempt to expand it
	*/
	if(!ExpandBuffer(buffer,BUFFER_INCREASE_HINT)){
	    /*
	    ** if for some reason the buffer cannot
	    ** be expanded then just return
	    */
	    Error();
	    printf("overflow in buffer '%s'\n",
	    Pathname(buffer));
	    return(0);
	}
    }
#ifdef FLOATTIME
    event.time = SimulationTime();
#else
    event.time = round(SimulationTime()/Clockrate(buffer));
#endif
    event.magnitude = magnitude;
    /*
    ** the only way that start == end is if the buffer is empty
    */
    if(buffer->start == buffer->end){
	if(buffer->current != buffer->end){
	    Error();
	    printf("illegal current buffer location\n"); 
	    printf("resetting current buffer location to buffer end\n"); 
	}
	/*
	** bump the end and add the event
	*/
	IncrementEnd(buffer,mode);
	ReplaceEvent(buffer,&event,&(buffer->event[buffer->end]));
	/*
	** bump the current location
	*/
	buffer->current = buffer->end;
	return(1);
    }
    delta_t = event.time - buffer->event[buffer->current].time;
    /*
    ** if delta_t is greater than zero then add the new
    ** event after the last buffer event
    */
#ifdef FLOATTIME
    error_correction = CORRECTION;
    if(delta_t > error_correction || buffer->current == buffer->start){
#else
    if(delta_t > 0 || buffer->current == buffer->start){
#endif
	/*
	** are we at the end of the buffer. If so then bump the
	** end pointer
	*/
	if(buffer->current == buffer->end){
		IncrementEnd(buffer,mode);
	}
	/*
	** advance the current pointer
	*/
	buffer->current = (buffer->current + 1) % buffer->size;
    }
    /*
    ** if delta t is less than zero then the event cannot be
    ** added
    */
#ifdef FLOATTIME
    if(delta_t < -error_correction){
#else
    if(delta_t < 0){
#endif
	ErrorMessage("PutEvent","Cannot add event.",buffer);
	return(0);
    }
    /*
    ** if delta t equals zero then just replace the current
    ** event with the new event and dont change any of the
    ** pointers
    */
    ReplaceEvent(buffer,&event,&(buffer->event[buffer->current]));
    return(1);
}

/*
* GetEventList
*
* fills a list with event times beginning at StartT for 
* the specified duration and returns the number of events found
* in that period
*/
int GetEventList (buffer, start_time, duration,list,listsize)
Buffer 		*buffer;
#ifdef FLOATTIME
float		start_time;
float		duration;
float		*list;
#else
int		start_time;
int		duration;
int		*list;
#endif
int		listsize;
{
int		list_cnt;
#ifdef FLOATTIME
float		error_correction;
float		end_time;
#else
int		end_time;
#endif
Event 		*baseptr; 
Event		*curptr; 
Event		*startptr;

    if(Debug(DEBUG_GetEventList) > 1){
#ifdef FLOATTIME
	    printf("GetEventList '%s' %f %f\n",
	    Pathname(buffer),start_time,duration);
#else
	    printf("GetEventList '%s' %d %d\n",
	    Pathname(buffer),start_time,duration);
#endif
    }

    list_cnt = 0;
    if(buffer->start == buffer->end ||
    buffer->start == buffer->current){
	    return(0);
    }
    /* 
    ** search from the starting time for duration
    */
#ifdef FLOATTIME
    error_correction = CORRECTION;
    if ((end_time = start_time - duration) < -error_correction) 
#else
    if ((end_time = start_time - duration) < 0) 
#endif
	end_time = 0;
    /* 
    ** baseptr points to the beginning of the spike history array
    ** and is used in checking for bounds while
    ** walking through the circular history buffer 
    */
    baseptr = buffer->event;
    /* 
    ** load up curptr to point to the most recent spike event 
    */
    curptr = baseptr + buffer->current;
    startptr = baseptr + buffer->start;
#ifdef FLOATTIME
    while ((curptr->time >= end_time - error_correction) 
    &&(list_cnt + 1 < listsize)) {
	if (curptr->time <= start_time + error_correction){ 
#else
    while ((curptr->time >= end_time) 
    &&(list_cnt + 1 < listsize)) {
	if (curptr->time <= start_time){ 
#endif
	    list[list_cnt++] = curptr->time;
	    if(Debug(DEBUG_GetEventList) > 0){
#ifdef FLOATTIME
		printf("event %f\n",curptr->time);
#else
		printf("event %d\n",curptr->time);
#endif
	    }
	}
	if (--curptr < baseptr) 
	    curptr = baseptr + buffer->size -1;
	if (curptr == startptr)
	    break;
    }
    return(list_cnt);
}

/*
** starting at the current location this routines searches the
** event list for an event with a matching time
*/
Event *FindEvent(buffer,time)
Buffer 		*buffer;
#ifdef FLOATTIME
float 		time;
#else
int 		time;
#endif
{
Event 		*event;
Event 		*curptr;
Event 		*startptr;
#ifdef FLOATTIME
float		error_correction;
#endif

    if(buffer == NULL) return(NULL);
    event = buffer->event;
    curptr = event + buffer->current;
    startptr = event + buffer->start;
#ifdef FLOATTIME
    error_correction = CORRECTION;
    while(curptr != startptr){
	if(curptr->time == time){
#else
    while(curptr != startptr){
	if(curptr->time == time){
#endif
	    return(curptr);
	}
	if(--curptr < event)
	    curptr = event + buffer->size -1;
    }
    return(NULL);
}

/*
** starting at the current location this routines searches back
** in event list for the event located at current - rel_position
*/
Event *PreviousEvent(buffer,rel_position)
Buffer		*buffer;
int 		rel_position;
{
Event 		*event;
Event 		*curptr;
Event 		*startptr;

    if(buffer == NULL) return(NULL);
    event = buffer->event;
    curptr = event + buffer->current;
    startptr = event + buffer->start;
    while(curptr != startptr){
	if(rel_position-- == 0){
	    return(curptr);
	}
	if(--curptr < event)
	    curptr = event + buffer->size -1;
    }
    return(NULL);
}

int do_insert_event(argc,argv)
int argc;
char **argv;
{
Buffer 	*buffer;
Event 	event;
int	nxtarg;

    initopt(argc, argv, "buffer-element time amplitude ...");
    if (G_getopt(argc, argv) != 0 || optargc%2 != 0)
      {
	printoptusage(argc, argv);
	return(0);
      }

    if((buffer = (Buffer *)GetElement(optargv[1])) == NULL){
	InvalidPath(optargv[0],optargv[1]);
	return(0);
    }
    if(!CheckClass(buffer,BUFFER_ELEMENT)){
	printf("'%s' is not a buffer\n",Pathname(buffer));
	return(0);
    }
    nxtarg = 2;
    while (optargc > nxtarg+1){
	event.time = Atof(optargv[nxtarg]);
	event.magnitude = Atof(optargv[nxtarg+1]);
	InsertEvent(buffer,&event,EXPAND);
	nxtarg += 2;
    }
    return(1);
}

