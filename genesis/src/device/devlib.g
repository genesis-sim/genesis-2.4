//genesis
//
// $Id: devlib.g,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $
// $Log: devlib.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:28  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.4  1998/06/30 22:17:22  dhb
// EDS22d merge
//
// Revision 1.3  1998/04/01 00:05:37  dhb
// Incorporated spike analysis library by Dieter Jaeger.  Added
// updated freq_monitor and peristim objects which fix known
// problems with these objects.  Can revert to old objects by
// setting UseOldFrequencyMonitor and UseOldPeristim non-zero.
// Also added include of spikelib.g to define the spike anal
// library objects proper.
//
// Revision 1.2.1.1  1998/06/30 22:15:29  dhb
// EDS 22d
//
// EDS22d revison: EDS BBF-UIA 98/06/16-98/06/16
//  Added calculator
//
// Revision 1.2  1994/03/20 07:11:09  dhb
// The diffamp object used a message slot name which started with a -
// which is not allowed.
//
// Revision 1.1  1992/12/11  19:02:56  dhb
// Initial revision
//

//
// The new spike analysis library now incorporated in the device
// library includes updated versions of freq_monitor and peristim.
// Setting UseOldFrequencyMonitor or UseOldPeristim will define the
// old object rather than the new ones.
//

int UseOldFrequencyMonitor = 0;
int UseOldPeristim = 0;

//genesis
newclass	device
newclass	channel
/*
******************************************************************************
**                           Device OBJECT TABLE                            **
******************************************************************************
*/
//============================================================================
//	name		data		function	class		
//============================================================================

object	diffamp		diffamp_type	DifferenceAmp	device	\
	-author 	"M.Wilson Caltech 2/89" \
	-actions	RESET PROCESS \
	-messages	PLUS 0		1 plus_input \
			MINUS 1		1 minus_input \
			GAIN 2		1 gain \
	-fields		gain saturation plus minus output \
	-description	"difference amplifier takes two inputs and" \
			"produces an output proportional to their difference"

object	efield		efield_type ExtracellularRecordingElectrode device \
	-author 	"M.Wilson Caltech 2/89" \
	-actions	RESET PROCESS RECALC \
	-messages	CURRENT 0	2 current 0.0 \
	-fields		scale field \
	-description	"sums current sources according to their" \
			"inverse distance from the recording electrode" \
			"to produce an estimate of the field potential" \
			"at that point."

object	expthresh	expthresh_type ExpThresh device \
	-author 	"M.Wilson Caltech 8/89" \
	-actions	RESET PROCESS CHECK \
	-messages	BUFFER 0	1 name \
	-fields		theta_s theta_0 tau_theta 

if (UseOldFrequencyMonitor)
object	freq_monitor	freq_mon_type	FrequencyMonitor	device	\
	-author 	"M.Wilson Caltech 2/89" \
	-actions	RESET PROCESS *CREATE BufferCreate \
	-messages	NONZERO_TRIG 	0	1 input \
			ZERO_TRIG 	1	1 input \
			POSITIVE_TRIG 	2	1 input \
			NEGATIVE_TRIG 	3	1 input \
			WINDOW 		4	1 time \
			CONTINUOUS 	5	1 input \
	-fields		exponent tmin \
	-description	"Calculates running event (spike) frequency" \
			"over a given time window using exponential weighting."
else // if (UseOldFrequencyMonitor)
object  freq_monitor    new_freq_mon_type   NewFrequencyMonitor        device  \
        -author         "M.Wilson Caltech 2/89; mods by D.Jaeger" \
        -actions        RESET PROCESS *CREATE BufferCreate \
        -messages       NONZERO_TRIG    0       1 input \
                        ZERO_TRIG       1       1 input \
                        POSITIVE_TRIG   2       1 input \
                        NEGATIVE_TRIG   3       1 input \
                        WINDOW          4       1 time \
                        CONTINUOUS      5       1 input \
        -fields         exponent tmin \
        -description    "Calculates running event (spike) frequency" \
                        "over a given time window using exponential weighting."
end // if (UseOldFrequencyMonitor)

object	nernst		nernst_type Nernst  device channel \
	-author 	"M.Wilson Caltech 3/89" \
	-actions 	PROCESS RESET CHECK \
	-messages	CIN 0		1 Cin \
			COUT 1		1 Cout \
			TEMP 2		1 T \
	-fields		"E = equilibrium potential" \
			"T = temperature in degrees celsius"  \
			"valency = ionic valency z"  \
			"scale = voltage scale factor"  \
	-description	"Calculates the Nernst potential for the given" \
			"ionic concentrations and temperature." \
			"E = scale*(RT/zF)*ln(Cout/Cin)" \
			"A scale factor of 1 gives E in volts." \
			"A scale factor of 1e3 gives E in millivolts."

object	funcgen		funcgen_type	FunctionGenerator	device	\
	-author 	"M.Wilson Caltech 4/89" \
	-actions	RESET PROCESS CHECK \
	-messages	AMPLITUDE 	0		1 amplitude \
			DC_OFFSET 	1		1 dc_offset \
			PHASE 		2		1 phase \
			FREQUENCY 	3		1 frequency \
	-fields		"mode  0=sine; 1=square; 2=triangle; 3=constant" \
			"amplitude dc_offset phase frequency" \
	-description	"function generator produces periodic output" \
			"with a waveform set by the mode field"

if (UseOldPeristim)
object  peristim  peristim_type  PeriStimulusDistribution  device  \
	-author     "E. De Schutter Caltech 8/91" \
	-actions    RESET PROCESS CHECK \
	-messages   INPUT   0       1 voltage \ // trigger_mode = FREE_RUN
				TRIGGER 1       1 trigger \ // trigger_mode = TRIGGERED
				SPIKE   2       1 time \ // trigger_mode = SPIKES
	-fields     "output: is the current bin" \
				"threshold  binwidth  num_bins: determine ps distribution" \
				"trigger_mode: free running (0) or triggered (1)" \
				"reset_mode: if 1 then bins will be emptied upon RESET," \
				"            if 2 then new table will be made upon RESET" \
	-description "generates on line a peristimulus spike histogram, will loop" \
				"through all the bins and then start again in the first" \
				"bin if the simulation continues (FREE_RUN) or unless " \
				"triggered. SPIKES-mode is used to generate under script" \
				"control (clock is NOT used)."
else // if (UseOldPeristim)
object  peristim  new_peristim_type  NewPeriStimulusDistribution  device  \
	-author     "E. De Schutter Caltech 8/91; embellished, D. Jaeger 8/92" \
	-actions    RESET PROCESS CHECK \
	-messages   INVM   0       1 voltage \ // trigger_mode = FREE_RUN
				TRIGGER 1       1 trigger \ // trigger_mode = TRIGGERED
				READTIME   2       1 time \ // trigger_mode = SPIKES
				INSPIKE		3		1 inspike \ // get "digital" input
	-fields     "output: is the current bin" \
				"threshold  binwidth  num_bins: determine ps distribution" \
				"trigger_mode: free running (0) or triggered (1)" \
				"reset_mode: if 1 then bins will be emptied upon RESET," \
				"            if 2 then new table will be made upon RESET" \
	-description "generates on line a peristimulus spike histogram, will loop" \
				"through all the bins and then start again in the first" \
				"bin if the simulation continues (FREE_RUN) or unless " \
				"triggered. INVM is used to pass a membrane potential " \
				" which gets thresholded to detect spikes. INSPIKE is " \
				" used to pass one-timestep spikes (amp > threshold), " \
				"INSPIKE-mode is used to generate under script" \
				"control (clock is NOT used)."
end // if (UseOldPeristim)

object  interspike  interspike_type  InterSpikeInterval  device  \
	-author     "E. De Schutter Caltech 8/91" \
	-actions    RESET PROCESS CHECK \
	-messages   INPUT   0       1 voltage \ // trigger_mode = FREE_RUN
				TRIGGER 1       1 trigger \ // trigger_mode = TRIGGERED
				SPIKE   2       1 time \ // trigger_mode = SPIKES
	-fields     "output: is the last isi" \
				"threshold  binwidth  num_bins: determine distribution" \
				"reset_mode: if 1 then bins will be emptied upon RESET" \
	-description "generates on line a interspike interval histogram. The" \
				"last bin contains also all values larger than the bin-range."

//
// Spike analysis library by Dieter Jaeger
//

include spikelib.g


object  calculator   calc_type   CalculatorFunc     device  \
    -author     "E. De Schutter BBF-UIA 6/98" \
    -actions    CREATE DELETE PROCESS RESET \
	-messages   SUM       0    1 number \ // sum number to output
				SUBTRACT  1    1 number \ // subtract number from output
				MULTIPLY  2    1 number \ // multiply number with output
				DIVIDE    3    1 number \ // divide number with output
    -readonly	output		"computed value" \
	-readwrite  resetclock	"clock # used to reset output value" \
	-readwrite  output_init "value used to initalize output at reset" \
	-hidden		next_init "" \
    -description    "calculates using messages." 
