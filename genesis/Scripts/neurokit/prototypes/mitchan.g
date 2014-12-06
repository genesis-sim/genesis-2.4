//genesis

/* FILE INFORMATION
** Several versions of Mitral cell voltage-dependent channels.
** Implemented by : Upinder S. Bhalla.
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
float SOMA_A = 1e-9  /* Square meters */

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

	setupalpha Na_mit_tchan X {320e3*(0.013 + EREST_ACT)} -320e3  \
	    -1.0 {-1.0*(0.013 + EREST_ACT)} -0.004  \
	    {-280e3*(0.040 + EREST_ACT)} 280e3 -1.0  \
	    {-1.0*(0.040 + EREST_ACT)} 5.0e-3

	setupalpha Na_mit_tchan Y 128.0 0.0 0.0  \
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
		setfield ^ Ek {EK} Gbar {360.0*SOMA_A} Ik 0 Gk 0  \
		    Xpower 4 Ypower 0 Zpower 0

	setupalpha K_mit_tchan X {32e3*(0.015 + EREST_ACT)} -32e3  \
	    -1.0 {-1.0*(0.015 + EREST_ACT)} -0.005 500.0 0.0 0.0  \
	    {-1.0*(0.010 + EREST_ACT)} 40.0e-3
end



//========================================================================
//			Tabulated Ca dependent K - channel.
//========================================================================

function make_Kca_mit_tab
	if (({exists Kca_mit_tab}))
		return
	end
	float EK = -0.08

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

	setupgate Kca_mit_tab/qca alpha  {5.0e5*0.01500} \
	     -5.0e5 -1.0 -0.01500 -0.00130 -size 1000 -range 0.0 0.01

	call Kca_mit_tab/qca TABCREATE beta 1 -1 100
	setfield Kca_mit_tab/qca beta->table[0] 5.0e1
	setfield Kca_mit_tab/qca beta->table[1] 5.0e1

	addmsg Kca_mit_tab/qv Kca_mit_tab/qca PRD_ALPHA output
	addmsg Kca_mit_tab/qca Kca_mit_tab MULTGATE m 1
	addfield  Kca_mit_tab addmsg1
	addfield  Kca_mit_tab addmsg2
	setfield  Kca_mit_tab  \
	    addmsg1 "../Ca_mit_conc	qca	VOLTAGE		Ca" \
	    addmsg2 "..			qv	INPUT		Vm"
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

	setupgate Ca_mit_tab/r beta  {25.0*200.0} -25.0  \
	    -1.0 -200.0 -20.0 -size 1000 -range -1 1000

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

//========================================================================
//                        ACTIVE NA CHANNEL - MITRAL
//========================================================================
function make_Na_mit_hh
	if (({exists Na_mit_hh}))
		return
	end

	create hh_channel Na_mit_hh
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

	// this part can't work since Traub uses 2 exps to get the Y. So I set
	// the Ypower to zero for now and ignore it.

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
