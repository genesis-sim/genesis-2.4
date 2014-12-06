Python example scripts
======================

This directory (python-tools/examples) contains some simple example
Python scripts and data files that were used in the January 2014
LASCON Python Tutorial.

The files
---------

*fplot.py* - A very simple script that uses Pylab to plot the
first two columns of a data file.  For example::

  fplot.py four_cells_Vm.txt

Compare it with *plotVm.py* in python-tools, which uses the Matplotlib
'pyplot' class.  For a fancier version, see *gipyplot.py* or *G3Plot.py*.

*filetest.py* - Another very simple script to illustrate reading
in a data file (e.g. four_cells_Vm.txt) line by line and extracting
data fields that are separated by whitespace (e.g. blanks or tabs).
The more advanced plotting scripts offer more detailed examples.

*pyr4a_Vm.txt* - time and Vm data from a current injection to the soma
of the ACnet2 layer 4 pyramidal cell model.

*four_cells_Vm.txt* - time and Vm data for the four cells in the
GENESIS 'four-RScells.g' simulation (described in the exercises below).

*four_cells_Isyn.txt* - time and synaptic current data from
'four-RScells.g'.

Exercises
---------

The file 'four_cells_Vm.txt' contains 5 columns with time and Vm for
the four cells cell[0-3].  The data is at 0.0001 sec intervals for times
0.0 through 0.25 sec, with a line for each time step.

* run and examine the simple scripts 'fplot.py four_cells_Vm.txt' and
  'filetest.py four_cells_Vm.txt'.  Understand other options to the commands.

* Modify filetest.py to print out Vm for cell[2] at 0.01 sec intervals
* Output the spike times (time at which Vm crosses 0.0 Volts) for cell[0]
* Print out the average interspike interval and standard deviation for each cell
* Print out a file containing one line for each cell that contains
  the spike times for that cell

**Possible project or advanced exercise**

Read enough of the documentation in ACnet2-GENESIS to
understand how to run the simulation ACnet2-main.g and
produce the files Ex_netview_B0003.txt and EPSC_netview_B0003.txt

Create a Python script to produce a file in the format of
python-tools/spike_times.txt that can be used in a raster plot.

Implement some of the other functionality of the GENESIS
'ACnet2-GENESIS/replay_netview10.g' script with Python scripts.

The examples/fourcells directory contains the GENESIS scripts
that were used to create the data files 'four_cells_Vm.txt' (soma Vm)
and 'four_cells_Isyn.txt' (Ex_channel current).  The simulation
creates four (or a number specified in the script) copies of the simple
regular spiking 'RScell" used in GENESIS tutorials.  Each receives
the same settable injection current, and a random background synaptic
activation at a default average frequency of 8 Hz.  This randomization
causes a desynchronization of what would otherwise be identical spike
trains.

The main script four-RScells.g has a GUI to change these parameters, and
others may be changed by setting strings defined at the beginning.
There is also an option (not used) to provide regular spike trains
as inputs to the cells.

