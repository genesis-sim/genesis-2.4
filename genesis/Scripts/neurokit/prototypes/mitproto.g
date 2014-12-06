//genesis

if (!({exists /library/mGABA}))
	create channelC2 /library/mGABA
end
	// sec
	// sec
	// Siemens
	setfield /library/mGABA Ek {EK} tau1 4e-3 tau2 4e-3  \
	    gmax {GMAX_K}

if (!({exists /library/mglu}))
	create channelC2 /library/mglu
end
	// sec
	// sec
	// Siemens
	setfield /library/mglu Ek {ENA} tau1 2e-3 tau2 2e-3  \
	    gmax {DISTAL_GMAX_NA}

if (!({exists /library/gglu}))
	create channelC2 /library/gglu
end
	// sec
	// sec
	// Siemens
	setfield /library/gglu Ek {ENA} tau1 2e-3 tau2 2e-3  \
	    gmax {DISTAL_GMAX_NA}

if (!({exists /library/aspike}))
	create spike /library/aspike
		// V
		// sec
		setfield /library/aspike thresh -0.01 abs_refract 0.01  \
		    output_amp 1
	create axon /library/aspike/axon
	addmsg /library/aspike /library/aspike/axon BUFFER name
end

if (!({exists /library/term}))
	create channelA /library/term
		// Seimens
		// V 
		setfield /library/term Gk 5e-9 Ek 1.0
end

// See mit definition file
EREST_ACT = -0.065
/* Adjusting the EREST dependent terms to fit with rest pot of cell */
// V
// V
// V
// V
setfield /library/MHH_Na_channel X_alpha_V0 {0.013 + {EREST_ACT}}  \
    X_beta_V0 {40.0e-3 + {EREST_ACT}} Y_alpha_V0 {0.017 + {EREST_ACT}}  \
    Y_beta_V0 {40.0e-3 + {EREST_ACT}}

// V
// V
setfield /library/MHH_K_channel X_alpha_V0 {0.015 + {EREST_ACT}}  \
    X_beta_V0 {0.010 + {EREST_ACT}}



// See gran definition file
EREST_ACT = -0.06
/* Adjusting the EREST dependent terms to fit with rest pot of cell */
// V
// V
// V
// V
setfield /library/GHH_Na_channel X_alpha_V0 {0.023 + {EREST_ACT}}  \
    X_beta_V0 {50.0e-3 + {EREST_ACT}} Y_alpha_V0 {0.027 + {EREST_ACT}}  \
    Y_beta_V0 {50.0e-3 + {EREST_ACT}}

// V
// V
setfield /library/GHH_K_channel X_alpha_V0 {0.025 + {EREST_ACT}}  \
    X_beta_V0 {0.020 + {EREST_ACT}}


if (!({exists /library/YCa}))
	int ndivs

	create vdep_channel /library/YCa
	// V
	// S
	setfield /library/YCa Ek {ECA_ACT} gbar {200*{SOMA_A}}

	create tabgate /library/YCa/mgate
	call /library/YCa/mgate TABCREATE alpha 100 -0.2 0.1
	call /library/YCa/mgate TABCREATE beta 100 -0.2 0.1
	x = get(/library/YCa/mgate,alpha->xmin)
	dx = get(/library/YCa/mgate,alpha->dx)
	ndivs = {getfield /library/YCa/mgate alpha->xdivs}
	// Mit definitions
	EREST_ACT = -0.065
	int i
	for (i = 0; i <= ndivs; i = i + 1)
		if ({x} < -0.032)
			setfield /library/YCa/mgate  \
			    alpha->table[{i}] 0.0

			y = (exp({({x} + 0.006 - {EREST_ACT})/0.016}) +  \
			    exp({-({x} + 0.006 - {EREST_ACT})/0.016})) / \
			     7.8e-3
			setfield /library/YCa/mgate beta->table[{i}] {y}
		else
			y = (exp({({x} + 0.006 - {EREST_ACT})/0.016}) +  \
			    exp({-({x} + 0.006 - {EREST_ACT})/0.016})) / \
			     (7.8e-3 * (1.0 +  \
			    exp({-({x} - 0.003 - {EREST_ACT})/0.008})))
			setfield /library/YCa/mgate  \
			    alpha->table[{i}] {y}

			y = (exp({({x} + 0.006 - {EREST_ACT})/0.016}) +  \
			    exp({-({x} + 0.006 - {EREST_ACT})/0.016})) / \
			     7.8e-3 * (1.0 - 1.0 / (1.0 +  \
			    exp({-({x} - 0.003 - {EREST_ACT})/0.008})))
			setfield /library/YCa/mgate beta->table[{i}] {y}
		end
		x = x + dx
	end


	create table /library/YCa/hgate
	call /library/YCa/hgate TABCREATE 100 0.0 1.0
	x = get(/library/YCa/hgate,table->xmin)
	dx = get(/library/YCa/hgate,table->dx)
	ndivs = {getfield /library/YCa/hgate table->xdivs}
	for (i = 0; i <= ndivs; i = i + 1)
		y = 0.01/(0.01 + x)
		setfield /library/YCa/hgate table->table[{i}] {y}
		x = x + dx
	end

	create table /library/YCa/nernst
	call /library/YCa/nernst TABCREATE 1000 0.00005 0.01
	x = get(/library/YCa/nernst,table->xmin)
	dx = get(/library/YCa/nernst,table->dx)
	ndivs = {getfield /library/YCa/nernst table->xdivs}
	for (i = 0; i <= ndivs; i = i + 1)
		y = 12.5e-3 * log({4.0/{x}})
		setfield /library/YCa/nernst table->table[{i}] {y}
		x = x + dx
	end

	create Ca_concen /library/YCa/conc
	// sec
	// Moles per coulomb, later scaled to conc
	// Moles per cubic m
	setfield /library/YCa/conc tau 0.00001 B 5.2e-6 Ca_base 1.0e-4

	addmsg /library/YCa /library/YCa/conc I_Ca Ik
	addmsg /library/YCa/conc /library/YCa/nernst INPUT Ca
	addmsg /library/YCa/conc /library/YCa/hgate INPUT Ca
	addmsg /library/YCa/nernst /library/YCa EK output
	addmsg /library/YCa/hgate /library/YCa MULTGATE output 1
	addmsg /library/YCa/mgate /library/YCa MULTGATE m 1


end
