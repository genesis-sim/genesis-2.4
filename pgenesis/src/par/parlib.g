// $Id: parlib.g,v 1.1.1.1 2005/06/14 04:38:39 svitak Exp $

// $Log: parlib.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:39  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.1  1999/12/19 03:59:42  mhucka
// Adding PGENESIS from GENESIS 2.1 CDROM release.
//
// Revision 1.13  1997/02/28 05:24:51  ngoddard
// support for lookahead optimisation, bug fixes
//
// Revision 1.12  1997/02/23 04:07:25  ngoddard
// log message for revision 1.11 had no newlines, code_g barfed
//
// Revision 1.11  1997/02/20 21:13:37  ngoddard
// support for xpvm; using pvm_trecv for timeout ops; fixed bugs
// related to futures; provide user warnings when appears stalled; barrer
// and barrierall take a third argument: how long to wait before stall is
// assumed; beginnings of performance stats package
//
// Revision 1.10  1997/01/31 05:14:25  ngoddard
// barriers don't complete asyncs now, minor bug fixes
//
// Revision 1.9  1997/01/21 22:20:24  ngoddard
// paragon port
//
// Revision 1.8  1997/01/21 20:19:47  ngoddard
// bug fixes for zones, implement string literal fields, first release of pgenesis script
//
// Revision 1.7  1996/08/13 21:25:04  ghood
// General debugging to get parallel example scripts working.
//
// Revision 1.6  1996/05/03 19:48:11  ghood
// moving to biomed area
//
//Revision 1.5  1995/07/05  21:21:18  mckeon
//Added skeleton for rvolumeconnect
//
//Revision 1.4  1995/05/10  18:56:48  ngoddard
//before making changes to allow ids on barriers
//
//Revision 1.3  1995/04/26  19:57:14  ngoddard
//commenced allowing out-of-zone setups out of step
//
//Revision 1.2  1995/04/11  04:26:25  ngoddard
//Patch-1-test
//
// Revision 1.1  1995/04/03  22:04:18  ngoddard
// Initial revision
//
// Revision 1.5  1993/12/30  00:06:26  ngoddard
// Release 12/14/93
//
// Revision 1.1  1993/12/29  19:36:02  ngoddard
// Initial revision
//

// genesis

/*
*
************************************************************************
**                       PARLIB SHELL COMMANDS                           **
************************************************************************
*/
addaction ZONING 205

addfunc async			do_async			char*
addfunc waiton			do_take				char*
addfunc raddmsg 		do_remote_add_msg		char*
addfunc rvolumeconnect		do_remote_volume_connect	char*
addfunc rvolumedelay		do_remote_volume_delay		char*
addfunc rvolumeweight		do_remote_volume_weight		char*
addfunc rshowmsg		do_remote_show_msg		char*
addfunc mynode 			do_mynode 			int
addfunc nnodes 			do_nnodes 			int
addfunc myzone 			do_myzone 			int
addfunc nzones 			do_nzones 			int
addfunc ntotalnodes		do_totalnodes			int
addfunc mytotalnode		do_mytotalnode 			int
addfunc mypvmid 		do_realmynode 			int
addfunc npvmcpu 		do_realnnodes 			int
addfunc rcommand 		do_remote_command_req 		char*
addfunc paron 			do_paron 			int
addfunc paroff 			do_paroff 			int
addfunc clearthread 		do_one_parallel 		int
addfunc clearthreads 		do_clear_parallel 		int
addfunc threadson 		do_enable_parallel		int
addfunc threadsoff 		do_disable_parallel 		int
// addfunc tab_copy 		do_remote_tab_copy 		int
// addfunc r3dmsg 		do_remote_3d_dd_msg 		int
addfunc barrier			do_node_sync
addfunc barrierall 		do_all_node_sync
// addfunc rget			do_remote_global_req		char*
// addfunc waitset		do_remote_global_poll 		char*
addfunc perfstats		do_print_perfstats		int
addfunc setlookahead		do_set_lookahead		int
addfunc showlookahead		do_show_lookahead		int
addfunc getlookahead		do_get_lookahead		float

// A way of creating 3-d surfaces of cells. Options permit
// automatic orientation, randomisation, and curving the dendrites
// of the cells to match the surface, and loading into the zones
// of each postmaster
// addfunc cell_sheet 		do_cell_sheet

newclass postmaster

/*
*************************************************************************
**                              OBJECT TABLE                           **
*************************************************************************
*/
//==========================================================================
//	name		data            function	class	
//==========================================================================

object	post	post_type PostAction 	postmaster \
	-author "Nigel Goddard, Pittsburgh Supercomputing Center, 8/94" \
	-actions 	INIT PROCESS RESET CHECK CREATE SET ZONING ADDMSGOUT EVENT\
	-messages 	PAR_NOTIFY_DELETE	0 	1  "source address" \
		 	PAR_ACTIVE_MSG		-1 	1  "message index" \
	-readonly	totalnodes	"total number of all nodes"	 \
	-readonly	myzone		"node's zone number"		 \
	-readonly	mynode		"node number within this zone"	 \
	-readonly	nzones		"number of zones"		 \
	-readonly	nnodes		"number of nodes in this zone"	 \
	-hidden		ipm		"the gory details"		 \
	-description	"Handles message passing between nodes."


