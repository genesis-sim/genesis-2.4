//genesis
//
// $Id: seglib.g,v 1.2 2006/03/07 22:35:31 svitak Exp $
// $Log: seglib.g,v $
// Revision 1.2  2006/03/07 22:35:31  svitak
// Added izcell object to implement Izhikevich simple spiking cell model.
//
//
// Revision 2006/03/02 dbeeman
// Added izcell object to implement Izhikevich simple spiking cell model
//
// Revision 1.1.1.1  2005/06/14 04:38:28  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.10  1998/06/30 23:30:50  dhb
// EDS 22d merge
//
// EDS22c revison: EDS BBF-UIA 97/11/28-97/12/03
// CONNECTSPHERE, RAXIALS and RAXIALC msgs have been deleted.
//
// Revision 1.9  1997/07/18 20:42:42  dhb
// Fixed description "bug" in ghk object
//
// Revision 1.8  1997/05/29 09:03:27  dhb
// Updated from Antwerp GENESIS version 21e
//
// EDS20i revison: EDS BBF-UIA 95/06/07
// (Removed Ca_shell), updated Ca_concen description, added ghk
//
// Revision 1.7  1995/07/29  17:26:06  dhb
// Set default value for compartment and symcompartment Rm and Cm
// fields to 1 to avoid divide by zero FPE in uninitialized
// compartments.  Also added field descriptions for most of the
// [sym]compartment fields.
//
// Revision 1.6  1995/07/18  18:13:05  dhb
// Added description of thich field to Ca_concen and removed
// Ca_shell which has been taken over by Ca_concen.
//
// Revision 1.5  1994/09/20  18:26:41  dhb
// Moved addfuncs for getinput and clearbuffer back to buffer library.
// These commands operate on buffers.
//
// Revision 1.4  1994/06/02  20:37:27  dhb
// Added SET action to compartment and symcompartment objects.
//
// Revision 1.3  1993/07/22  19:56:54  dhb
// Added fixes for channelC3 and site objects (ACTIVATION message).
//
// Revision 1.2  1993/02/25  18:15:58  dhb
// Moved addfuncs for do_get_input (getinput) and do_clear_buffer
// (clearbuffer) from buflib.g to here, since these functions are
// defined here.
//
// Revision 1.1  1992/12/11  19:04:19  dhb
// Initial revision
//

// genesis

newclass concentration

/*
************************************************************************
**                       SEGMENT SHELL COMMANDS                     **
************************************************************************
*/
addfunc		balanceEm		do_balance_Em

/*
*************************************************************************
**                              OBJECT TABLE                           **
*************************************************************************
*/
//==========================================================================
//	name		data            function	class	
//==========================================================================

object	channelA	channelA_type	ChannelA      	segment channel	\
	-author 	"M.Wilson Caltech 6/88" \
	-actions 	INIT PROCESS RESET CHECK \
	-messages 	VOLTAGE 0 	1 Vm \		/* type 0: Vm */
			ACTIVATION 1	1 activation \	/* type 1: activation */
	-fields		"Ek = equilibrium potential" \
	-description	"Direct mapping of activation to conductance." \
			"Calculates Ik = (Ek - Vk)Gk."

object	channelB	channelB_type 	ChannelB      	segment channel \
	-author 	"M.Wilson Caltech 6/88" \
	-actions 	INIT PROCESS RESET CHECK \
	-messages 	VOLTAGE 0 	1 Vm \	
			ACTIVATION 1 	1 activation \
	-fields		tau_ak gain_ak tau_gk gain_gk  \
			"Ek = equilibrium potential"

object	channelC	channelC_type 	ChannelC      	segment channel \
	-author 	"M.Wilson Caltech 6/88" \
	-actions 	INIT PROCESS RESET CHECK RECALC SAVE2 RESTORE2 \
	-messages	ACTIVATION 	0	1 activation \
			RAND_ACTIVATION 1 	2 probability amplitude \
	-fields		"tau1 tau2  = time constants of channel activation" \
			"gmax = peak conductance" 

object	channelC2	channelC2_type 	ChannelC2      	segment channel \
	-author 	"M.Wilson Caltech 6/88" \
	-actions 	INIT PROCESS RESET CHECK RECALC SAVE2 RESTORE2 \
	-messages 	VOLTAGE 	0 	1 Vm \	
			ACTIVATION 	1 	1 activation \
			RAND_ACTIVATION 2 	2 probability amplitude \
	-fields		"tau1 tau2  = time constants of channel activation" \
			"gmax = peak conductance" \
			"Ik = channel current"

object	channelC3	channelC3_type 	ChannelC3      	segment channel \
	-author 	"E. De Schutter 11/91, M.Wilson Caltech 6/88" \
	-actions 	INIT PROCESS RESET CHECK RECALC SAVE2 RESTORE2 \
	-messages 	VOLTAGE 	0 	1 Vm \	
			ACTIVATION 	1 	1 activation \
	-fields		"tau1 tau2  = time constants of channel activation" \
			"gmax = peak conductance" \
			"Ik = channel current" \
			"frequency = frequency of background firing"

object	manuelconduct	manuelconduct_type ManuelConduct  segment channel \
	-author 	"M.Wilson Caltech 2/89" \
	-actions 	INIT PROCESS RESET CHECK \
	-messages	VOLTAGE 	0	1 Vm \
			ACTIVATION 	1	1 activation \
			RAND_ACTIVATION 2 	2 probability amplitude \
	-fields		"tau_g = opening time" \
			"tau_h = closing time" \
			"tau_d = transmitter decay time" \
			"K_r  = dissociation constant for transmitter binding" \
			"gmax = channel conductance per unit of transmitter"

/* Change by Upi : concen needs to be updated late in the schedule table
** otherwise it will be seeing old values for current */
object	Ca_concen	Ca_concen_type CaConcen  segment concentration \
	-author 	"M.Wilson Caltech 2/89, E. De Schutter 91" \
	-actions 	INIT PROCESS RESET CHECK SAVE2 RESTORE2 \
	-messages	I_Ca 0		1 I_Ca \
			fI_Ca 1		2 I_Ca fraction \
			INCREASE 2	1 I_Ca \  //any sign of Ik 
			DECREASE 3	1 I_Ca \  //any sign of Ik
			BASE 4		1 Ca_base \
	-fields		"tau: time constant of decay" \
				"Ca_base: equilibrium concentration" \
				"B: equals 1/(ion_charge * Faraday * volume)" \
                "thick: if zero concen is complete compartment, if >0"\
                "  concen is a submembrane shell with specified thickness."\
	-description	"Single pool model for Ca concentration. "

object	Mg_block	Mg_block_type	MgBlock segment channel	\
	-author 	"E. De Schutter Caltech 11/90" \
	-actions 	INIT PROCESS RESET CHECK \
	-messages 	VOLTAGE		0 	1 Vm \		/* type 0: Vm */
				CHANNEL1	1	1 Gk \		/* type 1: unblocked G*/
				CHANNEL2	2	2 Gk Ek \		/* type 1: unblocked G*/
				CHANNEL		2	2 Gk Ek \		/* type 1: unblocked G*/
				CHARGE		3	1 Zk \		/* update "charge value" */
	-fields		"Ik, Gk and Ek are the blocked values fo current k" \
				"Zk "charge", may also be used as {charge / relative" \
				" fraction of I}, e.g. 2/(Ica/Itot) -> msg to difpool" \
				"KMg_A = A-factor K(v)-equat for Mg block reaction" \
				"KMg_B = B-factor K(v)-equat for Mg block reaction" \
				"CMg = [Mg] in mM" \
	-description	"Implementation of voltage and [Mg]-dependent" \
					"blocking of a channel."

object	leakage		leakage_type 	Leakage  	segment channel \
	-author 	"M.Wilson Caltech 6/88" \
	-actions 	INIT PROCESS RESET CHECK \
	-messages 	VOLTAGE 0 	1 Vm \
	-fields		Ek Gk

object	compartment	compartment_type Compartment  	segment membrane \
	-author 	"M.Wilson Caltech 6/88" \
	-actions 	INIT PROCESS RESET CHECK SET SAVE2 RESTORE2 \
	-messages 	CHANNEL 0 	2 Gk Ek \
			RAXIAL	1	2 Ra Vm \
			AXIAL	2	1 Vm \
			INJECT	3	1 inject \
			EREST   6	1 Em \
	-readwrite Rm "membrane resistance" 1 \
	-readwrite Cm "membrane capacitance" 1 \
	-readwrite Em "resting potential" \
	-readwrite initVm "initial Vm value at reset" \
	-readwrite Ra "axial resistance" \
	-readwrite inject "injected current" \
	-readwrite dia "compartment diameter" \
	-readwrite Vm "compartment potential" \
	-fields		"Rm = membrane resistance" \
			"Cm = membrane capacitance" \
			"Em = resting potential" \
			"Ra = axial resistance" \
			"inject = injected current" \
			"dia,len = compartment sizes (not directly used numerically)" \
	-description 	"Axially asymmetric compartment. Ra is located on" \
			"one side of the compartment. This is slightly more" \
			"computationally efficient than the symmetric" \
			"counterpart."

object	symcompartment	symcompartment_type SymCompartment  segment membrane \
	-author 	"M.Wilson Caltech 5/89, E.De Schutter 11/90" \
	-actions 	INIT PROCESS RESET CHECK SET SAVE2 RESTORE2\
	-messages 	CHANNEL 0 	2 Gk Ek \
			RAXIAL	1	2 Ra Vm \
			RAXIAL2	2	2 Ra Vm \
			AXIAL	2	2 Ra Vm \
			CONNECTTAIL	1	2 Ra Vm \
			CONNECTHEAD	2	2 Ra Vm \
			INJECT	3	1 inject \
			CONNECTCROSS	5	2 Ra Vm \
			EREST   6	1 Em \
	-readwrite Rm "membrane resistance" 1 \
	-readwrite Cm "membrane capacitance" 1 \
	-readwrite Em "resting potential" \
	-readwrite initVm "initial Vm value at reset" \
	-readwrite Ra "axial resistance" \
	-readwrite inject "injected current" \
	-readwrite dia "compartment diameter" \
	-readwrite Vm "compartment potential" \
	-fields		"Rm = membrane resistance" \
			"Cm = membrane capacitance" \
			"Em = resting potential" \
			"Ra = total axial resistance" \
			"inject = injected current" \
			"dia,len = compartment sizes (not directly used numerically)" \
	-description 	"Axially symmetric compartment. Ra is divided into" \
			"two equal components across the compartment."

object	unit		unit_type 	Unit 		segment 	\
	-author 	"M.Wilson Caltech 6/88" \
	-actions	INIT RESET PROCESS CHECK \
	-messages 	INJECT 	0 	1 inject \
	-fields		Em inject

object	RC		RCunit_type 	RC_Unit		segment 	\	
	-author 	"M.Wilson Caltech 6/88" \
	-actions	RESET PROCESS CHECK \
	-messages 	INJECT 	0 	1 inject \
	-fields		V0 R C state inject method

object	site		site_type 	Site 		segment 	\
	-author 	"M.Wilson Caltech 6/88" \
	-actions	INIT RESET PROCESS \
	-messages	ACTIVATION 	1 	1 activation

object   ghk        ghk_type    Ghk     channel device \
    -author     "S. Shankar 8/91" \
    -actions    RESET PROCESS CHECK \
    -messages   Cin     0   1 intracell_concen \
            CIN     0   1 intracell_concen \
            Cout    1   1 extracell_concen \
            COUT    1   1 extracell_concen \
            VOLTAGE 3   1 Vm    \
            PERMEABILITY 4  1 p     \
    -fields     "Ek      : reversal potential" \
            "Gk      : chord conductance" \
            "Ik      : current for this ion" \
            "T       : temperature, deg C" \
            "p       : permeability" \
            "Vm      : membrane potential" \
            "Cin     : concentration inside cell" \
            "Cout    : concentration outside cell"\
            "valency : charge on ion" \
    -description    "Calculates the Goldman-Hodgkin-Katz (constant" \
            "field) equation for a single ionic species.  Provides" \
            "current as well as reversal potential and chord" \
            "conductance.  Note that units on output fields" \
            "correspond to input field units, and inward current" \
            "is positive."

object  izcell     izcell_type IzCell    segment membrane \
        -author         "Dave Beeman, Aug 2004" \
        -actions        INIT PROCESS RESET CHECK \
        -messages       CHANNEL 1       2 Gk Ek \
                        INJECT  0       1 inject \
        -readwrite A "A parameter" 0 \
        -readwrite B "B parameter" 0 \
        -readwrite C "C parameter" 0 \
        -readwrite D "D parameter" 0 \
        -readwrite coeff2 "coeff of v*v in dv/dt" 40000 \
        -readwrite coeff1 "coeff of v in dv/dt" 5000 \
        -readwrite coeff0 "constant term in dv/dt" 140.0 \
        -readwrite inject "injected current density * 100" \
        -readwrite Vm "cell potential" \
        -readonly state "= 1 if cell fired this time step" \
        -readwrite Vmax "maximum action potential height" 0.03 \
        -description "Implementation of the Izhikevich simple spiking" \
      "neuron model.  Vm is determined from the equation:"\
      "   dVm/dt = coeff2*Vm*Vm + coeff1*Vm + coeff0 - u + I"\
      "with du/dt = A*(B*Vm - u), and the spike reset condition:" \
      "   if (Vm >= Vmax) then Vm = C; u = u +D"
