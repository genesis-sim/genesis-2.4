//genesis

/* FILE INFORMATION
** 
** Tabchannel implementation of Hodgkin-Huxley squid Na and K channels
** This version uses SI (MKS) units and is equivalent to (but faster than)
** hh_chan.g, except for the function and channel names.
*/

// CONSTANTS
float EREST_ACT = -0.060
float ENA = 0.045
float EK = -0.090
/* Square meters */
float SOMA_A = 1e-9

/*************************************************************************

The function setupalpha uses the same form for the gate variables
X and Y as the vdep_gate, namely: (A+B*V)/(C+exp((V+D)/F))
The constants above are related to the hh_channel constants by:

EXPONENTIAL :
gate variables          in terms of hh-channel variables
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

*************************************************************************/
//========================================================================
//			Tabchan Na channel 
//========================================================================

function make_Na_hh_tchan
	str chanpath = "Na_hh_tchan"
        if ({argc} == 1)
           chanpath = {argv 1}
        end
	if ({exists {chanpath}})
		return
	end

	create tabchannel {chanpath}
		//	V
		//	S
		//	A
		//	S
		setfield ^ Ek {ENA} Gbar {1.2e3*SOMA_A} Ik 0 Gk 0  \
		    Xpower 3 Ypower 1 Zpower 0

	setupalpha {chanpath} X {0.1e6*(0.025 + EREST_ACT)} -0.1e6  \
	    -1.0 {-1.0*(0.025 + EREST_ACT)} -0.01  \
	    4e3 0.0 0.0 {-1.0*EREST_ACT} 18e-3

	setupalpha {chanpath} Y 70.0 0.0 0.0  \
	    {-1.0*EREST_ACT} 0.02 1.0e3 0.0 1.0  \
	    {-1.0*(0.030 + EREST_ACT)} -10.0e-3
end

//========================================================================
//			Tabchan version of K channel
//========================================================================
function make_K_hh_tchan
	str chanpath = "K_hh_tchan"
        if ({argc} == 1)
           chanpath = {argv 1}
        end
	if (({exists {chanpath}}))
		return
	end

	create tabchannel {chanpath}
		//	V
		//	S
		//	A
		//	S
		setfield ^ Ek {EK} Gbar {360.0*SOMA_A} Ik 0 Gk 0  \
		    Xpower 4 Ypower 0 Zpower 0

	setupalpha {chanpath} X {10e3*(0.01 + EREST_ACT)} -10.0e3  \
	    -1.0 {-1.0*(0.01 + EREST_ACT)} -0.01 125.0 0.0 0.0  \
	    {-1.0*EREST_ACT} 80.0e-3
end
