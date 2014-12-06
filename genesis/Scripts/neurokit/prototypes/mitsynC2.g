//genesis

/* FILE INFORMATION
** A couple of synaptic channels for the mitral cell model
** Implemented by Upinder S. Bhalla.
**
** The channels have an alpha_function type conductance change for
** each synaptic event.
**
*/

// CONSTANTS
float EGlu = 0.045
float EGABA = -0.090
float SOMA_A = 1e-9
float GGlu = SOMA_A * 50
float GGABA = SOMA_A * 50

//===================================================================
//                     SYNAPTIC CHANNELS   (Values guessed at)
//===================================================================


function make_glu_mit_upi
	if ({exists glu_mit_upi})
		return
	end

	create		channelC2	glu_mit_upi
    	setfield	        glu_mit_upi \
		Ek			{EGlu} \
		tau1		{ 2.0e-3 } \	// sec
		tau2		{ 2.0e-3 } \ 	// sec
		gmax		{GGlu} // Siemens
end

function make_GABA_mit_upi
	if ({exists GABA_mit_upi})
		return
	end

	create		channelC2	GABA_mit_upi
    	setfield	        GABA_mit_upi \
	Ek			{ EGABA } \
	tau1		{ 20.0e-3 } \	// sec
	tau2		{ 20.0e-3 } \	// sec
	gmax		{GGABA}		// Siemens
end


function make_glu_gran_usb
	if ({exists glu_gran_usb})
		return
	end

	create		channelC2	glu_gran_usb
    	setfield	        glu_gran_usb \
		Ek			{EGlu} \
		tau1		{ 2.0e-3 } \	// sec
		tau2		{ 2.0e-3 } \ 	// sec
		gmax		{GGlu} // Siemens
end

function make_GABA_gran_usb
	if ({exists GABA_gran_usb})
		return
	end

	create		channelC2	GABA_gran_usb
    	setfield	        GABA_gran_usb \
	Ek			{ EGABA } \
	tau1		{ 20.0e-3 } \	// sec
	tau2		{ 20.0e-3 } \	// sec
	gmax		{GGABA}		// Siemens
end
