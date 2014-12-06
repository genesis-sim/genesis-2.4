//genesis

//==============================================================
//  Hodgkin-Huxley prototypes for squid giant axon channels 

//  From H & H (1952) J Physiol 117, 500-544.

//  RESTING VOLTAGE is assumed to be EREST_ACT.
//  SIGN CONVENTION for voltage has been flipped to agree with
//  modern convention (depolarization +, hyperpolarization -)

//  M. Nelson  Caltech  4/89
//==============================================================


if (!{exists /proto/hh_channels})
    create neutral /proto/hh_channels
end
ce /proto/hh_channels


//====================
//  Squid Na channel
//====================


create vdep_channel Na_squid

create vdep_gate Na_squid/m
	setfield ^ alpha_A {0.1*(25.0 + EREST_ACT)} alpha_B -0.1  \
	    alpha_C -1.0 alpha_D {-25.0 - EREST_ACT} alpha_F -10.0  \
	    beta_A 4.0 beta_B 0.0 beta_C 0.0 beta_D {0.0 - EREST_ACT}  \
	    beta_F 18.0 instantaneous 0

create vdep_gate Na_squid/h
	setfield ^ alpha_A 0.07 alpha_B 0.0 alpha_C 0.0  \
	    alpha_D {0.0 - EREST_ACT} alpha_F 20.0 beta_A 1.0 beta_B 0.0 \
	     beta_C 1.0 beta_D {-30.0 - EREST_ACT} beta_F -10.0  \
	    instantaneous 0

addmsg Na_squid/m Na_squid MULTGATE m 3
addmsg Na_squid/h Na_squid MULTGATE m 1



//===================
//  Squid K channel
//===================


create vdep_channel K_squid

create vdep_gate K_squid/n
	setfield ^ alpha_A {0.01*(10.0 + EREST_ACT)} alpha_B -0.01  \
	    alpha_C -1.0 alpha_D {-10.0 - EREST_ACT} alpha_F -10.0  \
	    beta_A 0.125 beta_B 0.0 beta_C 0.0 beta_D {0.0 - EREST_ACT}  \
	    beta_F 80.0 instantaneous 0

addmsg K_squid/n K_squid MULTGATE m 4
