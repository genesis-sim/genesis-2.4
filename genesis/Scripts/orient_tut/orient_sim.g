// genesis

include constants.g
include protodefs.g
include retina.g
include V1.g
include ret_V1.g
include ret_input.g
include V1_output.g
include syn_utils.g

// Setting a few simulation parameters

float dt = 1.0e-4		// sec
floatformat %g
float refresh_factor = 10.0

// setting the simulation clocks

setclock	0 {dt}		        // sec
setclock	1 {dt * refresh_factor} // sec

// Seeding the random number generator used later in the input pattern.
// Seeding with a defined number (integer) allows one to reproduce
// 'random' patterns from one simulation to the next.   Seeding
// without a defined seed gives different results each time you run
// the simulation.


randseed 34521

reset // This initialises and gets everything ready to go.


