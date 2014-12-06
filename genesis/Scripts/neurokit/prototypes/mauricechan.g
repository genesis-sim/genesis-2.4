// genesis


/* FILE INFORMATION
** Several Voltage dependent channels
** Developed by : Maurice Lee for Inferior Olive simulations
** Implemented in NEUROKIT format by : Upinder S. Bhalla
**
** This file depends on functions and constants defined in defaults.g
*/

// CONSTANTS
	float EREST_ACT = -0.07	// V
	float ENA = 0.045 	// V
	float EK = -0.085	// V
	float ECA = 0.070	// V
	float SOMA_A = 1e-9     // m^2

	//	----------------------------------------------
	//	HH CHANNELS
	//	----------------------------------------------

/*************************************************************** \
**
**			Na channel
*****************************************************************/

function make_Na_io_vdep
	if (({exists Na_io_vdep}))
		return
	end

	create vdep_channel Na_io_vdep
	setfield Na_io_vdep Ek {ENA} gbar {1200.0*SOMA_A} Ik 0 Gk 0

	create vdep_gate Na_io_vdep/m
	// 1/sec
	// 1/V*sec
	// no unit
	// V
	// V
	// 1/sec
	// 1/V*sec
	// no unit
	// V
	// V
	setfield ^ alpha_A {5000.0 - (-200000.0*EREST_ACT)}  \
	    alpha_B -200000.0 alpha_C -1.0  \
	    alpha_D {(25/-1000) - EREST_ACT} alpha_F {10.0/-1000.0}  \
	    beta_A 8000.0 beta_B 0 beta_C 0 beta_D {-EREST_ACT}  \
	    beta_F {18.0/1000.0}

	create vdep_gate Na_io_vdep/h
	// 1/sec
	// 1/V*sec
	// no unit
	// V
	// V
	// 1/sec
	// 1/V*sec
	// no unit
	// V
	// V
	setfield ^ alpha_A 70.0 alpha_B 0 alpha_C 0 alpha_D {-EREST_ACT} \
	     alpha_F {20.0/1000.0} beta_A 1000.0 beta_B 0 beta_C 1.0  \
	    beta_D {30.0/-1000.0 - EREST_ACT} beta_F {10.0/-1000.0}

	addmsg Na_io_vdep/m Na_io_vdep MULTGATE m 3
	addmsg Na_io_vdep/h Na_io_vdep MULTGATE m 1
	addfield  Na_io_vdep addmsg1
	addfield  Na_io_vdep addmsg2
	setfield  Na_io_vdep addmsg1 ".. m	VOLTAGE	Vm"  \
	    addmsg2 ".. h	VOLTAGE	Vm"
end

/*****************************************************************
**			Ca channel
*****************************************************************/

function make_Ca_io_vdep
	if (({exists Ca_io_vdep}))
		return
	end

	create vdep_channel Ca_io_vdep
	setfield Ca_io_vdep Ek {ECA} gbar {750.0*SOMA_A} Ik 0 Gk 0

	create vdep_gate Ca_io_vdep/d
	// 1/sec
	// 1/V*sec
	// no unit
	// V
	// V
	// 1/sec
	// 1/V*sec
	// no unit
	// V
	// V
	setfield ^ alpha_A 28.55 alpha_B 0 alpha_C 0  \
	    alpha_D {(-0.24/10.0) - EREST_ACT} alpha_F {-1.0/10.0}  \
	    beta_A 28.55 beta_B 0 beta_C 0  \
	    beta_D {1.728/-72.0 - EREST_ACT} beta_F {-1.0/-72.0}

	create vdep_gate Ca_io_vdep/f
	// 1/sec
	// 1/V*sec
	// no unit
	// V
	// V
	// 1/sec
	// 1/V*sec
	// no unit
	// V
	// V
	setfield ^ alpha_A 9.5 alpha_B 0 alpha_C 0  \
	    alpha_D {-2.26/-112.75 - EREST_ACT} alpha_F {-1.0/-112.75}  \
	    beta_A 9.5 beta_B 0 beta_C 0 beta_D {2.1/96.25 - EREST_ACT}  \
	    beta_F {-1/96.25}

	addmsg Ca_io_vdep/d Ca_io_vdep MULTGATE m 2
	addmsg Ca_io_vdep/f Ca_io_vdep MULTGATE m 1
	addfield  Ca_io_vdep addmsg1
	addfield  Ca_io_vdep addmsg2
	setfield  Ca_io_vdep addmsg1 ".. d	VOLTAGE	Vm"  \
	    addmsg2 ".. f	VOLTAGE	Vm"
end


/*****************************************************************
**			K0 channel
*****************************************************************/



function make_K0_io_tchan
	if (({exists K0_io_tchan}))
		return
	end
	create tabchannel K0_io_tchan
		// Volts
		// Seimens
		setfield ^ Ek {EK} Gbar {1.2e3*SOMA_A} Ik 0 Gk 0  \
		    Xpower 4 Ypower 0 Zpower 0

	setupalpha K0_io_tchan X {100.0 + 1e4*EREST_ACT} -1e4 -1.0  \
	    {-0.01 - EREST_ACT} -0.01 125.0 0.0 0.0 {-EREST_ACT} 0.08
end

/*****************************************************************
**			K1 channel
*****************************************************************/



function make_K1_io_tchan
	if (({exists K1_io_tchan}))
		return
	end
	create tabchannel K1_io_tchan
		setfield ^ Ek {EK} Gbar {1.2e3*SOMA_A} Ik 0 Gk 0  \
		    Xpower 1 Ypower 0 Zpower 0

	setupalpha K1_io_tchan X 17.5 0.0 0.0 0.0 100.0 250.0 0.0 1.0 \
	     {-8e-4 - EREST_ACT} 2e-5

	// FIXED
	// constant to return
	setfield K1_io_tchan X_A->calc_mode 2 X_A->oy 17.5
end
