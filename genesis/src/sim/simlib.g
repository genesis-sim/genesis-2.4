//genesis

/*
** $Id: simlib.g,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $
**
** $Log: simlib.g,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.32  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.31  2000/09/07 05:18:08  mhucka
** Addition from Hugo Cornelis: function countelementlist.
**
** Revision 1.30  1998/07/21 23:09:51  dhb
** do_version now returns a char*
**
** Revision 1.29  1998/07/15 06:52:26  dhb
** Merged 1.27.1.1
**
** Revision 1.28  1998/01/08  23:51:11  dhb
** Added the setrand command to allow switching between the
** NR and SPRNG random number generators.
**
** Revision 1.27.1.1  1998/07/15  06:50:44  dhb
** Upi update
**
** Revision 1.27  1996/11/04 06:35:31  dhb
** Added reclaim command to allow explicit recovery of memory from
** deleted elements.
**
** Revision 1.26  1995/09/29  16:41:27  dhb
** Added return types for openfile, closefile, writefile and flushfile
** commands which all return an int.
**
** Revision 1.25  1995/06/28  00:18:26  dhb
** Changed showalias to showcommand which shows the C function
** implementing a given command.
**
** Revision 1.24  1994/09/20  19:01:42  dhb
** Fixed deletemsgdefdef typo.
**
** Revision 1.23  1994/09/16  23:32:07  dhb
** Moved connection related commands to oldconn
**
** Revision 1.22  1994/08/08  22:08:19  dhb
** Changes from Upi.
**
** Revision 1.22  1994/06/13  22:36:51  bhalla
** added swapdump command
**
** Revision 1.21  1994/06/03  22:45:48  dhb
** Merged in 1.19.1.1 changes.
**
** Revision 1.20  1994/05/27  23:04:21  dhb
** Added isa, addforwmsg and deleteforwmsg commands.
**
** Revision 1.19.1.1  1994/04/13  16:46:20  bhalla
** Added dump-related commands
**
** Revision 1.19  1994/03/21  04:09:43  dhb
** Removed saveimage and restoreimage commands, both of which were buggy
** and system dependent.
**
** Revision 1.18  1994/03/21  03:02:39  dhb
** Moved logfile/do_logon to shell lib where it is implemented.
**
** Revision 1.17  1994/03/20  23:52:01  dhb
** Changed connection releated commands back to 1.4 names.
**
** Revision 1.16  1994/03/20  23:21:51  dhb
** 2.0 command name changes
** 	added commands for extended objects
**
** Revision 1.15  1994/03/20  20:40:31  dhb
** 2.0 command names:
** 	push ==> pushe
** 	pop ==> pope
**
** Revision 1.14  1994/03/20  20:31:16  dhb
** 2.0 command name:
** 	alias ==> addalias
** 	showbind ==> showalias
**
** Revision 1.13  1993/03/10  23:19:01  dhb
** Extended element fields.  Added addfuncs for addfield and deletefield
** commands.
**
** Revision 1.12  1993/02/22  23:45:24  dhb
** Removed addfunc for abort command since one exists in shelllib.g.
**
** Revision 1.11  1993/02/22  23:43:42  dhb
** Removed addfuncs for addescape and listescape as these exist in shelllib.g.
**
** Revision 1.10  1993/02/22  22:15:55  dhb
** Removed addfuncs for job related commands, as these belong (and exist) in
** shelllib.g
**
** Revision 1.9  1993/02/17  23:28:45  dhb
** 1.4 to 2.0 command name change.
**
** 	setobjenv changed to setdefault.
**
** Revision 1.8  1993/02/17  23:26:40  dhb
** Moved addfuncs for Error (error) and ResetError (clearerrors) to
** shelllib.g
**
** Added addfunc for do_list_actions (listactions).
**
** Revision 1.7  1993/02/16  22:24:35  dhb
** Removed "addfunc getinfo do_getinfo".  This command is added in the startup
** code in the shell library.
**
** Also, added "addfunc getconn do_get_connection_status float".
**
** Revision 1.6  1993/02/15  21:24:16  dhb
** Removed "addfunc silent do_silent int".  This is added in startup code in
** the shell library.  Note: May need to add this and do_debug to shellib.g
** as the built in code defined them as void.
**
** Revision 1.5  1993/02/15  21:20:56  dhb
** Removed "addfunc debug do_debug".  The debug command is added in the start
** up code in the shell library (e.g. in C code, not in shellib.g).
**
** Revision 1.4  1993/02/15  19:17:51  dhb
** Changed "addfunc getstat .." to bind to do_getstatus returning a float.
** Also fixed "addfunc showstat" to bind to do_showstat (previously not
** binding to anything!).
**
** Revision 1.3  1993/02/15  18:47:55  dhb
** Removed "addfunc sh do_shell" which is already in shell/shelllib.g where
** it should be.
**
** Revision 1.2  1993/01/28  18:13:34  snehal
** Jan 27, 1993 - new names of Genesis commands. Typed by Snehal.
**
# Revision 1.1  1992/10/27  20:39:07  dhb
# Initial revision
#
*/

/*
************************************************************************
**                       SIMULATOR SHELL COMMANDS                     **
************************************************************************
*/
addfunc		step			do_simulate
addfunc		cpu			do_ps
addfunc		getdate			do_date			char*
addfunc		reset			do_reset
addfunc		getstat			do_getstatus		float
addfunc  	showstat		do_showstatus
addfunc		randseed		do_srandom		int
addfunc		setrand			do_setRNG
addfunc		notes			do_notes
addfunc		showclocks		ShowClocks
addfunc		setclock		do_set_clock

/*
** commands for moving around in the element tree structure
*/
addfunc		ce			do_change_element
addfunc		pwe			do_pwe
addfunc		le			do_list_elements
addfunc		pushe			do_pushe
addfunc		pope			do_pope
addfunc		stack			do_show_estack

/*
** commands for creating and manipulating elements and their structures
*/
addfunc		setfield		do_set
addfunc		showfield		do_show
addfunc		create			do_create
addfunc		copy			do_copy
addfunc		move			do_move
addfunc		createmap		do_create_map
addfunc		position		do_position
addfunc		delete			do_delete_element
addfunc		reclaim			ElementReaper	int
addfunc		enable			do_enable
addfunc		disable			do_disable
	
/*
** simulator utilities
*/
addfunc		check			CheckSimulation
addfunc		help			do_help
addfunc		deleteall		do_clean
addfunc		version			do_version	char*

addfunc		showmsg			do_show_msg
addfunc		stop			SetBreakFlag

addfunc		listobjects		do_list_objects
addfunc		showobject		do_show_object

addfunc		listclasses		do_list_classes

addfunc		deletetasks		do_clear_schedule
addfunc		addtask			do_add_schedule
addfunc		resched			Reschedule
addfunc		showsched		do_list_tasks

addfunc		listactions		do_list_actions
addfunc		call			do_call_element		char*
addfunc		addalias		do_alias		int
addfunc		showcommand		do_show_binding
addfunc		useclock		do_element_clock
addfunc		addmsg			do_add_msg		int
addfunc		deletemsg		do_delete_msg
addfunc		openfile		do_openfile		int
addfunc		closefile		do_closefile		int
addfunc		writefile		do_writefile		int
addfunc		flushfile		do_flushfile		int
addfunc		listfiles		do_showfiles
addfunc		putevent		do_insert_event		int
addfunc		setmethod		do_set_method

/*
** functions not intended for the command line
*/
addfunc		getclock		do_getclock		float
addfunc		getfield		do_getfield		char*
addfunc		getelementlist		do_construct_list 	char**
addfunc		el			do_construct_list 	char**
addfunc		countelementlist	do_count_list		int
addfunc		readfile		do_readfile		char**
addfunc		eof			do_checkeof		int
addfunc		exists			do_exists		int
addfunc		getmsg			do_getmsg		char*
addfunc		getfieldnames		do_getfields		char**
addfunc		getpath 		do_pathname		char*
addfunc		save			do_saveparms		int
addfunc		restore			do_restoreparms		int

/*
** Extended object related commands
*/

addfunc		addobject		do_add_extobject
addfunc		isa			do_isa			int

addfunc		addfield		do_addfield		int
addfunc		deletefield		do_deletefield		int
addfunc		setfieldprot		do_setfieldprot		int

addfunc		setdefault		do_setdefault		int
addfunc		getdefault		do_getdefault		char*

addfunc		addmsgdef		do_add_msglist		int
addfunc		deletemsgdef		do_delete_msglist	int

addfunc		addforwmsg		do_forw_msg		int
addfunc		deleteforwmsg		do_del_forw_msg		int

addfunc		addclass		do_add_class		int
addfunc		deleteclass		do_delete_class		int

addfunc		addaction		do_add_action		int
addfunc		deleteaction		do_delete_action	int

/*
** Dump and undump commands
*/

addfunc		simobjdump		do_simobjdump
addfunc		initdump		do_initdump
addfunc		enddump			do_enddump
addfunc		simdump			do_simdump
addfunc		simundump		do_simundump
addfunc		swapdump		do_swap_dumplist	int
addfunc		objsubstitute	do_obj_substitute
addfunc		msgsubstitute	do_msg_substitute
addfunc		substituteinfo	do_substitute_info

