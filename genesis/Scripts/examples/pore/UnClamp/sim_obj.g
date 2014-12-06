//genesis
//*************************************************************************
//Adam Strassberg
//March 15, 1992

//Create Core Simulation Objects
//*************************************************************************


//*************************************************************************
//Current Pulse Input to a Patch of Membrane
//with Single Sodium and Potassium channels undergoing
//standard Hodgkin-Huxley channel kinetics

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
//Create Channel Objects
//Uses specially designed objects knewchanel and nanewchannel
create Kpores /patch/K
setfield ^ alpha_A 0.1 alpha_B -0.01 alpha_C -1.0 alpha_D -10.0  \
    alpha_F -10.0 beta_A 0.125 beta_B 0.0 beta_C 0.0 beta_D 0.0  \
    beta_F 80.0 Ek {VK_squid}
create Napores /patch/Na
setfield ^ malpha_A 2.5 malpha_B -0.1 malpha_C -1.0 malpha_D -25.0  \
    malpha_F -10.0 mbeta_A 4.0 mbeta_B 0.0 mbeta_C 0.0 mbeta_D 0.0  \
    mbeta_F 18.0 halpha_A 0.07 halpha_B 0.0 halpha_C 0.0 halpha_D 0.0  \
    halpha_F 20.0 hbeta_A 1.0 hbeta_B 0.0 hbeta_C 1.0 hbeta_D -30.0  \
    hbeta_F -10.0 Ek {VNa_squid}
//*************************************************************************


//*************************************************************************
//Create Current Pulse Generator for Current Clamp
create pulsegen /pulse
setfield ^ baselevel 0.0 level1 0.0 width1 50.0 delay1 10.0 delay2 999.0
//*************************************************************************
