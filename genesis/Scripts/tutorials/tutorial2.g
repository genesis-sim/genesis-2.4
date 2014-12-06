//genesis  -  tutorial2.g - GENESIS Version 2.0
/*======================================================================
  A sample script to create a soma-like compartment. SI units are used.
  ======================================================================*/

float   PI              =       3.14159

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
    setfield   {path}              \
                Em      { Erest }   \           // volts
                Rm      { RM/area } \           // Ohms
                Cm      { CM*area } \           // Farads
                Ra      { RA*length/xarea }     // Ohms
end

function make_Vmgraph
    float vmin = -0.100
    float vmax = 0.05
    float tmax = 0.100	// default simulation time = 100 msec
    create xform /data
    create xgraph /data/voltage
    setfield ^ xmax {tmax} ymin {vmin} ymax {vmax}
    create xbutton /data/RESET  -script reset
    create xbutton /data/RUN  -script "step "{tmax}" -time"
    create xbutton /data/QUIT -script quit
    xshow /data
end

//===============================
//         Main Script
//===============================

create neutral /cell
// create the soma compartment "/cell/soma"
makecompartment /cell/soma {soma_l} {soma_d} {Eleak}

// provide current injection to the soma
setfield /cell/soma inject  0.3e-9      // 0.3 nA injection current

// make the graph to display soma Vm and pass messages to the graph
make_Vmgraph
addmsg /cell/soma /data/voltage PLOT Vm *volts *red

check
reset
