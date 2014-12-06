//genesis  -  tutorial5.g - GENESIS Version 2.0

/*======================================================================
  A sample script which uses the cell reader to create a multicompartmental
  neuron with synaptic input.  SI units are used.
  ======================================================================*/

// Create a library of prototype elements to be used by the cell reader
include protodefs

float gmax = 5e-10              // maximum synaptic conductance (Siemen)

float tmax = 0.1                // simulation time in sec
float dt = 0.00005              // simulation time step in sec
setclock  0  {dt}               // set the simulation clock

//===============================
//      Function Definitions
//===============================

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
    setfield channel_Gk XUnits "sec" YUnits "Gk (siemens)"
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
// Build the cell from a parameter file using the cell reader
readcell cell.p /cell

setfield /cell/soma inject 0.0

// make the control panel
make_control

// make the graph to display soma Vm and pass messages to the graph
make_Vmgraph
addmsg /cell/soma /data/voltage PLOT Vm *volts *red

makeinput /cell/dend/Ex_channel        // Create synaptic inputs

// Make synaptic conductance graph and pass message to the graph
make_condgraph
addmsg /cell/dend/Ex_channel /condgraphs/channel_Gk PLOT Gk *Gk *black

// finally, we add some feedback from the axon to the dendrite
addmsg /cell/soma/spike /cell/dend/Ex_channel SPIKE
setfield /cell/dend/Ex_channel \
    synapse[1].weight 10 synapse[1].delay 0.005

check
reset
