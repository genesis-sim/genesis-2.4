// genesis
//
// $Id: outlib.g,v 1.2 2006/02/17 07:42:01 svitak Exp $
// $Log: outlib.g,v $
// Revision 1.2  2006/02/17 07:42:01  svitak
// Added float_format field and CREATE action to initialize it.
//
// Revision 1.1.1.1  2005/06/14 04:38:28  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.11  2000/09/11 15:51:24  mhucka
// Fixes to script_out from Hugo Cornelis.
//
// Revision 1.10  2000/03/31 01:14:29  mvanier
// Added extra actions to asc_file object (OUT_OPEN and OUT_WRITE).  These
// allow you to add lines to a file that is being written to by an
// asc_file object without having to worry about buffering problems
// (as you do if you just do writefile, for instance).
//
// Revision 1.9  1999/10/17 01:13:17  mhucka
// Merged in fix from Greg Hood for parameters in par_asc_file and par_disk_out.
// These fixes are from a message from Greg dated from 20 Jan 1999.
//
// Revision 1.8  1997/07/18 20:44:49  dhb
// Added description to spikehistory object.
//
// Revision 1.7  1997/07/18 19:55:38  dhb
// Changes from PSC: parallel versions of ascii and FMT1 I/O objects
//
// Revision 1.6  1997/05/29 08:55:11  dhb
// Update from Antwerp GENESIS version 21e
//
// EDS21b version 96/08/02
// EDS Caltech added res_asc_file object
// MAEX BBF-UIA added spikehistory object
//
// Revision 1.5  1994/10/04  23:01:56  dhb
// Added FLUSH action to asc_file.
//
// Revision 1.4  1994/03/21  02:33:05  dhb
// Removed deleteoutputdata.  Seems obsolete.
//
// Revision 1.3  1993/02/26  17:33:47  dhb
// Removed addfuncs for do_simplot and do_graph_pts.  These already exist
// in widglib.g where they belong.
//
// Revision 1.2  1993/01/28  19:09:21  snehal
// Jan 25, 1993. Changes to Genesis commands. Typed by Snehal.
//
// Revision 1.1  1992/12/11  19:03:33  dhb
// Initial revision
//

// genesis
// version EDS17
newclass	output
addaction       FLUSH     1001
addaction       OUT_OPEN  1002
addaction       OUT_WRITE 1003


/*
*************************************************************************
**                              OUTPUT OBJECTS                         **
*************************************************************************
*/
//==========================================================================
//	name		data            function	class	
//==========================================================================


object	print_out	output_type 	PrintOutput 	output	

object	disk_out	file_type 	FileOutput 	output \
	-author 	"M.Wilson Caltech 6/88" \
	-messages	SAVE 0 		1  data \
	-actions	RESET PROCESS DELETE SAVE

object	par_disk_out	file_type 	ParFileOutput	output \
	-author 	"G.Hood PSC 5/96" \
	-messages	SAVE 0 		2 index data \
	-actions	RESET PROCESS DELETE SAVE

object	xviewdata	xviewfile_type 	XviewFileOutput output \
	-author 	"M.Wilson Caltech 2/89" \
	-messages	SAVE 0 		1  data \
	-actions	RESET PROCESS DELETE SAVE

object	script_out	script_output_type 	ScriptOutput 	output	\
	-author 	"M.Wilson Caltech 2/89" \
	-actions	PROCESS RESET RECALC SET\
 	-readwrite	command "Command to execute when the object is scheduled" \
 	-readonly	argc "Internal use only" \
 	-readonly	argv "Internal use only" \
	-description	"executes a script command during a simulation"

object	asc_file	ascfile_type 	AscFileOutput 	output \
	-author 	"M.Wilson Caltech 4/89" \
	-messages	SAVE 0 		1  data \
	-fields		notime \
	-actions	CREATE RESET PROCESS DELETE SAVE FLUSH OUT_OPEN OUT_WRITE

object	par_asc_file	ascfile_type 	ParAscFileOutput	output \
	-author 	"G.Hood PSC 5/96" \
	-messages	SAVE 0 		2 index data \
	-fields		notime \
	-actions	CREATE RESET PROCESS DELETE SAVE FLUSH

object	res_asc_file	res_ascfile_type 	ResAscFileOutput 	output \
	-author 	"E. De Schutter Caltech 11/92" \
	-messages	SAVE 0 		1  data \
	-fields		time_res \
				value_res \
	-actions	CREATE RESET PROCESS DELETE SAVE \
	-description "asc_file that outputs points separated by at least"\
				 "time_res (X-axis) or value_res (Y-axis)"

object  spikehistory    spikehistory_type       SpikeHistory    output \
        -author         "R. Maex BBF-UIA 1/96" \
        -messages       SPIKESAVE -1            0  \
        -actions        RESET EVENT SAVE FLUSH \
	-description "Records the simulation time and a neuron identification"\
		     "label into an ascii file each time a spike event is"\
		     "received."
 

/*
*************************************************************************
**                     OUTPUT SHELL FUNCTIONS                          **
*************************************************************************
*/
addfunc         asciidata               do_ascii_convert
//
// deleteoutputdata seems to be out of date and can cause core dumps
// when applied to elements of class output which are not based on the
// OUTPUT_TYPE element structure.  I'm leaving this here in case anyone
// misses this command and wants to know why it was removed.
//
// addfunc         deleteoutputdata        do_delete_output_data
