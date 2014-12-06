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

/* Channel parameters */
float   ENA = 0.050
float   EK = -0.077
float   GNA = 1200                      // Siemens/m^2
float   GK = 360                        // Siemens/m^2
float	area = SOMA_A

create tabchannel /library/HH_Na_channel
        setfield     /library/HH_Na_channel   \
        Ek      {ENA}   \       //  V
        Gbar    { GNA * area }  \   //  S
        Ik      0   \           //  A
        Gk      0   \           //  S
        Xpower  3   \
        Ypower  1   \
        Zpower  0

    setupalpha /library/HH_Na_channel X {0.1e6*(0.025 + EREST_ACT)}  \
         -0.1e6 -1.0 {-0.025 - EREST_ACT} -0.010   \
         4e3 0.0 0.0 {-(0.0 + EREST_ACT) } 18.0e-3

    setupalpha /library/HH_Na_channel Y 70.0 0.0 0.0  \
        {-(0.0 + EREST_ACT)} 0.020  \
        1.0e3 0.0 1.0 {-(0.030 + EREST_ACT)} -10.0e-3

//========================================================================
//                        ACTIVE K CHANNEL
//========================================================================

create  tabchannel  /library/HH_K_channel
        setfield     /library/HH_K_channel   \
        Ek      {EK}    \           //  V
        Gbar    { GK * area }  \   //  S
        Ik      0   \           //  A
        Gk      0   \           //  S
        Xpower  4   \
        Ypower  0   \
        Zpower  0

    setupalpha /library/HH_K_channel X {10e3*(0.010+EREST_ACT)}  \
        -10e3 -1.0 {-(0.010+EREST_ACT)} -0.010  \
        125.0 0.0 0.0 {-(0.0+EREST_ACT)} 80.0e-3


//========================================================================
//                        SPIKE DETECTOR
//========================================================================
create 	spikegen	/library/spike
    setfield /library/spike \
	thresh		-40e-3 \ 			// V
	abs_refract	{ 10e-3 } \ 		// sec
	output_amp		1



