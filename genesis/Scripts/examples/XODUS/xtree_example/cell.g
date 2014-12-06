//genesis

/* here is some stuff for setting up the cell that the example reads */

float RA = 0.5
float CM = 0.01
float RM = 0.2
float EREST_ACT = -0.070

int EXPONENTIAL = 1
int SIGMOID = 2
int LINOID = 3

include hhchan.g

create neutral /library
create compartment /library/compartment
	setfield ^ \
		Cm	1 \
		Ra	1 \
		Em {EREST_ACT} \
		Rm 	1 \
		dia 1 \
		inject 0.0

pushe /library
	make_Na_squid_hh
	make_K_squid_hh
pope
readcell tree_cell.p /cell
