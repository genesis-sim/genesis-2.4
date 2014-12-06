//genesis
float EREST_ACT = -0.065

function make_cell

	create compartment /proto/cell
		setfield /proto/cell \
			Rm 3.2e9 \
			Cm 6.3e-12 \
			Ra 16e6 \
			Em -0.065

	create tabchannel /proto/cell/Na // set up the Na channel
		setfield /proto/cell/Na \
			Ek 0.045	Gbar 8e-7	Xpower 3	Ypower 1	Zpower 0

		setupalpha /proto/cell/Na X \ // setting up the X gate
			{320e3 * (0.013 + EREST_ACT)}  \
			-320e3 -1.0 {-1.0 * (0.013 + EREST_ACT)} -0.004 \
			{-280e3 * (0.040 + EREST_ACT) }  \
			280e3 -1.0 {-1.0 * (0.040 + EREST_ACT) } 5.0e-3 \
			-size 1000 -range -0.1 0.05

		setupalpha /proto/cell/Na Y \ // setting up the Y gate
			128 0 0 {-1.0 * (0.017 + EREST_ACT)} 0.018 \
 			4e3 0 1 {-1.0 * (0.040 + EREST_ACT)} -5e-3 \
			-size 1000 -range -0.1 0.05

	create tabchannel /proto/cell/K // set up the K channel
		// setfield /proto/cell/K \
			// Ek -0.090	Gbar 2.4e-7	Xpower 4	Ypower 0	Zpower 0
		setfield /proto/cell/K \
			Ek -0.090	Gbar 2.4e-6	Xpower 4	Ypower 0	Zpower 0
		setupalpha /proto/cell/K X \ // setting up the X gate
			{32e3 * (0.015 + EREST_ACT) } \
			-32e3 -1 {-1.0 * (0.015 + EREST_ACT)} -5e-3, \
			500 0 0 {-1.0 * (0.010 + EREST_ACT) } 40e-3 \
			-size 1000 -range -0.1 0.05

	addmsg /proto/cell /proto/cell/Na VOLTAGE Vm
	addmsg /proto/cell/Na /proto/cell CHANNEL Gk Ek

	addmsg /proto/cell /proto/cell/K VOLTAGE Vm
	addmsg /proto/cell/K /proto/cell CHANNEL Gk Ek

	create spikegen /proto/cell/axon 
	setfield /proto/cell/axon abs_refract 0.001 thresh 0.0 output_amp 1
	addmsg /proto/cell /proto/cell/axon INPUT Vm

	create synchan /proto/cell/glu
		setfield /proto/cell/glu \
			tau1	2e-3  tau2	2e-3 \
			gmax	1e-7  Ek	0.045
	addmsg /proto/cell /proto/cell/glu VOLTAGE Vm
	addmsg /proto/cell/glu /proto/cell CHANNEL Gk Ek

	create synchan /proto/cell/GABA
		setfield /proto/cell/GABA \
			tau1	20e-3 \
			tau2	20e-3 \
			gmax	2e-8 \
			Ek		-0.090
	addmsg /proto/cell /proto/cell/GABA VOLTAGE Vm
	addmsg /proto/cell/GABA /proto/cell CHANNEL Gk Ek
end
