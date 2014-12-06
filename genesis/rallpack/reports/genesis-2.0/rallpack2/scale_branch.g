//genesis
/* scale_Branch.g : description file for binary branching passive cable.
** By Upinder S. Bhalla      May 1992      Caltech.
** All units are in SI (Meter Kilogram Second Amp)
** usage : scale_branch dt depth filename
*/

/********************************************************************
**                                                                 **
**                       Model parameters                          **
**                                                                 **
********************************************************************/

/* Setting up the parameters for the model */
// 4.0 microns
float end_len = 4.0e-6
// 0.25 microns
float end_dia = 0.25e-6
float len
float dia
// for electrotonic length
float len_factor = {pow 2 {1.0/3.0}}
// Rall's power law
float power_law = 1.5
int tree_depth = $2
// ohms m^2
float RM = 4.0
// ohms m
float RA = 1.0
// Farads/m^2
float CM = 0.01
// -65 mV
float EM = -0.065
// 0.1 nA injection
float inj = 1.0e-10

// 0.05 msec
float dt = $1 * 1e-6
float iodt = 50e-6
// 0.25 sec
float runtime = 0.25
float PI = 3.1415926535

int X_flag = 0
int i

/********************************************************************
**                                                                 **
**                       Model construction                        **
**                                                                 **
********************************************************************/
if (iodt < dt)
	iodt = dt
end
showstat -process

/* We create an initial compartment */
dia = end_dia
len = end_len
create compartment /compt
setfield /compt Ra {RA*len*4.0/(dia*dia*PI)} Rm {RM/(PI*dia*len)}  \
    Cm {CM*PI*dia*len} Em {EM} dia {dia}

/* For the rest of the tree we make a new, larger parent compartment and
** attach the old compartment plus a duplicate to the parent. */
for (i = 1; i < tree_depth; i = i + 1)
	move /compt /old_compt
	dia = {pow {2.0*{pow {dia} {power_law}}} {1.0/power_law}}
	len = len*len_factor

	create compartment /compt
	setfield /compt Ra {RA*len*4.0/(dia*dia*PI)}  \
	    Rm {RM/(PI*dia*len)} Cm {CM*PI*dia*len} Em {EM} dia {dia}
	copy /old_compt /compt/branch1
		addmsg /compt /compt/branch1 AXIAL Vm
		addmsg /compt/branch1 /compt RAXIAL Ra Vm
		position /compt/branch1 R{len} R{len*i/10} I
	move /old_compt /compt/branch2
		addmsg /compt /compt/branch2 AXIAL Vm
		addmsg /compt/branch2 /compt RAXIAL Ra Vm
		position /compt/branch2 R{len} R{-len*i/10} I
end

/********************************************************************
**                                                                 **
**                    graphical and file I/0                       **
**                                                                 **
********************************************************************/

if (X_flag)
	xstartup
	create x1form /form [0,0,1000,500]
	create x1graph /form/graph [0%,0%,50%,100%]
	setfield /form/graph xmax {runtime} ymax 0.1 ymin -0.1
	addmsg /compt /form/graph PLOT Vm *root *red
	addmsg  \
	    /compt/branch1/branch1/branch1/branch1/branch1/branch1/branch1/branch1/branch1 \
	     /form/graph PLOT Vm *terminal *green
	create x1draw /form/draw [50%,0%,50%,100%] -wx 0.0002 -wy 0.0002 \
	     -cx 7e-5
	create x1cell /form/draw/cell -path /##[TYPE=compartment]
	disable /form/draw
	useclock /form/##[] 1
	xshow /form
end

create asc_file /output/out0
create asc_file /output/outx
setfield /output/out0 filename $3.out0 leave_open 1 flush 0
setfield /output/outx filename $3.outx leave_open 1 flush 0
addmsg /compt /output/out0 SAVE Vm
ce /compt
while (1)
	if (({exists branch1}))
		ce branch1
	else
		addmsg . /output/outx SAVE Vm
		break
	end
end
useclock /output/##[] 1

/********************************************************************
**                                                                 **
**                       Simulation control                        **
**                                                                 **
********************************************************************/

/* Set up the clocks that we are going to use */
setclock 0 {dt}
setclock 1 {iodt}

/* Set the stimulus conditions */
setfield /compt inject {inj}

/* Setting up the hines solver element */
create hsolve /solve
setfield /solve path /##[TYPE=compartment] comptmode 1 chanmode 3
/* using the crank-nicolson method */
setmethod 11
call /solve SETUP
/* run the simulation */

reset
showstat -process
step {runtime} -time
showstat -process

setfield /output/out# flush 1 leave_open 0
// flush files
call /output/out# PROCESS

rms $3.out0 ref_branch.0
rms $3.outx ref_branch.x
