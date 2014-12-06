//genesis
// initialize the random # generator
randseed 

float PI = 3.14159

// channel equilibrium potentials	mV
// resting membrane potential
float EREST = -70
float ENa = 115.0 + EREST
float EK = -12.0 + EREST
// leakage potential
float Eleak = 10.6 + EREST

// peak channel conductance	mS/synapse
float GNa = 80e-9
float GK = 40e-9
