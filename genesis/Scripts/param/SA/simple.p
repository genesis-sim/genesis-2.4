// simple.p: test cell for parameter searches
// Control lines start with '*'. Valid control options are 
// *relative 			- relative coords.
// *absolute			- absolute coords.
// *asymmetric			- use asymmetric compartments
// *symmetric			- use symmetric compartments

// #	name	parent		x	y	z	d     ch dens  ch dens...

*cartesian
*symmetric
*absolute

*set_compt_param	RM		2.0
*set_compt_param	RA		6.0
*set_compt_param	CM		0.008
*set_compt_param     EREST_ACT      -0.077

*compt /library/compt
*spherical

soma		none		0	0	0 	20        \
		Na_hip_traub91          800.0                     \
		Kdr_hip_traub91         100.0                     \
		Ka_hip_traub91           50.0                     \
		KM_bsg_yka              100.0

