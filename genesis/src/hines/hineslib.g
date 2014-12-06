//genesis

/* version EDS20i 95/06/08 */

//
// $Id: hineslib.g,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $
// $Log: hineslib.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:32  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.5  1999/10/17 21:39:37  mhucka
// New version of Hines code from Erik De Schutter, dated 30 March 1999
// and including an update to hines.c from Erik from June 1999.  This version
// does not include changes to support Red Hat 6.0 and the Cray T3E, which
// will need to be incorporated in a separate revision update.
//
// EDS22d revison: EDS BBF-UIA 98/05/05-98/06/18
// Added getsolvechildname
//
// Revision 1.3  1997/07/23 21:56:39  dhb
// Added HPROCESS action.
//
// Revision 1.2  1997/05/28 22:59:47  dhb
// Replaced with version from Antwerp GENESIS 21e
//
// Revision 1.1  1992/12/11  19:03:17  dhb
// Initial revision
//

newclass	hsolver
addaction DUPLICATE 202
addaction HPUT 300
addaction HGET 301
addaction HRESTORE 302
addaction HSAVE 303
addaction HCHANGE 304
addaction HPROCESS 250
addaction HSEVENT 251

//addfunc read_hines do_read_hines
addfunc findsolvefield do_findsolvefield char*
addfunc getsolvecompname do_solvecompname char*
addfunc getsolvechildname do_solvechildname char*
/*
******************************************************************************
**                           hsolve OBJECT TABLE                            **
******************************************************************************
*/
//============================================================================
//	name		data		function	class		
//============================================================================

object	hsolve	hsolve_type	HinesSolver	hsolver	\
	-author 	"U.S.Bhalla and E. De Schutter Caltech 91-93, E. De Schutter BBF-UIA 94-96" \
	-actions	RESET DELETE PROCESS CREATE SETUP DUPLICATE HPUT HGET HRESTORE HSAVE HCHANGE HSEVENT SAVE2 RESTORE2\
	-description	"uses Hines' method to solve cells"
