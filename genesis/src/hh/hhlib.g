//genesis
//
// $Id: hhlib.g,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $
// $Log: hhlib.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:34  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.2  1996/10/29 00:02:55  dhb
// Added SAVE2/RESTORE2 actions for hh_channel vgate and vdep_channle
// objects
//
// Revision 1.1  1992/12/11  19:03:05  dhb
// Initial revision
//

//genesis
newclass	channel
newclass	membrane
newclass	gate

/*
******************************************************************************
**                   Hodgkin-Huxley OBJECT TABLE                            **
******************************************************************************
*/
//============================================================================
//	name		data		function	class		
//============================================================================

addaction CALC_MINF 100
addaction CALC_ALPHA 101
addaction CALC_BETA 102

/*
** Upi Bhalla Jul 1991 : Cleaned out integ-method messages
*/

object	hh_channel	hh_channel_type	hh_channel	segment	channel	\
	-author		"M.Nelson Caltech 8/88" \
	-actions	INIT RESET PROCESS CHECK \
			SAVE2 RESTORE2 \
			*CALC_MINF hh_channel_CALC_MINF \
			*CALC_ALPHA hh_channel_CALC_ALPHA \
			*CALC_BETA hh_channel_CALC_BETA \
	-messages	VOLTAGE 0	1 Vm 

object	vdep_gate	vdep_gate_type	VDepGate	segment	gate	\
	-author		"M.Wilson Caltech 2/89" \
	-actions	RESET \
			PROCESS \
			SAVE2 RESTORE2 \
			*CALC_MINF VDepGate_CALC_MINF \
			*CALC_ALPHA VDepGate_CALC_ALPHA \
			*CALC_BETA VDepGate_CALC_BETA \
	-messages	VOLTAGE 0	1 voltage \
			EREST 	2	1 Erest

object	vdep_channel	vdep_channel_type VDepChannel segment channel \
	-author		"M.Wilson Caltech 2/89" \
	-actions	INIT RESET PROCESS CHECK \
			SAVE2 RESTORE2 \
	-messages	VOLTAGE 0	1 voltage \
			MULTGATE 1	2 gate_state power \
			ADDGATE 2	2 gate_state power \
			GMAX 3		1 gmax \
			EK 4		1 Ek \
	-description	"The activation of the channel corresponds to" \
			"the voltage across it. Computes channel current Ik."

/* lookup table implementation of Hodgkin-Huxley 

object	HH_channel	HH_channel_type	HH_channel	segment	channel	\
	-author		"M.Wilson Caltech 8/88" \
	-actions	INIT RESET PROCESS CHECK

object	HH_compartment	HH_compartment_type HH_compartment segment membrane \
	-author		"M.Wilson Caltech 8/88" \
	-actions	INIT RESET PROCESS CHECK
*/
