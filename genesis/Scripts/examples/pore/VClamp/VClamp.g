//genesis
//*************************************************************************
//Adam Franklin Strassberg
//March 15, 1992
//*************************************************************************


//*************************************************************************
//Voltage Clamp simulation of a Patch of Membrane
//with Single Sodium and Potassium Channels undergoing 
//standard Hodgkin-Huxley channel kinetics

//Uses specially designed objects Kpores and Napores

//time		msec
//area		um^2
//voltage	mV
//conductance	pS

//(Basic simulation runs for 20ms in 0.01ms steps)
//*************************************************************************


//*************************************************************************
//Reset Function - intializes the simulation 
function reset_sim

float area
float numNa
float numK
float KGmax
float NaGmax

area = {getfield /data/Area value}
numK = {getfield /data/Kdense value}*area
numNa = {getfield /data/Nadense value}*area
KGmax = {getfield /data/KGopen value}
NaGmax = {getfield /data/NaGopen value}

//Initialize the kinetic channels 
setfield /K Gmax {KGmax}
setfield /Na Gmax {NaGmax}
setfield /K Nt {numK}
setfield /Na Nt {numNa}

//Initiailize Gk`s for Vrest=0 volt steady state
setfield /K Gk 0
setfield /Na Gk 0
setfield /K Vk 0
setfield /Na Vk 0

//Initialize the voltage pulse generator
setfield /pulse x {getfield /data/Vmi value}

//Reset the simulation system
reset

end
//*************************************************************************


//*************************************************************************
//Run Function - runs the simulation 

function run_sim

//Initialize the Simulation
reset_sim

//Run the Simulation
step 500
setfield /pulse x {getfield /data/Vmf value}
step 1500

end
//*************************************************************************


//*************************************************************************
//Dummy Function - needed to satisfy certain script fields 
function void
end
//*************************************************************************


//*************************************************************************
//Graphing Functions - allow the user to change the scaling of the graph

function set_scale(gpath)
    str gpath
    float xmin, xmax, ymin, ymax, overlay
    xmin = {getfield {gpath} xmin}
    xmax = {getfield {gpath} xmax}
    ymin = {getfield {gpath} ymin}
    ymax = {getfield {gpath} ymax}
    overlay = {getfield {gpath} overlay}

    setfield /gscale/xmin value {xmin}
    setfield /gscale/xmax value {xmax}
    setfield /gscale/ymin value {ymin}
    setfield /gscale/ymax value {ymax}
    setfield /gscale/overlay value {overlay}
    xshow /gscale
end

function set_graph(field)
    str field
    str path
    path = "/gscale/" @ (field)
    setfield /data/outgraph {field} {getfield {path} value}
end
//*************************************************************************


//*************************************************************************
//Create Simulation Objects

setclock 0 0.01

//Use Standard Hodgkin-Huxley Rate Constants
//to create populations of both K and Na channels
create Kpores /K
setfield ^ alpha_A 0.1 alpha_B -0.01 alpha_C -1.0 alpha_D -10.0  \
    alpha_F -10.0 beta_A 0.125 beta_B 0.0 beta_C 0.0 beta_D 0.0  \
    beta_F 80.0
create Napores /Na
setfield ^ malpha_A 2.5 malpha_B -0.1 malpha_C -1.0 malpha_D -25.0  \
    malpha_F -10.0 mbeta_A 4.0 mbeta_B 0.0 mbeta_C 0.0 mbeta_D 0.0  \
    mbeta_F 18.0 halpha_A 0.07 halpha_B 0.0 halpha_C 0.0 halpha_D 0.0  \
    halpha_F 20.0 hbeta_A 1.0 hbeta_B 0.0 hbeta_C 1.0 hbeta_D -30.0  \
    hbeta_F -10.0

//Voltage Pulse Generator
create neutral /pulse
//*************************************************************************


//*************************************************************************
//Create Display Object for Simulation Interface 
create xform /data [0,0,400,650] -title "K+ and Na+ (N) Channels"
create xdialog /data/Vmf -script void -value 50.0 -title "Vmf(mV)"
create xdialog /data/Vmi -script void -value 0.0 -title "Vmi(mV)"
create xdialog /data/Area -script void -value 1.0 -title "Area(um2)"
create xdialog /data/KGopen -script void -value 20.0 -title "KGopen(pS)"
create xdialog /data/Kdense -script void -value 18.0 -title  \
    "Density of K Channels(/um2)"
create xdialog /data/NaGopen -script void -value 20.0 -title  \
    "NaGopen(pS)"
create xdialog /data/Nadense -script void -value 60.0 -title  \
    "Density of Na Channels(/um2)"
create xgraph /data/outgraph [0,0:Nadense,100%,300] -title "Conductance Gk(t)"
setfield /data/outgraph xmin 0 xmax 20 ymin 0 ymax 1000
setfield /data/outgraph XUnits ms YUnits pS
create xbutton /data/outgraph/scale  \
    [-50:outgraph.right,0:Nadense.bottom,50,25] -title "scale" -script  \
    "set_scale /data/outgraph"
create xbutton /data/RESET [0,0:outgraph.bottom,100%,25] -script reset_sim
create xbutton /data/RUN -script run_sim
create xbutton /data/STOP -script stop
create xbutton /data/QUIT -script quit
//*************************************************************************


//*************************************************************************
//Create Display for Graph Scaling
create xform /gscale [300,100,235,235] -title "Graph Scale"
create xdialog /gscale/xmin [10,45,200,25] -value "???" -script  \
    "set_graph xmin"
create xdialog /gscale/xmax [10,80,200,25] -value "???" -script  \
    "set_graph xmax"
create xdialog /gscale/ymin [10,115,200,25] -value "???" -script  \
    "set_graph ymin"
create xdialog /gscale/ymax [10,150,200,25] -value "???" -script  \
    "set_graph ymax"
create xdialog /gscale/overlay [10,185,200,25] -value "???" -script  \
    "set_graph overlay"
create xbutton /gscale/DONE [10,10,50,25] -script "xhide /gscale"
//*************************************************************************


//*************************************************************************
//Link Interface Elements
addmsg /pulse /K VOLTAGE x
addmsg /pulse /Na VOLTAGE x
addmsg /K /data/outgraph PLOT Gk *GK *red
addmsg /Na /data/outgraph PLOT Gk *GNa *blue

setclock 1 0.1

/*Creates output data files*/
/*
create asc_file /Kout
useclock /Kout 1
set /Kout filename Kout
sendmsg /K /Kout SAVE Gk

create asc_file /Naout
useclock /Naout 1
set /Naout filename Naout
sendmsg /Na /Naout SAVE Gk
*/

//*************************************************************************


//*************************************************************************
//Dump Simulation Interface Window to the Screen
xshow /data
//*************************************************************************
