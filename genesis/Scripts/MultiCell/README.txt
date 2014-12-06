
MultiCell:

    This is a simulation of two simplified, but realistic neurons.
in a feedback configuration. Each cell is composed of two compartments 
corresponding to a soma and a dendrite. Each compartment is composed of 
two variable conductance ionic channels for sodium (Na) and potassium (K). 
The dendritic channels are synaptically activated while the somatic channels 
are voltage dependent with Hodgkin-Huxley kinetics.

Look in the file 'MultiCell.txt' for a tutorial on how to run this
demo.

==========================================================================

This simulation, written by Matt Wilson in 1989, illustrates the "old" way
of making connections between neurons with the axon and channelC2 objects
used in GENESIS versions 1.x.  These scripts have been converted to GENESIS
2.0 with the "convert" utility and make use of the X1compat (XODUS 1
compatible) widgets and the oldconn library.  It thus serves as an example of
the conversion of a GENESIS 1 simulation to GENESIS 2.  For examples of
simulations that are similar to MultiCell, but which have been converted all
the way to XODUS 2 widgets with the "new" way of making connections, see
the Neuron and CPG tutorials.

Because of the conversion process, the line numbers used in MultiCell.doc
will not be accurate.
