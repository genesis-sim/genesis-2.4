// genesis

/* FILE INFORMATION
** Na and K channels
**	Bunow, Segev and Fleshman, Biol. Cyber 53, 41-56 (1985)
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

function make_Na_alphamn_bsf
    if (({exists Na_alphamn_bsf}))
        return
    end
// (I-current)
float ENa = 0.045

    create tabchannel Na_alphamn_bsf
    setfield Na_alphamn_bsf Ek {ENa} Gbar {1200.0*SOMA_A} Ik 0 Gk 0  \
        Xpower 3 Ypower 1 Zpower 0

	setup_tabchan Na_alphamn_bsf X {2.5e3 + 100.0e3*{EREST_ACT}}  \
	    -100.0e3 -1.0 {-0.0249 - {EREST_ACT}} 0.01 4.0e3 0.0 0.0  \
	    {-{EREST_ACT}} 0.018

	setup_tabchan Na_alphamn_bsf Y 70.0 0.0 0.0 {-{EREST_ACT}}  \
	    1.0/500.0 1e3 0.0 1.0 {0.03 + {EREST_ACT}} -0.010
end


/*********************************************************************
**                          K Current 
*********************************************************************/

function make_K_alphamn_bsf
    if (({exists K_alphamn_bsf}))
        return
    end
float EK = -0.08

    create tabchannel K_alphamn_bsf
    setfield K_alphamn_bsf Ek {EK} Gbar {1200.0*SOMA_A} Ik 0 Gk 0  \
        Xpower 1 Ypower 0 Zpower 0

	setup_tabchan K_alphamn_bsf X {100.0 + 10e3*{EREST_ACT}} -10.0e3 \
	     -1.0 {-0.01 - {EREST_ACT}} -0.01 125.0 0.0 0.0  \
	    {-{EREST_ACT}} {1.0/12.5}
end

/********************************************************************/
