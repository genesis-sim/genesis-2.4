//genesis
//
// $Id: toolconnlib.g,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $
// $Log: toolconnlib.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:34  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.3  1995/02/02 01:27:03  dhb
// Removed redundant addfunc commands from the main tools directory.
//
// Revision 1.2  1995/01/05  18:02:33  venkat
// Used "//" to comment log message instead of "#" to
// avoid syntax error when making 2.0.8.
//
// Revision 1.1  1994/09/23  16:16:27  dhb
// Initial revision
//
// Revision 1.5  1994/08/08  22:25:59  dhb
// Changes from Upi.
//
// Revision 1.5  1994/06/13  22:48:09  bhalla
// Added rmsmatch function
//
// Revision 1.4  1994/03/21  02:18:46  dhb
// Changed volumeconnect back to 1.4 name (e.g. volume_connect).
//
// Revision 1.3  1993/02/24  00:10:02  dhb
// Fixed addfunc for readcell command (was using read_cell).
//
// Revision 1.2  1993/01/28  18:29:01  snehal
// Jan 27, 1993. changes to Genesis commands. Typed by Snehal.
//
// Revision 1.1  1992/12/11  19:06:21  dhb
// Initial revision
//

//genesis

// A function for connecting cells based on their 3-d positions.
addfunc 	volume_connect 	do_volume_connect
