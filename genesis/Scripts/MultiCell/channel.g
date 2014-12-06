//genesis
/* 
	functions defined in this script
=============================================================================
	FUNCTION NAME		ARGUMENTS
=============================================================================
	makechannel		(compartment,channel,Ek,tau1,tau2,gmax)
	make_hhNa		(compartment,channel,activearea,ENa,Erest)
	make_hhK		(compartment,channel,activearea,EK,Erest)
=============================================================================
*/

// different forms for the HH rate coefficients 
int EXPONENTIAL = 1
int SIGMOID = 2
int LINOID = 3

//=================================================
//       BASIC TRANSMITTER ACTIVATED CHANNEL
//=================================================
function makechannel(compartment, channel, Ek, tau1, tau2, gmax)
    str compartment
    str channel
    // mV
    float Ek
    // msec
    float tau1, tau2
    // mS
    float gmax

    create channelC2 {compartment}/{channel}
    setfield ^ Ek {Ek} tau1 {tau1} tau2 {tau2} gmax {gmax}
    link_channel2 {compartment}/{channel} {compartment}
end

//=================================================
//                 HH NA CHANNEL
//=================================================
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

//=================================================
//                  HH K CHANNEL
//=================================================
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
