//genesis  -  tutorial3.g - GENESIS Version 2.0
/*======================================================================
  A sample script to create a compartment containing channels taken from
  hhchan.g in the neurokit prototypes library.  SI units are used.
  ======================================================================*/

include hhchan		// functions to create Hodgkin-Huxley channels
/* hhchan.g assigns values to the global variables EREST_ACT, ENA, EK,
   and SOMA_A.  These will be superseded by values defined below.      */

float   PI = 3.14159

// soma parameters - chosen to be the same as in SQUID (but in SI units)
float RM = 0.33333		// specific membrane resistance (ohms m^2)
float CM = 0.01			// specific membrane capacitance (farads/m^2)
float RA = 0.3			// specific axial resistance (ohms m)
float EREST_ACT = -0.07		// resting membrane potential (volts)
float Eleak = EREST_ACT + 0.0106  // membrane leakage potential (volts)
float ENA   = 0.045               // sodium equilibrium potential
float EK    = -0.082              // potassium equilibrium potential

// cell dimensions (meters)
float soma_l = 30e-6            // cylinder equivalent to 30 micron sphere
float soma_d = 30e-6
float SOMA_A = soma_l*PI*soma_d // variable used by hhchan.g for soma area

float tmax = 0.1		// simulation time in sec
float dt = 0.00005		// simulation time step in sec
setclock  0  {dt}		// set the simulation clock

//===============================
//      Function Definitions
//===============================

function makecompartment(path, length, dia, Erest)
    str path
    float length, dia, Erest
    float area = length*PI*dia
    float xarea = PI*dia*dia/4

    create      compartment     {path}
    setfield    {path}              \
		Em      { Erest }   \           // volts
		Rm 	{ RM/area } \		// Ohms
		Cm 	{ CM*area } \		// Farads
		Ra      { RA*length/xarea } 	// Ohms
end

function step_tmax
    step {tmax} -time
end

//===============================
//    Graphics Functions
//===============================

function make_control
    create xform /control [10,50,250,145]
    create xlabel /control/label -hgeom 50 -bg cyan -label "CONTROL PANEL"
    create xbutton /control/RESET -wgeom 33%       -script reset
    create xbutton /control/RUN  -xgeom 0:RESET -ygeom 0:label -wgeom 33% \
         -script step_tmax
    create xbutton /control/QUIT -xgeom 0:RUN -ygeom 0:label -wgeom 34% \
        -script quit
    create xdialog /control/Injection -label "Injection (amperes)" \
                -value 0.3e-9 -script "set_inject <widget>"
    xshow /control
end

function make_Vmgraph
    float vmin = -0.100
    float vmax = 0.05
    create xform /data [265,50,350,350]
    create xlabel /data/label -hgeom 10% -label "Soma with Na and K Channels"
    create xgraph /data/voltage  -hgeom 90%  -title "Membrane Potential"
    setfield ^ XUnits sec YUnits Volts
    setfield ^ xmax {tmax} ymin {vmin} ymax {vmax}
    xshow /data
end

function set_inject(dialog)
    str dialog
    setfield /cell/soma inject {getfield {dialog} value}
end

//===============================
//         Main Script
//===============================

create neutral /cell
// create the soma compartment "/cell/soma"
makecompartment /cell/soma {soma_l} {soma_d} {Eleak}
setfield /cell/soma initVm {EREST_ACT} // initialize Vm to rest potential

// provide current injection to the soma
setfield /cell/soma inject  0.3e-9      // 0.3 nA injection current

// Create two channels, "/cell/soma/Na_squid_hh" and "/cell/soma/K_squid_hh"
pushe /cell/soma
make_Na_squid_hh
make_K_squid_hh
pope

// The soma needs to know the value of the channel conductance
// and equilibrium potential in order to calculate the current
// through the channel.  The channel calculates its conductance
// using the current value of the soma membrane potential.

addmsg /cell/soma/Na_squid_hh /cell/soma CHANNEL Gk Ek
addmsg /cell/soma /cell/soma/Na_squid_hh VOLTAGE Vm
addmsg /cell/soma/K_squid_hh /cell/soma CHANNEL Gk Ek
addmsg /cell/soma /cell/soma/K_squid_hh VOLTAGE Vm

// make the control panel
make_control

// make the graph to display soma Vm and pass messages to the graph
make_Vmgraph
addmsg /cell/soma /data/voltage PLOT Vm *volts *red

check
reset
