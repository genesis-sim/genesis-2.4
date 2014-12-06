//chemesis2.0
//Cal7.g

int CHEMESIS=1
include const.g
//parameters for electrical aspects
//all units are SI 
float RA = 1  /* 100 ohm-cm */
float RM = 1    /* 1e4 ohm - cm^2; 5e-2 from Spruston et al */
float CM = 1e-2     /* 1e-6 Farads/cm^2; from Spruston et al. */
//currents through GHK need to be ~1e6x smaller than without GHK
float PCaT=20.0e-6

//function to create rxnpools of calcium, buffer and bimolecular reaction
include cabufSI.g
//function to create compartment
include compSI.g
//function to create calcium channel
include icaTab.g

//----------------start the model here------------------------
//neutral element which acts as directory, to organize elements
create neutral cell
ce cell

//invoke to function to create rxnpools and reaction
cabuf soma {CaInit} {CaBufInit} {BufTot} {somarad} {somalen} {lengthunits} {quantUnits}
cabuf dend {CaInit} {CaBufInit} {BufTot} {dendrad} {dendlen} {lengthunits} {quantUnits}

//create diffusion element which allows movement of Ca between compartments.
//Convert D to SI units wrt time (keep cm units)
create diffusion somadend
setfield somadend D {DiffConst} units {quantUnits} Dunits {lengthunits}
addmsg somaCa somadend POOL1 len SAside Conc
addmsg dendCa somadend POOL2 len SAside Conc
addmsg somadend somaCa RXN0MOLES difflux1
addmsg somadend dendCa RXN0MOLES difflux2

create conservepool caext
setfield caext Ctot 2 volume 0.001 Cinit 2

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

// Iunits of 1 indicates Amps, but -1 should convert from modeler to physiologists convention
   setfield somaCa Iunits -1
   setfield dendCa Iunits -1

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

