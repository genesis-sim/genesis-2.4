static char rcsid[] = "$Id: pc_update.c,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $";

/*
** $Log: pc_update.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:04:10  dhb
** Initial revision
**
*/

#include "seg_ext.h"

double PasteOnSpike(element,y)
Element 	*element;
double 		y;
{
Buffer		*buffer;
Event		*event;

    /*
    ** check to see if a spike has occured within the past
    ** 6 msec. If so then paste up a nice looking spike waveform
    */
    /*
    ** get the spike buffer
    */
    buffer = (Buffer *)GetElement("spike");
    if((event = PreviousEvent(buffer,0)) != NULL && 
#ifdef FLOATTIME
    event->time > SimulationTime() - 6.0){
	return( y + (E_SPIKE - y) *
	exp (event->time - SimulationTime()));
#else
    event->time > (int)((SimulationTime() - 6)/global_clock[buffer->clock])){
	return( y + (E_SPIKE - y) *
	exp (event->time*global_clock[buffer->clock] - SimulationTime()));
#endif
    } else {
	return(y);
    }
}

#ifdef LATER

/*
** increment the spike count for the element
*/
UpdateSpikeCount(buffer,mode)
struct spike_type *buffer;
int		mode;
{

    /*
    * compare the current buffer event with the current
    * simulation time
    */
    if( ValidEvent(buffer) &&
#ifdef FLOATTIME
    fabs(CurrentEvent(buffer).time - SimulationTime()) < global_clock[0]/2.0){
#else
    CurrentEvent(buffer).time == SimulationTime()/global_clock[buffer->clock]){
#endif
	buffer->spk_data += 1;
    }
}
#endif
