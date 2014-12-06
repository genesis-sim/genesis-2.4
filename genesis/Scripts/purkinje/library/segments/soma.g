float CM = 0.0164 		// *9 relative to passive
float RMs = 1.000 		// /3.7 relative to passive comp
float RMd = 3.0
float RA = 2.50

float ELEAK = -0.0800
float EREST_ACT = -0.0680

float len = 0.00e-6
float dia = 20.0e-6
float surf = dia*dia*{PI}

create compartment /library/soma

setfield /library/soma \
	Cm {{CM}*surf} \
	Ra {8.0*{RA}/(dia*{PI})} \
	Em {ELEAK} \
	initVm {EREST_ACT} \
	Rm {{RMs}/surf} \
	inject 0.0 \
	dia {dia} \
	len {len}

