//genesis - Purkinje cell M9 genesis2.1 script
/* Copyright E. De Schutter (Caltech and BBF-UIA) */

/**********************************************************************
** Sets of synapse objects developed for rat cerebellum Purkinje
** E. De Schutter, Caltech, 1991-1992
**********************************************************************/

/* Reference:
** E. De Schutter and J.M. Bower: An active membrane model of the
** cerebellar Purkinje cell: II. Simulation of synaptic responses.
** Journal of Neurophysiology  71: 401-419 (1994).
** http://www.tnb.ua.ac.be/TNB/TNB_pub7.html
** Consult this reference for sources of experimental data.
*/

// CONSTANTS
/* should be defined by calling routine (all correctly scaled):
**	E_non_NMDA
**	E_GABA, G_GABA */

int include_Purk_syn

if ( {include_Purk_syn} == 0 )

	include_Purk_syn = 1


/*********************************************************************
**               The synaptic conductance equations 
*********************************************************************/

function make_Purkinje_syns

echo making Purkinje synapse library...

/* The conductance equations in this library are general and not
**  specific to the Purkinje cell */

/* GABA channel, made by EDS */
/* Reference: voltage clamp data from 
** Miles R: . J Physiol 1990.
** Tpeak: 3.25 ms, Tdecay = 28 ms */

	/* asynchronously firing channel */
    if (!({exists GABA}))
		create synchan GABA
    end
    setfield GABA Ek {E_GABA} tau1 {0.93e-3} tau2 {26.50e-3}  \
        gmax {G_GABA} frequency {0.0}

	/* synchronously firing channel */
    if (!({exists GABA2}))
		create synchan GABA2
    end
    setfield GABA2 Ek {E_GABA} tau1 {0.93e-3} tau2 {26.50e-3}  \
        gmax {GB_GABA}

/* non-NMDA channel, made by EDS */
/* Reference: based on data in Holmes WR, Levy WB: Insights
**  into associative long-term potentiation from computational models
**  of NMDA-receptor-mediated calcium influx and intracellular calcium
**  concentration changes.  J Neurophysiol 63, p 1148-1168, 1990. */

	/* asynchronously firing channel */
    if (!({exists non_NMDA}))
		create synchan non_NMDA
    end

//     setfield non_NMDA Ek {E_non_NMDA} tau1 {2.60e-3} tau2 {63.00e-3}
    setfield non_NMDA Ek {E_non_NMDA} tau1 {0.50e-3} tau2 {1.20e-3}  \
        frequency {0.0}

	/* synchronously firing channel */
    if (!({exists non_NMDA2}))
        create synchan non_NMDA2
    end

    setfield non_NMDA2 Ek {E_non_NMDA} tau1 {0.50e-3} tau2 {1.20e-3}

end


end


