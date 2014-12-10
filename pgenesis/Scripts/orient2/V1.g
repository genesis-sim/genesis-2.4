// genesis

int V1_cells_per_slice = v1_nx * v1_ny / n_slices;

float	V1_NX = v1_nx
float	V1_NY = v1_ny
float   V1_xmin = {-V1_NX * V1_SEPX / 2}
float   V1_ymin = {-V1_NY * V1_SEPY / 2}
float   V1_xmax = {V1_NX * V1_SEPX / 2}
float   V1_ymax = {V1_NY * V1_SEPY / 2}
float   V1_zoffset = {5 * V1_SEPZ}

float V1_xscale  = 0.7
float V1_yscale  = 0.8
float V1_xoffset = V1_SEPX/2
float V1_yoffset = V1_SEPY/2

function create_V1_horiz_slice end
function create_V1_vert_slice end
function create_V1_prototype end
function setup_V1_horiz_slice_display end
function setup_V1_vert_slice_display end

function create_V1_horiz_slice
	// CREATING A SLICE OF HORIZONTALLY SELECTIVE V1 CELLS
	int i

	create_V1_prototype
	createmap /library/soma /V1/horiz  \
		{V1_NX / n_slices} {V1_NY} \
		-delta {V1_SEPX} {V1_SEPY} \
		-origin {-V1_NX * V1_SEPX / 2 + \
				slice * V1_SEPX * V1_NX / n_slices} \
			{-V1_NY * V1_SEPY / 2}
	for (i = 0; i < V1_cells_per_slice; i=i+1)
	    setfield /V1/horiz/soma[{i}] \
		     io_index {slice * V1_cells_per_slice + i}
	end
end

function create_V1_vert_slice
	// CREATING A SLICE OF VERTICALLY SELECTIVE V1 CELLS
	createmap /library/soma /V1/vert   \
		{V1_NX / n_slices} {V1_NY} \
		-delta {V1_SEPX} {V1_SEPY} \
		-origin {-V1_NX * V1_SEPX / 2 + \
				slice * V1_SEPX * V1_NX / n_slices} \
			{-V1_NY * V1_SEPY / 2}
	for (i = 0; i < V1_cells_per_slice; i=i+1)
	    setfield /V1/vert/soma[{i}] \
		     io_index {slice * V1_cells_per_slice + i}
	end
end

function create_V1_prototype
	// Creating V1 prototype cell
	create neutral /V1

	copy /library/compartment	/library/soma  
	addfield /library/soma io_index
	setfield /library/soma z 0 
		// Assigning exploded z coordinates for ease of display

	ce /library/soma

	// Creating the modules for the prototype V1 cell
	copy /library/HH_Na_channel	/library/soma/HH_Na_channel  
	setfield HH_Na_channel z {V1_SEPZ * 1}

	copy /library/HH_K_channel	/library/soma/HH_K_channel 
	setfield HH_K_channel z {V1_SEPZ  * 2}
	
	copy /library/Na_channel	/library/soma/exc_syn  
	setfield exc_syn z {V1_SEPZ * 3}

	copy /library/K_channel	/library/soma/inh_syn  
	setfield inh_syn z {V1_SEPZ * 4}

	copy /library/spike	/library/soma/spike  
	setfield spike z {V1_SEPZ * -1}
	setfield spike thresh 0
	setfield spike abs_refract 10

	ce /library/soma

	// LINKING THE MODULES OF THE PROTOTYPE V1 CELL TOGETHER.
	// link in the spike generating module
	addmsg . spike INPUT Vm

	// link in HH_Na_channel
	addmsg HH_Na_channel . CHANNEL Gk Ek
	addmsg . HH_Na_channel VOLTAGE	Vm

	// link in HH_K_channel
	addmsg HH_K_channel . CHANNEL Gk Ek
	addmsg . HH_K_channel VOLTAGE	Vm

	// link in exc_syn
	addmsg exc_syn . CHANNEL Gk Ek
	addmsg . exc_syn VOLTAGE	Vm

	// link in inh_syn
	addmsg inh_syn . CHANNEL Gk Ek
	addmsg . inh_syn VOLTAGE	Vm
end

function setup_V1_horiz_slice_display
	int i

	raddmsg /V1/horiz/soma[] \
		/xout/drawh/inputs@{output_node} \
		ICOORDS io_index x y z
	raddmsg /V1/horiz/soma[] \
		/xout/drawh/inputs@{output_node} \
		IVAL1 io_index Vm
end

function setup_V1_vert_slice_display
	raddmsg /V1/vert/soma[] \
		/xout/drawv/inputs@{output_node} \
		ICOORDS io_index x y z
	raddmsg /V1/vert/soma[] \
		/xout/drawv/inputs@{output_node} \
		IVAL1 io_index Vm
end

function setup_V1_horiz_slice_output
	raddmsg /V1/horiz/soma[] \
		/output/horiz@{output_node} \
		SAVE io_index Vm
end

function setup_V1_vert_slice_output
	raddmsg /V1/vert/soma[] \
		/output/vert@{output_node} \
		SAVE io_index Vm
end

// $Log: V1.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:39  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.1  1999/12/18 03:02:06  mhucka
// Added PGENESIS from GENESIS 2.1 CDROM distribution.
//
// Revision 1.3  1996/08/13 21:27:45  ghood
// General debugging to get parallel example scripts working.
//
// Revision 1.2  1996/06/21 16:09:34  ghood
// General debugging
//
// Revision 1.1  1996/05/03 19:11:43  ngoddard
// Initial revision
//
