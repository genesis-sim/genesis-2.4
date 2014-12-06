//genesis

function do_rel_position(src, x, y, z)
	str src
	float x, y, z
	int nmsgs, i

	float tx, ty, tz
	str child

	nmsgs = {getmsg {src} -incoming -count}

	tx = x + {getfield {src} x}
	ty = y + {getfield {src} y}
	tz = z + {getfield {src} z}
	setfield {src} x {tx}
	setfield {src} y {ty}
	setfield {src} z {tz}

	for (i = 0; i < nmsgs; i = i + 1)
		if ( \
		    {strcmp {getmsg {src} -incoming -type {i}} RAXIAL} == 0 \
		    )
			child = {getmsg {src} -incoming -source {i}}
			do_rel_position {child} {x} {y} {z}
		end
	end
end

function do_rel_position(src, x, y, z)
	/* testing the C version of the routine. */
	relposition {src} {x} {y} {z}
end

function do_z_rotate(name, x, y, sin, cos)
	str name
	float x, y, sin, cos

	int nmsgs, i
	str child
	float dx, dy

	nmsgs = {getmsg {name} -incoming -count}

	for (i = 0; i < nmsgs; i = i + 1)
		if ( \
		    {strcmp {getmsg {name} -incoming -type {i}} RAXIAL} == 0 \
		    )
			child = {getmsg {name} -incoming -source {i}}
			do_z_rotate {child} {x} {y} {sin} {cos}
		end
	end

	dx = {getfield {name} x} - x
	dy = {getfield {name} y} - y
	setfield {name} x {x + cos*dx - sin*dy}
	setfield {name} y {y + sin*dx + cos*dy}
end

function do_x_rotate(name, y, z, sin, cos)
	str name
	float y, z, sin, cos

	int nmsgs, i
	str child
	float dy, dz

	nmsgs = {getmsg {name} -incoming -count}

	for (i = 0; i < nmsgs; i = i + 1)
		if ( \
		    {strcmp {getmsg {name} -incoming -type {i}} RAXIAL} == 0 \
		    )
			child = {getmsg {name} -incoming -source {i}}
			do_x_rotate {child} {y} {z} {sin} {cos}
		end
	end

	dy = {getfield {name} y} - y
	dz = {getfield {name} z} - z
	setfield {name} y {y + cos*dy - sin*dz}
	setfield {name} z {z + sin*dy + cos*dz}
end


function do_y_rotate(name, x, z, sin, cos)
	str name
	float x, z, sin, cos

	int nmsgs, i
	str child
	float dx, dz

	nmsgs = {getmsg {name} -incoming -count}

	for (i = 0; i < nmsgs; i = i + 1)
		if ( \
		    {strcmp {getmsg {name} -incoming -type {i}} RAXIAL} == 0 \
		    )
			child = {getmsg {name} -incoming -source {i}}
			do_y_rotate {child} {x} {z} {sin} {cos}
		end
	end

	dx = {getfield {name} x} - x
	dz = {getfield {name} z} - z
	setfield {name} x {x + cos*dx - sin*dz}
	setfield {name} z {z + sin*dx + cos*dz}
end

function scale_el_props(elm, ratio, scope)
	str elm
	float ratio
	int scope

	float temp
	float nmsgs, i
	str name

	temp = {getfield {elm} Ra}
	setfield {elm} Ra {temp*ratio}
	temp = {getfield {elm} Rm}
	setfield {elm} Rm {temp/ratio}
	temp = {getfield {elm} Cm}
	setfield {elm} Cm {temp*ratio}
	if (scope == 0)
		return
	end
	nmsgs = {getmsg {elm} -incoming -count}

	for (i = 0; i < nmsgs; i = i + 1)
		if ( \
		    {strcmp {getmsg {elm} -incoming -type {i}} RAXIAL} == 0 \
		    )
			name = {getmsg {elm} -incoming -source {i}}
			scale_el_props {name} {ratio} 1
		end
	end
end


function do_stretch(x,y,z)  // click coords added DEB 8/2/95
	float x, y, z
	str elm, parent
	float cos, sin
	float dotprd, crossprd
	float dx, dy, dz, ex, ey, ez, px, py, pz, elx, ely, elz
	float len
	float init_len, final_len, ratio
	int nmsgs, i

	if ({strncmp {getfield /compt_select/draw transform} o 1} == 0)
		echo Draw transform should be one of xy, yz, xz.
		return
	end

	elm = comptpath
	nmsgs = {getmsg {elm} -incoming -count}
	for (i = 0; i < nmsgs; i = i + 1)
		if ( \
		    {strcmp {getmsg {elm} -incoming -type {i}} AXIAL} == 0 \
		    )
			parent = {getmsg {elm} -incoming -source {i}}
		end
	end

	elx = {getfield {elm} x}
	ely = {getfield {elm} y}
	elz = {getfield {elm} z}
	px = {getfield {parent} x}
	py = {getfield {parent} y}
	pz = {getfield {parent} z}

	init_len = (elx - px)*(elx - px) + (ely - py)*(ely - py) + (elz - pz)*(elz - pz)

	if ({strncmp {getfield /compt_select/draw transform} z 1} == 0)
		dx =  x - px
		dy =  y - py
		ex = elx - px
		ey = ely - py
		dotprd = dx*ex + dy*ey
		crossprd = ex*dy - dx*ey
		len = ex*ex + ey*ey
		cos = dotprd/len
		sin = crossprd/len
		dx = cos*ex - sin*ey - ex
		dy = sin*ex + cos*ey - ey
		if (({getfield /compt_select/scope state}) == 0)
			do_rel_position {elm} {dx} {dy} 0
		else
			do_z_rotate {elm} {px} {py} {sin} {cos}
		end
	end
	if ({strncmp {getfield /compt_select/draw transform} x 1} == 0)
		dz =  z - pz
		dy =  y - py
		ez = elz - pz
		ey = ely - py
		dotprd = dz*ez + dy*ey
		crossprd = dz*ey - ez*dy
		len = ez*ez + ey*ey
		cos = dotprd/len
		sin = crossprd/len
		dy = cos*ey - sin*ez - ey
		dz = sin*ey + cos*ez - ez
		if (({getfield /compt_select/scope state}) == 0)
			do_rel_position {elm} 0 {dy} {dz}
		else
			do_x_rotate {elm} {py} {pz} {sin} {cos}
		end
	end
	if ({strncmp {getfield /compt_select/draw transform} y 1} == 0)
		dx =  x - px
		dz =  z - pz
		ex = elx - px
		ez = elz - pz
		dotprd = dx*ex + dz*ez
		crossprd = ex*dz - dx*ez
		len = ex*ex + ez*ez
		cos = dotprd/len
		sin = crossprd/len
		dx = cos*ex - sin*ez - ex
		dz = sin*ex + cos*ez - ez
		if (({getfield /compt_select/scope state}) == 0)
			do_rel_position {elm} {dx} 0 {dz}
		else
			do_y_rotate {elm} {px} {pz} {sin} {cos}
		end
	end

	elx = {getfield {elm} x}
	ely = {getfield {elm} y}
	elz = {getfield {elm} z}
	final_len = (elx - px)*(elx - px) + (ely - py)*(ely - py) + (elz - pz)*(elz - pz)
	ratio = {sqrt {final_len/init_len}}
	scale_el_props {elm} {ratio}  \
	    {getfield /compt_select/scope state}
	if ((user_symcomps))
		setfield /compt_select/draw/cell  \
		    path {cellpath}/##[TYPE=symcompartment]
	else
		setfield /compt_select/draw/cell  \
		    path {cellpath}/##[TYPE=compartment]
	end
end

function do_rotate(x,y,z)  // click coords added DEB 8/2/95
	float x, y, z
	str elm, parent
	float cos, sin
	float dotprd, crossprd
	float dx, dy, dz, ex, ey, ez, px, py, pz, elx, ely, elz
	float len
	int nmsgs, i

	elm = comptpath
	nmsgs = {getmsg {elm} -incoming -count}
	for (i = 0; i < nmsgs; i = i + 1)
		if ( \
		    {strcmp {getmsg {elm} -incoming -type {i}} AXIAL} == 0 \
		    )
			parent = {getmsg {elm} -incoming -source {i}}
		end
	end

	elx = {getfield {elm} x}
	ely = {getfield {elm} y}
	elz = {getfield {elm} z}
	px = {getfield {parent} x}
	py = {getfield {parent} y}
	pz = {getfield {parent} z}

	if ({strncmp {getfield /compt_select/draw transform} z 1} == 0)
		dx =  x - px
		dy =  y - py
		ex = elx - px
		ey = ely - py
		dotprd = dx*ex + dy*ey
		crossprd = ex*dy - dx*ey
		len = {sqrt {dx*dx + dy*dy}}
		len = len*{sqrt {ex*ex + ey*ey}}
		cos = dotprd/len
		sin = crossprd/len
		dx = cos*ex - sin*ey - ex
		dy = sin*ex + cos*ey - ey
		if (({getfield /compt_select/scope state}) == 0)
			do_rel_position {elm} {dx} {dy} 0
		else
			do_z_rotate {elm} {px} {py} {sin} {cos}
		end
	end
	if ({strncmp {getfield /compt_select/draw transform} x 1} == 0)
		dz =  z - pz
		dy =  y - py
		ez = elz - pz
		ey = ely - py
		dotprd = dz*ez + dy*ey
		crossprd = dz*ey - ez*dy
		len = {sqrt {dz*dz + dy*dy}}
		len = len*{sqrt {ez*ez + ey*ey}}
		cos = dotprd/len
		sin = crossprd/len
		dy = cos*ey - sin*ez - ey
		dz = sin*ey + cos*ez - ez
		if (({getfield /compt_select/scope state}) == 0)
			do_rel_position {elm} 0 {dy} {dz}
		else
			do_x_rotate {elm} {py} {pz} {sin} {cos}
		end
	end
	if ({strncmp {getfield /compt_select/draw transform} y 1} == 0)
		dx =  x - px
		dz =  z - pz
		ex = elx - px
		ez = elz - pz
		dotprd = dx*ex + dz*ez
		crossprd = ex*dz - dx*ez
		len = {sqrt {dx*dx + dz*dz}}
		len = len*{sqrt {ex*ex + ez*ez}}
		cos = dotprd/len
		sin = crossprd/len
		dx = cos*ex - sin*ez - ex
		dz = sin*ex + cos*ez - ez
		if (({getfield /compt_select/scope state}) == 0)
			do_rel_position {elm} {dx} 0 {dz}
		else
			do_y_rotate {elm} {px} {pz} {sin} {cos}
		end
	end
	if ( \
	    {strcmp {getfield /compt_select/draw transform} o} == 0 \
	    )
		echo Draw transform should be one of xy ,yz, xz.
	else
		if ((user_symcomps))
			setfield /compt_select/draw/cell  \
			    path {cellpath}/##[TYPE=symcompartment]
		else
			setfield /compt_select/draw/cell  \
			    path {cellpath}/##[TYPE=compartment]
		end
	end
end

