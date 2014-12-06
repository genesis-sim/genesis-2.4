// genesis  -  hebb.g: demo of hebbian synapses
//
// Mike Vanier, 9/95
// 
// This demo is based on Scripts/tutorials/tutorial4.g
//

include hhchan		// functions to create Hodgkin-Huxley channels

// hhchan.g assigns values to the global variables EREST_ACT, ENA, EK,
// and SOMA_A.  These will be superseded by values defined below.      

// Soma parameters - chosen to be the same as in SQUID (but in SI units).

float PI        =  3.14159
float RM        =  0.33333               // specific membrane resistance (ohms m^2)
float CM        =  0.01                  // specific membrane capacitance (farads/m^2)
float RA        =  0.3                   // specific axial resistance (ohms m)
float EREST_ACT = -0.07		         // resting membrane potential (volts)
float Eleak     =  EREST_ACT + 0.0106    // membrane leakage potential (volts)
float ENA       =  0.045                 // sodium equilibrium potential
float EK        = -0.082                 // potassium equilibrium potential

// cell dimensions (meters)

float soma_l     =  30.0e-6              // cylinder equivalent to 30 micron sphere
float soma_d     =  30.0e-6
float dend_l     = 100.0e-6	         // we will add a 100 micron long dendrite
float dend_d     =   2.0e-6	         // give it a 2 micron diameter
float SOMA_A     = soma_l * PI * soma_d  // variable used by hhchan.g for soma area

float gmax       = 5.0e-10	         // maximum synaptic conductance (Siemens)

float tmax       = 0.5                   // simulation time in sec
float dt         = 0.00005	         // simulation time step in sec
float input_rate = 50.0                  // rate of input spike generator in Hz
float weight     = 1.5                   // starting weight of synapse

setclock  0  { dt }		         // set the simulation clock
setclock  1  { dt * 10 }	         // for graphics
setclock  2  0.001                       // for presynaptic activity updating
setclock  3  0.010                       // for weight updating      


//===============================
//      Function Definitions
//===============================


function makecompartment( path, length, dia, Erest )
    str   path
    float length, dia, Erest
    float area  = length * PI * dia
    float xarea = PI * dia * dia / 4

    create      compartment     {path}
    setfield    {path}                \
		Em      { Erest }     \           // volts
		Rm 	{ RM / area } \		  // Ohms
		Cm 	{ CM * area } \		  // Farads
		Ra      { RA * length / xarea }   // Ohms
end



function makechannel( compartment, channel, Ek, tau1, tau2, gmax )
    str   compartment
    str   channel
    float Ek                            // Volts
    float tau1, tau2                    // sec
    float gmax                          // Siemens (1/ohms)

    create      hebbsynchan              {compartment}/{channel}
    setfield    ^ \
                Ek                         { Ek }           \
                tau1                       { tau1 }         \
                tau2                       { tau2 }         \
                gmax                       { gmax }         \
		pre_tau1		   0.010            \ 
		pre_tau2		   0.100            \
		pre_thresh_lo		   3.0              \
		pre_thresh_hi		   3.0	            \
		post_tau		   0.020            \
		post_thresh_lo		  -0.07             \
		post_thresh_hi		  -0.07             \
		post_scale                 0.002            \  // mV
		weight_change_rate         2.0              \
		min_weight                 { weight / 2.0 } \
		max_weight		   { weight * 2.0 } \
		change_weights		   1                   //  allow weight changes

    addmsg      {compartment}/{channel}  {compartment}            CHANNEL Gk Ek
    addmsg      {compartment}            {compartment}/{channel}  VOLTAGE Vm
end



function makeneuron( path, soma_l, soma_d, dend_l, dend_d )
    str   path
    float soma_l, soma_d, dend_l, dend_d

    create neutral {path}
    makecompartment {path}/soma {soma_l} {soma_d} {Eleak}
    setfield {path}/soma initVm {EREST_ACT}

    // Create two channels, "{path}/soma/Na_squid_hh" and "{path}/soma/K_squid_hh".

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

    // Make the dendrite compartment and link it to the soma.

    makecompartment {path}/dend  { dend_l }  { dend_d }  { EREST_ACT }
    makechannel     {path}/dend Ex_channel { ENA }  0.003  0.003  { gmax }
    addmsg {path}/dend {path}/soma RAXIAL Ra previous_state
    addmsg {path}/soma {path}/dend AXIAL  previous_state

    // We don't bother adding a spike generator to the soma since it
    // isn't going to be connected to anything.
end 



function makeinput( path )
    str path
    int msgnum
 
    create neutral /input
    create pulsegen /input/pulse
    setfield ^ level1 1.0 width1 0.001 delay1 { 1.0 / { input_rate } }

    create spikegen /input/spike
    setfield ^ thresh 0.5 abs_refract 0.005 output_amp 1.0
 
    addmsg /input/pulse /input/spike INPUT output 
    addmsg /input/spike {path} SPIKE 
 
    msgnum = { getfield {path} nsynapses } - 1
    setfield {path} synapse[{msgnum}].weight {weight} synapse[{msgnum}].delay 0
end



function set_input_rate( widget )
    str widget

    setfield /input/pulse delay1  { 1.0 / {getfield {widget} value} }
end 



function set_pre_tau1( widget )
    str widget
    setfield /cell/dend/Ex_channel pre_tau1 {getfield {widget} value}
end 



function set_pre_tau2( widget )
    str widget
    setfield /cell/dend/Ex_channel pre_tau2 {getfield {widget} value}
end 



function set_pre_thresh_lo( widget )
    str widget
    setfield /cell/dend/Ex_channel pre_thresh_lo {getfield {widget} value}
end 



function set_pre_thresh_hi( widget )
    str widget
    setfield /cell/dend/Ex_channel pre_thresh_hi {getfield {widget} value}
end 



function set_post_tau( widget )
    str widget
    setfield /cell/dend/Ex_channel post_tau {getfield {widget} value}
end 



function set_post_thresh_lo( widget )
    str widget
    setfield /cell/dend/Ex_channel post_thresh_lo {getfield {widget} value}
end 



function set_post_thresh_hi( widget )
    str widget
    setfield /cell/dend/Ex_channel post_thresh_hi {getfield {widget} value}
end 



function set_weight_change_rate( widget )
    str widget
    setfield /cell/dend/Ex_channel weight_change_rate {getfield {widget} value}
end 



function set_min_weight( widget )
    str widget
    setfield /cell/dend/Ex_channel min_weight {getfield {widget} value}
end 



function set_max_weight( widget )
    str widget
    setfield /cell/dend/Ex_channel max_weight {getfield {widget} value}
end 



function set_change_weights( value )
    str value
    int change = 1 - { value }
    setfield /cell/dend/Ex_channel change_weights {change} 
end 



//===============================
//    Graphics Functions
//===============================

function make_control
    create xform  /graphics/control [0,50,300,460]
    create xlabel /graphics/control/label -hgeom 30 -label "CONTROL PANEL"

    create xbutton /graphics/control/RESET -wgeom 25% -script "check ; reset"
    create xbutton /graphics/control/RUN   -xgeom 0:RESET -ygeom 0:label -wgeom 25% \
        -script "step "{tmax}" -time"
    create xbutton /graphics/control/STOP  -xgeom 0:RUN   -ygeom 0:label -wgeom 25% \
        -script stop
    create xbutton /graphics/control/QUIT  -xgeom 0:STOP  -ygeom 0:label -wgeom 25% \
        -script quit

    create xdialog /graphics/control/input_rate -wgeom 100% \
        -value {input_rate} \
	-script "set_input_rate <w>"

    create xdialog /graphics/control/pre_tau1 -wgeom 100% \
        -value 0.010 \
	-script "set_pre_tau1 <w>"

    create xdialog /graphics/control/pre_tau2 -wgeom 100% \
        -value 0.100 \
	-script "set_pre_tau2 <w>"

    create xdialog /graphics/control/pre_thresh_lo -wgeom 100% \
        -value 3.0 \
	-script "set_pre_thresh_lo <w>"

    create xdialog /graphics/control/pre_thresh_hi -wgeom 100% \
        -value 3.0 \
	-script "set_pre_thresh_hi <w>"

    create xdialog /graphics/control/post_tau -wgeom 100% \
        -value 0.020 \
	-script "set_post_tau <w>"

    create xdialog /graphics/control/post_thresh_lo -wgeom 100% \
        -value -0.070 \
	-script "set_post_thresh_lo <w>"

    create xdialog /graphics/control/post_thresh_hi -wgeom 100% \
        -value -0.070 \
	-script "set_post_thresh_hi <w>"

    create xdialog /graphics/control/weight_change_rate -wgeom 100% \
        -value  2.0   \
	-script "set_weight_change_rate <w>"

    create xdialog /graphics/control/min_weight -wgeom 100% \
        -value  { weight / 2.0 } \
	-script "set_min_weight <w>"

    create xdialog /graphics/control/max_weight -wgeom 100% \
        -value  { weight * 2.0 } \
	-script "set_max_weight <w>"

    create xtoggle /graphics/control/change_weights -wgeom 100% 
    setfield ^ script "set_change_weights <v>" \
	state 0 \
	offlabel "synaptic weight can be changed" onlabel "synaptic weight is fixed" 

    xshow /graphics/control
end



function make_Vmgraph
    float vmin = -0.100
    float vmax = 0.05
    create xform  /graphics/data [305,50,350,350]
    create xlabel /graphics/data/label -hgeom 10% -label "Simple Neuron Model"
    create xgraph /graphics/data/voltage  -hgeom 90%  -title "Membrane Potential"
    setfield ^ XUnits sec YUnits Volts
    setfield ^ xmax {tmax} ymin {vmin} ymax {vmax}
    useclock /graphics/data/voltage 0

    xshow /graphics/data
end



function make_condgraph
    create xform /graphics/condgraph [660,50,475,350]
    pushe /graphics/condgraph
    create xgraph channel_Gk -hgeom 100% -title "Channel Conductance"
    setfield ^ xmin 0 xmax {tmax}  ymin { -gmax * 1.0 }  ymax { gmax * 5.0 } \
	       XUnits "sec" YUnits "Gk (Siemen)"
    useclock /graphics/condgraph 1
    pope

    xshow /graphics/condgraph
end



function make_hebb_pregraph
    create xform /graphics/hebb_pregraph [305,430,475,350]
    pushe /graphics/hebb_pregraph
    create xgraph channel_hebb_pre -hgeom 100% -title "hebbian presynaptic activity"
    setfield ^ xmin 0 xmax {tmax} ymin -0.5 ymax 10 \
	       XUnits "sec" YUnits "activity"
    useclock /graphics/hebb_pregraph 1
    pope

    xshow /graphics/hebb_pregraph
end



function make_hebb_postgraph
    create xform /graphics/hebb_postgraph [660,430,475,350]
    pushe /graphics/hebb_postgraph
    create xgraph channel_hebb_post -hgeom 100% -title "hebbian postsynaptic activity"
    setfield ^ xmin 0 xmax {tmax} ymin -0.080 ymax -0.040 \
	       XUnits "sec" YUnits "V"
    useclock /graphics/hebb_postgraph 1
    pope

    xshow /graphics/hebb_postgraph
end



function make_hebb_weightgraph
    create xform /graphics/hebb_weightgraph [660,530,475,350]
    pushe /graphics/hebb_weightgraph
    create xgraph channel_hebb_weight -hgeom 100% -title "synaptic weight"
    setfield ^ xmin 0 xmax {tmax} ymin 0 ymax 3 \
	       XUnits "sec" YUnits "V"
    useclock /graphics/hebb_weightgraph 1
    pope

    xshow /graphics/hebb_weightgraph
end



//===============================
//         Main Script
//===============================

// Create the neuron "/cell".

makeneuron /cell {soma_l} {soma_d} {dend_l} {dend_d}
setfield /cell/soma inject 0.0


// Create the input spike generator and connect it to the cell.

makeinput /cell/dend/Ex_channel


// Make the control panel.

create neutral /graphics
make_control


// Make the graphs.

make_Vmgraph
addmsg /cell/soma   /graphics/data/voltage PLOT Vm *volts *red
addmsg /input/spike /graphics/data/voltage PLOTSCALE state  *input *blue  0.01     0
//                                                 title  color  scale  offset


make_condgraph
addmsg /cell/dend/Ex_channel /graphics/condgraph/channel_Gk PLOT Gk *Gk *black


make_hebb_pregraph
addmsg /cell/dend/Ex_channel /graphics/hebb_pregraph/channel_hebb_pre       PLOT synapse[0].pre_activity *act *black


make_hebb_postgraph
addmsg /cell/dend/Ex_channel /graphics/hebb_postgraph/channel_hebb_post     PLOT avg_Vm *act *black


make_hebb_weightgraph
addmsg /cell/dend/Ex_channel /graphics/hebb_weightgraph/channel_hebb_weight PLOT synapse[0].weight *act *black

check
reset
