// genesis
//
// $Id: conclib.g,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $
// $Log: conclib.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:29  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.4  1999/10/17 22:13:23  mhucka
// New version from Erik De Schutter, dated circa March 1999.
//
//
// EDS22h: 1999/03/23 eds
// added TUBE shape_mode
//
// EDS22b: 1997/11/14 eds
// added hillpump object and messages to difshell, concpool, improved comments
//
// Revision 1.2  1997/05/28 21:09:29  dhb
// Complete update from Antwerp GENESIS
//
// Revision 1.1  1992/12/11  19:02:46  dhb
// Initial revision
//

/* Version EDS22b 97/09/22, Erik De Schutter, BBF-UIA 8/94-9/97 */

newclass concbuffer
/*
************************************************************************
**                    CONCENTRATION  SHELL COMMANDS                    **
************************************************************************
*/
// values for shape_mode
int SHELL = 0
int SLICE = 1
int SLAB = 1
int TUBE = 2
int USERDEF = 3

/*
*************************************************************************
**                              OBJECT TABLE                           **
*************************************************************************
*/

//==========================================================================
//	name		data            function	class	
//==========================================================================

object	concpool	concpool_type	ConcPool	segment	concentration \
	-author	  "E. De Schutter BBF-UIA 9/95" \
	-actions  INIT PROCESS RESET RECALC CHECK SAVE2 RESTORE2 \
	-messages CONCEN   0	1  C \    /* difshell concentration */
		  STOREINFLUX 4  1 flux \	/* flux into store */
		  STOREOUTFLUX 5  1 flux \	/* flux out store */
		  BUFFER  10  4	kBf kBb Bfree Bbound \ /* buffer*/
		  HILLPUMP  14  3  vmax Kd Hill \ /* Michaelis Menten pump */
	-fields	"C = concentration (integrated) in mM" \
		"Ceq = equilibrium concentration in mM" \
		"val = ionic valence" \
		"leak = constant in/outflow in mM/s" \
		"shape_mode;  SHELL, SLAB, TUBE, USERDEF" \
		"other fields depend on shape_mode setting:" \
		"SHELL: onion shell, radial diffusion. Set len (zero length for" \
		"       spherical shell), dia (outer diameter shell) and thick" \
		"       thickness shell), other fields computed." \
		"SLICE: salami slices of cylinder, axial diffusion. Set dia" \ 
		"      (diameter cylinder) and thick (thickness slab), len not" \
		"      used, other fields computed." \
		"TUBE: a cylinder with diameter dia and length len.  For" \
		"      concpools inside a difshell: make dia negative in" \
		"      prototyp; readcell will scale len so as to obtain an" \
		"      appropriate change in vol relative to the difshell." \
		"USERDEF: len, dia, thick not used. Set vol, surf_up and surf_down." \
	-description "Concentration pool without diffusion.  Has ionic current" \
		"flow, first order buffering and pumps." \
		"The FLUX messages assumes SI units for the Faraday " \
		"constant (mM, m, m^3, A), if not scale val.  Use INFLUX for an" \
		"increase with positive current, OUTFLUX for a decrease."

object	difshell	difshell_type	DifShell	segment	concentration \
	-author	  "E. De Schutter BBF-UIA 4/94-9/95" \
	-actions  INIT PROCESS RESET RECALC CHECK SAVE2 RESTORE2 \
	-messages INFLUX 0      1 I_Ca \	/* current flow (in A) */
		  OUTFLUX 1     1 I_Ca \	/* ionic current flow (in A) */
		  FINFLUX 2     2 I_Ca fraction \	/* scaled ionic current flow */
		  FOUTFLUX 3     2 I_Ca fraction \	/* scaled ionic current flow */
		  STOREINFLUX 4  1 flux \	/* flux into store */
		  STOREOUTFLUX 5  1 flux \	/* flux out store */
		  DIFF_DOWN 6  2 prev_C thick \ /* diffusion to inner shell */
		  DIFF_UP 7  2	prev_C thick \ /* diffusion to outer shell */
		  BUFFER  10  4	kBf kBb Bfree Bbound \ /* buffer*/
		  TAUPUMP   11  2  kP  Ceq \ /* tau pump */
		  EQTAUPUMP 12  1  kP  \ /* tau pump */
		  MMPUMP    13  2  vmax Kd \ /* Michaelis Menten pump */
		  HILLPUMP  14  3  vmax Kd Hill \ /* Michaelis Menten pump */
	-fields	"C = concentration (integrated) in mM" \
		"Ceq = equilibrium concentration in mM" \
		"D = diffusion constant in m^2/s" \
		"val = ionic valence" \
		"leak = constant in/outflow in mM/s" \
		"shape_mode;  SHELL, SLAB or USERDEF" \
		"other fields depend on shape_mode setting:" \
		"SHELL: onion shell, radial diffusion. Set len (zero length for" \
		"       spherical shell), dia (outer diameter shell) and thick" \
		"       thickness shell), other fields computed." \
		"SLICE: salami slices of cylinder, axial diffusion. Set dia" \ 
		"      (diameter cylinder) and thick (thickness slab), len not" \
		"      used, other fields computed." \
		"USERDEF: len, dia, thick not used. Set vol, surf_up and surf_down." \
	-description "Concentration shell.  Has ionic current flow" \
		"one-dimensional diffusion, first order buffering and pumps." \
		"The FLUX messages assumes SI units for the Faraday " \
		"constant (mM, m, m^3, A), if not scale val.  Use INFLUX for an" \
		"increase with positive current, OUTFLUX for a decrease."

object	taupump	taupump_type	TauPump		segment	gate \
	-author	  "E. De Schutter Caltech 11/90" \
	-actions   INIT PROCESS RESET CHECK \
	-messages  VOLTAGE 0  1	 Vm \	/* type 0: voltage */
	-fields	"kP = pump rate=inverse of tau in s" \
		"Ceq = equilibrium concentration in mM" \
		"T_A = factor for Tau" \
		"T_B = exp factor for Tau" \
		"T_C = constant offset for Tau" \
		"T_V = half-voltage for Tau" \
	-description "Pump with variable time constant, tau is" \
		"fixed to T_C (or to T_A if T_C==0) if no VOLTAGE msg is sent." 
	
object	mmpump	mmpump_type	MMPump		segment	gate \
	-author	  "E. De Schutter BBF-UIA 9/94" \
	-actions   INIT PROCESS RESET CHECK \
	-messages CONCEN  0  1 C  \ /* type 0: use for electrogenic effect only */ 
		  MMKD 1 1 Km  \	/* type 1: changes Kd value */
	-fields	"vmax = maximum pump velocity, scaled by surface (mmol/sec)" \
		"Kd =  half-maximal activating concentration in mM" \
		"val = charge of ion" \
		"Ik = computed electrogenic effect (if val!=0)" \
	-description "Michaelis Menten plasma membrane pump: to be coupled to"\
		  "a difshell.  For pump action only a MMPUMP msg out is"\
		  "needed.  The electrogenic effect (val!=0) can also be"\
		  "simulated by using a CONCEN msg in and an INJECT Ik msg"\
		  "out to the compartment." 
	
object	hillpump  hillpump_type	HillPump		segment	gate \
	-author	  "E. De Schutter BBF-UIA 11/97" \
	-actions   INIT PROCESS RESET CHECK \
	-messages MMKD 1 1 Km  \	/* type 1: changes Kd value */
	-fields	"vmax = maximum pump velocity, scaled by surface (mmol/sec)" \
		"Kd =  half-maximal activating concentration in mM" \
		"Hill = Hill coefficient" \
	-description "Michaelis Menten store pump: to be coupled to a difshell"\
	          "(removes ions from it) and optionally to a concpool (adds"\
		  "ions to it). For pump action only 1 or 2 HILLPUMP msgs out"\
		  "are needed."
	
object	fixbuffer fixbuffer_type  FixBuffer	segment	concbuffer \
	-author	  "E. De Schutter BBF-UIA 8/94" \
	-actions  INIT PROCESS RESET CHECK SAVE2 RESTORE2 \
	-messages CONCEN  0  1	C  \   /* type 0: [buffered ion] */
	-fields	"Bfree = free buffer concentration (integrated) in mM" \
		"Btot = total buffer concentration in mM (fre+bound)" \
		"kBf = forward rate constant in 1/mM.s" \
		"kBb = backward rate constant in 1/s" \
	-description "First order fixed buffer."
	
object	difbuffer difbuffer_type  DifBuffer	segment	concbuffer \
	-author	  "E. De Schutter BBF-UIA 8/94" \
	-actions  INIT PROCESS RESET RECALC CHECK SAVE2 RESTORE2 \
	-messages CONCEN  0  1	C  \   /*  [buffered ion] */
		  BDIFF_DOWN 1  2 prev_free thick \ /* buffer diffusion inward */
		  DIFF_DOWN 1  2 prev_free thick \ /* buffer diffusion inward */
		  BDIFF_UP 2  2	prev_free thick \ /* buffer diffusion outward */
		  DIFF_UP 2  2	prev_free thick \ /* buffer diffusion outward */
	-fields	"Bfree = free buffer concentration (integrated) in mM" \
		"Bbound = bound buffer concentration in mM" \
		"Btot = total buffer concentration in mM, only used for reset" \
		"kBf = forward rate constant in 1/mM.s" \
		"kBb = backward rate constant in 1/s" \
		"D = diffusion constant in m^2/s" \
		"shape_mode;  SHELL, SLAB or USERDEF" \
		"other fields depend on shape_mode setting:" \
		"SHELL: onion shell, radial diffusion. Set len (zero length for" \
		"       spherical shell), dia (outer diameter shell) and thick" \
		"       thickness shell), other fields computed." \
		"SLICE: salami slices of cylinder, axial diffusion. Set dia" \ 
		"      (diameter cylinder) and thick (thickness slab), len not" \
		"      used, other fields computed." \
		"USERDEF: len, dia, thick not used. Set vol, surf_up and surf_down." \
	-description "First order diffusable buffer. D Bfree == D Bbound."
	
object	dif2buffer dif2buffer_type  Dif2Buffer	segment	concbuffer \
	-author	  "E. De Schutter BBF-UIA 4/96" \
	-actions  INIT PROCESS RESET RECALC CHECK SAVE2 RESTORE2 \
	-messages CONCEN  0  1	C  \   /*  [buffered ion] */
		  BDIFF_DOWN 1  3 prev_free prev_bound thick \ /* diffusion inward */
		  BDIFF_UP 2  3	prev_free prev_bound thick \ /* diffusion outward */
	-fields	"Bfree = free buffer concentration (integrated) in mM" \
		"Bbound = bound buffer concentration (integrated) in mM" \
		"Btot = total buffer concentration in mM, only used for reset" \
		"kBf = forward rate constant in 1/mM.s" \
		"kBb = backward rate constant in 1/s" \
		"Dfree = diffusion constant for Bfree in m^2/s" \
		"Dbound = diffusion constant for Bbound in m^2/s" \
		"shape_mode;  SHELL, SLAB or USERDEF" \
		"other fields depend on shape_mode setting:" \
		"SHELL: onion shell, radial diffusion. Set len (zero length for" \
		"       spherical shell), dia (outer diameter shell) and thick" \
		"       thickness shell), other fields computed." \
		"SLICE: salami slices of cylinder, axial diffusion. Set dia" \ 
		"      (diameter cylinder) and thick (thickness slab), len not" \
		"      used, other fields computed." \
		"USERDEF: len, dia, thick not used. Set vol, surf_up and surf_down." \
	-description "First order diffusable buffer. D Bfree != D Bbound"
	
object	fura2 fura2_type  FuraRatio	segment	concentration \
	-author	  "E. De Schutter BBF-UIA 1/95" \
	-actions  PROCESS RESET \
	-messages CONCEN  0  3	Bfree Bbound vol  \  /* from all fura2 difbuffers */
	-fields	"F340 = F340 fluorescence" \
		"F380 = F380 fluorescence" \
		"ratio = F340/F380" \
	-description "Computes fura2 fluorescence in a single difshell."
