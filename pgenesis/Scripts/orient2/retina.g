// genesis

int retinal_cells_per_slice = retina_nx * retina_ny / n_slices

function create_retinal_slice end
function connect_retinal_slice end
function setup_retinal_slice_display end

function create_retinal_slice
	int i

	// creating the retina
	create neutral /retina

	// creating a receptor cell in the library 
	// which is just a randomspike input module; rate is in spikes/sec 
	if (!{exists /library})
	  create neutral /library 
	  disable /library 
	end

	create neutral     /library/rec
	create randomspike /library/rec/input  
	setfield ^ rate 1.0 abs_refract 0.001 
	addfield /library/rec/input io_index

	// creating a plane (map) of receptor cells on the retina
	// based on the /library/rec prototype 
	// Usage :
	// createmap prototype destination 
	// number_in_x_direction number_in_y_direction
	// -delta separation_in_x_direction separation_in_y_direction
	// -origin offset_in_x_direction offset_in_y_direction

	createmap /library/rec /retina/recplane \
	        {REC_NX / n_slices}   {REC_NY} \
		-delta  {REC_SEPX} {REC_SEPY}   \
		-origin	{-REC_NX * REC_SEPX / 2 + \
			    slice * REC_SEPX * REC_NX / n_slices} \
			{-REC_NY * REC_SEPY / 2}
	for (i = 0; i < retinal_cells_per_slice; i=i+1)
	    setfield /retina/recplane/rec[{i}]/input \
		     io_index {slice * retinal_cells_per_slice + i}
	end
end

function connect_retinal_slice
	echo Making connections from the retina to the V1 horiz cells
	echo workers {workers}
	rvolumeconnect /retina/recplane/rec[]/input \
		/V1/horiz/soma[]/exc_syn@{workers}   \
		-relative                         \
	        -sourcemask box -1 -1 0  1 1 0    \
		-destmask box {-2.4 * V1_SEPX} {-0.6 * V1_SEPY} {-5.0 * V1_SEPZ} \
	                      { 2.4 * V1_SEPX} { 0.6 * V1_SEPY} { 5.0 * V1_SEPZ}
	echo Making connections from the retina to the V1 vert cells
	rvolumeconnect /retina/recplane/rec[]/input \
		      /V1/vert/soma[]/exc_syn@{workers}   \
		      -relative			   \
		      -sourcemask box -1 -1 0  1 1 0 \
		      -destmask   box {-0.6 * V1_SEPX} {-2.4 * V1_SEPY} {-5.0 * V1_SEPZ} \
				      { 0.6 * V1_SEPX} { 2.4 * V1_SEPY} { 5.0 * V1_SEPZ}

	barrier
	echo Setting weights and delays for ret->V1 connections.
	// assigning delays using the rvolumedelay function
	rvolumedelay /retina/recplane/rec[]/input -radial {CABLE_VEL}

	// syndelay    /V1/horiz/soma[]/exc_syn 0.0001 -add
	// syndelay    /V1/vert/soma[]/exc_syn  0.0001 -add 

	barrier
	// assigning weights using the rvolumeweight function
	rvolumeweight /retina/recplane/rec[]/input -fixed 0.22
end

function setup_retinal_slice_display
	echo going to connect retina to display
	raddmsg /retina/recplane/rec[]/input  \
		/xout/drawr/inputs@{output_node} \
		ICOORDS io_index x y z
	raddmsg /retina/recplane/rec[]/input  \
		/xout/drawr/inputs@{output_node} \
		IVAL1 io_index state
	echo finished connecting retina to display
end

// $Log: retina.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:39  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.1  1999/12/18 03:02:07  mhucka
// Added PGENESIS from GENESIS 2.1 CDROM distribution.
//
// Revision 1.3  1996/08/13 21:27:49  ghood
// General debugging to get parallel example scripts working.
//
// Revision 1.2  1996/06/21 16:09:37  ghood
// General debugging
//
// Revision 1.1  1996/05/03 19:11:47  ngoddard
// Initial revision
//
