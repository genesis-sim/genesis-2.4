//genesis
include feedback.g // This is the file with the feedback model.
// We assume that the enzyme X is in /kinetics/X and Y is in /kinetics/Y.

// This functions generates the conc-eff curve for the enzyme enz,
function vary_enz(enz)
	str enz
	float conc
	float origconc = {getfield {enz} CoInit}
	float dconc = 0.1
	setfield {enz} slave_enable 4 // buffers the enz to CoInit
	echo % varying enz = {enz} >> conc_eff_file
	reset
	for (conc = 0.0; conc < 2; conc = conc + dconc)
		setfield {enz} CoInit {conc}
		step 200 -t
		echo {getfield /kinetics/X Co} {getfield /kinetics/Y Co} \
			>> conc_eff_file
	end
	setfield {enz} \
		slave_enable 0 \
		CoInit {origconc}
end

float dkf = 0.02
float kfx, kfy
// This loop varies kf for the X and Y degradation pathways, and generates
// a conc-eff curve for each case.
for (kfx = 0.08; kfx < 0.24; kfx = kfx + dkf)
	setfield /kinetics/degrade_X kf {kfx}
	for (kfy = 0.08; kfy < 0.24; kfy = kfy + dkf)
		setfield /kinetics/degrade_Y kf {kfy}
		echo % kfx={kfx}, kfy={kfy} >> conc_eff_file
		vary_enz   /kinetics/X
		vary_enz   /kinetics/Y
	end
end
quit

