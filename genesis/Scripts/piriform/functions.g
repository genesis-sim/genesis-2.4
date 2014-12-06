//genesis

// =================================
//      link in a new compartment
// =================================
function link_compartment
    addmsg $1 $2 RAXIAL Ra previous_state
    addmsg $2 $1 AXIAL previous_state
end

// =================================
//      link in a new channel
// =================================
function link_channel(chan, comp)
    addmsg {chan} {comp} CHANNEL Gk Ek
    addmsg {comp} {chan} VOLTAGE Vm
end

function singlelink_channel(chan, comp)
    addmsg {chan} {comp} CHANNEL Gk Ek
end

function link_hhchannel(chan, comp)
    addmsg {chan} {comp} CHANNEL Gk Ek
    addmsg {comp} {chan} VOLTAGE Vm
    addmsg {comp} {chan} EREST Erest
end

// =================================
//   somatic current injection (uA)
// =================================
function inject(neuron, value)
str neuron
float value
    setfield {neuron}/soma inject value
end

// =================================
//   modify spike output based on 
//   integration step to give constant 
//   impulse area
// =================================
function adjustspike
    setfield /##[TYPE=spike] output_amp {1.0/{getclock 0}}
    //    echo SPIKE:  {1.0/getclock(0)}
end

// =================================
//   sets baseline LOT input rates
// =================================
function baseline(min, max, rate)
    float min, max, rate
    str path

    path = "/bulb/mitral[]/input"
    setfield {path} min_amp {min} max_amp {max} rate {rate}
end

// =================================
//   simulate shock to the LOT
// =================================
function shock(value)
str path
float value
float oldrate, oldmin, oldmax
    // shock
    baseline {value} {value} 1000
    step 1
    // baseline 0.05 0.1 0.4		// 400 Hz noise
    baseline 0 0 0
end

