//genesis

function do_xcell_funcs
	create x1form /cell_params [480,470,520,450] -nolabel
	ce ^
	disable .
	create x1label "CELL PARAMETERS"
	create x1dialog "Cell Name" -value {cellpath}
	create x1label "COMPARTMENT PARAMETERS"
	create x1dialog "Selected Compartment" -value {comptpath}
	create x1dialog Length -script "set_len "<widget>
	create x1dialog Diameter -script "set_dia "<widget>
	create x1label "CHANNEL PARAMETERS"
	create x1dialog "Selected Channel" -value {channelpath}
	create x1dialog "Channel Type"
	create x1dialog "Conductance (S m^-2)" -script  \
	    "set_cond "<widget>
	create x1dialog "Reversal Potential" -script  \
	    "set_revpotl "<widget>
	ce /
end

function update_neuron_params
	float temp
	pushe /cell_params
	setfield "Cell Name" value {cellpath}

	setfield "Selected Compartment" value {comptpath}
	temp = {calc_len {comptpath}}
	setfield Length value {temp*1e6}
	temp = {calc_dia {comptpath}}
	setfield Diameter value {temp*1e6}

	if ({exists {channelpath}})
		setfield "Selected Channel" value {channelpath}
		setfield "Channel Type"  \
		    value {getfield {channelpath} object->name}
		temp = {calc_cond {channelpath}}
		setfield "Conductance (S m^-2)" value {temp}
		temp = {calc_reversal {channelpath}}
		setfield "Reversal Potential" value {temp}
	else
		setfield "Selected Channel"  \
		    value "No '"{channelname}"' channel here"
		setfield "Channel Type" value ""
		setfield "Conductance (S m^-2)" value 0
		setfield "Reversal Potential" value 0
	end
	pope
end

function do_cellselect
	str name

	name = {getfield /cell_select/draw value}
	if (({exists {name}}))
		cellpath = name
		cellname = {getfield {name} name}
		comptpath = (cellpath) @ "/" @ (comptname)
		channelpath = (comptpath) @ "/" @ (channelname)

		hilight_compt {comptpath}
		/*
		hilight_compt({comptpath},{get(/compt_select/scope,state)})
		*/

		if (({strcmp {analysis_level} neuron}) == 0)
			update_neuron_params
		end
		if (({strcmp {analysis_level} compt}) == 0)
			update_compt_params
		end
		if (({strcmp {analysis_level} channel}) == 0)
			update_channel_params
		end
		display_compt {comptpath}
	end
end
