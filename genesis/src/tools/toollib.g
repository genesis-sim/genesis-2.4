//genesis
//
// $Id: toollib.g,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $
// $Log: toollib.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:33  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.10  1999/11/28 07:33:19  mhucka
// Removed old parameter search functions, now that Mike Vanier's library
// is available.
//
// Revision 1.9  1998/07/22 00:06:09  dhb
// Moved randfield.c from oldconn personal library
//
// Revision 1.8  1995/03/15 01:38:32  dhb
// calcCm, calcRm and rallcalcRm were all returning char* and would
// return a static string in error cases which resulted in script
// code trying to free the string.  Changed the return types for
// these commands to float, since they always returned a float value
// as a char string.
//
// Revision 1.7  1995/02/02  00:37:43  dhb
// Added return types to addfunc statements which needed them (e.g.
// calcRm, rallcalcRm, calcCm, expfil, spikeparms, randcomp).
//
// Revision 1.6  1994/09/16  23:35:52  dhb
// Moved volume_connect command to oldconn
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

// The cell reading utility
addfunc 	readcell 	do_read_cell

// The cell writing utility
addfunc 	writecell 	do_write_cell

// A utility for positioning the dendrites of a cell based on 
// messages rather than parent-child relationships
addfunc 	relposition 	do_rel_position

// A utility for randomising the coords of a cell
addfunc 	randcoord 	do_rancoord

// A way of creating 3-d surfaces of cells. Options permit 
// automatic orientation, randomisation, and curving the dendrites 
// of the cells to match the surface
addfunc 	cellsheet 	do_cell_sheet

// A generalised function for doing 3-d messages. It finds the dist
// between the source and dest elements in 3-d space, and if they
// are close enough, sends the message.
addfunc 	gen3dmsg 	do_gen_3d_msg

// A specialised function for doing 3-d messaging between dendrites.
// It finds the shortest distance between
// the DENDS of the source and dest channels, and if this is
// within the specified range it sends the message. For use in 
// dd-synapses

addfunc 	dd3dmsg		 do_dd_3d_msg

// A function for doing rotations of cells in 3-d. Lots of options.
addfunc 	rotcoord 	do_rotate

// A function for getting cell input res using Rall's calculations
addfunc 	rallcalcRm 	do_rall_calc_Rm	float

// A function for getting cell input res using the compartment model
addfunc 	calcRm 		do_calc_Rm	float


// A function for getting approx cell capacitance using simple summing
// Clearly, there is no simple Cm since there are multiple time 
// constants in the cell.
addfunc 	calcCm 		do_sumCm	float

// A function for generating a set of points on an ellipsoid (egg)
// It tries to get fairly even spacing.
addfunc 	egg 		do_egg

// a function for generating a set of points on a plane
addfunc 	plane 		do_plane

// A utility for pasting a channel to a compt, handling all messages
addfunc 	pastechannel 	do_paste_channel

// A utility for randomly selecting a compartment
addfunc 	randcomp 	do_rand_comp	char*

//Set a field to a randomly selected value
addfunc		setrandfield	do_random_field
