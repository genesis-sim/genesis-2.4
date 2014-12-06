//genesis
/******************************************************************************
** **
** PROTODEFS.G : prototype definition file, neuron builder kit.  **
** **
** By U.S.Bhalla, June 1990 **
** **
** This file defines a library of prototype neuronal components.  **
** It depends on the constants defined in the file 'constants.g' **
** The values set up here are derived from a variety of sources, **
** if no reference is cited the values are my informed guesses.  **
** **
** The library is organised into a number of sections, (which are **
** implemented as children of suitably named 'neutral' elements).  **
** This orgranisation is based on sources of data, on type of neuronal **
** module being specified, and whatever else took my fancy. To avoid **
** confusion I suggest that your additions to the library be placed in **
** new sections in a similar manner, i.e. children of a neutral with **
** a suitable name. We would be very glad to incorporate your additions **
** to the prototype library into the general distribution, so if you **
** feel like sharing your modules, send them over.  **
** **
** All units are in SI (Meters Kilograms Seconds Amps). While this **
** does result in lots of powers of 10 in the parameters, it prevents **
** the chaos of interconversions when relating parameters of different **
** units.  **
** **
** **
** **
******************************************************************************/

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
(eg, addmsg Na_mitral/m      Na_mitral MULTGATE    m 3_

addmsg /soma /soma/channel/gate EREST Vm

*****************************************************************************/

create neutral /library
// We dont want the library to try to calculate anything,
// so we disable it
disable /library

// To ensure that all subsequent elements are made in the library
pushe /library

//========================================================================
// SYNAPTIC CHANNELS (Values guessed by Matt) (channelC2 from oldconn)
//========================================================================

create channelC2 glu
    setfield glu Ek {ENA} tau1 {2.0e-3} tau2 {2.0e-3}  \
        gmax {DISTAL_GMAX_NA}

create channelC2 GABA
    setfield GABA Ek {EK} tau1 {20.0e-3} tau2 {20.0e-3} gmax {GMAX_K}

create channelC2 inh_channel
    setfield inh_channel Ek {EK} tau1 {100.0e-3} tau2 {100.0e-3}  \
        gmax {GMAX_K}

    //========================================================================
    //                               COMPARTMENTS
    //========================================================================

create compartment compartment
setfield compartment Cm {{CM}*{SOMA_A}} Ra {{RA}*{SOMA_L}/{SOMA_XA}}  \
    Em {EREST_ACT} Rm {{RM}/{SOMA_A}} inject 0.0

create symcompartment symcompartment

setfield compartment Cm {{CM}*{SOMA_A}} Ra {{RA}*{SOMA_L}/{SOMA_XA}}  \
    Em {EREST_ACT} Rm {{RM}/{SOMA_A}} inject 0.0


//========================================================================
//  Original Hodgkin-Huxley squid parameters, implemented as hh_channel elements
//========================================================================

//========================================================================
//                        ACTIVE SQUID NA CHANNEL 
//      A.L.Hodgkin and A.F.Huxley, J.Physiol(Lond) 117, pp 500-544 (1952)
//========================================================================
create hh_channel HH_Na_channel
setfield HH_Na_channel Ek {ENA_ACT} Gbar {1.2e3*{SOMA_A}} Xpower 3.0  \
    Ypower 1.0 X_alpha_FORM {LINOID} X_alpha_A -0.1e6 X_alpha_B -0.010  \
    X_alpha_V0 {0.025 + {EREST_ACT}} X_beta_FORM {EXPONENTIAL}  \
    X_beta_A 4.0e3 X_beta_B -18.0e-3 X_beta_V0 {0.0 + {EREST_ACT}}  \
    Y_alpha_FORM {EXPONENTIAL} Y_alpha_A 70.0 Y_alpha_B -20.0e-3  \
    Y_alpha_V0 {0.0 + {EREST_ACT}} Y_beta_FORM {SIGMOID} Y_beta_A 1.0e3  \
    Y_beta_B -10.0e-3 Y_beta_V0 {30.0e-3 + {EREST_ACT}}

//========================================================================
//                        ACTIVE K CHANNEL - SQUID
//========================================================================
create hh_channel HH_K_channel
    setfield HH_K_channel Ek {EK} Gbar {360.0*{SOMA_A}} Xpower 4.0  \
        Ypower 0.0 X_alpha_FORM {LINOID} X_alpha_A -10.0e3  \
        X_alpha_B -10.0e-3 X_alpha_V0 {10.0e-3 + {EREST_ACT}}  \
        X_beta_FORM {EXPONENTIAL} X_beta_A 125.0 X_beta_B -80.0e-3  \
        X_beta_V0 {0.0 + {EREST_ACT}}



    //========================================================================
    // Parameters used by my mitral and granule cell models for the olfactory
    // bulb model. They are basically modified Traub params. These are also
    // implemented as hh_channels.
    //========================================================================

    //========================================================================
    //                        ACTIVE NA CHANNEL - MITRAL
    //========================================================================
create hh_channel MHH_Na_channel

setfield MHH_Na_channel Ek {ENA_ACT} Gbar {1.2e3*{SOMA_A}} Xpower 3.0  \
    Ypower 1.0 X_alpha_FORM {LINOID} X_alpha_A -0.32e6 X_alpha_B -0.004  \
    X_alpha_V0 {0.013 + {EREST_ACT}} X_beta_FORM {LINOID}  \
    X_beta_A 0.28e6 X_beta_B 5.0e-3 X_beta_V0 {40.0e-3 + {EREST_ACT}}  \
    Y_alpha_FORM {EXPONENTIAL} Y_alpha_A 128.0 Y_alpha_B -18.0e-3  \
    Y_alpha_V0 {0.017 + {EREST_ACT}} Y_beta_FORM {SIGMOID}  \
    Y_beta_A 4.0e3 Y_beta_B -5.0e-3 Y_beta_V0 {40.0e-3 + {EREST_ACT}}

//========================================================================
//                        ACTIVE NA CHANNEL - GRANULE CELL
//========================================================================
create hh_channel GHH_Na_channel

setfield GHH_Na_channel Ek {ENA_ACT} Gbar {1.2e3*{SOMA_A}} Xpower 3.0  \
    Ypower 1.0 X_alpha_FORM {LINOID} X_alpha_A -0.32e6 X_alpha_B -0.004  \
    X_alpha_V0 {0.013 + {EREST_ACT}} X_beta_FORM {LINOID}  \
    X_beta_A 0.28e6 X_beta_B 5.0e-3 X_beta_V0 {40.0e-3 + {EREST_ACT}}  \
    Y_alpha_FORM {EXPONENTIAL} Y_alpha_A 128.0 Y_alpha_B -18.0e-3  \
    Y_alpha_V0 {0.017 + {EREST_ACT}} Y_beta_FORM {SIGMOID}  \
    Y_beta_A 4.0e3 Y_beta_B -5.0e-3 Y_beta_V0 {40.0e-3 + {EREST_ACT}}

//========================================================================
//			ACTIVE CA CHANNEL - MITRAL
//========================================================================
create hh_channel MHH_Ca_channel

setfield MHH_Ca_channel \
        Ek              {ENA_ACT} \                     // V
        Gbar            { 2.0e3 * SOMA_A } \    // S
        Xpower          5.0 \
        Ypower          0.0 \    // Dont want to deal with Ca conc yet.
        X_alpha_FORM    {LINOID} \
        X_alpha_A       -0.04e6 \                               // 1/V-sec
        X_alpha_B       -0.010 \                                // V
        X_alpha_V0      { 0.060 + EREST_ACT } \ // V
        X_beta_FORM     {LINOID} \
        X_beta_A        5.0e3 \                                 // 1/sec
        X_beta_B        10.0e-3 \                               // V
        X_beta_V0       { 0.045 + EREST_ACT } \ // V
        Y_alpha_FORM    {EXPONENTIAL} \
        Y_alpha_A       5e3 \                   // 1/sec
        Y_alpha_B 20.0e6 \ // V : we want to make Y a const here
        Y_alpha_V0      { EREST_ACT } \         // V : Doesnt matter.
        Y_beta_FORM {SIGMOID} \ // Messy : depends on Ca conc.
        Y_beta_A        1.0e3 \                                 // 1/sec
        Y_beta_B        -10.0e-3 \                              // V
        Y_beta_V0       { 30.0e-3 + EREST_ACT } // V

//========================================================================
//                        ACTIVE K CHANNEL - MITRAL
//========================================================================
create hh_channel MHH_K_channel
    setfield MHH_K_channel \
        Ek              {EK} \                                  // V
        Gbar            {360.0*SOMA_A} \                        // S
        Xpower          4.0 \
        Ypower          0.0 \   
        X_alpha_FORM    {LINOID} \
        X_alpha_A       -32.0e3 \                               // 1/V-sec
        X_alpha_B       -5.0e-3 \                               // V
        X_alpha_V0      {0.015+EREST_ACT} \                     // V
        X_beta_FORM     {EXPONENTIAL} \
        X_beta_A        500.0 \                                 // 1/sec
        X_beta_B        -40.0e-3 \                              // V
        X_beta_V0       {0.010+EREST_ACT} \                     // V
        \ // this part cant work since Traub uses 2 exps to get the Y. So I set
        \ // the Ypower to zero for now and ignore it. I need Vclamp data !
        Y_alpha_FORM    {EXPONENTIAL} \
        Y_alpha_A       128.0 \                                 // 1/sec
        Y_alpha_B       -18.0e-3 \                              // V
        Y_alpha_V0      { 0.017 + EREST_ACT } \                 // V
        Y_beta_FORM     {SIGMOID} \
        Y_beta_A        4.0e3 \                                 // 1/sec
          Y_beta_B        -5.0e-3 \                               // V
        Y_beta_V0       { 40.0e-3 + EREST_ACT }                 // V

    //========================================================================
    //                        ACTIVE K CHANNEL - GRANULE CELL
    //========================================================================

create hh_channel GHH_K_channel
    setfield GHH_K_channel \
        Ek              {EK} \                                  // V
        Gbar            {360.0*SOMA_A} \                        // S
        Xpower          4.0 \
        Ypower          0.0 \   
        X_alpha_FORM    {LINOID} \
        X_alpha_A       -32.0e3 \                               // 1/V-sec
        X_alpha_B       -5.0e-3 \                               // V
        X_alpha_V0      {0.015+EREST_ACT} \                     // V
        X_beta_FORM     {EXPONENTIAL} \
        X_beta_A        500.0 \                                 // 1/sec
        X_beta_B        -40.0e-3 \                              // V
        X_beta_V0       {0.010+EREST_ACT} \                     // V
        \ // this part cant work since Traub uses 2 exps to get the Y. So I set
        \ // the Ypower to zero for now and ignore it. I need Vclamp data !
        Y_alpha_FORM    {EXPONENTIAL} \
        Y_alpha_A       128.0 \                                 // 1/sec
        Y_alpha_B       -18.0e-3 \                              // V
        Y_alpha_V0      { 0.017 + EREST_ACT } \                 // V
        Y_beta_FORM     {SIGMOID} \
        Y_beta_A        4.0e3 \                                 // 1/sec
        Y_beta_B        -5.0e-3 \                               // V
        Y_beta_V0       { 40.0e-3 + EREST_ACT }                 // V

    //========================================================================
    //			Miscellaneous things
    //========================================================================


    //========================================================================
    //                        SPIKE DETECTOR - for use with channelC2
    //========================================================================
create spike spike
    setfield spike thresh -40e-3 abs_refract {10e-3} output_amp 1

//========================================================================
//                        Dendro-dendritic synapse 
//========================================================================
create sigmoid sigmoid
	setfield sigmoid \
                amplitude       0.01    \               // Activation units ?
                thresh          -0.05   \               // V
                gain            1                       // dunno
//========================================================================
create graded graded
	setfield graded \
                baseline        -0.05   \               // V
                rectify         1       \               // Boolean
                tmin            0.001                   // Seconds

//========================================================================
create axonlink axonlink
//========================================================================
create linear linear
	setfield linear \
                tmin    0.001   \               // seconds
                thresh          -0.05   \               // V
                gain            1                       // dunno

//========================================================================
create diffamp diffamp
	setfield diffamp plus 0.0 minus -0.07 gain 1.0 saturation 0.1

//========================================================================
	//		Voltage clamp circuit (from Mark Nelson, SQUID demo)
	//========================================================================
create diffamp Vclamp
setfield ^ 	saturation 999.0 \ // unitless I hope
		gain 0.002         // 1/R  from the lowpass filter input

create RC Vclamp/lowpass
setfield ^ R 500.0 \ // ohms
	   C 0.1e-6  // Farads; for a tau of 50 us.


create PID Vclamp/PID
setfield   ^       gain    1e-6    \       // 10/Rinput of cell
                tau_i   20e-6   \       // seconds
                tau_d   5e-6    \       // seconds
                saturation      999.0   // unitless I hope
/*
addmsg dummy Vclamp/lowpass	INJECT	x	// The voltage to clamp at
*/
addmsg Vclamp/lowpass Vclamp PLUS state
addmsg Vclamp Vclamp/PID CMD output
/*
addmsg {dend} Vclamp/PID	SNS	Vm	// The fb from the dend
addmsg Vclamp/PID {dend}	INJECT	output	// The current into the dend
*/
//========================================================================
/*
** dC/dt = B*I_Ca - C/tau
** Ca = Ca_base + C
*/
create Ca_concen conc
// sec
// molarity
setfield conc \
        tau             {SOMA_D/CA_DIFF} \                      // sec
        Ca_base {BASE_CA_CONC} \                                // molarity
        Ca \
        B \
        C \
        activation
// Conc in moles/cubic meter, which happens to work out to mmol/L
// Number of moles of Ca per coulomb of Ca++ ions is 5.2e-6

//========================================================================

//========================================================================
//	vdep_gate versions of the mitral cell channels
//========================================================================

//========================================================================
//			Na Mitral cell channel
//========================================================================

create vdep_channel Na_mitral
	setfield ^ Ek {ENA_ACT} gbar {1.2e3*{SOMA_A}} Ik 0 Gk 0

create vdep_gate Na_mitral/m
	setfield ^ alpha_A {320e3*(0.013 + {EREST_ACT})} alpha_B -320e3  \
	    alpha_C -1.0 alpha_D {-1.0*(0.013 + {EREST_ACT})}  \
	    alpha_F -0.004 beta_A {-280e3*(0.040 + {EREST_ACT})}  \
	    beta_B 280e3 beta_C -1.0 beta_D {-1.0*(0.040 + {EREST_ACT})} \
	     beta_F 5.0e-3 instantaneous 0

create vdep_gate Na_mitral/h
	setfield ^ alpha_A 128.0 alpha_B 0.0 alpha_C 0.0  \
	    alpha_D {-1.0*(0.017 + {EREST_ACT})} alpha_F 0.018  \
	    beta_A 4.0e3 beta_B 0.0 beta_C 1.0  \
	    beta_D {-1.0*(0.040 + {EREST_ACT})} beta_F -5.0e-3  \
	    instantaneous 0

addmsg Na_mitral/m Na_mitral MULTGATE m 3
addmsg Na_mitral/h Na_mitral MULTGATE m 1

//========================================================================
//			K Mitral cell channel
//========================================================================

create vdep_channel K_mitral
	setfield ^ Ek {EK} gbar {360.0*{SOMA_A}} Ik 0 Gk 0

create vdep_gate K_mitral/n
	setfield ^ alpha_A {32.0e3*(0.015 + {EREST_ACT})}  \
	    alpha_B -32.0e3 alpha_C -1.0  \
	    alpha_D {-1.0*(0.015 + {EREST_ACT})} alpha_F -0.005  \
	    beta_A 500.0 beta_B 0.0 beta_C 0.0  \
	    beta_D {-1.0*(0.010 + {EREST_ACT})} beta_F 40.0e-3  \
	    instantaneous 0

create vdep_gate K_mitral/y1
	setfield ^ alpha_A 28.0 alpha_B 0.0 alpha_C 0.0  \
	    alpha_D {-1.0*(0.015 + {EREST_ACT})} alpha_F -0.015  \
	    beta_A 400.0 beta_B 0.0 beta_C 1.0  \
	    beta_D {-1.0*(0.040 + {EREST_ACT})} beta_F -0.01  \
	    instantaneous 0

create vdep_gate K_mitral/y2
	setfield ^ alpha_A 2000.0 alpha_B 0.0 alpha_C 1.0  \
	    alpha_D {-1.0*(0.085 + {EREST_ACT})} alpha_F -0.010  \
	    beta_A 400.0 beta_B 0.0 beta_C 1.0  \
	    beta_D {-1.0*(0.040 + {EREST_ACT})} beta_F -0.01  \
	    instantaneous 0

addmsg K_mitral/n K_mitral MULTGATE m 4
/*
addmsg K_mitral/y1	K_mitral	MULTGATE	m 1
addmsg K_mitral/y2	K_mitral	MULTGATE	m 1
*/


//========================================================================
// 	Tabulated versions of the Traub set of channels
// From : R.D.Traub, Neuroscience Vol 7 No 5 pp 1233-1242 (1982)
//========================================================================

//========================================================================
//			Tabulated Na Mitral cell channel 
//========================================================================

function setup_table3(gate, table, xdivs, xmin, xmax, A, B, C, D, F)
	str gate, table
	int xdivs
	float xmin, xmax, A, B, C, D, F

	int i
	float x, dx, y

	dx = xdivs
	dx = (xmax - xmin)/dx
	x = xmin

	for (i = 0; i <= (xdivs); i = i + 1)
		y = (A + B*x)/(C + ({exp {(x + D)/F}}))
		setfield {gate} {table}->table[{i}] {y}
		x = x + dx
	end
end

function setup_table2(gate, table, xdivs, xmin, xmax, A, B, C, D, F)
	str gate, table
	int xdivs
	float xmin, xmax, A, B, C, D, F

	if (xdivs <= 9)
		echo must have at least 9, preferably over 100 elements  \
		    in table
		return
	end
	call {gate} TABCREATE {table} {xdivs} {xmin} {xmax}
	setup_table3 {gate} {table} {xdivs} {xmin} {xmax} {A} {B} {C}  \
	    {D} {F}
end

function setup_table(gate, table, xdivs, A, B, C, D, F)
	str gate, table
	int xdivs
	float A, B, C, D, F

	setup_table2 {gate} {table} {xdivs} -0.1 0.1 {A} {B} {C} {D} {F}
end

create vdep_channel Na_t_mitral
	//	V
	//	S
	//	A
	//	S
	setfield ^ Ek {ENA_ACT} gbar {1.2e3*{SOMA_A}} Ik 0 Gk 0

create tabgate Na_t_mitral/m
setup_table Na_t_mitral/m alpha 100 {320e3*(0.013 + {EREST_ACT})} -320e3 \
     -1.0 {-1.0*(0.013 + {EREST_ACT})} -0.004
setup_table Na_t_mitral/m beta 100 {-280e3*(0.040 + {EREST_ACT})} 280e3  \
    -1.0 {-1.0*(0.040 + {EREST_ACT})} 5.0e-3

create tabgate Na_t_mitral/h
setup_table Na_t_mitral/h alpha 100 128.0 0.0 0.0  \
    {-1.0*(0.017 + {EREST_ACT})} 0.018
setup_table Na_t_mitral/h beta 100 4.0e3 0.0 1.0  \
    {-1.0*(0.040 + {EREST_ACT})} -5.0e-3

addmsg Na_t_mitral/m Na_t_mitral MULTGATE m 3
addmsg Na_t_mitral/h Na_t_mitral MULTGATE m 1

//========================================================================
//			Tabulated Ca Channel - mitral cell
//========================================================================


create vdep_channel Ca_t_mitral
	setfield ^ Ek {ECA_ACT} gbar {1.2e3*{SOMA_A}} Ik 0 Gk 0

create tabgate Ca_t_mitral/s
setup_table Ca_t_mitral/s alpha 100 {40e3*(0.060 + {EREST_ACT})} -40e3  \
    -1.0 {-1.0*(0.060 + {EREST_ACT})} -0.010
setup_table Ca_t_mitral/s beta 100 {-5e3*(0.045 + {EREST_ACT})} 5e3 -1.0 \
     {-1.0*(0.045 + {EREST_ACT})} 10.0e-3

create tabgate Ca_t_mitral/r
call Ca_t_mitral/r TABCREATE alpha 1 -100 100
setfield Ca_t_mitral/r alpha->table[0] 5.0
setfield Ca_t_mitral/r alpha->table[1] 5.0

setup_table2 Ca_t_mitral/r beta 1000 -1 100 {25.0*200.0} -25.0 -1.0  \
    -200.0 -20.0

create Ca_concen Ca_t_mitral/conc
setfield Ca_t_mitral/conc \
        tau             0.01    \                       // sec
        B               {5.2e-6/(SOMA_XA*SOMA_L)} \     // Current to conc
        Ca_base         0.0

addmsg Ca_t_mitral/s Ca_t_mitral MULTGATE m 5
addmsg Ca_t_mitral/r Ca_t_mitral MULTGATE m 1
addmsg Ca_t_mitral/conc Ca_t_mitral/r VOLTAGE Ca
addmsg Ca_t_mitral Ca_t_mitral/conc I_Ca Ik


//========================================================================
//			Tabulated K channel - Mitral cell
//========================================================================

create vdep_channel K_t_mitral
	setfield ^ Ek {EK} gbar {360.0*{SOMA_A}} Ik 0 Gk 0

create tabgate K_t_mitral/n
setup_table K_t_mitral/n alpha 100 {32e3*(0.015 + {EREST_ACT})} -32e3  \
    -1.0 {-1.0*(0.015 + {EREST_ACT})} -0.005
setup_table K_t_mitral/n beta 100 500.0 0.0 0.0  \
    {-1.0*(0.010 + {EREST_ACT})} 40.0e-3

create table K_t_mitral/ya2
call K_t_mitral/ya2 TABCREATE 100 -0.1 0.1
setup_table3 K_t_mitral/ya2 table 100 -0.1 0.1 2000 0 1  \
    {-1.0*(0.085 + {EREST_ACT})} -0.010
create tabgate K_t_mitral/y
setup_table K_t_mitral/y alpha 100 28 0 0 {-1.0*(0.015 + {EREST_ACT})}  \
    0.015
setup_table K_t_mitral/y beta 100 400 0 1 {-1.0*(0.040 + {EREST_ACT})}  \
    -0.010

addmsg K_t_mitral/n K_t_mitral MULTGATE m 4
addmsg K_t_mitral/y K_t_mitral MULTGATE m 1
addmsg K_t_mitral/ya2 K_t_mitral/y SUM_ALPHA output

//========================================================================
//			Tabulated Ca dependent K - channel.
//========================================================================

create vdep_channel Kca_t_mitral
	setfield ^ Ek {EK} gbar {360.0*{SOMA_A}} Ik 0 Gk 0

create table Kca_t_mitral/qv
call Kca_t_mitral/qv TABCREATE 100 -0.1 0.1
int i
float x, dx, y
x = -0.1
dx = 0.2/100.0
for (i = 0; i <= 100; i = i + 1)
	y = {exp {(x - {EREST})/0.027}}
	setfield Kca_t_mitral/qv table->table[{i}] {y}
	x = x + dx
end

create tabgate Kca_t_mitral/qca

setup_table2 Kca_t_mitral/qca alpha 1000 -1 100 {5.0*200.0} -5.0 -1.0  \
    -200.0 -20.0

call Kca_t_mitral/qca TABCREATE beta 1 -1 100
setfield Kca_t_mitral/qca beta->table[0] 2.0
setfield Kca_t_mitral/qca beta->table[1] 2.0

addmsg Kca_t_mitral/qv Kca_t_mitral/qca PRD_ALPHA output
addmsg Kca_t_mitral/qca Kca_t_mitral MULTGATE m 1

pope

//========================================================================
