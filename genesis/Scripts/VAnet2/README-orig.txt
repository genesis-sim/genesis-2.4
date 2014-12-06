
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<HTML>
<HEAD>
<TITLE></TITLE>
<META NAME="generator" CONTENT="txt2html v1.28">
</HEAD>
<BODY>

<H1><A NAME="section-1.">README file for the GENESIS implementation of a network benchmark</A></H1>

<H2>1. Introduction</H2>

<P>
This directory contains the GENESIS implementations of the
Vogels and Abbott (2005) model using single compartment neurons with
Hodgkin-Huxley dynamics.  This was used as a benchmark for neural
simulators in the review by Brette et al. (2007), which gives further
details of the benchmarks.

<P>
Two versions of the benchmark are provided here.  The script
dualexpVA-HHnet.g implements a version that uses a dual exponential form of
the synaptic conductances, having a 2 msec rise time, with a 5 msec decay
for excitatory connections and 10 msec for inhibitory connections.  The
script instcondVA-HHnet.g, is nearly the same, but it uses synaptic
conductances with an instantaneous rise time, as specified by the
benchmark.

<P>
Both versions are based on simple modifications of the RSnet.g example from
the GENESIS Neural Modeling Tutorials (Beeman, 2005).  The latest versions
are available from <A HREF="http://www.genesis-sim.org">http://www.genesis-sim.org</A>.
The tutorial "Creating large networks with GENESIS" analyzes this script in
detail, and explains how to modify it for other network models.

<P> These simulations implement a network of 3200 simplified
neocortical neurons providing excitatory connections and 800
interneurons providing inhibitory connections, randomly
interconnected with 2% probability.  As specified by the benchmark, these
are simple one compartment neurons with Hodgkin-Huxley sodium and potassium
channels.  The excitatory and inhibitory neurons are identical, and fire
tonically without spike frequency adaptation.  Comments in the scripts and
the tutorial on RSnet explain how to modify the scripts to use more
biologically realistic neuron models and how to modify the network
connections.  The benchmark specifies that there be no axonal propagation
delays (nor even a location of neurons in space), but this implementation
allows optional delays through the GUI provided by the graphics.g file.


<H2>2. Running the simulations</H2>

<H2>2.1 Getting and installing GENESIS</H2>

<P>
To run the simulations, you must have the GENESIS simulator installed, and
in your search path.  You can download the latest GENESIS distribution from
the GENESIS Web site (http://www.genesis-sim.org/GENESIS), with
documentation and installation instructions. If you are using Linux, Mac
OS/X, or Microsoft Windows with Cygwin (a UNIX-like environment for
Windows), you may download the appropriate GENESIS binary distribution that
includes a precompiled "genesis" executable file. This will save you the
trouble of compiling GENESIS from the source code that is included with the
GENESIS source distribution.

<H2>2.2 Running the Dual Exponential VA HH Model script</H2>

<P>
Open a console window in this directory, and type

<PRE>
     genesis dualexpVA-HHnet.g
</PRE>
<P>
The CONTROL PANEL that appears has a toggle button to turn current
injection pulses on and off, and text fields to set parameters such as
maximal synaptic conductances, synaptic weight scaling, and propagation
delays, injection pulse characteristics, and the frequency of random
Poisson-distributed synaptic activation.

<P>
The plots in the middle will show the membrane potentials of three
excitatory neurons (0, 1536, and 1567), and inhibitory neuron 0.  The
netview displays at the right show the membrane potentials of the
excitatory neurons (top) and inhibitory neurons (bottom).  With no
propagation delays, the positions of the neurons on the grid are
irrelevant.  Nevertheless, this two-dimensional representation of the
network layers makes it easy to visualize the number of cells firing at any
time during the simulation.

<P>
The first figure in the GENESIS section of the paper shows the results of
running this simulation for 0.05 msec with a random background synaptic
activation applied to all excitatory cells at a mean frequency of 70 Hz.,
removing the activation, and then running for another 4.95 seconds.  This
figure is included here as <A
HREF="FigG1-dualexpVA-HH.gif">FigG1-dualexpVA-HH.gif</A>.

In order to produce the results shown in the figure:

<OL>
  <LI>Use the mouse to place the cursor in the text field to the right of
   "runtime (sec)", click to the right of the number displayed there, use
   the backspace key to delete the value, and enter "0.05".  NOTE: With
   GENESIS/XODUS, you then have to hit "Enter" to cause the new value to be
   recognized.
  <LI>In a similar manner, change the value of "Frequency (Hz)" from "0"
   to "70".
  <LI>Click on "RESET".
  <LI>Click on "RUN".
  <LI>When the simulation stops plotting, change the runtime to 4.95 and
   the Frequency back to 0.
  <LI>Click on "RUN" - do NOT click on "RESET".
  <LI>In order to see the plots for the full 5 seconds, instead of the
   default setting of 2 seconds, click on the "scale" button on each graph
   and use the menu that appears to increase the value of "xmax" to 5.0.

</OL>
<H2>2.3 Running the Instantaneous VA HH Model script</H2>

<P>
The instantaneous rise in the synaptic conductance that is used in the
Vogels-Abbott model and benchmark is a very efficient model to implement
with a simulator specialized for IF networks, but such a non-biological
conductance is not normally provided by GENESIS.

<P>
Fortunately, GENESIS has a simple procedure for adding user-contributed
objects and commands to extend it.  The user-contributed code for the
"isynchan" object in the isynchan directory adds a new GENESIS object
type called the "isynchan", which is used by the instcondVA-HHnet.g
script and its associated scripts.

<P> If you are using Linux, you may be able to run the executable binary
"isynchan/igenesis", without compiling the isynchan code.  Because of its
size, it is not included here, but is contained with the original version
on the GENESIS web site <A
HREF="http://www.genesis-sim.org">http://www.genesis-sim.org</A>, under
"Models" and at ModelDB as "GENESIS_HH_benchmark".  If you have problems
with libraries not being found, or are using another operating system, you
will have to follow the directions in <A
HREF="isynchan/README.html">isynchan/README</A> to create the igenesis
executable.

<P>
Once you have a usable "igenesis" you can run the instcondVA-HHnet.g
with the command,

<PRE>
     isynchan/igenesis instcondVA-HHnet.g
</PRE>

<P>
and proceed as before to get the second figure
(<A HREF="FigG2-instVA-HH.gif">FigG2-instVA-HH.gif</A>), which
displays the Instantaneous VA HH Model simulation results, generated under
the same conditions as the first figure.

<H2>3. Some other experiments with this network</H2>

<P>
<B>Introducing propagation delays:</B>

<P>
Turn on current injection by clicking on the the "Current Injection OFF"
toggle so that it reads "Current Injection ON".  With the default settings
of the injection fields, this will apply a 50 msec pulse of 1 nA injection
to the soma of the center cell of the excitatory cell grid.

<P>
The field labeled "Prop delay (sec/m)" is used to hold the reciprocal of
the axonal conduction velocity.  In the benchmark simulation, there is
an infinite conduction velocity with no delays, so this delay field is set
to zero.  In this case, the propagation delays of all synapic connections
are set to zero.  For a non-zero delay, the the conduction velocity will be
used to calculate delays based on radial distances to the target synapses.

<P>
Experiment with the effect of using different propagation delays, with
short injection pulses to the center cell.

<P>
<B>Using synapse_info to inspect connections:</B>

<P> You can explore the connections that are made by invoking the
'synapse_info' function at the genesis prompt. This function is defined,
with further explanation, in the file synapseinfo.g, which is included by
the main simulation script.  For example, to see a list of all the synaptic
connections to both the excitatory and inhibitory channels of the three
excitatory neurons for which the membrane potential was plotted, give the
commands to the genesis prompt:

<PRE>
    synapse_info  /Ex_layer/Ex_cell[1536]/soma/Ex_channel
    synapse_info  /Ex_layer/Ex_cell[1536]/soma/Inh_channel
    synapse_info  /Ex_layer/Ex_cell[1567]/soma/Ex_channel
    synapse_info  /Ex_layer/Ex_cell[1536]/soma/Inh_channel
    synapse_info  /Ex_layer/Ex_cell[0]/soma/Ex_channel
    synapse_info  /Ex_layer/Ex_cell[0]/soma/Inh_channel
</PRE>

<P>
How do these results relate to the relative amount of spiking produced
by these three cells?

<P>
You may alter the set of random numbers used (and therefore the connections
produced) by starting GENESIS first, giving the "randseed" command to
change the random number seed (to the system clock time, if no argument
is given), and then load the simulation script, e.g.

<PRE>
    % genesis
        (some startup messages)<BR>
    genesis #0 &gt; randseed<BR>
    genesis #1 &gt; dualexpVA-HHnet.g
</PRE>

<H2>4. Modifying the scripts to create other networks</H2>

<P>
The tutorial "Creating large networks with GENESIS" from the GENESIS Neural
Modeling Tutorials explains how to modify scripts based on RSnet.g for
other network models.  Simple changes to the scripts can substitute
different neuron models for the ones specified in the Ex_cellfile and
Inh_cellfile strings.  The arguments of the planarconnect and planardelay
commands can be changed to provide probabilities of connections or
propagation delays, that either depend on distance, or are assigned
according to various probability distributions.

<P>
<B>Here is a suggested project:</B>

<P>
Bush and Sejnowski (1996) constructed a realistic model of a single column
in layer V of the visual cortex.  The model used 80 9-compartment pyramidal
cells and 20 7-compartment fast spiking inhibitory basket cells, with
random 10% probablility all-to-all connections.  In spite of introduced
random variations in cells, it fires synchronously when the pyramidal cells
are given uncorrelated spike train inputs, with results similar to those
observed by Gray et al. (1992).

<P>
<B>Question:</B> To what extent do these results depend on the details
of the cell models and the network connections?

<P> Modify the dualexpVA-HHnet.g script to produce a network similar to the
one used by Bush and Sejnowski and try using the "VA_HHcell" used in this
script, or more realistic cell models that are available with the GENESIS
Modeling Tutorial.  Experiment with different ways to specify the
propagation delays, and with the GENESIS commands for introducing random
variations in neuron passive parameters and positions.




<H2>5. GENESIS tools for analyzing spiking networks</H2>

<P>
The GENESIS Reference Manual section on Objects describes several
"Device Objects" that can be used for spike train analysis:

<PRE>
  autocorr        Calculates histograms of auto-correlations
  crosscorr       Calculates histograms of cross-correlations
  interspike      Generates an interspike interval histogram
  peristim        Generates a peristimulus spike histogram
  event_tofile    Records event times; e.g spikes or threshold crossings
</PRE>
<P>

These objects can be used to generate histograms for analysis of spike
trains produced by the network, similar to those in Fig. 2 of the Vogels
and Abbott paper.

<H2>References</H2>

<P>
Beeman D (2005) GENESIS Modeling Tutorial. Brains, Minds, and Media. 
1: bmm220 (urn:nbn:de:0009-3-2206).  (http://www.brains-minds-media.org)

<P>
Brette R, Rudolph M, Carnevale T, Hines M, Beeman D, Bower JM, Diesmann M,
Morrison A, Goodman PH, Harris Jr FC, Zirpe M, Natschlager T, Pecevski D,
Ermentrout B, Djurfeldt M, Lansner A, Rochel O, Vieville T, Muller E,
Davison AP, El Boustani S, and Destexhe A (2007) Simulation of networks of
spiking neurons: a review of tools and strategies. J. Comput. Neurosci.
23: 349-398.

<P>
Bush P, Sejnowski T (1996) Inhibition synchronizes sparsely connected
cortical neurons within and between columns in realistic network models.
J. Comput. Neurosci. 3: 91-110.

<P>
Gray CM, Engel AK, Konig P, Singer W (1992) Synchronization of
oscillatory neuronal responses in cat striate cortex: Temporal properties.
Vis. Neurosci. 8: 337-347.

<P> Vogels TP, Abbott LF. (2005) Signal propagation and logic gating in
networks of integrate-and-fire neurons. J. Neurosci. 25: 10786-10795.

</BODY>
</HTML>
