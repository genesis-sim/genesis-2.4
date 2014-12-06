// genesis
function add_squid_electronics

    // Electronics for voltage and current clamp 

    create pulsegen /pulsegen
    setfield ^ level1 25.0 width1 50.0 delay1 2.0 delay2 999.0  \
        trig_mode 1.0 trig_time 0.0

    // changed time constant from 0.1 msec to 0.03 msec for sharper Vclamp pulse
    create RC /lowpass
    setfield ^ R 1.0 C 0.03

    create diffamp /Iclamp
    setfield ^ saturation 999.0 gain 0.0

    create diffamp /Vclamp
    setfield ^ saturation 999.0 gain 0.0

    create PID /PID
    setfield ^ gain 0.50 tau_i 0.02 tau_d 0.005 saturation 999.0
end


function connect_squid_electronics(path)
    str path

    // hook up current clamp circuitry

    addmsg /pulsegen /Iclamp PLUS output
    addmsg /Iclamp {path} INJECT output


    // hook up voltage clamp circuitry

    addmsg /pulsegen /lowpass INJECT output
    addmsg /lowpass /Vclamp PLUS state
    addmsg /Vclamp /PID CMD output
    addmsg {path} /PID SNS Vm
    addmsg /PID {path} INJECT output
end
