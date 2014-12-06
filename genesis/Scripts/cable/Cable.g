//genesis
// Cable.g - main script

//==================================================
// global variables used in the simulation
//==================================================

// start with no passive cable compartments
int NCableSects = 0
// use the Crank-Nicholson numerical integration method 
int IntegMethod = 11
// start with injection to soma (comp 0)
int InjComp = 0
// start with synaptic input to soma (comp 0)
int SynComp = 0
// default time to run simulation (msecs)
float tmax = 50
// default integration time step  (msecs)
float dt = 0.01
float PI = 3.14159
// resting membrane potential
float EREST = 0

// Specific resistances and capacitance
// Kohm-cm^2
float RM = 10.0
// uF/cm^2
float CM = 1.0
// Kohm-cm
float RA = 0.1

// compartment dimensions (cm.)
float soma_l = 50e-4
float soma_d = 50e-4
float dend_l = 50e-4
float dend_d = 1e-4

// default (initial) parameters for current injection
// msec
float injwidth = 10
float injdelay = 0
// uA (200 nA)
float injcurrent = 2e-4

// default (initial) parameters synaptic input
// msec
float syndelay = 0
// choose width and interval to give a single spike
float synwidth = 1000
float syninterval = 100000

// milliSiemen
float gsyn = 1e-5
// milliseconds
float tsyn = 1
// mV above resting potential (defined as zero)
float Vsyn = 50
// for comparison
// Vsyn = 125 ==> Na channel (+55 mV with Erest = -70)
// Vsyn = -12 ==>  K channel (-82 mV with Erest = -70)

// While simulation is  being loaded, put up some credits
     echo " "
     echo "*************************************************"
     echo "*                                               *"
     echo "*                    Cable                      *"
     echo "*                                               *"
     echo "*    A simulation of a simple dendritic cable   *"
     echo "*                                               *"
     echo "*                (version 2.0)                  *"
     echo "*                                               *"
     echo "*                  D. Beeman                    *"
     echo "*                                               *"
     echo "*            UNIVERSITY OF COLORADO             *"
     echo "*                                               *"
     echo "*************************************************"
     echo " "
     echo "Loading simulation ....."

     //==================================================
     //                  STARTUP SCRIPTS
     //==================================================

include compartment.g 
include methods.g // sets integration method and creates hsolver
include inputs.g // functions for injection and synaptic input
include logifier.g // constructs lookup tables for log plots
// include view.g // graphics for propagation along the cable
include addcable.g // functions to add and delete cable sections
include forms.g // Control Panel and Graphs
include setcolors.g // function colorize sets widget colors

//=========================================================================
//  Some functions used in the main script

//  SendPlotMsgs  - sends messages to the graphics forms

//  ResetGlobals  - resets the values of the global variables to defaults

//  ResetDefaults - similar to the main script, but graph forms pre-exist
//=========================================================================

function SendPlotMsgs
  pushe /output/somagraphs
  addmsg /cell/soma somaVm_grf PLOT Vm *soma *black
  addmsg /cell/soma /logtable INPUT Vm
  addmsg /logtable loggraph PLOT output *soma *black
  addmsg /cell/syn_channel /output/condgraphs/channel_Gk PLOT Gk *Gk  \
      *blue
  pope
end// SendPlotMsgs


function ResetGlobals
    // start with no passive cable compartments
    NCableSects = 0
    IntegMethod = 11
    // start with injection to soma (comp 0)
    InjComp = 0
    // start with synaptic input to soma (comp 0)
    SynComp = 0
    // default time to run simulation (msecs)
    tmax = 50
    // default integration time step  (msecs)
    dt = 0.01
    // Kohm-cm^2
    RM = 10.0
    // uF/cm^2
    CM = 1.0
    // Kohm-cm
    RA = 0.1
    soma_l = 50e-4
    soma_d = 50e-4
    dend_l = 50e-4
    dend_d = 1e-4
end

function ResetDefaults
    stop
    // in case we are plotting cable sections
    remove_cable_plots
    delete /cell
    delete /input
    delete /output/control
    delete /output/param_menu
    delete /output/inject_menu
    delete /output/synapse_menu
    delete /output/method_menu
    delete /output/plot_menu
    deletemsg /logtable 0 -outgoing
    // re-initialize global constants
    ResetGlobals
    // simulation time step in msec
    setclock 0 {dt}
    create neutral /cell
    makecompartment /cell/soma {soma_l} {soma_d} {EREST}
    // inputs.g
    makeinputs
    load_menus
    SendPlotMsgs
    //    check
    colorize
    reset
end

//=========================================================================

//                         MAIN SCRIPT - Cable

//=========================================================================

//==================================================
// check and initialize the simulation
//==================================================

// set the prompt
setprompt "Cable !"
// simulation time step in msec
setclock 0 {dt}
					// max size for reasonable accuracy
// output interval
setclock 1 0.05

create neutral /cell
makecompartment /cell/soma {soma_l} {soma_d} {EREST} 
makeinputs  // inputs.g
loadgraphs  // in forms.g (This is done after the elements are created)
load_menus
loadhelp
SendPlotMsgs
// check
colorize
reset

echo ==================
echo Simulation loaded.
echo ==================
