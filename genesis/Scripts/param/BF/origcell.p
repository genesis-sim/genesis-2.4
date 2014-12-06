// cell.p.perfect: "correct" test cell for parameter searches

*cartesian
*asymmetric
*relative

*set_compt_param    RM          2.0
*set_compt_param    RA          6.0
*set_compt_param    CM          0.008
*set_compt_param    EREST_ACT  -0.077

*compt /library/compartment

// #    name    parent      x   y   z   d     ch dens  ch dens...

soma none  0  0  20  20  \
    Na_hip_traub91      900.0   \
    Kdr_hip_traub91     100.0   \
    KM_bsg_yka           60.0

