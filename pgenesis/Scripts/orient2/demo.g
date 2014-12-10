// genesis

// this script demonstrates the n-way slice decomposition for the
//	orient2 example

// parameters for this run
int n_slices = 5	// 5 slices
int display = 1		// display neurons and graphs
int output = 0		// don't dump neural output to a file
int retina_nx = 10	// retina has 10 x 10 cells
int retina_ny = 10
int v1_nx = 5		// each V1 population has 5 x 5 cells
int v1_ny = 5
int sim_steps = 1000	// when sweep is requested, we will do 100 steps
int batch = (display == 0) // we are running interactively

include o.g		// main orient2 script file

// $Log: demo.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:39  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.1  1999/12/18 03:02:06  mhucka
// Added PGENESIS from GENESIS 2.1 CDROM distribution.
//
// Revision 1.3  1997/03/02 09:29:59  ngoddard
// typo
//
// Revision 1.2  1997/03/02 08:45:17  ngoddard
// batch now determined by display value
//
// Revision 1.1  1996/09/20 18:55:21  ghood
// Initial revision
//
