// genesis

int	step_count = 0

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

function create_V1 end
function create_V1_prototype end
function display_V1 end
function output_V1 end

function create_V1
	create_V1_prototype
	// now create a set of V1 cells
	createmap /library/soma /V1/{dir}  \
		{V1_NX} {V1_NY} \
		-delta {V1_SEPX} {V1_SEPY} \
		-origin {-V1_NX * V1_SEPX / 2} {-V1_NY * V1_SEPY / 2}
end

function create_V1_prototype
	// Creating V1 prototype cell
	create neutral /V1

	copy /library/compartment	/library/soma  
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

function display_V1
	xcolorscale rainbow2
	if ({dir} == "horiz")
		create xform /xout  [400,300,800,400]
	else
		create xform /xout  [400,600,800,400]
	end

	create xlabel /xout/"Activity Pattern" [4%,2%,45%,30]
	create xdraw /xout/draw3 [4%,10%,45%,300]
	setfield     /xout/draw3 \
	    xmin     {V1_xmin / V1_xscale} \
	    xmax     {V1_xmax / V1_xscale} \
	    ymin     {V1_ymin / V1_yscale} \
	    ymax     {V1_ymax / V1_yscale}

	if ({dir} == "horiz")
		create xlabel /xout/"V1 horizontal cells" [2%,2%,45%,30]
	else
		create xlabel /xout/"V1 vertical cells" [2%,2%,45%,30]
	end

	// create labels for the xview widgets corresponding to V1 cells
//	create xshape /xout/draw3/{dir}_label -text {dir}  \
//			-tx -0.00028 -ty -0.0003
	create xview /xout/draw3/{dir}
	setfield     /xout/draw3/{dir}          \
	             tx { V1_xoffset}           \
	             ty { V1_yoffset}           \
		     value_min[0] -0.090        \
		     value_max[0]  0.060        \
	             sizescale {V1_SEPX * 30}   \
		     morph_val 0                \
		     path /V1/{dir}/soma[]      \
		     field Vm

	/* Simgraph plots out the membrane potential of selected cells */
	create xgraph /xout/simgraph [52%,10,45%,350] \
	    -range [0,-0.5,0.1,0.1] -title "membrane potential Vm (V)"  
	setfield /xout/simgraph yoffset -0.1

	addmsg /V1/{dir}/soma[14] /xout/simgraph PLOT Vm *Vm14 *black
	addmsg /V1/{dir}/soma[13] /xout/simgraph PLOT Vm *Vm13 *blue
	addmsg /V1/{dir}/soma[12] /xout/simgraph PLOT Vm *Vm12 *orange
	addmsg /V1/{dir}/soma[11] /xout/simgraph PLOT Vm *Vm11 *red
	addmsg /V1/{dir}/soma[10] /xout/simgraph PLOT Vm *Vm10 *green

	xshow /xout
end

function output_V1
	create disk_out /output/{dir}
	setfield /output/{dir} leave_open 1 flush 1
	addmsg /V1/{dir}/soma[] /output/{dir} SAVE Vm
end

function set_step_count (n)
	int n
	step_count = n
end

// $Log: V1.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:39  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.1  1999/12/18 03:02:04  mhucka
// Added PGENESIS from GENESIS 2.1 CDROM distribution.
//
// Revision 1.4  1996/09/20 18:36:56  ghood
// cleaned up for PSC release
//
// Revision 1.3  1996/06/21 16:43:56  ngoddard
// removed debugging mods
//
// Revision 1.2  1996/06/21  16:09:31  ghood
// General debugging
//
// Revision 1.1  1996/05/03 19:11:36  ngoddard
// Initial revision
//
