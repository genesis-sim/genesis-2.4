// genesis

// $Id: newconnlib.g,v 1.3 2006/01/09 16:29:34 svitak Exp $
// Version EDS21b 96/08/02
//
// RCS Log messages at end of file


newclass synchannel 
newclass spiking
addaction RESETBUFFER 1001 

object	synchan	        Synchan_type Synchan segment channel synchannel              \
	-author 	"Mike Vanier 11/94"                                          \ 
	-actions 	CREATE INIT PROCESS HPROCESS RESET CHECK RECALC SAVE2 RESTORE2 EVENT  \
			ADDMSGIN DELETEMSGIN MSGINDELETED RESETBUFFER DELETE COPY SET SHOW    \
	-messages 	VOLTAGE 	0 	1 Vm                                 \	
			ACTIVATION 	1 	1 activation                         \
			RAND_ACTIVATION 2 	2 probability amplitude              \
			MOD		3	1 modulation                         \
			SPIKE	       -1	0                                    \
	-readonly       activation            "driving force for channel conductance"                    \
	-readwrite		Ik                    "channel current"                                          \
	-readwrite		Gk                    "conductance of channel"                                   \
	-readwrite		Ek                    "reversal potential of channel"                            \
	-readwrite		X                     "internal variable for implementing synaptic conductance"  \
	-readwrite		Y                     "internal variable for implementing synaptic conductance"  \
	-hidden		xconst1               "internal variable for synaptic conductances"              \
	-hidden		yconst1               "internal variable for synaptic conductances"              \
	-hidden		xconst2               "internal variable for synaptic conductances"              \
	-hidden		yconst2               "internal variable for synaptic conductances"              \
	-hidden		norm                  "normalization factor for psp amplitude"                   \
	-readwrite	tau1                "first time constant of channel activation"                \
	-readwrite	tau2                 "second time constant of channel activation"               \
	-readwrite	gmax                 "peak conductance"                                         \
	-readwrite	frequency           "random activation frequency" 0.0                          \
	-readonly	nsynapses             "Number of incoming spike messages"                        \
	-hidden		synapse_size          "size of synapse in bytes"                                 \
	-readonly	event_buffer_size     "size of event buffer" 0                                   \
	-readonly       pending_events        "number of pending spike events in buffer" 0               \
	-hidden         FreeSynapticEvents    "free list of synaptic event nodes"                        \
	-hidden		PendingSynapticEvents "list of pending synaptic events"                          \
	-readwrite      nodes_per_synapse     "number of event nodes to allocate per synapse" 1.0        \
	-hidden         list_alloced          "flag for whether the freelist has been allocated" 0       \
        -hidden         allocednodes          "number of nodes that are the head of a malloced block" 0  \
	-readwrite	synapse               "synapse buffer"                                           \   
	-description    "Synaptically activated channel."

// This is a duplicate of synchan, for backwards-compatibility:

object	synchan2        Synchan_type Synchan segment channel synchannel              \
	-author 	"Mike Vanier 11/94"                                          \ 
	-actions 	CREATE INIT PROCESS HPROCESS RESET CHECK RECALC SAVE2 RESTORE2 EVENT  \
			ADDMSGIN DELETEMSGIN MSGINDELETED RESETBUFFER DELETE COPY SET SHOW    \
	-messages 	VOLTAGE 	0 	1 Vm                                 \	
			ACTIVATION 	1 	1 activation                         \
			RAND_ACTIVATION 2 	2 probability amplitude              \
			MOD		3	1 modulation                         \
			SPIKE	       -1	0                                    \
	-readonly       activation            "driving force for channel conductance"                    \
	-readonly       Ik                    "channel current"                                          \
	-readonly       Gk                    "conductance of channel"                                   \
	-readwrite      Ek                    "reversal potential of channel"                            \
	-hidden		X                     "internal variable for implementing synaptic conductance"  \
	-hidden		Y                     "internal variable for implementing synaptic conductance"  \
	-hidden		xconst1               "internal variable for synaptic conductances"              \
	-hidden		yconst1               "internal variable for synaptic conductances"              \
	-hidden		xconst2               "internal variable for synaptic conductances"              \
	-hidden		yconst2               "internal variable for synaptic conductances"              \
	-hidden		norm                  "normalization factor for psp amplitude"                   \
	-readwrite	tau1                  "first time constant of channel activation"                \
	-readwrite	tau2                  "second time constant of channel activation"               \
	-readwrite	gmax                  "peak conductance"                                         \
	-readwrite	frequency             "random activation frequency" 0.0                          \
	-readonly	nsynapses             "Number of incoming spike messages"                        \
	-hidden		synapse_size          "size of synapse in bytes"                                 \
	-readonly	event_buffer_size     "size of event buffer" 0                                   \
	-readonly       pending_events        "number of pending spike events in buffer" 0               \
	-hidden         FreeSynapticEvents    "free list of synaptic event nodes"                        \
	-hidden		PendingSynapticEvents "list of pending synaptic events"                          \
	-readwrite      nodes_per_synapse     "number of event nodes to allocate per synapse" 1.0        \
	-hidden         list_alloced          "flag for whether the freelist has been allocated" 0       \
        -hidden         allocednodes          "number of nodes that are the head of a malloced block" 0  \
	-readwrite	synapse               "synapse buffer"                                           \   
	-description    "Synaptically activated channel."



object  hebbsynchan     HebbSynchan_type HebbSynchan segment channel synchannel                    \
        -author         "Mike Vanier 4/96 Caltech"                                                 \
        -actions        CREATE INIT PROCESS RESET RECALC CHECK SAVE2 RESTORE2 EVENT                \
                        ADDMSGIN DELETEMSGIN MSGINDELETED RESETBUFFER DELETE COPY SET SHOW         \
        -messages       VOLTAGE         0       1 Vm                                               \
                        ACTIVATION      1       1 activation                                       \
                        RAND_ACTIVATION 2       2 probability amplitude                            \
                        MOD             3       1 modulation                                       \
                        WEIGHT_CHANGE_MOD  4    1 modulation                                       \
                        SPIKE          -1       0                                                  \
	-readonly       activation            "driving force for channel conductance"                    \
	-readonly       Ik                    "channel current"                                          \
	-readonly       Gk                    "conductance of channel"                                   \
	-readwrite      Ek                    "reversal potential of channel"                            \
        -hidden         X                     "internal variable for synaptic conductances"              \
        -hidden         Y                     "internal variable for synaptic conductances"              \
        -hidden         xconst1               "internal constant for synaptic conductances"              \
        -hidden         xconst2               "internal constant for synaptic conductances"              \
        -hidden         yconst1               "internal constant for synaptic conductances"              \
        -hidden         yconst2               "internal constant for synaptic conductances"              \
        -hidden         norm                  "normalization factor for psp amplitude"                   \
        -readwrite      tau1                  "first time constant of channel activation"                \
        -readwrite      tau2                  "second time constant of channel activation"               \
        -readwrite      gmax                  "peak conductance"                                         \
        -readwrite      frequency             "random activation frequency" 0.0                          \
        -readonly       nsynapses             "Number of incoming spike messages"                        \
	-hidden		synapse_size          "size of synapse in bytes"                                 \
        -readonly       event_buffer_size     "size of event buffer" 0                                   \
        -readonly       pending_events        "number of pending spike events in event buffer" 0         \
        -hidden         FreeSynapticEvents    "free list of synaptic event nodes"                        \
        -hidden         PendingSynapticEvents "list of pending synaptic events"                          \
        -readwrite      nodes_per_synapse     "number of event nodes to allocate per synapse" 1.0        \
        -hidden         list_alloced          "flag for whether the freelist has been allocated"   0     \
        -hidden         allocednodes          "number of nodes that are the head of a malloced block" 0  \ 
        -readwrite      synapse               "synapse buffer"                                           \
	-readwrite      pre_tau1              "first time constant for presynaptic averaging"   0.010    \
	-readwrite      pre_tau2              "second time constant for presynaptic averaging"  0.100    \
	-readwrite      pre_thresh_lo         "lower presynaptic threshold" 3.0                          \ 
        -readwrite      pre_thresh_hi         "upper presynaptic threshold" 3.0                          \ 
        -hidden         pre_xconst            "internal constant for presynaptic averaging"              \
	-hidden         pre_yconst1           "internal constant for presynaptic averaging"              \
	-hidden         pre_yconst2           "internal constant for presynaptic averaging"              \
	-hidden         pre_norm              "normalization factor for presynaptic averaging"           \
	-readonly       avg_Vm                "averaged membrane potential"                              \
	-readwrite      post_tau              "time constant of postsynaptic averaging"                  \
	-hidden         post_const            "internal constant for postsynaptic averaging"             \
	-readwrite      post_thresh_lo        "lower postsynaptic threshold" -0.065                      \
	-readwrite      post_thresh_hi        "upper postsynaptic threshold" -0.065                      \
	-readwrite      post_scale            "scaling factor for postsynaptic activities" 0.002         \
	-readwrite      weight_change_rate    "rate of weight change" 1.0                                \
	-readwrite      min_weight            "minimum weight" 0.0                                       \
	-readwrite      max_weight            "maximum weight" 100.0                                     \
	-readwrite      change_weights        "flag: nonzero means weights can be changed" 1             \
	-description    "Like synchan, but weights are dynamically modified"  \
                        "according to a function of the pre- and postsynaptic" \
                        "activations."  

/*
 * The facsynchan code is copyright as follows:
 * Copyright (c) 1999 Michael Christopher Vanier
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without license
 * or royalty fees, to use, copy, modify, and distribute this software and its
 * documentation for any purpose, provided that the above copyright notice and
 * the following two paragraphs appear in all copies of this software.
 *
 * In no event shall Michael Christopher Vanier (hereinafter referred to as
 * "the author") or the Genesis Developer's Group be liable to any party for
 * direct, indirect, special, incidental, or consequential damages arising out
 * of the use of this software and its documentation, even if the author
 * and/or the Genesis Developer's Group have been advised of the possibility
 * of such damage.
 *
 * The author and the Genesis Developer's Group specifically disclaim any
 * warranties, including, but not limited to, the implied warranties of
 * merchantability and fitness for a particular purpose.  The software
 * provided hereunder is on an "as is" basis, and the author and the Genesis
 * Developer's Group have no obligation to provide maintenance, support,
 * updates, enhancements, or modifications.
 *  
 */

//
// The `facsynchan' object.
//
//     This object is essentially identical to the standard synchan object
//     except that its maximal conductance can be modulated by a global
//     neuromodulator value and it also contains a model for multi-component
//     facilitation/depression.
//

// Use this version if you do not have Mike Vanier's cellreader library.
// You will not be able to use this object in a .p file without hacking
// the (default genesis) cellreader.

object facsynchan fac_synchan_type FacSynchan segment channel synchannel   \
    -author     "Mike Vanier"                                              \
    -actions    CREATE PROCESS RESET CHECK RECALC SAVE2                    \
                RESTORE2 EVENT ADDMSGIN DELETEMSGIN MSGINDELETED           \
                RESETBUFFER DELETE COPY SET SHOW                           \
    -messages   VOLTAGE     0  1 Vm                                        \
                SPIKE      -1  0                                           \
    -readonly   activation  "Internal synaptic conductance variable."      \
    -readonly   Ik          "Channel current."                             \
    -readonly   Gk          "Conductance of channel."                      \
    -readwrite  Ek          "Reversal potential of channel."               \
    -hidden     X           "Alpha-function activation variable."          \
    -hidden     Y           "Alpha-function activation variable."          \
    -hidden     xconst1     "Internal state variable."                     \
    -hidden     yconst1     "Internal state variable."                     \
    -hidden     xconst2     "Internal state variable."                     \
    -hidden     yconst2     "Internal state variable."                     \
    -hidden     norm        "Internal state variable."                     \
    -readwrite  tau1        "Alpha-function rise time constant."           \
    -readwrite  tau2        "Alpha-function fall time constant."           \
    -readwrite  gmax        "Maximal conductance of the synchan."          \
    -hidden     frequency   "Unused; for synchan compatibility."           \
    -readonly   nsynapses   "Number of synapses."                          \
    -hidden     normalize_weights       "Unused."                          \
    -hidden     time_last_event         "Unused."                          \
    -hidden     hsolve                  "Unused."                          \
    -hidden     solve_index             "Unused."                          \        
    -hidden     synapse_size            "Size of synapse in bytes."        \
    -readonly   event_buffer_size       "Size of event buffer."            \
    -readonly   pending_events          "Number of pending events."        \
    -hidden     FreeSynapticEvents      "Free list of events."             \
    -hidden     PendingSynapticEvents   "List of pending events."          \
    -readwrite  nodes_per_synapse   "# of nodes per synapse."              \
    -hidden     list_alloced    "Flag: was free list allocated?"           \
    -hidden     allocednodes    "Number of allocated event nodes."         \
    -readwrite  synapse         "Synapse buffer."                          \
    -readwrite  fac_depr_on     "Flag: no fac/depr if 0."                  \
    -readwrite  max_fac         "Upper limit on facilitation."             \
    -readwrite  fac_per_spike   "Synaptic facilitation per spike."         \
    -readwrite  fac_tau         "Time constant of synaptic facilitation."  \
    -hidden     fac_const       "= exp(-dt/fac_tau)."                      \
    -readwrite  depr_per_spike  "Synaptic depression per spike."           \
    -readwrite  depr_tau        "Time constant of synaptic depression."    \
    -hidden     depr_const      "= exp(-dt/depr_tau)."                     \
    -description "Synaptically activated channel incorporating "           \
                 "synaptic facilitation and/or synaptic depression."


// Use this version if you are lucky enough to have Mike Vanier's 
// cellreader library.  Unlike your unfortunate counterparts who are not
// using Mike's cellreader, you will be able to use this object in a .p file 
// without hacking anything.

/*
object facsynchan fac_synchan_type FacSynchan segment channel synchannel   \
    -author     "Mike Vanier"                                              \
    -actions    CREATE PROCESS RESET CHECK RECALC SAVE2                    \
                RESTORE2 EVENT ADDMSGIN DELETEMSGIN MSGINDELETED           \
                RESETBUFFER DELETE COPY READCELL SET SHOW                  \
    -messages   VOLTAGE     0  1 Vm                                        \
                SPIKE      -1  0                                           \
    -readonly   activation  "Internal synaptic conductance variable."      \
    -readonly   Ik          "Channel current."                             \
    -readonly   Gk          "Conductance of channel."                      \
    -readwrite  Ek          "Reversal potential of channel."               \
    -hidden     X           "Alpha-function activation variable."          \
    -hidden     Y           "Alpha-function activation variable."          \
    -hidden     xconst1     "Internal state variable."                     \
    -hidden     yconst1     "Internal state variable."                     \
    -hidden     xconst2     "Internal state variable."                     \
    -hidden     yconst2     "Internal state variable."                     \
    -hidden     norm        "Internal state variable."                     \
    -readwrite  tau1        "Alpha-function rise time constant."           \
    -readwrite  tau2        "Alpha-function fall time constant."           \
    -readwrite  gmax        "Maximal conductance of the synchan."          \
    -hidden     frequency   "unused; for synchan compatibility."           \
    -readonly   nsynapses   "Number of synapses."                          \
    -hidden     synapse_size            "Size of synapse in bytes."        \
    -readonly   event_buffer_size       "Size of event buffer."            \
    -readonly   pending_events          "Number of pending events."        \
    -hidden     FreeSynapticEvents      "Free list of events."             \
    -hidden     PendingSynapticEvents   "List of pending events."          \
    -readwrite  nodes_per_synapse   "# of nodes per synapse."              \
    -hidden     list_alloced    "Flag: was free list allocated?"           \
    -hidden     allocednodes    "Number of allocated event nodes."         \
    -readwrite  synapse         "Synapse buffer."                          \
    -readwrite  fac_depr_on     "Flag: no fac/depr if 0."                  \
    -readwrite  max_fac         "Upper limit on facilitation."             \
    -readwrite  fac_per_spike   "Synaptic facilitation per spike."         \
    -readwrite  fac_tau         "Time constant of synaptic facilitation."  \
    -hidden     fac_const       "= exp(-dt/fac_tau)."                      \
    -readwrite  depr_per_spike  "Synaptic depression per spike."           \
    -readwrite  depr_tau        "Time constant of synaptic depression."    \
    -hidden     depr_const      "= exp(-dt/depr_tau)."                     \
    -description "Synaptically activated channel incorporating "           \
                 "synaptic facilitation and/or synaptic depression."
*/

//
// End of objects.
//

object	spikegen	Spikegen_type Spikegen spiking               \
	-author 	"M.Wilson Caltech 6/88, Dave Bilitch 1/94"   \
	-actions 	PROCESS RESET CHECK SAVE2 RESTORE2           \
	-readonly	state       "current state of object"        \
	-readwrite	thresh      "threshold for spike generation" \
	-readwrite	output_amp  "amplitude of spike event" 1.0   \
	-readwrite	abs_refract "absolute refractory period"     \
	-readwrite	lastevent   "time of last spike"             \
	-messages	INPUT 0		1 input                      \
			THRESH 1	1 threshold                  \
	-description	"Performs threshold spike discrimination."   \
			"Generates an impulse each time an input crosses the"   \
			"spike threshold at a maximal rate set by abs_refract."

object	randomspike	Randomspike_type Randomspike spiking                       \
	-author 	"M.Wilson Caltech 6/88, Dave Bilitch 1/94"                 \
	-actions 	INIT PROCESS RESET SET                                     \
	-readonly	state       "current state of object"                      \
	-readwrite	rate        "rate of generation of events"                 \
	-readwrite      reset       "flag for whether to reset after each event" 1 \
	-readwrite	reset_value "what to reset state to" 0.0                   \
	-readwrite      min_amp     "minimum amplitude of event" 1.0               \
	-readwrite      max_amp     "maximum amplitude of event" 1.0               \
	-readwrite	abs_refract "absolute refractory period" 0.0               \
	-hidden		lastevent   "time of last spike"                           \
	-hidden		realrate    "rate corrected for refractory period"         \
	-messages	RATE 0		1 rate                                     \
			MINMAX 1	2 min max                                  \
	-description	"Generates a time series of events at a rate"              \
			"given by the rate parameter. The probability"             \
			"of an event for a single time step is adjusted"           \
			"based on the setting of abs_refract such that"            \
			"the requested rate is maintained.  No event will"         \
			"be generated during the refractory period defined"        \
			"by abs_refract.  When an event has been generated"        \
			"the amplitude of the event is a random variable"          \
			"uniformly distributed between min_amp and max_amp."       \
			"The state field has the value of the event amplitude"     \
			"if an event has been generated. If an event is not"       \
			"generated then the value of the state field"              \
			"depends on the reset field. If reset is non-zero"         \
			"(the default) then the state takes on the value given in" \
			"reset_value. Otherwise the state will behave like a"      \
			"latch containing the amplitude of the previous event."    \
			"A reset time, randomspike is initialized to a random"     \
			"phase of the refractory period to avoid artificial"       \
			"synchronization at high rate settings."



object  SynS_object     SynS_type       SynS_f  synchannel  device \
        -author         "ron  lab, 3/97 " \
        -actions        INIT PROCESS RESET CHECK SAVE2 RESTORE2  \
        -messages       VOLTAGE 0       1  Vm \
        -readwrite      m_SynS     "modulation factor" \
	-readwrite      A          "A"           \
        -readwrite      B          "B"           \
        -readwrite      C          "C"           \
        -readwrite      D          "D"           \
        -readwrite      E          "E"      \
	-hidden		Ek         "Ek not used " \
	-hidden 	Ik         "Ik not used " \
	-hidden         Gk         "Gk not used " \
	-hidden    	activation "activation not used" \
        -description    "a new object for voltage-dependent modulation " \
                        "of spike-mediated synaptic transmission." \
                        "Modulation is a sigmoidal function of membrane" \
                        "potential given by " \
                        "ModInf=A+B/(1+exp(C*(Vm+D)))" \
                        "E is the time constant"


object	SynG_object	SynG_type	SynG	synchannel  device \
	-author 	"ron  lab,3/97 " \
	-actions	INIT PROCESS RESET CHECK SAVE2 RESTORE2 \
	-messages	VOLTAGE 0       1  Vm  \
			CAF     1       1  FastCa  \
                        CAS     2       1  SlowCa  \
                        POSTVOLTAGE 3   1  postvoltage \
        -readwrite      A          "shunts effective Ca current"  \
        -readwrite      B          "buffering parameter "  \
        -readwrite      P          "Ca factor governing transmitter release"  \
        -readwrite      C          "limit parameter(limits release to Gbar)"  \
        -readwrite      R          "release = p^3/(p^3+C)"    \
        -readwrite      Gbar       "maximal conductance" \
        -readwrite      A1         "A1 in Ainf=A1+A2/(1+exp(A3+(Vm+A4)))" \
        -readwrite      A2         "A2" \
        -readwrite      A3         "A3" \
        -readwrite      A4         "A4" \
        -readwrite      A5         "A5 is the time constant" \
        -readwrite      POWER         "POWER" \
	-readonly       Ik         "Ik" \
	-hidden 	activation "activation not used" \
	-description	"A new object specifically for" \
			"graded synaptic inhibition in the leech"

object	SynE_object	SynE_type	SynE	synchannel  device \
	-author 	"ron  lab,5/99 " \
	-actions	INIT PROCESS RESET CHECK SAVE2 RESTORE2 \
	-messages	VOLTAGE 0       1  voltage \
                        POSTVOLTAGE 1   1  postvoltage \
        -readwrite      TauPre     "Tau for RC filter of presynaptic Vm" \
        -readwrite      TauPost    "Tau for RC filter of presynaptic Vm"   \
        -readwrite      Vpre       "RC filtered presynaptic Vm" \
        -readwrite      Vpost      "RC filtered postsynaptic Vm" \
        -readwrite      rectify    "0 no rectify, 1 pass pos., 1 pass neg." \
        -readwrite      Gbar       "maximal conductance" \
        -readonly       Ik         "Ik" \
	-hidden		Ek         "Ek not used " \
	-hidden         Gk         "Gk not used " \
	-hidden 	activation "activation not used" \
	-description	"A new object specifically for" \
			"electrical synaptic transmission in the leech"

object	stdp_rules  stdp_rules_type StdpRules       output \
	-author	     "Dave Beeman, June 2014" \
	-actions     CREATE INIT PROCESS RESET CHECK \
	-readwrite   cellpath	  "full wildcard path to the postsynaptic cells" \
	-readwrite   synpath	  "relative path to synchan to be modified" \
	-readwrite   spikepath	 "relative path to the cell spikegen" \
	-readwrite   tau_pre	  "decay constant for pre before post spike" \
	-readwrite   tau_post	  "decay constant for post before pre spike" \
	-readwrite   min_dt	  "minimum time diff to account for latency" \
	-readwrite   min_weight	  "minimum synaptic weight allowed" \
	-readwrite   max_weight	  "maximum synaptic weight allowed" \
	-readwrite   dAplus	  "increment for postive weight changes" \
	-readwrite   dAminus	  "increment for negative weight changes" \
	-readwrite   change_weights "flag: 0 = plasticity off, otherwise on" \
	-readwrite   debug_level "flag: 0 = no messages, 1 = some messages, 2 = more" \
  -readonly    celllist     "GENESIS ElementList of cells to modify" \
	-description "A clocked object to modify synaptic weights for all " \
		     "synapses of a specified (wildcarded) cell and synchan, " \
         "e.g. '/layer4/pyr[]' and 'apical3/AMPA'. " \
		     "The spike timing dependent plasticity (STDP) rules " \
		     "algorithm is from Song, Miller, and Abbott (2000)."

addfunc planardelay         PlanarDelay
addfunc volumedelay         VolumeDelay
addfunc syndelay            SynDelay 
addfunc planarweight        PlanarWeight
addfunc volumeweight        VolumeWeight
addfunc planarconnect       do_planarconnect
addfunc volumeconnect       do_volumeconnect
addfunc volumeconnect3      do_volumeconnect3
addfunc normalizeweights    NormalizeWeights
addfunc getsynindex         do_getsynindex int
addfunc getsyncount         do_getsyncount int 
addfunc getsynsrc           do_getsynsrc   char*
addfunc getsyndest          do_getsyndest  char*
addfunc resetsynchanbuffers ResetSynchanBuffers
addfunc planarweight2       PlanarWeight2
addfunc planardelay2        PlanarDelay2
addfunc volumedelay2        VolumeDelay2
addfunc volumedelay3        VolumeDelay3
addfunc volumeweight2       VolumeWeight2
addfunc volumeweight3       VolumeWeight3
addfunc fileconnect       	do_fileconnect

/*
** $Log: newconnlib.g,v $
** Revision 1.3  2006/01/09 16:29:34  svitak
** Added leech channels.
**
** Revision 1.2  2005/12/16 07:01:09  svitak
** Changes for Dieter's lab. Made Ik, Gk, X, Y, and lastevent readwrite.
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.20  2000/10/12 22:26:35  mhucka
** New spikegen support from E. De Schutter and H. Cornelis.
**
** Revision 1.19  2000/07/09 23:57:29  mhucka
** New function "fileconnect" from Upi Bhalla.
**
** Revision 1.18  2000/04/27 07:29:53  mhucka
** Added fix from Mike Vanier for synchan objects involving SET and SHOW
** operations.  The problem was that they failed to trap attempts to set
** fields in nonexistent synapses.
**
** Revision 1.17  1999/12/01 22:54:56  mhucka
** Added function volumeweight2.
**
** Revision 1.16  1999/11/29 07:38:25  mhucka
** Renamed newconn.g to newconnlib.g
**
** Revision 1.15  1999/11/27 07:49:16  mhucka
** Added M. Vanier's facsynchan object.
**
** Revision 1.14  1997/08/08 23:42:23  dhb
** Added SET action for randomspike and updated randomspike
** description to reflect change in handling of rate and abs_refract
** values and resulting spike generation
**
** Revision 1.13  1997/08/04 17:38:27  dhb
** Added HPROCESS to synchan2 actions
**
** Revision 1.12  1997/08/01 18:57:44  dhb
** Fix to randomspike to adjust for the rate damping effect of
** the refractory period.  (From Erik De Schutter)
**
** Revision 1.11  1997/07/29 00:58:02  dhb
** Added planardelay2 and volumedelay2 commands
**
** Revision 1.10  1997/07/23 22:01:11  dhb
** Added HPROCESS to synchan actions.
**
** Revision 1.9  1997/05/29 06:52:39  dhb
** Update from Antwerp GENESIS version 21e
**
** Version EDS21b 96/08/02
** MAEX added field normalize_weights to synchan2 object
**      added function planarweight2
**
# Revision 1.8  1996/07/23  00:03:40  dhb
# Merged in 1.6.1.1 changes
#
# Revision 1.7  1995/09/28  00:01:48  venkat
# Changes from Mike Vanier:
# Removed references to hebbsynchan2.
# Reorganized field positions to conform with newconn_struct.h
#
# Revision 1.6.1.1  1996/07/23  00:00:14  dhb
# Changes from Mike Vanier:
#   synchan2 --> synchan
#
# Revision 1.6  1995/08/03  20:02:17  dhb
# Changed hebbsynchan back to HebbSynchan implementation and commented
# hebbsynchan2 object.
#
# Revision 1.5  1995/08/03  00:53:44  dhb
# hebbsynchan is now the HebbSynChan2 version and there is no
# hebbsynchan2 object defined.
#
# Revision 1.4  1995/03/28  02:13:49  mvanier
# Cosmetic changes.
#
# Revision 1.3  1995/03/27  20:36:21  mvanier
# Changed order of fields in synchan object to conform
# to newconn_struct.h.
#
# Revision 1.2  1995/02/27  23:04:48  mvanier
# Spiking objects are no longer of class membrane.
#
# Revision 1.1  1995/01/11  23:09:02  dhb
# Initial revision
#
*/
