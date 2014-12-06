//genesis

// time scale factor 
// convert msec to sec
float SCALE = 1e-3
// no conversions
float SCALE = 1

// =====================================================================
//                          NETWORK DIMENSIONS
// =====================================================================

// cortical dimensions
// mm
float CORTEX_X = 10.0
// mm
float CORTEX_Y = 6.0

//                        PYRAMIDAL POPULATION

// # neurons in x 
int PYR_NX = 15
// # neurons in y
int PYR_NY = 9


echo Pyramidal array dimensions {PYR_NX} by {PYR_NY}

// cell spacing
// mm/cell 
float PYR_DX = CORTEX_X/PYR_NX
// mm/cell 
float PYR_DY = CORTEX_Y/PYR_NY

//                        FB INTERNEURON POPULATION

// # neurons in x 
int FB_NX = 1.0*PYR_NX
// # neurons in y
int FB_NY = 1.0*PYR_NY

echo FB interneuron array dimensions {FB_NX} by {FB_NY}

// mm/cell 
float FB_DX = CORTEX_X/FB_NX
// mm/cell 
float FB_DY = CORTEX_Y/FB_NY

//                        FF INTERNEURON POPULATION

// # neurons in x 
int FF_NX = 1.0*PYR_NX
// # neurons in y
int FF_NY = 1.0*PYR_NY

echo FF interneuron array dimensions {FF_NX} by {FF_NY}

// mm/cell 
float FF_DX = CORTEX_X/FF_NX
// mm/cell 
float FF_DY = CORTEX_Y/FF_NY

//                        AFFERENT POPULATION

// # neurons in x 
int BULB_NX = 10
// # neurons in y 
int BULB_NY = 1

echo Bulbar dimensions {BULB_NX} by {BULB_NY}

// shrink the scales to that of the LOT input dimensions 

// mm/fiber 
float BULB_DX = 1e-3
// mm/fiber 
float BULB_DY = 1e-3

// deep collaterals in rat - Haberly and Presto (1986), J.Comp.Neur.,248,464-474
// local axon collaterals remain in layer III for 1-2 mm
//                         PATHWAY EXTENTS

// 0.5 			// mm
float RPYR_to_LOCAL = 1.0
// mm
float RPYR_to_FB = 2.0
// mm
float RPYR_to_FF = 0.5
// mm
float RFB_to_PYR = 2.0
// mm
float RFF_to_PYR = 2.0
// mm
float RPYR_to_RDIST = 20.0
// mm
float RPYR_to_CDIST = 20.0

// =====================================================================
//                REPRESENTATIVE CELL DENSITY 
// =====================================================================
// The locus of each simulated cell represents some number of 
// actual cells. This can be specified as a fixed number which
// is invariant over the number of simulated cells. 
// This is equivalent to having the simulation represent a sparse sample
// of cortical cells versus a lumped representation of the total number of
// cells as reflected in the weight of synaptic enervation.
// The aim is to break the amplification which arises when single cells
// represent large numbers of cells (small simulations).
// The desire is to normalize the effect of a single cell on another cell
// to a reasonable, stable value.
// The total number of actual cells represented will vary with simulation
// size while the average input activity to a cell will remain more
// stable in that a given number of active cells in the simulated network will
// activate a consistent number of representative synapses.
// The total number of potential synaptic contacts onto a cell will
// vary directly with the ratio of cells included in the simulated cortex 
// versus the actual number of cells.
float DENSITY_FACTOR = 1.0

// =====================================================================
//                          CONNECTION DENSITY
// =====================================================================
// connection probabilities
float PPYR_to_RLOCAL = 1.2
float PPYR_to_CLOCAL = 1.2
float PPYR_to_RDIST = 1.1
float PPYR_to_CDIST = 1.1
float PPYR_to_FB = 0.2
if ({LOCAL_FF})
    float PPYR_to_FF = 0.2
else
    float PPYR_to_FF = 0.2
end
float PFB_to_PYR = 1.0
float PFF_to_PYR = 1.0
float PBULB_to_PYR = 0.1
float PBULB_to_FF = 0.1
float PBULB_to_FB = 0.1

// synaptic target count	(synapses/path)
float NSYN_PYR_from_BULB = 1200
float NSYN_PYR_from_CA = 1200
float NSYN_PYR_from_RA = 700
float NSYN_PYR_from_FB = 800
// standard deviation
float SDSYN_PYR_from_FB = 80
float NSYN_PYR_from_FF = 200
// standard deviation
float SDSYN_PYR_from_FF = 20
float NSYN_PYR_from_CLOCAL = 300
float NSYN_PYR_from_RLOCAL = 300

float NSYN_FB_from_PYR = 800
// standard deviation
float SDSYN_FB_from_PYR = 80

float NSYN_FB_from_FB = 80
// standard deviation
float SDSYN_FB_from_FB = 8

float NSYN_FF_from_PYR = 200
// standard deviation
float SDSYN_FF_from_PYR = 20

float NSYN_FB_from_BULB = 75
float NSYN_FF_from_BULB = 200

// source cell count 		(src_cells)
float NBULB_to_PYR = PBULB_to_PYR*BULB_NX*BULB_NY
float NBULB_to_FF = PBULB_to_FF*BULB_NX*BULB_NY
float NBULB_to_FB = PBULB_to_FB*BULB_NX*BULB_NY

// network space constants
// mm	
float LPYR_to_RLOCAL = 5.0
// mm	
float LPYR_to_CLOCAL = 5.0
// mm	
float LPYR_to_CA = 5.0
// mm	
float LPYR_to_RA = 5.0
// mm	
float LPYR_to_FB = 5.0
// mm	
float LPYR_to_FF = 5.0
// mm	
float LFB_to_PYR = 5.0
// mm	
float LFB_to_FB = 5.0
// mm	
float LFF_to_PYR = 5.0
// mm	
float LBULB_to_MAIN = 20.0
// mm	
float LBULB_to_COLL = 10.0


// connection normalization factors	(synapses/src_cell)
// calculate the convergent synapses to a predetermined target point
// and normalize the total number to a fixed value

//	target path		  target  path  src 
// this will normalize the number of synapses of the most rostral
// targets and not the central target used for other pathway 
// normalization
float WBULB_to_FF = NSYN_FF_from_BULB/NBULB_to_FF
float WBULB_to_FB = NSYN_FB_from_BULB/NBULB_to_FB
float WBULB_to_PYR = NSYN_PYR_from_BULB/NBULB_to_PYR

// asymptotic connection level		(fractional synapses/src_cell)
float AWPYR_to_RLOCAL = 0.2
float AWPYR_to_CLOCAL = 0.2
float AWPYR_to_CA = 0.6
// MODIFIED
float AWPYR_to_CA = 0.4
float AWPYR_to_RA = 0.4
float AWPYR_to_FB = 0.2
float AWPYR_to_FF = 0.2
float AWFB_to_PYR = 0.2
float AWFB_to_FB = 0.2
float AWFF_to_PYR = 0.2
float AWBULB_to_FF = 0.2*WBULB_to_FF
float AWBULB_to_FB = 0.2*WBULB_to_FB
float AWBULB_to_PYR = 0.2*WBULB_to_PYR

// velocities				(m/s)
// association fiber conduction velocity - Haberly (???) ,Neuro.Sci.Abs,213
// velocity of caudally directed = .25 to .48 m/s
// velocity of rostral directed = .45 to 1.25 m/s
float VPYR_CA = 0.6
float SD_VPYR_CA = 0.05
float VPYR_CA_MIN = 0.45
float VPYR_CA_MAX = 0.75

float VPYR_RA = 0.85
float SD_VPYR_RA = 0.15
float VPYR_RA_MIN = 0.45
float VPYR_RA_MAX = 1.25

float VPYR_FF = 1.0
float SD_VPYR_FF = 0.05
float VPYR_FF_MIN = 0.8
float VPYR_FF_MAX = 1.2

float VPYR_FB = 1.0
float SD_VPYR_FB = 0.05
float VPYR_FB_MIN = 0.8
float VPYR_FB_MAX = 1.2

// LOT conduction velocity opossum - Haberly (1973), J.Neurophys.,36:4,775-788
// measured using evoked potential latencies
// velocity of main LOT = 7.0 m/s
// velocity of LOT collaterals = 1.6 m/s
float VLOT_MAIN = 7.0
float SD_VLOT_MAIN = 0.05
float VLOT_MAIN_MIN = 6.8
float VLOT_MAIN_MAX = 7.2

float VLOT_COLL = 1.6
float SD_VLOT_COLL = 0.05
float VLOT_COLL_MIN = 1.4
float VLOT_COLL_MAX = 1.8

float VFB_PYR = 1.0
float SD_VFB_PYR = 0.05
float VFB_PYR_MIN = 0.8
float VFB_PYR_MAX = 1.2

float VFF_PYR = 1.0
float SD_VFF_PYR = 0.05
float VFF_PYR_MIN = 0.8
float VFF_PYR_MAX = 1.2

// =====================================================================
//                          CELL DIMENSIONS
// =====================================================================

float PI = 3.14159

// pyramidal cell dimensions

// um
float PYR_SOMA_D = 20
// um
float PYR_SOMA_L = 70
// um^2
float PYR_SOMA_A = PI*PYR_SOMA_D*PYR_SOMA_L
// um^2
float PYR_SOMA_XA = PI*PYR_SOMA_D*PYR_SOMA_D/4
// um
float PYR_DEND_D = 4
// um
float PYR_DEND_L = 120
// um^2
float PYR_DEND_A = PI*PYR_DEND_D*PYR_DEND_L
// um^2
float PYR_DEND_XA = PI*PYR_DEND_D*PYR_DEND_D/4

// interneuron dimensions
// Haberly et al. (1987),J.Comp.Neurol,266,269-290
// mean GABA positive somata diameters in layers I,II, and III 
// from EM (table 2, pg283).
//     globular I and II, md = 10.6um
//     multipolar and fusiform II and III, md = 15.0um
// um
float GLOB_SOMA_D = 10
// um
float GLOB_SOMA_L = 10

// um
float FUS_SOMA_D = 15
// um
float FUS_SOMA_L = 15

// =====================================================================
//                        PHYSIOLOGICAL PARAMETERS
// =====================================================================

//                      IONIC EQUILIBRIUM POTENTIALS
// mV
float EKA = 0
// mV
float ENA = 55
// mV
float ECL = -65
// mV
float EK = -90
// mV
float PYR_EREST = -55
// mV
float I_EREST = -55


// Kandel,Schwartz, Principles of Neuroscience, 2nd ed.  pg 83
// gNa 8-18 pS,  gK 4-12 pS
// squid giant axon - Conti et al.(1975),J.Physiol(Lond),248,45-82
// gNa = 4pS
// squid giant axon - Conti and Neher(1980),Nature(Lond),285,140-143
// gK = 18pS
// mouse spinal neurone - Study and Barker(1981),PNAS USA,78:11,7180-7184
// GABA gCl = 18pS +- 5
// mouse spinal neurone - Mathers and Barker(1982),Int.Rev.Neurobiol,23,1-34
// gCl = 18pS
// rat hippocampal neurone - Segal and Barker(1984),J.Neurophys.,51:3,500-515
// gCl = 20pS response to GABA, muscimol, and glycine
//                           UNIT CONDUCTANCES
// 				(mS/channel)
float UNIT_GNA = 8e-9
float UNIT_GK = 4e-9
float UNIT_GCL = 20e-9

float SUNIT_GNA = 8e-9
float SUNIT_GK = 4e-9


// mouse spinal neurone - Study and Barker PNAS USA, 1981,78:11, 7180-7184
// GABA tau_Cl = 18.3 msec
// GABA+PB tau_Cl = 30-130 msec
//                           CHANNEL LIFETIMES
//				(msec)
float TAU_CL = 18

// squid giant axon - Conti et al.(1975),J.Physiol(Lond),248,45-82
// rhoNa = 330 channels/um^2
// squid giant axon - Conti and Neher(1980),Nature(Lond),285,140-143
// rhoK = 72 channels/um^2
//                           CHANNEL DENSITIES
// 				(channel/um^2)
float RHO_NA = 30
float RHO_CL = 20
float RHO_K = 30
float SRHO_NA = 330
float SRHO_K = 72
float IRHO_NA = 15
float IRHO_CL = 7

//                           SYNAPTIC AREA
// Haberly and Presto(1986),J.Comp.Neurol,248,464-474
// basal spine d = .40um
// distal spine d = .74um
// presynaptic to dendritic spines d = .61um
// presynaptic to dendritic shafts d = .89um
// presynaptic to initial segment d = .89um

// using synaptic contact area A = pi*d^2/4 
// 				(um^2/synapse)
// d=.40
float ASYN_LOCAL_NA = 0.12
// d=.74
float ASYN_DISTAL_NA = 0.43
// d=.89
float ASYN_CL = 0.62
// d=.74
float ASYN_K = 0.43

float IASYN_NA = 0.29
// d=.89
float IASYN_CL = 0.62

//                           ACTIVE AREA
// fraction of somatic area containing active channels
float fAC = 0.04


// muse spinal neurone - Study and Barker PNAS USA, 1981,78:11, 7180-7184
// GABA gCl_peak = 112nS
//                           PEAK CONDUCTANCE
// peak_g = unit_g * channel_density * synaptic_area
//				(mS/synapse)
float I_GMAX_NA = UNIT_GNA*IRHO_NA*IASYN_NA
float I_GMAX_CL = UNIT_GCL*IRHO_CL*IASYN_CL
float LOCAL_GMAX_NA = UNIT_GNA*RHO_NA*ASYN_LOCAL_NA
float DISTAL_GMAX_NA = UNIT_GNA*RHO_NA*ASYN_DISTAL_NA
float GMAX_K = UNIT_GK*RHO_K*ASYN_K
float GMAX_CL = UNIT_GCL*RHO_CL*ASYN_CL

// mS/um^2
float SGMAX_NA = SUNIT_GNA*SRHO_NA
// mS/um^2
float SGMAX_K = SUNIT_GK*SRHO_K

//                           MEMBRANE PARAMETERS
// Kohm-um^2
float RM = 4e+8
// Kohm-um
float RA = 0.10e+4
// uF/um^2
float CM = 2.0e-8

// Kohm-um^2
float I_RM = 2.0e+8
// Kohm-um
float I_RA = RA
// uF/um^2
float I_CM = CM

// Kohm-um^2
float I_RM2 = 2.0e+8
// Kohm-um
float I_RA = RA
// uF/um^2
float I_CM = CM

// time constants for pyramidal and deep multipolar
// Tseng and Haberly, J.Neurophys, 62:386-400,1989
// pyramidal = 8.2 msec
// deep multipolar = 12.9 msec

/*
** VARIABLES USED BY ACTIVE COMPONENTS
*/
int EXPONENTIAL = 1
int SIGMOID = 2
int LINOID = 3

// mV
float PYR_EREST_ABS = -70
// mV
float PYR_ENa = 115.0 + PYR_EREST_ABS
// mV
float PYR_EK = -12.0 + PYR_EREST_ABS
// mV
float PYR_Eleak = 10.6 + PYR_EREST_ABS

// msec
float SYNDELAY = 0.8
// msec
float FFDELAY = 8.0

/*
** BASELINE WEIGHTS FOR EEG SIMULATIONS
*/

float weight_RA = 1.0
float weight_CA = 1.0
float weight_fb_pyr = 1.075
float weight_fb_fb = 0
float weight_ff_pyr = 1.0
// 400	
float weight_bulb_pyr = 100.0
// 0.5	// 1.0	
float weight_bulb_ff = 0.5
// 0.25 	// 0.025	
float weight_bulb_fb = 0.30
// 0.25	// 0.75	
float weight_pyr_fb = 0.30
// 1.0	// 1.5
float weight_pyr_ff = 0.75
float weight_pyr_local = 1.0

