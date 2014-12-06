//genesis
create neutral /library
disable /library

//========================================================================
//                              CHANNELS   
//========================================================================
create channelC2 /library/Na_channel
    // { 2.0 * SCALE } \	// msec
    //{ 2.0 * SCALE } 	// msec
    setfield /library/Na_channel Ek {EKA} tau1 {1.0*{SCALE}}  \
        tau2 {3.0*{SCALE}}
create channelC2 /library/K_channel
    //{ 2.0 * SCALE } \	// msec
    //{ 100.0 * SCALE } 	// msec
    setfield /library/K_channel Ek {EK} tau1 {10.0*{SCALE}}  \
        tau2 {100.0*{SCALE}}
create channelC2 /library/Cl_channel
    //{ 2.0 * SCALE } \	// msec
    //{ 18.0 * SCALE } 	// msec
    setfield /library/Cl_channel Ek {ECL} tau1 {1.0*{SCALE}}  \
        tau2 {7.0*{SCALE}}

    //========================================================================
    //                        ACTIVE NA CHANNEL
    //========================================================================
create hh_channel /library/HH_Na_channel
// mV
// mS
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
setfield /library/HH_Na_channel Ek {ENA} Gbar {120.0e-8*{PYR_SOMA_A}}  \
    Xpower 3.0 Ypower 1.0 X_alpha_FORM {LINOID} X_alpha_A -0.1  \
    X_alpha_B -10.0 X_alpha_V0 {25.0 + {PYR_EREST_ABS}}  \
    X_beta_FORM {EXPONENTIAL} X_beta_A 4.0 X_beta_B -18.0  \
    X_beta_V0 {0.0 + {PYR_EREST_ABS}} Y_alpha_FORM {EXPONENTIAL}  \
    Y_alpha_A 0.07 Y_alpha_B -20.0 Y_alpha_V0 {0.0 + {PYR_EREST_ABS}}  \
    Y_beta_FORM {SIGMOID} Y_beta_A 1.0 Y_beta_B -10.0  \
    Y_beta_V0 {30.0 + {PYR_EREST_ABS}}

//========================================================================
//                        ACTIVE K CHANNEL
//========================================================================
create hh_channel /library/HH_K_channel
    // mV
    // mS
    // 1/mV-msec
    // mV
    // mV
    // 1/msec
    // mV
    // mV
    setfield /library/HH_K_channel Ek {EK} Gbar {36.0e-8*{PYR_SOMA_A}}  \
        Xpower 4.0 Ypower 0.0 X_alpha_FORM {LINOID} X_alpha_A -0.01  \
        X_alpha_B -10.0 X_alpha_V0 {10.0 + {PYR_EREST_ABS}}  \
        X_beta_FORM {EXPONENTIAL} X_beta_A 0.125 X_beta_B -80.0  \
        X_beta_V0 {0.0 + {PYR_EREST_ABS}}

    //========================================================================
    //                        SPIKE DETECTOR
    //========================================================================
create spike /library/spike
    // mV
    // msec
    setfield /library/spike thresh -40 abs_refract {10*{SCALE}}  \
        output_amp 1

