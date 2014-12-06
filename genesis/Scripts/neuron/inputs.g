//genesis
/*   inputs.g
---------------------------------------------------------------------------
makeinputs
     used in Neuron.g to provide pulsed current injection to soma and
     pulsed spike trains to the dendrite excitatory and inhibitory synapses

setspikes(source,delay,width,interval)
     used in makeinputs and forms.g to set the spike train parameters
     for a pulsegen object

setinjection(delay,width)
     used in makeinputs and forms.g to set the injection parameters
---------------------------------------------------------------------------
*/

function setspikes(source, delay, width, interval)
    str source
    str pulser = "/input/" @ (source)
    str spiker = (pulser) @ "/spiketrain"
    float delay, width, interval
    setfield {pulser} level1 1.0 width1 {width} delay1 {delay}  \
        baselevel 0.0 trig_mode 0 delay2 100000
    // free run mode with very long delay for 2nd pulse (non-repetitive)
    setfield {spiker} abs_refract {interval} output_amp 1 thresh 0.5
end

function setinjection(delay, width)
    float delay, width
    setfield /input/injectpulse level1 1.0 width1 {width} delay1 {delay} \
         baselevel 0.0 trig_mode 0 delay2 100000
    // free run mode with very long delay for 2nd pulse (non-repetitive)
end

function makeinputs

// Default values of parameters - dialog boxes in forms.g should be consistent
// msec
float injwidth = 40
float injdelay = 20
// uA (200 nA)
float somacurrent = 2e-4
float dend1current = 0
float dend2current = 0

float Adelay = 10
float Awidth = 50
// msec
float Ainterval = 10

float Bdelay = 20
float Bwidth = 50
// msec
float Binterval = 10

create neutral /input
//==================================================
// Make a periodic injection current step
//==================================================
create pulsegen /input/injectpulse
setinjection {injdelay} {injwidth}
// set the amplitude (current) with the gain of a differential amplifier
create diffamp /input/injectpulse/somacurr
create diffamp /input/injectpulse/dend1curr
create diffamp /input/injectpulse/dend2curr
setfield /input/injectpulse/somacurr saturation 1 gain {somacurrent}
setfield /input/injectpulse/dend1curr saturation 1 gain {dend1current}
setfield /input/injectpulse/dend2curr saturation 1 gain {dend2current}
addmsg /input/injectpulse /input/injectpulse/somacurr PLUS output
addmsg /input/injectpulse /input/injectpulse/dend1curr PLUS output
addmsg /input/injectpulse /input/injectpulse/dend2curr PLUS output
addmsg /input/injectpulse/somacurr /cell/soma INJECT output
addmsg /input/injectpulse/dend1curr /cell/dend1 INJECT output
addmsg /input/injectpulse/dend2curr /cell/dend2 INJECT output

//==================================================
// spike trains for synaptic input
//==================================================
// pulse to gate spike train A
create pulsegen /input/Apulse
// Use pulse as input to a spikegen object
create spikegen /input/Apulse/spiketrain
setspikes Apulse {Adelay} {Awidth} {Ainterval}
addmsg /input/Apulse /input/Apulse/spiketrain INPUT output

// pulse to gate spike train B
create pulsegen /input/Bpulse
// Use pulse as input to a spikegen object
create spikegen /input/Bpulse/spiketrain
setspikes Bpulse {Bdelay} {Bwidth} {Binterval}
addmsg /input/Bpulse /input/Bpulse/spiketrain INPUT output

// Lastly, makeconnection to chemically activated dendrite channels
addmsg /input/Apulse/spiketrain /cell/dend2/Inh_channel SPIKE
addmsg /input/Apulse/spiketrain /cell/dend2/Ex_channel SPIKE
addmsg /input/Apulse/spiketrain /cell/dend1/Inh_channel SPIKE
addmsg /input/Apulse/spiketrain /cell/dend1/Ex_channel SPIKE
addmsg /input/Bpulse/spiketrain /cell/dend2/Inh_channel SPIKE
addmsg /input/Bpulse/spiketrain /cell/dend2/Ex_channel SPIKE
addmsg /input/Bpulse/spiketrain /cell/dend1/Inh_channel SPIKE
addmsg /input/Bpulse/spiketrain /cell/dend1/Ex_channel SPIKE

// no delay, initial weights 0
// I could probably do this with wildcards
setfield /cell/dend1/Ex_channel synapse[0].delay 0 synapse[0].weight 0 \
	synapse[1].delay 0 synapse[1].weight 0
setfield /cell/dend1/Inh_channel synapse[0].delay 0 synapse[0].weight 0 \
	synapse[1].delay 0 synapse[1].weight 0
setfield /cell/dend2/Ex_channel synapse[0].delay 0 synapse[0].weight 0 \
	synapse[1].delay 0 synapse[1].weight 0
setfield /cell/dend2/Inh_channel synapse[0].delay 0 synapse[0].weight 0 \
	synapse[1].delay 0 synapse[1].weight 0

end// makeinputs
