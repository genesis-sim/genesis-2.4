//genesis

/* FILE INFORMATION
** "Generic" excitatory (glutamate) and inhibitory (GABA) synaptically
**  activated channels  (based on mitsyn.g by Upi Bhalla)
**
** Modified by D. Beeman for conductances similar to those used by
** T. Vogels and L. Abbott, J. Neurosci. 25: 10786-10795

** The channels have a dual exponential type conductance change, with a
** rapid rise to gmax for each synaptic event.
**
*/

// CONSTANTS
float EGlu = 0.0
float EGABA = -0.08
float SOMA_A = 20e-9
float GGlu = SOMA_A * 50 // this will get changed
float GGABA = SOMA_A * 50

//===================================================================
//                     SYNAPTIC CHANNELS
//===================================================================


function make_Ex_channel
	if ({exists Ex_channel})
		return
	end
        float tau1 = 0.005
        float tau2 = 0.001
	create		synchan	Ex_channel
    	setfield	        Ex_channel \
		Ek			{EGlu} \
		tau1		{tau1} \	// sec
		tau2		{tau2} \ 	// sec
		gmax		{GGlu} // Siemens
end

function make_Inh_channel
	if ({exists Inh_channel})
		return
	end
        float tau1 = 0.010
        float tau2 = 0.001
	create		synchan	Inh_channel
    	setfield	        Inh_channel \
	Ek			{ EGABA } \
        tau1            {tau1} \        // sec
        tau2            {tau2} \        // sec
	gmax		{GGABA}		// Siemens
end


function make_iEx_channel
	if ({exists Ex_channel})
		return
	end
        float tau1 = 0.005
        float tau2 = 0.001
	create		isynchan	Ex_channel
    	setfield	        Ex_channel \
		Ek			{EGlu} \
		tau1		{tau1} \	// sec
		tau2		{tau2} \ 	// sec
		gmax		{GGlu} // Siemens
end

function make_iInh_channel
	if ({exists Inh_channel})
		return
	end
        float tau1 = 0.010
        float tau2 = 0.001
	create		isynchan	Inh_channel
    	setfield	        Inh_channel \
	Ek			{ EGABA } \
        tau1            {tau1} \        // sec
        tau2            {tau2} \        // sec
	gmax		{GGABA}		// Siemens
end
