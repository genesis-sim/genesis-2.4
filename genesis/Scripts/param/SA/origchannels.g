// genesis

//
// channels.g: voltage-dependent channel definitions.
//

// passive membrane parameters
float   CM    // Farads/m^2 = 100x ohm/cm^2
float   RA    // Ohms m     = 0.01x ohm-cm
float   RM    // Ohms m^2   = 0.0001x ohm-cm^2

// channel equilibrium potentials (V)
float   EREST_ACT = -0.060
float   ENA       =  0.055
float   EK        = -0.090


//========================================================================
//                Tabchannel Hippocampal fast Na channel
//========================================================================

function make_Na_hip_traub91
    if ({exists Na_hip_traub91})
        return
    end

    create tabchannel Na_hip_traub91
    setfield ^  \
        Ek      {ENA}   \               //      V
        Ik      0       \               //      A
        Gk      0       \               //      S
        Xpower  2       \
        Ypower  1       \
        Zpower  0

    setupalpha Na_hip_traub91 X  \
        {320e3  * (0.0131 + EREST_ACT)}                 \
        -320e3 -1.0 {-1.0   * (0.0131 + EREST_ACT)}     \
        -0.004                                          \
        {-280e3 * (0.0401 + EREST_ACT)}                 \
        280e3                                           \
        -1.0                                            \
        {-1.0   * (0.0401 + EREST_ACT)}                 \
        5.0e-3

    setupalpha Na_hip_traub91 Y  \
        128.0                           \
        0.0                             \
        0.0                             \
        {-1.0 * (0.017 + EREST_ACT)}    \
        0.018                           \
        4.0e3                           \
        0.0                             \
        1.0                             \
        {-1.0 * (0.040 + EREST_ACT)}    \
        -5.0e-3
end


//========================================================================
//                Tabchannel K(DR) Hippocampal cell channel
//========================================================================
function make_Kdr_hip_traub91
    if ({exists Kdr_hip_traub91})
        return
    end

    create tabchannel Kdr_hip_traub91
    setfield ^  \
        Ek      {EK}    \               //      V
        Ik      0       \               //      A
        Gk      0       \               //      S
        Xpower  1       \
        Ypower  0       \
        Zpower  0

    setupalpha Kdr_hip_traub91 X  \
        {16e3 * (0.0351 + EREST_ACT)}   \
        -16e3                           \
        -1.0                            \
        {-1.0 * (0.0351 + EREST_ACT) }  \
        -0.005                          \
        250                             \
        0.0                             \
        0.0                             \
        {-1.0 * (0.02 + EREST_ACT) }    \
        0.04
end


//========================================================================
//            Non-inactivating Muscarinic K current
//
// This channel is derived from the genesis prototype channel, but has a
// few minor changes.
//
//========================================================================

function make_KM_bsg_yka
    if ({exists KM_bsg_yka})
        return
    end

    int i
    float x, dx, y

    create tabchannel KM_bsg_yka
    setfield KM_bsg_yka  \
        Ek     {EK}      \
        Ik     0         \
        Gk     0         \
        Xpower 1         \
        Ypower 0         \
        Zpower 0

    call KM_bsg_yka TABCREATE X 49 -0.1 0.05
    x = -0.1
    dx = 0.15 / 49.0

    for (i = 0 ; i <= 49 ; i = i + 1)
        // tau(V) curve:
        y = 2.0 / (3.3 * {exp {(x + 0.035) / 0.02}} + \
            {exp {-(x + 0.035) / 0.02}})
        setfield KM_bsg_yka X_A->table[{i}] {y}

        // minf(V) curve:
        y = 1.0 / (1.0 + {exp {-(x + 0.045) / 0.010}})
        setfield KM_bsg_yka X_B->table[{i}] {y}
        x = x + dx
    end

    tweaktau KM_bsg_yka X
    setfield  KM_bsg_yka X_A->calc_mode 1 X_B->calc_mode 1
    call KM_bsg_yka TABFILL X 3000 0
end


//========================================================================
//                Make library of prototypes
//========================================================================

// We have to delete the tabchannel tables explicitly when deleting the
// library or the interpol_structs in the tabchans will not get deallocated
// and will cause a memory leak.

function delete_channel_library
    ce /library
    call Na_hip_traub91     TABDELETE
    call Kdr_hip_traub91    TABDELETE
    call KM_bsg_yka         TABDELETE
    ce /
    delete /library
end


function make_channel_library
    if ({exists /library})
        delete_channel_library
    end

    create neutral /library
    ce /library
    create compartment compartment
    make_Na_hip_traub91
    make_Kdr_hip_traub91
    make_KM_bsg_yka
    ce /
    disable /library
end

