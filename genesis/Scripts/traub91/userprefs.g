// genesis

// Customized userprefs.g to run the "Traub91" simulation with neurokit

echo Using local user preferences

/**********************************************************************
**
**	1	Including script files for prototype functions
**
**********************************************************************/

/* file for standard compartments */
include compartments
/* file for 1991 Traub model channels */
include traub91proto.g

/**********************************************************************
**
**	2	Invoking functions to make prototypes in the /library element
**
**********************************************************************/

/* To ensure that all subsequent elements are made in the library */
	ce /library

	make_cylind_compartment		/* makes "compartment" */
	make_cylind_symcompartment	/* makes "symcompartment" */

/* Assign some constants to override those used in traub91proto.g */
EREST_ACT = -0.06       // resting membrane potential (volts)
float ENA = 0.115 + EREST_ACT // 0.055  when EREST_ACT = -0.060
float EK = -0.015 + EREST_ACT // -0.075
float ECA = 0.140 + EREST_ACT // 0.080


make_Na
make_Ca
make_K_DR
make_K_AHP
make_K_C
make_K_A
make_Ca_conc

ce /		/* return to the root element */

/**********************************************************************
**
**	3	Setting preferences for user-variables.
**
**********************************************************************/
/* See defaults.g for default values of these */

user_symcomps = 1 // symmetric
user_intmethod = 11
str user_help = "traub_tut.help"

float user_wx = 2e-3  // wx, wy, cx, cy, cz for the draw widget
float user_wy = 2e-3
float user_cx = -0.001
float user_cy = 0.0
float user_cz = 0.2e-3

user_cell = "/CA3"
user_pfile = "CA3.p"

user_runtime = 0.10
user_dt = 50e-6  // 0.05 msec
user_refresh = 10

user_inject = 0.0 		// default injection current (nA)

// Set the scales for the graphs in the two cell windows
user_ymin1 = -0.1
user_ymax1 = 0.15
user_xmax1 = 0.1
user_xmax2 = 0.1
user_ymin2 = 0.0
user_ymax2 = 5e-7
user_yoffset2 = 0.0

// user_colmax2 = 10000 this should keep it black, even if colfix doen't work
/*  This displays the second cell window and plots the "Gk" field of the
    "Ca" channel for the compartment in which a recording electrode
    has been planted.  The default values of the field and path
    are "Vm" and ".", meaning to plot the Vm field for the compartment
    which is selected for recording.
*/

user_numxouts = 1
user_field2	= "Gk"
user_path2 = "Ca"
