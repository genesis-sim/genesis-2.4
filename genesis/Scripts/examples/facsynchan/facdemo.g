// genesis 2.2 or later

// facdemo.g
//
// This script tests the `facsynchan' object.  A simple spike generator 
// sends a steady stream of spikes to a `facsynchan' object.  

setclock 0  20.0e-6 //  20 microseconds
setclock 1 100.0e-6 // 100 microseconds, for output

float tmax = 1.0 // seconds
float gmax = 1.0e-9 // 1 nS

int facdep = 1  // flag for facilitation/depression (1=fac,-1=dep,0=none)
float perspike = 1.0 // facilitation or depression per spike
float tau = 0.100    // 100 msec time constant for facilitation or depression

//===============================
//      Function Definitions
//===============================

function step_tmax
    step {tmax} -time
end

/* This function modifies one of the global variable parameters for the
    type of facilitation/depression (facdep), the facilitation or depression
    per spike (perspike), or the time constant for facilitation or depression
    (tau), and then sets up the facsynchan.  Note that this is not as general
    as it might be.  As described in facsynchan.txt, the facsynchan object can
    exibit both facilitation and depression at the same time, with different
    fac_per_spike, depr_per_spike, fac_tau, and depr_tau.  In this demo,
    the choices are either facilitation, depression, or neither.
*/

function setup_fac(param, value)
  str value, param
  // Note use of the new GENESIS 2.2 setglobal command
  setglobal {param} {value}
  str facsynpath = "/cell/syn"   // location of the facsynchan
/* Select the facilitation/depression type:
     facdep =  1 --> facilitation
     facdep = -1 --> depression
     facdep =  0 --> neither
*/
  if (facdep > 0)
    echo Synaptic facilitation ON...
    setfield {facsynpath}  \
        fac_depr_on      1          \
        fac_per_spike    {perspike} \
        depr_per_spike   0.0      \
        fac_tau          {tau}
  elif (facdep < 0)
    echo Synaptic depression ON...
    setfield {facsynpath} \
        fac_depr_on      1        \
        depr_per_spike    {perspike} \
        fac_per_spike   0.0      \
        depr_tau         {tau}
  else
    echo No synaptic facilitation or depression...
    setfield {facsynpath} fac_depr_on 0
  end
  call {facsynpath} RESET
end

//===============================
//    Graphics Functions
//===============================

function make_control
    create xform /control [10,50,275,155]
    create xlabel /control/label -hgeom 30 -bg cyan -label "CONTROL PANEL"
    create xbutton /control/RESET -wgeom 33%       -script reset
    create xbutton /control/RUN  -xgeom 0:RESET -ygeom 0:label -wgeom 33% \
         -script step_tmax
    create xbutton /control/QUIT -xgeom 0:RUN -ygeom 0:label -wgeom 34% \
        -script quit
    create xdialog /control/facdep -label "facdep (1=fac,-1=dep,0=none)" \
        -value {facdep} -script "setup_fac facdep <value>"
    create xdialog /control/perspike -label "fac/depr per spike" \
        -value {perspike} -script "setup_fac perspike <value>"
    create xdialog /control/tau -label "time constant (sec.)" \
        -value {tau} -script "setup_fac tau <value>"
    xshow /control
end

function make_Vmgraph
    float vmin = -0.075
    float vmax = -0.055
    create xform /data [290,50,350,350]
    create xlabel /data/label -hgeom 10% -label "Synaptic Facilitation Model"
    create xgraph /data/voltage  -hgeom 90%  -title "Post-synaptic Potential"
    setfield ^ XUnits sec YUnits Volts
    setfield ^ xmax {tmax} ymin {vmin} ymax {vmax}
    useclock ^ 1
    xshow /data
end

function make_condgraph
    create xform /condgraphs [645,50,350,350]
    pushe /condgraphs
    create xgraph channel_Gk -hgeom 100% -title "Channel Conductance"
    setfield channel_Gk xmin 0 xmax {tmax} ymin 0 ymax {gmax*5}
    setfield channel_Gk XUnits "sec" YUnits "Gk (siemens)"
    useclock channel_Gk 1
    pope
    xshow /condgraphs
end

//===============================
//     Main Simulation Script
//===============================

// Create a facilitating synapse connected to a compartment.

// cell dimensions (meters)

float PI    = 3.14159265358979
float len   = 30e-6            // cylinder equivalent to 30 micron sphere
float dia   = 30e-6
float area  = PI * dia * len
float xarea = PI * dia * dia / 4.0

float RM    =  0.33333      // specific membrane resistance (ohms m^2)
float CM    =  0.010        // specific membrane capacitance (farads/m^2)
float RA    =  0.3          // specific axial resistance (ohms m)
float Erest = -0.070        // resting membrane potential (volts)

create neutral /cell
pushe /cell
create compartment soma
setfield ^ \
    Em   { Erest }     \           // volts
    Rm   { RM / area } \           // Ohms
    Cm   { CM * area } \           // Farads
    Ra   { RA * len / xarea }      // Ohms


create facsynchan syn
setfield ^ \
    gmax             {gmax}   \   // 1 nS
    Ek               0.0      \
    tau1             0.001    \
    tau2             0.003

setup_fac facdep {facdep}  // Initialize the facsynchan

pope

addmsg /cell/soma /cell/syn  VOLTAGE  Vm
addmsg /cell/syn  /cell/soma CHANNEL  Gk Ek


/* Add a spike generator generating a regular series of spikes.
   The interspike interval is set by the abs_refract field of the
   /spiker object, which is a bit of a hack.  A more interesting
   demonstration might use pulsed trains of spikes, in order to better
   illustrate the effect of fac_tau and dep_tau on the recovery during
   periods of no spiking.  (see Scripts/neuron/inputs.g)
*/

create pulsegen /pulse
setfield ^ \
    baselevel -0.070 \
    level1 0.0  width1 100.0  delay1 0.050 \
    level2 10.0 width2 0.01 delay2 1000

create spikegen /spiker
setfield ^ thresh -0.020  abs_refract 0.050
addmsg /pulse /spiker INPUT output

//
// Create synapse and set synapse fields.
//

addmsg /spiker /cell/syn SPIKE
setfield /cell/syn synapse[0].weight 1.0 synapse[0].delay 0.001

/* Add the graphical interface */

// make the control panel
make_control

// make the graph to display soma Vm and pass messages to the graph
make_Vmgraph
addmsg /cell/soma /data/voltage PLOT Vm *volts *red

// Make synaptic conductance graph and pass message to the graph
make_condgraph
addmsg /cell/syn /condgraphs/channel_Gk PLOT Gk *Gk *black

reset
check
