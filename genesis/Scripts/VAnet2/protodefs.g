// protodefs.g - Definition of prototype elements for VA_HHnet variations

// include the definitions for the functions to create the VA_HHcell channels
include VA_HHchans.g

/* The remaining included files are in genesis/Scripts/neurokit/prototypes */

/* file for standard compartments */
include compartments

/* file for synaptic channels */
include VA_synchans.g

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

// Create the VA_HHcell prototype channels with functions defined in VA_HHchans.g
make_Na_traub_mod
make_K_traub_mod 

// Make a prototype excitatory channel, "Ex_channel"
make_Ex_channel
make_Inh_channel
make_spike      /* Make a spike generator element "spike"*/

pope // Return to the original place in the element tree

