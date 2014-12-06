// genesis

// Customized userprefs.g to run the molluscan burster simulation with neurokit
echo Using local user preferences

/**********************************************************************
**
**	1	Including script files for prototype functions
**
**********************************************************************/

/* file for standard compartments */
include compartments

/* file for channels */
include ASTchan_tut
/**********************************************************************
**
**	2	Invoking functions to make prototypes in the /library element
**
**********************************************************************/

/* To ensure that all subsequent elements are made in the library */
	ce /library

	make_cylind_compartment		/* makes "compartment" */

// override defaults of -60 mV and -63 mV to give typical Aplysia R15 values
EK = -0.068
EA =-0.068

make_Na_aplysia_ag	// Na
make_K_trit_agt		// K
make_KA_moll_cs		// A
make_Ca_aplysia_ag	// Ca
make_B_trit_st		// B
make_Ca_conc		// Ca_conc
make_Kc_aplysia_gt	// K_C

// Eliminate K inactivation - it has little effect
setfield K Ypower 0

ce /		/* return to the root element */
disable /library

/**********************************************************************
**
**	3	Setting preferences for user-variables.
**
**********************************************************************/

/* See defaults.g for default values of these */
str user_help = "README"

user_cell = "/mollusc"
user_pfile = "mollusc.p"

user_runtime = 40.0
//user_intmethod = 0
//user_dt = 0.0005  // 0.5 msec if method 0
user_intmethod = 11  // Hines Crank-Nicholson
user_dt = 0.002    // 2 msec if method 11
user_refresh = 5

user_inject = 0  // default injection current (nA)

// Set the scales for the graphs in the two cell windows
user_ymin1 = -0.1
user_ymax1 = 0.05
user_xmax1 = 40.0
user_xmax2 = 40.0
user_ymin2 = 0.0
user_ymax2 = 200e-6
user_yoffset2 = 0.0

user_numxouts = 2
user_field2	= "Ca"
user_path2 = "Ca_conc"
user_colmin2 = 0        // use appropriate color scale for Ca_conc values
user_colmax2 = 0.00015

include I_plots  // "run paradigm" will plot currents, but slows execution
