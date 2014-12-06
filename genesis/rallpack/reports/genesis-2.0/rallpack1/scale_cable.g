//genesis
/* scale_cable.g : description file for unbranched passive cable
** with additional functions for performing scaling tests.
** argv1 = dt, argv2 = len, argv3 = filename
** By Upinder S. Bhalla     May 1992     Caltech.
** All units are in SI (Meter Kilogram Second Amp)
*/

/********************************************************************
**                                                                 **
**                       Model parameters                          **
**                                                                 **
********************************************************************/

/* Setting up the parameters for the model */
// 1 mm
float entire_len = $2 * 1e-6
// 1 microns
float dia = 1.0e-6
// 1 microns
float len = 1.0e-6
float RM = 4
float RA = 1.0
float CM = 0.01
// -65 mV
float EM = -0.065
// 0.1 nA injection
float inj = 1.0e-10

// 0.05 msec
float dt = $1 * 1e-6
// 0.05 msec 
float iodt = 50e-6
// 0.25 sec
float runtime = 0.25
float PI = 3.1415926535
float ncompts = entire_len/len
int incompts = ncompts
float area = PI*len*dia
float Xarea = PI*dia*dia/4.0

int X_flag = 0

/********************************************************************
**                                                                 **
**                       Model construction                        **
**                                                                 **
********************************************************************/

if (iodt < dt)
	iodt = dt
end

showstat -process

/* Setting up a prototype compartment */
/* The cable compartments could be directly created, but that
** would mean iterating over and setting all the values */
create compartment /proto/compartment
setfield /proto/compartment Ra {RA*len/Xarea} Rm {RM/area} Cm {CM*area}  \
    Em {EM}

/******************************************************************/

/* Creating all the copies of the prototype compartment
** Because Genesis finds elements by searching through all previous
** ones, it takes N^2 time to send messages between them.
** So we do the copying in two stages to reduce this time */
int i, n1, n2

	n1 = {sqrt {incompts}}
	n2 = n1 + incompts - (n1*n1)
	createmap /proto/compartment /low {n1} 1
	/* Setting up the inter-compartment communications */
	for (i = 1; i < (n1); i = i + 1)
		ce /low/compartment[{i}]
		addmsg ../compartment[{i - 1}] . RAXIAL Ra Vm
		addmsg . ../compartment[{i - 1}] AXIAL Vm
	end

	createmap /low /cable {n1 - 1} 1
	move /low /cable/low[{n1 - 1}]
	/* Finishing up the remaining compartments */
	ce /cable/low[{n1 - 1}]
	for (i = n1; i < n2; i = i + 1)
		copy /proto/compartment  \
		    /cable/low[{n1 - 1}]/compartment[{i}]
		ce ^
		addmsg ../compartment[{i - 1}] . RAXIAL Ra Vm
		addmsg . ../compartment[{i - 1}] AXIAL Vm
	end

	/* Linking up the groups */
	for (i = 1; i < n1; i = i + 1)
		addmsg /cable/low[{i - 1}/compartment[{n1 - 1}]  \
		    /cable/low[{i}]/compartment[0] RAXIAL Ra Vm
		addmsg /cable/low[{i}/compartment[0]  \
		    /cable/low[{i - 1}]/compartment[{n1 - 1}] AXIAL Vm
	end

	/*************************************************************** \
	    *****
**                                                                 **
**                    graphical and file I/0                       **
**                                                                 **
********************************************************************/

if (X_flag)
	xstartup
	create x1form /form
	create x1graph /form/graph
	setfield /form/graph xmax {runtime} ymax 0.1 ymin -0.1
	addmsg /cable/low[0]/compartment[0] /form/graph PLOT Vm *comp_0  \
	    *red
	addmsg /cable/low[{n1 - 1}]/compartment[{n2 - 1}] /form/graph  \
	    PLOT Vm *comp_{incompts - 1} *green
	useclock /form 1
	xshow /form
end

create asc_file /output/out0
create asc_file /output/outx
setfield /output/out0 filename $3.out0 leave_open 1 flush 0
setfield /output/outx filename $3.outx leave_open 1 flush 0
addmsg /cable/low[0]/compartment[0] /output/out0 SAVE Vm
addmsg /cable/low[{n1 - 1}]/compartment[{n2 - 1}] /output/outx SAVE Vm

useclock /output/out0 1
useclock /output/outx 1

/********************************************************************
**                                                                 **
**                       Simulation control                        **
**                                                                 **
********************************************************************/

/* Set up the clocks that we are going to use */
setclock 0 {dt}
setclock 1 {iodt}

/* Set the stimulus conditions */
setfield /cable/low[0]/compartment[0] inject {inj}

/* Setting up the hines solver element */
create hsolve /cable/solve
setfield /cable/solve path /cable/low[]/compartment[] comptmode 1  \
    chanmode 3

/* using the crank-nicolson method */
setmethod 11
call /cable/solve SETUP

/* run the simulation */

reset

showstat -process
step {runtime} -time
showstat -process
setfield /output/# flush 1
call /output/# PROCESS

rms $3.out0 ref_cable.0
rms $3.outx ref_cable.x

quit
