     Neurokit : Neuronal simulation environment (version 2.2)

		Developed by Upinder S. Bhalla May 1990
		Embellished by Erik DeSchutter Dec 1990.
		Further enhanced by Upinder S. Bhalla July 1991
		Additional changes by Dave Beeman May 1992, Feb 1993
		Conversion to GENESIS 2.0 by Dave Bilitch July 1995
		California Institute of Technology, 
 			Written under GENESIS/XODUS


1	INTRODUCTION.

This is our Neuron simulation environment, developed to facilitate 
simulations of single neurons. It provides the following facilities :
*	Running simulations of neurons with a variety of electrophysiological
	manipulations.
*	Storage of simulated data, for later analysis.

*	An interface to the neuron descriptor files, which concisely specify
	the geometry and electrical properties of neurons.
*	A means for editing the electrical and geometrical properties of
	neurons in the simulation.
*	A way of looking at the kinetics of most kinds of channel elements.
*	A facility, still under development, for cutting and pasting neuronal
	components to redesign the simulated neuron.

This is meant to be a functional system, and not a prettified demo program.
The script files are pretty grim, and are not good for novices to tinker
with.  Having made this Government Health Notice, I welcome enhancements and
bugfixes to neurokit. Please notify me of such changes, so I can incorporate
them into the next version.

At this point in the development of this utility, the recommended way of
designing new cells is to edit the cell parameter file in parallel with 
the simulations.  Simple changes can be made using the editing functions
in the utility, but global changes are not easy to do in this way. When
the cell has evolved enough in the course of tinkering with it, the new
version can be easily saved and loaded back in.

The parameters used for the prototype elements in the library are defined in
the prototype directory. Only those elements which are needed for a
simulation should be loaded in. At this point the prototypes cannot be
edited from within the utility. If you wish to define new prototype elements,
follow the instructions in prototype/README so as to keep things compatible.
The currently defined prototype channels are derived from a number of
sources, which are referenced in the prototype files. New channels (ie,
channel parameters based on experiments) for the library will be most useful.

The recommended way for serious use of neurokit is to set it up in a separate
directory, and to set your SIMPATH in .simrc to include the neurokit and
neurokit/prototypes directories at the end. Each cell simulation is typically
run from its own directory, with a local copy of userprefs.g customised to
set up the interface as you like it. All the parameters for userprefs are
initialised in the file defaults.g, which should NOT be altered.

The cell reading utility now works for a wide variety of channels. There is
a slight change in the syntax for complex, multi-element channels which
permits one to specify arbitrarily interlinked dependencies (such as on
[Ca]) using the standard GENESIS 'addmsg' format. Examples of this are in
the prototype library, especially some of the Ca channels.

2	A QUICK EXAMPLE

2.1	STARTING UP.

Type : genesis Neurokit

The credits will flash on the screen, and then it will say /library and sit
there for anywhere up to 5 minutes depending on your machine. (This is when
the library of prototype elements is being set up.)  Eventually a title bar
with options :

    quit  help  file  run cell  edit cell  edit compt  edit channel

will appear. Hit the 'help' button, and this file will be displayed. I
suggest that you proceed with these steps forthwith, and then you will be
able to read the instructions and try them out at the same time.  You can
move around the file using the scroll bar at the left.

2.2	LOADING IN NEURON DESCRIPTIONS.

Hit the 'file' button. A menu with several options will appear. The default
parameter file should be ok to use, so hit 'Load from file'. This will
read the file specified by 'source_file_name' into the cell specified by
'Cell for IO'. The menu will then vanish in a puff of bytes, and the
'run cell' will temporarily dissappear while the cell is being set up.

2.3	RUNNING THE CELL

Hit 'run cell', once it reappears. Three windows should appear : To the
left, a hideously complex menu with all sorts of options for running cells
and saving them. On the top, a picture of the cell itself. At the bottom
right of the screen, there is a graph which will contain the plots of
intracellular potentials.

The 'Record' button in the menu should be depressed. Using the LEFT mouse
button, click on a number of places on the cell displayed in the top window.
Each time, a representation of an electrode will appear, stuck into a
compartment. The MOUSE window in the top of the cell window indicates the
function of the 3 mouse buttons.

Hit the 'Iclamp' button on the main menu, and go to the cell and click
(again with the left mouse button) on the central compartment in the cell,
the soma. A black, two-tailed electrode will appear to represent current
injection.

Hit the 'run' button. The simulation will start off, with the cell turning
bright colors (hot representing high membrane potentials), and the graph
displaying plots of the potentials of the compartments you stuck the
recording electrodes into.

You can add and remove electrodes, both recording and stimulating, while
the simulation is in progress. 

2.4	EDITING CELL PROPERTIES

Hit 'edit cell'. A variety of windows open up. Except for the Parameter 
Window, all have a 'Mouse' window on top which tells you what the mouse
buttons do.

 ------------------------	---------------------------------
 |			|	|				|
 |    Library Window	|	|	Compartment window
 |			|	|				|
 ------------------------	---------------------------------

 ------------------------	---------------------------------
 |			|	|				|
 |      Cell Window	|	|	Parameter window
 |			|	|				|
 ------------------------	---------------------------------

The library window has a list of the available prototype elements defined
in the userprefs file. One can click-to-select as indicated by the Mouse
window for the library. The currently selected element is hilit in red.

The Compartment window has a representation of the currently selected 
compartment, and the ion channels associated with it. The currently selected
channel is outlined in red.

The Cell window has a representation of the current cell. The currently
selected compartment turns red.

2.4.1 Selecting compartments.
Using the left mouse button, ('Select', as indicated in the mouse window)
click on various compartments in the Cell Window. They turn red, and the
Compartment Window now represents the selected compartment. The parameter
window displays the properties of the selected compartment.

2.4.2 Selecting Channels.
In the Compartment Window, click on the channel you are interested in and it
should be hilit, and a brief list of channel properties will appear in the
Parameter window.

2.4.3 Editing parameters. 
Go to the 'Length' dialog in the parameter window. This represents the length
(in microns) of the selected compartment. Change it. The length of the
compartment in the cell window should change accordingly. Similarly, the
other parameters of the selected compartment and its channels can be
altered. 

Another way to change the geometry of a compartment is to go to the cell
window, and use the 'stretch' and 'rotate' buttons on the mouse. These
options work in the 3 orthographic projections : z, x, y. Select z (Looking
down from the z axis) by hitting 'z' in the cell window.  Now you can do the
stretch and rotate operations in the xy plane.  'stretch' brings the end of
the seleected comaprtment to the cursor position when the mouse is clicked.
(Thus, it may also do a rotation.)  'rotate' brings the compartment in line
with the cursor position, without changing its length.  Similarly you can
change to the xz plane by hitting 'y'. A list of the control command options
for the draw windows is given in section 3.2 below.  You can stretch and
rotate several compartments at once by clicking the 'select one compartment'
toggle in the cell window so that it says 'select sub tree'.  Now if you try
the select, stretch and rotate options you will see that they operate on all
sub-branches of the element you clicked on.

2.4.5 Cut and paste.
Set the toggles on the cell window to 'select one compartment' and
'cut_n_paste'. Copy a compartment from the cell into the library by
clicking the right button on the desired compartment. The library will
now have a new item in it, a duplicate of the compartment you just copied.
Select the compartment in the library by clicking on it using the left mouse
button. The name of the compartment appears in the 'sel' dialog in the library
window. If you were to try to paste this to the cell, it would complain that
the element already exists. There are two ways to get around this.

One is to rename the element in the library, by typing something else into
the sel dialog. So, change the name of the element to 'newone'.  Now do the
paste. 

The other method is to flip the 'Use sel name when pasting' toggle to 'Do
Rall renaming'. This activates automatic renaming of elements following the
Rall scheme, every time they are copied. This allows you to copy
compartments without having to manually rename them every time.  Paste a
couple of compartments on to see how this works. Check out the new names by
going back to 'edit shape' mode in the cell widget and selecting the new
compartments.

'Cut' works as you might expect.  Clicking the middle button on a
compartment in the Cell Window removes it.  Cut and paste also works, in a
similar manner, in the Compartment Window.  Again, you can't overwrite
existing elements.

2.5	EDITING COMPARTMENT PROPERTIES

This is essentially identical to the process of editing cell properties,
except that the parameters displayed are specific to the compartment
selected and are more detailed.

2.6	EDITING CHANNEL PROPERTIES

The 'edit channel' selection allows you to look at the channel kinetics of
voltage-activated channels.  The form displays alpha, beta, tau and minf.
Select a compartment (in the Cell window), then a channel (in the
Compartment Window).  The "gate" dialog must contain a valid activation or
inactivation gate name.  The default is "X", the name used for the
activation gate of hh_channels and tabchannels.  For hh_channels, the gate
names must be X or Y.  For tabchannels, they are X, Y and Z.  For
vdep_channels, the gate names are arbitrary, being assigned by the function
which creates the particular channel and its associated gates.  One
generally has to find these in the file in neurokit/prototypes which defines
the particular channel.

There are also dialog boxes for sx, sy, ox, and oy.  Buttons for alpha,
beta, tau, or m_inf allow you to apply these scaling and offsets values to
modify the appropriate curve.  However, hh_channels and vdep_channels cannot
yet be manipulated in this way.  In the present version of neurokit, only
tabchannels and vdep_channels can be scaled or shifted.  The tutorial in
'Scripts/channels' gives an illustration of this feature.

2.7	SAVING AN EDITED FILE

Hit the 'file' button on the title bar. This pops up the file menu. The
name of the file that your edited cell is going to be written to is in
the 'Save to file' dialog. As soon as you hit return, the file is written.
You could also change the 'Your Name' field so that the saved file has your
name as the author. Now go back and look at the new file and see how
your changes are reflected in the parameters of the cell.

NOTE:  At present, the writecell routine cannot handle all the features used
by readcell (the cell reader).  This means that the saved cell parameter
file may not contain all the information needed to fully specify the
the edited cell.  You may need to further revise the resulting cell parameter
file with an editor.


2.8	DISPLAYING AN EXTRA WINDOW ONTO THE CELL

When in the 'run cell' mode, hit the 'Show extra cell window' toggle.
This will cause the original cell window and graph window to shrink a bit,
and display another cell and graph window for the same neuron. This extra
window is very handy for looking at a different parameter from the one
in the first window, or zooming in onto a part of the cell while keeping
the whole cell in view in the other window. In order to activate this window
you need to hit 'reset'.

2.9	EXITING

Hit the 'quit' button. A menu will pop up asking if you are serious. If you
are, kaboom.

3	INFORMATION ABOUT THE 'run cell' ENVIRONENT

3.1	SIMULATION CONTROL WINDOW
The menu window 'CELL RUN CONTROL PANEL' provides a number of dialogs
(the yellow areas where you can enter values) and buttons (The red areas).

3.1.1
The top couple of lines are for controlling the running of a simulation.
'stop' stops a simulation in progress, 'reset' puts the cell back into
resting potential, 'run' causes a simulation to run, and 'run paradigm' runs
a user-defined function, 'do_paradigm'.  This script function may be defined
in your 'userprefs.g' file, or in a file which is included by 'userprefs.g'.
This enables you to set up a complex sequence of manipulations to be
performed during a simulation.  The 'nstep' and 'runtime' dialogs are
linked, and specify how long the simulation is to go, in simulation steps
and simulated time respectively.  The method dialog selects the integration
method. The following methods are currently available :

    -1 : Forward Euler. Simple, inaccurate.
     0 : Exponential Euler. Also simple, more accurate. The default.
     2 : Adams-Bashforth 2nd order predictor
     3 : Adams-Bashforth 3rd order predictor
     4 : Adams-Bashforth 4th order predictor
     5 : Adams-Bashforth 5th order predictor
    10 : Backward Euler. Implicit, quite accurate. Always stable
    11 : Crank-Nicolson. Implicit, accurate, stable. 

The last two methods involve the creation of a special element (hsolve) to
solve the matrix arising from the integration scheme, using Hines' method.
There are limitations on the manipulations one can perform when using these
methods.  In particular, elements should not be deleted from or added to the
cell.  The 'Scripts/int_methods' tutorial will let you compare the various
methods.  More information is given in the documentation for the hsolve
object.

3.1.2
The nstep and runtime fields are related by the duration of the simulation
clock 'clock' under the 'TIMING' category. The 'current_time' keeps track
of where the simulation is at the moment, and the 'refresh_factor' specifies
the number of steps that the simulation should go between graphics refreshes.
The latter should be kept large specially if you have a slow display,
otherwise the graphics will hold up the simulation.

3.1.3
'ELECTROPHYSIOLOGY' contains a number of things you can do to torment your
neuron. Any of these can be done before or while the simulation is run.  You
have already stuck recording and current injection electrodes into the
neuron.

You can change the injection of the last (and future) injection electrodes
by setting the 'inject' dialog.

The 'Vclamp' option does voltage clamping. See 3.2.4 to find out how
to plot and visualise the current used by the Vclamp.

The 'Syn act' option sets the synaptic activation on a particular
compartment. It is equivalent to squirting some transmitter (selected by the
mouse button) onto the selected compartment. This stuff will hang around
until you remove it by clicking with the middle mouse button.  Many channels
have long time constants - the delay in effects you see is not due to the
transmitter hanging around. The amount of transmitter is set by the 'Amount
of synaptic activation' dialog.

The 'Syn spike' option enables you to deliver a single pre-synaptic impulse
to the synapse each time you click the left mouse button on the
compartment.  The spike amplitude is determined by the 'Weight of spike'
dialog.

The 'Syn rand' options delivers random presynaptic input, at an average rate
set by the 'Spike rate' dialog.

3.1.4 
The 'Simulation IO' options are available by clicking on 'Show Simulation IO
Control'.  They set the options for saving and restoring simulation data
with the 'save' and 'restore' buttons near the top of the 'run cell' menu.
The 'filepath' is the name of the directory to which the output will be
sent. The 'file_index' is a suffix added to the files created by this
particular run (useful when running a simulation series).  The run-info
toggle decides whether or not a file containing some info about the
simulation (name of other files, time, notes) will be written. This file is
named according to the 'info_file' dialog.  Similarly, you can choose
whether or not to save the data from each of the recording electrodes you
stuck into the cell. The destination file is specified by the 'plot_file'
dialog.

Finally, you can save any desired field (dialog 'field') in any compartment(s)
(dialog 'field_path') of the neuron. The 'field_path' is the wildcard path
of the desired compartments. By default, Vm (the membrane potential) of 
every compartment in the cell will be saved. The destination file is specified
in the 'field_file' dialog.

3.1.5
Running paradigm files.
The 'run paradigm' button invokes the 'do_paradigm'
function, which may be defined by loading in a script file. Since a script
file is being used, just about anything you can code can be done in the
paradigm.  The 'Scripts/vclamp' tutorial provides an example.

3.2	CELL WINDOW
The top window is of the class 'Draw Widget', which is mentioned at length in
the documentation. This displays the selected cell geometry in 3d, and also
the membrane potential according to a hot colorscale where blue (cold) is
low potential (hyperpolarized) and yellow (hot) is depolarised.

3.2.1
Keyboard operations
You can do the following things in the draw widget, by pressing the control
key and a letter key as follows :

PAN COMMANDS	keyboard arrow keys		- up, down, left and right.
ZOOM COMMANDS	>, <				- in and out.
ROTATE COMMANDS	shift-arrow keys		- up, down, left and right.
						  (The side nearest you)
TRANSFORMATIONS	x, y, z, o, p			- Look along the x,y,z axes,
						  orthographic, perspective
PRINT		ctrl-p				- If you have a postscript
						  laser printer.
3.2.2
Button operations
You can also perform a number of mouse point-and-click operations, specified
by the green 'MOUSE' window with red buttons (to represent mouse buttons) in
the top right hand corner of the cell window. The 'MOUSE' window changes
according to the currently selected electrophysiology option.

3.2.3
Scaling the Cell window.
This permits you to alter a number of display parameters in the cell window.
Colmin and colmax refer to the range for the colors in the display. Autocol
does autoscaling for these two fields. Colfix prevents the colors from being
recalculated every timestep. Similar ideas apply to the Fat fields. Normally
the Fat parameters are fixed for the 'dia' fatfield in order to display the
dendritic thickness. The color fields are the ones usually used to display
data values.

3.2.4
Displaying other fields in the Cell window.
Another option is to display different fields, possibly on other elements
than the compartment itself. For instance, if you wanted to do a voltage
clamp on a compartment, you would be interested in the current being
injected by the clamp. This current is injected by the Vclamp/PID element on
the clamped dendrite, through a field called 'output'. The way to do this is
to set the 'colfield' to 'output'; and the 'fieldpath' to 'Vclamp/PID'.
Since the current is only being injected at those dendrites which are being
voltage clamped, the rest of the cell will have nothing to display, and will
turn black. You would, of course, have to adjust the colmax and colmin to
fit the range of the new field being displayed, or turn autocol on.
Existing recording electrodes will now be recording the new field, so the
parameter being displayed on the graph will abruptly change.

3.2.5
Fatrange
The fatrange is a display option for thickness of dendrites. Negative values
specify fixed thickness of the thickest compartment in pixels. Positive
values need to be used with care, since they specify the thickness of the
thickest compartment in meters. A large value will result in dendrites that
are fatter than the screen, and will take depressingly long to draw.
			
3.3 	GRAPH WINDOW
The graph window displays plots of membrane potential at the selected
recording electrode sites. By default, the traces are offset by 100 mV.
Like the cell window, graphs can be plotted out by hitting ctrl-p.

3.3.1 SCALING GRAPHS.
You can alter scales by hitting the 'scale' button at the top left.
It opens a menu with some obvious dialogs. The arrow keys scale the axis
named by 'field', in assorted and easily figured out ways. The selected
field may be changed by clicking on the 'field' button. The dialogs can also
be operated directly in the usual fashion. The new values will be assigned
to the graph when the APPLY or APPLY_AND_VANISH buttons are hit.

3.3.2 OVERLAYING PLOTS
The 'overlay' toggle does what it says, overlaying (or not) traces from
successive simulations.

3.3.3 new_colors
This button assigns new colors to new graph traces.

RECENT CHANGES

As of GENESIS version 2.2, Neurokit allows the faster and more accurate
implicit integration methods 10 and 11 to be used with cell models
containing symcompartments.

KNOWN BUGS

Under some circumstances, using "stretch" in the cell edit window can
produce a compartment with zero length.  This can produce a floating
exception error during the calculation of a cosine in function
do_stretch in the xgeom.g script.

Loading a new cell parameter file after using Neurokit with another
cell parameter file can cause errors.  It is best to quit and restart
Neurokit when changing cell models.

Some of the editing features do not work properly unless you have first
selected 'run cell' before switching to 'edit cell', or 'edit channel'.
It is not necessary to actually run the simulation.  Pasting of compartments
into the cell only works for compartments, but not for symcompartments.
