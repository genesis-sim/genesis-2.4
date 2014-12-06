//genesis
//
// $Id: synlib.g,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $
// $Log: synlib.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:34  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.1  1992/12/11 19:05:25  dhb
// Initial revision
//

// genesis

newclass terminal
/*
*************************************************************************
**                              OBJECT TABLE                           **
*************************************************************************
*/
//==========================================================================
//	name		data            function	class	
//==========================================================================

object	synapse		connection_type SimpleSynapse 	connection \
	-author 	"M.Wilson Caltech 6/88" \
	-actions	PROCESS \
		        *CHECK CHECK_SimpleSynapse \
			*SHOW SynapseField \
	-fields		weight delay

object	defsynapse	connection_type SimpleSynapse 	connection \
	-author 	"M.Wilson Caltech 6/88" \
	-actions	PROCESS \
			*CREATE CREATE_SimpleSynapse \
			*CHECK CHECK_SimpleSynapse \
			*SHOW SynapseField \
	-fields		weight delay

object	synapseA	synapseA_type 	SynapseA 	connection \
	-author 	"M.Wilson Caltech 6/88" \
	-actions	PROCESS 

object	synapseB	synapseB_type 	SynapseB 	connection \
	-author 	"M.Wilson Caltech 6/88" \
	-actions	PROCESS \
			*RESET RESET_SynapseB
