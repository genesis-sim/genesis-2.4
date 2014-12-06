//chemesis2.0
//cal2-SI.g - SI units

//function to create rxnpools of calcium, buffer and bimolecular reaction
include cabufSI.g

//dimensions of compartments defined near the top for ease of changing
//units are m (SI)
//Repeat simulations with length of soma and den = 2, 1 and then 0.5e-4
//Note that diffusion is quite slow with longer
float somarad=6e-6
float somalen=6e-6
float dendrad=2e-6
float dendlen=6e-6
float lengthunits=1 //1e-2  meters/cm, if using cm units
float quantUnits=1 // moles  (/m^3 = mMol)
float DiffConst=600e-12 //m^2/sec
float CaInit=0.0609e-3
float CaBufInit=12e-3
float BufTot=150e-3

//neutral element which acts as directory, to organize elements
create neutral cell
ce cell

//invoke to function to create rxnpools and reaction
cabuf soma {CaInit} {CaBufInit} {BufTot} {somarad} {somalen} {lengthunits} {quantUnits}
cabuf dend {CaInit} {CaBufInit} {BufTot} {dendrad} {dendlen} {lengthunits} {quantUnits}

//create diffusion element which allows movement of Ca between compartments.
create diffusion somadend
setfield somadend D {DiffConst} units {quantUnits} Dunits {lengthunits}
addmsg somaCa somadend POOL1 len SAside Conc
addmsg dendCa somadend POOL2 len SAside Conc
addmsg somadend somaCa RXN0MOLES difflux1
addmsg somadend dendCa RXN0MOLES difflux2

//setup graphs to display results
include cal2graphSI.g

//again, use a larger dt for output
//These are so small because units are sec
setclock 0 0.002e-3
setclock 1 1e-3

reset
//this simulation has slow diffusion because immobile buffers
setfield somaCa Cinit 1e-3
setfield /graph1/# xmax 2
setfield /graph1/buf ymin 11 ymax 13
setfield /graph1/ca ymin 0.05 ymax 0.08
reset
step 2 -t

//Repeate simulation with diffusion of buffer
create diffusion sdbuf
setfield sdbuf D {DiffConst} units {quantUnits} Dunits {lengthunits}
addmsg somaCabuf sdbuf POOL1 len SAside Conc
addmsg dendCabuf sdbuf POOL2 len SAside Conc
addmsg sdbuf somaCabuf RXN0MOLES difflux1
addmsg sdbuf dendCabuf RXN0MOLES difflux2

setfield /graph1/# overlay 1
reset
step 2 -t

