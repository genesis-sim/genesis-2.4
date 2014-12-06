//genesis
echo " "
echo "*********************************************************"
echo "*                                                       *"
echo "*                    PIRIFORM CORTEX                    *"
echo "*                 TUTORIAL VERSION 2.0                  *"
echo "*                                                       *"
echo "*  Original Research Simulation Written by Matt Wilson  *"
echo "*    Converted into Tutorial form by Alex Protopapas    *"
echo "*          California Institute of Technology           *"
echo "*                                                       *"
echo "*              GENESIS 2.0 implementation               *"
echo "*                       8/5/95                          *"
echo "*                                                       *"
echo "*      Report bugs to: alexp@smaug.bbb.caltech.edu      *"
echo "*                                                       *"
echo "*********************************************************"
echo " "

deleteall -force

include X1compat.g
include startup.g // set up some basic things
include constants.g // set local variables 
include protodefs.g // create some prototype objects
include functions.g // set up some functions
include cells.g // set up the cells
include pyramidal.g // create the cell populations
include fb.g 
include ff.g 
include bulb.g 
include connections.g // set up the connections
include field.g // place field potential electrodes
include output.g // set up the outputs
include savefuncs.g 
include adjustweights.g // adjust strength of connections
include el_active.g // enable and disable elements
include xfuncs.g 
include x_interface.g 
include menu.g 


echo checking and initializing simulation
// check
echo " "
echo Piriform cortex simulation loaded.
