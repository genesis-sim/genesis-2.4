//genesis

float	PI		=	3.14159

// =====================================================================
//                        PHYSIOLOGICAL PARAMETERS
// =====================================================================
// All units in SI  and usually dimensions are given as well 

//                      IONIC EQUILIBRIUM POTENTIALS
float	ENA 		=	 55.0e-3			// V
float	ECL 		=	-65.0e-3			// V
float	EK 		=	-90.0e-3			// V
float	EREST 		=	-70.0e-3			// V
float	I_EREST		=	-70.0e-3			// V

//			ACTIVE COMPARTMENT POTENTIALS
float	EREST_ACT	=	-70.0e-3			// V
float 	ENA_ACT		=	115.0e-3 + EREST_ACT		// V
float	EK_ACT		=	-12.0e-3 + EREST_ACT		// V
float	ELEAK_ACT	=	 10.6e-3 + EREST_ACT		// V


// squid giant axon - Conti et al.(1975),J.Physiol(Lond),248,45-82
// gNa = 4pS
// squid giant axon - Conti and Neher(1980),Nature(Lond),285,140-143
// gK = 18pS
// mouse spinal neurone - Mathers and Barker(1982),Int.Rev.Neurobiol,23,1-34
// gCl = 18pS
//                           UNIT CONDUCTANCES
// 			     (Siemens/channel)
float	UNIT_GNA	=	12.0e-12	// S
float	UNIT_GCL	=	18.0e-12	// S
float	UNIT_GK		=	 4.0e-12	// S
float	SUNIT_GNA	=	 8.0e-12	// S
float	SUNIT_GK	=	 4.0e-12	// S

// squid giant axon - Conti et al.(1975),J.Physiol(Lond),248,45-82
// rhoNa = 330 channels/um^2
// squid giant axon - Conti and Neher(1980),Nature(Lond),285,140-143
// rhoK = 72 channels/um^2
//                           CHANNEL DENSITIES
// 				(channels/m^2)
float	RHO_NA		=	 60.0e12	
float	RHO_CL		=	 30.0e12
float	RHO_K		=	 30.0e12
float	SRHO_NA		=	330.0e12
float	SRHO_K		=	 72.0e12
float	IRHO_NA		=	 10.0e12

//                           SYNAPTIC AREA
// Haberly and Presto(1986),J.Comp.Neurol,248,464-474
// basal spine d = .40um
// distal spine d = .74um
// presynaptic to dendritic spines d = .61
// presynaptic to dendritic shafts d = .89
// presynaptic to initial segment d = .89
//
// using synaptic contact area A = pi*d^2/4 
// 				(m^2/synapse)
float	ASYN_LOCAL_NA	=	0.12e-12
float	ASYN_DISTAL_NA	=	0.43e-12
float	ASYN_CL		=	0.62e-12
float	ASYN_K		=	0.43e-12

float	IASYN_NA	=	0.29e-12

//                      ACTIVE AREA
// fraction of somatic area containing active channels
float	fAC		=	0.1


//                      PEAK CONDUCTANCE
//			(Siemens/synapse)
float	I_GMAX_NA	=	UNIT_GNA * IRHO_NA * IASYN_NA
float	LOCAL_GMAX_NA	=	UNIT_GNA * RHO_NA  * ASYN_LOCAL_NA
float	DISTAL_GMAX_NA	=	UNIT_GNA * RHO_NA  * ASYN_DISTAL_NA
float	GMAX_K		=	UNIT_GK  * RHO_K   * ASYN_K
float	GMAX_CL		=	UNIT_GCL * RHO_CL  * ASYN_CL

float	SGMAX_NA	=	SUNIT_GNA * SRHO_NA		// S/m^2
float	SGMAX_K		=	SUNIT_GK  * SRHO_K		// S/m^2

//                      MEMBRANE PARAMETERS
float	RM		=	0.2		// ohm-m^2
float	RA		=	0.5		// ohm-m
float	CM		=	0.01		// F/m^2

//			TYPICAL CELL DIMENSIONS

float	SOMA_D	=	20.0e-6				// m
float	SOMA_L	=	20.0e-6				// m
float	SOMA_A	=	PI * SOMA_D * SOMA_L		// m^2
float	SOMA_XA	=	PI * SOMA_D * SOMA_D / 4	// m^2
float	DEND_D	=	1.5e-6				// m
float	DEND_L	=	100.0e-6			// m
float	DEND_A	=	PI * DEND_D * DEND_L		// m^2
float	DEND_XA	=	PI * DEND_D * DEND_D / 4	// m^2

/*
** VARIABLES USED BY ACTIVE COMPONENTS
*/
int	EXPONENTIAL	=	1
int	SIGMOID		=	2
int	LINOID		=	3

