// genesis

// this script demonstrates the 3-way parallel decomposition for the
// 	orient1 example

int display = 1		// display neurons and graphs
int output = 0		// don't dump neural output to a file
int retina_nx = 10	// retina has 10 x 10 cells
int retina_ny = 10
int v1_nx = 5		// each V1 population has 5 x 5 cells
int v1_ny = 5
int sim_steps = 1000	// when sweep is requested, we will do 1000 steps
int batch = (display == 0)	// we are running interactively

include o.g		// main orient1 script file

// $Log: demo.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:39  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.1  1999/12/18 03:02:04  mhucka
// Added PGENESIS from GENESIS 2.1 CDROM distribution.
//
// Revision 1.4  1997/07/15 00:14:32  ngoddard
// *** empty log message ***
//
// Revision 1.3  1997/03/02 09:29:19  ngoddard
// cleaned up termination
//
// Revision 1.2  1996/09/20 18:52:25  ghood
// *** empty log message ***
//
