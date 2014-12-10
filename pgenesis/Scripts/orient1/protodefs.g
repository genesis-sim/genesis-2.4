// genesis

create neutral /library 
// We don't want the library to try to calculate anything,
// so we disable it.
disable /library

//========================================================================
//                              CHANNELS   
//========================================================================
create		synchan	/library/Na_channel
    setfield	        /library/Na_channel \
	Ek			{ENA} \
	tau1		{ 2.0e-3 } \	// sec
	tau2		{ 2.0e-3 } \ 	// sec
	gmax		{DISTAL_GMAX_NA}

create		synchan	/library/K_channel
    setfield	        /library/K_channel \
	Ek			{ EK } \
	tau1		{ 100.0e-3 } \	// sec
	tau2		{ 100.0e-3 } \	// sec
	gmax		{GMAX_K}		// S

create		synchan	/library/Cl_channel
    setfield	        /library/Cl_channel \
	Ek			{ECL} \
	tau1		{ 4.0e-3 } \	// sec
	tau2		{ 4.0e-3 } \	// sec
	gmax		{GMAX_CL}		// S

//========================================================================
//                               COMPARTMENT
//========================================================================
create	compartment /library/compartment
setfield /library/compartment \
	Cm		{CM*SOMA_A} \				// F
	Ra		{RA*SOMA_L/SOMA_XA} \		// ohm
	Em  		{EREST_ACT} \			// V
	Rm			{RM/SOMA_A} \  			// ohm
	inject		0.0

//========================================================================
//                        ACTIVE NA CHANNEL
//========================================================================
create		hh_channel	/library/HH_Na_channel
setfield /library/HH_Na_channel \
	Ek 		{ENA_ACT} \			// V
	Gbar		{ 1.2e3 * SOMA_A } \	// S
	Xpower		3.0 \
	Ypower		1.0 \   
	X_alpha_FORM	{LINOID} \
	X_alpha_A	-0.1e6 \				// 1/V-sec
	X_alpha_B	-0.010 \				// V
	X_alpha_V0	{ 0.025 + EREST_ACT } \	// V
	X_beta_FORM	{EXPONENTIAL} \
	X_beta_A	4.0e3 \					// 1/sec
	X_beta_B	-18.0e-3 \				// V
	X_beta_V0	{ 0.0 + EREST_ACT } \	// V
	Y_alpha_FORM	{EXPONENTIAL} \
	Y_alpha_A	70.0 \					// 1/sec
	Y_alpha_B	-20.0e-3 \				// V
	Y_alpha_V0	{ 0.0 + EREST_ACT } \	// V
	Y_beta_FORM	{SIGMOID} \
	Y_beta_A	1.0e3 \					// 1/sec
	Y_beta_B	-10.0e-3 \				// V
	Y_beta_V0	{ 30.0e-3 + EREST_ACT }	// V

//========================================================================
//                        ACTIVE K CHANNEL
//========================================================================
create		hh_channel	/library/HH_K_channel
    setfield /library/HH_K_channel \
	Ek 		{EK} \						// V
	Gbar		{360.0*SOMA_A} \		// S
	Xpower		4.0 \
	Ypower		0.0 \   
	X_alpha_FORM	{LINOID} \
	X_alpha_A	-10.0e3 \				// 1/V-sec
	X_alpha_B	-10.0e-3 \				// V
	X_alpha_V0	{10.0e-3+EREST_ACT} \	// V
	X_beta_FORM	{EXPONENTIAL} \
	X_beta_A	125.0 \					// 1/sec
	X_beta_B	-80.0e-3 \				// V
	X_beta_V0	{0.0+EREST_ACT} 	 	// V

//========================================================================
//                        SPIKE DETECTOR
//========================================================================
create 	spikegen	/library/spike
    setfield /library/spike \
	thresh		-40e-3 \ 			// V
	abs_refract	{ 10e-3 } \ 		// sec
	output_amp		1

// $Log: protodefs.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:39  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.1  1999/12/18 03:02:05  mhucka
// Added PGENESIS from GENESIS 2.1 CDROM distribution.
//
// Revision 1.1  1996/05/03 19:11:39  ngoddard
// Initial revision
//
