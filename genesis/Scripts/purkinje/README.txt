$ProjectVersion: Release2-2.17 $

README file for the Tutorial simulation of a cerebellar Purkinje cell


INTRODUCTION
------------

This is the second major version of the Purkinje cell tutorial.  This
tutorial is based upon a GENESIS simulation of a cerebellar Purkinje
cell, modeled and fine-tuned by Erik de Schutter. The tutorial assumes
that you have a basic knowledge of the Purkinje cell and its synaptic
inputs. It gives visual insights in how different properties as
concentrations and channel conductances vary and interact in a real
Purkinje cell.


RUNNING
-------

In order to run the simulation, you need the following : 

1) A fast computer.

2) Genesis 2.2 or above installed and correctly configured.

3) A graphical display with at least a resolution of 1024*768

4) the UNIX shell commands : sort, touch


To explore the tutorial, do the following :

1) Make room for the graphical output : move your shell window to the lower
   right hand corner of your display.

2) Change to the directory where the TUTORIAL.g script resides. If you are
   reading this file, you should be in the correct directory.  The tutorial
   will create some files in this directory, so you will need to have write
   privileges.  If the tutorial is installed in a system directory for which
   you do not have write permission, copy the tutorial files to a directory
   in which you are allowed to create files.

3) Type at the shell prompt : "genesis TUTORIAL.g"
   or "genesis TUTORIAL_excitation_demo.g"


LIBRARY
-------

The TUTORIAL can work with different cells.  The file 'cell.g'
contains pointers to the cell configuration and the cell morphology
file.  In the official purkinje cell tutorial distribution, this file
is a copy of the file
'library/cells/purkinje_eds1994/purkinje_eds1994.g'.  This configures
the tutorial to use Erik De Schutter's 1994 Purkinje cell model.  To
switch cells, modify 'cell.g' such that it contains pointers to other
configuration data.

The cell configuration file has to set the global variable 'cellfile'
to point to the morphology file that you want to load.  Further the
cell configuration file is responsible to create any compartmental
prototypes needed for the cell to be loaded.  Note that by selecting
different compartmental prototypes, you can easily switch from an
active cell to a passive cell and do other interesting stuff.  For an
example of a cell configuration file, check out the configuration file
for Erik's Purkinje cell
('library/cells/purkinje_eds1994/purkinje_eds1994.g') and the bare
bones example configuration file ('library/cells/example/passive.g').

Note that the mechanism for selecting the cell and the compartmental
prototypes is subject to change.


For feedback write: Hugo Cornelis - hugo.cornelis@gmail.com

Copyright for the first  version of these scripts: Born-Bunge Foundation - UA 1998-2002.
Copyright for the second version of these scripts: UTHSCSA - University of Texas system 2005-2006

This code is GPL.  You are free to modify this code as you like, and
distribute the modified code.  See license.txt for more information.


