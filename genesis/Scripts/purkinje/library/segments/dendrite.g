float len = 200.00e-6
float dia = 2.00e-6
float surf = len*dia*{PI}

create compartment /library/dendrite

setfield /library/dendrite \
	Cm {{CM}*surf} \
	Ra {4.0*{RA}*len/(dia*dia*{PI})} \
        Em {ELEAK} \
	initVm {EREST_ACT} \
	Rm {{RMd}/surf} \
	inject 0.0 \
        dia {dia} \
	len {len}

