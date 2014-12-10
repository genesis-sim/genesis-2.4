// genesis

// Setting a few simulation parameters

float dt = 1.0e-4		// sec
floatformat %g
float refresh_factor = 10.0

float	V1_SEPX = 80e-6
float	V1_SEPY = 80e-6
float	V1_SEPZ = 1e-6

// setting the simulation clocks

setclock	0 {dt}		        // sec
setclock	1 {dt * refresh_factor} // sec

// Seeding the random number generator used later in the input pattern.
// Seeding with a defined number (integer) allows one to reproduce
// 'random' patterns from one simulation to the next.   Seeding
// without a defined seed gives different results each time you run
// the simulation.

randseed 34521
// $Log: params.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:39  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.1  1999/12/18 03:02:05  mhucka
// Added PGENESIS from GENESIS 2.1 CDROM distribution.
//
// Revision 1.1  1996/05/03 19:11:38  ngoddard
// Initial revision
//
