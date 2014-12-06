      A Tutorial on Ion Channels in Bursting Molluscan Neurons

                  Implemented by David Beeman

This tutorial examines mechanisms which lead to burst firing in molluscan
neurons and the effects of the various types of ionic channels which are
commonly found.  The simulation is implemented within the 'Neurokit' cell
builder environment.  By using Neurokit, you will be able to modify most of
the relevant parameters and assumptions used in the model and perform
experiments to investigate the causes of many of the features seen in
molluscan pacemaker firing patterns.  This "README" file may be printed or
may be viewed by clicking on the 'help' button in the Neurokit title bar.

The cell which is modeled is a "generic burster", loosely modeled after the
Aplysia R15 cell, although it contains channel models taken from
measurements on bursting neurons in Tritonia and Ansidoris as well as
Aplysia.  This model would be appropriate at low temperatures (10-15 deg. C),
where a calcium-actived potassium current is believed to be the major
cause of the hyperpolarization between bursts [1].

An overview of most of the channels are described in the review paper by
Adams, Smith and Thompson [2] and in other references listed below.  Further
details of the tutorial and the implementation of the channel models will be
given in Chapter 7 of "The Book of GENESIS", to be published in early 1994
by TELOS/Springer.

SUMMARY OF THE MODEL

The model consists of a single compartment, a soma having a diameter of 250
microns.  The specific membrane resistance, RM = 4.0 ohm m^2 (gleak = 0.25
S/m^2), and specific capacitance, CM = 0.07 Farad/m^2, may be modified using
the "edit comparment" menu.  The six types of ionic conductances are
summarized in the table below.

Conductance                     Equation   gbar(S/m^2)   Erev(V)   Refs
===========                     ========   ===========   ========  ====
Na - fast sodium                 X^3 Y        138         0.050    3
K - late potassium               X^2           66        -0.068    4
B - slow inward "burst" (Ca)     X              5         0.068    5
K_C - [Ca] dependent potassium   [Ca] X       124        -0.068    6,2
Ca - high threshold calcium      X^2 Y         65         0.064    3
A - transient potassium          X^4 Y         82         0.068    7,4

The steady state activation (X) and inactivation (Y) parameters for each
channel may be viewed and edited by using the "edit cell" menu.  The
default maximum channel conductances (gbar) and reversal potentials (Erev)
may be modified from the "edit cell" menu.

RUNNING THE SIMULATION

In this tutorial, we will explain and use only a few of the basic features
of Neurokit.  If you wish to learn more about the advanced features, you may
run 'Neurokit' from the Scripts/neurokit directory and try some of the
examples which are suggested in the on-line help.  (Alternatively, you may
print out the Scripts/neurokit/README file.)

In order to allow room for the graphical display, move a terminal window
into the lower left hand corner of your screen.  (This is normally done by
using the left mouse button to click and drag on the title bar of the
window.)  After changing into the directory in which the simulation resides
usually "Scripts/burster"), type "genesis" to the unix prompt.  After the
"genesis" prompt appears on the screen, type "Neurokit", in order to load
the Neurokit simulation.  There will be a delay while the simulation is
being set up, and eventually, a title bar with options

    quit   help   file   run cell   edit cell   edit compt   edit channel

will appear at the top of the screen.  Click on the 'help' button, and this
file will be displayed.  (As usual, when we say "click on ..." we mean
"click the left mouse button on ...", unless another button is specified.)
When you feel that you are ready to begin, click on the "CANCEL" button at
the bottom of the help window.  The next time that you call up 'help', you
will return to the same place in the help file.

Click on the "file" option on the title bar.  The "file" menu will appear,
showing various 'dialog boxes' which indicate that we are running the
"mollusc" cell model simulation.  As the default values in the boxes are all
correct, click the "Load from file" button on the "file" menu.  A few obscure
messages will appear in your terminal window.  Then click "run cell" on the
title bar.  You should see the "SIMULATION CONTROL PANEL", and two "cell
windows", each containing a representation of the cell with a graph below.
The graphs have been initialized to plot the membrane potential at the left
and the internal concentration of calcium ions at the right.  (In our MKS
units, 1e-6 moles/m^3 = 1 nM concentration.)  In other experiments, we may
use the "scale" button in the upper left corner of the second cell window to
plot other quantities of interest, such as the current, conductance, or
activation variable for a given channel.

Initially, the 'Recording' button under the "ELECTROPHYSIOLOGY" heading will
be highlighted, indicating that you are ready to plant recording electrodes
in the cell.  Start by clicking the left mouse button in the round soma in
each cell window.  A recording electrode should appear in each cell and the
boxes labled "click_site1" and "click_site2" should each display
"/mollusc/soma".  Other buttons under the "ELECTROPHYSIOLOGY" heading
provide options for performing current clamp and voltage clamp experiments
and for applying various types of synaptic input to models which contain
synaptically activated channels.

We would like to begin by examining the endogenous bursting of this model in
the absence of any input, so go ahead and click on "run" in the SIMULATION
CONTROL PANEL.  The dialog box for "runtime" indicates that the simulation
will run for 40 seconds of simulated time each time you click on "run".  The
"refesh_factor" dialog indicates that the graph will be updated every five
time steps.  This gives a reasonable compromise between executon speed and
accuracy in depicting the details of the action potentials, but you will
notice that the action potentials will have irregular heights.  If you need
to examine individual action potentials in detail and don't mind the slower
performance, you may wish to set this value to one and decrease the time
step in the "clock" dialog box.

Once the run has finished, let's try to understand the what we see in the
two plots.  The simulation begins with all channel conductances initialized
to their steady state values when the membrane potential is clamped to the
nominal resting potential of -40 mV.  Of course, this cell is never "at
rest", so the intial burst of spikes will be slightly different from the
following ones.  After the end of the first burst of action potentials,
there is a gradual decrease in Vm, reaching a maximum hyperpolarization of
about -58 mV.  The membrane potential then begins to increase.  Once it gets
over about -50 mV, the rate of increase accelerates.  Once it gets to -40
mV, the cell begins firing again.  At this point, the calcium concentration
is at its minimum value.  During the burst of action potentials, the
concentration rises rapidly, reaching its maximum at the end of the burst.
It then begins a slow decline during the inter-burst interval.  (In this
model, the calcium concentration builds up at a rate which is proportional
to the B and Ca currents, and has an exponential decay with a time constant
of 17.5 seconds.)

We can understand this behavior by looking at the activation variables and
their time constants.  These may be most conveniently seen in Fig. 7.1 and
Table 7.1 in "The Book of GENESIS".  We can also examine them by clicking on
"edit channel" in the title bar.  This will bring up a Cell Window at the
lower left containing the green spherical soma.  Click on the soma.  It will
turn red, and icons representing the soma and the six channel types will
appear in the Compartment Window at the upper left.  Begin by clicking on
the icon for the B-current in order to select it.  The "gate" dialog box will
show the default gate to be examined, the X gate.  Position the cursor in
this box and hit "Return".  The resulting plots will reveal that the slow
inward B-current begins activating at about -50 mV, with a time constant of
about 2 seconds.  This should explain the buildup of the depolarization
which allows action potentials to be generated during the burst.  Also
examine the activation and inactivation (Y gate) for the Ca current.  When
is it likely to make a significant contribution to the Ca concentration?
Examine the activation of the C current.  The outward current due to this
channel will be proportional to its activation, the internal Ca
concentration, and the difference between the membrane potential and the Ca
reveral potential.  Can you use this to understand how the burst is
terminated and how the hyperpolarized interval comes to an end?

Later, you may wish to experiment with the effects of applying offsets ("ox"
and "oy" dialogs) or scaling ("sx" and "sy") of the activation curves or
their time constants.  

It may also be helpful to examine the various channel currents over the
course of the simulation.  We could use the second cell window to plot one
of the channel currents instead of the Ca concentration, but there is
another way which will let us see all six channel currents at the same time.
The "run paradigm" button in the SIMULATION CONTROL PANEL may be used to
execute some previously defined function which is not normally part of
Neurokit.  In this simulation, clicking on "run paradigm" calls up a window
containing plots for the six channel currents.  Examining these currents
before, during, and after a burst should give you further insight into the
interplay between the various conductances during this cycle.  Note that we
have used the GENESIS convention of plotting inward currents as positive and
outward currents as negative.  The "Hide Plots" button at the top left of
the Channel Currents window may be used to temporarily hide the plots from
view while the plotting continues.  These additional plots will slow the
simulation down considerably.  The "Delete Plots" button at the top right of
the window will disable plotting of channel currents completely.

Other experiments listed below will let you explore the effects of the various
ionic conductances on the firing patterns of the model neuron.

SUGGESTED EXPERIMENTS

(1) Some explanations or models of bursting in the Aplysia R15 neuron
    ignore the high threshold Ca current.  Explore the effects of
    eliminating this current by setting its maximum conductance to zero.
    You may do this by this by selecting "edit cell" from top menu bar.
    Click on the soma in the lower left window and then click on the icon
    for the Ca channel in the upper right window.  Change the "Conductance
    (S m^-2)" dialog from 65.2 to 0, and hit "Return".  Click on "run
    cell" in the title bar.  Finally, click on "reset" and "run".  Explain
    the reasons for the differences which you see.

(2) The Aplysia L11 cell is a repetitively discharging pacemaker ("beater")
    instead of a "burster".  Gorman and Hermann [8] have estimated that in
    the R15 cell, the conductance density for the B-current channels is 6
    times greater, that of the high threshold Ca channels is 8 times
    greater, and that of the K_C channels is 23 times greater than in the
    L11.  Use "edit cell", as described in the previous exercise, to
    transform the model cell to a "beater".
    
(3) When sodium channels are blocked with TTX, a slow oscillation in the
    R15 membrane potential is seen.  Set the Na conductance to zero and
    compare the period of the resulting slow oscillation to that of the
    bursts when the Na conductance is present.  How does it compare to the
    period of the bursts in Exercise (1)?  Can you explain any differences?
    (For these measurements, use the "scale" button on the graphs to
    increase the time scale to 80 seconds and run the simulation for
    80 seconds.)

(4) Examine the effect of the A-current by performing one of these two
    experiments.

    a) Use the default parameters for producing bursts and set the time
    scale for the Vm plot so that you can observe the second burst in
    detail.  Notice that the firing rate increases at the beginning of the
    burst and then decreases at the end, approximately following the
    pattern of the "slow oscillation".  After studying the results, set
    the channel conductance for the A-current to zero and repeat the
    experiment.  What differences do you see?  Also compare the shape of
    the action potentials with and without the A current.  Explain these
    results in terms of what you know about the properties of the
    A-current.  It will be useful to refer to the A-current
    activation/inactivation curves and their time constants.

    b) Alternatively, examine the effects of the A-current in the absence
    of the K_C, B, and the Ca currents by setting their maxiumum channel
    conductances to zero.  This will leave only the Na, K, and A
    currents present.  Without the inward B current, you will need some
    current injection in order to produce firing, so set the injection
    current to 1 nA.  (You may do this by clicking on "Iclamp", planting an
    injection electrode in the soma in the left Cell Window, and entering
    the current in the "inject" dialog box.)  Estimate the firing rate and
    repeat for 2, 3, 4 and 5 nA injections.  Then remove the A currrent
    and estimate the firing rate for these injection currents.  As in part
    (a), explain the differences in response and in the shape of the
    action potentials.

(5) In the "Squid" tutorial, we examined the phenomenon of "anodal break"
    (post-hyperpolariztion rebound).  Insert a current injection electrode
    into the cell as described above, but leave the injection current set
    to zero.  Run the simulation for about 15 seconds, so that it ends
    during the interburst interval.  Now set the injection current to -5 nA
    and the run time to 5 seconds.  After running for 5 seconds, restore
    the injection current to zero and run for another 15 seconds.  Explain
    the reasons for the differences between this experiment and the case
    with "squid-like" Hodgkin-Huxley channels.   (Hint: It may be useful to
    observe the the sodium inactivation variable during the course of the
    experiment.  You may plot it, rather than the Ca concentration, by
    clicking on the "scale" button in the right hand Cell Window (labeled
    "molluscxout2").  When the scale menu comes up, change the "colfield"
    dialog box entry to "Y" and the "fieldpath" to "Na".  Now use the
    "scale" button on the graph below to set "ymax" to 1.)


REFERENCES

[1] S. Thompson, S. J. Smith, and J. W. Johnson, J. Neurosci. 6: 3169-3176
    (1986)

[2] David J. Adams, Stephen J.  Smith and Stuart H. Thompson, Ann. Rev.
    Neurosci. 3: 141-167 (1980).

[3] D. J. Adams and P. W. Gage, J. Physiol. 289: 143-161 (1979)

[4] Stuart H. Thompson, J. Physiol. 265: 465-488 (1977)

[5] Stephen J. Smith and Stuart H. Thompson, J.Physiol. 382: 425-448 (1987)

[6] A. L. F. Gorman and M. V. Thomas, J. Physiol. 308: 287-313 (1980)

[7] J. A. Connor and C. F. Stevens, J. Physiol. 213: 31-53 (1971)

[8] A. L. F. Gorman and A. Hermann, J. Physiol. 333: 681-699 (1982)
ÿ