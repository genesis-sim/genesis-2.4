Voltage clamp tutorial using Neurokit.
		By Upinder S. Bhalla, March 1991

This directory has paradigm files for doing a voltage clamp analysis on ion
channels.  It is meant as a tutorial on how neurokit can be used to simulate
electrophysiological experiments.  Familiarity with neurokit is assumed.  Be
sure that your .simrc file sets SIMPATH to include neurokit and
neurokit/prototypes.


(Note that the sign convention for channel currents used in Genesis
corresponds to the electrical circuit equivalents : i.e. a depolarizing
current is positive. One can easily flip the sign to correspond to
electrophysiological conventions using the PLOTSCALE option in the message to
the graph. See the graph documentation.)

Steps :

1 Start genesis       type : genesis
2 Start neurokit      type : Neurokit
3 Load soma          click : file
		     click : Load from file
4 select run cell    click : run cell
	(Two cell windows with a single round compartment in each will pop
	up, with the usual control window)
5 Record from cells  click : soma in both windows
6 Set voltage clamp  click : Vclamp on SIMULATION CONTROL PANEL
                     click : soma in second cell window
                     click : reset on the control panel
7 Read paradigm		  type : include paradigm
8 Run vclamp series  click : run paradigm
	(The left window represents the currents flowing through the 
	channel, and the right is the voltage clamp being applied.)
This paradigm gives a series of depolarizing voltage steps from a
fixed holding potential. Another common paradigm is to have a range
of holding potentials and step to a fixed value.

9 Clear the graphs   click : overlay on both graph windows
					 click : reset on the control panel
10 Read paradigm2     type : include paradigm2
11 run Vclamp series click : run paradigm
