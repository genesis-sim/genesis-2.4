static char rcsid[] = "$Id: setup_table.c,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $";

/*
** $Log: setup_table.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:06:08  dhb
** Initial revision
**
*/

/* 
** Has standard HH variables for alpha and beta as arguments, builds
** the tabchannel
*/
do_setup_tabchan(argc,argv)
	int argc;
	char	**argv;
{
}

/* 
** Has standard HH variables for minf and tau as arguments,
** builds the tabchannel
*/
do_tau_setup_tabchan(argc,argv)
	int argc;
	char	**argv;
{
}

/*
** Has values entered in table in alpha form, sets up usual
** table from them
*/
do_alpha_fill(argc,argv)
	int argc;
	char	**argv;
{
}

/*
** Has values entered in table in tau form, sets up usual
** table from them
*/
do_tau_fill(argc,argv)
	int argc;
	char	**argv;
{
}

/*
** Scales the table entries according to specified changes to
** the alpha function form
** Shift left/right
** Scale up/down
** Offset in voltage
*/
do_scale_alpha(argc,argv)
	int argc;
	char	**argv;
{
}


/*
** Scales the table entries according to specified changes to
** the tau-minf function form
*/
do_scale_tau(argc,argv)
	int argc;
	char	**argv;
{
}

/*
** Useful when a bunch of tables are specified separately. The
** hsolver needs them in uniform form when in higher opt modes
** Converts table to specified xdivs,xmin,xmax.
*/
do_standardize_table(argc,argv)
	int argc;
	char	**argv;
{
}
