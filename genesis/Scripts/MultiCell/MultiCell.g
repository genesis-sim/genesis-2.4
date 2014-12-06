//genesis
echo " "
echo "*************************************************"
echo "*                  MultiCell                    *"
echo "*      A simple neuronal circuit simulation.    *"
echo "*                                               *"
echo "*            Written by Matt Wilson             *"
echo "*    California Institute of Technology         *"
echo "*                   4/89                        *"
echo "*************************************************"
echo " "

// set the prompt
setprompt "MultiCell !"
// simulation time step in msec
setclock 0 0.1
// output interval
setclock 1 1.0
int GRAPHICS = 1

echo SIMULATION CONFIGURATION
echo =========================
echo Hodgkin-Huxley
echo multi-compartmental neurons (soma + dendrite)
echo with synaptic connections

//==================================================
//                  STARTUP SCRIPTS
//==================================================
include X1compat.g
include constants.g 
include tools.g 
include channel.g 
include compartment.g 
include cell.g 

if (GRAPHICS)
    include graphics.g 
end

//==================================================
//                      CELL1
//==================================================
makeneuron /cell1 30e-4 30e-4 100e-4 2e-4
position /cell1 100e-4 100e-4 500e-4

//==================================================
//                      CELL2
//==================================================
makeneuron /cell2 10e-4 10e-4 100e-4 1e-4
position /cell2 200e-4 200e-4 500e-4

//==================================================
//                  CONNECTIONS
//==================================================
connect /cell1/axon /cell2/soma/dend/Na_channel with synapse

/* set delay and weight parameters for connections from cell1 */
// msec 
// synapses
setconn /cell1/axon:0 delay 5 weight 30

/* set up connections for cell2 */
connect /cell2/axon /cell1/soma/dend/K_channel with synapse
// msec 
// synapses
setconn /cell2/axon:0 delay 5 weight 300

//==================================================
// provide current injection to cell 1 
//==================================================
// uA
setfield /cell1/soma inject 3e-4

//==================================================
//                    OUTPUT
//==================================================
// write out the intracellular soma potentials to disk 
//==================================================

// These statements are provided to illustrate the disk_output capability,
// but are not needed for the simulation.  They have been commented out in
// case the simulation is run from a directory in which the user has no
// write priveleges.  In order to try them out, remove the "//" and cd to
// your own directory after loading the simulation.

// disk_output	/cell1/Vm1 	ascii
// addmsg	/cell1/soma 	/cell1/Vm1  	SAVE Vm

// disk_output	/cell2/Vm2 	ascii
// addmsg	/cell2/soma 	/cell2/Vm2  	SAVE Vm

//==================================================
//                    GRAPHICS
//==================================================
if (GRAPHICS)
    loadgraphics
    makegraphics /cell1
    makegraphics /cell2 580 120
end

//==================================================
// check and initialize the simulation
//==================================================
//check
reset
adjustspike

echo ==================
echo Simulation loaded.
echo ==================
