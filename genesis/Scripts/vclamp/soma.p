//	PARAMETER FILE FOR NEURON 'soma'
//	Author : Upi Bhalla 


//	Format of file :
// x,y,z,dia are in microns, all other units are SI (Meter Kilogram Second Amp)
// In polar mode 'r' is in microns, theta and phi in degrees 
// Control line options start with a '*'
// The format for each compartment parameter line is :
//name	parent	r	theta	phi	d	ch	dens ...
//in polar mode, and in cartesian mode :
//name	parent	x	y	z	d	ch	dens ...


//		Coordinate mode
*cartesian
*relative

//		Specifying constants
*set_global	RM	4
*set_global	RA	0.5
*set_global	CM	0.01
*set_global	EREST_ACT	-0.065

soma	none	28	0	0	19	NCa_drg_fnt 1
