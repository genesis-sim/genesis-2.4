//genesis


function create_lib_infrastructure
	create neutral /buffer
	create neutral /buffer/temp
end

function create_lib(libname)
	str libname

	create x1form /{libname}_form [0,50,480,370] -nolabel
	disable /{libname}_form
	//DHB create xdraw /{libname}_form/draw [0,0,100%,300] -wx 20 -wy 20  \
	    //DHB -cx 7 -cy 8
	create x1draw /{libname}_form/draw [0,0,100%,300] \
	    -xmin -3 -xmax 17 -ymin -2 -ymax 10
	setfield /{libname}_form/draw refresh_flag FALSE
	setup_mouse /{libname}_form/draw 0 0 select  \
	    "do_lib_select "{libname} dummy do_dummy delete  \
	    "do_lib_delete "{libname}
	copy /xproto/draw/hilight /{libname}_form/draw
	create neutral /{libname}
	disable /{libname}
	create x1dialog /{libname}_form/sel  \
	    -script "do_lib_rename "{libname}
	setfield /{libname}_form/sel ygeom 0:draw.bottom
	create x1toggle /{libname}_form/auto_rename
	setfield ^ label0 "Use sel name when pasting"  \
	    label1 "Do Rall renaming"
end

/*
** add_to_lib : Assumes that the element being added already lives on
** its own neutral carrier which has been named appropriately
*/
function add_to_lib(libname, elm, icon)
	str libname
	str elm
	str icon
	str temp
	int nkids = 0
	str  \
	    iname = {getfield {elm} name} @ "[" @ {getfield {elm} index} @ "]"
	int x

	if ({exists /{libname}/{iname}})
		delete /{libname}/{iname}
	else
		// copy icon over
		// find how many items in this library
		foreach temp ({el /{libname}/#})
			nkids = nkids + 1
		end
		copy /xproto/draw/{icon} /{libname}_form/draw/{iname}
		x = 3*(nkids%6)
		setfield /{libname}_form/draw/{iname} text {iname}  \
		    tx {x} ty {nkids/2.0} value /{libname}/{iname}

		xupdate /{libname}_form/draw
	end

	move {elm} /{libname}/{iname}
end

function move_from_lib(libname, elm, dest)
	str libname
	str elm
	str dest
	str iname = {getfield {elm} name} @ {getfield {elm} index}
	int nkids = 0
	str temp

	move {elm} {dest}
	delete /{libname}_form/draw/{iname}

	foreach temp ({el /{libname}_form/draw/#})
		nkids = nkids + 1
		setfield {temp} tx {2*(nkids%4) - 3} ty {2*(nkids/4)}
	end

	xupdate /{libname}_form/draw
end

function do_lib_select(libname)
	str libname
	str selname = {getfield /{libname}_form/draw value}
	str  \
	    iname = {getfield {selname} name} @ "[" @ {getfield {selname} index} @ "]"

	pushe /{libname}_form
	selname = "draw/" @ (iname)
	setfield draw/hilight tx {getfield {selname} tx}  \
	    ty {getfield {selname} ty}
	setfield sel value {getfield draw value}
	xupdate draw
	pope
end


function do_lib_delete(libname)
	str libname
	str selname = {getfield /{libname}_form/draw value}
	str  \
	    iname = {getfield {selname} name} @ "[" @ {getfield {selname} index} @ "]"

	delete {selname}
	delete /{libname}_form/draw/{iname}
	xupdate /{libname}_form/draw
end


/*
** function do_rename(fullname,new_name,index)
**
** This function traverses the message heirarchy in a cell and renames
** the elements according to Rall's convention, building on the 
** core 'new_name'. Array indices in root names are not 'built on', so
** xy12[3] would have children xy121 and xy122. When there is only
** one child of a given element, it assumes that it is part of a branch
** and uses array indices with the same core name, eg xy12[0],xy12[1]..
** The routine may fail if any new name overlaps existing ones.
*/
function do_rename(fullname, new_name, index)
	str fullname
	str new_name
	int index

	str dst
	int nmsgs, i
	str child
	int nkids = 0
	int kidno = 1
	str skidno
	str kidname
	str sindex

	sindex = index

	ce {fullname}/..
	dst = (new_name) @ "[" @ (sindex) @ "]"
	if (({exists {dst}}))
		echo do_rename failed at duplicated name '{dst}'
		return
	end

	nmsgs = {getmsg {fullname} -incoming -count}
	for (i = 0; i < nmsgs; i = i + 1)
		if ( \
		    {strcmp {getmsg {fullname} -incoming -type {i}} RAXIAL} == 0 \
		    )
			nkids = nkids + 1
		end
	end
	for (i = 0; i < nmsgs; i = i + 1)
		if ( \
		    {strcmp {getmsg {fullname} -incoming -type {i}} RAXIAL} == 0 \
		    )
			child = {getmsg {fullname} -incoming -source {i}}
			if ((nkids) > 1)
				skidno = kidno
				kidname = (new_name) @ (skidno)
				kidno = kidno + 1
				do_rename {child} {kidname} 0
			else
				do_rename {child} {new_name} {index + 1}
			end
		end
	end

	//	echo move {fullname} {dst}
	move {fullname} {dst}
	ce /
end

function do_child_rename(orig, dst)
	str orig, dst

	str newname
	int nmsgs, i, nkids = 0
	int dstindex = {getfield {dst} index}
	str dstname = {getfield {dst} name}
	int kidindex
	str skidindex
	str kidname

	nmsgs = {getmsg {dst} -incoming -count}
	for (i = 0; i < nmsgs; i = i + 1)
		if ( \
		    {strcmp {getmsg {dst} -incoming -type {i}} RAXIAL} == 0 \
		    )
			nkids = nkids + 1
		end
	end

	if (nkids == 0)
		kidindex = 1 + dstindex
		kidname = dstname
	end
	if (nkids > 0)
		kidindex = 0
		kidname = (dstname) + (nkids)
	end

	do_rename {orig} {kidname} {kidindex}

	skidindex = (kidindex)
	newname = (kidname) @ "[" @ (skidindex) @ "]"
	return ((newname))
end



function do_lib_rename(libname)
	str libname

	str newelm = {getfield /{libname}_form/sel value}
	str oldelm = {getfield /{libname}_form/draw value}
	str  \
	    oldiname = {getfield {oldelm} name} @ "[" @ {getfield {oldelm} index} @ "]"

	move {oldelm} {newelm}
	str  \
	    iname = {getfield {newelm} name} @ "[" @ {getfield {newelm} index} @ "]"

	// This is where the Rall renaming routine fits in
	if ( \
	    {strcmp {getfield {newelm}/{oldiname} object->name} "compartment"} == 0 \
	    )
		//	function do_rename(fullname,new_name,index)
		do_rename {newelm}/{oldiname} {getfield {newelm} name}  \
		    {getfield {newelm} index}
	else
		move {newelm}/{oldiname} {newelm}/{iname}
	end

	move /{libname}_form/draw/{oldiname}  \
	    /{libname}_form/draw/{iname}

	setfield /{libname}_form/draw/{iname} value /{libname}/{iname}  \
	    text {iname}
	setfield /{libname}_form/draw value /{libname}/{iname}

	xupdate /{libname}_form/draw
end


/*
** function subtree_copy(src,dst)
**
** copies the subtree off src to the directory dst, including
** all messages (I hope !). This is done in a sly way, by 
** moving all the originals to a temporary element, then
** copying that element to the dst element, then moving
** the originals back. This convoluted approach is necessary because
** somewhere along the line I made the design decision to put
** all the compartments in the same directory.
*/
function subtree_move(src, dst)
	str src, dst

	int nmsgs, i
	str child

	nmsgs = {getmsg {src} -incoming -count}

	for (i = 0; i < nmsgs; i = i + 1)
		if ( \
		    {strcmp {getmsg {src} -incoming -type {i}} RAXIAL} == 0 \
		    )
			child = {getmsg {src} -incoming -source {i}}
			subtree_move {child} {dst}
		end
	end
	move {src} {dst}
end

function reposition_elm(src, dupli)
	str src, dupli

	str parent_dend
	float x, y, z
	int nmsgs, i

	if ({strcmp {getfield {src} object->name} "compartment"} == 0)
        nmsgs = {getmsg {src} -incoming -count}
        for (i = 0; i < nmsgs; i = i + 1)
            if ({strcmp {getmsg {src} -incoming -type {i}} AXIAL} == 0)
                parent_dend = {getmsg {src} -incoming -source {i}}
				break
            end
        end
		if (!({exists {parent_dend}}))
			return
		end
        x = ({getfield {parent_dend} x})
        y = ({getfield {parent_dend} y})
        z = ({getfield {parent_dend} z})
		position {dupli} R{-x} R{-y} R{-z}
	end
end


function glue_kids(src)
	str src

	str parent_dend
	str kid
	float x, y, z, temp
	int nmsgs, i

	if ({strcmp {getfield {src} object->name} "compartment"} == 0)
        nmsgs = {getmsg {src} -incoming -count}
        for (i = 0; i < nmsgs; i = i + 1)
            if ({strcmp {getmsg {src} -incoming -type {i}} AXIAL} == 0)
                parent_dend = {getmsg {src} -incoming -source {i}}
				break
            end
        end
        x = ({getfield {parent_dend} x})
        x = x - ({getfield {src} x})
        y = ({getfield {parent_dend} y})
        y = y - ({getfield {src} y})
        z = ({getfield {parent_dend} z})
        z = z - ({getfield {src} z})
        nmsgs = {getmsg {src} -incoming -count}
        for (i = 0; i < nmsgs; i = i + 1)
            if ({strcmp {getmsg {src} -incoming -type {i}} RAXIAL} == 0)
                kid = {getmsg {src} -incoming -source {i}}
				relposition {kid} {x} {y} {z}
				addmsg {parent_dend} {kid} AXIAL Vm
				addmsg {kid} {parent_dend} RAXIAL Ra Vm
            end
        end
	end
end


function copy_to_lib(libname, src, mode)
	str libname
	str src
	int mode

	str icon = {getfield {src} object->name} @ ".icon"
	str parent
	str  \
	    elmname = ({getfield {src} name}) @ "[" @ ({getfield {src} index}) @ "]"

	parent = {el {src}/..}

	if (({exists /buffer/temp2}))
		delete /buffer/temp2
	end

	if (mode == 0)	// copy element and its direct kids only

		create neutral /buffer/temp2
		copy {src} /buffer/temp2
	else	// copy entire subtree

		subtree_move {src} /buffer/temp
		copy /buffer/temp /buffer/temp2
		// moving original back to where it came from
		subtree_move /buffer/temp/{elmname} {parent}
	end
	// repositioning elm if it is a compt, to compensate for parent
	reposition_elm {src} /buffer/temp2
	// Need to put in routine to inspect source elm type for icon
	// renaming temp2 correctly
	move /buffer/temp2 /buffer/{elmname}
	// moving copy to library
	add_to_lib {libname} /buffer/{elmname} {icon}
end

function cut_to_lib(libname, src, mode)
	str libname
	str src
	int mode

	str icon = {getfield {src} object->name} @ ".icon"
	str  \
	    elmname = ({getfield {src} name}) @ "[" @ ({getfield {src} index}) @ "]"

	if (({exists /buffer/temp2}))
		delete /buffer/temp2
	end

	if (mode == 0)	// need to put in glue stuff for later compts

		glue_kids {src}
		move {src} /buffer/temp
		copy /buffer/temp /buffer/temp2
		// repositioning elm if it is a compt, to compensate for parent
		reposition_elm /buffer/temp /buffer/temp2
		delete /buffer/temp
		create neutral /buffer/temp
	else
		subtree_move {src} /buffer/temp
		copy /buffer/temp /buffer/temp2
		// repositioning elm if it is a compt, to compensate for parent
		reposition_elm /buffer/temp /buffer/temp2
		//	getting rid of original
		delete /buffer/temp
		create neutral /buffer/temp
	end
	// moving copy to library
	// renaming temp2 correctly
	move /buffer/temp2 /buffer/{elmname}
	add_to_lib {libname} /buffer/{elmname} {icon}
end
