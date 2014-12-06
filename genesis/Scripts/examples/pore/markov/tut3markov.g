//genesis  -  tutorial3.g - modified to use readcell with markovchan.

/*======================================================================
  A sample script to create a compartment containing channels taken from
  markovchan.g in the neurokit prototypes library.  SI units are used.
  ======================================================================*/

float   PI = 3.14159

// soma parameters - chosen to be the same as in SQUID (but in SI units)

include markovchan		// functions to create stochastic channels
/* markovchan.g assigns values to the global variables EREST_ACT, ENA, EK,
   and SOMA_A.  These will be superseded by values defined below.
*/

float EREST_ACT = -0.07		// resting membrane potential (volts)
float Eleak = EREST_ACT + 0.0106  // membrane leakage potential (volts)
float ENA   = 0.045               // sodium equilibrium potential
float EK    = -0.082              // potassium equilibrium potential

// soma dimensions (meters) - will get initial values from cell.p
float soma_l
float soma_d
float SOMA_A

float tmax = 0.1		// simulation time in sec
float dt = 0.00005		// simulation time step in sec
setclock  0  {dt}		// set the simulation clock
setclock  1  {10*dt}		// graphics clock

float Idens = 0.03 // default injection current density in A/m^2
// use 0.1061 to make it equivalent to the 0.3 nA used in tutorial3.g

//===============================
//      Function Definitions
//===============================

function step_tmax
    step {tmax} -time
end

function overlaytoggle(widget)
    str widget
    setfield /##[TYPE=xgraph] overlay {getfield {widget} state}
end

/* For the initial cell parameters, readcell takes care of all of this.
   However if we change the soma dimensions, we need to make these changes
   to the soma fields.
*/

function resize_soma
    soma_l = {getfield /control/len value}
    soma_d = {getfield /control/dia value}
    SOMA_A = soma_l*PI*soma_d
    echo "Setting soma len to " {soma_l} "dia to " {soma_d}
    echo "Soma area is " {SOMA_A}
    // Rm and Cm need to be recomputed if we change the area
    setfield /cell/soma Rm {RM/SOMA_A}
    setfield /cell/soma Cm {CM*SOMA_A}
    // channels need soma len and dia to recalculate Nt
    setfield /cell/soma len {soma_l}
    setfield /cell/soma dia {soma_d}
    setfield /cell/soma inject {{getfield  /control/Injection value}*SOMA_A}
// This causes the channels to recompute the number of pores Nt
   call /cell/soma/Na_squid_markov RESET
   call /cell/soma/K_squid_markov RESET
   reset  // then reset everything
end

function set_inject(dialog)
    str dialog
    Idens = {getfield {dialog} value}
    setfield /cell/soma inject {Idens*SOMA_A}
    echo "Setting injection to " {Idens} "A/m^2 = " {Idens*SOMA_A} "A."
end

//===============================
//    Graphics Functions
//===============================

function make_control
    create xform /control [10,50,250,145]
    create xlabel /control/label -hgeom 20 -bg cyan -label "CONTROL PANEL"
    create xbutton /control/RESET -wgeom 25%       -script reset
    create xbutton /control/RUN  -xgeom 0:RESET -ygeom 0:label -wgeom 25% \
         -script step_tmax
    create xbutton /control/STOP  -xgeom 0:RUN -ygeom 0:label -wgeom 25% \
         -script stop
    create xbutton /control/QUIT -xgeom 0:STOP -ygeom 0:label -wgeom 25% \
        -script quit
    create xdialog /control/Injection -label "Injection (Amp/m^2)" \
                -value {Idens} -script "set_inject <widget>"
    create xdialog /control/len -wgeom 50%  -script resize_soma
    create xdialog /control/dia -xgeom 0:len -wgeom 50% -ygeom 0:Injection \
	         -script resize_soma
    // A trick to limit the number of sig figs shown in the dialogs
    floatformat %0.5g
    setfield /control/len value {soma_l}
    setfield /control/dia value {soma_d}
    floatformat %0.10g
    create xtoggle /control/overlay   -script "overlaytoggle <widget>"
    setfield /control/overlay offlabel "Overlay OFF" onlabel "Overlay ON" state 0

    xshow /control
end

function make_Vmgraph
    float vmin = -0.100
    float vmax = 0.05
    create xform /data [265,50,350,350]
    create xlabel /data/label -hgeom 10% \
	-label "Soma with stochastic Na and K Channels"
    create xgraph /data/voltage  -hgeom 90%  -title "Membrane Potential"
    setfield ^ XUnits sec YUnits Volts
    setfield ^ xmax {tmax} ymin {vmin} ymax {vmax}
    useclock /data/voltage 1
    xshow /data
end

//===============================
//         Main Script
//===============================

// Build the cell from a parameter file using the cell reader

// create prototypes

create neutral /library

// We don't want the library to try to calculate anything, so we disable it
disable /library
pushe /library
make_Na_squid_markov
make_K_squid_markov
make_markov_compt
pope

readcell cell.p /cell   // make the cell
// get the initial values of soma_l and soma_d from the cell
soma_l = {getfield /cell/soma len}
soma_d = {getfield /cell/soma dia}
SOMA_A = soma_l*PI*soma_d

// make the control panel
make_control

// make the graph to display soma Vm and pass messages to the graph
make_Vmgraph
addmsg /cell/soma /data/voltage PLOT Vm *volts *red

// provide current injection to the soma
setfield /cell/soma inject {Idens*SOMA_A}

// use the better SPRNG random number generator instead of the default
setrand -sprng

check
reset
// give an extra reset to be sure that everything was reset in the right order
reset
