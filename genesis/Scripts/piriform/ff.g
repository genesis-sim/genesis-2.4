//genesis
echo creating ff population
create neutral /ff

interneuron /interneuron {GLOB_SOMA_D} {GLOB_SOMA_L} 0 0
setfield /interneuron/axon latency {FFDELAY}
createmap /interneuron /ff {FF_NX} {FF_NY} -delta {FF_DX} {FF_DY}
delete /interneuron
position /ff I I 200e-3

echo setting ff thresholds
setrandfield /ff/interneuron[]/spike thresh -gaussian -40 2

if ({T_NOISE})
    // ==========================================================
    //                    TRANSMITTER NOISE
    // ==========================================================
    create funcgen /ff/Nanoise
    setfield ^ mode 3 amplitude 1 x {150*{getclock 0}}
    // prob amplitude
    addmsg ^ /ff/interneuron[]/soma/Na_channel RAND_ACTIVATION output x

end
