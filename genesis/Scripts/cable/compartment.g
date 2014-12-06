//genesis
/* compartment.g

	functions defined in this script
=============================================================================
	FUNCTION NAME		ARGUMENTS
=============================================================================
	makecompartment		(path,l,d,Erest)
	setcompartment		(path,l,d,Erest)
=============================================================================
*/

//=================================================
//                    COMPARTMENT
//=================================================
function makecompartment(path, l, d, Erest)
    str path
    float l, d
    float Erest
    float area = l*{PI}*d
    float xarea = {PI}*d*d/4
    /* global variables defined in cable.g
    float RM				// Kohm-cm^2
    float CM				// uF/cm^2
    float RA				// Kohm-cm
*/
    create compartment {path}
    // mV
    // Kohm
    // uF
    // Kohm
    setfield {path} Em {Erest} Rm {{RM}/area} Cm {{CM}*area}  \
        Ra {{RA}*l/xarea}
end

function setcompartment(path, l, d, Erest)
    str path
    float l, d
    float Erest
    float area = l*{PI}*d
    float xarea = {PI}*d*d/4
    // mV
    // Kohm
    // uF
    // Kohm
    setfield {path} Em {Erest} Rm {{RM}/area} Cm {{CM}*area}  \
        Ra {{RA}*l/xarea}
end
