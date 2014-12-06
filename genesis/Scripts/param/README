This directory contains demonstrations of the parameter search library
developed by Mike Vanier, and first introduced in GENESIS 2.2.  For an
introduction to parameter searching with GENESIS, see the documentation
for ParameterSearch and Paramtable.

The directories BF, CG, GA, SA, and SS contain demonstrations of the five
optimization algorithms:  Brute Force search over every point on a grid,
coarsely sampling the parameter space; Conjugant Gradient; Genetic
Algorithm; Simulated Annealing; and Stochastic Search.

Each directory has a README file that gives more details about the
demonstration and how to run it.  As explained in these files, the results
are written to files, instead of wasting computation time on graphics.
You may view the progress of the searches by comparing the file listing
the current "best match" values of the parameters ("best.params") with
those of the original model ("goal.params").

THE CELL MODEL

The cell model used in these demonstrations is a single soma compartment
containing three active conductances: fast Na, delayed-rectifier K (Kdr),
and the slow, noninactivating K (M-current, KM) that is responsible for
the strong spike frequency adaption seen with current injection.

The original model is specified by the cell parameter file "origcell.p"
and the file "origchannels.g" containing the functions that create the
prototype channels.  You if you wish, you could experiment with this model
by writing your own GENESIS script based on Scripts/tutorials/tutorial3.g.
Like the variation in tutorial5.g, it would create the cell by using the
readcell command to read the cell parameter file and create channels made
from the prototypes.  Or you could use these files with Neurokit and your
own userprefs.g.  By varying the injection current, you would see
different spike trains corresponding to different currents.

In these demonstrations, the injection is a pulse applied from a pulsegen
element with delay 0.050 second, duration 0.500 seconds, and a total run of
0.600 seconds.  The simulation is run and the membrane potential (Vm) is
recorded for current steps of 0.10, 0.20, 0.40, 0.60, 0.80, and 1.00 nA.

The behavior of the original model serves as a target for parameter
searches that start from a modified model with different parameters.  The
model with these "non-optimum" parameters is specified in the files cell.g
and channels.g.  The goal is to vary the parameters of the modified model
in order to get the best match to the spike times produced by the original
model with these current injection steps.  Of course, in an actual use of
parameter search to find the best model parameters, you would use actual
experimental data instead of the output of the original model for the
target data.

The parameters to be varied are

   a) Gbar (maximal conductance) of the Na channel
   b) Gbar of the Kdr channel
   c) Gbar of the KM channel
   d) Scaling factor for the time constant of the KM channel activation
   e) Midpoint of the activation curve of the KM channel

The file "goal.params" contains the parameter settings (scalings and
translations) that will give rise to the target model.  At various points
during the search, the file "best.params" is updated to give the parameter
modifications that result in the best match of the modified model to the
original model.  Thus, a succesful search results in a close agreement
between the values given in these two files.

The criteria for determining when the search is done are specified by
setting fields of the the paramtable element for the particular method.
These fields will vary from method to method, and are described in the
documentation for the specific paramtable object (e.g. paramtableBF,
paramtableSA, etc.)  The script params.g is used to set the paramtable
fields, and the script search.g contains algorithm-specific functions
for running the parameter search.

Most of these searches will take a few hours to run on a moderately fast
computer (e.g. a 400 Mhz Pentium).  The individual README files give
instructions for stopping the search at an earlier point.

VIEWING THE RESULTS

The genesis/src/contrib/xplot directory contains a README file, source
code, and a Makefile for the xplot program, a plotting program originally
written by Matt Wilson, and modified by Mike Vanier.  This program (which
is unrelated to the GENESIS object of the same name), may be used to view
the results of the parameter fit and to compare them with the target data.

If you compile and install xplot, it may be used to compare the output of
the current "best match" model during the injection steps ("Vm.best") with
that of the original model ("Vm.data").  As explained in the README files
for each demo, this is done with the command "make view" that indirectly
gives the command

  cat Vm.best Vm.data | xplot -geometry 800x600

The file genesis/src/contrib/xplot/xplot.txt (NOT to be confused
with Doc/xplot.txt!) describes the many commands and options that can be
used with xplot.  If you prefer another plotting program like gnuplot,
you could undoubtedly issue appropriate commands for that package.
(For extra credit, write a GENESIS script to generate plots comparing
the outputs, then send it to babel@genesis-sim.org).

The plot produced by "make view" shows overlayed plots of the cell membrane
potential Vm for the different injection steps.  The upper graph is for
the best match, and the lower one is for the original model.  Type a
Ctrl-D inside the plot to exit.

At intervals during the search, the current best fit parameters are used
to generate and record the membrane potential Vm in the file "Vm.best"
during the sequence of current injection steps.  However, you must
generate the file "Vm.data" for the original model yourself.  This may be
done by giving the command "make orig", before you begin the search with
"make search".
