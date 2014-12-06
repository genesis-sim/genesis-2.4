// genesis - squid_consts.g


//==============================
// GLOBAL VARIABLE DEFINITIONS
//==============================


float PI = 3.14159
float R = 8.314
float F = 9.65e4
//temp used in fitting HH rate constants
float T = 273.15 + 6.3


// Ion concentrations inside and outside squid axons 
// (from KNM, p 119)

// float K_in_squid	=	400	// mM
// mM
float K_out_squid = 10
// float Na_in_squid	=	 50	// mM
// mM
float Na_out_squid = 460
// mM
float Cl_in_squid = 100
// mM
float Cl_out_squid = 540
// mM
float Ca_in_squid = 0.0001
// mM
float Ca_out_squid = 10

// Modified internal concentrations used to give HH values of equilibrium
// constants from the Nernst equation at 6.3 deg C.
// HH 1952a, p. 455
float Na_in_squid = 70.96
// not calculated by HH
float K_in_squid = 301.4


// Specific membrane capacitance and conductances, 
// (from HH, J. Physiol. 117, 500-544 (1952) - Table 3) 

// uF/cm^2
float Cm_squid = 1
// mmho/cm^2
float GNa_squid = 120
// mmho/cm^2
float GK_squid = 36
// mmho/cm^2
float Gleak_squid = 0.3

// Specific axial resistance (KNM, p 171)
// kohm-cm
float Ra_squid = 0.030


// Vvoltages calculated from Nernst equation
// (added by Ed Vigmond 11/10/94, modified by D. Beeman 12/15/94)

// HH defined the resting potential to be 0.  If you prefer -70 mV,
// you may change EREST_ACT to -70.0
float EREST_ACT = 0.0

// mV
float Vrest_squid = EREST_ACT
float  \
    VK_squid = ((R*T/F)*1000*{log {K_out_squid/K_in_squid}} + 70.0 + EREST_ACT)
float  \
    VNa_squid = ((R*T/F)*1000*{log {Na_out_squid/Na_in_squid}} + 70.0 + EREST_ACT)

// This value of Vleak balances the resting current for the
// reversal potentials calculated using the default concentrations
// ([Na]_e = 460, [Na]_i = 70.96, [K]_e = 10, [K]_i = 301.4 mM ).
// When [K] is increased, spontaneous oscillations will now occur 
// mV
float Vleak_squid = 10.613 + EREST_ACT
