//genesis
/*   inputs.g for cable tutorial
---------------------------------------------------------------------------
makeinputs
     used to provide pulsed current injection to a compartment and
     pulsed spike trains to the synapse

set_inj_timing(delay,width)
     used to set the injection timing parameters

add_injection
     checks for valid compartment no., deletes message to previous
     compartment, and sends message to inject {comp_no}

set_injection
     gets info from form defined in forms.g and uses above functions to
     set injection parameters
     
setspikes(delay,width,interval)
     used in makeinputs and forms.g to set the spike train parameters
     for a pulsegen object

set_chan_params(gysn,tsyn,Vsyn)
     
add_syn_input

set_inputs
     used for setting synaptically activated channel input parameters

---------------------------------------------------------------------------
*/

function set_inj_timing(delay, width)
    float delay, width
    setfield /input/injectpulse width1 {width} delay1 {delay}  \
        baselevel 0.0 trig_mode 0 delay2 100000
    // free run mode with very long delay for 2nd pulse (non-repetitive)
    // level1 is set by the inj_toggle function
end

function inj_toggle// toggles current injection ON/OFF

    if ({getfield /output/inject_menu/injtoggle state} == 1)
        // ON
        setfield /input/injectpulse level1 1.0
    else
        // OFF
        setfield /input/injectpulse level1 0.0
    end
end

function add_injection
   // compartment # for injection - #0 = Soma
   int comp_no
   comp_no = ({getfield /output/inject_menu/comp_no value})
   if ((comp_no) > ({NCableSects}))
      echo "There are only "{NCableSects}" cable compartments!"
      return
   end
   if ((comp_no) == ({InjComp}))
      return
   end
   if ({exists /cell/solve})
       delete /cell/solve
   end
   InjComp = comp_no
   if (({getmsg /input/injectpulse/injcurr -outgoing -count}) > 0)
      deletemsg /input/injectpulse/injcurr  0 -outgoing      // only outgoing message

   end
   if ((comp_no) <= 0)   // then inject soma

      addmsg /input/injectpulse/injcurr /cell/soma INJECT output
      echo "Current injection is to the soma"
   else
      addmsg /input/injectpulse/injcurr /cell/cable[{comp_no - 1}]  \
          INJECT output
      echo "Current injection is to cable compartment number "{comp_no}
   end
   // re-makes solver, if needed
   make_hsolve
end

function set_injection// gets info from form defined in forms.g

   str dialog = "/output/inject_menu"
   set_inj_timing {getfield {dialog}/delay value}  \
       {getfield {dialog}/width value}
   setfield /input/injectpulse/injcurr  \
       gain {getfield {dialog}/inject value}
   echo  \
       "Injection current = "{getfield {dialog}/inject value}" microamperes"
   echo  \
       "Injection pulse delay = "{getfield {dialog}/delay value}" millisec"
   echo  \
       "Injection pulse width = "{getfield {dialog}/width value}" millisec"
end


function setspikes(delay, width, interval)//this is more general than necessary

    str pulser = "/input/synpulse"
    str spiker = "/input/synpulse/spiketrain"
    float delay, width, interval, width2
    setfield {pulser} width1 {width} delay1 {delay} baselevel 0.0  \
        trig_mode 0 delay2 100000
    // free run mode with very long delay for 2nd pulse (non-repetitive)
    // level1 is set by the syn_toggle function 
    setfield {spiker} abs_refract {interval} output_amp 1 thresh 0.5
end

function syn_toggle// toggles synaptic input ON/OFF

    if ({getfield /output/synapse_menu/syntoggle state} == 1)
        // ON
        setfield /input/synpulse level1 1.0
    else
        // OFF
        setfield /input/synpulse level1 0.0
    end
end

function set_chan_params(gsyn, tsyn, Vsyn)
   str gsyn, tsyn, Vsyn
   setfield /cell/syn_channel Ek {Vsyn}
   setfield /cell/syn_channel tau1 {tsyn} tau2 {tsyn}
   setfield /cell/syn_channel gmax {gsyn}
end

// links channel which computes channel current (channelC2) with compartment
function link_channel2(channel, compartment)
    addmsg {channel} {compartment} CHANNEL Gk Ek
    addmsg {compartment} {channel} VOLTAGE Vm
end

function add_syn_input
   // compartment # for input - #0 = Soma
   int comp_no
   comp_no = ({getfield /output/synapse_menu/comp_no value})
   if ((comp_no) > ({NCableSects}))
      echo "There are only "{NCableSects}" cable compartments!"
      return
   end
   if ((comp_no) == ({SynComp}))
      return
   end
   if ({exists /cell/solve})
       delete /cell/solve
   end
   SynComp = comp_no
   // NOTE: We could disconnect the channel from the old compartment by
   // deleting all its messages.  If the conductance (Gk) is plotted this
   // won't work, unless we are careful to keep the message numbering staight.
   // To be safe, we locate the right "OUT" message.
    int i, n, nmsg
    nmsg = {getmsg /cell/syn_channel -outgoing -count}
    for (i = 0; i < nmsg; i = i + 1)
        if ( \
            ({getmsg /cell/syn_channel -outgoing -type {i}}) == "CHANNEL" \
            )
            n = i
            break
        else
            n = -1
        end
    end
    if (n >= 0)
        deletemsg /cell/syn_channel  {n} -outgoing
    end
    //  To avoid deleting ACTIVATION message, look for the VOLTAGE message
    nmsg = {getmsg /cell/syn_channel -incoming -count}
    for (i = 0; i < nmsg; i = i + 1)
        if ( \
            ({getmsg /cell/syn_channel -incoming -type {i}}) == "VOLTAGE" \
            )
            n = i
            break
        else
            n = -1
        end
    end
    if (n >= 0)
        deletemsg /cell/syn_channel  {n} -incoming
    end

   if ((comp_no) <= 0)   // then input to soma

      link_channel2 /cell/syn_channel /cell/soma
      echo "Synaptic input is to the soma"
   else
      link_channel2 /cell/syn_channel /cell/cable[{comp_no - 1}]
      echo "Synaptic input is to cable compartment number "{comp_no}
   end
   // re-makes solver, if needed
   make_hsolve
end

function set_inputs// set parameters for synaptic input from dialog boxes

   str dialog = "/output/synapse_menu"
   setspikes {getfield {dialog}/delay value} {synwidth} {syninterval}
   set_chan_params {getfield {dialog}/gsyn value}  \
       {getfield {dialog}/tsyn value} {getfield {dialog}/vsyn value}
   // after so many changes, we need to reset everything
   reset
   echo "Synaptic input delay = "{getfield {dialog}/delay value}
   echo "Maximum conductance = "{getfield {dialog}/gsyn value}
   echo "Time constant for conductance = "{getfield {dialog}/tsyn value}
   echo "Channel equilibrium potential = "{getfield {dialog}/vsyn value}
end

//=======================================================================
// makeinputs - the main function to set up injection and synaptic input
//=======================================================================

function makeinputs


/* Default values of parameters - (global variables defined in Cable.g)
float injwidth  =   10  // msec
float injdelay  =    0
float injcurrent       = 2e-4  // uA (200 nA)

float syndelay	=  0	// msec
float synwidth 	= 1000	// choose width and interval to give a single spike
float syninterval	= 100000

float gsyn	= 1e-6	// milliSiemen
float tsyn	= 1		// milliseconds
float Vsyn	= 50		// mV above resting potential (defined as zero)
	 Vsyn = 125 ==> Na channel (+55 mV with Erest = -70)
	 Vsyn = -12 ==>  K channel (-82 mV with Erest = -70)
*/
create neutral /input
//==================================================
// Make a periodic injection current step
//==================================================
create pulsegen /input/injectpulse
set_inj_timing {injdelay} {injwidth}
// set the amplitude (current) with the gain of a differential amplifier
create diffamp /input/injectpulse/injcurr
setfield /input/injectpulse/injcurr saturation 1 gain {injcurrent}
addmsg /input/injectpulse /input/injectpulse/injcurr PLUS output

//==================================================
// spike trains for synaptic input - could be simpler if only a single spike
//==================================================
// pulse to gate spike train
create pulsegen /input/synpulse
// Use pulse as input to a spike generator object
create spikegen /input/synpulse/spiketrain
setspikes {syndelay} {synwidth} {syninterval}
addmsg /input/synpulse /input/synpulse/spiketrain INPUT output

// Lastly, make connection to chemically activated dendrite channel
// using a SPIKE message
create synchan /cell/syn_channel
set_chan_params {gsyn} {tsyn} {Vsyn}
addmsg /input/synpulse/spiketrain /cell/syn_channel SPIKE
setfield /cell/syn_channel synapse[0].delay 0 synapse[0].weight 1

// initially, make both injection and synaptic input to the soma
addmsg /input/injectpulse/injcurr /cell/soma INJECT output
link_channel2 /cell/syn_channel /cell/soma
// makes solver, if needed
make_hsolve
end// makeinputs
