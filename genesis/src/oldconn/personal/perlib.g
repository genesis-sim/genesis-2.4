//genesis
//
// $Id: perlib.g,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $
// $Log: perlib.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:34  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.7  1998/07/21 23:40:23  dhb
//  Moved setrandfield to tools library
//
// Revision 1.6  1995/07/28 00:45:27  dhb
// Added affweight and affdelay as commands.
//
// Revision 1.5  1994/03/21  19:23:56  dhb
// Changed connection related commands back to 1.4 names.
//
// Revision 1.4  1993/02/25  18:05:30  dhb
// More 1.4 to 2.0 command name changes (added set to commands that set element
// fields).
//
// Revision 1.3  1993/02/24  22:29:41  dhb
// Removed addfunc for balanceEm since it already exists in seglib.g where
// it belongs.
//
// Revision 1.2  1993/02/24  22:28:13  dhb
// 1.4 to 2.0 command name changes.
//
// Revision 1.1  1992/12/11  19:03:47  dhb
// Initial revision
//

// genesis

/*
************************************************************************
**                      PERSONAL SHELL COMMANDS                     **
************************************************************************
*/
addfunc		expweight		ExpWeight
addfunc		radialdelay		RadialDelay
addfunc		affweight		AffWeight
addfunc		affdelay		AffDelay
addfunc		expsum			ExpSum			float
addfunc		setspatialfield		SpatialDistField
addfunc		normalize_synapses	do_NormalizeSynapses
addfunc		scaleweight		ScaleWeight
