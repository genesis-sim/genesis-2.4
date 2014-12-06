// genesis
/**********************************************************************
** This program is part of the kinetics library and is
**           copyright (C) 1995 Upinder S. Bhalla.
** It is made available under the terms of the
**           GNU Library General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/
/*
************************************************************************
**                       KINETICS SHELL COMMANDS                           **
************************************************************************
*/

addfunc copyleft_kin copyleft_kin
addfunc resetfastmsg do_reset_alloc_msgdata
addfunc clonemsgs do_clone_msgs
addfunc dirlist do_dir_list char**
addfunc fileexists do_file_exists int
addfunc maxfileversion do_max_file_version int
addfunc randseed2 do_srandom2 int
addaction TO_XTEXT 225
addaction FROM_XTEXT 226
addaction LOAD 227

/*
*************************************************************************
**                              OBJECT TABLE                           **
*************************************************************************
*/
//==========================================================================
//	name		data            function	class	
//==========================================================================



object	pool	pool_type PoolFunc	segment concentration \
	-author 	"U.S.Bhalla MSSM Aug/93" \
	-actions 	CREATE RESET PROCESS SET \
	-messages REAC	0 	2 A B 	\	 // state variable increments
		MM_PRD	1	1 A 	\	 // only 1 state variable here
		SLAVE	2	1 n \  // value of # to follow
		REMAINING 3	1 n \  // value of # to subtract
		CONSERVE 4	2 n nInit \  // # to add to do conservation.
		VOL		5	1 vol \	// volume of pool
		SUMTOTAL 6	2 n nInit \  // # to add to make total.
		MIRROR 7	1 n \  // Make src pool a mirror pool.
	-fields	"n"		"Free # of reagent" \
		"nTotal"	"Total # of reagent in all forms" \
		"nInit"		"Initial # of Co following RESET" \
		"nRemaining"	"Total # of pool in combined forms" \
		"nMin"		"minimum allowed # of pool" \
		"vol"		"Volume of pool" \
		"Co"		"Free concentration of reagent" \
		"CoTotal"	"Total conc of reagent in all forms" \
		"CoInit"	"Initial conc of Co following RESET" \
		"CoRemaining"	"Total conc of pool in combined forms" \
		"CoMin"		"minimum allowed Co of pool" \
		"slave_enable"	"value is 1 or 0. Enables slave mode" \
		"consv_flag"	"Internal flag." \
	-description	"A concentration pool used in reactions."\
	"Does Michaelis-Menten enzyme kinetics when hooked to the 'enz'"\
	"object, and standard kinetic reactions with the 'reac' object."\
	"The MM scheme is modeled as:" \
		"				 k1		k3"\
		" Substrate (S) + Enzyme (Co)  <----> CoComplex ----> Co + Prd"\
		"				k2"\
		"The Bimol scheme is:"\
		"				 kf"\
		" Substrate1 + S2 + ..         <----> Co + Prd1 + Prd2 + ..."\
		"				kb"\
	"Note that vol, n and Co are interdependent. vol is never changed" \
	"except by the user or by messages. n changes if Co is changed." \
	"Co changes if either vol or n is changed. All calculations are" \
	"done in terms of n, and, where needed, vol." \
	"'pool' should be considered the foundation class for more complex"\
	"objects which do e.g., diffusion and nernst potentials"

object	reac	reac_type ReacFunc	segment \
	-author 	"U.S.Bhalla MSSM Aug/93" \
	-actions 	CREATE RESET PROCESS SET SHOW \
	-messages SUBSTRATE	0 	1 n \	 // # of pool
		PRODUCT		1	1 n \ // # of pool
		KF		2	1 kf \ // forward rate constt
		KB		3	1 kb \ // backward rate constt
	-fields	"A"		"State variables for return msgs to pools" \
		"B"		"State variables for return msgs to pools" \
		"kf"		"Forward and Backward rate consts" \
		"kb"		"Forward and Backward rate consts" \
		"stoch_number"	"Upper mol limit for mixed stoch calculation" \
		"stoch_propensity"	"Upper propensity limit for mixed stoch calculation" \
	-description	"A reaction kinetics element which handles " \
	"standard kinetic reaction schemes for any number of substrates"\
	"and products. Can interface with 'pool'. The state variables "\
	"A and B are used in the return messages to the substrates and"\
	"products, except that they are swapped for the products."\
		"The Bimol scheme is:"\
		"				 kf"\
		" Substrate1 + S2 + ..         <----> Co + Prd1 + Prd2 + ..."\
		"				kb"


object	enz	enz_type EnzFunc	segment \
	-author 	"U.S.Bhalla MSSM Aug/93" \
	-actions 	CREATE RESET PROCESS SET \
	-messages ENZYME	0 	1 n \	 // # of enzyme
		SUBSTRATE	1	1 n \ // # of substrate
		VOL	2	1 vol \ // Vol of enzyme
		INTRAMOL	3	1 n \ // total # of molecules in intramol reac
	-fields \
		"sA"	"State variables for return msgs to pools" \
		"pA"	"State variables for return msgs to pools" \
		"eA"	"State variables for return msgs to pools" \
		"B"		"State variables for return msgs to pools" \
		"CoComplexInit"	"Initial conc of CoComplex" \
		"CoComplex"	"Enzyme complex formed in MM scheme" \
		"k1"		"Michaelis-Menten kinetic parms" \
		"k2"		"Michaelis-Menten kinetic parms" \
		"k3"		"Michaelis-Menten kinetic parms" \
	-description	"A reaction kinetics element which handles " \
	"Michaelis-Menten enzyme kinetics for the 'pool' object"\
	"There can be multiple SUBSTRATE pools but only 1 enzyme pool "\
	"The MM scheme is modeled as:" \
		"				 k1		k3"\
		" Substrate (S) + Enzyme (Co)  <----> CoComplex ----> Co + Prd"\
		"				k2"\
	"The fields for the return messages are:"\
	"to the enzyme: eA and B"\
	"to the substrate(s): sA and B"\
	"to the product(s): pA"\
	"The INTRAMOL msg sets total # of molecules in intramol reacn."


object	concchan	concchan_type ConcchanFunc	segment \
	-author 	"U.S.Bhalla MSSM Aug/93" \
	-actions 	RESET PROCESS SET \
	-messages SUBSTRATE	0 	2 n vol \	 // # and vol of pool
		PRODUCT		1	2 n vol \ // # and vol of pool
		NUMCHAN		2	1 n \ // Number of channels
		Vm		3	1 Vm \ // Voltage across channel
	-fields	"A"		"State variables for return msgs to pools" \
		"B"			"State variables for return msgs to pools" \
		"perm"		"Permeability" \
		"gmax"		"conductance" \
		"n"			"number of concchans" \
		"Vm"		"Potential across channel" \
	-description	"A concchan element handles " \
	"flow of molecules across a membrane. "\ 
	"Is normally used in conjunction with a pool. Numerically"\
	"the concchan is equivalent to 1-d diffusion. The state variables"\
	"A and B are used in the return messages to the substrates and"\
	"products, except that they are swapped for the products."

object	text	text_type TextFunc	segment \
	-author 	"U.S.Bhalla NCBS May/2001" \
	-actions 	SET TO_XTEXT FROM_XTEXT SAVE DUMP LOAD CREATE COPY \
	-fields	"text"	"Text string." \
	-readonly "len"	"Length of text string." \
	-description	"Handles arbitrarily long text strings." \
	"Also talks to xtext for getting and setting these strings. "\ 
	"Also deals with saving and loading from files."
object	gsolve	gsolve_type GsolveFunc	segment concentration \
	-author 	"Karan Vasudeva 2002" \
	-actions 	CREATE RESET PROCESS SET DELETE \
	-fields	"n"		"Free # of reagent" \
			"internal_time"	"Time of most recent transition" \
	-description	"TBA."

	//-messages // REAC	0 	2 kf kb \	 // rate consts \
		//POOL	1	3 n nInit slave_enable 	\ // Number of molecules \

