//genesis - Purkinje cell M9 genesis2 script
/* Copyright E. De Schutter (Caltech and BBF-UIA) */

/**********************************************************************
** Set of currents developed for rat cerebellum Purkinje neuron by
**  and dendritic spine prototypes.
** This version actually computes all the tables
** E. De Schutter, Caltech, 1991-1992
**********************************************************************/

/* Reference:
** E. De Schutter and J.M. Bower: An active membrane model of the
** cerebellar Purkinje cell. I. Simulation of current clamps in slice.
** Journal of Neurophysiology  71: 375-400 (1994).
** http://www.tnb.ua.ac.be/TNB/TNB_pub8.html
** Consult this reference for sources of equations and experimental data.
*/


/* The data on which these currents are based were collected at room
**   temperature (anything in the range 20 to 25C).  All rate factors were
**   adapted for body temperature (37C), assuming a Q10 of about 3;
**   in practice all rate factors were multiplied by a factor of 5 */

// CONSTANTS
/* should be defined by calling routine (all correctly scaled):
**	ENa, GNa
**	ECa, GCa
**	EK, GK 
**  	Eh, Gh */

int include_Purk_chan

if ( {include_Purk_chan} == 0 )

	include_Purk_chan = 1


//! if you are interested in the tables being saved to files, set this to one.

int bSaveTables = 0

/*********************************************************************
**                  Central channel creation routine
*********************************************************************/

/* Make tabchannels */
/// XAA: multiplier divided by 1e3
/// XAB: multiplier membrane dependence, convert to DBL_MAX
/// XAC: (de)nominator offset
/// XAD: membrane offset
/// XAF: denormalized time constant
function make_chan(chan, Ec, Gc, Xp, XAA, XAB, XAC, XAD, XAF, XBA, XBB,  \
    XBC, XBD, XBF, Yp, YAA, YAB, YAC, YAD, YAF, YBA, YBB, YBC, YBD, YBF \
    , Zp, ZAA, ZAB, ZAC, ZAD, ZAF, ZBA, ZBB, ZBC, ZBD, ZBF)
    str chan
    int Xp, Yp, Zp
    float Ec, Gc
    float XAA, XAB, XAC, XAD, XAF, XBA, XBB, XBC, XBD, XBF
    float YAA, YAB, YAC, YAD, YAF, YBA, YBB, YBC, YBD, YBF
    float ZAA, ZAB, ZAC, ZAD, ZAF, ZBA, ZBB, ZBC, ZBD, ZBF

    if ({exists {chan}})
        return
    end

    create tabchannel {chan}
    setfield {chan} Ek {Ec} Gbar {Gc} Ik 0 Gk 0 Xpower {Xp} Ypower {Yp}  \
        Zpower {Zp}
    if (Xp > 0)
        setupalpha {chan} X {XAA} {XAB} {XAC} {XAD} {XAF} {XBA} {XBB} {XBC} \
			{XBD} {XBF} -size {tab_xfills+1} -range {tab_xmin} {tab_xmax}
    end
    if (Yp > 0)
        setupalpha {chan} Y {YAA} {YAB} {YAC} {YAD} {YAF} {YBA} {YBB} {YBC} \
			{YBD} {YBF} -size {tab_xfills+1} -range {tab_xmin} {tab_xmax}
	end
    if (Zp != 0)
        setupalpha {chan} Z {ZAA} {ZAB} {ZAC} {ZAD} {ZAF} {ZBA} {ZBB} {ZBC} \
			{ZBD} {ZBF} -size {tab_xfills+1} -range {tab_xmin} {tab_xmax}
    end

end


/******************************************************************* \
**
**               The current equations themselves 
*********************************************************************/

function make_Purkinje_chans
    int i, cdivs
    float zinf, ztau, c, dc, cmin, cmax
    float x, dx, y
    float a, b

    echo "making Purkinje channel library"
    if (bSaveTables)
	echo "saving the channel tables, set bSaveTables to 0 if this is not what you want"
	echo "Hint : > grep -R bSaveTables ."
    else
	echo "I will not save the channel tables,"
	echo "set bSaveTables to 1 if you want to save the tables."
	echo "Hint : > grep -R bSaveTables ."
    end

/* Equations specific to the Purkinje cell, made by EDS */
    /* Fast Na current, eq#3a */
    make_chan Purk_NaF {ENa} {GNa} 3 35.0e3 0.0 0.0 0.005 -10.0e-3 7.0e3 \
         0.0 0.0 0.065 20.0e-3 1 0.225e3 0.0 1.0 0.080 10.0e-3 7.5e3 0.0 \
         0.0 -0.003 -18.0e-3 0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0

    if (bSaveTables)
	call Purk_NaF TABSAVE Purk_NaF.tab
    end

    /* Persistent Na current, eq#2b */
    make_chan Purk_NaP {ENa} {GNa} 3 200.0e3 0.0 1.0 -0.018 -16.0e-3  \
        25.00e3 0.0 1.0 0.058 8.0e-3 0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0  \
        0.0 0.0 0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0

    if (bSaveTables)
	call Purk_NaP TABSAVE Purk_NaP.tab
    end

    /* P type Ca current, eq#1e */
    make_chan Purk_CaP {ECa} {GCa} 1 8.50e3 0.0 1.0 -0.0080 -12.5e-3  \
        35.0e3 0.0 1.0 0.074 14.5e-3 1 0.0015e3 0.0 1.0 0.029 8.0e-3  \
        0.0055e3 0.0 1.0 0.023 -8.0e-3 0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 \
         0.0 0.0

    if (bSaveTables)
	call Purk_CaP TABSAVE Purk_CaP.tab
    end

    /* T type Ca current, eq#2 */
    make_chan Purk_CaT {ECa} {GCa} 1 2.60e3 0.0 1.0 0.021 -8.0e-3  \
        0.180e3 0.0 1.0 0.040 4.0e-3 1 0.0025e3 0.0 1.0 0.040 8.0e-3  \
        0.190e3 0.0 1.0 0.050 -10.0e-3 0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 \
         0.0 0.0

    if (bSaveTables)
	call Purk_CaT TABSAVE Purk_CaT.tab
    end

    /* A type K current, eq#2b */
    make_chan Purk_KA {EK} {GK} 4 1.40e3 0.0 1.0 0.027 -12.0e-3 0.490e3  \
        0.0 1.0 0.030 4.0e-3 1 0.0175e3 0.0 1.0 0.050 8.0e-3 1.30e3 0.0  \
        1.0 0.013 -10.0e-3 0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0

    if (bSaveTables)
	call Purk_KA TABSAVE Purk_KA.tab
    end

    /* non-inactivating BK-type Ca-dependent K current eq 3 (Gruol K1)
    /* scaled for units: V, sec, mM */
    make_chan Purk_KC {EK} {GK} 1 7.5e3 0.0 0.0 0.0 1.0e12 0.110e3 0.0  \
        0.0 -0.035 14.9e-3 0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0  \
        0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0

    /* Z: Ca-activation, tabchannel specific implementation */
    cmin = ({CCaI}) // will create a z-range from 0.0021 to 0.9727
    cmax = ({Ca_tab_max})
    cdivs = tab_xfills+1
    c = cmin
    dc = cdivs
    dc = (cmax - cmin)/cdivs
    //slow (Gola, Ikemoto:50-280ms delay)
    ztau = 0.010
    call Purk_KC TABCREATE Z {cdivs} {cmin} {cmax}
    for (i = 0; i <= (cdivs); i = i + 1)
	    zinf = 1/(1 + (4.0e-3/c))
	    setfield Purk_KC Z_A->table[{i}] {zinf/ztau}
	    setfield Purk_KC Z_B->table[{i}] {1/ztau}
	    c = c + dc
    end
    setfield Purk_KC Zpower 2
    setfield Purk_KC Z_A->calc_mode 0
    setfield Purk_KC Z_B->calc_mode 0

    if (bSaveTables)
	call Purk_KC TABSAVE Purk_KC.tab
    end

    /* non-inactivating Ca-dependent K current eq 1 (Gruol K2) */
    /* scaled for units: V, sec, mM */
    make_chan Purk_K2 {EK} {GK} 1 25.0e3 0.0 0.0 0.0 1.0e12 0.075e3 0.0  \
        0.0 0.025 6.0e-3 0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0 0.0 \
         0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0
    /* Z: Ca-activation, tabchannel specific implementation */
    cmin = ({CCaI}) // will create a z-range from 0.0021 to 0.9727
    cmax = ({Ca_tab_max})
    cdivs = tab_xfills+1
    c = cmin
    dc = cdivs
    dc = (cmax - cmin)/cdivs
    ztau = 0.010
    call Purk_K2 TABCREATE Z {cdivs} {cmin} {cmax}
    for (i = 0; i <= (cdivs); i = i + 1)
	    zinf = 1/(1 + (0.2e-3/c))
	    setfield Purk_K2 Z_A->table[{i}] {zinf/ztau}
	    setfield Purk_K2 Z_B->table[{i}] {1/ztau}
	    c = c + dc
    end
    setfield Purk_K2 Zpower 2
    setfield Purk_K2 Z_A->calc_mode 0
    setfield Purk_K2 Z_B->calc_mode 0

    if (bSaveTables)
	call Purk_K2 TABSAVE Purk_K2.tab
    end


    /* Equations from the literature */
    /* delayed rectifier type K current eq1
    ** Refs: Yamada (his equations are also for 22C) */
    if (!{exists Purk_Kdr})
	    float VKTAU_OFFSET = 0.000
	    float VKMINF_OFFSET = 0.020
	    create tabchannel Purk_Kdr
	    setfield Purk_Kdr Ek {EK} Gbar {GK} Ik 0 Gk 0 Xpower 2  \
		Ypower 1 Zpower 0

	    call Purk_Kdr TABCREATE X {tab_xdivs} {tab_xmin}  \
		{tab_xmax}
	    x = {tab_xmin}
	    dx = ({tab_xmax} - {tab_xmin})/{tab_xdivs}

	    for (i = 0; i <= ({tab_xdivs}); i = i + 1)
		    a = -23.5e3*(x + 0.012 - VKTAU_OFFSET)/({exp {(x + 0.012 - VKTAU_OFFSET)/-0.012}} - 1.0)
		    b = 5.0e3*{exp {-(x + 0.147 - VKTAU_OFFSET)/0.030}}
		    setfield Purk_Kdr X_A->table[{i}] {1.0/(a + b)}

		    a = -23.5e3*(x + 0.012 - VKMINF_OFFSET)/({exp {(x + 0.012 - VKMINF_OFFSET)/-0.012}} - 1.0)
		    b = 5.0e3*{exp {-(x + 0.147 - VKMINF_OFFSET)/0.030}}
		    setfield Purk_Kdr X_B->table[{i}] {a/(a + b)}
		    x = x + dx
	    end
	    tau_tweak_tabchan Purk_Kdr X
	    setfield Purk_Kdr X_A->calc_mode 0 X_B->calc_mode 0
	    call Purk_Kdr TABFILL X {tab_xfills + 1} 0


	    call Purk_Kdr TABCREATE Y {tab_xdivs} {tab_xmin}  \
		{tab_xmax}
	    x = {tab_xmin}

	    for (i = 0; i <= ({tab_xdivs}); i = i + 1)
		    if (x < -0.025)
			    setfield Purk_Kdr Y_A->table[{i}] 1.2
		    else
			    setfield Purk_Kdr Y_A->table[{i}] 0.010
		    end

		    y = 1.0 + {exp {(x + 0.025)/0.004}}
		    setfield Purk_Kdr Y_B->table[{i}] {1.0/y}
		    x = x + dx
	    end
	    tau_tweak_tabchan Purk_Kdr Y
	    setfield Purk_Kdr Y_A->calc_mode 0 Y_B->calc_mode 0
	    call Purk_Kdr TABFILL Y {tab_xfills + 1} 0
    end

    if (bSaveTables)
	call Purk_Kdr TABSAVE Purk_Kdr.tab
    end

    /* non-inactivating (muscarinic) type K current, eq#2 
    ** eq#2: corrected typo in Yamada equation for tau: 20 instead of 40
    ** Refs: Yamada (his equations are also for 22C) */
    if (!{exists Purk_KM})
	    create tabchannel Purk_KM
	    setfield Purk_KM Ek {EK} Gbar {GK} Ik 0 Gk 0 Xpower 1  \
		Ypower 0 Zpower 0

	    call Purk_KM TABCREATE X {tab_xdivs} {tab_xmin} {tab_xmax}
		x = {tab_xmin}
		dx = ({tab_xmax} - {tab_xmin})/{tab_xdivs}

	    for (i = 0; i <= ({tab_xdivs}); i = i + 1)
		    y = 0.2/(3.3*({exp {(x + 0.035)/0.02}}) + {exp {-(x + 0.035)/0.02}})
		    setfield Purk_KM X_A->table[{i}] {y}

		    y = 1.0/(1.0 + {exp {-(x + 0.035)/0.01}})
		    setfield Purk_KM X_B->table[{i}] {y}
		    x = x + dx
	    end
	    tau_tweak_tabchan Purk_KM X
	    setfield Purk_KM X_A->calc_mode 0 X_B->calc_mode 0
	    call Purk_KM TABFILL X {tab_xfills + 1} 0
    end

    if (bSaveTables)
	call Purk_KM TABSAVE Purk_KM.tab
    end

    /* Anomalous rectifier eq#2: */
    if (!{exists Purk_h1})
	    create tabchannel Purk_h1
	    setfield Purk_h1 Ek {Eh} Gbar {Gh} Ik 0 Gk 0 Xpower 1  \
		Ypower 0 Zpower 0
	    call Purk_h1 TABCREATE X {tab_xdivs} {tab_xmin} {tab_xmax}

	    create tabchannel Purk_h2
	    setfield Purk_h2 Ek {Eh} Gbar {Gh} Ik 0 Gk 0 Xpower 1  \
		Ypower 0 Zpower 0
	    call Purk_h2 TABCREATE X {tab_xdivs} {tab_xmin} {tab_xmax}

	    x = {tab_xmin}
	    dx = ({tab_xmax} - {tab_xmin})/{tab_xdivs}

	    for (i = 0; i <= ({tab_xdivs}); i = i + 1)
		    //fast component
		    setfield Purk_h1 X_A->table[{i}] 0.0076
		    //slow component
		    setfield Purk_h2 X_A->table[{i}] 0.0368
		    y = 1.0/(1 + {exp {(x + 0.082)/0.007}})
		    setfield Purk_h1 X_B->table[{i}] {0.8*y}
		    setfield Purk_h2 X_B->table[{i}] {0.2*y}
		    x = x + dx
	    end
	    tau_tweak_tabchan Purk_h1 X
	    setfield Purk_h1 X_A->calc_mode 0 X_B->calc_mode 0
	    call Purk_h1 TABFILL X {tab_xfills + 1} 0
	    tau_tweak_tabchan Purk_h2 X
	    setfield Purk_h2 X_A->calc_mode 0 X_B->calc_mode 0
	    call Purk_h2 TABFILL X {tab_xfills + 1} 0
    end
    if (bSaveTables)
	call Purk_h1 TABSAVE Purk_h1.tab
    end
    if (bSaveTables)
	call Purk_h2 TABSAVE Purk_h2.tab
    end

end
/*********************************************************************/


end


