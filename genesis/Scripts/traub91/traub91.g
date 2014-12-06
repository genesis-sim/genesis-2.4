// genesis 2.2 - "Stand-alone" version of the traub91 tutorial with
// hsolvable tab2Dchannel for K_C current

int hflag = 1    // use hsolve if hflag = 1
int chanmode = 3

/* chanmodes 0 and 3 allow outgoing messages to non-hsolved elements.
   chanmode 3 is fastest.
*/

float tmax = 0.1               // simulation time in sec
float dt = 0.00005              // simulation time step in sec
setclock  0  {dt}               // set the simulation clock
float injcurr = 0.2e-9		// default injection

// Create a library of prototype elements to be used by the cell reader

/* file for standard compartments */
// include compartments
/* file for 1991 Traub model channels */
include traub91proto.g

create neutral /library
pushe /library
//    make_cylind_symcompartment  /* makes "symcompartment" */
create symcompartment symcompartment
/* Assign some constants to override those used in traub91proto.g */
EREST_ACT = -0.06       // resting membrane potential (volts)
float ENA = 0.115 + EREST_ACT // 0.055  when EREST_ACT = -0.060
float EK = -0.015 + EREST_ACT // -0.075
float ECA = 0.140 + EREST_ACT // 0.080

make_Na
make_Ca
make_K_DR
make_K_AHP
make_K_C
make_K_A
make_Ca_conc

pope

//===============================
//      Function Definitions
//===============================

function step_tmax
    step {tmax} -time
end

//===============================
//    Graphics Functions
//===============================

function make_control
    create xform /control [10,50,250,170]
    create xlabel /control/label -hgeom 40 -bg cyan -label "CONTROL PANEL"
    create xbutton /control/RESET -wgeom 33%       -script reset
    create xbutton /control/RUN  -xgeom 0:RESET -ygeom 0:label -wgeom 33% \
         -script step_tmax
    create xbutton /control/QUIT -xgeom 0:RUN -ygeom 0:label -wgeom 34% \
        -script quit
    create xdialog /control/Injection -label "Injection (amperes)" \
		-value {injcurr}  -script "set_inject <widget>"
    create xdialog /control/stepsize -title "dt (sec)" -value {dt} \
                -script "change_stepsize <widget>"
    create xtoggle /control/overlay  \
           -script "overlaytoggle <widget>"
    setfield /control/overlay offlabel "Overlay OFF" onlabel "Overlay ON" state 0

    xshow /control
end

function make_Vmgraph
    float vmin = -0.100
    float vmax = 0.05
    create xform /data [265,50,350,350]
    create xlabel /data/label -hgeom 10% -label "Traub CA3 Pyramidal Cell "
    create xgraph /data/voltage  -hgeom 90%  -title "Membrane Potential"
    setfield ^ XUnits sec YUnits Volts
    setfield ^ xmax {tmax} ymin {vmin} ymax {vmax}
    xshow /data
end

function make_xcell
    create xform /cellform [620,50,400,400]
    create xdraw /cellform/draw [0,0,100%,100%]
    setfield /cellform/draw xmin -0.003 xmax 0.001 ymin -5e-5 ymax 5e-5 \
        zmin -1e-3 zmax 1e-3 \
        transform z
    xshow /cellform
    echo creating xcell
    create xcell /cellform/draw/cell
    setfield /cellform/draw/cell colmin -0.1 colmax 0.1 \
        path /cell/##[TYPE=symcompartment] field Vm \
        script "echo <w> <v>"
end

function set_inject(dialog)
    str dialog
    setfield /cell/soma inject {getfield {dialog} value}
end

function change_stepsize(dialog)
   str dialog
   dt =  {getfield {dialog} value}
   setclock 0 {dt}
   echo "Changing step size to "{dt}
end

// Use of the wildcard sets overlay field for all graphs
function overlaytoggle(widget)
    str widget
    setfield /##[TYPE=xgraph] overlay {getfield {widget} state}
end

//===============================
//         Main Script
//===============================
// Build the cell from a parameter file using the cell reader
readcell CA3.p /cell

setfield /cell/soma inject {injcurr}

// make the control panel
make_control

// make the graph to display soma Vm and pass messages to the graph
make_Vmgraph
addmsg /cell/soma /data/voltage PLOT Vm *volts *red

/* comment out the two lines below to disable the cell display (faster)  */
make_xcell // create and display the xcell
xcolorscale hot

if (hflag)
    create hsolve /cell/solve
    setfield /cell/solve path "/cell/##[][TYPE=symcompartment]"
    setmethod 11
    setfield /cell/solve chanmode {chanmode}
    call /cell/solve SETUP
    reset
    echo "Using hsolve"
end

//check
reset
