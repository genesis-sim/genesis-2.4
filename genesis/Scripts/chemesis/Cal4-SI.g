//chemesis2.0
//Cal4-SI.g

//function to create rxnpools of calcium, buffer and bimolecular reaction
include cabufSI.g
//function to set up iicr
include iicrfunc.g
//function to set up serca pump, to return calcium to ER
include calpumpSI.g
include const.g

//neutral element which acts as directory, to organize elements
create neutral cell
ce cell

//invoke to function to create rxnpools and reaction
// also create rxnpool and buffer in ER
cabuf soma {CaInit} {CaBufInit} {BufTot} {somarad} {somalen} {lengthunits} {quantUnits}
cabuf dend {CaInit} {CaBufInit} {BufTot} {dendrad} {dendlen} {lengthunits} {quantUnits}
cabuf ER {CaER} {CaBufER} {BufTotER} {somarad} {somalen*0.185} {lengthunits} {quantUnits}

//create diffusion element which allows movement of Ca between compartments.
/* This could be put into a function, which could be invoked when creating
diffusion between multiple sets of compartments. */
create diffusion somadend
setfield somadend D {DiffConst} units {quantUnits} Dunits {lengthunits}
addmsg somaCa somadend POOL1 len SAside Conc
addmsg dendCa somadend POOL2 len SAside Conc
addmsg somadend somaCa RXN0MOLES difflux1
addmsg somadend dendCa RXN0MOLES difflux2

/*Because of the "fractal" nature of the ER membrane, calcium release scales better if maximal conductance or permeability is made proportional to volume */
float area = {getfield somaCa vol}
float iicrflux=maxiicr*area
float serca=sercarate*area

create conservepool ip3
setfield ip3 Ctot 0.0 volume 0.001

//setup ip3 induced calcium release using 8 state DeYoung Keizer model.
makeiicr ERCa
makeiicrflux somaCa ERCa ip3 {maxiicr} {iicrpower} {quantUnits}

// set up SERCA pump
makepump somaCa ERCa {serca} {sercapower} {sercaKm}

/* set up compensatory leak.  SERCA pump will continue
to work until Ca = 0.  Thus, leak must be set to balance pump at resting
or equilibrium calcium concentration. Equation for sercaflux determines the
leak conductance from the ER calcium, serca rate, and Ceq. */
float Ceq = {getfield somaCa Cinit}
float CaER = {getfield ERCa Cinit}
float sercaflux=(serca* {pow {Ceq} {sercapower}})/({pow {Ceq} {sercapower}} + {pow {sercaKm} {sercapower}})
float leak=sercaflux / (CaER - Ceq)
makeleak somaCa ERCa {leak} 1

//setup graphs to display results
include cal4graphSI.g

//use large dt for output
setclock 0 0.25e-6
setclock 1 0.1e-3

reset

  step 5000
  setfield ip3 Ctot 2e-3 
  step 45000 
//  setfield ip3 Ctot 0.0
  step 50000
  step 100000
  step 1800000
//step 16000000
