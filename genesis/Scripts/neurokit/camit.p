// coarse asymmetrical mitral cell (olf bulb) model : camit
// Control lines start with '*'. Valid control options are 
// *relative 			- relative coords.
// *absolute			- absolute coords.
// *asymmetric			- use asymmetric compartments
// *symmetric			- use symmetric compartments

// #	name	parent		x	y	z	d	ch	dens	ch	dens...
// *hsolve

*asymmetric
*absolute

*set_global	RM	4.0
*set_global	RA	0.5
*set_global	CM	0.01

soma		none		0	0	28	19	Na_mit_hh	0.3e3	K_mit_hh	0.8e3

*set_global	RM	2.0
*set_global	RA	0.5
*set_global	CM	0.01

primary_dend	soma		0	0	660	7	Na_mit_hh	60	K_mit_hh	100

*relative

glom1		primary_dend	40	40	60	1	Na_mit_hh	60	K_mit_hh	100	glu_mit_upi	246	GABA_mit_upi	357
glom2		primary_dend	-40	40	60	1	Na_mit_hh	60	K_mit_hh	100	glu_mit_upi	246	GABA_mit_upi	357
glom3		primary_dend	-40	-40	60	1	Na_mit_hh	60	K_mit_hh	100	glu_mit_upi	246	GABA_mit_upi	357
glom4		primary_dend	40	-40	60	1	Na_mit_hh	60	K_mit_hh	100	glu_mit_upi	246	GABA_mit_upi	357

second_dend1	soma		0	200	40	4.0	Na_mit_hh	60	K_mit_hh	90	glu_mit_upi	246	GABA_mit_upi	357
second_dend11	second_dend1	200	600	60	3.0	Na_mit_hh	60	K_mit_hh	90	glu_mit_upi	246	GABA_mit_upi	357
second_dend12	second_dend1	-200	600	60	3.0	Na_mit_hh	60	K_mit_hh	90	glu_mit_upi	246	GABA_mit_upi	357



second_dend2	soma		0	-200	40	4.0	Na_mit_hh	60	K_mit_hh	90	glu_mit_upi	246	GABA_mit_upi	357
second_dend21	second_dend2	200	-600	60	3.0	Na_mit_hh	60	K_mit_hh	90	glu_mit_upi	246	GABA_mit_upi	357
second_dend22	second_dend2	-200	-600	60	3.0	Na_mit_hh	60	K_mit_hh	90	glu_mit_upi	246	GABA_mit_upi	357


second_dend3	soma		200	0	40	4.0	Na_mit_hh	60	K_mit_hh	90	glu_mit_upi	246	GABA_mit_upi	357
second_dend31	second_dend3	600	200	60	3.0	Na_mit_hh	60	K_mit_hh	90	glu_mit_upi	246	GABA_mit_upi	357
second_dend32	second_dend3	600	-200	60	3.0	Na_mit_hh	60	K_mit_hh	90	glu_mit_upi	246	GABA_mit_upi	357


second_dend4	soma		-200	0	40	4.0	Na_mit_hh	60	K_mit_hh	90	glu_mit_upi	246	GABA_mit_upi	357
second_dend41	second_dend4	-600	200	60	3.0	Na_mit_hh	60	K_mit_hh	90	glu_mit_upi	246	GABA_mit_upi	357
second_dend42	second_dend4	-600	-200	60	3.0	Na_mit_hh	60	K_mit_hh	90	glu_mit_upi	246	GABA_mit_upi	357
