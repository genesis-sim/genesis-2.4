// genesis

/* FILE INFORMATION
** Na and K channels
**	Traub and Llinas, J Neurophysiol 42:2, 476-496, 1979
** 
** Implemented in NEUROKIT format by Upinder S Bhalla - March 1991
** This file depends on functions and constants defined in defaults.g
*/

// CONSTANTS
// (I-current)
float ENa = 0.045
// sq m
float SOMA_A = 1e-9


/*********************************************************************
**                          Na Current
*********************************************************************/

function make_Na_hip_pyr_tl
    if (({exists Na_hip_pyr_tl}))
        return
    end
// (I-current)
float ENa = 0.045

    create tabchannel Na_hip_pyr_tl
    setfield Na_hip_pyr_tl Ek {ENa} Gbar {1200.0*SOMA_A} Ik 0 Gk 0  \
        Xpower 3 Ypower 1 Zpower 0

	setup_tabchan Na_hip_pyr_tl X {4e5*(0.013 + {EREST_ACT})} -4.0e5 \
	     -1.0 {-0.013 - {EREST_ACT}} -0.005  \
	    {4.0e5*(-0.045 - {EREST_ACT})} 4.0e5 -1.0  \
	    {-0.045 - {EREST_ACT}} 0.005

	setup_tabchan Na_hip_pyr_tl Y 280 0.0 0.0  \
	    {-0.0075 - {EREST_ACT}} 0.020 4e3 0.0 1.0  \
	    {-0.0375 - {EREST_ACT}} -0.020
end


/*********************************************************************
**                          K Current 
*********************************************************************/

function make_K_hip_pyr_tl
    if (({exists K_hip_pyr_tl}))
        return
    end
float EK = -0.08

    create tabchannel K_hip_pyr_tl
    setfield K_hip_pyr_tl Ek {EK} Gbar {1200.0*SOMA_A} Ik 0 Gk 0  \
        Xpower 4 Ypower 0 Zpower 0

	setup_tabchan K_hip_pyr_tl X {7e4*(-0.03 + {EREST_ACT})} -7e4  \
	    -1.0 {-0.03 + {EREST_ACT}} -0.006 100 0.0 0.0  \
	    {-0.005 - {EREST_ACT}} 0.08
end

/*********************************************************************
**                          Ca Current 
*********************************************************************/

function make_Ca_hip_pyr_tl
    if (({exists Ca_hip_pyr_tl}))
        return
    end
float ECa = 0.07

    create tabchannel Ca_hip_pyr_tl
    setfield Ca_hip_pyr_tl Ek {ECa} Gbar {1200.0*SOMA_A} Ik 0 Gk 0  \
        Xpower 5 Ypower 1 Zpower 0

	setup_tabchan Ca_hip_pyr_tl X {5e4*(0.035 + {EREST_ACT})} -5.0e4 \
	     -1.0 {-0.035 - {EREST_ACT}} -0.010  \
	    {1.0e4*(-0.015 - {EREST_ACT})} 1.0e4 -1.0  \
	    {-0.015 - {EREST_ACT}} 0.010

	setup_tabchan Ca_hip_pyr_tl Y 14 0.0 0.0 {0.025 - {EREST_ACT}}  \
	    0.020 200 0.0 1.0 {-0.045 - {EREST_ACT}} -0.010

end

/********************************************************************/
