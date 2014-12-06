//genesis  - Purkinje cell M9 genesis2.1 script
/* Copyright E. De Schutter (Caltech and BBF-UIA) */

/* This file defines important simulation parameters, including gmax
** and Ek for all channels.  Everything uses SI units. */

int include_purk_const

if ( {include_purk_const} == 0 )

	include_purk_const = 1


echo Using Purkinje 2M9 preferences. Crank Nicholson method.
/* variables controlling hsolve integration */
float dt = 2.0e-5
int tab_xdivs = 149; int tab_xfills = 2999
/* The model is quite sensitive to these values in NO_INTERP (caclmode=0) */
float tab_xmin = -0.10; float tab_xmax = 0.05; float Ca_tab_max = 0.300
// only used for proto channels
float GNa = 1, GCa = 1, GK = 1, Gh = 1
/* cable parameters */
float CM = 0.0164 		// *9 relative to passive
float RMs = 1.000 		// /3.7 relative to passive comp
float RMd = 3.0
float RA = 2.50
/* preset constants */
float ELEAK = -0.0800 		// Ek value used for the leak conductance
float EREST_ACT = -0.0680 	// Vm value used for the RESET
/* concentrations */
float CCaO = 2.4000 		//external Ca as in normal slice Ringer
float CCaI = 0.000040		//internal Ca in mM
//diameter of Ca_shells
float Shell_thick = 0.20e-6
float CaTau = 0.00010	 	// Ca_concen tau
/* Currents: Reversal potentials in V and max conductances S/m^2 */
/* Codes: s=soma, m=main dendrite, t=thick dendrite, d=spiny dendrite */
float ENa = 0.045
float GNaFs = 75000.0
float GNaPs = 10.0
float ECa = 0.0125*{log {CCaO/CCaI}} // 0.135 V
float GCaTs = 5.00
float GCaTm = 5.00
float GCaTt = 5.00
float GCaTd = 5.00
float GCaPm = 45.0
float GCaPt = 45.0
float GCaPd = 45.0
float EK = -0.085
float GKAs = 150.0
float GKAm = 20.0
float GKdrs = 6000.0
float GKdrm = 600.0
float GKMs = 0.400
float GKMm = 0.100
float GKMt = 0.130
float GKMd = 0.130
float GKCm = 800.0
float GKCt = 800.0
float GKCd = 800.0
float GK2m = 3.90
float GK2t = 3.90
float GK2d = 3.90
float Eh = -0.030
float Ghs = 3.00
/* synapses: */
float E_GABA = -0.080
//float G_GABA = 70.0/dt
float G_GABA = 70.0
//float GB_GABA = 20.0/dt
float GB_GABA = 20.0
float E_non_NMDA = 0.000
//float G_par_syn = 750.0/dt
float G_par_syn = 750.0
//float G_cli_syn = 150.0/dt
float G_cli_syn = 150.0


end


