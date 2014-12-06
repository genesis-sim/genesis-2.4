//genesis

function write_lambda(cellpath)
	str cellpath
	str name
	float nlambda, rm, ri

	openfile lambdafile w
	openfile lambdaf w
	if ((user_symcomps))
		foreach name ({el {cellpath}/#[][TYPE=symcompartment]})
			rm = ({getfield {name} Rm})
			ri = ({getfield {name} Ra})
			nlambda = {sqrt {ri/rm}}
			echo {name} nlambda = {nlambda}
			writefile lambdafile {name} nlambda = {nlambda}
			writefile lambdaf {nlambda}
		end
	else
		foreach name ({el {cellpath}/#[][TYPE=compartment]})
			rm = ({getfield {name} Rm})
			ri = ({getfield {name} Ra})
			nlambda = {sqrt {ri/rm}}
			echo {name} nlambda = {nlambda}
			writefile lambdafile {name} nlambda = {nlambda}
			writefile lambdaf {nlambda}
		end
	end

	closefile lambdafile
	closefile lambdaf
end

function chop_compt(compt, destname, dest_index, prevd, max_nlambda)
	str compt
	str destname
	int dest_index
	str prevd
	float max_nlambda

	float rm, ri, cm, nlambda
	int i, nchops
	str si
	int nmsgs
	int index
	int nkids
	float next_dia
	float this_dia
	float this_len
	float dlen
	float ddia, cdia
	float sub_len
	float fchops
	float gbar
	str child
	str chan
	float px, py, pz, dx, dy, dz
	str parent

	si = dest_index


	nmsgs = {getmsg {compt} -incoming -count}
	for (i = 0; i < nmsgs; i = i + 1)
		if ( \
		    {strcmp {getmsg {compt} -incoming -type {i}} RAXIAL} == 0 \
		    )
			child = {getmsg {compt} -incoming -source {i}}
			nkids = nkids + 1
		end
	end

	parent = {find_parent {compt}}

	px = {getfield {parent} x}
	py = {getfield {parent} y}
	pz = {getfield {parent} z}

	dx = {getfield {compt} x}
	dy = {getfield {compt} y}
	dz = {getfield {compt} z}
	dx = dx - px
	dy = dy - py
	dz = dz - pz

	this_dia = {calc_dia {compt}}
	if (nkids > 0)
		next_dia = {calc_dia {child}}
	end
	this_len = {calc_len {compt}}

	rm = ({getfield {compt} Rm})
	cm = ({getfield {compt} Cm})
	ri = ({getfield {compt} Ra})
	nlambda = {sqrt {ri/rm}}
	// echo nlambda = {nlambda}
	if (nlambda > max_nlambda)
		nlambda = nlambda/max_nlambda
		nchops = nlambda + 1
		fchops = nchops
		dlen = this_len/fchops
		if (nkids == 1)		/* we can do sensible tapering*/

			ddia = (next_dia - this_dia)/fchops
		else
			ddia = 0.0
		end
		cdia = this_dia
		dx = dx/fchops
		dy = dy/fchops
		dz = dz/fchops
		for (i = dest_index; i < (nchops + dest_index);  \
		    i = i + 1)
			si = i
			px = px + dx
			py = py + dy
			pz = pz + dz
			copy {compt} {destname}[{i}]
			addmsg {prevd} {destname}[{i}] AXIAL Vm
			addmsg {destname}[{i}] {prevd} RAXIAL Ra Vm
			prevd = (destname) @ "[" @ (si) @ "]"
			setfield {destname}[{i}]  \
			    Rm {rm*this_len*this_dia/dlen/cdia}  \
			    Ra {ri/this_len*this_dia*this_dia*dlen/cdia/cdia} \
			     Cm {cm/this_len/this_dia*dlen*cdia} x {px}  \
			    y {py} z {pz}
			/* also set all channels */
			foreach chan ( \
			    {el {destname}[{i}]/#[TYPE=channelC2]})
				gbar = {getfield {chan} gmax}
				setfield {chan}  \
				    gmax {gbar/this_len/this_dia*dlen*cdia}
			end
			foreach chan ( \
			    {el {destname}[{i}]/#[TYPE=hh_channel]})
				gbar = {getfield {chan} Gbar}
				setfield {chan}  \
				    Gbar {gbar/this_len/this_dia*dlen*cdia}
			end
			foreach chan ( \
			    {el {destname}[{i}]/#[TYPE=vdep_channel]})
				gbar = {getfield {chan} Gbar}
				setfield {chan}  \
				    Gbar {gbar/this_len/this_dia*dlen*cdia}
			end
			cdia = cdia + ddia
		end
	else
		i = dest_index
		si = i
		copy {compt} {destname}[{i}]
		addmsg {prevd} {destname}[{i}] AXIAL Vm
		addmsg {destname}[{i}] {prevd} RAXIAL Ra Vm
		prevd = (destname) @ "[" @ (si) @ "]"
	end
	return (prevd)
end

/*
** This function needs to start at the soma of the source cell
** The destpath is the name of the neutral element on which the
** new cell will be built. Newname is ignored. Index should
** initially be '0' (zero). Max_nlambda is the max electrical
** length in lambdas, that you wish for each compartment
*/
function chop_lambda(cellpath, destpath, newname, index, prevd,  \
    max_nlambda)
	str cellpath
	str destpath
	str newname
	int index
	str prevd
	float max_nlambda

	str name
	float nlambda, rm, ri
	int nmsgs, i, nkids = 0
	str child
	int kidno = 1
	str skidno
	str lastd
	int ndest
	str kidname

	// echo {cellpath},{newname},{index}

	nmsgs = {getmsg {cellpath} -incoming -count}
	for (i = 0; i < nmsgs; i = i + 1)
		if ( \
		    {strcmp {getmsg {cellpath} -incoming -type {i}} RAXIAL} == 0 \
		    )
			nkids = nkids + 1
		end
	end

	for (i = 0; i < nmsgs; i = i + 1)
		if ( \
		    {strcmp {getmsg {cellpath} -incoming -type {i}} RAXIAL} == 0 \
		    )
			child = {getmsg {cellpath} -incoming -source {i}}
			if (nkids > 1)
				/*
				skidno = kidno
				kidname = {new_name} + {skidno}
				kidno = kidno + 1
				*/
				kidname = {getfield {child} name}
				/*
function chop_compt(compt,last_dest,dest_index,max_nlambda)
*/
				ce {destpath}
				lastd = {chop_compt {child} {kidname} 0 {prevd} {max_nlambda}}
				ndest = {getfield {lastd} index}
				ndest = ndest + 1


				chop_lambda {child} {destpath} {kidname} \
				     {ndest} {lastd} {max_nlambda}
			else			/* only one kid */

				/*
				kidname = new_name
				*/
				kidname = {getfield {child} name}
				ce {destpath}
				lastd = {chop_compt {child} {kidname} {index} {prevd} {max_nlambda}}
				ndest = {getfield {lastd} index}
				ndest = ndest + 1
				chop_lambda {child} {destpath} {kidname} \
				     {ndest} {lastd} {max_nlambda}

			end
		end
	end
end


/*
** Just needs source soma path, place to put dest (need not
** be assigned yet) and max_nlambda.
*/

function subdivide_cell(source, dest, max_nlambda)
	str source
	str dest
	float max_nlambda

	int ndest

	if ({strcmp {getfield {source} object->name} "compartment"} != 0 \
	    )
		echo subdivide_cell must start at a compartment
		return
	end

	if (!({exists {dest}}))
		create neutral {dest}
	end

	if (!({exists {dest}/soma}))
		copy {source} {dest}
	end

	ce {dest}
	/*
	ndest = chop_compt({source},soma,0,{max_nlambda})
	*/
	echo starting subdivision
	chop_lambda {source} {dest} soma 0 soma {max_nlambda}
	echo subdivision completed

end
