//genesis
//
// $Id: porelib.g,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $
// $Log: porelib.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:28  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.1  1992/12/11 19:03:54  dhb
// Initial revision
//

//genesis

//*************************************************************************
//Adam Franklin Strassberg
//March 15, 1991
//strass@cns.caltech.edu
//
//Ion Pore Populations represented through Markov Processes
//
//Strassberg and DeFelice, 1992, Neural Computation (in Press)
//Strassberg and DeFelice, 1992, CNS Memo
//*************************************************************************

//*************************************************************************
//Sodium Pore Population Object Type
object	Napores	Na_pore_type	NaPorePop	segment \
		-actions		INIT PROCESS RESET CHECK \
		-messages		VOLTAGE 0	1 Vm \
		-author			"Adam Strassberg" \
		-description		"Sodium pore population represented through eight state Markov kinetic scheme (see Strassberg and DeFelice, 1992)."
//*************************************************************************

//*************************************************************************
//Potassium Pore Population Object Type
object	Kpores	K_pore_type	KPorePop	segment \
		-actions		INIT PROCESS RESET CHECK \
		-messages		VOLTAGE 0	1 Vm \
		-author			"Adam Strassberg" \
		-description		"Potassium pore population represented through five state Markov kinetic scheme (see Strassberg and DeFelice, 1992)."
//*************************************************************************
