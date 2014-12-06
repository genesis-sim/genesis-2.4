// genesis
// cell parameter file for the 1991 Traub CA3 hippocampal cell
*cartesian
*relative
*symmetric

*set_global RM 1.0	//ohm*m^2
*set_global RA 1.0	//ohm*m
*set_global CM 0.03     //F/m^2
*set_global EREST_ACT	-0.06	// volts

// The format for each compartment parameter line is :
// name  parent  x       y       z       d       ch      dens ...
// For channels, "dens" =  maximum conductance per unit area of compartment

/* NOTE: The format of the cell descriptor files does not allow for
   continuation to another line.  The following long line lists the
   soma channels with their "density" parameters.

   Ca_conc	-17.402e12
	Not really a channel, but a "Ca_concen" object.  Normally, the B 
	field is set to "dens"/compt_volume (m^3), but the negative sign
	causes the absolute value to be used with no scaling by volume.
   Na		300  
   Ca		 40
   K_DR		150
   K_AHP	  8/20
   K_C		100/20
   K_A		 50
*/
// The compartment numbering corresponds to that in the paper, with soma = 9

apical_10 none     -120  0  0  5.78 Ca_conc -26.404e12 Na 150 Ca 80 K_DR 50 K_AHP 0.4 K_C 10

soma 	apical_10  -125  0  0  8.46 Ca_conc -17.402e12 Na 300 Ca 40 K_DR 150 K_AHP 0.4 K_C 5 K_A 50

basal_8  soma	   -110  0  0  4.84 Ca_conc -34.53e12 Na 150 Ca 80 K_DR 50 K_AHP 0.4 K_C 10
