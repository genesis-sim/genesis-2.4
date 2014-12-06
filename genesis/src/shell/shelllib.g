//genesis
//
// $Id: shelllib.g,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $
// $Log: shelllib.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:34  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.10  2003/03/28 21:10:46  gen-dbeeman
// Changes from Hugo Cornelis to enable debugging commands gctrace and gftrace
//
// Revision 1.9  2001/06/29 21:27:05  mhucka
// Fixed return type of restore_context, so that shell_g@.c would compile.
//
// Revision 1.8  1998/04/08 16:15:01  dhb
// Added addglobal, setglobal and getglobal commands.
//
// Revision 1.7  1994/10/26 00:00:15  dhb
// Added "hashfunc iofunc" so that showjobs shows the job function.
//
// Revision 1.6  1994/07/13  00:05:18  dhb
// Changed substr back to substring and changed replacesubstr to
// strsub.
//
// Revision 1.5  1994/07/12  01:32:48  dhb
// Changed substring command to substr.
//
// Added replacesubstr command.
//
// Revision 1.4  1994/03/21  03:34:59  dhb
// Changed getmaxerrors and getmaxwarnings back to the 1.4 command names
// (maxerrors and maxwarnings).  Also added a warning command to complement
// the error command.
//
// Revision 1.3  1993/02/17  23:24:49  dhb
// Moved addfuncs for Error (error) and ResetError (clearerrors) from simlib.g.
//
// Revision 1.2  1993/01/28  19:20:50  snehal
// Jan 25, 1993. Changes to Genesis commands. Typed by Snehal.
//
// Revision 1.1  1992/12/11  19:04:58  dhb
// Initial revision
//

//genesis

hashfunc iofunc

/*
************************************************************************
**                              SHELL COMMANDS                        **
************************************************************************
*/
addfunc		input			do_INPUT		char*
addfunc		rand			do_Random		float
addfunc		gaussian		do_Gaussian		float
addfunc		min			do_min			float
addfunc		max			do_max			float
addfunc		trunc			do_trunc		int
addfunc		round			do_round		int
addfunc		tan			do_tan			float
addfunc		sin			do_sin			float
addfunc		cos			do_cos			float
addfunc		asin			do_asin			float
addfunc		acos			do_acos			float
addfunc		atan			do_atan			float
addfunc		pow			do_pow			float
addfunc		sqrt			do_sqrt			float
addfunc		abs			do_fabs			float
addfunc		log			do_log			float
addfunc		exp			do_exp			float
addfunc		argc			do_argc			int
addfunc		argv			do_argv			char**
addfunc		getarg			do_getarg		char*
addfunc		showjobs		do_list_jobs
addfunc		addjob			do_add_job
addfunc		deletejob		do_remove_job
addfunc		setpriority		do_change_priority
addfunc		sh			do_shell
addfunc		addescape		do_escape_seq
addfunc		listescape		do_list_esc
addfunc		abort			restore_context		SIGTYPE
addfunc		listglobals		do_listglobals
addfunc		addglobal		do_addglobal		int
addfunc		setglobal		do_setglobal		int
addfunc		getglobal		do_getglobal		char*

addfunc		maxerrors		do_set_maxerrors
addfunc		maxwarnings		do_set_maxwarnings
addfunc		clearerrors		ResetErrors
addfunc		error			Error
addfunc		warning			Warning
addfunc		logfile			do_logon

addfunc		gctrace			do_gctrace		int

addfunc		strcat			do_cat			char*
addfunc		strcmp			do_strcmp		int
addfunc		strncmp			do_strncmp		int
addfunc		strlen			do_strlen		int
addfunc		strsub			do_replacesubstr	char*
addfunc		findchar		do_findchar		int
addfunc		countchar		do_countchar		int
addfunc		substring		do_substring		char*
addfunc		arglist			do_arglist		char**
addfunc		chr			do_chr			char*
addfunc		floatformat	do_set_float_format

addfunc		gftrace			do_gftrace		int

