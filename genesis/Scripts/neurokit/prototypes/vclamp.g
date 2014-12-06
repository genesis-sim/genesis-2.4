//genesis

/* FILE INFORMATION
** Voltage clamp circuit
** Implemented by : Mark Nelson in the SQUID DEMO
*/



function make_Vclamp
	if (({exists Vclamp}))
		return
	end

	create diffamp Vclamp
	setfield ^ saturation 999.0 \	// unitless I hope
		   gain 0.002	// 1/R  from the lowpass filter input

	create RC Vclamp/lowpass
	setfield ^ R 500.0 C 0.1e-6	// mhms and farads; for a tau of 50 us

	create PID Vclamp/PID
	setfield ^ gain 1e-6 \ 	// 10/Rinput of cell
		tau_i 20e-6 \ 	// seconds
		tau_d 5e-6 saturation 999.0

	addmsg Vclamp/lowpass Vclamp PLUS state
	addmsg Vclamp Vclamp/PID CMD output

	/*
	** Where the remaining messages typically go
	addmsg dummy Vclamp/lowpass	INJECT	x	// Voltage to clamp at
	addmsg {dend} Vclamp/PID	SNS	Vm	// The fb from the dend
	addmsg Vclamp/PID {dend}	INJECT	output	// Current into the dend
	*/
end
