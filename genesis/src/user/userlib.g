//genesis
//
// $Id: userlib.g,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $
// $Log: userlib.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:29  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.1  1992/12/11 19:06:27  dhb
// Initial revision
//

//genesis
/*
********************************************************************
**                 USER OBJECT TABLE                              **
********************************************************************
*/

//==================================================================
//	name		data		function	class		
//==================================================================

object  pulsegen        pulsegen_type    PulseGen   device  \
	-author         "M.Nelson Caltech 4/89" \
	-actions        RESET PROCESS \
	-messages       INPUT 0		1 "gate/trig"  \
			LEVEL 1		2 pulse# level \
			WIDTH 2		2 pulse# width \
			DELAY 3		2 pulse# delay \
	-fields         "level1 width1 delay1" \
	                "level2 width2 delay2" \
			"baselevel" \
			"trig_mode: run(0), trig(1), gate(2)" \
			"output" \ 
	-description    "General purpose pulse generator" 

object  PID	        PID_type    PIDcontroller    device  \
	-author         "M.Nelson Caltech 4/89" \
	-actions        INIT RESET PROCESS \
	-messages       CMD 0		1 command \
			SNS 1		1 sense   \
			GAIN 2		1 gain    \
	-fields         "cmd sns output" \
	                "gain tau_i tau_d saturation" \
	-description    "PID (proportional, integral, derivative) feedback controller" 
