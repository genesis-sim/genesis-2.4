//genesis 2.0
// This is the old-style "prototypes" version, with 2.0 syntax

/* FILE INFORMATION
** hh_channel implementation of squid giant axon voltage-dependent
** channels, according to :
** A.L.Hodgkin and A.F.Huxley, J.Physiol(Lond) 117, pp 500-544 (1952)
**
** This file depends on functions and constants defined in library.g
*/

// CONSTANTS
float EREST_ACT = -0.060 /* granule cell resting potl */
float ENA = 0.045
float EK = -0.090
float SOMA_A = 1e-9		/* Square meters */

int EXPONENTIAL =   1
int SIGMOID     =   2
int LINOID      =   3

/******************************************************************************
Some conventions in using the HH_CHANNELS and the VDEP_GATES

HH_CONVENTIONS
==============
Activation state variable is called x for all channels
Inactivation state variable is called y for all channels
In the traditional hh notations: x=m, y=h for Na channel; x=n for K_channel

There are three functional forms for alpha and beta for each state variable:
FORM 1: alpha(v) = A exp((v-V0)/B)                  (EXPONENTIAL)
FORM 2: alpha(v) = A / (exp((v-V0)/B) + 1)          (SIGMOID)
FORM 3: alpha(v) = A (v-V0) / (exp((v-V0)/B) - 1)   (LINOID)
The same functional forms are used for beta.
In the simulator, the FORM, A, B and V0 are designated by:
X_alpha_FORM, X_alpha_A, X_alpha_B, X_alpha_V0  alpha function for state var x
X_beta_FORM,  X_beta_A,  X_beta_B,  X_beta_V0   beta  function for state var x
Y_alpha_FORM, Y_alpha_A, Y_alpha_B, Y_alpha_V0  alpha function for state var y
Y_beta_FORM,  Y_beta_A,  Y_beta_B,  Y_beta_V0   beta  function for state var y

The conductance is calculated as g = Gbar*x^Xpower * y^Ypower
For a squid axon Na channel: Xpower = 3, Ypower = 1 (m^3 h)
                 K  channel: Xpower = 4, Ypower = 0 (n^4)

These are linked to the soma by two messages :
addmsg /soma/hh_channel /soma CHANNEL Gk Ek
addmsg /soma /soma/hh_channel VOLTAGE Vm

----------------------------------------------------------------------

For the VDEP Gates, the form of each gate is
alpha = (A+B*V)/(C+exp((V+D)/F))
This is related to the above forms as follows :
EXPONENTIAL :
gate variables		Value of gate variable in terms of hh-channel variables
A			A
B			0
C			0
D			-V0
F			-B

SIGMOID :
Gate			in terms of hh-ch variables
A			A
B			0
C			1
D			-V0
F			B

LINOID :
A			-A * V0
B			A
C			-1
D			-V0
F			B


These are linked to the soma and the channel proper as follows :
addmsg /soma/channel /soma CHANNEL Gk Ek
addmsg /soma /soma/channel VOLTAGE Vm

addmsg /soma/channel/m /soma/channel {gate_type} m {Power}
(eg, addmsg Na_mitral/m      Na_mitral MULTGATE    m 3

addmsg /soma /soma/channel/gate EREST Vm

******************************************************************************/
//========================================================================
//  Original Hodgkin-Huxley squid parameters, implemented as hh_channel elements
//========================================================================

//========================================================================
//                        ACTIVE SQUID NA CHANNEL 
//      A.L.Hodgkin and A.F.Huxley, J.Physiol(Lond) 117, pp 500-544 (1952)
//========================================================================
function make_Na_squid_hh
	if ({exists Na_squid_hh})
		return
	end
	
	create		hh_channel	Na_squid_hh
	setfield Na_squid_hh \
		Ek 		{ENA} \			// V
		Gbar		{ 1.2e3 * SOMA_A } \		// S
		Xpower		3.0 \
		Ypower		1.0 \   
		X_alpha_FORM	{LINOID} \
		X_alpha_A	-0.1e6 \			// 1/V-sec
		X_alpha_B	-0.010 \			// V
		X_alpha_V0	{ 0.025 + EREST_ACT } \		// V
		X_beta_FORM	{EXPONENTIAL} \
		X_beta_A	4.0e3 \				// 1/sec
		X_beta_B	-18.0e-3 \			// V
		X_beta_V0	{ 0.0 + EREST_ACT } \		// V
		Y_alpha_FORM	{EXPONENTIAL} \
		Y_alpha_A	70.0 \				// 1/sec
		Y_alpha_B	-20.0e-3 \			// V
		Y_alpha_V0	{ 0.0 + EREST_ACT } \		// V
		Y_beta_FORM	{SIGMOID} \
		Y_beta_A	1.0e3 \				// 1/sec
		Y_beta_B	-10.0e-3 \			// V
		Y_beta_V0	{ 30.0e-3 + EREST_ACT }		// V
end

//========================================================================
//                        ACTIVE K CHANNEL - SQUID
//      A.L.Hodgkin and A.F.Huxley, J.Physiol(Lond) 117, pp 500-544 (1952)
//========================================================================
function make_K_squid_hh
	if ({exists K_squid_hh})
		return
	end
	
	create		hh_channel	K_squid_hh
    	setfield K_squid_hh \
		Ek 		{EK} \				// V
		Gbar		{360.0*SOMA_A} \		// S
		Xpower		4.0 \
		Ypower		0.0 \   
		X_alpha_FORM	{LINOID} \
		X_alpha_A	-10.0e3 \			// 1/V-sec
		X_alpha_B	-10.0e-3 \			// V
		X_alpha_V0	{10.0e-3+EREST_ACT} \		// V
		X_beta_FORM	{EXPONENTIAL} \
		X_beta_A	125.0 \				// 1/sec
		X_beta_B	-80.0e-3 \			// V
		X_beta_V0	{0.0+EREST_ACT}  		// V
end

//========================================================================
