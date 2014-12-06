//genesis
// cell.g
/* 
	functions defined in this script
=============================================================================
	FUNCTION NAME		ARGUMENTS
=============================================================================
	makeneuron		(path,soma_l,soma_d,dend_l,dend_d)
	reset_kludge
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
    setfield soma initVm {EREST_ACT}  // properly initialize Vm on reset
    //=============================================
    //          voltage dependent conductances
    //=============================================
    make_hhNa soma Ex_channel {active_area} {ENa} {EREST_ACT}
    make_hhK soma Inh_channel {active_area} {EK} {EREST_ACT}

    //=============================================
    //                   dendrites
    //=============================================
    makecompartment dend1 {dend_l} {dend_d} {EREST}
    link_compartment dend1 soma
    makecompartment dend2 {dend_l} {dend_d} {EREST}
    link_compartment dend2 dend1
    //=============================================
    //    synaptic conductances on the dendrites
    //=============================================
    makechannel dend1 Ex_channel {EEx} 3.0 3.0 {GEx}
    makechannel dend1 Inh_channel {EInh} 3.0 3.0 {GInh}
    makechannel dend2 Ex_channel {EEx} 3.0 3.0 {GEx}
    makechannel dend2 Inh_channel {EInh} 3.0 3.0 {GInh}

    pope
end

// reset_kludge was a kludge to properly initialize soma Vm to EREST on reset
// In 2.0, this is handled by initVm field
function reset_kludge
    reset
    /* Should add some tricky business to allow cycling of colors with
      overlayed soma Vm plots, as was done in 1.4 version          */
end
