//genesis

/* FILE INFORMATION
** Several versions of Mitral cell voltage-dependent channels.
** Implemented by : Upinder S. Bhalla.
**
** Additional variations have been implemented in this file for doing
** benchmarks and comparisons on computational accuracy on the various
** channel options.
**
** The mitral cell channels have been adapted from Traub's channels for
** the hippocampal pyramidal cell :  
** 	R.D.Traub, Neuroscience Vol 7 No 5 pp 1233-1242 (1982)
**
** This file depends on functions and constants defined in defaults.g
*/

// CONSTANTS
/* mitral cell resting potl */
float EREST_ACT = -0.065
float ENA = 0.045
float EK = -0.090
float ECA = 0.070
/* Square meters */
float SOMA_A = 1e-9

//========================================================================
//                        ACTIVE NA CHANNEL - MITRAL
//========================================================================
function make_Na_mit_hh
	if (({exists Na_mit_hh}))
		return
	end

	create hh_channel Na_mit_hh
	// V
	// S
	// 1/V-sec
	// V
	// V
	// 1/sec
	// V
	// V
	// 1/sec
	// V
	// V
	// 1/sec
	// V
	// V
	setfield Na_mit_hh Ek {ENA} Gbar {1.2e3*SOMA_A} Xpower 3.0  \
	    Ypower 1.0 X_alpha_FORM {LINOID} X_alpha_A -0.32e6  \
	    X_alpha_B -0.004 X_alpha_V0 {0.013 + EREST_ACT}  \
	    X_beta_FORM {LINOID} X_beta_A 0.28e6 X_beta_B 5.0e-3  \
	    X_beta_V0 {40.0e-3 + EREST_ACT} Y_alpha_FORM {EXPONENTIAL}  \
	    Y_alpha_A 128.0 Y_alpha_B -18.0e-3  \
	    Y_alpha_V0 {0.017 + EREST_ACT} Y_beta_FORM {SIGMOID}  \
	    Y_beta_A 4.0e3 Y_beta_B -5.0e-3  \
	    Y_beta_V0 {40.0e-3 + EREST_ACT}
end

//========================================================================
//                        ACTIVE K CHANNEL - MITRAL
//========================================================================
function make_K_mit_hh
	if (({exists K_mit_hh}))
		return
	end

	create hh_channel K_mit_hh
	// V
	// S
	// 1/V-sec
	// V
	// V
	// 1/sec
	// V
	// V
	// this part cant work since Traub uses 2 exps to get the Y. So I set
	// the Ypower to zero for now and ignore it.
	// 1/sec
	// V
	// V
	// 1/sec
	// V
	// V
	setfield K_mit_hh Ek {EK} Gbar {360.0*SOMA_A} Xpower 4.0  \
	    Ypower 0.0 X_alpha_FORM {LINOID} X_alpha_A -32.0e3  \
	    X_alpha_B -5.0e-3 X_alpha_V0 {0.015 + EREST_ACT}  \
	    X_beta_FORM {EXPONENTIAL} X_beta_A 500.0 X_beta_B -40.0e-3  \
	    X_beta_V0 {0.010 + EREST_ACT} Y_alpha_FORM {EXPONENTIAL}  \
	    Y_alpha_A 128.0 Y_alpha_B -18.0e-3  \
	    Y_alpha_V0 {0.017 + EREST_ACT} Y_beta_FORM {SIGMOID}  \
	    Y_beta_A 4.0e3 Y_beta_B -5.0e-3  \
	    Y_beta_V0 {40.0e-3 + EREST_ACT}
end

//========================================================================
//	vdep_gate versions of the mitral cell channels
//========================================================================

//========================================================================
//			Active Na Channel 
//========================================================================

function make_Na_mit_vdep
	if (({exists Na_mit_vdep}))
		return
	end

	create vdep_channel Na_mit_vdep
	setfield ^ Ek {ENA} gbar {1.2e3*SOMA_A} Ik 0 Gk 0

	create vdep_gate Na_mit_vdep/m
	//	1/sec
	//	1/V*sec
	//	no unit
	//	V
	//	V
	//	1/sec
	//	1/V*sec
	//	no unit
	//	V
	//	V
	setfield ^ alpha_A {320e3*(0.013 + EREST_ACT)} alpha_B -320e3  \
	    alpha_C -1.0 alpha_D {-1.0*(0.013 + EREST_ACT)}  \
	    alpha_F -0.004 beta_A {-280e3*(0.040 + EREST_ACT)}  \
	    beta_B 280e3 beta_C -1.0 beta_D {-1.0*(0.040 + EREST_ACT)}  \
	    beta_F 5.0e-3 instantaneous 0

	create vdep_gate Na_mit_vdep/h
	//	1/sec
	//	no unit
	//	no unit
	//	V
	//	V
	//	1/sec
	//	no unit
	//	no unit
	//	V
	//	V
	setfield ^ alpha_A 128.0 alpha_B 0.0 alpha_C 0.0  \
	    alpha_D {-1.0*(0.017 + EREST_ACT)} alpha_F 0.018  \
	    beta_A 4.0e3 beta_B 0.0 beta_C 1.0  \
	    beta_D {-1.0*(0.040 + EREST_ACT)} beta_F -5.0e-3  \
	    instantaneous 0

	addmsg Na_mit_vdep/m Na_mit_vdep MULTGATE m 3
	addmsg Na_mit_vdep/h Na_mit_vdep MULTGATE m 1
        addfield  Na_mit_vdep addmsg1
        addfield  Na_mit_vdep addmsg2
	setfield  Na_mit_vdep addmsg1 "..	m	VOLTAGE	Vm"  \
	    addmsg2 "..	h	VOLTAGE	Vm"
end

//========================================================================
//			K Mitral cell channel
//========================================================================

function make_K_mit_vdep
	if (({exists K_mit_vdep}))
		return
	end

	create vdep_channel K_mit_vdep
	setfield ^ Ek {EK} gbar {360.0*SOMA_A} Ik 0 Gk 0

	create vdep_gate K_mit_vdep/n
	//	1/sec
	//	1/V*sec
	//	no unit
	//	V
	//	V
	//	1/sec
	//	1/V*sec
	//	no unit
	//	V
	//	V
	setfield ^ alpha_A {32.0e3*(0.015 + EREST_ACT)} alpha_B -32.0e3  \
	    alpha_C -1.0 alpha_D {-1.0*(0.015 + EREST_ACT)}  \
	    alpha_F -0.005 beta_A 500.0 beta_B 0.0 beta_C 0.0  \
	    beta_D {-1.0*(0.010 + EREST_ACT)} beta_F 40.0e-3  \
	    instantaneous 0
	addmsg K_mit_vdep/n K_mit_vdep MULTGATE m 4
	addfield  K_mit_vdep addmsg1
	setfield  K_mit_vdep addmsg1 "..	n	VOLTAGE	Vm"
end

//========================================================================
// 	Slow Tabulated versions of the Mitral cell channels
//========================================================================
//			Tabulated Na Mitral cell channel 
//========================================================================

function make_Na_mit_stab
	if (({exists Na_mit_stab}))
		return
	end


	create vdep_channel Na_mit_stab
		setfield ^ Ek {ENA} gbar {1.2e3*SOMA_A} Ik 0 Gk 0

	create tabgate Na_mit_stab/m
	setup_table Na_mit_stab/m alpha 99 {320e3*(0.013 + EREST_ACT)}  \
	    -320e3 -1.0 {-1.0*(0.013 + EREST_ACT)} -0.004
	setup_table Na_mit_stab/m beta 99 {-280e3*(0.040 + EREST_ACT)}  \
	    280e3 -1.0 {-1.0*(0.040 + EREST_ACT)} 5.0e-3

	create tabgate Na_mit_stab/h
	setup_table Na_mit_stab/h alpha 99 128.0 0.0 0.0  \
	    {-1.0*(0.017 + EREST_ACT)} 0.018
	setup_table Na_mit_stab/h beta 99 4.0e3 0.0 1.0  \
	    {-1.0*(0.040 + EREST_ACT)} -5.0e-3

	addmsg Na_mit_stab/m Na_mit_stab MULTGATE m 3
	addmsg Na_mit_stab/h Na_mit_stab MULTGATE m 1
	addfield  Na_mit_stab addmsg1
	addfield  Na_mit_stab addmsg2
	setfield  Na_mit_stab \
		addmsg1 	"..	m	VOLTAGE	Vm"  \
		addmsg2		"..	h	VOLTAGE	Vm"
end

//========================================================================
//			Slow Tabulated K channel - Psuedo traub.
//========================================================================
function make_K_mit_stab
	if (({exists K_mit_stab}))
		return
	end

	create vdep_channel K_mit_stab
		setfield ^ Ek {EK} gbar {360.0*SOMA_A} Ik 0 Gk 0

	create tabgate K_mit_stab/n
	setup_table K_mit_stab/n alpha 99 {32e3*(0.015 + EREST_ACT)}  \
	    -32e3 -1.0 {-1.0*(0.015 + EREST_ACT)} -0.005
	setup_table K_mit_stab/n beta 99 500.0 0.0 0.0  \
	    {-1.0*(0.010 + EREST_ACT)} 40.0e-3
	addmsg K_mit_stab/n K_mit_stab MULTGATE m 4
	addfield  K_mit_stab addmsg1
	setfield  K_mit_stab addmsg1 "..	n	VOLTAGE	Vm"
end


//========================================================================
// 	Fast Tabulated versions of the Mitral cell channels
//========================================================================
//			Tabulated Na Mitral cell channel 
//========================================================================

function make_Na_mit_ftab
	if (({exists Na_mit_ftab}))
		return
	end

	create vdep_channel Na_mit_ftab
		setfield ^ Ek {ENA} gbar {1.2e3*SOMA_A} Ik 0 Gk 0
	create tabgate Na_mit_ftab/m
	setup_table Na_mit_ftab/m alpha 99 {320e3*(0.013 + EREST_ACT)}  \
	    -320e3 -1.0 {-1.0*(0.013 + EREST_ACT)} -0.004
	setup_table Na_mit_ftab/m beta 99 {-280e3*(0.040 + EREST_ACT)}  \
	    280e3 -1.0 {-1.0*(0.040 + EREST_ACT)} 5.0e-3
	setfield Na_mit_ftab/m alpha->calc_mode 0 beta->calc_mode 0
	call Na_mit_ftab/m TABFILL alpha 3000 0
	call Na_mit_ftab/m TABFILL beta 3000 0

	create tabgate Na_mit_ftab/h
	setup_table Na_mit_ftab/h alpha 99 128.0 0.0 0.0  \
	    {-1.0*(0.017 + EREST_ACT)} 0.018
	setup_table Na_mit_ftab/h beta 99 4.0e3 0.0 1.0  \
	    {-1.0*(0.040 + EREST_ACT)} -5.0e-3
	setfield Na_mit_ftab/h alpha->calc_mode 0 beta->calc_mode 0
	call Na_mit_ftab/h TABFILL alpha 3000 0
	call Na_mit_ftab/h TABFILL beta 3000 0

	addmsg Na_mit_ftab/m Na_mit_ftab MULTGATE m 3
	addmsg Na_mit_ftab/h Na_mit_ftab MULTGATE m 1
	addfield  Na_mit_ftab addmsg1
	addfield  Na_mit_ftab addmsg2
	setfield  Na_mit_ftab addmsg1 "..	m	VOLTAGE	Vm"  \
	    addmsg2 "..	h	VOLTAGE	Vm"
end

//========================================================================
//			Fast Tabulated K channel - psuedo-traub
//========================================================================
function make_K_mit_ftab
	if (({exists K_mit_ftab}))
		return
	end

	create vdep_channel K_mit_ftab
		setfield ^ Ek {EK} gbar {360.0*SOMA_A} Ik 0 Gk 0

	create tabgate K_mit_ftab/n
	setup_table K_mit_ftab/n alpha 99 {32e3*(0.015 + EREST_ACT)}  \
	    -32e3 -1.0 {-1.0*(0.015 + EREST_ACT)} -0.005
	setup_table K_mit_ftab/n beta 99 500.0 0.0 0.0  \
	    {-1.0*(0.010 + EREST_ACT)} 40.0e-3
	setfield K_mit_ftab/n alpha->calc_mode 0 beta->calc_mode 0
	call K_mit_ftab/n TABFILL alpha 3000 0
	call K_mit_ftab/n TABFILL beta 3000 0
	addmsg K_mit_ftab/n K_mit_ftab MULTGATE m 4
	addfield  K_mit_ftab addmsg1
	setfield  K_mit_ftab addmsg1 "..	n	VOLTAGE	Vm"
end

//========================================================================
// 	Super-Fast Tabchan versions of Mitral cell channels
//========================================================================
//			Tabchan Na Mitral cell channel 
//========================================================================

function make_Na_mit_tchan
	if (({exists Na_mit_tchan}))
		return
	end

	create tabchannel Na_mit_tchan
		setfield ^ Ek {ENA} Gbar {1.2e3*SOMA_A} Ik 0 Gk 0  \
		    Xpower 3 Ypower 1 Zpower 0

	setup_tabchan Na_mit_tchan X {320e3*(0.013 + EREST_ACT)} -320e3  \
	    -1.0 {-1.0*(0.013 + EREST_ACT)} -0.004  \
	    {-280e3*(0.040 + EREST_ACT)} 280e3 -1.0  \
	    {-1.0*(0.040 + EREST_ACT)} 5.0e-3

	setup_tabchan Na_mit_tchan Y 128.0 0.0 0.0  \
	    {-1.0*(0.017 + EREST_ACT)} 0.018 4.0e3 0.0 1.0  \
	    {-1.0*(0.040 + EREST_ACT)} -5.0e-3
end

//========================================================================
//			Tabchan version of K channel - psuedo-traub
//========================================================================
function make_K_mit_tchan
	if (({exists K_mit_tchan}))
		return
	end

	create tabchannel K_mit_tchan
		//	V
		//	S
		//	A
		//	S
		setfield ^ Ek {EK} Gbar {360.0*SOMA_A} Ik 0 Gk 0  \
		    Xpower 4 Ypower 0 Zpower 0

	setup_tabchan K_mit_tchan X {32e3*(0.015 + EREST_ACT)} -32e3  \
	    -1.0 {-1.0*(0.015 + EREST_ACT)} -0.005 500.0 0.0 0.0  \
	    {-1.0*(0.010 + EREST_ACT)} 40.0e-3
end


//========================================================================
// 	slowed down Tabchan versions of Mitral cell channels
//========================================================================
//			Tabchan Na Mitral cell channel 
//========================================================================

function make_Na_mit_stchan
	if (({exists Na_mit_stchan}))
		return
	end

	create tabchannel Na_mit_stchan
		setfield ^ Ek {ENA} Gbar {1.2e3*SOMA_A} Ik 0 Gk 0  \
		    Xpower 3 Ypower 1 Zpower 0

	setup_tabchan Na_mit_stchan X {320e3*(0.013 + EREST_ACT)} -320e3 \
	     -1.0 {-1.0*(0.013 + EREST_ACT)} -0.004  \
	    {-280e3*(0.040 + EREST_ACT)} 280e3 -1.0  \
	    {-1.0*(0.040 + EREST_ACT)} 5.0e-3

	setup_tabchan Na_mit_stchan Y 128.0 0.0 0.0  \
	    {-1.0*(0.017 + EREST_ACT)} 0.018 4.0e3 0.0 1.0  \
	    {-1.0*(0.040 + EREST_ACT)} -5.0e-3

	// kluge to make it use linear interp rather than no interp
	setfield Na_mit_stchan X_A->calc_mode 1 X_B->calc_mode 1  \
	    Y_A->calc_mode 1 Y_B->calc_mode 1
end

//========================================================================
//			Tabchan version of K channel - psuedo-traub
//========================================================================
function make_K_mit_stchan
	if (({exists K_mit_stchan}))
		return
	end

	create tabchannel K_mit_stchan
		setfield ^ Ek {EK} Gbar {360.0*SOMA_A} Ik 0 Gk 0  \
		    Xpower 4 Ypower 0 Zpower 0

	setup_tabchan K_mit_stchan X {32e3*(0.015 + EREST_ACT)} -32e3  \
	    -1.0 {-1.0*(0.015 + EREST_ACT)} -0.005 500.0 0.0 0.0  \
	    {-1.0*(0.010 + EREST_ACT)} 40.0e-3

	// kluge to make it use linear interp rather than no interp
	setfield K_mit_stchan X_A->calc_mode 1 X_B->calc_mode 1
end

//========================================================================
//			Tabulated Ca dependent K - channel.
//========================================================================

function make_Kca_mit_tab
	if (({exists Kca_mit_tab}))
		return
	end

	create vdep_channel Kca_mit_tab
		setfield ^ Ek {EK} gbar {360.0*SOMA_A} Ik 0 Gk 0

	create table Kca_mit_tab/qv
	call Kca_mit_tab/qv TABCREATE 100 -0.1 0.1
	int i
	float x, dx, y
	x = -0.1
	dx = 0.2/100.0
	for (i = 0; i <= 100; i = i + 1)
		y = {exp {(x - EREST_ACT)/0.027}}
		setfield Kca_mit_tab/qv table->table[{i}] {y}
		x = x + dx
	end

	create tabgate Kca_mit_tab/qca

	setupgate Kca_mit_tab/qca alpha  {5.0*200.0} -5.0  \
	    -1.0 -200.0 -20.0 -size 1000 -range -1 100

	call Kca_mit_tab/qca TABCREATE beta 1 -1 100
	setfield Kca_mit_tab/qca beta->table[0] 2.0
	setfield Kca_mit_tab/qca beta->table[1] 2.0

	addmsg Kca_mit_tab/qv Kca_mit_tab/qca PRD_ALPHA output
	addmsg Kca_mit_tab/qca Kca_mit_tab MULTGATE m 1
        addfield Kca_mit_tab  addmsg1
        addfield Kca_mit_tab  addmsg2
	setfield  Kca_mit_tab  \
	    addmsg1 "../Ca_mit_conc	qca	VOLTAGE     Ca" \
	    addmsg2 "..		qv	INPUT       Vm"
end


//========================================================================
//			Tabulated Ca Channel - mitral cell
//========================================================================


function make_Ca_mit_tab
	if (({exists Ca_mit_tab}))
		return
	end


	create vdep_channel Ca_mit_tab
		setfield ^ Ek {ECA} gbar {1.2e3*SOMA_A} Ik 0 Gk 0

	create tabgate Ca_mit_tab/s
	/* there is a singularity at x=0, so I hack around that by using
	** an odd number of sample points */
	setup_table Ca_mit_tab/s alpha 101 {40e3*(0.060 + EREST_ACT)}  \
	    -40e3 -1.0 {-1.0*(0.060 + EREST_ACT)} -0.010
	setup_table Ca_mit_tab/s beta 101 {-5e3*(0.045 + EREST_ACT)} 5e3 \
	     -1.0 {-1.0*(0.045 + EREST_ACT)} 10.0e-3

	create tabgate Ca_mit_tab/r
	call Ca_mit_tab/r TABCREATE alpha 1 -1 1000
	setfield Ca_mit_tab/r alpha->table[0] 5.0
	setfield Ca_mit_tab/r alpha->table[1] 5.0

	setupgate Ca_mit_tab/r beta 1000 -1 1000 {25.0*200.0} -25.0  \
	    -1.0 -200.0 -20.0  -size 1000 -range -1 1000

/*
	create Ca_concen Ca_mit_tab/conc
	set Ca_mit_tab/conc \
		tau		0.01	\			// sec
		B		{5.2e-6/(SOMA_XA* \
	    SOMA_L)} \	// Curr to conc
		Ca_base		0.0
*/

	addmsg Ca_mit_tab/s Ca_mit_tab MULTGATE m 5
	addmsg Ca_mit_tab/r Ca_mit_tab MULTGATE m 1
	addfield  Ca_mit_tab addmsg1
	addfield  Ca_mit_tab addmsg2
	addfield  Ca_mit_tab addmsg3
	setfield  Ca_mit_tab  \
	    addmsg1 "../Ca_mit_conc	r	VOLTAGE	Ca" \
	    addmsg2 ".		../Ca_mit_conc	I_Ca	Ik" \
	    addmsg3 "..		s		VOLTAGE	Vm"

end

//========================================================================
//			Ca conc - mitral cell
//========================================================================

function make_Ca_mit_conc
	if (({exists Ca_mit_conc}))
		return
	end
	create Ca_concen Ca_mit_conc
	// sec
	// Curr to conc
	setfield Ca_mit_conc tau 0.01 B 5.2e-6 Ca_base 0.00001
end
