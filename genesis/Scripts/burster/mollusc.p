// mollusc.p - cell parameter file for molluscan burster tutorial

//	Format of file :
// x,y,z,dia are in microns, all other units are SI (Meter Kilogram Sec Amp)
// In polar mode 'r' is in microns, theta and phi in degrees 
// Control line options start with a '*'
// The format for each compartment parameter line is :
//name	parent	r	theta	phi	d	ch	dens ...
//in polar mode, and in cartesian mode :
//name	parent	x	y	z	d	ch	dens ...
// For channels, "dens" =  maximum conductance per unit area of compartment
// For spike elements, "dens" is the spike threshold
//		Coordinate mode
*cartesian
*relative

//		Specifying constants
*set_global	RM	4.0
*set_global	RA	0.5
*set_global	CM	0.07

*set_global     EREST_ACT	-0.04

soma  none 250 0 0 250 Na 138 K 66 A 82 Ca 65 B 5 Ca_conc -1000  K_C 124
