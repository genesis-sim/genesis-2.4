//genesis

/*
** $Id: simconnlib.g,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $
**
** $Log: simconnlib.g,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1994/09/23 16:13:48  dhb
** Initial revision
**
*/

/*
************************************************************************
**                       SIMULATOR SHELL COMMANDS                     **
************************************************************************
*/

/*
** commands for creating and manipulating elements and their structures
*/
addfunc		setconn			do_setconn
addfunc		showconn		do_showconn
addfunc		connect			do_add_connection
addfunc		delete_connection	do_delete_connection
	
/*
** simulator utilities
*/
addfunc		region_connect		do_region_connect
addfunc		cstat			do_connection_status

/*
** functions not intended for the command line
*/
addfunc		getconn			do_getconn		char*
