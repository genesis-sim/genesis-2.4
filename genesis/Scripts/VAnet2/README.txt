README file for VAnet2
======================

Introduction and Purpose
------------------------

This is an improved and much more efficient GENESIS implementation of
the dual exponential conductance version of the Vogels-Abbott (Vogels
and Abbott 2005) network model with Hodgkin-Huxley neurons and
conductance-based synaptic activation (COBA). Details are given in
Brette et al. (2007). The original scripts for GENESIS and several
other simulators are available from ModelDB at
http://senselab.med.yale.edu/SenseLab/ModelDB/ShowModel.asp?model=83319.

However, the original GENESIS 'dualexpVA-HHnet.g' script was designed
to show off as many GENESIS features as possible, and not for
computational speed.  In particular, it did not make use of the
considerable speed and accuracy improvements offered by the use of the
GENESIS 'hsolve' object.  The files in this package include the
original files needed to run 'dualexpVA-HHnet.g' with its GUI, and the
original README documentation files as `README-orig.txt
<README-orig.txt>`_ and `README-orig.html <README-orig.html>`_.  These
describe the model, how to run the simulation, suggested experiments
to try, and suggestions for extending the model.

The added simulation script 'VAnet2-batch.g' is a stripped down
version of 'dualexpVA-HHnet.g' with no graphics, but with data output
to a file instead of to a plot.  It uses hsolve and runs 16 times faster
than the original.  The GENESIS 3 Python utility script 'gipyplot.py' is
included for plotting of the simulation output.

The Vogels-Abbot model has very simple point cells with no location in
space, but many synaptic connections.  Although I do not consider this
model to be as scientifically interesting as other more structurally
realistic models, it is a good benchmark for testing the efficiency
and time performance of simulations of highly connected networks.

I offer this GENESIS implementation a beginning step to test tools
such as NeuroML (neuroml.org) and neuroConstruct (neuroConstruct.org)
for representing and building cortical network models.  The simple
four-way synaptic interactions between two populations of excitatory
and inhibitory cells have made the Vogels-Abbott model a popular test
for implementations of synaptic plasticity.  The 'VAnet2-batch.g'
script is intended to be extended for testing GENESIS spike timing
dependent plasticity (STDP) implementations with hsolve.

I encourage you to extend and share this tutorial/documentation and
simulation script package with others. The files here may be freely
distributed under the terms of the `GNU Lesser General Public License
version 2.1 <copying.txt>`_.

David Beeman
University of Colorado, Boulder
dbeeman@colorado.edu
Mon Jul  7 15:35:23 MDT 2014

Running the simulation
----------------------

Of course, the first step in running the simulation is to have GENESIS
installed.  If you have not used GENESIS before, it is simplest to download
the entire 'Ultimate GENESIS Tutorial Distribution' package with all source
code, installation instructions, and the complete set of tutorials
(about 50 MB) from `http://genesis-sim.org/GENESIS/UGTD.html
<http://genesis-sim.org/GENESIS/UGTD.html>`_.  GENESIS usually installs without
problems under modern versions of Linux.  Most questions related to
installation have been answered in the archives of the 'genesis-sim-users'
mailing list, which are available from the GENESIS 2 Sourceforge page at
`http://sourceforge.net/projects/genesis-sim/
<http://sourceforge.net/projects/genesis-sim/>`_.

Once GENESIS 2.3 has been installed, the command::

  genesis VAnet2-batch.g

runs the simulation with the default parameters used to generate the
data similar to that plotted by dualexpVA-HHnet.g, and shown in
`FigG1-dualexpVA-HH.gif <FigG1-dualexpVA-HH.gif>`_.  It prints some
informative information about the simulation, including the START and
END times of runs, then ends with the prompt::

  genesis #0 >

At this point, you can type 'quit' to the prompt, or explore the
model with interactive GENESIS commands.

The three excitatory cells for which the soma membrane potential
is plotted in FigG1-dualexpVA-HH.gif are cells 0, 1536, and 1567.
Although spatial location plays no role in the default model, these
correspond respectively to the lower left corner, middle right edge,
and center cells.  This data will be written to a file 'Vm_out_1000.txt',
which may be plotted with the included G-3 Pyhthon utility 'gipyplot.py'.

'gipyplot' is a general plotting program, developed for use with GENESIS
that can overplot multi-column data from single or multiple files.  It
also has command line options, including '-h' for help.  
In order to run gipyplot you will need python version 2.5 or later,
and the Matplotlib library for python, which can be downloaded from
http://sourceforge.net/projects/matplotlib.  The installation instructions
explain other requirememts, such as NumPy (numerical libraries for python)
that are usually installed along with python on most Linux systems.

For this data, you can type::

    gipyplot.py -t "VAnet2 RUN 1000" Vm_out_1000.txt

to plot this data with cell 0 in blue, cell 1536 in green, and cell
1567 in red.

The GUI for the original 'dualexpVA-HHnet.g' script showed a
visualization of the membrane potential for each cell, using the
GENESIS/XODUS 'xview' object.  If you would like to add output of the
soma Vm or Ex_channel Ik, for all cells at specified time intervals,
you can use the functions defined and Python tools that are provided
in the 'ACnet2' model of primary auditory cortex layer IV (Beeman
2013), which is based on the original dualexpVA-HHnet.g script.  The
scripts and tutorial for the 'ACnet2' model may be viewed or
downloaded at http://genesis-sim.org/GENESIS/ACnet2-GENESIS or from
ModelDB at http://senselab.med.yale.edu/ModelDB/ShowModel.asp?model=150678.

The Open Source Brain project repository is at
https://github.com/OpenSourceBrain/ACnet2.

Settable options and parameters
-------------------------------

Nearly everything about the model or its parameters can be modified by
following the suggestions in `README-orig.html <README-orig.html>`_ or
the comments in 'VAnet2-batch.g'.  For performing benchmarks to test
speed and accuracy, these option strings are of particular interest::

  int connect_network = 1  // Set to 0 for testing with unconnected cells
  int hflag = 1    // use hsolve if hflag = 1
  int hsolve_chanmode = 4  // chanmode to use if hflag != 0

For debugging modifications to the script, or determining how much
time is spent performing operations other than processing spike events
(e.g. solving multicompartmental cell equations and processing I/O),
connect_network can be set to 0. This skips the step of connecting
up the cells.

To see how much slower the simulation runs without hsolve, or to
test to see if script changes work the same with hsolve as without,
hflag can be set to 0, to disable the setup of hsolve.  hsolve_chanmode
is used only if hflag > 0, and sets the chanmode (described in the
hsolve documentation) to the fastest most efficient mode.

Strings can be set to specify other cells to be used in the model.
The default time step used for solving the cell and network equations
is set with::

  float dt = 50e-6        // default simulation time step

This is the time step used in the original script, which is rather large.
A more appropriate value for most cortical network simulations would
be ``dt = 20e-6`` seconds.  The time step for input to the model and
output of results is set with::

  float out_dt = 0.0001   // output every 0.1 msec (also used to clock input)

Using a separate slower clock for I/O not only saves time, but it insures
that when input spike events are generated with a random number generator,
their distribution will be independent of the simulation time step.

The random number seed has been set to a particular value with::

  // Set random number seed. If seed is 0, set randseed from clock,
  // else from given seed
  // int seed = 0  // Simulation will give different random numbers each time
  int seed = 1404706052  // This is to insure that it will reproduce the results

This seed produced a particular set of randomly generated connections
(with 0.02 probability, and distribution of spike events that are
generated from an array of randomspike elements.  Other seeds will
produce more or less activation of the network.

Using hsolve with GENESIS network simulations
---------------------------------------------

The object-oriented paradigm for scripting simulations in GENESIS 2 is
one of its most attractive features.  By isolating cell variables
and parameters into separate objects that communicate with 'messages'
it is easy to modify, share, and reuse pieces of simulation scripts
without needing to understand most details of the simulation from
which they were taken.  However, this message-based approach produces
inefficiency at the implementation level, and makes it difficult to
implement matrix-based numerical solutions, such as the Hines (1984)
algorithm for solving branched cable equations.

The GENESIS 2 'hsolve' object was developed to bypass the object-oriented
implementation and allow fast cable model solutions, as well as more
efficient delivery of spike events to synaptically activated channels.
It does this by taking over the entire element hierarchy of a cell
model and treating it as a single 'hsolve' solver element, rather
than a set of elements liked by messages.

The speed improvements when this simulation is run with hsolve are
truly impressive.  With the rather large default time step of 50
microsec (used in the original dualexpVA-HHnet.g script), the
simulation runs 16 times faster than without hsolve.  When the time
step is lowered to 20 microsec, the simulation runs 23 times faster.

However, the price for this efficiency is that not all fields of
the original elements are accessible once they are taken over
by hsolve, and not all messages will work as before.  The GENESIS
documentation for hsolve and the example code in 'ACnet2-batch.g'
describe how to overcome these limitations.

Setting up hsolve for a network requires setting up a solver for one
cell of each type in the network and then duplicating the solvers for
the remaining cells.  The procedure is described in the advanced
tutorial 'Simulations with GENESIS using hsolve by Hugo Cornelis' at
`http://genesis-sim.org/GENESIS/UGTD/Tutorials/advanced-tutorials
<http://genesis-sim.org/GENESIS/UGTD/Tutorials/advanced-tutorials>`_ and in the
documentation and scripts for ACnet2.

Reproduceability, random numbers, and hsolve
--------------------------------------------

The figure from Brett el al. (2007) that is included here
(`FigG1-dualexpVA-HH.gif <FigG1-dualexpVA-HH.gif>`_) shows the results
of the original run of dualexpVA-HHnet.g.  It used the default seed,
which is not known.  Depending on your computer CPU's numerical
processor, and the random number generator, you may or may not get the
same plot of Vm for the three excitatory cells.

The plots that you generate from VA-HHnet2-batch.g with other seeds
than the default value of 1404706052 will be different for the three
cells, and there will be more or less activation of the network. The
figure `VAnet2-RUNID-31xx.png <VAnet2-RUNID-31xx.png>`_ shows the
result of four runs with different seeds.  RUN 3115 corresponds to the
result with the default seed.

If you compare results using the same seed with and without hsolve,
and for different time steps, you will find that the plots will be the
same for a few action potentials, and will then diverge.  Over the
five second run, there will only be qualitative similarities in the
firing patterns.  Why is this so?  When comparing results from
different simulators, numerical algorithms, or time steps, how can one
determine if simulation results are "correct" even if they are
"different"?

The dualexpVA-HHnet.g script produces different results for different
simulation time step size, because the initial 0.05 sec random stimulus
is applied by setting the synchan 'frequency' field of the cells.
This causes a new set of random numbers to be generated at each
time step, and the sequence will be longer or shorter for
different step sizes.  But, VAnet2-batch.g uses a separate clock
for the array of randomspike elements in order to avoid this problem.
Nevertheless, a time step of only 20 microsec produces results that
initially differ slighly (and thus differ significantly at later times)
from those with a step of 19 microsec.

A cortical network is a strongly coupled non-linear dynamical
system.  The pyramidal cells have calcium and calcium concentration
dependent potassium channels with slow dynamics that prolong the
hyperpolarized period following an AP. This contributes to spike
frequency adaptation and causes a greater sensitivity to inputs that
occur when the cell is near threshold for firing, enabling information
to be encoded in spike timing. This also means that small deviations
in Vm caused by variation in the biological system, or by very small
numerical inaccuracies in a simulation, will produce different final
outcomes in this chaotic system. The comparison and validation of
network models becomes very difficult.

One approach is to devise statistical tests based on the overal behavior
of the network.  Correlations in the firing of different cells, or
the power spectra of summed excitatory postsynaptic currents (EPSCs)
can a more meaningful basis of comparison than the individual patterns
of APs.  Functions taken from the ACnet2 simulation could be added
to these scripts to provide this output.

Appendix - Network simulation workflow and hsolve
-------------------------------------------------

Because using hsolve restricts the ability to access the fields of
GENESIS elements that have been taken over by hsolve, it is useful to
see how it affects the steps that are performed in a typical GENESIS
network simulation.The main simulation section of both
'VAnet2-batch.g' and the ACnet2 model simulation scripts follows this
workflow:

1. create prototype elements for the cells

2. create prototype cells for each cell population in the network

3. create network cell populations be assigning positions in
   space to copies of the cell prototype.  This is typically done on a
   2-D grid with createmap, but could be created with "copy" using a
   list of cell coordinates, as in the Nenadic et al. (2003) Turtle Visual
   Cortex simulation.

4. If using hsolve, set up a solver for one cell of each type in the
   network and then duplicate the solvers for the others. The
   procedure is described in the advanced tutorial Simulations with
   GENESIS using hsolve by Hugo Cornelis', referenced above.

5. Create connections between and within the populations of cells.
   This is done with either planarconnect, volumeconnnect, or
   with a loop establishing SPIKE messages according to a particular
   algorithm.

6.  Set up weights and delays for the connections

7. Set up inputs to the network.  This is done either by setting
   fields (e.g. 'soma inject', or 'dend/Ex_channel frequency'), or by
   establishing messages.  For example, the message could be an INJECT
   message to a compartment from a pulsgen or a SPIKE message to a
   synchan from a spikegen.  Sometimes a 'script_out' object with its
   own simulation clock and time step may be used to call a script
   function that sets fields that control the input.

8. Set up outputs.  These may be to files, graphical displays,
   or to analysis functions or objects.

9. Modify any desired settable parameters.

10. Run the simulation for a specified amount of time with the 'step'
    command, either directly, or indirectly with a function such as
    'step_tmax' that could output additional information or perform
    analyses.

**Notes regarding the use of hsolve chanmode 4 with these steps**

Steps 5 and 6 present no problems with networks of hsolved cells.

Step 7 presents some problems with hsolve.

Most, but not all fields of hsolved elements can be set using
findsolvefield.  In particular, the synchan frequency field cannot
accessed with findsolvefield.

The hsolve documentation sas "Incoming and outgoing messages to and
from the disabled elements will work properly, provided that they were
added prior to setting up the hsolve element." for chanmodes >= 2.
But, this doesn't seem to be true for all messages. Some are not
supported by hsolve. For example, the synchan RAND_ACTIVATION message
does not seem to be supported by hsolve, even if the input is added
out of the normal order, before the setup of hsolve and the
connections.  In this case it gives the warning:

** Warning - during SETUP of /Ex_layer/Ex_cell/solver: message type 2
to /Ex_layer/Ex_cell/soma/Ex_channel not supported by hsolver.

This is also the case with ACTIVATION messages, although no errors
messages were produced.  There does not seem to be documentation for
which messages are supported by hsolve, and which fields are accessible
with findsolvefield.

The synchan SPIKE message and the resulting synapse weights and delays
are a special case.  They can be established after the hsolve setup,
and it is possible to change weights during a simulation without
needing to perform a reset.  In fact, it is possible to modify the
'synapse' substructure of the synchan in order to add new synapse
fields, as was done in GENESIS 2.4. This makes it possible to
implement global synaptic plasticity mechanisms that exist outide of
the hsolved cells. This was done with the new 'stdp_rules' object.

Step 8 typically involves setting up messages to send values of
fields that are accessible with findsolvefield such as compartment Vm
or synchan Ik, so it is unlikely to be a problem.

Step 9 Is applicable when a simulation has been set up and one wants
to run it with different parameters.  This is typically the case when
using a GUI when "tuning" parameters interactively or with tutorial
simulations.  The simulation is loaded and set up with default
parameters, parameters are changed, and the simulation is stepped
(Step 10).  Often a *reset* command is issued between parameter
changes, so the simulation can be re-run with the new parameters
without exiting the simulation and performing a new set up of cells
and connections.  In principle, this should be possible with hsolve,
as long as a *reset* command is issued after fields in the original
disabled non-solved elements are changed.  The reset should then
tranfer these to the solvers (equivalent to a call to HRESTORE). Step
7 presents a greater problem, because it may be necessary to change
hsolved parameters related to the input while the simulation runs,
without performing a reset between changes.

This is true of most parameters, such as the synchan gmax value.
However, there appears to be a bug when they synchan dual exponential
conductance tau1 and tau2 parameters are changed under hsolve.  This
results in an incorrect scaling of gmax.

Implementation of synaptic plasticity is something of a special case.
The GENESIS 'hebbsynchan' and 'facsynchan' objects implement Hebbian
or facilitating/depressing synapses with modified synchan objects.
This would be done in Step 1.  However, these objects are not
hsolvable.  Most synaptic plasticity mechanisms, and especially STDP,
require a knowledge of the spiking time of other connected cells, in
order to modify their connection weights.  This can be efficiently achieved
by performing the weight changes with a clocked object or function
that is external to the hsolved network. This done with the
stdp_rules object.  For details of the implementation, see the GENESIS
2.4 documentation 'Doc/NewPlasticityObjects.txt'. The use of
stdp_rules is documented in 'Doc/stdp_rules.txt' and the example
scripts and documentation in 'Scripts/stdp_rules'.

References
----------

Beeman D (2013) A modeling study of cortical waves in primary auditory
cortex. BMC Neuroscience, 14(Suppl 1):P23 doi:10.1186/1471-2202-14-S1-P23
(http://www.biomedcentral.com/1471-2202/14/S1/P23)

Brette R, Rudolph M, Carnevale T, Hines M, Beeman D, Bower JM, Diesmann M,
Morrison A, Goodman PH, Harris Jr FC, Zirpe M, Natschlager T, Pecevski D,
Ermentrout B, Djurfeldt M, Lansner A, Rochel O, Vieville T, Muller E,
Davison AP, El Boustani S, and Destexhe A (2007).
Simulation of networks of spiking neurons: A review of tools and
strategies.  J. Comput. Neurosci. 23: 349-398.
http://senselab.med.yale.edu/SenseLab/ModelDB/ShowModel.asp?model=83319

Hines M (1984) Efficient computation of branched nerve equations.
Int. J. Bio-Med. Comput. 15: 69-79.

Nenadic Z, Ghosh BK and Ulinski P. (2003) Propagating Waves in Visual
Cortex: A Large Scale Model of Turtle Visual Cortex. 
J. Computat. Neurosci. 14:161-184.

Vogels TP, Abbott LF. (2005) Signal propagation and logic gating in
networks of integrate-and-fire neurons. J. Neurosci. 25: 10786-10795.
