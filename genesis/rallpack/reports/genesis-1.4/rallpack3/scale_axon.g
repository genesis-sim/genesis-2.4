//genesis
/* scale_axon.g : description file for unbranched axon, using standard
** Hodkin-Huxley channels and kinetics.
** By Upinder S. Bhalla       May 1992         Caltech.
** All units are in SI (Meter Kilogram Second Amp)
** Usage : scale_axon dt len filename
*/

/********************************************************************
**                                                                 **
**                       Model parameters                          **
**                                                                 **
********************************************************************/

/* Setting up the parameters for the model */
float	entire_len	= $2	// 1 mm
float	dia=1.0e-6			// 1 microns
float	len=1.0e-6			// 1 microns
float	RM=4				
float	RA=1.0
float	CM=0.01
float	EM=-0.065			// -65 mV
float	EREST_ACT=EM
float	inj=1.0e-10			// 0.1 nA injection

/* Channel parameters */
float	ENA = 0.050
float	EK = -0.077
float	GNA = 1200			// Siemens/m^2
float	GK = 360			// Siemens/m^2

float	dt=$1				// 0.05 msec
float	iodt=50e-6				// 0.05 msec 
float	runtime=0.25			// 0.25 sec
float	PI=3.1415926535
float	ncompts=entire_len/len
int		incompts=ncompts
float	area=PI*len*dia
float	Xarea=PI*dia*dia/4.0

int		X_flag = 0

/********************************************************************
**                                                                 **
**                       Model construction                        **
**                                                                 **
********************************************************************/

if (iodt<dt)
	iodt=dt
end
status -process

/* Setting up a prototype compartment */
/* The axon compartments could be directly created, but that
** would mean iterating over and setting all the values */
create compartment /proto/compartment
set /proto/compartment \
		Ra {RA*len/Xarea} \
		Rm {RM/area} \
		Cm {CM*area} \
		Em {EM}

ce /proto/compartment
/* Setting up the prototype channels. Note that these are all
** accessible from the standard Genesis channel library, and the
** creation routines are put here only for completeness.
** A standard form is used for the rate functions :
** alpha = (A+B*V)/(C+exp((V+D)/F))
** which can express all the forms used by H and H.
*/
create tabchannel /proto/compartment/Na
        set     Na   \
        Ek      {ENA}   \       //  V
        Gbar    { GNA * area }  \   //  S
        Ik      0   \           //  A
        Gk      0   \           //  S
        Xpower  3   \
        Ypower  1   \
        Zpower  0

    setup_alpha(Na,X,{0.1e6*(0.025 + EREST_ACT)}, \
         -0.1e6,-1.0,{-0.025 - EREST_ACT},-0.010,  \
         4e3,0.0,0.0,{-(0.0 + EREST_ACT) },18.0e-3)

    setup_alpha(Na,Y,70.0,0.0,0.0, \
        {-(0.0 + EREST_ACT)},0.020, \
        1.0e3,0.0,1.0,{-(0.030 + EREST_ACT)},-10.0e-3)

create  tabchannel  /proto/compartment/K
        set     K   \
        Ek      {EK}    \           //  V
        Gbar    { GK * area }  \   //  S
        Ik      0   \           //  A
        Gk      0   \           //  S
        Xpower  4   \
        Ypower  0   \
        Zpower  0

    setup_alpha(K,X,{10e3*(0.010+EREST_ACT)}, \
        -10e3,-1.0,{-(0.010+EREST_ACT)},-0.010, \
        125.0,0.0,0.0,{-(0.0+EREST_ACT)},80.0e-3)


sendmsg Na . CHANNEL Gk Ek
sendmsg . Na VOLTAGE	Vm
sendmsg K . CHANNEL Gk Ek
sendmsg . K VOLTAGE	Vm

/******************************************************************/

/* Creating all the copies of the prototype compartment
** Because Genesis finds elements by searching through all previous
** ones, it takes N^2 time to send messages between them.
** So we do the copying in two stages to reduce this time */
int i,n1,n2

	n1=sqrt({incompts})
	n2=n1+incompts-(n1*n1)
	createmap /proto/compartment /low {n1} 1
/* Setting up the inter-compartment communications */
	for(i=1;i<{n1};i=i+1)
		ce /low/compartment[{i}]
		sendmsg ../compartment[{i-1}] . RAXIAL Ra Vm
		sendmsg . ../compartment[{i-1}] AXIAL Vm
	end

	createmap /low /axon {n1-1} 1
	move /low /axon/low[{n1-1}]
/* Finishing up the remaining compartments */
	ce /axon/low[{n1-1}]
	for (i=n1;i<n2;i=i+1)
		copy /proto/compartment /axon/low[{n1-1}]/compartment[{i}]
		ce ^
		sendmsg ../compartment[{i-1}] . RAXIAL Ra Vm
		sendmsg . ../compartment[{i-1}] AXIAL Vm
	end

/* Linking up the groups */
	for(i=1;i<n1;i=i+1)
		sendmsg /axon/low[{i-1}/compartment[{n1-1}] /axon/low[{i}]/compartment[0] RAXIAL Ra Vm
		sendmsg /axon/low[{i}/compartment[0] /axon/low[{i-1}]/compartment[{n1-1}] AXIAL Vm
	end

/********************************************************************
**                                                                 **
**                    graphical and file I/0                       **
**                                                                 **
********************************************************************/

if (X_flag)
	xstartup
	create xform /form
	create xgraph /form/graph
	set /form/graph xmax {runtime} ymax 0.1 ymin -0.1  
	sendmsg /axon/low[0]/compartment[0] /form/graph PLOT Vm *comp_0 *red
	sendmsg /axon/low[{n1-1}]/compartment[{n2-1}] /form/graph \
		PLOT Vm *comp_{incompts-1} *green
	useclock /form/# 1
	xshow /form
end

create asc_file /output/out0
create asc_file /output/outx
set /output/out0 filename $3.out0 leave_open 1 flush 0  
set /output/outx filename $3.outx leave_open 1 flush 0
sendmsg /axon/low[0]/compartment[0] /output/out0 SAVE Vm
sendmsg /axon/low[{n1-1}]/compartment[{n2-1}] /output/outx SAVE Vm

useclock /output/# 1

/********************************************************************
**                                                                 **
**                       Simulation control                        **
**                                                                 **
********************************************************************/

/* Set up the clocks that we are going to use */
setclock 0 {dt}
setclock 1 {iodt}

/* Set the stimulus conditions */
set /axon/low[0]/compartment[0] inject {inj}

/* Setting up the hines solver element */
create hsolve /axon/solve
set /axon/solve path /axon/low[]/compartment[] comptmode 1 chanmode 3

setmethod 11	 /* using the crank-nicolson method */
call /axon/solve SETUP

/* run the simulation */

reset
reset
status -process
ps aux | head

step -t {runtime}
status -process
set /output/# flush 1 leave_open 0
call /output/# PROCESS

srms $3.out0 ref_axon.0.neuron
srms $3.outx ref_axon.x.neuron

quit
