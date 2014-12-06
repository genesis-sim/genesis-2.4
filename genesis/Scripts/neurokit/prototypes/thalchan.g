//genesis - thalchan.g

/**************************************************************************
**                                                                         
**      THALCHAN : channel definition file                                 
**                                                                         
**      By Pratik Mukherjee, Aug. 1991                                     
**                                                                         
**      All units are in SI (Meters Kilograms Seconds Amps). While this    
**      does result in lots of powers of 10 in the parameters, it prevents 
**      the chaos of interconversions when relating parameters of different
**      units.                                                             
**                                                                         
***************************************************************************/

//========================================================================
//                  ACTIVE SQUID NA CHANNEL (TABULATED)
//      A.L.Hodgkin and A.F.Huxley, J.Physiol(Lond) 117, pp 500-544 (1952)
//========================================================================
// This channel forcibly sets EREST_ACT to -0.045 V to compute V0
function make_Na_squid_tab
   float tempvar
	if (({exists Na_squid_tab}))
		return
	end
	tempvar = {EREST_ACT}
	EREST_ACT = -0.045
	create tabchannel Na_squid_tab
	setfield Na_squid_tab Ek {ENA_ACT} Gbar {1.2e3*{SOMA_A}}  \
	    Xpower 3.0 Ypower 1.0 Zpower 0.0
	setupalpha Na_squid_tab X {0.1e6*(0.025 + {EREST_ACT})}  \
	    -0.1e6 -1.0 {-0.025 - {EREST_ACT}} -0.010 4.0e3 0.0 0.0  \
	    {0.0 - {EREST_ACT}} 18.0e-3
        setupalpha Na_squid_tab Y 70.0 0.0 0.0 {0.0 - {EREST_ACT}}  \
            20.0e-3 1.0e3 0.0 1.0 {-0.030 - {EREST_ACT}} -10.0e-3
	EREST_ACT = tempvar
end

//========================================================================
//          SEJNOWSKI CORTICAL FAST NA CHANNEL (TABULATED)
//     Lytton & Sejnowski (1991), J. Neurophysiology, in press
//========================================================================
// This channel assumes an EREST_ACT of -0.062 V for published kinetics
function setup_tabchan_bg(chan, table, z, gamma, V0, A0, B0, temp, tmin)
        str chan, table
        float z, gamma, V0, A0, B0, temp, tmin
        float AA, AB, AC, AD, AF, BA, BB, BC, BD, BF
        float T = temp + 273.15
        int i
        str tabB = (table) @ "_B"

        AB = 0         /* convert from Borg-Graham to Hodgkin-Huxley format */
        BB = 0
        AC = 0
        BC = 0
        AA = A0
        BA = B0
        AD = (-1.0*V0)
        BD = (-1.0*V0)
        AF = (-1.0*{R}*T/(z*gamma*{F}))
        BF = (-1.0*{R}*T/(-1*z*(1 - gamma)*{F}))
        setupalpha {chan} {table} {AA} {AB} {AC} {AD} {AF} {BA} {BB}  \
            {BC} {BD} {BF}
end

function make_Na_sej_tab
        if (({exists Na_sej_tab}))
                return
        end

    create tabchannel Na_sej_tab
    setfield Na_sej_tab Ek {ENA} Gbar {1.737e3*{SOMA_A}} Ik 0 Gk 0  \
        Xpower 3 Ypower 1 Zpower 0
        setup_tabchan_bg Na_sej_tab X 3.3 0.7 {0.0275 + {EREST_ACT}}  \
            4.2e3 4.2e3 37.0 5.0e-5
        setup_tabchan_bg Na_sej_tab Y -3.0 0.27 {0.017 + {EREST_ACT}}  \
            90.0 90.0 37.0 2.5e-4
end

//----------------------------------------------------------------------------
//                      ACTIVE SQUID K CHANNEL (TABULATED) 
//      A.L.Hodgkin and A.F.Huxley, J.Physiol(Lond) 117, pp 500-544 (1952) 
//----------------------------------------------------------------------------
function make_K_squid_tab
        if (({exists K_squid_tab}))
                return
        end
        create tabchannel K_squid_tab
        setfield K_squid_tab Ek {EK} Gbar {360.0*{SOMA_A}} Xpower 4.0  \
            Ypower 0.0 Zpower 0.0
        setup_tabchan K_squid_tab X {10.0e3*(0.010 + {EREST_ACT})}  \
            -10.0e3 -1.0 {-10.0e-3 - {EREST_ACT}} -10.0e-3 125.0 0.0 0.0 \
             {-1.0*{EREST_ACT}} 80.0e-3
end

//----------------------------------------------------------------------------
//                      DELAYED RECTIFIER K CURRENT
//      McCormick et. al. (1991), Single Neuron Computation, Academic Press
//----------------------------------------------------------------------------
// This channel forcibly sets EREST_ACT to -0.085 V to compute V0
// Kinetics at -0.085 V match published kinetics
function make_K_lgn_hh
   float tempvar

	if (({exists K_lgn_hh}))
		return
	end

	tempvar = {EREST_ACT}
	// McCormick's EREST
	EREST_ACT = -0.085
	create hh_channel K_lgn_hh
	setfield K_lgn_hh Ek {EK} Gbar {180.0*{SOMA_A}} Xpower 4.0  \
	    Ypower 0.0 X_alpha_FORM {LINOID} X_alpha_A -9.6e4  \
	    X_alpha_B -5.0e-3 X_alpha_V0 {{EREST_ACT} + 0.040}  \
	    X_beta_FORM {EXPONENTIAL} X_beta_A 1.5e3 X_beta_B -4.0e-2  \
	    X_beta_V0 {{EREST_ACT} + 0.035}
	EREST_ACT = tempvar
end

//----------------------------------------------------------------------------
//                DELAYED RECTIFIER K CURRENT (TABULATED)
//      McCormick et. al. (1991), Single Neuron Computation, Academic Press
//----------------------------------------------------------------------------
// This channel forcibly sets EREST_ACT to -0.085 V to compute V0
// Kinetics at -0.085 V match published kinetics
function make_K_lgn_tab
   float tempvar

        if (({exists K_lgn_tab}))
                return
        end

        tempvar = {EREST_ACT}
        EREST_ACT = -0.085         // McCormick's EREST
        create tabchannel K_lgn_tab
        setfield K_lgn_tab Ek {EK} Gbar {180.0*{SOMA_A}} Xpower 4.0  \
            Ypower 0.0 Zpower 0.0
        setup_tabchan K_lgn_tab X 9.6e4*{{EREST_ACT} + 0.040} -9.6e4  \
            -1.0 {-{EREST_ACT} - 0.040} -5.0e-3 1.5e3 0.0 0.0  \
            {-{EREST_ACT} - 0.035} 4.0e-2
        EREST_ACT = tempvar
end

//========================================================================
//                    LOW-THRESHOLD CA (T) CHANNEL 
//     McCormick (1991), Single Neuron Computation, Academic Press 
//========================================================================
// This channel forcibly sets EREST_ACT to -0.085 V to compute V0
// Kinetics at -0.085 V match published kinetics
function make_Ca_lgn_hh
   float tempvar

	if (({exists Ca_lgn_hh}))
		return
	end

        tempvar = {EREST_ACT}
        // McCormick's EREST
        EREST_ACT = -0.085
	create hh_channel Ca_lgn_hh
	setfield Ca_lgn_hh Ek {ECA} Gbar {379.1*{SOMA_A}} Xpower 2.0  \
	    Ypower 1.0 X_alpha_FORM {LINOID} X_alpha_A -7.5e4  \
	    X_alpha_B -7.5e-3 X_alpha_V0 {{EREST_ACT} + 0.035}  \
	    X_beta_FORM {LINOID} X_beta_A 1.01e4 X_beta_B 4.4e-3  \
	    X_beta_V0 {{EREST_ACT} + 0.034} Y_alpha_FORM {LINOID}  \
	    Y_alpha_A 344.0 Y_alpha_B 4.45e-3  \
	    Y_alpha_V0 {{EREST_ACT} + 0.006} Y_beta_FORM {LINOID}  \
	    Y_beta_A -309.3 Y_beta_B -4.0e-3 Y_beta_V0 {EREST_ACT}
	EREST_ACT = tempvar
end

//========================================================================
//             SEJNOWSKI CA (T) CHANNEL (TABULATED)
//     Lytton & Sejnowski (1991), J. Neurophysiology, in press
//========================================================================
// This channel assumes an EREST_ACT of -0.055 V for published kinetics
function make_Ca_sej_tab
        if (({exists Ca_sej_tab}))
                return
        end

    create tabchannel Ca_sej_tab
    setfield Ca_sej_tab Ek {ECA} Gbar {10.0*{SOMA_A}} Ik 0 Gk 0 Xpower 3 \
         Ypower 1 Zpower 0
        setup_tabchan_bg Ca_sej_tab X 3.43 0.5 {-0.008 + {EREST_ACT}}  \
            60.0 60.0 23.0 2.5e-3
        setup_tabchan_bg Ca_sej_tab Y -4.24 0.75 {-0.0285 + {EREST_ACT}} \
             8.0 8.0 23.0 18.0e-3
end

//========================================================================
//	  HYPERPOLARIZATION-ACTIVATED K (H) CHANNEL (TABULATED)
//     McCormick (1991), Single Neuron Computation, Academic Press
//========================================================================
function setup_Atable3_H(gate, table, xdivs, xmin, xmax)
        str gate, table
        int xdivs
        float xmin, xmax

        int i
        float x, dx, y

        dx = xdivs
        dx = (xmax - xmin)/dx
        x = xmin
        for (i = 0; i <= (xdivs); i = i + 1)
                y = 1.0/(({exp {-6.26 - 76.7*x}}) + ({exp {7.13 + 107.9*x}}))
                setfield {gate} {table}->table[{i}] {y}
                x = x + dx
        end
end

function setup_Btable3_H(gate, table, xdivs, xmin, xmax)
        str gate, table
        int xdivs
        float xmin, xmax

        int i
        float x, dx, y

        dx = xdivs
        dx = (xmax - xmin)/dx
        x = xmin
        for (i = 0; i <= (xdivs); i = i + 1)
                y = (x + 0.069)/7.1e-3
                y = 1.0/(1.0 + ({exp {y}}))
                setfield {gate} {table}->table[{i}] {y}
                x = x + dx
        end
end


/* Sets up a tabchan with special non-hh params, with the non-interp option in
** a table of 3000 entries.
** This version uses parameters for tau and minf instead
** of alpha and beta */
function setup_tabchan_tau_H(chan, table)
        str chan, table
        str tabA = (table) @ "_A"
        str tabB = (table) @ "_B"
        call {chan} TABCREATE {table} 49 -0.1 0.05
        setup_Atable3_H {chan} {tabA} 49 -0.1 0.05
        setup_Btable3_H {chan} {tabB} 49 -0.1 0.05
        tweaktau {chan} {table}
        setfield {chan} {tabA}->calc_mode 0 {tabB}->calc_mode 0
        call {chan} TABFILL {table} 3000 0
end

function make_H_lgn_tab
        if (({exists H_lgn_tab}))
                return
        end

        create tabchannel H_lgn_tab
        setfield H_lgn_tab Ek {EH} Gbar {360.0*{SOMA_A}} Gk 0 Ik 0  \
            Xpower 2 Ypower 0 Zpower 0
        setup_tab_tau_H H_lgn_tab X
end

//==========================================================================
//                            K LEAK CHANNEL
//                           Pratik Mukherjee
//==========================================================================
// This channel is mainly used to regulate the resting membrane potential
function make_K_leak
        if (({exists K_leak}))
                return
        end

        create leakage K_leak
        setfield K_leak Ek {EK} activation 0.0 Gk {Gleak} Ik 0.0  \
            inject 0.0
end

