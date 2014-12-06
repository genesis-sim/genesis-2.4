//genesis  -  tutorial4.g - GENESIS Version 2.0

/*======================================================================
  A sample script to create a multicompartmental neuron with synaptic
  input.  SI units are used.
  ======================================================================*/

include hhchan		// functions to create Hodgkin-Huxley channels
/* hhchan.g assigns values to the global variables EREST_ACT, ENA, EK,
   and SOMA_A.  These will be superseded by values defined below.      */

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
float dend_l =100e-6		// we will add a 100 micron long dendrite
float dend_d =  2e-6		// give it a 2 micron diameter
float SOMA_A = soma_l*PI*soma_d // variable used by hhchan.g for soma area

float gmax = 5e-10		// maximum synaptic conductance (Siemen)

float tmax = 0.1                // simulation time in sec
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
		Ra      { RA*length/xarea }	// Ohms
end

function makechannel(compartment,channel,Ek,tau1,tau2,gmax)
    str compartment
    str channel
    float Ek                            // Volts
    float tau1,tau2                     // sec
    float gmax                          // Siemens (1/ohms)

    create      synchan               {compartment}/{channel}
    setfield    ^ \
                Ek                      {Ek} \
                tau1                    {tau1} \
                tau2                    {tau2} \
                gmax                    {gmax}
    addmsg   {compartment}/{channel}   {compartment} CHANNEL Gk Ek
    addmsg   {compartment}   {compartment}/{channel} VOLTAGE Vm
end

function makeneuron(path, soma_l, soma_d, dend_l, dend_d)
    str path
    float soma_l, soma_d, dend_l, dend_d

    create neutral {path}
    makecompartment {path}/soma {soma_l} {soma_d} {Eleak}
    setfield /cell/soma initVm {EREST_ACT}

// Create two channels, "{path}/soma/Na_squid_hh" and "{path}/soma/K_squid_hh"
    pushe {path}/soma
    make_Na_squid_hh
    make_K_squid_hh
    pope

    // The soma needs to know the value of the channel conductance
    // and equilibrium potential in order to calculate the current
    // through the channel.  The channel calculates its conductance
    // using the current value of the soma membrane potential.
    addmsg {path}/soma/Na_squid_hh {path}/soma CHANNEL Gk Ek
    addmsg {path}/soma {path}/soma/Na_squid_hh VOLTAGE Vm
    addmsg {path}/soma/K_squid_hh {path}/soma CHANNEL Gk Ek
    addmsg {path}/soma {path}/soma/K_squid_hh VOLTAGE Vm

    // make the dendrite compartment and link it to the soma
    makecompartment {path}/dend {dend_l} {dend_d} {EREST_ACT}
    makechannel {path}/dend Ex_channel {ENA} 0.003 0.003 {gmax}
    addmsg {path}/dend {path}/soma RAXIAL Ra previous_state
    addmsg {path}/soma {path}/dend AXIAL  previous_state

// add a spike generator to the soma
    create spikegen {path}/soma/spike
    setfield {path}/soma/spike  thresh 0  abs_refract 0.010  output_amp 1
    /* use the soma membrane potential to drive the spike generator */
    addmsg  {path}/soma  {path}/soma/spike  INPUT Vm
end  // makeneuron

function step_tmax
    step {tmax} -time
end

function makeinput(path)
    str path
    int msgnum
    create randomspike /randomspike
    setfield ^ min_amp 1.0 max_amp 1.0 rate 200 reset 1 reset_value 0
    addmsg /randomspike {path} SPIKE
    msgnum = {getfield {path} nsynapses} - 1
    setfield {path} \
        synapse[{msgnum}].weight 1 synapse[{msgnum}].delay 0
    addmsg /randomspike /data/voltage \
	PLOTSCALE state  *input *blue  0.01     0
// 			  title  color  scale  offset
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
		-value 0.0 -script "set_inject <widget>"
    create xtoggle /control/feedback  -script toggle_feedback
    setfield /control/feedback offlabel "Feedback OFF" \
	onlabel "Feedback ON" state 1
    xshow /control
end

function make_Vmgraph
    float vmin = -0.100
    float vmax = 0.05
    create xform /data [265,50,350,350]
    create xlabel /data/label -hgeom 10% -label "Simple Neuron Model"
    create xgraph /data/voltage  -hgeom 90%  -title "Membrane Potential"
    setfield ^ XUnits sec YUnits Volts
    setfield ^ xmax {tmax} ymin {vmin} ymax {vmax}
    xshow /data
end

function set_inject(dialog)
    str dialog
    setfield /cell/soma inject {getfield {dialog} value}
end

function make_condgraph
    create xform /condgraphs [620,50,475,350]
    pushe /condgraphs
    create xgraph channel_Gk -hgeom 100% -title "Channel Conductance"
    setfield channel_Gk xmin 0 xmax {tmax} ymin 0 ymax {gmax*10}
    setfield channel_Gk XUnits "sec" YUnits "Gk (Siemen)"
    pope
    xshow /condgraphs
end

function toggle_feedback
    int msgnum
    if ({getfield /control/feedback state} == 0)
	deletemsg  /cell/soma/spike 0 -out
	echo "Feedback connection deleted"
    else
        addmsg /cell/soma/spike /cell/dend/Ex_channel SPIKE
	msgnum = {getfield /cell/dend/Ex_channel nsynapses} - 1
        setfield /cell/dend/Ex_channel \
	    synapse[{msgnum}].weight 10 synapse[{msgnum}].delay 0.005
	echo "Feedback connection added"
    end
end

//===============================
//         Main Script
//===============================

// create the neuron "/cell"
makeneuron /cell {soma_l} {soma_d} {dend_l} {dend_d}
setfield /cell/soma inject 0.0

// make the control panel
make_control

// make the graph to display soma Vm and pass messages to the graph
make_Vmgraph
addmsg /cell/soma /data/voltage PLOT Vm *volts *red

makeinput /cell/dend/Ex_channel

make_condgraph
addmsg /cell/dend/Ex_channel /condgraphs/channel_Gk PLOT Gk *Gk *black

// finally, we add some feedback from the axon to the dendrite
addmsg /cell/soma/spike /cell/dend/Ex_channel SPIKE
setfield /cell/dend/Ex_channel \
    synapse[1].weight 10 synapse[1].delay 0.005

check
reset
