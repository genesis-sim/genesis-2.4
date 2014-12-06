// genesis

/* FILE INFORMATION
   GENESIS implementation by D. Beeman of the channel models described in
     Alain Destexhe and Denis Par, Impact of network activity on the
     integrative properties of neocortical pyramidal neurons in vivo.
     Journal of Neurophysiology 81: 1531-1547, 1999

   Some adjustments were made to represent the model in

     Destexhe A, Rudolph M, Fellous JM and Sejnowski TJ.
     Fluctuating synaptic conductances recreate in-vivo-like activity in
     neocortical neurons. Neuroscience 107: 13-24, 2001.

   Based on the NEURON demonstration 'FLUCT' by Alain Destexhe.
   http://cns.iaf.cnrs-gif.fr
*/

// passive membrane parameters
float   CM    // Farads/m^2 = 100x ohm/cm^2
float   RA    // Ohms m     = 0.01x ohm-cm
float   RM    // Ohms m^2   = 0.0001x ohm-cm^2

// channel equilibrium potentials (V)
float   EREST_ACT = -0.063  // value for vtraub in Destexhe et al. (2001)
// float   EREST_ACT = -0.058 // value for vtraub in Destexhe and Par (1999)
float   ENA       =  0.050
float   EK        = -0.090

/* These channels use the setupalpha function to create tabchannel tables
   to represent alpha and beta values in the form (A+B*V)/(C+exp((V+D)/F))
   The first 6 arguments are the coefficients for alpha, and the last 6
   are for beta
*/

//========================================================================
//                Tabchannel Hippocampal fast Na channel
// Based on Traub, R. D. and Miles, R.  Neuronal Networks of the hippocampus
// Cambridge University Press (1991)
//========================================================================

function make_Na_pyr_dp
    str chanpath = "Na_pyr_dp"
    if ({argc} == 1)
       chanpath = {argv 1}
    end
    if (({exists {chanpath}}))
                return
    end

    create tabchannel {chanpath}
    setfield ^  \
        Ek      {ENA}   \               //      V
        Ik      0       \               //      A
        Gk      0       \               //      S
        Xpower  3       \
        Ypower  1       \
        Zpower  0

    setupalpha {chanpath} X  \
        {320e3  * (0.013 + EREST_ACT)}                 \
        -320e3 -1.0 {-1.0   * (0.013 + EREST_ACT)}     \
        -0.004                                          \
        {-280e3 * (0.040 + EREST_ACT)}                 \
        280e3                                           \
        -1.0                                            \
        {-1.0   * (0.040 + EREST_ACT)}                 \
        5.0e-3
    // Traub and Miles Na activation was shifted by Destexhe and Par
    float offset = -0.010 
    setupalpha {chanpath} Y  \
        128.0                           \
        0.0                             \
        0.0                             \
        {-1.0 * (0.017 + EREST_ACT + offset)}    \
        0.018                           \
        4.0e3                           \
        0.0                             \
        1.0                             \
        {-1.0 * (0.040 + EREST_ACT + offset)}    \
        -5.0e-3
end

//========================================================================
//                Tabchannel K(DR) Hippocampal cell channel
// Based on Traub, R. D. and Miles, R.  Neuronal Networks of the hippocampus
// Cambridge University Press (1991)
//========================================================================
function make_Kdr_pyr_dp
    str chanpath = "Kdr_pyr_dp"
    if ({argc} == 1)
       chanpath = {argv 1}
    end
    if (({exists {chanpath}}))
                return
    end

    create tabchannel {chanpath}
    setfield ^  \
        Ek      {EK}    \               //      V
        Ik      0       \               //      A
        Gk      0       \               //      S
        Xpower  4       \
        Ypower  0       \
        Zpower  0

    setupalpha {chanpath} X  \
        {32e3 * (0.015 + EREST_ACT)}    \
        -32e3                           \
        -1.0                            \
        {-1.0 * (0.015 + EREST_ACT) }   \
        -0.005                          \
        500                             \
        0.0                             \
        0.0                             \
        {-1.0 * (0.010 + EREST_ACT) }   \
        0.04
end


//========================================================================
//            Non-inactivating Muscarinic K current
//
//  Noninactivating K+ current responsible for the adaptation of firing rate
//  and the slow afterhyperpolarization (AHP) of cortical pyramidal cells
//
//  Model from Mainen, Z.M. and Sejnowski, T.J., 1996
//  Written for NEURON by Alain Destexhe, Laval University, 1996
//========================================================================

function make_KM_pyr_dp
    str chanpath = "KM_pyr_dp"
    if ({argc} == 1)
       chanpath = {argv 1}
    end
    if (({exists {chanpath}}))
                return
    end

    float offset = 0.0  // Just in case someone wants to shift it
    create tabchannel {chanpath}

    setfield ^  \
        Ek      {EK}    \               //      V
        Ik      0       \               //      A
        Gk      0       \               //      S
        Xpower  1       \
        Ypower  0       \
        Zpower  0

    setupalpha {chanpath} X  \
        {-3.0 -100.0 * offset}    \
        -100.0                           \
        -1.0                            \
        {0.03 - offset}   \
        -0.009                          \
        {3.0 - 100.0*offset}            \
        100.0                             \
        -1.0                             \
        {0.030 - offset}   \
        0.009

end
