README file for the Tutorial Simulation of a CA3 Hippocampal Neuron

This is a tutorial based upon a GENESIS simulation of the hippocampal
pyramidal cell model of Traub, et. al. [R.D.Traub, R. K. S.  Wong, R. Miles,
and H. Michelson, Journal of Neurophysiology, Vol. 66, p.  635 (1991)] You
will get the most benefit from the tutorial if you consult this paper while
using the tutorial.  The tutorial explores some of the properties of this
model and also serves as an introduction to the 'Neurokit' simulation
environment.

A description of the properties of the various types of ionic channels used
in the model may be found in Chapter 7 of "The Book of GENESIS".  The file
"traub91proto.g" may also be used as an example of the use of tabchannels
to implement Ca concentration dependent channels.

   November 1999 update for GENESIS 2.2 (implementation note):  Previous
   versions of traub91proto.g used a combination of a table, tabgate, and
   vdep_channel to implement the Ca-dependent K Channel - K(C).  This new
   version uses the new tabchannel "instant" field, introduced in GENESIS 2.2,
   to implement an "instantaneous" gate for the multiplicative Ca-dependent
   factor in the conductance.  This allows these channels to be used with the
   fast hsolve chanmodes > 1.  Note that GENESIS 2.2 now allows the use of
   hsolve with the symmetric compartments (symcompartments) that are used in
   the Traub 1991 model.  The file "traub91.g" is now included as a
   demonstration script to show how to construct a standalone Traub model
   simulation without the complexity (or tutorial features) of the simulation
   using Neurokit.

In order to run the simulation, carry out the following steps:

1) In order to allow room for the graphical display, move a terminal window
   into the lower left hand corner of your screen.  (This is normally done
   by using the left mouse button to click and drag on the title bar of the
   window.)

2) After changing into the directory in which this file resides, (usually
   "traub91"), type "genesis" to the unix prompt.

3) After the "genesis #0>" prompt appears, type "Neurokit", in order to load
   the Neurokit simulation.  (Don't forget that unix is case sensistive!)
   There will be a delay while the simulation is being set up, and a few
   obscure messages will appear.  If all goes well, a title bar with various
   options will eventually appear at the top of the screen.  (If not, consult
   the notes below on "installing the simulation".)

4) Click the left mouse button on the box labeled 'help' and follow the
   instructions which appear.  (This on-line documentation is in the file
   "traub_tut.help", which may also be printed.)

INSTALLING THE SIMULATION

If GENESIS has not yet been installed on your machine, refer to the main
README file accompanying GENESIS or the printed documentation for GENESIS.
The paths to necessary files should be set up as specified in the GENESIS
README file, the necessary ".simrc" file should exist in your home
directory, and X-windows should be started.  In order to access the the
Neurokit simulation, your .simrc file should use the SIMPATH environment
variable to set a path to the neurokit and neurokit/prototypes directories.
(This is done in the usual default .simrc that is provided with GENESIS.)

Dave Beeman - dbeeman@dogstar.colorado.edu
