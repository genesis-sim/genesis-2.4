Integration methods tutorial for Neurokit.
By Upinder S. Bhalla, July 1991, Caltech.

This directory contains a userprefs file and two cell parameter files to
illustrate the effects of using different integration methods in Genesis.
The simulations described here run with neurokit version 1.3 or later.  You
should use a ".simrc" file which sets a SIMPATH which includes neurokit and
neurokit/prototypes.

--------------------------------------------------------------------

Steps

1 start genesis			type : genesis

2 start neurokit		type : Neurokit

3 load soma			click : file
				click : load from file
				click:  run cell
	
4 Put recording electrode	click : soma of cell in Cell window.
in soma

5 Put current injection		click : Iclamp toggle on control panel 
electrode in soma		click : soma of cell in Cell window.

6 Select overlay mode		click : overlay toggle on graph widget
	for graph widget

7 reset simulator		click : reset button on control panel

8 run simulation		click : run button on control panel

	
--------------------------------------------------------------------

		Wait till simulation stops. There should be 3 spikes.
		In order to try another method :

9 change integration 		Enter : new method (0, 2 to 5, and 10 and 11)
method					in Method dialog on control panel

10 change plot color		click : new_colors button on graph widget

11 Reset and run simulation	click : reset, then run button on control
					panel

		Repeat steps 9 to 11 till you have tried out all the 
		integration schemes. The 'help' screen lists them all.
--------------------------------------------------------------------

The following methods are currently available :
-1 : Forward Euler. Simple, inaccurate.
0 : Exponential Euler. Also simple, more accurate. The default.
2 : Adams-Bashforth # 2
3 : Adams-Bashforth # 3
4 : Adams-Bashforth # 4
5 : Adams-Bashforth # 5
10 : Backward Euler. Implicit, quite accurate. Always stable
11 : Crank-Nicolson. Implicit, accurate, stable.

The last two methods involve the creation of a special element to solve the
matrix arising from the integration scheme, using Hines' method. There are
limitations on the manipulations one can perform when using these methods.
In particular, elements should not be deleted from or added to the cell.

--------------------------------------------------------------------

		It is instructive to see how the different methods
		fare with different timesteps.

12 Increase timestep to		Enter : 100e-6 in the 'clock' dialog.
100 microseconds

13 Increase the screen 		Enter : 1 in the 'refresh_factor' dialog.
update rate to compensate

		Go back to steps 9-11 to see how the integration methods
		handle this much longer timestep.
--------------------------------------------------------------------

Exercises :

1 Even if a timestep works with one integration scheme on a particular
simulation, it may not be satisfactory for another. Estimate the timestep
one would need for the detailed granule cell model, using an explicit method.
Compare this with an implicit method.  (If you decide to load aand run the
granule.p cell, be prepared for a long wait - this passive cell model has
944 compartments.)

2 Try to benchmark the various integration schemes. This should be done without
starting up XODUS, since the graphics puts a large overhead on simulation time.

Also see the scripts in Script/examples/hines.


