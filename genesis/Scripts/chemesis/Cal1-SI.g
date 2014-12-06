//chemesis2.4
//cal1-SI.g

//units are m (SI)
float PI=3.14159
float somarad=12e-6
float somalen=24e-6
float LengthUnits=1 //1e-2  meters/cm, if using cm units
float areaout=2*PI*somarad*somalen
float volume=PI*somarad*somarad*somalen

float quantUnits=1  //indicates Moles (/m^3 = mMol)
float CaInit=0.0609e-3
float CaBufInit=12e-3
float BufTot=150e-3

create neutral cell
ce cell
/* create pools of molecules */
create rxnpool somaCa
setfield somaCa Cinit {CaInit} vol {volume} SAout {areaout} radius {somarad} units {quantUnits}  Dunits {LengthUnits}
create rxnpool somaCabuf
setfield somaCabuf Cinit {CaBufInit} vol {volume} SAout {areaout} radius {somarad} units {quantUnits}  Dunits {LengthUnits}
create conservepool somabuf
setfield somabuf Ctot {BufTot} Cinit {BufTot} volume {volume}  Dunits {LengthUnits}

/* create reaction */
create reaction somacabufrxn
setfield somacabufrxn kf 0.028e6 kb 19.6

/* messages from substrates and product to reaction */
addmsg somaCa somacabufrxn SUBSTRATE Conc
addmsg somabuf somacabufrxn SUBSTRATE Conc
addmsg somaCabuf somacabufrxn PRODUCT Conc

/* messages from reaction to substrates and products */
addmsg somacabufrxn somaCa RXN2 kbprod kfsubs
addmsg somacabufrxn somaCabuf RXN2 kfsubs kbprod

/* conserve pool messages */
addmsg somaCabuf somabuf CONC Conc

include cal1graphSI.g

setclock 0 0.001e-3
setclock 1 0.1e-3

setfield somaCa Cinit 1e-3
reset

step 10000

