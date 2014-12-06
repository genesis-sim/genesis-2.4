//genesis
/* 
	functions defined in this script
=============================================================================
	FUNCTION NAME		ARGUMENTS
=============================================================================
	makeneuron		(path,soma_l,soma_d,dend_l,dend_d)
=============================================================================
*/

//=================================================
//                    NEURON
//=================================================
function makeneuron(path, soma_l, soma_d, dend_l, dend_d)
    str path
    float soma_l, soma_d
    float dend_l, dend_d

    // 100% of the soma active
    float active_area = soma_l*PI*soma_d*1.0

    create neutral {path}
    pushe {path}
    //=============================================
    //                   cell body
    //=============================================
    makecompartment soma {soma_l} {soma_d} {Eleak}
    position soma I I R{-soma_l/2.0}

    //=============================================
    //          voltage dependent conductances
    //=============================================
    make_hhNa soma hh_Na {active_area} {ENa} {EREST}
    make_hhK soma hh_K {active_area} {EK} {EREST}

    //=============================================
    //                   dendrite
    //=============================================
    makecompartment soma/dend {dend_l} {dend_d} {Eleak}
    position soma/dend I I R{-dend_l/2.0}
    link_compartment soma/dend soma

    //=============================================
    //    synaptic conductances on the dendrite
    //=============================================
    makechannel soma/dend Na_channel {ENa} 3.0 3.0 {GNa}
    makechannel soma/dend K_channel {EK} 10.0 10.0 {GK}

    //=============================================
    //                 spike detector
    //=============================================
    create spike spike
    setfield spike thresh 0 abs_refract 10 output_amp 1
    /* use the soma membrane potential to drive the spike detector */
    addmsg soma spike INPUT Vm

    //=============================================
    //                 output axon
    //=============================================
    create axon axon
    /* use the spike detector output to drive the axon */
    addmsg spike axon BUFFER name

    pope
end
