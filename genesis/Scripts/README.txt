		GENESIS tutorials and examples

The directories beneath genesis/Scripts include a series of simulations
designed to be run under GENESIS.  In each case, you will need to change
into the appropriate directory and follow the directions given in the
README file for that directory.

These simulations fall into two categories.  The "neuroscience tutorials"
and their README files are:

* squid: squid/README.txt, squid/squid.txt
* cable: cable/README.txt
* neuron: neuron/README.txt
* burster: burster/README.txt
* traub91: traub91/README.txt
* CPG: CPG/README.txt
* piriform: piriform/piriform-help.txt
* purkinje: purkinje/README.txt

These  simulations were designed as interactive tutorials for
teaching concepts in neurobiology and neural modeling.  As their use
requires no knowldge of GENESIS programming, they are suitable for use in a
computer simulation laboratory which would accompany upper division
undergraduate and graduate neuroscience courses.  Each of these has on-line
help and a number of suggested exercises or "experiments" which may be
either assigned as homework or used for self-study.  These tutorials may
also be taken apart and modified to create your own simulations, as they are
reasonably well commented.  Several of them are derived from existing
research simulations.

Further details of these tutorials and the theory behind them are given
in "The Book of GENESIS" by James M. Bower and David Beeman.  The file
"../Doc/BoG.txt" provides more information about this book.

The tutorial on single neuron behavior in "Scripts/neuron" is a good one to
start with, as it has fairly extensive built-in help available.  To run it,
give the command "genesis Neuron" and click the left mouse button on the box
labeled "help" once it appears.

"Scripts/squid" contains a simulation of voltage and current clamp
experiments on the giant squid axon.  The tutorial also provides the ability
to block channels, to plot the Hodgkin-Huxley activation variables, and to
generate state plots.  The accompanying exercises help the student
understand the Hodgkin-Huxley model and the procedure used to calculate rate
parameters from voltage clamp experiments.  Other exercises elucidate the
basis of post-inhibitory rebound and refractory period.  Documentation is
contained in the file squid/squid.text.  Give the command "genesis Squid" to run
the tutorial and then click on "help".

The "cable" tutorial simulates an extensible neuronal cable.  Current
injection or synaptic input may be provided to any one of the compartments,
and all relevant parameters are adjustable from "pop-up" menus.

The "burster" tutorial examines mechanisms which lead to burst firing in
molluscan neurons and the effects of the various types of ionic channels
which are commonly found.  The simulation is implemented within the Neurokit
cell builder environment and can also serve as an introduction to Neurokit.
This allows the user to modify most of the relevant parameters and
assumptions used in the model and to investigate the causes of many of the
features seen in molluscan pacemaker firing patterns.  The cell which is
modeled is a "generic burster", loosely modeled after the Aplysia R15
cell, although it contains channel models fitted to measurements on bursting
neurons in several different molluscs.  The README file gives instructions
for running the tutorial.

The "traub91" tutorial is based upon a GENESIS recreation of the hippocampal
pyramidal cell model of Traub, et. al.  [R.D.Traub, R. K. S.  Wong, R.
Miles, and H. Michelson, Journal of Neurophysiology, Vol. 66, p.  635
(1991)] This tutorial also uses Neurokit to explore some of the properties
of the model.  The file "traub91proto.g" may also be used as an example of
the implementation of Ca concentration dependent channels in GENESIS.  The
README file gives instructions for running the tutorial.

"CPG" is a tutorial on central pattern generator circuits by Sharon
Crook (U. of Maryland).  The simulation is designed to allow you to
study the activation patterns and emergent behavior of a simple
neuronal network of four cells.  The simulation allows you to set any
pattern of connections between the cells, using either excitatory or
inhibitory connections, and to vary the cell and channel parameters.

The "piriform" directory contains a "user-friendly" adaptation of
the research simulation used by Wilson and Bower to model the processing of
olfactory inputs to the piriform cortex.  This scaled-down version of the
original simulation uses 135 each of pyramidal cells, feedforward and
feedback inhibitory interneurons, arranged in a 9 x 15 array.  To run the
simulation, type "genesis Piriform" and click on the box labeled "help" when
it appears.

The "purkinje" tutorial is based upon a GENESIS simulation of a cerebellar
Purkinje cell, modeled and fine-tuned by Erik de Schutter. The tutorial
assumes that you have a basic knowledge of the Purkinje cell and its synaptic
inputs. It gives visual insight in how different properties as concentrations
and channel conductances vary and interact within a real Purkinje cell.

--------------------------------------------------------------------------

The "demos" are intended as examples of simulations which can be built
with the package.  The demosand their documentation currently include:

* tutorials: tutorials/README.txt
* MultiCell: MultiCell/README.txt, MultiCell/README.txt
* orient_tut: orient_tut/README.txt
* neurokit: neuroktit/README.txt
* kinetikit: kinetikit/README.txt
* channels: channels/README.txt
* vclamp: vclamp/README.txt
* int_methods: int_methods/README.txt
* examples: examples
* newlib: newlib/README.txt
* param: param/README.txt
* chemesis: chemesis/README.html, chemesis/CalTut.html
* gpython-tools: gpython-tools/README.html, gpython-tools/README-gipyplot.html
* stdp_rules: stdp_rules/README.html
* VAnet2: VAnet2/README.html

The Scripts/tutorials directory contains the GENESIS scripts which are
listed in Appendix B of "The Book of GENESIS".  These scripts are used
in Part II of the book with tutorials which guide the user through the
process of constructing GENESIS simulations.

The Orient_tut simulation (in Scripts/orient_tut) is a simplistic version of
the Hubel and Weisel orientation selectivity model over a small region of
the V1.  It is used as an example in Chapter 18 of "The Book of GENESIS" in
order to describe the utility functions for setting up large networks.
Chapter 22 is also relevant for understanding some of the advanced XODUS
features which are used here.  It does not have built-in help, but has
a detailed README file.

"MultiCell" simulates two simple neurons in a feedback configuration.  This
simulation, written by Matt Wilson in 1989, illustrates the "old" way of
making connections between neurons with the axon and channelC2 objects used
in GENESIS versions 1.x.  These scripts have been converted to GENESIS 2.0
with the "convert" utility and make use of the X1compat (XODUS 1 compatible)
widgets and the Connection Compatibility (oldconn) library.  It thus serves
as an example of the conversion of a GENESIS 1 simulation to GENESIS 2.

The "neurokit" directory contains one of the most elaborate of the
simulations.  It provides a number of tools for building and editting
complex neurons and uses a sophisticated graphical interface.  Although the
scripts are quite complicated, the program is easy to use.  Type "genesis
Neurokit" to start the simulation and, after a while, a command bar will
appear at the top of the screen.  Click the left mouse button on "help" and
follow the directions. 

The "kinetikit" directory contains the latest of our "kits" for building
simulations with a graphical interface.  This program is used for modeling
biochemical kinetics reactions, and provides a convienient interface for the
use of the objects in the kinetics library.  See the README file in this
directory for details, or plunge ahead and type "genesis Kinetikit" from
within this directory.

The files in genesis/Scripts/channels provide an introduction to the
channel editing facilities provided by neurokit, and illustrate the use of
the library of prototype channels. This tutorial should be run after the
user has tried out Neurokit itself.

The genesis/Scripts/vclamp directory has paradigm files for doing a
voltage clamp analysis of ion channels. It is meant as a tutorial on how
neurokit can be used to simulate electrophysiological experiments.
Familiarity with neurokit is assumed.

The Scripts/int_methods tutorial is a tutorial on the different integration
methods accessible through neurokit and available for use in your
simulations.

The Scripts/examples directory contains a few short demonstration scripts
giving examples of simulations involving new GENESIS features.  The
Scripts/examples/XODUS subdirectory contains examples illustrating new
features of XODUS that have been incorporated into GENESIS 2.0 and later
versions.  There are several new and revised examples for GENESIS 2.2.  The
Scripts/examples/README file gives further details.

The Scripts/newlib directory contains sample C source code, header files,
and Makefiles for compiling new commands and objects into GENESIS.  The
procedure is documented in the GENESIS Reference Manual Section "Customizing
GENESIS", and in the text files in the genesis/Doc directory,
Customizing.txt, NewGenesis.txt, and NewObjects.txt.

The Scripts/param directory contains demonstrations of the parameter search
library developed by Mike Vanier, and first introduced in GENESIS 2.2.  For an
introduction to parameter searching with GENESIS, see the documentation for
ParameterSearch and Paramtable and the README file in Scripts/param.

The Scripts/chemesis directory contains tutorial scripts and example
scripts to teach how to develop models of second messengers and
calcium dynamics.  A step-by-step explanation of the Calx.g scripts is
provided in CalTut.txt and CalTut.html. 'chemesis' is a library of
objects for creating models of biochemical reactions, second
messengers, and calcium dynamics that was developed and contributed to
the GENESIS 2.4 libraries by K. T. (Avrama) Blackwell.
The Scripts/gpython-tools directory contains a collection of
stand-alone Python-based graphical utilities for plotting,
visualizing, and analyzing the output of GENESIS 2 and GENESIS 3 (G-3)
simulations. The scripts are well documented and commented, and may be
modified for use with other data.

The Scripts/stdp_rules directory contains both a text and HTML
tutorial on using the stdp_rules object, along with example scripts.
This new GENESIS 2.4 object has been added for efficiently
implementing spike timing dependent plasticity in large networks of
hsolved multicompartmental cells.

The Scripts/VAnet2 directory contains an improved hsolved
implementation of the dual exponential conductance version of the
Vogels-Abbott network model. It serves as a tutorial on how to achieve
a speed improvement of 10 to 20 times when using hsolve with network
models.

--------------------------------------------------------------------------

To try out the simulations, cd to the desired directory and look at the
README file for information on running the simulation.  It is best to do
this from a terminal window at the bottom of the screen, so that that it will
not be completely covered up by the displays created by the simulations.

--------------------------------------------------------------------------

IMPORTANT NOTE:

Before running these simulations, make sure that you have added the
directory in which GENESIS resides to your search path.  You will also need
to have a copy of .simrc in your home directory.  This file will need to be
edited if GENESIS has not been installed in the place specified in the
file.  In order to access the Neurokit simulation, your .simrc file should
use the SIMPATH environment variable to set a path to the neurokit and
neurokit/prototypes directories.  Normally, the .simrc file will be
properly configured during the installation of GENESIS.  Further details
are given in the src/README file.

To run the simulations, you will need to change into the appropriate
directory and follow the directions given in the README file for that
directory.  In most cases, this will consist of typing "genesis
main-script-name", where "main-script-name" is the main simulation script
that includes other files in the directory.  In a few cases, the simulation
will attempt to write data to files when it runs.  Usually the README file
will warn you of this.  If the Scripts directories are installed in a
system area for which you do not have write permission, you may need to
copy the simulation directory to a directory of your own in which you are
allowed to create files.
