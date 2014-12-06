//genesis2.2
/* Copyright E. De Schutter BBF-UIA Oct 1998 */
/* Sets up model for SPINEDEMO.g */

function make_all_comparts

    float surf

    ce /solve
    /* create spine head */
    create compartment head
    surf = head_len*head_dia*{PI}
    setfield head Cm {CM*surf} Em {EREST_ACT} Rm {RM/surf} \
	    Ra {4.0 * RA * head_len /(head_dia * head_dia * PI)} \
	    inject 0.0  dia {head_dia} len {head_len} initVm {EREST_ACT}

    ce head
    /* add a NMDA channel: is used to compute channel conductance only */
    create synchan NMDA
    setfield NMDA gmax {G_NMDA} Ek {E_NMDA} tau1 {tau1_NMDA} tau2 {tau2_NMDA}

    /* add the Mg block: the blocked NMDA current is used to compute voltage */
    create Mg_block Mg_block
    setfield Mg_block CMg {CMg}  Ek {E_NMDA} Zk 2 \
            KMg_A {1/eta} \ \\ *({exp {EREST_ACT*gamma}})} \
            KMg_B {1.0/gamma}
    addmsg NMDA Mg_block CHANNEL Gk Ek
    addmsg /solve/head Mg_block VOLTAGE Vm
    addmsg Mg_block /solve/head CHANNEL Gk Ek

    /* add a non-NMDA channel: is always activated together with the NMDA
    ** channel */
    ce /solve/head
    create synchan AMPA
    setfield AMPA gmax {G_AMPA} Ek {E_AMPA} tau1 {tau1_AMPA} tau2 {tau2_AMPA}
    addmsg /solve/head AMPA VOLTAGE Vm
    addmsg AMPA /solve/head CHANNEL Gk Ek

    ce /solve

    /* create presynaptic element for NMDA and AMPA channels */
    create neutral presyn
    setfield presyn z 0
    /* a trick: we use coordinate field to set activation */
    addmsg presyn head/NMDA ACTIVATION z
    addmsg presyn head/AMPA ACTIVATION z

    /* create spine neck */
    create compartment neck
    surf = neck_len*neck_dia*{PI}
    setfield neck Cm {CM*surf} Em {EREST_ACT} Rm {RM/surf} \
	    Ra {4.0 * RA * neck_len /(neck_dia * neck_dia * PI)} \
	    inject 0.0  dia {neck_dia} len {neck_len} initVm {EREST_ACT}
    addmsg head neck RAXIAL Ra previous_state
    addmsg neck head AXIAL previous_state

    /* create dendritic shaft compartment */
    create compartment dend
    surf = dend_len*dend_dia*{PI}
    setfield dend Cm {CM*surf} Em {EREST_ACT} Rm {RM/surf} \
	    Ra {4.0 * RA * dend_len /(dend_dia * dend_dia * PI)} \
	    inject 0.0  dia {dend_dia} len {dend_len} initVm {EREST_ACT}
    addmsg neck dend RAXIAL Ra previous_state
    addmsg dend neck AXIAL previous_state

    /* create skeleton cell */
    create compartment ddend	// distal end of dendrite
    surf = ddend_len*ddend_dia*{PI}
    setfield ddend Cm {CM*surf} Em {EREST_ACT} Rm {RM/surf} \
	    Ra {4.0 * RA * ddend_len /(ddend_dia * ddend_dia * PI)} \
	    inject 0.0  dia {ddend_dia} len {ddend_len} initVm {EREST_ACT}
    addmsg ddend dend RAXIAL Ra previous_state
    addmsg dend ddend AXIAL previous_state
    create compartment apical // proximal end of dendrite
    surf = apical_len*apical_dia*{PI}
    setfield apical Cm {CM*surf} Em {EREST_ACT} Rm {RM/surf} \
	    Ra {4.0 * RA * apical_len /(apical_dia * apical_dia * PI)} \
	    inject 0.0  dia {apical_dia} len {apical_len} initVm {EREST_ACT}
    addmsg dend apical RAXIAL Ra previous_state
    addmsg apical dend AXIAL previous_state
    create compartment soma
    surf = soma_dia*soma_dia*{PI}
    setfield soma Cm {CM*surf} Em {EREST_ACT} Rm {RM/surf} \
	    Ra {13.5 * RA /(soma_dia * PI)} \
	    inject 0.0  dia {soma_dia} len 0.0 initVm {EREST_ACT}
    addmsg apical soma RAXIAL Ra previous_state
    addmsg soma apical AXIAL previous_state
end

function make_all_shells
/* We use two types of diffusion elements:
** SLABS: see Fig. 6.4 of De Schutter and Smolen (1998), this fig. also
**  illustrates the shape of the spine.
** SHELLS: see Fig. 6.3 of De Schutter and Smolen (1998).
** De Schutter E., and Smolen P.: Calcium dynamics in large neuronal models.
**  in Methods in neuronal modeling: from ions to networks (2nd edition),
**  C. Koch and I. Segev editors, 211-250 (1998).
*/

    int i,numshells
    str shell,fast,pshell,pump

    /* All the difshells MUST be attached to the head compartment because
    **  they are considered to be one continuous diffusion system */
    ce /solve/head

    /* First the shells corresponding to the spine head: these are SLABs */
    numshells = head_len/thick
    for (i = 0; i < {numshells}; i = i + 1)
	shell = "headshell" @ {i}
	fast =  "headbuf" @ {i}
	pump =  "headpump" @ {i}

	/* make the shell */
	create difshell {shell}
	setfield {shell} C {CCaI} Ceq {CCaI} D {DCa} val 2.0  leak 0.0 
	setfield {shell} shape_mode {SLAB} len 0.0 dia {head_dia} thick {thick}

	if (i==0)
	    /* Ca fraction of NMDA current is influx into outer shell:
	    **  in reality this fraction increases with depolarization,
	    **  but we do not model this.
	    */
	    addmsg Mg_block {shell} FINFLUX  Ik 0.128
	end

	/* attach fast immobile buffer */
	create fixbuffer {fast}
	if (i==0) // different concentration in first shell
	    setfield {fast} Btot {FBuf0}
	else
	    setfield {fast} Btot {FBuf}
	end
	setfield {fast} kBf {kfFBuf} kBb {kbFBuf}
	addmsg {fast} {shell} BUFFER kBf kBb Bfree Bbound
	addmsg {shell} {fast} CONCEN C

	/* attach the pump */
	create taupump {pump}
	setfield {pump} Ceq {CCaI}
	/* The pump rate is proportional to the surface-to-volume ratio */
	if (i==0) /* end of cylinder is a circle: stv=1/thick */
	    setfield {pump} T_C {thick/Pump_kP}	// inverse of pump rate
	else	/* for a cylinder stv=4/dia */
	    setfield {pump} T_C {head_dia/4/Pump_kP}	// inverse of pump rate
	end
	addmsg {pump} {shell} TAUPUMP kP Ceq

	/* setup diffusion messages */
	if (i > 0)
	    addmsg {pshell} {shell} DIFF_DOWN prev_C  thick
	    addmsg {shell} {pshell} DIFF_UP prev_C thick
	end

	pshell = {shell}
    end

    /* Next the shells corresponding to the spine neck: these are SLABs */
    numshells = neck_len/thick
    for (i = 0; i < {numshells}; i = i + 1)
	shell = "neckshell" @ {i}
	fast =  "neckbuf" @ {i}
	pump =  "neckpump" @ {i}

	/* make the shell */
	create difshell {shell}
	setfield {shell} C {CCaI} Ceq {CCaI} D {DCa} val 2.0  leak 0.0 
	setfield {shell} shape_mode {SLAB} len 0.0 dia {neck_dia} thick {thick}

	/* attach fast immobile buffer */
	create fixbuffer {fast}
	setfield {fast} Btot {FBuf} kBf {kfFBuf} kBb {kbFBuf}
	addmsg {fast} {shell} BUFFER kBf kBb Bfree Bbound
	addmsg {shell} {fast} CONCEN C

	/* attach the pump */
	create taupump {pump}
	setfield {pump} Ceq {CCaI}
	/* The pump rate is proportional to the surface-to-volume ratio */
	setfield {pump} T_C {neck_dia/4/Pump_kP}	// inverse of pump rate
	addmsg {pump} {shell} TAUPUMP kP Ceq

	/* setup diffusion messages */
	addmsg {pshell} {shell} DIFF_DOWN prev_C  thick
	addmsg {shell} {pshell} DIFF_UP prev_C thick

	pshell = shell
    end

    /* Finally the shells corresponding to the dendritic shaft: these are SHELLs */
    numshells = dend_dia/thick/2
    for (i = 0; i < {numshells}; i = i + 1)
	shell = "dendshell" @ {i}
	fast =  "dendbuf" @ {i}
	pump =  "dendpump" @ {i}

	/* make the shell */
	create difshell {shell}
	setfield {shell} C {CCaI} Ceq {CCaI} D {DCa} val 2.0  leak 0.0 
	setfield {shell} shape_mode {SHELL} len {dend_len} \
            dia {dend_dia - i*2*thick} thick {thick}

	/* attach fast immobile buffer */
	create fixbuffer {fast}
	setfield {fast} Btot {FBuf} kBf {kfFBuf} kBb {kbFBuf}
	addmsg {fast} {shell} BUFFER kBf kBb Bfree Bbound
	addmsg {shell} {fast} CONCEN C

	/* attach the pump */
	if (i==0)
	    create taupump {pump}
	    setfield {pump} Ceq {CCaI}
	    /* The pump rate is proportional to the surface-to-volume ratio */
	    setfield {pump} T_C {dend_dia/4/Pump_kP}	// inverse of pump rate
	    addmsg {pump} {shell} TAUPUMP kP Ceq
	end

	/* setup diffusion messages */
	addmsg {pshell} {shell} DIFF_DOWN prev_C  thick
	addmsg {shell} {pshell} DIFF_UP prev_C thick

	pshell = shell
    end
end

/*********************************************************************/
