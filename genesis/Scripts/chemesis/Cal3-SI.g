//chemesis2.0
//Cal3-SI.g

//function to create rxnpools of calcium, buffer and bimolecular reaction
include cabufSI.g
//function to set up iicr
include iicrfunc.g

//dimensions of compartments defined near the top for ease of changing
//These will go into const.g in next simulation
//units are m (SI)
float somarad=6e-6
float somalen=12e-6
float dendrad=2e-6
float dendlen=12e-6
float lengthunits=1 //e-4  //meters/cm, if using cm units
float quantUnits=1 // moles
float DiffConst=600e-12 //m^2/sec
float CaInit=0.0609e-3 //mMolar
float CaBufInit=12e-3
float BufTot=150e-3
float CaER=0.02
float CaBufER=4.83
float BufTotER=5.0
int iicrpower=3
float maxiicr=0.2e-9 //meters/sec

//neutral element which acts as directory, to organize elements
create neutral cell
ce cell

//invoke to function to create rxnpools and reaction
// also create rxnpool and buffer in ER
cabuf soma {CaInit} {CaBufInit} {BufTot} {somarad} {somalen} {lengthunits} {quantUnits}
cabuf dend {CaInit} {CaBufInit} {BufTot} {dendrad} {dendlen} {lengthunits} {quantUnits}
cabuf ER {CaER} {CaBufER} {BufTotER} {somarad} {somalen*0.185} {lengthunits} {quantUnits}

//create diffusion element which allows movement of Ca between compartments.
create diffusion somadend
setfield somadend D {DiffConst} units {quantUnits} Dunits {lengthunits}
addmsg somaCa somadend POOL1 len SAside Conc
addmsg dendCa somadend POOL2 len SAside Conc
addmsg somadend somaCa RXN0MOLES difflux1
addmsg somadend dendCa RXN0MOLES difflux2

/*setup ip3 induced calcium release.  This illustrates how two of the
elements are created. X_state is a 0 or 1 variable indicating whether
ip3 (alpha) or calcium (beta and gamma) are bound.  rate constants are
forward rates: transition from the state of the element _to_ a
different state.  conserve is a 0 or 1 variable indicating whether
fraction is calculated from ODE or by subtracting fraction in all
other states.*/

   create cicr ERCa/x000
   setfield ERCa/x000 \
	alpha_state	0 \
	beta_state	0 \
	gamma_state	0 \
	alpha	400e3 \
	beta	20e3 \
	gamma	0.2e3 \
	conserve	0 \
	xinit		0.325 \
	xmin		0 \
	xmax		1
	
   create cicr ERCa/x100
   setfield ^ \
	alpha_state	1 \
	beta_state	0 \
	gamma_state	0 \
	alpha	52 \
	beta	20e3 \
	gamma	0.2e3 \
	conserve	0 \
	xinit		0.0 \
	xmin		0 \
	xmax		1 

/*Messages between these two states: when going from two states which differ in alpha value, the message is ASTATE.  pass the rate constant, and fraction of channels in that state.  Because of the sequential way genesis numerically solves this system, need to pass previous_state from elements that are created (and therefore processed) first. */
addmsg ERCa/x100 ERCa/x000 ASTATE alpha fraction
addmsg ERCa/x000 ERCa/x100 ASTATE alpha previous_state

// this function sets up all 8 states for the DeYoung Keizer model
makeiicr ERCa

//Create an object provide a constant amount of ip3
create conservepool ip3
setfield ip3 Ctot 0.0 volume 1.0 Dunits {lengthunits}

/*This function finishes setting up calcium release.
1.messages go from somaCa to each cicr element
2. message go from ip3 to each cicr element
3. messages go from the open and conducting state (x110) to an element which
calculates flux through the open channel
*/
makeiicrflux somaCa ERCa ip3 {maxiicr} {iicrpower} {quantUnits}

//setup graphs to display results
include cal3graphSI.g

//again, use a larger dt for output
setclock 0 0.25e-6
setclock 1 0.1e-3

reset

  step 5000
  setfield ip3 Ctot 1.0 
  step 45000 
  setfield ip3 Ctot 0.0
  step  50000
  step 100000

/*Notice that calcium remains elevated, because diffusion is a slow way to
return calcium to equilibrium.  The elevated calcium pushes the release
channels into refractory model. */
//Simulating for additional 2 sec or so will show the diffusion
//Step 2 -t
