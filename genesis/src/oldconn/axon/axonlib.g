//genesis
//
// $Id: axonlib.g,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $
// $Log: axonlib.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:34  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.3  1994/10/20 22:01:38  dhb
// Added CREATE and COPY actions to axon and axonlink.
//
// Revision 1.2  1994/06/15  20:56:20  dhb
// Added CHECK action to axonlink
//
// Revision 1.1  1992/12/11  19:02:32  dhb
// Initial revision
//

// genesis
/*
*************************************************************************
**                         AXON OBJECT TABLE                           **
*************************************************************************
*/
//==========================================================================
//	name		data            function	class	
//==========================================================================

object	axon		delay_projection_type DelayConnection projection \
	-author 	"M.Wilson Caltech 6/88" \
	-actions	CREATE COPY PROCESS RESET RECALC CHECK \
	-messages	BUFFER 0	1 name

object	axonlink	projection_type NoDelayConnection projection	\
	-author 	"M.Wilson Caltech 6/88" \
	-actions	CREATE COPY PROCESS RESET RECALC CHECK \
	-messages	BUFFER 0	1 name

