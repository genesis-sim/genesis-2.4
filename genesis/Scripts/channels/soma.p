// soma model for looking at channels
// Control lines start with '*'. Valid control options are 
// *relative 			- relative coords.
// *absolute			- absolute coords.
// *asymmetric			- use asymmetric compartments
// *symmetric			- use symmetric compartments

// #	name	parent		x	y	z	d	ch	dens	ch	dens...

*asymmetric
*absolute

*set_global	RM	4.0
*set_global	RA	0.5
*set_global	CM	0.01

soma		none		0	0	28	19
