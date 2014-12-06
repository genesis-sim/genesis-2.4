//genesis
//
// $Id: buflib.g,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $
// $Log: buflib.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:28  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.5  1994/09/20 18:31:07  dhb
// Added addfunc for getintput and clearbuffer back from segment library.
// These commands are buffer related so the code has been moved here.
//
// Revision 1.4  1994/03/20  06:42:03  dhb
// Removed obsolete -evenironment option for graded object.
//
// Revision 1.3  1993/02/25  18:13:53  dhb
// Moved addfuncs for do_get_input (getinput) and do_clear_buffer
// (clear_buffer) to seglib.g as these functions are defined there.
//
// Revision 1.2  1993/01/28  18:58:49  snehal
// Jan 25, 1993. Changes to Genesis commands. Typed by Snehal.
//
// Revision 1.1  1992/12/11  19:02:38  dhb
// Initial revision
//

// genesis

addfunc		getinput	do_get_input
addfunc		clearbuffer	do_clear_buffer

/*
*************************************************************************
**                       BUFFER OBJECT TABLE                           **
*************************************************************************
*/
//==========================================================================
//	name		data            function	class	
//==========================================================================

object	graded		graded_type 	GradedEvent  	buffer 	\
	-author 	"M.Wilson Caltech 6/88" \
	-actions 	INIT PROCESS RESET CHECK \
			*COPY BufferCopy *CREATE BufferCreate \
	-fields		rectify baseline scale \
	-messages	INPUT 1		1 input

object	linear		linear_type 	LinearEvent  	buffer 	\
	-author 	"M.Wilson Caltech 6/88" \
	-actions 	PROCESS RESET CHECK *COPY BufferCopy *CREATE BufferCreate \
	-messages	INPUT 0		1 input

object	periodic	periodic_type 	PeriodicEvent  	buffer 	\
	-author 	"M.Wilson Caltech 6/88" \
	-actions 	PROCESS RESET *COPY BufferCopy *CREATE BufferCreate 

object	playback	playback_type 	PlaybackBuffer 	buffer 	\
	-author 	"M.Wilson Caltech 6/88" \
	-actions 	PROCESS RESET *COPY BufferCopy *CREATE BufferCreate 

object	random		random_type 	RandomEvent 	buffer 		\
	-author 	"M.Wilson Caltech 6/88" \
	-actions 	INIT PROCESS RESET *COPY BufferCopy *CREATE BufferCreate \
	-fields		min_amp max_amp rate reset reset_value \
	-messages	RATE 0		1 rate \
			MINMAX 1	2 min max \
	-description	"Generates a time series of events at a rate" \
			"given by the rate parameter. The probability" \
			"of an event for a single time step is given" \
			"by rate*dt where dt is the clock rate of the" \
			"element. When an event has been generated, the" \
			"amplitude of the event is a random variable" \
			"uniformly distributed between min_amp and max_amp." \
			"The state field has the value of the event amplitude" \
			"if an event has been generated. If an event is not" \
			"generated then the value of the state field" \
			"depends on the reset field. If reset is non-zero" \
			"then the state is takes on the value given in" \
			"reset_value. Otherwise the state will behave like a"\
			"latch containing the amplitude of the previous event."

object	sigmoid		sigmoid_type 	SigmoidEvent 	buffer 	segment \
	-author 	"M.Wilson Caltech 6/88" \
	-actions 	PROCESS RESET *COPY BufferCopy *CREATE BufferCreate \
	-fields		thresh amplitude input state gain \
	-messages	INPUT 0		1 input \
			THRESH 1	1 threshold \
			GAIN 2		1 gain \
			AMP 3		1 amplitude \
	-description	"computes the sigmoidal tranformation of the input" \
			"and places the result in 'state'"

object	spike		spike_type 	SpikeEvent 	buffer \
	-author 	"M.Wilson Caltech 6/88" \
	-actions 	PROCESS RESET CHECK \
			*COPY BufferCopy *CREATE BufferCreate \
	-messages	INPUT 0		1 input \
			THRESH 1	1 threshold \
	-description	"Performs threshold spike discrimination." \
			"Generates an impulse each time an input crosses the" \
			"spike threshold at a maximal rate set by abs_refract."

// New object for use with integration methods (BPG 15-5-91) 
object	passive_buffer	passive_type 	StateEvent	buffer \
	-author 	"B. Graham JCSMR,ANU 5/91" \
	-actions	RESET *COPY BufferCopy *CREATE BufferCreate
