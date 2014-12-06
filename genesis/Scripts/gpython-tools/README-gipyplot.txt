README for gipyplot.py
======================

Background
----------

The original G-3 'g3plot' was implemented as a g-tube component by
Mando Rodriguez, based on a stand-alone program G3Plot.py, written
by David Beeman.  G3Plot.py is found in::

    ~/neurospaces_project/gshell/source/snapshots/0/bin
    ~/neurospaces_project/g-tube/source/snapshots/0/gtube/plot

*G3Plot.py* is a plotting program that wraps Matplotlib plots within a
wxPhython GUI.  This has several fancy Help menu features and plotting
options to illustrate the capabilities of wxWidgets as a GUI for displaying
simulation results in Python.  It can either take the filename list as
arguments, or to be entered in a dialog from the File/Open menu selection.
It can overlay plots of data from multiple files and provide labels,
but does not have the ability to plot multiple columns of data.
To avoid confusion, the next version of G3Plot.py will be renamed to
g3plot-gui.

*g3plot* added the capability of adding multiple plots from multicolumn data 'x, y1,
y2, ...'  given at successive x-values, given in one or more files.
Wildcard file names are accepted, and the data is taken from all data
columns of all files.  The GUI is the basic Matplotlib Figure with
Navigation Toolbar, without the menus of G3Plot.py, but it allows
unix-style short and long command line options to be specified.

The original g3plot was (under Linux with G-3) installed in
/usr/bin/g3plot, as a very short Python script that references two other
files.  The separation of the functionality of g3plot into several
distributed files does not add any generality, and makes it difficult to
run from any location.  gipyplot.py is a backwards-compatible replacement
that can simply be copied into /usr/local/bin or any other location, or be
run under ipython or the gipyshell.

NOTE:  ipython can run it with the '%run" command, but it does not
return control to ipython until the window is killed.

In addition to code cleanup and documentation, improvements were added to
the option parsing and help messages, previously non-functional command
options were implemented, and others were added for axis scaling.  The
current options are::

    $ g3plot2.py -h
    Usage: %s [OPTIONS] <files>

    g3plot creates a frame with a toolbar that holds a G-3 graph (a 2D y vs x
    plot).  The graph allows multiple plots from multicolumn data 'x, y1, y2,
    ...' given at successive x-values, given in one or more files. Wildcard
    file names are accepted.

    Options:
       --version             show program's version number and exit
       -h, --help            show this help message and exit
       -v, --verbose         show verbose output
       - V, --show-version    print the program version
       -t TITLE, --title=TITLE
                        Title for the graph
       -f FORMT, --format=FORMT
                        The plot format and color. e.g 'k'
                     	default: None cycles color for multiple plots
       -x XLABEL, --xlabel=XLABEL
                        The label for the X axis
       -y YLABEL, --ylabel=YLABEL
     			The label for the Y axis

    Axis Range Options:
    Autoscaling is used unless both min and max values are given:
        --xmin=XMIN    Minimum value for the X axis
        --xmax=XMAX    M aximum value for the X axis
        --ymin=YMIN    Minimum value for the Y axis
        --ymax=YMAX    Maximum value for the Y axis

Example
-------

::

  $g3plot2.py -v -t 'Purkinje RUN: 1234' -x seconds -y volts edsjb1994_pulse_inject.out
  xmin, xmax, ymin, ymax:  None None None None
  Processing 1 files.
  Plotting file: /home/dbeeman/info/python/GXgraph/edsjb1994_pulse_inject.out
	Plotting data set 1 of 5
	Plotting data set 2 of 5
	Plotting data set 3 of 5
	Plotting data set 4 of 5
	Plotting data set 5 of 5

Dave Beeman
Tue May 14 17:21:21 MDT 2013
