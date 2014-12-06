//genesis
echo creating the fb population
create neutral /fb

if ({DISINHIB})
    interneuron /interneuron {FUS_SOMA_D} {FUS_SOMA_L} 1 1
else
    interneuron /interneuron {FUS_SOMA_D} {FUS_SOMA_L} 0 0
end
if ({EXTRA})
    create axon /interneuron/axon2
    addmsg /interneuron/spike /interneuron/axon2 BUFFER name
end

createmap /interneuron /fb {FB_NX} {FB_NY} -delta {FB_DX} {FB_DY}
delete /interneuron
position /fb I I 800e-3

echo setting fb thresholds
setrandfield /fb/interneuron[]/spike thresh -gaussian -40 5

if ({T_NOISE})
    // ==========================================================
    //                    TRANSMITTER NOISE
    // ==========================================================
    create funcgen /fb/Nanoise
    setfield ^ mode 3 amplitude .2 x {35/{getclock 0}}
    // prob amplitude
    addmsg ^ /fb/interneuron[]/soma/Na_channel RAND_ACTIVATION output x

end

// ==========================================================
//                    TRANSMITTER NOISE
// ==========================================================
create funcgen /fb/Nanoise
setfield ^ mode 3 amplitude .2 x {10/{getclock 0}}
// prob amplitude
addmsg ^ /fb/interneuron[]/soma/Na_channel RAND_ACTIVATION output x

