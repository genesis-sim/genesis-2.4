                         THE NEURON TUTORIAL
Introduction
~~~~~~~~~~~~
"Neuron" is an introduction to single neuron behavior which was developed
for use in an interdisciplinary upper-division undergraduate course on
modeling of biological and connectionist neural networks at Harvey Mudd
College and for an introductory graduate level course in Caltech's
Computational Neural Systems program.  Our goal was to make the GENESIS
simulator accessible to students who are new to the X windows workstation
environment and are unfamiliar with the GENESIS script language.  The
menu-driven approach used allows a wide variety of situations to be
investigated by using the mouse to click on "control buttons" displayed on
the screen, or by entering values in "dialog boxes".  Readily available
on-screen help allows the tutorial to be used with a minimum of external
documentation.  The file "exercises.txt" gives some suggested exercises
which might be assigned when the tutorial is used in a course.  The
instructions below for running the simulation may be edited and used as
part of the instructions to the student.

Setting Up the Simulation
~~~~~~~ ~~ ~~~ ~~~~~~~~~~
If GENESIS has not yet been installed on your machine, refer to the main
README file accompanying GENESIS or the printed documentation for GENESIS.
The paths to necessary files should be set up as specified in the GENESIS
README file, the necessary ".simrc" and ".Xdefaults" files should exist in
your home directory and X-windows should be started.  The help system for
the tutorial makes use of some new graphical widgets which were not
available in earlier distributions of GENESIS, so you should make sure that
your version is sufficiently up-to-date.  You may do this by noting the
dates displayed with the messages telling which libraries were loaded when
you start GENESIS. The libraries com, draw, gen, widg and xo should be dated
May 15, 1990 or later.  Once these conditions have been met, you may run the
tutorial by performing the steps listed below.

Running the Simulation
~~~~~~~ ~~~ ~~~~~~~~~~
o Create an xterm window at the bottom left of your screen.

o Move the cursor into the window.  (This will be necessary whenever you
  want to direct keyboard input to a window or to type something into a
  "dialog box".)

o "cd" into the directory containing Neuron.g and associated files.  (For
   instructional use in a course, it may be desireable to accomplish this
   and the creation of the xterm window with a unix shell script.)
   
o Type "genesis".

o After the "genesis #0>" prompt appears, type "Neuron".  (Don't forget
  that unix is case sensistive!)

o After the graphs and "control buttons" appear on the screen, click the
  left mouse button on the box labeled "HELP".  A help menu should appear.

o The various items on the help menu should tell you everything you need
  to know.  Start by clicking on "Using Help" and follow the directions.
  Be sure to look at "Running the Simulation".  "Neuron Inputs" and
  "Things to Try" will also be useful.

o When you have finished, and are ready to leave the simulation, click on
  the "QUIT" button and verify by clicking on "yes".  The graphs will
  disappear and a number of irrelevant warning messages may appear.

o Type "exit" to leave GENESIS.

o Type "exit" again to close the window.

Implementation Notes
~~~~~~~~~~~~~~ ~~~~~
This section contains a few hints for those who are interested in
understanding the scripts associated with this tutorial.  The main script,
"Neuron.g", and its included ".g" files borrow many ideas from the MultiCell
demonstration program which is distributed with GENESIS.  MultiCell comes
with extensive documentation describing the syntax of the GENESIS scripts
which are used.  Once you have understood the workings of MultiCell, the
scripts used by Neuron should be easier to understand. There are a few new
GENESIS commands and objects used in Neuron which you might note.  The
included file "make_cable.g" makes use of the getmsg and deletemsg commands
in functions which delete passive cable compartments between the two
dendrite compartments.  "inputs.g" makes use of the pulsegen and diffamp
objects in order to provide injection curents and to provide spike trains to
the synapses.

