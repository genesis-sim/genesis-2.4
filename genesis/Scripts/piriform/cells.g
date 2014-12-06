//genesis

str string

//========================================================================
//                        PYRAMIDAL CELL
//========================================================================
/*
** this function constructs a pyramidal cell according to the arguments
** passed to it
*/
function pyramidal_cell(pathname, active, multi, spike, connect, soma_d \
    , soma_l, dend_d, dend_l)

str pathname
int active
int multi
int spike
int connect
// um
float soma_d
// um
float soma_l
// um
float dend_d
// um
float dend_l

    // um^2
    float soma_a = {PI}*soma_d*soma_l
    // um^2
    float soma_xa = {PI}*soma_d*soma_d/4
    // um^2
    float dend_a = {PI}*dend_d*dend_l
    // um^2
    float dend_xa = {PI}*dend_d*dend_d/4

    create neutral {pathname}
    ce {pathname}
    if (active)
	// add the Hodgkin-Huxley compartment

	//	copy 	/library/HHcompartment	soma

	// add two voltage dependent conductances

	copy /library/HH_Na_channel HH_Na_channel
	copy /library/HH_K_channel HH_K_channel
	link_hhchannel HH_Na_channel soma
	link_hhchannel HH_K_channel soma
    else
	// passive soma

	create compartment soma
	// mV
	// Kohm
	// uF
	// Kohm 
	setfield soma Em {PYR_EREST} Rm {{RM}/soma_a}  \
	    Cm {{CM}*soma_a*{SCALE}} Ra {{RA}*soma_l/soma_xa}
    end

    /*
    ** add a buffer for generating thresholded spike events 
    */
    copy /library/spike .
    setfield spike abs_refract 10
    if (active)
	// mV
	setfield spike thresh 0
    else
	// mV
	setfield spike thresh -40
    end
    /*
    ** use the soma potential (or state) to drive the thresholded
    ** spike generation buffer
    */
    addmsg soma spike INPUT Vm

    /*
    ** axons
    */
    foreach string (RA CA LRA LCA FF FB)
	create axon {string}axon
	setfield {string}axon latency {SYNDELAY}
	addmsg spike {string}axon BUFFER name
    end

    if (multi)
	pushe soma
	/*
	** add a passive dendritic compartments
	*/
	create compartment III_dend
	    // mV
	    // Kohms
	    // uF
	    // Kohm
	    setfield III_dend Em {PYR_EREST} Rm {{RM}/dend_a}  \
	        Cm {{CM}*dend_a*{SCALE}} Ra {{RA}*dend_l/dend_xa}

	copy III_dend deepIb_dend
	copy III_dend supIb_dend
	copy III_dend Ia_dend

	/*
	** and link them 
	*/
	link_compartment III_dend .
	link_compartment deepIb_dend .
	link_compartment supIb_dend deepIb_dend
	link_compartment Ia_dend supIb_dend
	pope
    end

    if (spike)
	/*
	** add spike producing channels
	*/
	create channelC SNa_channel
	    // mV
	    // msec
	    // msec
	    // mS
	    setfield SNa_channel Ek {ENA} tau1 {0.2*{SCALE}}  \
	        tau2 {0.2*{SCALE}} gmax {{SGMAX_NA}*{fAC}*soma_a}
	create channelC SK_channel
	    // mV
	    // msec
	    // msec
	    // mS
	    setfield SK_channel Ek {EK} tau1 {1*{SCALE}}  \
	        tau2 {1*{SCALE}} gmax {{SGMAX_K}*{fAC}*soma_a}
	/*
	** use the spike output as activation for the channels
	*/
	addmsg spike SNa_channel ACTIVATION state
	addmsg spike SK_channel ACTIVATION state
	singlelink_channel SNa_channel soma
	singlelink_channel SK_channel soma
    end

    if (connect)
	/*
	** add synaptically activated channels
	*/
	if (multi)
	    pushe soma/Ia_dend
	    copy /library/Na_channel Aff_Na_channel
	    // mS/synapse
	    setfield Aff_Na_channel gmax {DISTAL_GMAX_NA}
	    link_channel Aff_Na_channel .
	    if ({EXTRA})
		copy /library/Cl_channel Cl_channel
		setfield Cl_channel gmax {GMAX_CL}
		link_channel Cl_channel .
	    end
	    pope

	    pushe soma/supIb_dend
	    copy /library/Na_channel CA_Na_channel
	    setfield CA_Na_channel gmax {DISTAL_GMAX_NA}
	    link_channel CA_Na_channel .
	    pope

	    pushe soma/deepIb_dend
	    copy /library/Na_channel RA_Na_channel
	    setfield RA_Na_channel gmax {DISTAL_GMAX_NA}
	    link_channel RA_Na_channel .
	    pope

	    pushe soma/III_dend
	    copy /library/Na_channel Local_Na_channel
	    setfield Local_Na_channel gmax {LOCAL_GMAX_NA}
	    link_channel Local_Na_channel .
	    pope

	    pushe soma/Ia_dend
	    copy /library/K_channel K_channel
	    setfield K_channel gmax {GMAX_K}
	    link_channel K_channel .
	    pope

	    pushe soma
	    copy /library/Cl_channel Cl_channel
	    setfield Cl_channel gmax {GMAX_CL}
	    link_channel Cl_channel .
	    pope
	else
	    pushe soma
	    copy /library/Na_channel Aff_Na_channel
	    setfield Aff_Na_channel gmax {DISTAL_GMAX_NA}
	    link_channel Aff_Na_channel .

	    copy /library/Na_channel CA_Na_channel
	    setfield CA_Na_channel gmax {DISTAL_GMAX_NA}
	    link_channel CA_Na_channel .

	    copy /library/Na_channel RA_Na_channel
	    setfield RA_Na_channel gmax {DISTAL_GMAX_NA}
	    link_channel RA_Na_channel .

	    copy /library/Na_channel Local_Na_channel
	    setfield Local_Na_channel gmax {LOCAL_GMAX_NA}
	    link_channel Local_Na_channel .

	    copy /library/K_channel K_channel
	    setfield K_channel gmax {GMAX_K}
	    link_channel K_channel .

	    copy /library/Cl_channel Cl_channel
	    setfield Cl_channel gmax {GMAX_CL}
	    link_channel Cl_channel .
	    pope
	end	// multi

    end    // connect

    if (multi)
	pushe soma
	/*
	** set dendritic positions relative to the soma
	*/
	// 100um deep to the soma
	position III_dend I I R120e-3
	// 100um super to the soma
	position deepIb_dend I I R-190e-3
	// 200um super to the soma
	position supIb_dend I I R-310e-3
	// 300um super to the soma
	position Ia_dend I I R-430e-3
	pope
    end    // multi

end// pyramidal_cell


//========================================================================
//                        INTERNEURON
//========================================================================
function interneuron(pathname, soma_d, soma_l, Cl, multaxon)
str pathname
// um
float soma_d
// um
float soma_l
// flag for incorporating a Cl channel
int Cl
// flag for additional axon
int multaxon

    // um^2
    float soma_a = {PI}*soma_d*soma_l
    // um^2
    float soma_xa = {PI}*soma_d*soma_d/4

    create neutral {pathname}
    ce {pathname}

    /*
    ** soma
    */
    create compartment soma
    // mV
    // Kohms
    // uF
    // Kohm
    setfield soma Em {I_EREST} Rm {{I_RM}/soma_a}  \
        Cm {{I_CM}*soma_a*{SCALE}} Ra {{I_RA}*soma_l/soma_xa}
    /*
    ** channels
    */
    copy /library/Na_channel soma
    setfield soma/Na_channel gmax {I_GMAX_NA}
    link_channel soma/Na_channel soma

    if (Cl)
        copy /library/Cl_channel soma
        setfield soma/Cl_channel gmax {I_GMAX_CL}
        link_channel soma/Cl_channel soma
    end

    /*
    ** add spike producing channels
    */
    create channelC SNa_channel
    // mV
    // msec
    // msec
    // mS
    setfield SNa_channel Ek {ENA} tau1 {0.2*{SCALE}} tau2 {0.2*{SCALE}}  \
        gmax {{SGMAX_NA}*{fAC}*soma_a}
    create channelC SK_channel
    // mV
    // msec
    // msec
    // mS
    setfield SK_channel Ek {EK} tau1 {1*{SCALE}} tau2 {1*{SCALE}}  \
        gmax {{SGMAX_K}*{fAC}*soma_a}
    /*
    ** use the spike output as activation for the channels
    */
    singlelink_channel SNa_channel soma
    singlelink_channel SK_channel soma

    copy /library/spike .
    setfield spike thresh -40
    setfield spike abs_refract 10
    addmsg soma spike INPUT Vm
    addmsg spike SNa_channel ACTIVATION state
    addmsg spike SK_channel ACTIVATION state

    create axon axon
    setfield axon latency {SYNDELAY}
    addmsg spike axon BUFFER name
    if (multaxon)
        create axon fbaxon
        setfield axon latency {SYNDELAY}
        addmsg spike fbaxon BUFFER name
    end
end// interneuron


//========================================================================
//                        BULBAR INPUT
//========================================================================
function bulb(path)
str path

    create neutral {path}
    ce ^
    create random input
    setfield input min_amp 3
    setfield input max_amp 6
    // spikes/msec
    setfield input rate .2
    create axon axon
    addmsg input axon BUFFER name
    create axon axon2
    addmsg input axon2 BUFFER name
    create axon axon3
    addmsg input axon3 BUFFER name
end

