//genesis2.3
//Cal7difshell.g

int CHEMESIS=0
include const.g
//parameters for electrical aspects
//all units are SI 
float RA = 1  /* 100 ohm-cm */
float RM = 1    /* 1e4 ohm - cm^2; 5e-2 from Spruston et al */
float CM = 1e-2     /* 1e-6 Farads/cm^2; from Spruston et al. */
//currents through GHK need to be ~1e6x smaller than without GHK
float PCaT=20.0e-6

//function to create rxnpools of calcium, buffer and bimolecular reaction
include cabufdifshell.g
//function to create compartment
include compSI.g
//function to create calcium channel
include icaTab.g

//----------------start the model here------------------------
//neutral element which acts as directory, to organize elements
create neutral cell
ce cell

//invoke to function to create rxnpools and reaction
cabufdifshell soma {CaInit} {BufTot} {somarad} {somalen} {DiffConst} {DiffConst}
cabufdifshell dend {CaInit} {BufTot} {dendrad} {dendlen} {DiffConst} {DiffConst}
reset
//create diffusion which allows movement of Ca between compartments.
addmsg somaCa dendCa DIFF_DOWN prev_C thick
addmsg dendCa somaCa DIFF_UP prev_C thick

//Use neutral object to store 2 mM Ca concentration
create neutral caext
setfield caext z 2

//*********create electrical cell compartment***********
make_comp soma {somalen} {somarad}
make_comp dendrite {dendlen} {dendrad}
addmsg dendrite soma RAXIAL Ra previous_state
addmsg soma dendrite AXIAL  previous_state

//add calcium channels, use SI units else ghk won't work
//consider making this channel in the library and copy into both compartments
   float Actvhalf=-20e-3
   float Actslope=-10e-3
   float Acttau=5.0e-3
   float Inactvhalf=-49e-3
   float Inactslope=5e-3
   float Inacttau=75e-3
   make_ica_ghk soma somaCa caext "CaT" {PCaT} {Actvhalf} {Actslope} {Acttau} {Inactvhalf} {Inactslope} {Inacttau}

//setup graphs to display results
include cal7graph.g

//again, use a larger dt for output
setclock 0 0.25e-6
setclock 1 0.1e-3

reset
step 100000
setfield soma inject 0.1e-9
step 10000
showmsg /graph1/ca
setfield soma inject 0
step 40000
showmsg /graph1/ca
step 50000
step 800000
//The next commands reveal some diffusion
setfield /graph1/# xmax 0.5
step 1000000
