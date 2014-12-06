//genesis
//*************************************************************************
//Adam Strassberg
//March 15, 1992

//Create Core Simulation Objects
//*************************************************************************


//*************************************************************************
//Current Pulse Input to a Patch of Membrane
//using standard Hodgkin-Huxley continuous dynamics

//time		ms
//area		um^2
//voltage	mV
//current	pA
//capacitance	pF
//conductance	pS
//*************************************************************************


//*************************************************************************
//  Hodgkin-Huxley prototypes for squid giant axon channels 

//  From H & H (1952) J Physiol 117, 500-544.

//  RESTING VOLTAGE is assumed to be ZERO.
//  SIGN CONVENTION for voltage has been flipped to agree with
//  modern convention (depolarization +, hyperpolarization -)
//*************************************************************************


//*************************************************************************
//Simulation Constants

// Specific membrane capacitance and conductances, 
// (from HH, J. Physiol. 117, 500-544 (1952) - Table 3) 


// Vvoltages (from HH, J. Physiol. 117, 500-544 (1952) - Table 3,
// but with opposite sign convention) 

// mV
float Vrest_squid = 0.0
// mV
float VNa_squid = 115.0
// mV
float VK_squid = -12.0
//*************************************************************************


//*************************************************************************
//Create Patch Object
create compartment /patch
//*************************************************************************


//*************************************************************************
//Create Continuous H-H Channel Variables

//====================
//  Squid Na channel
//====================


create vdep_channel /patch/Na
	setfield /patch/Na Ek {VNa_squid}

create vdep_gate /patch/Na/m
	setfield ^ alpha_A 2.5 alpha_B -0.1 alpha_C -1.0 alpha_D -25.0  \
	    alpha_F -10.0 beta_A 4.0 beta_B 0.0 beta_C 0.0 beta_D 0.0  \
	    beta_F 18.0 instantaneous 0

create vdep_gate /patch/Na/h
	setfield ^ alpha_A 0.07 alpha_B 0.0 alpha_C 0.0 alpha_D 0.0  \
	    alpha_F 20.0 beta_A 1.0 beta_B 0.0 beta_C 1.0 beta_D -30.0  \
	    beta_F -10.0 instantaneous 0

addmsg /patch/Na/m /patch/Na MULTGATE m 3
addmsg /patch/Na/h /patch/Na MULTGATE m 1



//===================
//  Squid K channel
//===================


create vdep_channel /patch/K
        setfield /patch/K Ek {VK_squid}

create vdep_gate /patch/K/n
	setfield ^ alpha_A 0.1 alpha_B -0.01 alpha_C -1.0 alpha_D -10.0  \
	    alpha_F -10.0 beta_A 0.125 beta_B 0.0 beta_C 0.0 beta_D 0.0  \
	    beta_F 80.0 instantaneous 0

addmsg /patch/K/n /patch/K MULTGATE m 4
//*************************************************************************



//*************************************************************************
//Create Current Pulse Generator for Current Clamp
create pulsegen /pulse
setfield ^ baselevel 0.0 level1 0.0 width1 50.0 delay1 10.0 delay2 999.0
//*************************************************************************
