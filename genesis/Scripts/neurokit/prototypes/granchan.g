//genesis

/* FILE INFORMATION
** hh_channel implementation of Granule cell voltage-dependent channels.
** Implemented by : Upinder S. Bhalla.
**
** The granule cell channels have been adapted from Traub's channels for
** the hippocampal pyramidal cell :  
** 	R.D.Traub, Neuroscience Vol 7 No 5 pp 1233-1242 (1982)
**
** This file depends on functions and constants defined in library.g
*/

// CONSTANTS
/* granule cell resting potl */
float EREST_ACT = -0.060
float ENA = 0.045
float EK = -0.090
/* Square meters */
float SOMA_A = 1e-9


//========================================================================
//                        ACTIVE NA CHANNEL - GRANULE CELL
//========================================================================
function make_Na_gran_hh
	if (({exists Na_gran_hh}))
		return
	end

	create hh_channel Na_gran_hh
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
	setfield Na_gran_hh Ek {ENA} Gbar {1.2e3*SOMA_A} Xpower 3.0  \
	    Ypower 1.0 X_alpha_FORM {LINOID} X_alpha_A -0.32e6  \
	    X_alpha_B -0.004 X_alpha_V0 {0.023 + EREST_ACT}  \
	    X_beta_FORM {LINOID} X_beta_A 0.28e6 X_beta_B 5.0e-3  \
	    X_beta_V0 {50.0e-3 + EREST_ACT} Y_alpha_FORM {EXPONENTIAL}  \
	    Y_alpha_A 128.0 Y_alpha_B -18.0e-3  \
	    Y_alpha_V0 {0.027 + EREST_ACT} Y_beta_FORM {SIGMOID}  \
	    Y_beta_A 4.0e3 Y_beta_B -5.0e-3  \
	    Y_beta_V0 {50.0e-3 + EREST_ACT}
end

//========================================================================
//                        ACTIVE K CHANNEL - GRANULE CELL

//========================================================================
function make_K_gran_hh
	if (({exists K_gran_hh}))
		return
	end

	create hh_channel K_gran_hh
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
	setfield K_gran_hh Ek {EK} Gbar {360.0*SOMA_A} Xpower 4.0  \
	    Ypower 0.0 X_alpha_FORM {LINOID} X_alpha_A -32.0e3  \
	    X_alpha_B -5.0e-3 X_alpha_V0 {0.025 + EREST_ACT}  \
	    X_beta_FORM {EXPONENTIAL} X_beta_A 500.0 X_beta_B -40.0e-3  \
	    X_beta_V0 {0.020 + EREST_ACT} Y_alpha_FORM {EXPONENTIAL}  \
	    Y_alpha_A 128.0 Y_alpha_B -18.0e-3  \
	    Y_alpha_V0 {0.017 + EREST_ACT} Y_beta_FORM {SIGMOID}  \
	    Y_beta_A 4.0e3 Y_beta_B -5.0e-3  \
	    Y_beta_V0 {40.0e-3 + EREST_ACT}
end
//========================================================================
