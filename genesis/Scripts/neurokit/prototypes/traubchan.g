//genesis

/* FILE INFORMATION
** The Traub set of voltage and conc dependent channels
** Implemented by : Upinder S. Bhalla.
** 	R.D.Traub, Neuroscience Vol 7 No 5 pp 1233-1242 (1982)
**
** This file depends on functions and constants defined in defaults.g
*/

// CONSTANTS
/* hippocampal cell resting potl */
float EREST_ACT = -0.070
float ENA = 0.045
float EK = -0.085
float ECA = 0.070
float SOMA_A = 1e-9 /* Square meters */

//========================================================================
//			Tabulated Ca Channel
//========================================================================


function make_Ca_hip_traub
	if (({exists Ca_hip_traub}))
		return
	end


	create vdep_channel Ca_hip_traub
		setfield ^ Ek {ECA} gbar {1.2e3*SOMA_A} Ik 0 Gk 0

	create tabgate Ca_hip_traub/s
	/* there is a singularity at x=0, so I hack around that by using
	** an odd number of sample points */
	setup_table Ca_hip_traub/s alpha 101 {40e3*(0.060 + EREST_ACT)}  \
	    -40e3 -1.0 {-1.0*(0.060 + EREST_ACT)} -0.010
	setup_table Ca_hip_traub/s beta 101 {-5e3*(0.045 + EREST_ACT)}  \
	    5e3 -1.0 {-1.0*(0.045 + EREST_ACT)} 10.0e-3

	create tabgate Ca_hip_traub/r
	call Ca_hip_traub/r TABCREATE alpha 1 -1 1000
	setfield Ca_hip_traub/r alpha->table[0] 5.0
	setfield Ca_hip_traub/r alpha->table[1] 5.0

	setupgate Ca_hip_traub/r beta  {25.0*200.0} -25.0 \
	     -1.0 -200.0 -20.0  -size 1000 -range -1 1000

/*
	create Ca_concen Ca_hip_traub/conc
	set Ca_hip_traub/conc \
		tau		0.01	\			// sec
		B		{5.2e-6/(SOMA_XA* \
	    SOMA_L)} \	// Curr to conc
		Ca_base		0.0
*/

	addmsg Ca_hip_traub/s Ca_hip_traub MULTGATE m 5
	addmsg Ca_hip_traub/r Ca_hip_traub MULTGATE m 1
	addfield  Ca_hip_traub	addmsg1
	addfield  Ca_hip_traub	addmsg2
	addfield  Ca_hip_traub	addmsg3
	setfield  Ca_hip_traub  \
	    addmsg1 "../Ca_mit_conc	r	VOLTAGE	Ca" \
	    addmsg2 ".		../Ca_mit_conc	I_Ca	Ik" \
	    addmsg3 "..	s		VOLTAGE	Vm"

end

//========================================================================
//			Ca conc
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
//			Tabulated K channel - 
//========================================================================


function make_K_hip_traub
	if (({exists K_hip_traub}))
		return
	end

	create vdep_channel K_hip_traub
	setfield ^ Ek {EK} gbar {360.0*SOMA_A} Ik 0 Gk 0

	create tabgate K_hip_traub/n
	setup_table K_hip_traub/n alpha 100 {32e3*(0.015 + EREST_ACT)}  \
	    -32e3 -1.0 {-1.0*(0.015 + EREST_ACT)} -0.005
	setup_table K_hip_traub/n beta 100 500.0 0.0 0.0  \
	    {-1.0*(0.010 + EREST_ACT)} 40.0e-3

	create table K_hip_traub/ya2
	call K_hip_traub/ya2 TABCREATE 100 -0.1 0.1
	setup_table3 K_hip_traub/ya2 table 100 -0.1 0.1 2000 0 1  \
	    {-1.0*(0.085 + EREST_ACT)} -0.010
	create tabgate K_hip_traub/y
	setup_table K_hip_traub/y alpha 100 28 0 0  \
	    {-1.0*(0.015 + EREST_ACT)} 0.015
	setup_table K_hip_traub/y beta 100 400 0 1  \
	    {-1.0*(0.040 + EREST_ACT)} -0.010

	addmsg K_hip_traub/n K_hip_traub MULTGATE m 4
	addmsg K_hip_traub/y K_hip_traub MULTGATE m 1
	addmsg K_hip_traub/ya2 K_hip_traub/y SUM_ALPHA output
	addfield K_hip_traub  addmsg1
	addfield K_hip_traub  addmsg2
	addfield K_hip_traub  addmsg3
	setfield  K_hip_traub addmsg1 "..	n	VOLTAGE	Vm"  \
	    addmsg2 "..	y	VOLTAGE	Vm"  \
	    addmsg3 "..	ya2	INPUT	Vm"
end

//========================================================================
//			Tabulated Ca dependent K - channel.
//========================================================================

function make_Kca_hip_traub
	if (({exists Kca_hip_traub}))
		return
	end

	create vdep_channel Kca_hip_traub

	setfield ^ Ek {EK} gbar {360.0*SOMA_A} Ik 0 Gk 0

	create table Kca_hip_traub/qv
	call Kca_hip_traub/qv TABCREATE 100 -0.1 0.1
	int i
	float x, dx, y
	x = -0.1
	dx = 0.2/100.0
	for (i = 0; i <= 100; i = i + 1)
		y = {exp {(x - EREST_ACT)/0.027}}
		setfield Kca_hip_traub/qv table->table[{i}] {y}
		x = x + dx
	end

	create tabgate Kca_hip_traub/qca

	setupgate Kca_hip_traub/qca alpha  {5.0*200.0}  \
	    -5.0 -1.0 -200.0 -20.0 -size 1000 -range -1 100

	call Kca_hip_traub/qca TABCREATE beta 1 -1 100
	setfield Kca_hip_traub/qca beta->table[0] 2.0
	setfield Kca_hip_traub/qca beta->table[1] 2.0

	addmsg Kca_hip_traub/qv Kca_hip_traub/qca PRD_ALPHA output
	addmsg Kca_hip_traub/qca Kca_hip_traub MULTGATE m 1
	addfield  Kca_hip_traub addmsg1
	addfield  Kca_hip_traub addmsg2
	setfield  Kca_hip_traub  \
	    addmsg1 "../Ca_mit_conc	qca	VOLTAGE		Ca" \
	    addmsg2 "..		qv	INPUT		Vm"
end

//========================================================================
