//genesis - Purkinje cell M9 genesis2.1 script
/* Copyright E. De Schutter (Caltech and BBF-UIA) */

/**********************************************************************
** Sets up active membrane Purkinje cell compartment prototypes 
**  and dendritic spine prototypes.
** E. De Schutter, Caltech, 1991-1992
** This version sets up active compartments with basket cell, climbing
**  fiber and stellate cell synapses.
**********************************************************************/

/* References:
** E. De Schutter and J.M. Bower: An active membrane model of the
** cerebellar Purkinje cell. I. Simulation of current clamps in slice.
** Journal of Neurophysiology  71: 375-400 (1994).
** http://www.tnb.ua.ac.be/TNB/TNB_pub8.html
** E. De Schutter and J.M. Bower: An active membrane model of the
** cerebellar Purkinje cell: II. Simulation of synaptic responses.
** Journal of Neurophysiology  71: 401-419 (1994).
** http://www.tnb.ua.ac.be/TNB/TNB_pub7.html
** Consult these references for sources of experimental data.
*/


int include_Purk_cicomp

if ( {include_Purk_cicomp} == 0 )

	include_Purk_cicomp = 1


function make_Purkinje_comps
/* separate function so we can have local variables */

	float len, dia, surf, shell_vol, shell_dia
	int i

	echo making Purkinje compartment library...

	/* make spherical soma prototype with sodium currents*/
	len = 0.00e-6
	dia = 20.0e-6
	surf = dia*dia*{PI}
	shell_dia = 2*{Shell_thick}
	shell_vol = (3*dia*dia*shell_dia - 3*dia*shell_dia*shell_dia + shell_dia*shell_dia*shell_dia)*{PI}/6.0
	if (!({exists Purk_soma}))
		create compartment Purk_soma
	end
	setfield Purk_soma Cm {{CM}*surf} Ra {8.0*{RA}/(dia*{PI})}  \
	    Em {ELEAK} initVm {EREST_ACT} Rm {{RMs}/surf} inject 0.0  \
	    dia {dia} len {len}

	copy Purk_NaF Purk_soma/NaF
	addmsg Purk_soma Purk_soma/NaF VOLTAGE Vm
	addmsg Purk_soma/NaF Purk_soma CHANNEL Gk Ek
	setfield Purk_soma/NaF Gbar {{GNaFs}*surf}
	copy Purk_NaP Purk_soma/NaP
	addmsg Purk_soma Purk_soma/NaP VOLTAGE Vm
	addmsg Purk_soma/NaP Purk_soma CHANNEL Gk Ek
	setfield Purk_soma/NaP Gbar {{GNaPs}*surf}
	copy Purk_CaT Purk_soma/CaT
	addmsg Purk_soma Purk_soma/CaT VOLTAGE Vm
	addmsg Purk_soma/CaT Purk_soma CHANNEL Gk Ek
	setfield Purk_soma/CaT Gbar {{GCaTs}*surf}
	copy Purk_KA Purk_soma/KA
	addmsg Purk_soma Purk_soma/KA VOLTAGE Vm
	addmsg Purk_soma/KA Purk_soma CHANNEL Gk Ek
	setfield Purk_soma/KA Gbar {{GKAs}*surf}
	copy Purk_Kdr Purk_soma/Kdr
	addmsg Purk_soma Purk_soma/Kdr VOLTAGE Vm
	addmsg Purk_soma/Kdr Purk_soma CHANNEL Gk Ek
	setfield Purk_soma/Kdr Gbar {{GKdrs}*surf}
	copy Purk_KM Purk_soma/KM
	addmsg Purk_soma Purk_soma/KM VOLTAGE Vm
	addmsg Purk_soma/KM Purk_soma CHANNEL Gk Ek
	setfield Purk_soma/KM Gbar {{GKMs}*surf}
	copy Purk_h1 Purk_soma/h1
	addmsg Purk_soma Purk_soma/h1 VOLTAGE Vm
	addmsg Purk_soma/h1 Purk_soma CHANNEL Gk Ek
	setfield Purk_soma/h1 Gbar {{Ghs}*surf}
	copy Purk_h2 Purk_soma/h2
	addmsg Purk_soma Purk_soma/h2 VOLTAGE Vm
	addmsg Purk_soma/h2 Purk_soma CHANNEL Gk Ek
	setfield Purk_soma/h2 Gbar {{Ghs}*surf}
	create Ca_concen Purk_soma/Ca_pool
	setfield Purk_soma/Ca_pool tau {CaTau}  \
	    B {1.0/(2.0*96494*shell_vol)} Ca_base {CCaI}  \
	    thick {Shell_thick}
	addmsg Purk_soma/CaT Purk_soma/Ca_pool I_Ca Ik
	copy GABA2 Purk_soma/basket
	setfield Purk_soma/basket gmax    {{GB_GABA}*surf}
	addmsg Purk_soma/basket  Purk_soma  CHANNEL Gk  Ek
	addmsg Purk_soma  Purk_soma/basket    VOLTAGE Vm

    /* make main dendrite prototype with fast Ca currents */
    len = 200.00e-6
    dia = 2.00e-6
    surf = len*dia*{PI}
    shell_vol = (2*dia*shell_dia - shell_dia*shell_dia)*len*{PI}/4.0
    if (!({exists Purk_maind}))
        create compartment Purk_maind
    end
    setfield Purk_maind Cm {{CM}*surf} Ra {4.0*{RA}*len/(dia*dia*{PI})}  \
        Em {ELEAK} initVm {EREST_ACT} Rm {{RMd}/surf} inject 0.0  \
        dia {dia} len {len}

    if (!({exists Purk_thickd}))    // Lets copy before we add currents

        copy Purk_maind Purk_thickd
    end
    if (!({exists Purk_spinyd}))    // Lets copy before we add currents

        copy Purk_maind Purk_spinyd
    end

	copy Purk_CaT Purk_maind/CaT
	addmsg Purk_maind Purk_maind/CaT VOLTAGE Vm
	addmsg Purk_maind/CaT Purk_maind CHANNEL Gk Ek
	setfield Purk_maind/CaT Gbar {{GCaTm}*surf}
	copy Purk_KA Purk_maind/KA
	addmsg Purk_maind Purk_maind/KA VOLTAGE Vm
	addmsg Purk_maind/KA Purk_maind CHANNEL Gk Ek
	setfield Purk_maind/KA Gbar {{GKAm}*surf}
	copy Purk_Kdr Purk_maind/Kdr
	addmsg Purk_maind Purk_maind/Kdr VOLTAGE Vm
	addmsg Purk_maind/Kdr Purk_maind CHANNEL Gk Ek
	setfield Purk_maind/Kdr Gbar {{GKdrm}*surf}
	copy Purk_KM Purk_maind/KM
	addmsg Purk_maind Purk_maind/KM VOLTAGE Vm
	addmsg Purk_maind/KM Purk_maind CHANNEL Gk Ek
	setfield Purk_maind/KM Gbar {{GKMm}*surf}
	copy Purk_CaP Purk_maind/CaP
	addmsg Purk_maind Purk_maind/CaP VOLTAGE Vm
	addmsg Purk_maind/CaP Purk_maind CHANNEL Gk Ek
	setfield Purk_maind/CaP Gbar {{GCaPm}*surf}
	copy Purk_KC Purk_maind/KC
	addmsg Purk_maind Purk_maind/KC VOLTAGE Vm
	addmsg Purk_maind/KC Purk_maind CHANNEL Gk Ek
	setfield Purk_maind/KC Gbar {{GKCm}*surf}
	copy Purk_K2 Purk_maind/K2
	addmsg Purk_maind Purk_maind/K2 VOLTAGE Vm
	addmsg Purk_maind/K2 Purk_maind CHANNEL Gk Ek
	setfield Purk_maind/K2 Gbar {{GK2m}*surf}
	create Ca_concen Purk_maind/Ca_pool
	setfield Purk_maind/Ca_pool tau {CaTau}  \
	    B {1.0/(2.0*96494*shell_vol)} Ca_base {CCaI}  \
	    thick {Shell_thick}
	addmsg Purk_maind/CaT Purk_maind/Ca_pool I_Ca Ik
	addmsg Purk_maind/CaP Purk_maind/Ca_pool I_Ca Ik
	addmsg Purk_maind/Ca_pool Purk_maind/KC CONCEN Ca
	addmsg Purk_maind/Ca_pool Purk_maind/K2 CONCEN Ca
	create nernst Purk_maind/Ca_nernst
	setfield Purk_maind/Ca_nernst Cin {CCaI} Cout {CCaO} valency {2} \
	     scale {1.0} T {37}
	addmsg Purk_maind/Ca_pool Purk_maind/Ca_nernst CIN Ca
	addmsg Purk_maind/Ca_nernst Purk_maind/CaP EK E
	addmsg Purk_maind/Ca_nernst Purk_maind/CaT EK E
	copy non_NMDA2 Purk_maind/climb
	setfield Purk_maind/climb gmax    {{G_cli_syn}*surf/2}
	addmsg Purk_maind/climb  Purk_maind  CHANNEL Gk  Ek
	addmsg Purk_maind  Purk_maind/climb    VOLTAGE Vm
	copy GABA2 Purk_maind/basket
	setfield Purk_maind/basket gmax    {{GB_GABA}*surf/2}
	addmsg Purk_maind/basket  Purk_maind  CHANNEL Gk  Ek
	addmsg Purk_maind  Purk_maind/basket    VOLTAGE Vm


	/* make thick dendrite prototype with fast Ca current */
	/* similar to main dendrite but has CaP and no Kdr */
	/* passive prototype already copied from Purk_maind */
	copy Purk_CaT Purk_thickd/CaT
	addmsg Purk_thickd Purk_thickd/CaT VOLTAGE Vm
	addmsg Purk_thickd/CaT Purk_thickd CHANNEL Gk Ek
	setfield Purk_thickd/CaT Gbar {{GCaTt}*surf}
	copy Purk_CaP Purk_thickd/CaP
	addmsg Purk_thickd Purk_thickd/CaP VOLTAGE Vm
	addmsg Purk_thickd/CaP Purk_thickd CHANNEL Gk Ek
	setfield Purk_thickd/CaP Gbar {{GCaPt}*surf}
	copy Purk_KM Purk_thickd/KM
	addmsg Purk_thickd Purk_thickd/KM VOLTAGE Vm
	addmsg Purk_thickd/KM Purk_thickd CHANNEL Gk Ek
	setfield Purk_thickd/KM Gbar {{GKMt}*surf}
	copy Purk_KC Purk_thickd/KC
	addmsg Purk_thickd Purk_thickd/KC VOLTAGE Vm
	addmsg Purk_thickd/KC Purk_thickd CHANNEL Gk Ek
	setfield Purk_thickd/KC Gbar {{GKCt}*surf}
	copy Purk_K2 Purk_thickd/K2
	addmsg Purk_thickd Purk_thickd/K2 VOLTAGE Vm
	addmsg Purk_thickd/K2 Purk_thickd CHANNEL Gk Ek
	setfield Purk_thickd/K2 Gbar {{GK2t}*surf}
	create Ca_concen Purk_thickd/Ca_pool
	setfield Purk_thickd/Ca_pool tau {CaTau}  \
	    B {1.0/(2.0*96494*shell_vol)} Ca_base {CCaI}  \
	    thick {Shell_thick}
	addmsg Purk_thickd/CaT Purk_thickd/Ca_pool I_Ca Ik
	addmsg Purk_thickd/CaP Purk_thickd/Ca_pool I_Ca Ik
	addmsg Purk_thickd/Ca_pool Purk_thickd/KC CONCEN Ca
	addmsg Purk_thickd/Ca_pool Purk_thickd/K2 CONCEN Ca
	create nernst Purk_thickd/Ca_nernst
	setfield Purk_thickd/Ca_nernst Cin {CCaI} Cout {CCaO}  \
	    valency {2} scale {1.0} T {37}
	addmsg Purk_thickd/Ca_pool Purk_thickd/Ca_nernst CIN Ca
	addmsg Purk_thickd/Ca_nernst Purk_thickd/CaP EK E
	addmsg Purk_thickd/Ca_nernst Purk_thickd/CaT EK E
	for (i=1; i<=2; i = i+1)
	    copy GABA Purk_thickd/stell{i}
	    setfield Purk_thickd/stell{i} gmax {{G_GABA}/5*surf}
	    addmsg Purk_thickd/stell{i} Purk_thickd CHANNEL Gk Ek
		addmsg Purk_thickd  Purk_thickd/stell{i}  VOLTAGE Vm
	end
	copy non_NMDA2 Purk_thickd/climb
	setfield Purk_thickd/climb gmax    {{G_cli_syn}*surf}
	addmsg Purk_thickd/climb  Purk_thickd  CHANNEL Gk  Ek
	addmsg Purk_thickd  Purk_thickd/climb    VOLTAGE Vm

	/* make spiny dendrite prototype with high threshold Ca current  \
	    */
	/* passive prototype already copied from Purk_maind */
	copy Purk_CaT Purk_spinyd/CaT
	addmsg Purk_spinyd Purk_spinyd/CaT VOLTAGE Vm
	addmsg Purk_spinyd/CaT Purk_spinyd CHANNEL Gk Ek
	setfield Purk_spinyd/CaT Gbar {{GCaTd}*surf}
	copy Purk_CaP Purk_spinyd/CaP
	addmsg Purk_spinyd Purk_spinyd/CaP VOLTAGE Vm
	addmsg Purk_spinyd/CaP Purk_spinyd CHANNEL Gk Ek
	setfield Purk_spinyd/CaP Gbar {{GCaPd}*surf}
	copy Purk_KM Purk_spinyd/KM
	addmsg Purk_spinyd Purk_spinyd/KM VOLTAGE Vm
	addmsg Purk_spinyd/KM Purk_spinyd CHANNEL Gk Ek
	setfield Purk_spinyd/KM Gbar {{GKMd}*surf}
	copy Purk_KC Purk_spinyd/KC
	addmsg Purk_spinyd Purk_spinyd/KC VOLTAGE Vm
	addmsg Purk_spinyd/KC Purk_spinyd CHANNEL Gk Ek
	setfield Purk_spinyd/KC Gbar {{GKCd}*surf}
	copy Purk_K2 Purk_spinyd/K2
	addmsg Purk_spinyd Purk_spinyd/K2 VOLTAGE Vm
	addmsg Purk_spinyd/K2 Purk_spinyd CHANNEL Gk Ek
	setfield Purk_spinyd/K2 Gbar {{GK2d}*surf}
	create Ca_concen Purk_spinyd/Ca_pool
	setfield Purk_spinyd/Ca_pool tau {CaTau}  \
	    B {1.0/(2.0*96494*shell_vol)} Ca_base {CCaI}  \
	    thick {Shell_thick}
	addmsg Purk_spinyd/CaP Purk_spinyd/Ca_pool I_Ca Ik
	addmsg Purk_spinyd/CaT Purk_spinyd/Ca_pool I_Ca Ik
	addmsg Purk_spinyd/Ca_pool Purk_spinyd/KC CONCEN Ca
	addmsg Purk_spinyd/Ca_pool Purk_spinyd/K2 CONCEN Ca
	create nernst Purk_spinyd/Ca_nernst
	setfield Purk_spinyd/Ca_nernst Cin {CCaI} Cout {CCaO}  \
	    valency {2} scale {1.0} T {37}
	addmsg Purk_spinyd/Ca_pool Purk_spinyd/Ca_nernst CIN Ca
	addmsg Purk_spinyd/Ca_nernst Purk_spinyd/CaP EK E
	addmsg Purk_spinyd/Ca_nernst Purk_spinyd/CaT EK E
	copy GABA Purk_spinyd/stell
	setfield Purk_spinyd/stell gmax {{G_GABA}*surf}
	addmsg Purk_spinyd/stell Purk_spinyd CHANNEL Gk Ek
	addmsg Purk_spinyd  Purk_spinyd/stell  VOLTAGE Vm
end

/*********************************************************************/
function make_Purkinje_spines
/* separate function so we can have local variables */
/* one standard format, may be rescaled in attach_spines */
/* Reference: Harris KM, Stevens JK: Dendritic spines of rat
**  cerebellar Purkinje cells: serial electron microscopy with
**  reference to their biophysical characteristics.  J Neurosci 8: p
**  4455-4469, 1988. */

	float len, dia, surf, vol

    /* make spine with average length and diameter */
	/* make spine neck */
	len = 0.66e-6
	dia = 0.20e-6
	surf = len*dia*{PI}
	vol = len*dia*dia*{PI}/4.0
	if (!({exists Purk_spine}))
		create compartment Purk_spine
	end
	setfield Purk_spine Cm {{CM}*surf}  \
	    Ra {4.0*{RA}*len/(dia*dia*{PI})} Em {ELEAK}  \
	    initVm {EREST_ACT} Rm {{RMd}/surf} inject 0.0 x {len} dia {dia} \
		len {len}

	/* make spine head */
	len = 0.00e-6
	// derived from average surface
	dia = 0.54e-6
	surf = dia*dia*{PI}
	create compartment Purk_spine/head
	setfield Purk_spine/head Cm {{CM}*surf} Ra {8.0*{RA}/(dia*{PI})} \
	     Em {ELEAK} initVm {EREST_ACT} Rm {{RMd}/surf} inject 0.0  \
	    x {dia} dia {dia} len {len}

	/* make a combined prototype: neck+head */
	addmsg Purk_spine/head Purk_spine RAXIAL Ra Vm
	addmsg Purk_spine Purk_spine/head AXIAL Vm

	copy Purk_spine Purk_spine2

	/* put non-NMDA channel on combined prototype */
	copy non_NMDA Purk_spine/head/par
	setfield Purk_spine/head/par gmax {{G_par_syn}*surf}
	addmsg  Purk_spine/head  Purk_spine/head/par	VOLTAGE Vm
	addmsg Purk_spine/head/par Purk_spine/head CHANNEL Gk Ek

	/* put non-NMDA channel on combined prototype */
	copy non_NMDA2 Purk_spine2/head/par2
	setfield Purk_spine2/head/par2 gmax {{G_par_syn}*surf}
	addmsg  Purk_spine2/head  Purk_spine2/head/par2	VOLTAGE Vm
	addmsg Purk_spine2/head/par2 Purk_spine2/head CHANNEL Gk Ek

end
/*********************************************************************/


end


