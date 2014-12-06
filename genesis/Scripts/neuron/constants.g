//genesis
// constants.g
// initialize the random # generator
randseed 

float PI = 3.14159

// channel equilibrium potentials	mV
// nominal rest potential used as a
float EREST_ACT = -70
					// reference potential for H-H channels
// actual resting membrane potential
float EREST = -70

// +55 mV
float ENa = 125.0 + EREST_ACT
// -80 mV
float EK = -10.0 + EREST_ACT
// -10 mv ( Na-K channel)
float EEx = 60.0 + EREST_ACT
// -80 mV ( K channel)
float EInh = -10.0 + EREST_ACT

//  leakage potential used in place of EREST in soma
// -58.3 mV
float Eleak = 11.7 + EREST_ACT

// peak channel conductance	mS/synapse
// Excitatory (Na-K channel)
float GEx = 100e-9
// Inhibitory (K channel)
float GInh = 100e-9

// This notation should be changed to RM, CM, RA
// Kohm-cm^2
float rm = 5.0
// uF/cm^2
float cm = 1.0
// Kohm-cm
float ra = 0.025

// compartment dimensions (cm.)
float soma_l = 30e-4
float soma_d = 30e-4
float dend_l = 100e-4
float dend_d = 2e-4
