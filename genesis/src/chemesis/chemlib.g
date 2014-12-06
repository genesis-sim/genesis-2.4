//genesis
//chemlib.g
/*
************************************************************************
**                      SHELL COMMANDS                    	     **
************************************************************************
*/

/*
*************************************************************************
**                              OBJECT TABLE                           **
*************************************************************************
*/

//==========================================================================
//	name		data            function	class	
//==========================================================================

object	diffusion	diffusion_type	diffusion	segment channel\
	-author	  "A. Blackwell GMU Dec 97; revised GMU Dec 99; GMU Mar 02" \
	-actions  INIT PROCESS RESET CHECK \
	-messages POOL1	  0  3  len1 area1 conc1 \ /* pool 1 dimensions */
		  POOL2   1  3  len2 area2 conc2 \ /* pool 2 dimensions*/
	-fields	"D = diffusion constant in cm^2/s" \
		"difflux1 = D*area/len*conc21 send value to A field of pool 2" \
		"difflux2 = D*area/len*conc2, send value to A field of pool 1" \
		"units = 1 for moles, 1e-3 for mmoles, 1e-6 for umoles"\
		"Dunits = 1 for meters, 1e-4 for cm, etc"\
 	-description "Computes flux in molecules due to diffusion through surface between two rxnpools" \
		"(1-D or 2-D). Input must be concentration (not quantity). return messages:"\
		" addmsg from diffusion object to pool2 RXN0MOLES difflux2 " \
		" addmsg from diffusion object to pool1 RXN0MOLES difflux1 "

object	rxnpool		rxnpool_type 	rxnpool		segment concentration \
	-author	  "A. Blackwell ERIM Aug 96; revised GMU Dec 99; GMU Mar 02" \
	-actions  INIT PROCESS RESET CHECK SAVE2 RESTORE2 \
	-messages RXN0  0	1    A  \   /* type 0: 0th order rxn, concentration units*/
		  RXN1  1	1    B  \   /* type 1: 1st order rxn, units: per time */
		  RXN2	2 	2    A  B  \  /* type 2: 2nd order rxn, concentration units */
		  RXN2MOLES 3 	2    A  B \   /* type 3: 2nd order rxn, molecule units */
		  CURRENT   4 	2   charge current \ /* type 4: electrical current, nA input */
		  RXN0MOLES 5 	1    A \ /* type 5: molecule influx due to  MM pump, diffusion, etc.*/
		  VOLUME    6 	3   vol SAin SAout \ /* type 6: volume and SAin input */
	-fields	"Conc = concentration" \
		"Cmin = minimum concentration" \
		"Cinit = initial concentration" \
		"quantity = quantity in molecules" \
		"Qinit = initial quantity in molecules" \
		"len = length of cylindrical pool in cm" \
		"radius = radius of cylindrical or spherical pool" \
		"vol = volume of pool" \
		"SAside = side surface area of cylinder" \
		"SAout = outer surface area of spherical shell" \
		"SAin = inner surface area of spherical shell" \
		"type = whether pool integrates concentration (0) or quantity (1)"\
		"units = 1 for moles, 1e-3 for mmoles, 1e-6 for umoles"\
		"Iunits = 1e-12 to convert from nA, msec to A,sec"\
		"Dunits = 1 for meters, 1e-4 for cm, etc"\
        "VolUnitConv: Dunits^3, converts volume to SI units"\
	-description "Concentration pool. 1-D or 2-D. Interacts with reactions, diffusion, pumps," \
		"current, CICR or IICR.  Accumulates fluxes into A and B to solve d[C]/dt = A - B*[C]"\ 
		"Input in Concentration for RXN0, RXN1 and RXN2 (0th, 1st and 2nd order reactions." \ 
		"Input for CURRENT in any units, set Iunits=1 for SI; Iunits=1e-12 for nA, msec" \
        "Uses physiologist convention: negative current is inward (increases Ca conc. " \
 		"Input from diffusion,CICR flux, M-M rxn or pump (molecules): RXN0MOLES " \
		"Input in moles for RXN2MOLES (second order reaction, in units of moleculess:"\
		"units = 1 to use moles, 1e-3 for mmoles, 1e-6 for umoles"\
        "Dunits = 1 for meters, 1e-4 for cm, etc"\
		"If type = 0, converts molecule inputs to concentration and then integrates."\
		"If type = 1, converts concentration inputs to molecules and then integrates." 

object	cicr		IP3R_type	cicrpool	segment gate\
	-author   "A. Blackwell ERIM Aug 96; revised GMU Aug 99" \
	-actions  INIT PROCESS RESET CHECK \
	-messages ASTATE 0 2    kbalpha conc \ /* back rate const and conc in alpha dir */
		  BSTATE 1 2    kbbeta  conc \ /* back rate const and conc in beta dir */
		  GSTATE 2 2    kbgamma conc \ /* back rate const and conc in gamma dir */
		  IP3    3 1    ip3conc      \ /* concentration of ip3 */
		  CALCIUM 4 1   caconc       \ /* concentration of calcium */
		  CONSERVE 5 1  xconc	     \ /* conc of other states */
	-fields "fraction = fraction of channel in state "\
	        "alpha = forward rate constant in alpha direction "\
	        "beta = forward rate constant in beta direction "\
	        "gamma = forward rate constant in gamma direction "\
	        "alpha_state = if ip3 is bound (1) or not (0) for this state "\
	        "beta_state = if calcium is bound (1) or not (0) to open part for this state "\
	        "gamma_state = if calcium is bound(1) or not (0) to closed part for this state "\
	        "xinit = initial value"\
	        "xmin = minimum value"\
	        "xmax = maximum value"\
	        "conserve = flag indicating a conserve pool"\
	 -description " computes state variables associated with ip3 receptor channel."\
	 	      " Needs to send messages to cicrflux, which computes flux between two "\
	 	      " pools connected by the channel (i.e. cytosol and ER). See script for usage."

object	cicrflux	flux_type	cicrflux	segment channel\
	-author	  "A. Blackwell ERIM Aug 96; revised GMU Dec 99; GMU Mar 02" \
	-actions  INIT PROCESS RESET CHECK \
	-messages CONC1	 0  1   conc1 \ /* type 0: pool 1 concentration */
		  CONC2  1  1   conc2 \ /* type 1: pool 2 concentration*/
		  IP3R   2  1   ip3r  \ /* type 2: pool 2 fraction ip3r conducting*/
	-fields	"maxflux = single channel flux (cm/msec) * number of channels * Surface area (cm^2)" \
		"power = exponent; fraction open = ip3r^type" \
		"deltaflux2 = maxflux*openfraction*(conc1-conc2), send value to A of rxnpool 2" \
		"deltaflux1 = maxflux*openfraction*(conc2-conc1), send value to A of rxnpool 1" \
		"units = 1 for moles, 1e-3 for mmoles, 1e-6 for umoles"\
	-description "Computes flow through ip3 receptor channel, between ER and cytosol rxnpools." \
		"Requires inputs from two rxnpools (units must be concentration) and the ip3r object "\
		"return messages as follows: "\
		"addmsg from cicrflux object to pool 1 RXN0MOLES deltaflux2 " \
		"addmsg from cicrflux object to pool 2 RXN0MOLES deltaflux1 "

object	enzyme		enzyme_type	enzyme		segment \
	-author	  "A. Blackwell ERIM Aug 96; revised GMU Dec 99; revised GMU Mar 02" \
	-actions  INIT PROCESS RESET SET CHECK\
	-messages ENZ   0   1   conc \  /* concentration or quantity of enzyme */
		  FEEDBACK 1 2  feedback rateconst \ /* value of feedback, which rate const. */
		  SUBSTRATE 2 1  conc \ /* conc. of substrate */
		  RHODOPSIN 3 4 mrho vol SAin SAout\  /* effective rhodopsin, volume and SAin input from rhodopsin */
		  VOLUME    4 	3   vol SAin SAout \ /* type 6: volume and SAin input */
	-fields	"Conc = concentration (integrated)" \
		"Cmin = minimum concentration" \
		"Cinit = initial concentration" \
		"quantity = quantity in molecules" \
		"Qinit = initial quantity in molecules" \
		"len = length of cylindrical pool in cm" \
		"radius = radius of cylindrical or spherical pool" \
		"vol = volume of pool in cm^3" \
		"SAside = side surface area of cylinder in cm^2" \
		"SAout = outer surface area of spherical shell" \
		"SAin = inner surface area of spherical shell" \
		"type = whether pool integrates concentration (0) or quantity (1)"\
		"units= 1 for moles, 1e-3 for mmoles, 1e-6 for umoles"\
		"kf = forward rate  constant"\
		"kb = backward rate  constant"\
		"kcat = catalytic rate constant"\
		"deltacat = change in product of whole reaction  = kcat*EScomplex"\
		"deltaenz = change in free enzyme conc = (kb+kcat)*EScomplex"\
		"kbprod = change in substrate conc = kb * EScomplex conc"\
		"kfsubs = kf * e * s"\
		"feedback = computed feedback value" \
		"rateconst = which rate const is modified by feedback" \
		"fbconc	= concentration of feedback substance"\
		"thresh = above or below this no feedback computed." \
		"sign = -1 for feedback below threshold, +1 for fb above thresh" \
		"pow = raise fb to this power " \
		"form = 0 for hyperbolic, 1 for sigmoid increase, 2 for sigmoid decrease" \
		"Dunits = 1 for meters, 1e-4 for cm, etc"\
        "VolUnitConv: Dunits^3, converts volume to SI units"\
	-description "Michaelis-Menton enzyme kinetics. Vmax = kcat; Km = (Kcat+kb)/kf"\
		"			kf	  kcat"\
		"[Enzyme] + [Substrate] <-> [E-S] -> [Enzyme] + [Product]"\
		"			 kb"\
		"Computes dprod/dt =  kcat*E-Scomplex"\
		"Solves for E-Scomplex concentration from:"\
		"dES/dt = -(kcat+kb)*ES + kf*E*S"\
		"kcat and kb units are per time. Kf units must be"\
		"consistent with substrate units."\
		"dProduct/dt units are quantity (not concentration)"\  
		"addmsg from enz object to rxnpool RXN0MOLES product."\
	        "Feedback allows decrease in rate constants as function of "\
		"concentration of feedback substance.  Feedback message requires "\
		"feedback conc and rate const: 0 for kf, 1 for kb, 2 for kcat."\
		"hyperbolic form: fb propto 1 / conc(feedback substance) for neg feedback " \
	        "                 fb propto conc (feeback substance) for positive feedback. " \
    		"Sigmoid form: fb propto conc/conc + halfmax (for inc feedback) " \
                "		fb propto halfmax / conc + halfmax (for dec feedback) "

object	mmenz		mmenz_type	mmenz		segment \
	-author	  "A. Blackwell ERIM Aug 96; revised GMU Dec 99" \
	-actions  INIT PROCESS RESET SET\
	-messages ENZ   0   1   conc \  /* concentration or weight or mmol of enzyme */
		  FEEDBACK 1 1  conc \ /* conc. of feedback substance */
		  SUBSTRATE 2 1  conc \ /* conc. of substrate */
	-fields	  "Vmax = maximal rate of reaction, units of mmole per msec-(mmol or mg or conc enz)" \
		  "Km = combination rate constant: (kf2+kb1)/kf1:" \
		  "Product = mmoles of product produced" \
		  "thresh = below this conc, pos feedback, above -> neg fb." \
		  "pos_pow = raise pos fb to this power " \
		  "pos_form = 0 for hyperbolic, 1 for sigmoid " \
		  "pos_halfmax = param for sigmoid feedback " \
		  "neg_pow = raise neg fb to this power " \
		  "neg_form = 0 for hyperbolic, 1 for sigmoid " \
		  "neg_halfmax = param for sigmoid feedback " \
	     	  "feedback = computed value of feedback " \
	-description "Simplified Michaelis-Menton enzyme kinetics.  Can assume constant substrate, "\
		" or can compute effect of substrate on reaction rate.  Vmax = kf2;  Km = (Kf2+kb1)/kf1"\
		"			kf1	  kf2"\
		"[Enzyme] + [Substrate] <-> [E-S] -> [Enzyme] + [Product]"\
		"			 kb1"\
		"Under assumptions of M-M dynamics, Solves   dprod/dt =  kf1*Enzyme*Subs/(Subs + Km"\
		"Vmax units are mmol product per (mg or mmol or mM enzyme) per msec.  Must be consistent "\
		"with Enzyme units (input from enzyme rxnpool.  Substrate and Km must have units of Conc."\
		"Product (mmol units) = Vmax * E * S/ (S+Km).  addmsg from enz object to rxnpool RXN0MOLES product."\
		"For more accurate MM kinetics, use reaction objects OR Bhalla enz object in kinetikit."\
	        "Feedback allows decrease in enzyme activity as function of concentration of feedback substance " \  
		"hyperbolic form: fb propto 1 / conc(feedback substance) for neg feedback " \
	        "                 fb propto conc (feeback substance) for positive feedback. " \
    		"Sigmoid form: fb propto conc/conc + halfmax (for neg feedback) " \
                "		fb propto halfmax / conc + halfmax (for pos feedback) "
	
object	feedback	feedback_type	feedback	segment \
	-author	  "A. Blackwell GMU feb 02" \
	-actions  INIT PROCESS RESET SET\
	-messages CONC   0   1   conc \  /* conc of feedback substance */
	-fields	"thresh = above or below this no feedback computed." \
		"sign = -1 for feedback below threshold, +1 for fb above thresh" \
		"pow = raise fb to this power " \
		"form = 0 for hyperbolic, 1 for sigmoid increase, 2 for sigmoid decrease" \
		"feedback = computed feedback value" \
		"halfmax = half maximum value for sigmoid"\
	-description "An object for computing modification of enzyme rate constants"\
			"caused by feedback of some substance, e.g. calcium, sent as message." \
		"hyperbolic form: fb propto 1 / conc(feedback substance) for neg feedback " \
	        "                 fb propto conc (feeback substance) for positive feedback. " \
    		"Sigmoid form: fb propto conc/conc + halfmax (for inc feedback) " \
                "		fb propto halfmax / conc + halfmax (for dec feedback) "

object	rhodopsin	rhodopsin_type	rhodopsin	segment \
	-author "A. Blackwell GMU March 02" \
	-actions INIT PROCESS RESET SET CHECK \
	-messages ISOM 0 1 isomerization \ /* floating point between 0 and 1, converted to villi number */
		  INACT 1 1 inactivation \ /* floating point between 0 and 1, converted to villi number */
	-fields "input_slice: slice of input isomerization"\
		"slice: slice of present element"\
		"isom: total_villi * value of input isomerization"\
		"last_isom: floating value of last isomerization"\
		"villus:  villus of present isomerization"\
		"total_villi: number of villi in rhabdomere"\
		"slice_villi: number of villi per slice"\
		"villi_isom[420]: number of isomerizations in each villus"\
		"isom_time[420]: time of first isomerization in each villus"\
		"total_time: time since start of sim"\
		"active_villi: number of villi with isomerizations"\
		"villus_vol: volume of single villus"\
		"villus_xarea: cross sectional area of single villus"\
		"villus_sa: surface area of single villus"\
		"slice_vol: combined volume of active villi"\
		"slice_xarea: combined cross sectional area of active villi"\
		"slice_sa: combined surface area of active villi"\
		"factor[10]: effectime dependent tiveness of nth isomerization in villus"\
		"total_isom: total number of isomerizations (mrho) in slice"\
		"depletion: time dependent decrease in mrho effectiveness"\
		"effective: combined effectiveness of all mrho in slice"\
		"villi_list[420]: list of villi with isomerizations"\
		"inact_const: Krkcat*Krkf*RKtot/(Krkcat+Krkb) or rate of inactivation"\
		"inact_rate:  inact_const * mrho: rate needed by Poisson generator"\
		"inact: which villus has an inactivated rhodopsin"\
		"last_inact: floating value of last inactivation"\
		"total_inact: total number of inactivated mrho"\
		"conc: concentration of metarhodopsin"\
		"quantity: quantity of metarhodopsin"\
		"units= 1 for moles, 1e-3 for mmoles, 1e-6 for umoles"\
		"len: of villus"\
		"radius: of villus"\
		"type: presently unused"\
	-description	"Stochastic activation of rhodopsin molecules by Poisson generator"\
			"includes stochastic inactivation by Rhodopsin Kinase or Arrestin"\
			"Keeps track of: "\
			"   Number of rhodopsin molecules per villi"\
			"       which allows decrease effectiveness with additional villi"\
			"   time of rhodopsin activation"\
			"       which allows decrease effectiveness over time due to depletion"

object	mmpump2		pump_type	mmpump2		segment  channel\
	-author	  "A. Blackwell ERIM Aug 96; revised GMU Dec 99; GMU Mar 02" \
	-actions  INIT PROCESS RESET SET\
	-messages CONC   0   1   conc \  
	-fields	"max_rate = Maximum Pump Rate"\
		"moles_in = State variable for return message to pools" \
		"moles_out = State variable for return message to pools" \
		"half_conc = concentration at which pump rate is half maximal"\
		"power = exponent applied to Concentration"\
		"units= 1 for moles, 1e-3 for mmoles, 1e-6 for umoles"\
	-description " A Michaelis-Menton type of formulation of pump.  Either SERCA ATPase pump or "\
		     " Na-Ca ATPase cytosolic pump. Requires input message in concentration units."\
		     " addmsg from mmpump2 to cytosol-rxnpool RXN0MOLES moles_out."\
		     " addmsg from mmpump2 to ER or extracellular rxnpool RXN0MOLES moles_in."\
		     " Computes right hand side of dC/dt = (+/-)max_rate * Ca^n / (Ca^n + half_conc^n). "

object	ncx		exchange_type	ncx		segment  channel\
	-author	  "A. Blackwell GMU Sept 02" \
	-actions  INIT PROCESS RESET SET CHECK \
	-messages CAINT   0   1   conc \  /* Ca internal concentration */
              CAEXT   1   1   conc \ /* Ca external Concentration */
              NAINT   2   1   conc \ /* Na internal Concentration */
              NAEXT   3   1   conc \ /* Na external Concentration */
              VM   4   1    Vm \ /* membrane potential*/
	-fields	" Ca_int: internal calcium concentration " \
            "  Ca_ext: external calcium concentration " \
            "  Na_int: internal calcium concentration " \
            "  Na_ext: external calcium concentration " \
            "  Vm: membrane potential " \
            "  Vnaca: reveral potential of ncx " \
            "  naterm: Na_ext^stoich, used for Jafri formula " \
            " caterm: Ca_ext ^hill, used for Gall formula " \
            " chi: F/RT" \
            " Kmca: affinity for calcium " \
            " Kmna: affinity for sodium " \
            " kmhill: km^hill" \
            " naicao: nai^hill*cao" \
            " gamma: partition coefficient " \
            " T: temperature in Kelvin " \
            " I: current " \
            " Gcurrent: conductance for compartment " \
            " Gbar: maximal conductance " \
            " ksat: another constant used by jafri " \
            " Vunits: units of membrane potential " \
            " valence: stoich - calcium charge " \
            " hill: hill coefficient " \
            " stoich: stoichiometry of pump " \
            " Na_msg: whether sodium conc messages are being passed " \
            " ncxtype: whether using Gall (0) or Jafri (1) formula " \
	-description " Sodium calcium exchanger equations.  if ncxtype = 0 uses Gall formula:" \
       " I = K * Cai^Hill/(Cai^Hill + Kmca^H) * (Vm - Vnaca) " \
       " where Vnaca = RT/F(stoic*ln(Nao/Nai) - ln(Cao/Cai)) " \
       " if ncxtype = 1 uses jafri formula: "  \
       " I = K* (Cao*Nai^stoich*exp(gamma*FV/RT) - Cai*Nao^stoich*exp((1-gamma*FV/RT))" \
       " divided by (Kmna^stoich+nao^stoich)*(Kmca+cao)*(1+ksat*exp(1-gamma)FV/RT)) " \ 
    " if Na_msg = 0 then Na concentrations are initialized, no Na messages passed" \
    "   Requires calcium and sodium input message in concentration units."\
    " Requires Vm message, units indicated with Vunit field " \
	" addmsg from ncx to rxnpool CURRENT valence current"
		  
object	reaction	react_type	reaction	segment \
	-author	  "A. Blackwell ERIM Aug 96 ; revised GMU Dec 99" \
	-actions  INIT PROCESS RESET SET\
	-messages SUBSTRATE 0   1   conc \  /* concentration or quantity of substrates */
		  PRODUCT   1   1   conc \  /* concentration or quantity of products */
	-fields	  "kf = forward rate constant"\
		  "kb = backward rate constant"\
		  "kbprod = kb * products" \
		  "kfsubs = kf * substrates" \
	-description "An element to compute kf*substrates and kb*products for nth order reactions."\
		"dC/dt = kf * substrate * substrate * ... - kb*product* ..."\  
		"For each substrate and product in reaction, a message must be received (n times "\
		"if n moles required). Input and rate constants both must be in units of concentration,"\
		"(use RXN2 to send message to rxnpool) or both must be in units of moles (use RXN2MOLE)."\
	   	    "addmsg from reaction to substrate rxnpool RXN2 (or RXN2MOLES) kbprod kfsubs"\ 
	   	    "addmsg from reaction to product rxnpool RXN2 (or RXN2MOLES) kfsubs kbprod	"

object conservepool	conserve_type	conservepool	segment concentration\
	-author "A. Blackwell ERIM Aug 96; revised GMU Dec 99; GMU Mar 02" \
	-actions INIT PROCESS RESET CHECK \
	-messages CONC 	0   	1   	conc \ /* concentration of other forms */
		  MOLES	1	1	moles \ /* quantity of other forms */
		  VOLUME 2	1	volume \ /* volume of compartment */
	-fields "Conc = concentration of molecule"\
		"quantity = moles of molecule"\
		"Qinit = initial quantity of molecule"\
		"Qtot = total quantity of this and other forms of molecule"\
		"Cinit = initial concentration"\
		"Ctot = total concentration of this and other forms of molecule" \
		"Cmin = minimum concentration"\
		"type = whether specifying quantity or concentration of molecule"\
		"volume: used to convert from concentration to quantity and vice versa"\
		"units= 1 for moles, 1e-3 for mmoles, 1e-6 for umoles"\
		"Dunits = 1 for meters, 1e-4 for cm, etc"\
        "VolUnitConv: Dunits^3, converts volume to SI "\
	-description "Implementation of pool for conserved molecules. If type = 0, computes"\
		"concentration of molecule which is the total conc less the conc in all"\
		"other states. If type = 1, computes quantity of molecule which is the "\
		"total quantity less the quantity in all other states.  Then computes"\
 		"concentration by dividing by volume."

object	wgtavg	wgtavg_type	wgtavg	segment \
	-author	  "A. Blackwell GMU Oct 2011" \
	-actions  INIT PROCESS RESET\
	-messages  ValueWgt  0   2   Value wgt \  /* Value then weight */
	-fields	  "totValue = total weighted value"\
		  "totwgt = total weights"\
		  "meanValue = weighted mean"\
	-description "An element to weighted average.  Divides total weighted value by total weights."\ 
	   	    "Can calculated the mean calcium from unequal size calcium shells by using volume as weight."
