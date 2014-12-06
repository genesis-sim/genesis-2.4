// Simple regular spiking neocortical pyramidal cell model
// Alain Destexhe, Michael Rudolph, Jean-Marc Fellous and Terrence J Sejnowski
// Neuroscience 107: 13-24, (2001) Fluctuating synaptic conductances recreate
//  in-vivo-like activity in neocortical neurons

*cartesian
*asymmetric
*relative

*set_compt_param    RM          2.2
*set_compt_param    RA          2.5
*set_compt_param    CM          0.01
*set_compt_param    ELEAK      -0.080   // volts (Em = leakage potential)
*set_compt_param    EREST_ACT  -0.070   // volts (initVm = resting potential)

// #    name    parent      x   y   z   d     ch dens  ch dens...

soma none  0  0  105 105  \
    Na_pyr_dp      516    \
    Kdr_pyr_dp     100    \
    KM_pyr_dp        5    \
    Ex_channel     0.3    \
    spike 0.0
