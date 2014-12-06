//genesis
/* 
	functions defined in this script
=============================================================================
	FUNCTION NAME		ARGUMENTS
=============================================================================
	makecompartment		(path,l,d,Eleak)
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
    // Kohm-cm^2
    float rm = 10.0
    // uF/cm^2
    float cm = 1.0
    // Kohm-cm
    float ra = 0.05

    create compartment {path}
    // mV
    // Kohm
    // uF
    // Kohm
    setfield {path} Em {Erest} Rm {rm/area} Cm {cm*area} Ra {ra*l/xarea}
end
