//genesis
//
// $Id: outlib.nx.g,v 1.2 2006/02/17 07:42:01 svitak Exp $
// $Log: outlib.nx.g,v $
// Revision 1.2  2006/02/17 07:42:01  svitak
// Added float_format field and CREATE action to initialize it.
//
// Revision 1.1.1.1  2005/06/14 04:38:28  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.3  1999/10/17 01:13:43  mhucka
// Merged in fix from Greg Hood for parameters in par_asc_file and par_disk_out.
// These fixes are from a message from Greg dated from 20 Jan 1999.
//
// Revision 1.2  1997/05/29 08:55:11  dhb
// Update from Antwerp GENESIS version 21e
//
// Revision 1.1  1992/12/11  19:03:34  dhb
// Initial revision
//
// EDS21b version 96/08/02
// EDS Caltech added res_asc_file object
// MAEX BBF-UIA added spikehistory object
//

newclass	output


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
	-fields		command \
	-description	"executes a script command during a simulation"

object	asc_file	ascfile_type 	AscFileOutput 	output \
	-author 	"M.Wilson Caltech 4/89" \
	-messages	SAVE 0 		1  data \
	-fields		notime \
	-actions	CREATE RESET PROCESS DELETE SAVE

object	par_asc_file	ascfile_type 	ParAscFileOutput	output \
	-author 	"G.Hood PSC 5/96" \
	-messages	SAVE 0 		2 index data \
	-fields		notime \
	-actions	CREATE RESET PROCESS DELETE SAVE

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
        -actions        RESET EVENT SAVE FLUSH
 

/*
*************************************************************************
**                     OUTPUT SHELL FUNCTIONS                          **
*************************************************************************
*/
addfunc         asciidata               do_ascii_convert
addfunc         delete_output_data      do_delete_output_data
//addfunc         simplot                 do_simplot
//addfunc         graphpts                do_graph_pts
