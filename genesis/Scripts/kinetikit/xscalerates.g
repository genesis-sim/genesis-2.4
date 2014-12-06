//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-2003 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

function do_scale_rates(value)
	float value
	float orig = {getfield /scalerate cumulative_scale}
	str name
	float temp

	if (value < 1e-6)
		do_inform "Error: Cannot scale rates below 1e-6"
		setfield /scalerate/do_scale_rates value 1.0
		return
	end
	
	foreach name ({el /kinetics/##[][TYPE=kreac]})
		temp = {getfield {name} kf}
		setfield {name} kf {temp * value}
		temp = {getfield {name} kb}
		setfield {name} kb {temp * value}
	end
	
	foreach name ({el /kinetics/##[][TYPE=kenz]})
		temp = {getfield {name} k1}
		setfield {name} k1 {temp * value}
		temp = {getfield {name} k2}
		setfield {name} k2 {temp * value}
		temp = {getfield {name} k3}
		setfield {name} k3 {temp * value}
	end
	do_update_reacinfo
	do_update_enzinfo

	setfield /scalerate/do_scale_rates value 1.0
	setfield /scalerate/cumscale \
		label "Cumulative scale factor = "{orig * value}
	setfield /scalerate cumulative_scale {orig * value}
end

function do_restore_scale_rate
	float orig = {getfield /scalerate cumulative_scale}

	do_scale_rates {1.0 / orig}

	setfield /scalerate cumulative_scale 1.0
end

function make_scalerate
	create xform /scalerate [{EX},{EY},{EW},200]
	ce /scalerate
	addfield . cumulative_scale
	setfield . cumulative_scale 1.0

	create xlabel label -label "Reaction/Enzyme rate scaling tool" \
		-bg cyan

	create xdialog do_scale_rates -label "Apply scale factor:" \
		-value 1.0 -script "do_scale_rates <v>"

	create xlabel cumscale -label "Cumulative scale factor = 1.0"
	create xbutton restore -label "Restore to original scale" \
		-script do_restore_scale_rate
	create xbutton Cancel -label "Hide" \
		-script "xhide /scalerate"
	ce /
end
