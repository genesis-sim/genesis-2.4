//genesis

function do_xcompt_funcs

	int channo

	create x1form /compt_params [480,440,520,450] -nolabel
	ce ^
	disable .
	create x1label "COMPARTMENT PARAMETERS"
	create x1dialog "Selected Compartment" -value {comptpath}
	create x1dialog "Parent Compartment"
	create x1label "Electrical properties"
	create x1dialog "Membrane resistance (Ohms)" -script  \
	    "set_Rm "<widget>
	create x1dialog "Axial resistance (Ohms)" -script  \
	    "set_Ra "<widget>
	create x1dialog "Membrane capacitance (Farads)" -script  \
	    "set_Cm "<widget>
	create x1label "Constants"
	create x1dialog "Membrane Resistivity (Ohm m^2)" -script  \
	    "set_RM "<widget>
	create x1dialog "Axial Resistivity (Ohm m)" -script  \
	    "set_RA "<widget>
	create x1dialog "Membrane Capacitance (F m^-2)" -script  \
	    "set_CM "<widget>
	create x1label Geometry -title  \
	    "Compartment Geometry. All values in microns"
	create x1dialog Length [2%,3:Geometry.bottom,47%,30] -value 0  \
	    -script "set_len "<widget>
	create x1dialog Diameter [51%,3:Geometry.bottom,47%,30] -value 1  \
	    -script "set_dia "<widget>
	create x1dialog x [2%,36:Geometry.bottom,30%,30] -script  \
	    "set_x "<widget>
	create x1dialog y [34%,36:Geometry.bottom,32%,30] -script  \
	    "set_y "<widget>
	create x1dialog z [68%,36:Geometry.bottom,30%,30] -script  \
	    "set_z "<widget>
end

function find_parent(compt)
	str compt
	int i, nmsgs

	nmsgs = {getmsg {compt} -incoming -count}
	for (i = 0; i < nmsgs; i = i + 1)
		if ( \
		    {strcmp {getmsg {compt} -incoming -type {i}} AXIAL} == 0 \
		    )
			return  \
			    (({getmsg {compt} -incoming -source {i}}))
		end
	end
	return ("none")
end

function calc_len(compt)
	str compt

	str parent
	float len, l1, l2, Ra, Rm, dia

	parent = {find_parent {compt}}
	if ({strcmp {parent} none} == 0)
		dia = {getfield {compt} dia}
		if (dia > 0.0)
			len = {getfield {compt} Cm}
			len = len/(CM*PI*dia)
			return ((len))
		else
			Rm = {getfield {compt} Rm}
			Ra = {getfield {compt} Ra}
			len = RM*RM*Ra/(4*PI*RA*Rm*Rm)
			len = {log {len}}
			len = {exp {len/3}}
			return ((len))
		end
	end
	l1 = {getfield {compt} x}
	l2 = {getfield {parent} x}
	len = (l1 - l2)*(l1 - l2)

	l1 = {getfield {compt} y}
	l2 = {getfield {parent} y}
	len = len + (l1 - l2)*(l1 - l2)

	l1 = {getfield {compt} z}
	l2 = {getfield {parent} z}
	len = len + (l1 - l2)*(l1 - l2)

	len = {sqrt {len}}

	return ((len))
end

function calc_dia(compt)
	str compt

	float dia, len, res, cap

	dia = {getfield {compt} dia}
	if (dia <= 0.0)
		len = {calc_len {compt}}
		cap = {getfield {compt} Cm}
		res = {getfield {compt} Rm}
		dia = RM/(res*len*PI)
	//	dia = cap / (CM * len * PI)
	end
	return ((dia))
end

function update_compt_params
	float temp
	pushe /compt_params

	// Compartment identifiers
	setfield "Selected Compartment" value {comptpath}
	setfield "Parent Compartment" value {find_parent {comptpath}}

	// Electrical properties
	setfield "Membrane resistance (Ohms)"  \
	    value {getfield {comptpath} Rm}
	setfield "Axial resistance (Ohms)"  \
	    value {getfield {comptpath} Ra}
	setfield "Membrane capacitance (Farads)"  \
	    value {getfield {comptpath} Cm}

	// Electrical Constants
	setfield "Membrane Resistivity (Ohm m^2)" value {RM}
	setfield "Axial Resistivity (Ohm m)" value {RA}
	setfield "Membrane Capacitance (F m^-2)" value {CM}

	// Geometry
	temp = {calc_len {comptpath}}
	setfield Length value {temp*1e6}
	temp = {calc_dia {comptpath}}
	setfield Diameter value {temp*1e6}
	temp = {getfield {comptpath} x}
	setfield x value {temp*1e6}
	temp = {getfield {comptpath} y}
	setfield y value {temp*1e6}
	temp = {getfield {comptpath} z}
	setfield z value {temp*1e6}
	pope
end

function set_Rm(widget)
	str widget
	setfield {comptpath} Rm {getfield {widget} value}
end

function set_Ra(widget)
	str widget
	setfield {comptpath} Ra {getfield {widget} value}
end

function set_Cm(widget)
	str widget
	setfield {comptpath} Cm {getfield {widget} value}
end

function set_RM(widget)
	str widget
	RM = {getfield {widget} value}
end

function set_RA(widget)
	str widget
	RA = {getfield {widget} value}
end

function set_CM(widget)
	str widget
	CM = {getfield {widget} value}
end

function set_len(widget)
	str widget
	str parent
	float len, old_len, ratio
	float dx, dy, dz, temp

	old_len = {calc_len {comptpath}}
	len = {getfield {widget} value}
	len = len/1e6
	if (len <= 0)
		echo Invalid compartment length
		return
	end
	ratio = len/old_len

	// Setting electrical properties
	temp = {getfield {comptpath} Rm}
	setfield {comptpath} Rm {temp/ratio}
	temp = {getfield {comptpath} Ra}
	setfield {comptpath} Ra {temp*ratio}
	temp = {getfield {comptpath} Cm}
	setfield {comptpath} Cm {temp*ratio}

	// Setting geometry
	parent = {find_parent {comptpath}}
	if (({strcmp {parent} none}) == 0)	// changing soma only

		update_compt_params
		return
	end

	temp = {getfield {parent} x}
	dx = {getfield {comptpath} x}
	dx = (dx - temp)*(ratio - 1)
	temp = {getfield {parent} y}
	dy = {getfield {comptpath} y}
	dy = (dy - temp)*(ratio - 1)
	temp = {getfield {parent} z}
	dz = {getfield {comptpath} z}
	dz = (dz - temp)*(ratio - 1)

	do_rel_position {comptpath} {dx} {dy} {dz}

	update_compt_params
	if ((user_symcomps))
		setfield /compt_select/draw/cell  \
		    path {cellpath}/##[TYPE=symcompartment]
	else
		setfield /compt_select/draw/cell  \
		    path {cellpath}/##[TYPE=compartment]
	end
end

function set_dia(widget)
	str widget
	float dia, old_dia, ratio
	float dx, dy, dz, temp

	old_dia = {calc_dia {comptpath}}
	dia = {getfield {widget} value}
	dia = dia/1e6
	if (dia <= 0)
		echo Invalid compartment diameter
		return
	end
	ratio = dia/old_dia

	// Setting electrical properties
	temp = {getfield {comptpath} Rm}
	setfield {comptpath} Rm {temp/ratio}
	temp = {getfield {comptpath} Ra}
	setfield {comptpath} Ra {temp/(ratio*ratio)}
	temp = {getfield {comptpath} Cm}
	setfield {comptpath} Cm {temp*ratio}
	setfield {comptpath} dia {dia}

	update_compt_params
	if ((user_symcomps))
		setfield /compt_select/draw/cell  \
		    path {cellpath}/##[TYPE=symcompartment]
	else
		setfield /compt_select/draw/cell  \
		    path {cellpath}/##[TYPE=compartment]
	end
end

function set_x(widget)
	str widget
	float len, old_len, ratio, temp
	float oldx, newx, dx

	old_len = {calc_len {comptpath}}
	newx = {getfield {widget} value}
	newx = newx/1e6
	oldx = {getfield {comptpath} x}
	dx = newx - oldx
	do_rel_position {comptpath} {dx} 0 0

	len = {calc_len {comptpath}}
	if (len <= 0)
		echo Invalid compartment length
		return
	end
	ratio = len/old_len

	// Setting electrical properties
	temp = {getfield {comptpath} Rm}
	setfield {comptpath} Rm {temp/ratio}
	temp = {getfield {comptpath} Ra}
	setfield {comptpath} Ra {temp*ratio}
	temp = {getfield {comptpath} Cm}
	setfield {comptpath} Cm {temp*ratio}

	// updating the screen
	update_compt_params
	if ((user_symcomps))
		setfield /compt_select/draw/cell  \
		    path {cellpath}/##[TYPE=symcompartment]
	else
		setfield /compt_select/draw/cell  \
		    path {cellpath}/##[TYPE=compartment]
	end
end

function set_y(widget)
	str widget
	float len, old_len, ratio, temp
	float oldy, newy, dy

	old_len = {calc_len {comptpath}}
	newy = {getfield {widget} value}
	newy = newy/1e6
	oldy = {getfield {comptpath} y}
	dy = newy - oldy
	do_rel_position {comptpath} 0 {dy} 0

	len = {calc_len {comptpath}}
	if (len <= 0)
		echo Invalid compartment length
		return
	end
	ratio = len/old_len

	// Setting electrical properties
	temp = {getfield {comptpath} Rm}
	setfield {comptpath} Rm {temp/ratio}
	temp = {getfield {comptpath} Ra}
	setfield {comptpath} Ra {temp*ratio}
	temp = {getfield {comptpath} Cm}
	setfield {comptpath} Cm {temp*ratio}

	// updating the screen
	update_compt_params
	if ((user_symcomps))
		setfield /compt_select/draw/cell  \
		    path {cellpath}/##[TYPE=symcompartment]
	else
		setfield /compt_select/draw/cell  \
		    path {cellpath}/##[TYPE=compartment]
	end
end

function set_z(widget)
	str widget
	float len, old_len, ratio, temp
	float oldz, newz, dz

	old_len = {calc_len {comptpath}}
	newz = {getfield {widget} value}
	newz = newz/1e6
	oldz = {getfield {comptpath} z}
	dz = newz - oldz
	do_rel_position {comptpath} 0 0 {dz}

	len = {calc_len {comptpath}}
	if (len <= 0)
		echo Invalid compartment length
		return
	end
	ratio = len/old_len

	// Setting electrical properties
	temp = {getfield {comptpath} Rm}
	setfield {comptpath} Rm {temp/ratio}
	temp = {getfield {comptpath} Ra}
	setfield {comptpath} Ra {temp*ratio}
	temp = {getfield {comptpath} Cm}
	setfield {comptpath} Cm {temp*ratio}

	// updating the screen
	update_compt_params
	if ((user_symcomps))
		setfield /compt_select/draw/cell  \
		    path {cellpath}/##[TYPE=symcompartment]
	else
		setfield /compt_select/draw/cell  \
		    path {cellpath}/##[TYPE=compartment]
	end
end

