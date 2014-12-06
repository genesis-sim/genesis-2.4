// genesis

// Creating V1 cells and arranging them in space.

float	V1_SEPX = 80e-6
float	V1_SEPY = 80e-6
float	V1_SEPZ = 1e-3
float	V1_NX = 5
float	V1_NY = 5
float   V1_xmin = {-V1_NX * V1_SEPX / 2}
float   V1_ymin = {-V1_NY * V1_SEPY / 2}
float   V1_xmax = {V1_NX * V1_SEPX / 2}
float   V1_ymax = {V1_NY * V1_SEPY / 2}
float   V1_zoffset = {5 * V1_SEPZ}


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


// CREATING THE PLANE OF HORIZONTALLY SELECTIVE  V1 CELLS
createmap /library/soma /V1/horiz  \
	{V1_NX} {V1_NY} \
	-delta {V1_SEPX} {V1_SEPY} \
	-origin {-V1_NX * V1_SEPX / 2} {-V1_NY * V1_SEPY / 2}


// CREATING THE PLANE OF VERTICALLY SELECTIVE  V1 CELLS
createmap /library/soma /V1/vert   \
	{V1_NX} {V1_NY} \
	-delta {V1_SEPX} {V1_SEPY} \
	-origin {-V1_NX * V1_SEPX / 2} {-V1_NY * V1_SEPY / 2}

// Note that these cells' positions overlap.  This doesn't cause any
// problems since we can refer to them as separate groups.

