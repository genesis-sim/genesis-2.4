//genesis

/***********************************************************************
**                                                                    **
**   Spiny.g : compartment definition file, neuron builder kit        **
**                                                                    **
**      By U.S. Bhalla March 1991                                     **
***********************************************************************/

/* FUNCTIONS TO MAKE DEFAULT LIBRARY COMPARTMENTS */
/* Note the use of environment variables Len(gth) and Dia(meter)
**	these should always be present in /library definitions of
**	compartments, otherwise scaling of conductance_Gbars and 
**	pool_volumes in .p-files may not be correct and the
**	*spherical options in the parsing routines will not work! 
** A Len(gth) == 0.0 makes the (sym)compartment spherical to the
**	parsing routines, if you want to use the *spherical
**	routines the library must contain prototypes for spherical
**	compartments.  They should be named ..._sphere.
** Note also that all copies of these library compartments
**	keep the original Len and Dia values */

/* In GENESIS 2.0, the environment variables Len and Dia are replaced by
   the compartment fields len and dia.  Older scripts which use these
   functions should be changed to use len and dia instead of Len and Dia */

function internal_make_spiny(compt, clen, cdia, nlen, ndia, hlen, hdia)
	str compt
	str clen, cdia, nlen, ndia, hlen, hdia

	float len = (clen)
	float dia = (cdia)
	float surface = len*dia*{PI}
	float temp

	if (!({exists {compt}}))
		create compartment {compt}
		create compartment {compt}/neck
		create compartment {compt}/head

		addmsg {compt} {compt}/neck AXIAL Vm
		addmsg {compt}/neck {compt} RAXIAL Ra Vm

		addmsg {compt}/neck {compt}/head AXIAL Vm
		addmsg {compt}/head {compt}/neck RAXIAL Ra Vm

		setfield {compt} Cm {{CM}*surface}  \
		    Ra {4.0*{RA}*len/(dia*dia*{PI})} Em {EREST_ACT}  \
		    Rm {{RM}/surface} inject 0.0
		setfield {compt} \
                        len             {clen}   \      // m, >0 is cylinder
                        dia             {cdia}          // m
		len = (nlen)
		dia = (ndia)
		surface = len*dia*{PI}
		setfield {compt}/neck Cm {{CM}*surface}  \
		    Ra {4.0*{RA}*len/(dia*dia*{PI})} Em {EREST_ACT}  \
		    Rm {{RM}/surface} inject 0.0 x {len} y 0

		temp = len
		len = (hlen)
		dia = (hdia)
		surface = len*dia*{PI}
		setfield {compt}/head Cm {{CM}*surface}  \
		    Ra {4.0*{RA}*len/(dia*dia*{PI})} Em {EREST_ACT}  \
		    Rm {{RM}/surface} inject 0.0 x {len + temp} y 0

	end
end

function make_spiny_compartment
	internal_make_spiny spiny 10e-6 1e-6 1e-6 0.2e-6 3e-6 1e-6
end

function make_spiny2_compartment
	internal_make_spiny spiny2 10e-6 1e-6 1e-6 0.3e-6 2e-6 1e-6
end
