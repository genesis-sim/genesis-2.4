//genesis cell.g

/*                 functions which appear in this file

=============================================================================
	FUNCTION NAME		ARGUMENTS
=============================================================================
	makechannel		(compartment,channel,Ek,tau1,tau2,gmax)
	make_hhNa		(compartment,channel,activearea,ENa,Erest)
	make_hhK		(compartment,channel,activearea,EK,Erest)
	makecompartment		(path,l,d,Eleak)
	makeneuron		(path,soma_l,soma_d,dend_l,dend_d)
=============================================================================
*/



       //==========================================================================
       //                   declare constants
       //==========================================================================
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

 // different forms for the HH rate coefficients 
 int EXPONENTIAL = 1
 int SIGMOID = 2
 int LINOID = 3

 // default input level parameters
 float injwidth = 10
 float injdelay = 0
 float injcurrent = 2e-4

   //===========================================================================
   //                basic transmitter activated channel
   //===========================================================================
function makechannel(compartment, channel, Ek, tau1, tau2, gmax)
    str compartment
    str channel
    // mV
    float Ek
    // msec
    float tau1, tau2
    // mS
    float gmax

    create synchan2 {compartment}/{channel}
    setfield ^ Ek {Ek} tau1 {tau1} tau2 {tau2} gmax {gmax}
    link_channel2 {compartment}/{channel} {compartment}
end

//===========================================================================
//                           HH NA channel
//===========================================================================
function make_hhNa(compartment, channel, activearea, ENa, Erest)
    str compartment
    str channel
    // cm^2
    float activearea
    // mV
    float ENa, Erest

    create hh_channel {compartment}/{channel}
    // mV
    // mmhos/cm^2
    // 1/mV-msec
    // mV
    // mV
    // 1/msec
    // mV
    // mV
    // 1/msec
    // mV
    // mV
    // 1/msec
    // mV
    // mV
    setfield {compartment}/{channel} Ek {ENa} Gbar {120.0*activearea}  \
        Xpower 3.0 Ypower 1.0 X_alpha_FORM {LINOID} X_alpha_A -0.1  \
        X_alpha_B -10.0 X_alpha_V0 {25.0 + Erest}  \
        X_beta_FORM {EXPONENTIAL} X_beta_A 4.0 X_beta_B -18.0  \
        X_beta_V0 {0.0 + Erest} Y_alpha_FORM {EXPONENTIAL}  \
        Y_alpha_A 0.07 Y_alpha_B -20.0 Y_alpha_V0 {0.0 + Erest}  \
        Y_beta_FORM {SIGMOID} Y_beta_A 1.0 Y_beta_B -10.0  \
        Y_beta_V0 {30.0 + Erest}
    link_channel2 {compartment}/{channel} {compartment}
end

//==========================================================================
//                          HH K channel
//==========================================================================
function make_hhK(compartment, channel, activearea, EK, Erest)
    str compartment
    str channel
    float activearea
    float EK, Erest

    create hh_channel {compartment}/{channel}
    // mV
    // mmhos/cm^2 
    // 1/mV-msec
    // mV
    // mV
    // 1/msec
    // mV
    // mV
    setfield ^ Ek {EK} Gbar {36.0*activearea} Xpower 4.0 Ypower 0.0  \
        X_alpha_FORM {LINOID} X_alpha_A -0.01 X_alpha_B -10.0  \
        X_alpha_V0 {10.0 + Erest} X_beta_FORM {EXPONENTIAL}  \
        X_beta_A 0.125 X_beta_B -80.0 X_beta_V0 {0.0 + Erest}
    link_channel2 {compartment}/{channel} {compartment}
end


//===========================================================================
//                       create compartment
//===========================================================================
function makecompartment(path, l, d, Erest)
    str path
    float l, d
    float Erest
    float area = l*PI*d
    float xarea = PI*d*d/4
    // Kohm-cm^2
    float rm = 10.0
    // uF/cm^2
    float cm = 1.0
    // Kohm-cm
    float ra = 0.05

    create compartment {path}
    // mV
    // Kohm
    // uF
    // Kohm
    setfield {path} Em {Erest} Rm {rm/area} Cm {cm*area} Ra {ra*l/xarea}
end


//===========================================================================
//                   function to create neuron
//===========================================================================
function makeneuron(path, soma_l, soma_d, dend_l, dend_d)
    str path
    float soma_l, soma_d
    float dend_l, dend_d

    // 100% of the soma active
    float active_area = soma_l*PI*soma_d*1.0

    create neutral {path}
    pushe {path}
    //=============================================
    //                   cell body
    //=============================================
    makecompartment soma {soma_l} {soma_d} {Eleak}
    position soma I I R{-soma_l/2.0}

    //=============================================
    //          voltage dependent conductances
    //=============================================
    make_hhNa soma hh_Na {active_area} {ENa} {EREST}
    make_hhK soma hh_K {active_area} {EK} {EREST}

    //=============================================
    //                   dendrite
    //=============================================
    makecompartment soma/dend {dend_l} {dend_d} {Eleak}
    position soma/dend I I R{-dend_l/2.0}
    link_compartment soma/dend soma

    //=============================================
    //    synaptic conductances on the dendrite
    //=============================================
    makechannel soma/dend Na_channel {ENa} 3.0 3.0 {GNa}
    makechannel soma/dend K_channel {EK} 10.0 10.0 {GK}

    //=============================================
    //                 spike detector
    //=============================================
    create spikegen spike
    setfield spike thresh 0 abs_refract 10 output_amp 1
    /* use the soma membrane potential to drive the spike detector */
    addmsg soma spike INPUT Vm

    //=============================================
    //              injection pulse
    //=============================================
    create pulsegen soma/injectpulse
    create diffamp soma/injectpulse/injcurr
    setfield soma/injectpulse/injcurr saturation 1 gain {injcurrent}
    addmsg soma/injectpulse soma/injectpulse/injcurr PLUS output
    addmsg soma/injectpulse/injcurr soma INJECT output

    create pulsegen soma/dend/injectpulse
    create diffamp soma/dend/injectpulse/injcurr
    setfield soma/dend/injectpulse/injcurr saturation 1  \
        gain {injcurrent}
    addmsg soma/dend/injectpulse soma/dend/injectpulse/injcurr PLUS  \
        output
    addmsg soma/injectpulse/injcurr soma/dend INJECT output

    pope
end
