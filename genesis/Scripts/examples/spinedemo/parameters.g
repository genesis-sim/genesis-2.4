//genesis
/* Copyright E. De Schutter BBF-UIA Oct 1998 */
/* Contains constants for SPINEDEMO.g */

/* Constants controlling the simulation */
    float PI = 3.14159
    /* Cable parameters */
    float CM = 0.01
    float RM = 1.20
    float RA = 2.00
    float EREST_ACT = -0.070

    /* Sizes */
    /* Spine: realistic */
    float head_dia = 0.5e-6
    float head_len = 0.5e-6
    float neck_dia = 0.1e-6
    float neck_len = 0.7e-6
    /* Rest of neuron: made very small so as to increase imput impedance */
    float dend_dia = 2.0e-6
    float dend_len = 0.5e-6
    float ddend_dia = 0.5e-6
    float ddend_len = 5.0e-6
    float apical_dia = 2.0e-6
    float apical_len = 10.0e-6
    float soma_dia = 10.0e-6
    /* Shell thickness */
    float thick = 0.1e-6

    /* Calcium */
    float CCaI = 0.000020 // resting concentration in mM
    float DCa = 6.0e-10   // diffusion constant in m^2/sec

    /* Fast immobile buffer */
    float FBuf = 0.1 // total buffer concentration in mM
    float FBuf0 = 0.2 // total buffer concentration in mM in first shell
    float kfFBuf = 5.0e5  // forward binding rate in 1/(mM.sec) 
    float kbFBuf = 500  // backward binding rate in 1/sec

    /* Pump rate */
    float Pump_kP = 1.4e-5
    //float Pump_kP = 0

    /* Synaptic channels */
    float G_NMDA = 50.0e-12	// maximum conductance
    float E_NMDA = 0.0		// reversal potential
    float tau1_NMDA = 20.0e-3	// open time constant
    float tau2_NMDA = 40.0e-3	// close time constant
    float CMg = 1.2	// Magnesium concentration for magnesium block
    float eta = 0.2801
    float gamma = 62
    float CA_FRACT = 1	// relative fraction of Ca current flowing into shell

    float G_AMPA = 5.0e-12	// maximum conductance
    float E_AMPA = 0.0		// reversal potential
    float tau1_AMPA = 2.0e-3	// open time constant
    float tau2_AMPA = 9.0e-3	// close time constant
