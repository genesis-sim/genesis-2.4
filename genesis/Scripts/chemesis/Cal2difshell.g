//genesis
//caldifshell.g
//This gives qualitatively similar results, and is faster than chemesis
//function to create difshell of calcium, and difbuffer with reaction
include cabufdifshell

//dimensions of compartments defined near the top for ease of changing
//units are m (SI)
//Repeat simulations with length of soma and den = 2, 1 and then 0.5e-4
//Note that diffusion is quite slow with longer
float somarad=4e-4
float somalen=0.5e-4
float dendrad=2e-4
float dendlen=0.5e-4
float lengthunits=1 //e-4  //meters/cm, if using cm units
float ConcUnits=1
float DiffConst=600e-12 //m^2/sec
float CaInit=0.0609e-3
float CaBufInit=12e-3
float BufTot=150e-3
//neutral element which acts as directory, to organize elements
create neutral cell
ce cell

//invoke to function to create rxnpools and reaction
cabufdifshell soma {CaInit} {BufTot} {somarad} {somalen} {DiffConst} {DiffConst}
cabufdifshell dend {CaInit} {BufTot} {dendrad} {dendlen} {DiffConst} {DiffConst}

//create diffusion which allows movement of Ca between compartments.
addmsg somaCa dendCa DIFF_DOWN prev_C thick
addmsg dendCa somaCa DIFF_UP prev_C thick

//setup graphs to display results
include cal2graphDifshell.g

//again, use a larger dt for output
//These are so small because units are sec
setclock 0 0.002e-3
setclock 1 1e-3

//this simulation will not show diffusion, because the compartments are too large
setfield /graph1/# xmax 1
reset
setfield somaCa C 1e-3
step 1 -t

setfield /graph1/ca ymin 0.05 ymax 0.08
//Repeat simulation with diffusion of buffer between compartments.
addmsg somaCabuf dendCabuf DIFF_DOWN prev_free thick
addmsg dendCabuf somaCabuf DIFF_UP prev_free thick

setfield /graph1/# overlay 1
reset
setfield somaCa C 1e-3
step 1 -t


