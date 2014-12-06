//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-2000 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

/******************************************************************
      Stuff for connecting output modules up
******************************************************************/

function isachild(kid, pa)
	str kid
	str pa = {el {pa}}
	str name = {el {kid}/..}

	if ({strlen {kid}} == 0)
		return 0
	end

	while ({strcmp {name} "/"} != 0)
		if ({strcmp {name} {pa}} == 0)
			return 1
		end
		name = {el {name}/..}
	end
	return 0
end

function link_in_use(uplink)
	str uplink

	int i
	
	if ({isa downlink {uplink}})
		i = {getmsg {uplink} -incoming -count}
	else
		i = {getmsg {uplink} -outgoing -count}
	end
	if (i > 0)
		do_inform "Error: Attempting to modify link "{uplink}" which is already in use" 0
		return 1
	end
	return 0
end

// Adds or drops things from uplink depending on whether or not it is
// already there.
function thing_to_uplink(thing, uplink)
	str thing
	str uplink

	if ({isa uplink {thing}})
		return
	end

	if ({link_in_use {uplink}})
		return
	end

	if (!{isachild {thing} {uplink}/..})
		do_inform "Error: "{thing}" is external to "{uplink}". Not added." 0
		return
	end

	return {thing_to_proto {thing} {uplink}/proto}
end


// Connect up a uplink to a downlink.
function uplink_to_downlink_add(uplink, downlink)
	str uplink, downlink

	// echo in uplink_to_downlink_add

	if ({check_compatibility {uplink}/proto {downlink}/proto} == 0)
		do_warning 0 "Error: Incompatible links"
		return
	end

	create_link {uplink} {downlink}
end

function uplink_to_downlink_drop(uplink, downlink)
	str uplink, downlink

	str name, dest
	// Identify and delete linkinfos involved in link from either side.
	foreach name ({el {uplink}/#[][TYPE=linkinfo]})
		dest = {getmsg {name} -outgoing -dest 0}
		if ({strcmp {el {dest}/..} {downlink}} == 0)
			delete {name}
			delete {dest}
		end
	end
	
	// Delete the display INPUT msg.
	deletemsg {downlink} 0 -find {uplink} INPUT

	call /edit/draw/tree RESET
end


/******************************************************************
      Stuff for editing module
******************************************************************/


function do_update_uplinkinfo
	if (!{exists /parmedit/uplink})
		return
	end
	str uplink = {getfield /parmedit/uplink elmpath}
	if ({strlen {uplink}} == 0)
		return
	end
	if (!{exists {uplink}})
		return
	end

	setfield /parmedit/uplink/path value {getpath {uplink} -head}
	setfield /parmedit/uplink/name value {getpath {uplink} -tail}
	call {uplink}/proto/notes TO_XTEXT /parmedit/uplink/notes
	setfield /parmedit/uplink/Color value {getfield {uplink} xtree_fg_req}
	set_disp_range {uplink}/proto/##[] /parmedit/uplink/draw
	setfield /parmedit/uplink/ddraw/msgr[] pixflags v
	int n = {getmsg {uplink}/proto -in -count}
	int i
	str temp
	for (i = 0; i < n; i = i + 1)
		temp = {getmsg {uplink}/proto -in -source {i}}
		// setfield {temp} xtree_shape_req msgr
		setfield /parmedit/uplink/ddraw/msgr[{i}] \
			pixflags ~v \
			text {getpath {temp} -tail}
	end
	call /parmedit/uplink/draw/tree RESET
	xupdate /parmedit/uplink/ddraw
end

function drag_from_proto(dest, srcval, x, y)
	str dest, srcval
	float x, y

	str temp

	if ({strcmp {dest} "/parmedit/uplink/draw"} == 0)
		position {srcval} {x} {y} I
		call {dest}/tree RESET
		call /edit/draw/tree RESET
		return
	end
	if ({strcmp {dest} "/parmedit/uplink/ddraw"} == 0)
		str linkname = {getfield /parmedit/uplink elmpath}
		if ({link_in_use {linkname}} > 0)
			return
		end

		if ({getmsg {srcval}/.. -in -find {srcval} MSGR} >= 0)
			deletemsg {srcval}/.. 0 -in -find {srcval} MSGR
		else
			addmsg {srcval} {srcval}/.. MSGR
		end
		do_update_uplinkinfo
	end
end

function make_xedit_uplink
	create xform /parmedit/uplink [{EX},{EY},{EW},500]
	addfield /parmedit/uplink elmpath \
		-description "path of elm being edited"
	ce /parmedit/uplink
	create xdialog path -wgeom 60% -title "Parent"
	create xdialog name -xgeom 60% -ygeom 0 -wgeom 40% -title "Name" \
		-script "set_item_name <w> <v>"

	create xcoredraw draw [0,0:last,50%,200]
	create xtree draw/tree -treemode geometry -hlmode none \
		-script "edit_elm.D <v>; drag_from_proto.w <d> <S> <x> <y>"
    str name
    foreach name ({el /edit/draw/tree/shape[]})
        if ({getfield {name} index} > 0)
            copy {name} draw/tree
        end
    end

	create xcoredraw ddraw [0:last,0:name,50%,200] \
		-xmin -4 -xmax 2 -ymin -1 -ymax 9
	create xshape ddraw/msgr \
		-drawmode DrawLines \
		-fg white \
		-coords [1,.5,0][0,.2,0][-1,.5,0][1,.5,0][1,-.5,0][-1,-.5,0][-1,.5,0][1,.5,0] \
		-text "msgr 0" \
		-pixflags c \
		-pixflags v \
		-value "msgr" \
		-tx 0 -ty 8
	copy ddraw/msgr ddraw/msgr[1]
	setfield ddraw/msgr[1] text "msgr1" ty 6
	copy ddraw/msgr ddraw/msgr[2]
	setfield ddraw/msgr[2] text "msgr2" ty 4
	copy ddraw/msgr ddraw/msgr[3]
	setfield ddraw/msgr[3] text "msgr3" ty 2
	copy ddraw/msgr ddraw/msgr[4]
	setfield ddraw/msgr[4] text "msgr4" ty 0

	copy ddraw/msgr draw/tree/shape -autoindex

	make_colorbar /parmedit/uplink draw
	create xbutton savenotes -label NOTES [0,0:last,10%,130] \
		-script "save_link_notes <w>" -offbg gray
	create xtext notes [0:last,0:Color,90%,130] -editable 1
	create xbutton UPDATE [0%,0:notes,50%,30] \
		-script "do_update_uplinkinfo"
	create xbutton HIDE [50%,0:notes,50%,30] \
		-script "save_link_notes <w>; xhide /parmedit/uplink"
	ce /
end

function edit_uplink(uplink)
	str uplink

	setfield /parmedit/uplink elmpath {uplink}
	do_update_uplinkinfo
	xshowontop /parmedit/uplink
end

/******************************************************************
      Stuff for initializing links
******************************************************************/

function uplink_create_func(uplink)
	str uplink

	create proto {uplink}/proto
end

function uplinkproto
	create neutral /uplink

//    addfield /uplink notes -description "string for uplink notes and refs"
    addfield /uplink editfunc -description "func for uplink edit"
    addfield /uplink createfunc -description "func for uplink creation"
    addfield /uplink xtree_fg_req -description "color for uplink icon"
    addfield /uplink xtree_textfg_req -description "textcolor for uplink icon"
    // addfield /uplink plotfield -description "field to plot"
    addfield /uplink manageclass -description "Class that can manage it"

    addobject uplink /uplink \
        -author "Upi Bhalla NCBS Feb 2001" \
        -description "link for connecting modules"

    setdefault uplink editfunc "edit_uplink"
    setdefault uplink createfunc "uplink_create_func"
    setdefault uplink xtree_fg_req "cyan"
    setdefault uplink xtree_textfg_req "blue"
    setdefault uplink manageclass "group"
end

function xuplinkproto
	ce /control/lib/tree
	create xshape shape -autoindex \
		-fg cyan \
		-coords [-0.5,-1,0][-1,0,0][-0.5,1,0][-0.5,0.5,0][0.5,0.5,0][0.5,1,0][1,0,0][0.5,-1,0][0.5,-0.5,0][-0.5,-0.5,0] \
		-drawmode FillPoly \
		-linewidth 5 \
		-value "uplink" \
		-pixflags v \
		-pixflags c \
		-script "edit_uplink.D"

	copy {el ^}  /edit/draw/tree -autoindex
	setfield ^ \
		script "edit_uplink.D"

    ce /
end

function init_xuplink
    // set up the prototype
	uplinkproto
	if (DO_X)
	    xuplinkproto
	
		// Set up the calls used to handle uplink to downlink
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=uplink]" \
			"/kinetics/##[TYPE=downlink]" INPUT downlink red -1 0 \
			"echo.p dragging <S> to <D> for interacting modules" \
			"uplink_to_downlink_add.p <S> <D>" \
			"uplink_to_downlink_drop.p <S> <D>"
	
		// Set up call for dragging things to uplink
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[]" \
			"/kinetics/##[TYPE=uplink]" all all none -1 0 \
			"thing_to_uplink.p <S> <D>" \
			"" \
			""
	
	    // make the editor for uplinks
	    make_xedit_uplink
	end
	// Set up the dumping fields
	simobjdump uplink \
		notes xtree_fg_req \
		xtree_textfg_req x y z
end

/******************************************************************/


