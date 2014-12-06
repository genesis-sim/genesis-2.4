// genesis

// Setting the axonal propagation velocity
float CABLE_VEL = 1	// scale factor = 1/(cable velocity) sec/meter

/*
 * Usage :
 * planarconnect source-path destination-path
 *		 [-relative]
 *		 [-sourcemask {box,ellipse} x1 y1 x2 y2]
 *		 [-sourcehole {box,ellipse} x1 y1 x2 y2]
 *		 [-destmask   {box,ellipse} x1 y1 x2 y2]
 *		 [-desthole   {box,ellipse} x1 y1 x2 y2]
 *		 [-probability p]
 */

echo Making connections from the retina to the V1 horiz cells.
planarconnect /retina/recplane/rec[]/input  \
	      /V1/horiz/soma[]/exc_syn	    \
	      -relative			    \
	      -sourcemask box -1 -1  1  1   \
	      -destmask   box  {-V1_SEPX * 2.4} {-V1_SEPY * 0.6} \
			       { V1_SEPX * 2.4} { V1_SEPY * 0.6}


echo Making connections from the retina to the V1 vert cells.
planarconnect /retina/recplane/rec[]/input \
	      /V1/vert/soma[]/exc_syn	   \
	      -relative			   \
	      -sourcemask box -1 -1  1  1  \
	      -destmask   box {-V1_SEPX * 0.6} {-V1_SEPY * 2.4} \
			      { V1_SEPX * 0.6} { V1_SEPY * 2.4}


echo Setting weights and delays for ret->V1 connections.
// assigning delays using the planardelay function

/* 
 * Usage :
 * planardelay path 
 * [-fixed delay]
 * [-radial propagation_velocity] 
 * [-uniform range]   (not used here)
 * [-gaussian sd max] (not used here)
 * [-exp mid max]     (not used here)
 * [-absoluterandom]  (not used here)
 */

planardelay /retina/recplane/rec[]/input -radial {CABLE_VEL}
// syndelay    /V1/horiz/soma[]/exc_syn 0.0001 -add
// syndelay    /V1/vert/soma[]/exc_syn  0.0001 -add 

// assigning weights using the planarweight function

/* 
 * Usage :
 *  planarweight sourcepath 
 *          [-fixed weight]
 *          [-decay decay_rate max_weight min_weight]
 *          [-uniform range] 
 *          [-gaussian sd max] 
 *          [-exponential mid max]
 *          [-absoluterandom]
 */

planarweight /retina/recplane/rec[]/input \
	-decay 0.5 0.2 0.05

// planarweight /retina/recplane/rec[]/input -fixed 0.22




