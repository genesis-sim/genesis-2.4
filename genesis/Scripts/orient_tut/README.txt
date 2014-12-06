***********************************************************************

	The Orientation Selectivity tutorial.
		Written by U.S.Bhalla
		Copyright 1989 Caltech.
	Converted to GENESIS 2.0 by Mike Vanier July 1995
***********************************************************************

New features added by Upi Bhalla for GENESIS 2.2:
    * The sweep velocity can now be negative
    * The basal retinal firing rate can be changed

	This tutorial is an example of how one would use GENESIS in a
complete simulation involving planes of simple neurons rather than a few
complex neurons. The simulation we are performing here is a simple and
simple-minded version of the Hubel and Weisel orientation selectivity
circuit over a small region of the V1 , with only a couple of orientations.
The input is in the form of either a horizontal or a vertical bar swept
across the field of view, and the output is the state of the assorted
components of the cells in the V1.

	The objective of this tutorial is to provide an example of
setting up a reasonably large network simulation, not to provide any
particular instruction into the wonders of orientation selectivity. I
would suggest that you start by running the simulation with the
following brief introduction to guide you, then try to dissect out how
the various display and setup functions are performed. There are a
large number (a dozen or so) of simulation files; don't be intimidated
by this. Most are well under a page in length, and the subdivision is
made so that each file specifies a particular aspect of the simulation
or display.  This simulation is used as an example in Chapter 17 of
"The Book of GENESIS" in order to describe the utility functions for
setting up large networks.  Chapter 20 is also relevant for understanding
some of the advanced XODUS features which are used here.

	NOTE: This tutorial writes two files (vert_disk and
horiz_disk) to the current working directory.  If you are loading the
tutorial files from a directory for which you do not have write
privileges (for example, /usr/genesis/Scripts/orient_tut), you should
cd to a directory of your own after the files have been read into the
simulator.

	Also Note: These scripts have been converted from the original
GENESIS 1 versions to use synchan elements (instead of the original
channelC2 and axon elements).  The display portions now use XODUS 2
widgets.

The following display options have not yet been converted for the GENESIS 2.0
release:  "the element tree display", "the connection display", and
"file-access graph".  These are not needed to run the network simulation,
and intended to illustrate certain features of XODUS.  Similar examples
may be found in the Scripts/examples/XODUS directory.

***********************************************************************

The simulation is divided into the following script files :

Simulation setup files...
	Orient_tut.g
	orient_sim.g
	constants.g
	protodefs.g
	retina.g
	V1.g
	ret_V1.g
	ret_input.g
	V1_output.g
	syn_utils.g

Interface setup files
	xorient.g
	xcontrol.g
	xtree.g
	xgraphs.g
	xout.g
	xconn.g
	icons.g

Orient_tut.g: calls the network setup file (orient_sim), its associated
utilities (syn_utils.g), and the x-interface setup file (xorient.g).

orient_sim.g : sets up the network, but no graphics, calling the
following files...

	constants.g : defines the constants to be used for the various
	simulation parameters.

	protodefs.g : defines the prototype cell compartments and functions to 
	be used later.

	retina.g : creates the retinal cells

	V1.g : creates the V1 cells

	ret_V1.g : performs the interconnections between the retina and V1.

	ret_input.g : specifies the pattern of inputs to the retina.

	V1_output.g : contains the output routines for the V1.


xorient.g : sets up the graphics for the network. This should only be
called after the network has been set up from orient_sim.g. It calls

	xcontrol.g : the control panel for the simulation

	xtree.g : Display for simulation hierarchy.  (Includes icons.g.)

	xgraphs.g : the graph display.

	xout.g : The output display.

	xconn.g : the connection display.

I have tried to keep all the script files small and readable, since
this is meant to be a learn-by-example tutorial rather than an
elaborate, prettified demo. A brief description of the function of the
script commands is next to them in the script files.
	
Many of the interface files (such as xtree) can be adapted
with little effort to other simulations.

***********************************************************************

	RUNNING THE ORIENTATION SELECTIVITY TUTORIAL

( 0 : 	Type 'genesis orient_all.g')

1	the control panel, 
2	the tree section
3	the graph section
4	the output section
5	the connection section
6	the command window. 

***********************************************************************

1. The control panel : Most of the buttons and dialogs on this should
   be self explanatory -- 
	The 'sweep_horiz' and 'sweep_vert' buttons cause the simulation to
		run, with either a horizontal or vertical bar swept across
		the "retina". You might have to cd to a write-permitted
		directory.
	The 'step' and 'nstep' widgets tell the simulation
		to perform one or several iterations. 
	The 'stop' button interrupts a simulation before all its iterations
		have been completed. The step or nstep commands will start
		from where you left off, but will not try to complete the 
		remaining steps in the interrupted simulation.
	The 'quit' button exits from the simulator.
	The current_time dialog monitors the simulation time. It can be 
		updated from the screen by clicking on it, or from within the
		script by means of a function call.
	The input pattern is controlled by the 'bar_vel' and
		'bar_firing_rate' dialogs.  The 'bar_vel' dialog changes
		the rate at which the input pattern (bar) is swept across the
		retina.  The default firing rate (500/sec) gives a modest
		increase over the background rate.  You may increase it to
		make the moving bar more obvious.
	The 'hide_xconn' toggle controls the display of the connection section.
	The 'hide_xout' toggle controls the display of the output section.
	The 'hide_xtree' toggle controls the display of the tree section.

***********************************************************************

[This selection is not yet available in the initial GENESIS 2.0 release.]

2 The tree section : This is useful when setting up a simulation to make sure
that the hierarchy is what you think it is. It also provides a means for
rapidly selecting and examining elements in the network. A third function is
to display the messages being passed around in the network (The blue arrows).

	The tree widget is a bit messy when seen from the root up, so the
	user can zoom in onto a particular subsection of the tree by
	clicking with the middle button on the element which is to become
	the base of the displayed portion of the tree. Another click with
	the middle button will restore the root-element at base of the tree.
	The third button toggles elements between 'truncated' (displayed as
	a half-tree icon), where all of the children are hidden, and fully
	displayed.  The main use of the widget is to assist rapid traversal
	and display of the tree. This is accomplished by the first button.
	A click with this button causes the clicked element to be selected
	as the current element in the simulation. In addition, if the
	'show_element' toggle is ON (hilit blue) then the command window
	will spew out all the fields of the element and their values, when
	the first button is clicked in the tree widget. This exactly as if
	you had entered the 'show *' command, and looking at the script you
	will see that this in fact is precisely what the click does.
	
***********************************************************************

3 The graph window. This illustrates the use of the graph in two modes:

First, the file-access mode where the graph takes a file from disk and 
displays the history of the selected cell.

	The index and filename dialogs apply only to the file_access graph.
They can be directly altered to select the desired plot, and they are also
changed every time an element is selected in the 'Activity Pattern' window.
To generate a plot for a particular cell, hit "Return" in the filename
dialog (or click on the dialog label), once the simulation has finished.
[This selection is not yet available in the initial GENESIS 2.0 release.]

	The x range  dialogs apply to both graphs. They set the time range
that the graphs display.

	The second graph is in the real-time simulation monitoring
mode. Several traces can be maintained at a time. The history of a
trace is maintained internally. This has the advantage of permitting
rescaling and refreshing of the widget, but unfortunately takes up a
lot of memory.  It is possible to use up all the memory of the machine
using the graph widgets, so beware!

***********************************************************************

4 The output window. This contains two view widgets and a few control buttons.
The first widget displays the input plane of receptors and is used to monitor
the input patterns. The second displays the activity of the  V1 horiz and
vert planes.  Although these planes in the simulation lie over one another, the
'offset' fields permit the user to move them around to clarify the display. See
the view documentation for details.
	The output window starts up displaying the following three planes
of elements : 
	In the Input Pattern view, the 'random-buffers' of the receptor cells
which basically take the input pattern given to them in the form of firing
rates and convert them into stochastic spikes. These are the red boxes,
representing spikes.
	In the Activity pattern view, the 'Vm' field of the 'soma'
compartment of the horizontally selective V1 cells. These are to the left.
	The 'Vm' field of the 'soma' compartment of the vertically selective
V1 cells are to the right. These two display the membrane potential 
using the selected colorscale.
	Listing the widgets in the output window, and their functions :

	The 'field' dialog selects which field you wish to display for
		the V1 elements in the view. Use the tree in the control 
		panel to 'show' the valid fields. Remember that numerical
		fields only are allowed.
	The 'soma','hh-Na','hh-K','exc-syn' and 'axon' 
		toggles are an interlinked set of toggle widgets which select
		the compartment of the V1 cells that will be displayed in
		the view widget. Note that only one of them can be on at any
		time.  Also note that the compartments have been assigned
		different 'z' fields during setup. If you toggle to, say,
		hhK, and it complains "field 'Vm' unknown" that means that
		there is no field Vm in the hhK channels. Try Ik instead.
		Now everything turns red - that is because the range of Ik
		is around 1e-6 while the current settings for the view are
		for Vm, -0.1 to 0.07 volts. So, hit 'autoscale'.....
	The 'autoscale' toggle permits you to switch the views between 
		autoscale on and off. This means that they will (or will not)
		find the minimum and maximum values of the displayed data,
		and assign the colors to the elements on the basis of this 
		range. See the view documentation for details.

	[Autoscaling is not yet available in this version of the simulation.]

***********************************************************************

[This selection is not yet available in the initial GENESIS 2.0 release.]

5 The connection window : Consists of the view widget for connections and a
few buttons for modifying connections. As started up, the axons of the
retina cells and the excitatory synapses of the V1 cells are being
displayed, with the axons to the left and the plane of vertical selective V1
cells in the top right. These positions have been set up using the 'offset'
fields for convenience in display, though the elements themselves all lie
over one another as specified in the simulation.

	The connection view widget : displays the connectivity pattern of
		the network. The way to think of the operation of the widget
		is as if you have injected a tracer dye at the point where
		you clicked (highlit by an asterisk). The strength (or
		delay) of the connection is indicated by the color coding,
		or size coding, or whatever display mode you have chosen.

	The 'anterograde | retrograde' toggle decides which direction the
		dye will go in. Blue is anterograde and red retrograde. 
	The 'weight | delay' toggle decides which field in the connection to
		display. If it is blue it will display the synaptic weight,
		otherwise the delay.
	The 'Set max weight' dialog allows you to change the synaptic weight.
		The weights are computed using an exponential decay routine,
		and the 
	'Weight decay' dialog sets the decay length (in cell spacings) of
		the weights.
Frequently the user wishes to know what the range of values for some
parameter in a view or conn pix is. This is computed every time the
image updates, if the 'autoscale' flag is TRUE. 'vmin' and 'vmax'
are the minimum and maximum values on which the output display scale
is based, and they return the range of values when 'autoscale' is TRUE.
See the xview documentation for details.

***********************************************************************

6 The command window. This is the window from which the simulation was
run. I recommend positioning this in the lower right hand corner of the
screen where there is a bit of space to see the thing. All the usual
SLI commands and display options are available from here. In addition,
some of the widgets send output to the command window.

***********************************************************************
	
	The objective of this tutorial is as much to introduce you to some
tricks of the trade when designing an interface as to illustrate the
process of setting up a network. In particular, the following nifty
tricks are worth familiarizing yourself with :

	Control panel :
	The use of toggles to hide and show widgets so as to keep the 
	screen uncluttered.

	Tree window
	The use of the element tree to select (ce) and show an element
	in the simulation.
	The use of different click buttons (as in the element tree) to
	perform different operations in the same widget.

	Graph window : 
	Rescaling graphs from dialogs.
	Using the offset fields for plotting multiple traces in the same 
	graph

	Output window :
	The use of dialogs to select the field of a view.
	Using views to select which element's history a graph will display.
	Creating and using a linked set of toggles to select one of several
	options.

	Connection window :
	Using offsets in conns and views to display things in a less
	cluttered fashion.
	Using toggles to perform flag settings and selection between two
	options, for multiple elements (in this case there were two 
	conn pixes whose fields had to be changed)
	
It should be pointed out that all the tricks are just quick ways of
doing things which could be specified from the SLI. But then, the
whole point of using the graphical interface is to make it quick and
easy to manage a simulation.

***********************************************************************

	The tutorial uses a number of widgets that will probably be unfamiliar
to you. The best way to learn about them (and their limitations) is to 
set up the simulation, and then 'ce' to the widget of interest and start
messing around with the fields in the widget. Preferably with the 
documentation in front of you so you aren't working in the dark.

***********************************************************************
