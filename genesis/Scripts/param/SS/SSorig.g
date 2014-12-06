// genesis

//
// SSorig.g
//

//
// This script is used to regenerate the original target data files
// for use by the parameter search.
//

int orig = 1

include siminit.g          // basic simulation definitions
include fI.g               // sets up table of currents
include origchannels.g     // defines the channels
include paramfuncs.g       // helper functions for parameter searches
include params.g           // sets up parameter table
include cell.g             // functions to load in the cell model
include search.g           // functions for running parameter searches

make_orig

quit
