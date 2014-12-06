//genesis

/***********************************************************************
**                                                                    **
**  Compartments.g : compartment definition file, neuron builder kit  **
**                                                                    **
**      By E. De Schutter, Nov 1990                                   **
**      Modified June 1990                                            **
**                                                                    **
**	Modified by D. Beeman for GENESIS 2.0,  April 1995            **
************************************************************************/

/* FUNCTIONS TO MAKE DEFAULT LIBRARY COMPARTMENTS */

// NOTE: Previous (GENESIS 1.*) versions assumed that global variables
// for RM, CN, RA and EREST_ACT had been previously declared and set
// This is no longer necessary

function make_cylind_compartment
        // These default compartment parameters can be overridden by readcell
	float RM = 0.33333      // specific membrane resistance (ohms m^2)
	float CM = 0.01         // specific membrane capacitance (farads/m^2)
	float RA = 0.3          // specific axial resistance (ohms m)
	float EREST_ACT = -0.07 // resting membrane potential (volts)
	float	len = 100.0e-6
	float	dia = 2.0e-6
	float PI = 3.14159
	float surface = len * dia * PI

	if (!{exists compartment})
		create	compartment compartment
	end
	setfield compartment \
		Cm		{CM * surface} \		// F
		Ra		{4.0*RA*len / (dia*dia*PI)} \	// ohm
		Em  	{EREST_ACT} \			// V
		Rm		{RM / surface} \ 		// ohm
                dia             {dia} \
		len		{len} \	
		inject		0.0
end

function make_cylind_symcompartment
        // These default compartment parameters can be overridden by readcell
	float RM = 0.33333      // specific membrane resistance (ohms m^2)
	float CM = 0.01         // specific membrane capacitance (farads/m^2)
	float RA = 0.3          // specific axial resistance (ohms m)
	float EREST_ACT = -0.07 // resting membrane potential (volts)
	float	len = 100.0e-6
	float	dia = 2.0e-6
	float PI = 3.14159
	float surface = len * dia * PI

	if (!{exists symcompartment})
		create	symcompartment symcompartment
	end
	setfield symcompartment \
		Cm		{CM * surface} \		// F
		Ra		{4.0*RA*len / (dia*dia*PI)} \	// ohm
		Em  	{EREST_ACT} \			// V
		Rm		{RM / surface} \ 		// ohm
                dia             {dia} \
		len		{len} \	
		inject		0.0

end

/******************************************************************************/
/* These functions are included for compatibility with older neurokit
**  versions, one does not need to call them anymore though! */

function make_sphere_compartment
        // These default compartment parameters can be overridden by readcell
	float RM = 0.33333      // specific membrane resistance (ohms m^2)
	float CM = 0.01         // specific membrane capacitance (farads/m^2)
	float RA = 0.3          // specific axial resistance (ohms m)
	float EREST_ACT = -0.07 // resting membrane potential (volts)
	float	dia = 20.0e-6
	float PI = 3.14159
	float surface = dia * dia * PI

	if (!{exists compartment_sphere})
		create	compartment compartment_sphere
	end
	setfield compartment_sphere \
		Cm		{CM * surface} \	// F
		Ra		{8.0*RA / (dia*PI)} \	// ohm
		Em  	        {EREST_ACT} \			// V
		Rm		{RM / surface} \ 		// ohm
                dia             {dia} \
		len		0.0 \	
		inject		0.0
end

function make_sphere_symcompartment
        // These default compartment parameters can be overridden by readcell
	float RM = 0.33333      // specific membrane resistance (ohms m^2)
	float CM = 0.01         // specific membrane capacitance (farads/m^2)
	float RA = 0.3          // specific axial resistance (ohms m)
	float EREST_ACT = -0.07 // resting membrane potential (volts)
	float	dia = 20.0e-6
	float PI = 3.14159
	float surface = dia * dia * PI

	if (!{exists symcompartment_sphere})
		create	symcompartment symcompartment_sphere
	end
	setfield symcompartment_sphere \
		Cm		{CM * surface} \		// F
		Ra		{8.0*RA / (dia*PI)} \	// ohm
		Em  	        {EREST_ACT} \			// V
		Rm		{RM / surface} \ 		// ohm
                dia             {dia} \
		len		0.0   \	
		inject		0.0
end
