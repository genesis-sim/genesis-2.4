//chemesis2.0
// iicr-func.g
// sets up cicr objects and communication between them
// parameters are ai, bi, gi = alphai/k3, betai/k3, gammai/k3 from 
//Li and Rinzel, converted from uM to mM where dependent on Ca or IP3

function makeiicr(path)
   str path

float init000	=	0.325	/*  for ca=0.0110 */
float init100	=	0.0	/*  for ca=0.0110 */
float init010	=	0.435	/*  for ca=0.0110 */
float init001	=	0.103	/*  for ca=0.0110 */
float init101	=	0.0	/*  for ca=0.0110 */
float init011	=	0.137	/*  for ca=0.0110 */
float init110	=	0.0	/*  for ca=0.0110 */

	
   create cicr {path}/x000
   setfield {path}/x000 \
	alpha_state	0 \
	beta_state	0 \
	gamma_state	0 \
	alpha	400e3 \
	beta	20e3 \
	gamma	0.2e3 \
	conserve	0 \
	xinit		{init000} \
	xmin		0 \
	xmax		1
	
   create cicr {path}/x100
   setfield ^ \
	alpha_state	1 \
	beta_state	0 \
	gamma_state	0 \
	alpha	52 \
	beta	20e3 \
	gamma	0.2e3 \
	conserve	0 \
	xinit		{init100} \
	xmin		0 \
	xmax		1 
	
   create cicr {path}/x010
   setfield ^ \
	alpha_state	0 \
	beta_state	1 \
	gamma_state	0 \
	alpha	400e3 \
	beta	1.6468 \
	gamma	0.2e3 \
	conserve	0 \
	xinit		{init010} \
	xmin		0 \
	xmax		1
	
   create cicr {path}/x001
   setfield ^ \
	alpha_state	0 \
	beta_state	0 \
	gamma_state	1 \
	alpha	400e3 \
	beta	20e3 \
	gamma	0.0289 \
	conserve	0 \
	xinit		{init001} \
	xmin		0 \
	xmax		1
	
   create cicr {path}/x101
   setfield ^ \
	alpha_state	1 \
	beta_state	0 \
	gamma_state	1 \
	alpha	377.36 \
	beta	20e3 \
	gamma	0.2089 \
	conserve	0 \
	xinit		{init101} \
	xmin		0 \
	xmax		1
	
   create cicr {path}/x011
   setfield ^ \
	alpha_state	0 \
	beta_state	1 \
	gamma_state	1 \
	alpha	400e3 \
	beta	1.6468 \
	gamma	0.0289 \
	conserve	0 \
	xinit		{init011} \
	xmin		0 \
	xmax		1

   create cicr {path}/x110
   setfield ^ \
	alpha_state	1 \
	beta_state	1 \
	gamma_state	0 \
	alpha	52 \
	beta	1.6468 \
	gamma	0.2e3 \
	conserve	0 \
	xinit		{init110} \
	xmin		0  \
	xmax		1
	
   create cicr {path}/x111
   setfield ^ \
	alpha_state	1 \
	beta_state	1 \
	gamma_state	1 \
	alpha	377.36 \
	beta	1.6468 \
	gamma	0.2098 \
	conserve	1 \
	xinit		{1-init000-init100-init010-init001-init101-init110-init011} \
	xmin		0  \
	xmax		1

addmsg {path}/x100 {path}/x000 ASTATE alpha fraction
addmsg {path}/x010 {path}/x000 BSTATE beta fraction
addmsg {path}/x001 {path}/x000 GSTATE gamma fraction
	
addmsg {path}/x000 {path}/x100 ASTATE alpha previous_state
addmsg {path}/x110 {path}/x100 BSTATE beta fraction
addmsg {path}/x101 {path}/x100 GSTATE gamma fraction
	
addmsg {path}/x110 {path}/x010 ASTATE alpha fraction
addmsg {path}/x000 {path}/x010 BSTATE beta previous_state
addmsg {path}/x011 {path}/x010 GSTATE gamma fraction
	
addmsg {path}/x101 {path}/x001 ASTATE alpha fraction
addmsg {path}/x011 {path}/x001 BSTATE beta fraction
addmsg {path}/x000 {path}/x001 GSTATE gamma previous_state
	
addmsg {path}/x010 {path}/x110 ASTATE alpha previous_state
addmsg {path}/x100 {path}/x110 BSTATE beta previous_state
addmsg {path}/x111 {path}/x110 GSTATE gamma fraction
	
addmsg {path}/x001 {path}/x101 ASTATE alpha previous_state
addmsg {path}/x111 {path}/x101 BSTATE beta fraction
addmsg {path}/x100 {path}/x101 GSTATE gamma previous_state
	
addmsg {path}/x111 {path}/x011 ASTATE alpha fraction
addmsg {path}/x001 {path}/x011 BSTATE beta previous_state
addmsg {path}/x010 {path}/x011 GSTATE gamma previous_state

addmsg {path}/x000 {path}/x111 CONSERVE previous_state
addmsg {path}/x001 {path}/x111 CONSERVE previous_state
addmsg {path}/x010 {path}/x111 CONSERVE previous_state
addmsg {path}/x100 {path}/x111 CONSERVE previous_state
addmsg {path}/x110 {path}/x111 CONSERVE previous_state
addmsg {path}/x101 {path}/x111 CONSERVE previous_state
addmsg {path}/x011 {path}/x111 CONSERVE previous_state

end

/********************************************************************/

function makeiicrflux(Capath,erpath,ip3path,maxcond,exponen,quantUnits)
   str Capath,erpath,ip3path
   float maxcond
   int exponen
   
   create cicrflux {Capath}/iicrflux
   setfield {Capath}/iicrflux \
	power {exponen} \	
	maxflux {maxcond} \
    units {quantUnits}

/* Messages from cytosolic calcium to iicr receptor channel states */
addmsg {Capath} {erpath}/x100 CALCIUM Conc	
addmsg {Capath} {erpath}/x000 CALCIUM Conc	
addmsg {Capath} {erpath}/x010 CALCIUM Conc
addmsg {Capath} {erpath}/x001 CALCIUM Conc
addmsg {Capath} {erpath}/x110 CALCIUM Conc	
addmsg {Capath} {erpath}/x101 CALCIUM Conc	
addmsg {Capath} {erpath}/x011 CALCIUM Conc	

/* Messages from cytosolic IP3 compartment to iicr receptor states */
addmsg {ip3path} {erpath}/x000 IP3 Conc	
addmsg {ip3path} {erpath}/x100 IP3 Conc	
addmsg {ip3path} {erpath}/x010 IP3 Conc	
addmsg {ip3path} {erpath}/x001 IP3 Conc	
addmsg {ip3path} {erpath}/x110 IP3 Conc	
addmsg {ip3path} {erpath}/x101 IP3 Conc	
addmsg {ip3path} {erpath}/x011 IP3 Conc	
  
	/*Messages to iicr flux  (compute channel permeability) */

addmsg {Capath} {Capath}/iicrflux CONC1 Conc
addmsg {erpath} {Capath}/iicrflux CONC2 Conc
addmsg {erpath}/x110 {Capath}/iicrflux IP3R fraction

/* Messages back to core and er: deltaflux1 goes to CONC1 pool
   deltaflux2 goes to CONC2 pool */

addmsg {Capath}/iicrflux {Capath} RXN0MOLES deltaflux1
addmsg {Capath}/iicrflux {erpath} RXN0MOLES deltaflux2

end
