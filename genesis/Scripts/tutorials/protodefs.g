// protodefs.g - Definition of prototype elements for Tutorial #5

/* file for standard compartments */
include compartments

// include the definitions for the functions to create H-H channels
include hhchan

/* hhchan.g assigns values to the global variables EREST_ACT, ENA, EK, and
   SOMA_A.  The first three will be superseded by values defined below.  The
   value of SOMA_A is not relevant, as the cell reader calculates the
   compartment area.  */

EREST_ACT = -0.07	// resting membrane potential (volts)
ENA   = 0.045           // sodium equilibrium potential
EK    = -0.082          // potassium equilibrium potential

/* file for synaptic channels */
include synchans

/* file which makes a spike generator */
include protospike

// Make a "library element" to hold the prototypes which will be used
// by the cell reader to add compartments and channels to the cell.
create neutral /library

// We don't want the library to try to calculate anything, so we disable it
disable /library

// To ensure that all subsequent elements are made in the library
pushe /library

// Make a prototype compartment.  The internal fields will be set by
// the cell reader, so they do not need to be set here.
make_cylind_compartment         /* makes "compartment" */

// Create the prototype H-H channels "Na_squid_hh" and "K_squid_hh"
make_Na_squid_hh
make_K_squid_hh

// Make a prototype excitatory channel, "Ex_channel"
make_Ex_channel     /* synchan with Ek = 0.045, tau1 = tau2 = 3 msec */

make_spike      /* Make a spike generator element "spike"*/

pope
