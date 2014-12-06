// cell.p - Cell parameter file used in the simplecell tutorial

//	Format of file :
// x,y,z,dia are in microns, all other units are SI (Meter Kilogram Sec Amp)
// In polar mode 'r' is in microns, theta and phi in degrees 
// readcell options start with a '*'
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

//		Specifying constants  -- SI units
*set_compt_param	RM	2
*set_compt_param	RA	0.3
*set_compt_param	CM	0.01
*set_compt_param     EREST_ACT	-0.065

// soma  diameter was chosen to have an area of 20000 um.
soma none 79.7885  0  0  79.7885  \
    Na_traub_mod 1000.0 K_traub_mod 300.0 \
    Ex_channel -6e-9 Inh_channel -67e-9 spike 0.0
