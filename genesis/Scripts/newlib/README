This directory contains sample source code and Makefiles for compiling new
commands and objects into GENESIS.  The procedure is documented in the
GENESIS Reference Manual Section "Customizing GENESIS", and in the text files
in the genesis/Doc directory, Customizing.txt, NewGenesis.txt, and
NewObjects.txt.

The files in this directory are:

Makefile - the edited library Makefile, derived from the template file
           "Libmake".  This should not need any additional edits to create
           the example command and object given here.

command.c - source code for the function "do_example"
example.c - source code for the object "ExampleElement"
example_ext.h, example_struct.h - header files included by example.c
examplelib.g - library initialization script which makes new commands and
             objects known to GENESIS (used only during compilation)
testexample.g - a GENESIS script to test the new object created here

The following file should go into parent directory of the one containing
the library files listed above:

Makefile.usermake - Upper level Makefile, derived from the template file
        "Usermake" (should be renamed "Makefile" and edited as described
        in the comments and in genesis/Doc/NewGenesis.txt.)

If the edits to the Makefile generated from Makefile.usermake have been
properly carried out, you should be able to simply type "make" in the
directory in which it resides.  This should produce the new GENESIS
executable "newgenesis".  If not, please re-read NewGenesis.txt.
