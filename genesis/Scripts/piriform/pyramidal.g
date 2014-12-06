//genesis
echo creating pyramidal cells
/*
** create the pyramidal population element on the root
*/
create neutral /pyr

// ==========================================================
//                    PYRAMIDAL CELL ARRAY
// ==========================================================

pyramidal_cell /pyramidal {ACTIVE} {MULTI} {SPIKE} {CONNECT}  \
    {PYR_SOMA_D} {PYR_SOMA_L} {PYR_DEND_D} {PYR_DEND_L}

createmap /pyramidal /pyr {PYR_NX} {PYR_NY} -delta {PYR_DX} {PYR_DY}
delete /pyramidal
position /pyr I I 350e-3

// ==========================================================
//                    PYRAMIDAL CELL THRESHOLDS
// ==========================================================

if ({ACTIVE})
    setrandfield /pyr/pyramidal[]/HH_Na_channel X_alpha_V0 -gaussian -40 \
         3
else
    setrandfield /pyr/pyramidal[]/spike thresh -gaussian -40 3
end

if ({T_NOISE})
    // ==========================================================
    //                    TRANSMITTER NOISE
    // ==========================================================
    create funcgen /pyr/Affnoise
    setfield ^ mode 3 amplitude 1 x {1000*{getclock 0}}
    addmsg ^ /pyr/pyramidal[]/soma/Ia_dend/Aff_Na_channel  \
        RAND_ACTIVATION output x

    create funcgen /pyr/ffnoise
    setfield ^ mode 3 amplitude .2 x {100*{getclock 0}}
    // prob amplitude
    addmsg ^ /pyr/pyramidal[]/soma/Ia_dend/K_channel RAND_ACTIVATION  \
        output x
end
