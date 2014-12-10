// genesis

// this script demonstrates a simple parameter search based on a genetic
//      algorithm technique

int n_nodes = 4			// number of nodes to use
int simulation_time = 0		// the number of seconds to sleep to
				//   simulate one genesis run of the
				//   neural model (i.e., one "invididual")

int individuals = 10000		// the total number of individuals to try out
int population = 10		// the maximum number of "alive" individuals
				//   in the population

randseed 67351			// we explicitly set the random seed

include pga.g			// main parallel genetic algorithm script

// $Log: demo.g,v $
// Revision 1.2  2006/02/03 17:28:27  ghood
// Changed to Gray-encoded parameter values with a more realistic evaluation function
//
// Revision 1.1.1.1  2005/06/14 04:38:39  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.1  1999/12/18 03:02:08  mhucka
// Added PGENESIS from GENESIS 2.1 CDROM distribution.
//
// Revision 1.2  1996/09/20 18:58:54  ghood
// Cleaned up for PSC release
//
// Revision 1.1  1996/08/16 19:04:25  ghood
// Initial revision
//
