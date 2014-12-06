//genesis

/* FILE INFORMATION
** "Generic" excitatory (glutamate) and inhibitory (GABA) synaptically
**  activated channels  (based on mitsyn.g by Upi Bhalla)
**
** The channels have an alpha_function type conductance change for
** each synaptic event.
**
** Modified by D. Beeman for GENESIS 2.0 syntax and synchan object - 5/19/94
*/

// CONSTANTS
float EGlu = 0.045
float EGABA = -0.082
float SOMA_A = 1e-9
float GGlu = SOMA_A * 50
float GGABA = SOMA_A * 50

//===================================================================
//                     SYNAPTIC CHANNELS   (Values guessed at)
//===================================================================


function make_Ex_channel
	if ({exists Ex_channel})
		return
	end

	create		synchan	Ex_channel
    	setfield	        Ex_channel \
		Ek			{EGlu} \
		tau1		{ 3.0e-3 } \	// sec
		tau2		{ 3.0e-3 } \ 	// sec
		gmax		{GGlu} // Siemens
end

function make_Inh_channel
	if ({exists Inh_channel})
		return
	end

	create		synchan	Inh_channel
    	setfield	        Inh_channel \
	Ek			{ EGABA } \
	tau1		{ 20.0e-3 } \	// sec
	tau2		{ 20.0e-3 } \	// sec
	gmax		{GGABA}		// Siemens
end
