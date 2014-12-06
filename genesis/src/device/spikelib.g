// genesis

//
// $Id: spikelib.g,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $
//
// $Log: spikelib.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:28  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.3  1998/08/28 15:28:52  dhb
// Removed sumin object.  Use calculator instead.
//
// Revision 1.2  1998/04/01 00:08:30  dhb
// Changed names of doac and doxc to autocorr and crosscorr
// respectively.  Removed the datanal class and changed
// autocorr and crosscorr to use device class instead.
//
// Revision 1.1  1998/03/31 23:35:03  dhb
// Initial revision
//
//
 
/*  Table of data analysis object types   */ 
/*  Also includes spike table object */
 
addaction TABCREATE 200
addaction TABFILL 201
addaction TUPDATE 250
addaction OPEN 300
addaction CLOSE 301
addaction FLUSH 302

object crosscorr  doxc_type  Doxc device  \
	-author "D. Jaeger 8/92"  \
	-actions RESET PROCESS \
	-messages  YSPIKE 1 1 input_2 \
	           XSPIKE 0 1 input_1 \
	-fields  "others : user shouldn't touch"    \
		 "*xcarray : histogram array of size num_bins" \ 
		 "reset-mode : 0 or 1, 1 deletes histogram on RESET" \
		 "threshold : signal amplitude triggering spike event, user must specify"   \
		 "binwidth : in sec, user must specify"    \
		 "num_bins : number of bins, user must specify"  \
	-description "Cross correlation object." \
		"Data are stored in xcarray. Center bin contains zero time delay."
 
object autocorr  doac_type  Doac device  \
        -author "D. Jaeger 8/92"  \
        -actions RESET PROCESS \
        -messages  SPIKE 0 1 input \
        -fields  ""   \
		 "" \
                 ""   \
        -fields  "others : user shouldn't touch"    \
                 "*acarray : histogram array of size num_bins" \
                 "reset-mode : 0 or 1, 1 deletes histogram on RESET" \
                 "threshold : signal amplitude triggering spike event, user must specify"   \
                 "binwidth : in sec, user must specify"    \
                 "num_bins : number of bins, user must specify"  \
        -description "Auto-correlation object. Data are stored in *acarray"
 

object timetable    timetable_type TimetableFunc device \
    -author     "Dieter Jaeger Caltech March 95, revised 7/97" \
    -actions    RESET \
            SET \
            PROCESS \
            TUPDATE \
            TABFILL \
            TABCREATE \
    -fields     "activation : output field " \
            "act_val    : value that activation assumes" \
            "seed       : random seed (0 = no new seed)" \
            "method     : 1 = exponential inter-time interval (iti)" \
			"			: 2 = gamma inter-time interval " \
			"			: 3 = regular inter-time interval" \
			"			: 4 = read activation times from asci file" \
            "meth_desc1 :  method parameter 1 : signifies mean iti" \
            "meth_desc2 :  method parameter 2 : absolute refractory period (methods 1-2)"  \ 
            "meth_desc3 :  method parameter 3 : order of gamma distribution, max. 6 (method 2)" \
			"fname		: file name to load spiktimes from" \
    -description    "A table of ascending simulation time values." \
            "The activation field is set to act_val when" \
            "SimulationTime() matches the next table entry." \
            "Mainly useful for controlled sequences of" \
            "synaptic activation."


object event_tofile event_tofile_type Event_tofile device \
    -author     "Dieter Jaeger Caltech March 95" \
    -actions    RESET \
            PROCESS \
            OPEN \
            CLOSE \
            FLUSH \
    -messages   INPUT   0   1 "input" \
    -fields     "threshold  : input threshold" \    
            "open   : file open flag" \
            "fname  : file name" \
    -description    "Takes input messages and writes current simulation" \
            "time to asci file if input > threshold."
