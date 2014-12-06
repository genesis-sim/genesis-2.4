// protodefs.g - Definition of prototype elements for RScell.g

// include the definitions for the functions to create the channels
include DPchans.g

/* The remaining included files are in genesis/Scripts/neurokit/prototypes */

/* file for standard compartments */
include compartments

/* file for synaptic channels */
include synchans

/* file which makes a spike generator */
include protospike

// Make a "library element" to hold the prototypes which will be used
// by the cell reader to add compartments and channels to the cell.

if (!{exists /library})     // But, only if it doesn't already exist
    create neutral /library
end


// We don't want the library to try to calculate anything, so we disable it
disable /library

// To ensure that all subsequent elements are made in the library
pushe /library

// Make a prototype compartment.  The internal fields will be set by
// the cell reader, so they do not need to be set here.
make_cylind_compartment         /* makes "compartment" */

// Create the prototype channels with functions defined in
make_Na_pyr_dp
make_Kdr_pyr_dp
make_KM_pyr_dp

// Make a prototype excitatory channel, "Ex_channel"
make_Ex_channel     /* synchan with Ek = 0.045, tau1 = tau2 = 3 msec */
make_spike      /* Make a spike generator element "spike"*/

pope // Return to the original place in the element tree

