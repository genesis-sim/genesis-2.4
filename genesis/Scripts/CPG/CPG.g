//genesis CPG.g

echo " "
echo "          *************************************************"
echo "          *                      CPG                      *"
echo "          *     A simple neuronal network simulation.     *"
echo "          *                                               *"
echo "          *            Hacked by Sharon Crook             *"
echo "          *                     8/92                      *"
echo "          *************************************************"
echo " "
echo " "
echo "                   Special Thanks to Matt Wilson            "
echo "                          for the Use of                    "
echo "                    Documentation and Files from            "
echo "                             MultiCell                      "
echo " "
echo " "
// set the prompt
setprompt "CPG >"
// simulation time step in msec
setclock 0 0.1
// output interval
setclock 1 1.0
floatformat %.3g // do reasonable rounding for display

//========================================================================
//                            startup scripts
//========================================================================

include setcolors.g
include tools.g 
include xtools.g 
include toggles.g 
include cell.g 
include help.g 
include graphs.g 
include settings.g 
include inputs.g 
include viewform.g 
include connections.g 
include defaults.g 
include initialize.g 

//=======================================================================
//                      start graphics
//=======================================================================
makesystem
setdefaults
loadgraphics
loadhelp
makegraph /L1
makegraph /L2
makegraph /R1
makegraph /R2
makesettings /L1
makesettings /L2
makesettings /R1
makesettings /R2
makeconnections /L1
makeconnections /L2
makeconnections /R1
makeconnections /R2
makeinputs /L1 0
makeinputs /L2 5
makeinputs /R1 10
makeinputs /R2 15
makeallgraph
makeviewform

//======================================================================
//                   check and initialize the simulation
//======================================================================
colorize  // change default colors of all widgets.
// check
reset
echo Simulation loaded.
