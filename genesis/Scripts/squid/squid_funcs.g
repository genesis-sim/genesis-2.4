// genesis

include squid_consts.g 
include squid_proto.g 

function squid_cmpt(path, length, diameter)
str path
// um
float length, diameter

    // save incoming path
    pushe


    //==============================
    // construct a new compartment
    //==============================

    create compartment {path}
    addfield  {path} parent_cmpt  
    addfield  {path} child_cmpt_1
    addfield  {path} child_cmpt_2
    addfield  {path} length
    addfield  {path} diameter
    addfield  {path} angle

    setfield  {path} parent_cmpt "" child_cmpt_1 "" child_cmpt_2 ""  \
        length {length} diameter {diameter} angle 90
    ce {path}


    // Add Na and K channels

    copy /proto/hh_channels/Na_squid Na
    copy /proto/hh_channels/K_squid K

    // Gates need an initial voltage (RESET) and
    // the current voltage (PROCESS):
    // We'll send them the compartment voltage in both cases,
    // But we'll have to make sure that the compartment is RESET
    // before the gates are RESET so that the initial value is correct.

    addmsg {path} {path}/Na VOLTAGE Vm
    addmsg {path} {path}/Na/m VOLTAGE Vm
    addmsg {path} {path}/Na/h VOLTAGE Vm
    addmsg {path} {path}/Na/m EREST Vm
    addmsg {path} {path}/Na/h EREST Vm

    addmsg {path} {path}/K VOLTAGE Vm
    addmsg {path} {path}/K/n VOLTAGE Vm
    addmsg {path} {path}/K/n EREST Vm

    // The compartment needs the channel conductance
    // and equilibrium voltage

    addmsg {path}/Na {path} CHANNEL Gk Ek
    addmsg {path}/K {path} CHANNEL Gk Ek


    // compartment parameters

    // cm^2
    float xarea = 1e-8*PI*diameter*diameter/4.0
    // cm^2
    float area = 1e-8*length*PI*diameter

    // mV
    setfield Em {Vleak_squid} 
    // uF
    setfield Cm {Cm_squid*area} 
    // kohm
    setfield Rm {1/(Gleak_squid*area)} 
    // kohm
    setfield Ra {Ra_squid*length/xarea} 

    // channel parameters

    setfield Na Ek {VNa_squid}
    // mS
    setfield Na gbar {GNa_squid*area}

    setfield K Ek {VK_squid}
    // mS
    setfield K gbar {GK_squid*area}

    //restore path
    pope
end
