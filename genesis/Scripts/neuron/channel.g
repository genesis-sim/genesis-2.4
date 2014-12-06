
//genesis
/* channel.g


	functions defined in this script
=============================================================================
	FUNCTION NAME		ARGUMENTS
=============================================================================
	makechannel		(compartment,channel,Ek,tau1,tau2,gmax)
	make_hhNa		(compartment,channel,activearea,ENa,Erest)
	make_hhK		(compartment,channel,activearea,EK,Erest)
=============================================================================
*/


//=================================================
//       BASIC TRANSMITTER ACTIVATED CHANNEL
//=================================================
function makechannel(compartment, channel, Ek, tau1, tau2, gmax)
    str compartment
    str channel
    // mV
    float Ek
    // msec
    float tau1, tau2
    // mS
    float gmax

    create synchan {compartment}/{channel}
    setfield ^ Ek {Ek} tau1 {tau1} tau2 {tau2} gmax {gmax}
    link_channel2 {compartment}/{channel} {compartment}
end

/*
 The Hodgkin-Huxley voltage activated channels are implemented with
 tabchannel objects using parameters taken from
 A.L.Hodgkin and A.F.Huxley, J.Physiol(Lond) 117, pp 500-544 (1952)

 The following two functions may be compared to those in MultiCell/channel.g,
 where the hh_channel object is used.  These are based upon hh_tchan.g in
 the neurokit/prototypes directory, but use physiological units (mV and msec).
 This means that the "-range" option of the setupalpha function must be
 specified, as the default range is -0.1 to 0.05 Volts.
*/

//=================================================
//                 HH NA CHANNEL
//=================================================
function make_hhNa(compartment, channel, activearea, ENa, EREST_ACT)
    str compartment
    str channel
    float activearea     // cm^2
    float ENa, EREST_ACT    // mV

    create tabchannel {compartment}/{channel}
    setfield ^ Ek {ENa} Gbar {120.0*activearea} Xpower 3 Ypower 1 Zpower 0

    setupalpha {compartment}/{channel} X {0.1*(25.0 + EREST_ACT)} -0.1  \
            -1.0 {-1.0*(25.0 + EREST_ACT)} -10.0  \
            4.0 0.0 0.0 {-1.0*EREST_ACT} 18.0 -range -100.0 50.0

    setupalpha {compartment}/{channel} Y 0.07 0.0 0.0  \
            {-1.0*EREST_ACT} 20.0 1.0 0.0 1.0  \
            {-1.0*(30.0 + EREST_ACT)} -10.0  -range -100.0 50.0

    addmsg {compartment} {compartment}/{channel} VOLTAGE Vm
    addmsg {compartment}/{channel} {compartment} CHANNEL Gk Ek
end

//=================================================
//                  HH K CHANNEL
//=================================================
function make_hhK(compartment, channel, activearea, EK, EREST_ACT)
    str compartment
    str channel
    float activearea
    float EK, EREST_ACT    // mV

    create tabchannel {compartment}/{channel}
    setfield ^ Ek {EK} Gbar {36.0*activearea} Xpower 4 Ypower 0 Zpower 0

    setupalpha {compartment}/{channel} X {0.01*(10.0 + EREST_ACT)} -0.01  \
            -1.0 {-1.0*(10.0 + EREST_ACT)} -10.0 0.125 0.0 0.0  \
            {-1.0*EREST_ACT} 80.0 -range -100.0 50.0

    addmsg {compartment} {compartment}/{channel} VOLTAGE Vm
    addmsg {compartment}/{channel} {compartment} CHANNEL Gk Ek
end
