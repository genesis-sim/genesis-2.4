//genesis
/* compartment.g

	functions defined in this script
=============================================================================
	FUNCTION NAME		ARGUMENTS
=============================================================================
	makecompartment		(path,l,d,Erest)
=============================================================================
*/

//=================================================
//                    COMPARTMENT
//=================================================
function makecompartment(path, l, d, Erest)
    str path
    float l, d
    float Erest
    float area = l*PI*d
    float xarea = PI*d*d/4
    /* global variables defined in constants.g
    float rm				// Kohm-cm^2
    float cm				// uF/cm^2
    float ra				// Kohm-cm
*/
    create compartment {path}
    // mV
    // Kohm
    // uF
    // Kohm
    setfield {path} Em {Erest} Rm {rm/area} Cm {cm*area} Ra {ra*l/xarea}
end
