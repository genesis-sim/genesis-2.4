//chemesis2.0
//mglu-ip3-chemesis.g
//always indicate which units you are using: cm, uM, sec

// mglu-param.g stores parameters used in simulation
include mglu-param.g

/*Glu-mGLuR + G <-> G-Glu-mGluR -> aG + Glu-mGluR (uses Vmax2 Km2)
 aG + PLC <-> aPLC-aG (uses G2)
 aPLC + PIP2 <-> aPLC-PIP2 -> aPLC +IP3+ DAG (uses Vmax1 Km1)
 */

/*** organize elements under the subelement /chem ***/
create neutral /chem
str cell="/chem"

/*Adjust time step; use larger output time step to improve simulation speed*/
/* simulate at dt=1e-5 sec, but output to graphs at slower rate: 1e-3 */
setclock 0 1e-5
setclock 1 1e-3

/* Binding of glutamate to receptor is very fast.  Instead of
modeling this explicitly, just create a simple element which provides
a constant value of bound mGluR.  
Initialize z to 0, but we will change it later during simulations.*/
create neutral {cell}/Glu_mGluR
setfield {cell}/Glu_mGluR z 0

/* Create pools of molecules.  One element is required for each molecule.
Conservepool calculates concentration by subtracting other forms of
molecule.  Three of these elements are needed. */

create conservepool {cell}/Gtot /* pool of G protein*/
setfield {cell}/Gtot Cinit {K_G} Ctot {K_G} volume {Volume} units {uM} Dunits {lenUnits}

create conservepool {cell}/PLCtot /* pool of PLC */
setfield {cell}/PLCtot Cinit {K_PLC} Ctot {K_PLC} volume {Volume} units {uM} Dunits {lenUnits}

create conservepool {cell}/PIP2tot /* pool of PIP2 substrate */
setfield {cell}/PIP2tot Cinit {K_PIP2} Ctot {K_PIP2} volume {Volume} units {uM} Dunits {lenUnits}
 
/* create three elements to calculate & store concentrations and quantities 
of molecules determined from differential equations describing 
the change in concentration from various fluxes.*/ 
/*Assign parameter values such as compartment volumes 
(to calculate concentration from quantity and vice versa), and initial 
quantities using the genesis setfield command. */

create rxnpool {cell}/aG /*pool of active G_alpha */
setfield {cell}/aG vol {Volume} Cinit 0.0007 units {uM} Dunits {lenUnits}

create rxnpool {cell}/aPLC_aG /*pool of active PLC (bound to G_alpha) */
setfield {cell}/aPLC_aG vol {Volume} Cinit 0.0035 units {uM} Dunits {lenUnits}

create rxnpool {cell}/IP3          /* pool of IP3 (Created from PIP2)*/
setfield {cell}/IP3 vol {Volume} Cinit 0.1 units {uM} Dunits {lenUnits}

//verify that elements are setup correctly
le
showfield {cell}/aG -a
showfield {cell}/aPLC_aG -a
showfield {cell}/IP3 -a

/* Now create the reactions that will increase or decrease various molecules*/
/* Both implement bimolecular and enzyme reactions: Create reaction,
then set up messages between substrates, products and reactions.
 reactions only have rate constants, no volume or concentrations */
/*First enzyme reaction uses the Michaelis-Menton formulation.
I.e., quantity of enzyme-substrate complex is not explicit */

/* Glu-mGluR + Gtot <-> GGlu-mGluR -> Ga + Glu-mGluR + Gbg */
create mmenz {cell}/GGlu_mGluR
setfield {cell}/GGlu_mGluR Vmax {Vmax2} Km {Km2} 
addmsg {cell}/Gtot {cell}/GGlu_mGluR SUBSTRATE Conc
addmsg {cell}/Glu_mGluR {cell}/GGlu_mGluR ENZ z
addmsg {cell}/GGlu_mGluR {cell}/aG RXN0MOLES product

/* Next reaction is bimolecular - one aG to one PLC 
incoming Messages are concentration of both substrates and product
outgoing messages are the  change in substrate and product concentrations */
/* aG + PLC <-> aPLC-aG (uses G2) */
create reaction {cell}/PLC_rxn
setfield {cell}/PLC_rxn kf {G2f} kb {G2b} 
addmsg {cell}/aG {cell}/PLC_rxn SUBSTRATE Conc
addmsg {cell}/PLCtot {cell}/PLC_rxn SUBSTRATE Conc
addmsg {cell}/aPLC_aG {cell}/PLC_rxn PRODUCT Conc
addmsg {cell}/PLC_rxn {cell}/aG RXN2 kbprod kfsubs 
addmsg {cell}/PLC_rxn {cell}/aPLC_aG RXN2 kfsubs kbprod 

/* Second enzyme reaction uses the enzyme object. This explicitly
calculates the quantity of enzyme substrate complex.  
Thus, this object functions in part as a rxnpool, and has a volume */
/* aPLC-aG + PIP2 <-> aPLC-PIP2 -> aPLC-aG+IP3+ DAG */
create enzyme {cell}/pip2ip3
setfield {cell}/pip2ip3 kcat {Vmax1} kf {kfplc} kb {kbplc} units {uM} vol {Volume} Dunits {lenUnits}
addmsg {cell}/aPLC_aG {cell}/pip2ip3 ENZ Conc
addmsg {cell}/PIP2tot {cell}/pip2ip3 SUBSTRATE Conc
addmsg {cell}/pip2ip3 {cell}/IP3 RXN0MOLES deltacat
addmsg {cell}/pip2ip3 {cell}/aPLC_aG RXN2 deltaenz kfsubs

//verify that messages have been setup correctly
showmsg {cell}/GGlu_mGluR
showmsg {cell}/PLC_rxn
showmsg {cell}/pip2ip3

/* Last set of messages is quantities from rxnpools to conservepools*/

/* G=K_G-aG-aPLC_aG-aPLC_PIP2 */
addmsg {cell}/aG {cell}/Gtot CONC Conc
addmsg {cell}/aPLC_aG {cell}/Gtot CONC Conc
addmsg {cell}/pip2ip3 {cell}/Gtot CONC complex_conc

/* PLC=K_PLC-aPLC_aG -aPLC_PIP2*/
addmsg {cell}/aPLC_aG {cell}/PLCtot CONC Conc
addmsg {cell}/pip2ip3 {cell}/PLCtot CONC complex_conc

/* PIP2=K_PI-IP3-aPLC_PIP2 */
addmsg {cell}/IP3 {cell}/PIP2tot  CONC Conc
addmsg {cell}/pip2ip3 {cell}/PIP2tot CONC complex_conc

useclock {cell}/# 0
setmethod {cell}/# -1

/*Method of output is graphical.  Alternatively, could write to file.*/
include mglu-graphs.g
graphs {cell} 
reset

/*This part runs the simulation*/
step 10000
setfield {cell}/Glu_mGluR z {K_mGluR}
step 10000
setfield {cell}/Glu_mGluR z 0
step 180000

/* Since no inactivation mechanisms for IP3 or G, Ip3 continues to increase*/
//step 200000
