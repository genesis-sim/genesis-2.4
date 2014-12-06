// cell.p - Cell parameter file used in Tutorial #5

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
*relative
*cartesian
*asymmetric

//		Specifying constants
*set_compt_param	RM	0.33333
*set_compt_param	RA	0.3
*set_compt_param	CM	0.01
*set_compt_param     EREST_ACT	-0.07

// For the soma, use the leakage potential (-0.07 + 0.0106) for Em
*set_compt_param     ELEAK	-0.0594
soma  none   30  0  0  30   Na_squid_hh 1200   K_squid_hh 360   spike 0.0

// The dendrite has no H-H channels, so ELEAK = EREST_ACT
*set_compt_param     ELEAK	-0.07
dend  soma  100  0  0   2   Ex_channel 0.795775
