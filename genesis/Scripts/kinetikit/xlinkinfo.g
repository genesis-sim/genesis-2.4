//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 2001 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

/******************************************************************
	Linkinfos handle the contents of a link between two modules.
	There is a linkinfo at each end of a link, both uplink and 
	downlink. They are independent from the data structure point
	of view, but to the user they are identical and refer to the
	link as a single entity. They are container objects for
	the stuff in protos, as well as info for the links themselves.
******************************************************************/

/******************************************************************
      Linkinfos cannot be dragged to or from
******************************************************************/

// This creates a link. It uses messages extensively to ensure
// coherence. It also creates the link objects.
function create_link(uplink, downlink)
	str uplink
	str downlink

	str upname = {getpath {uplink} -tail}
	str downname = {getpath {downlink} -tail}

	addmsg {uplink} {downlink} INPUT
	create linkinfo {uplink}/{downname} -autoindex
	str uli = {el ^}
	int y = {getarg {el {uplink}/#[][TYPE=linkinfo]} -count}
	setfield {uli} x {getfield {uplink} x} \
		y {{getfield {uplink} y} + y}

	create linkinfo {downlink}/{upname} -autoindex
	str dli = {el ^}
	y = {getarg {el {downlink}/#[][TYPE=linkinfo]} -count}
	setfield {dli} x {getfield {downlink} x} \
		y {{getfield {downlink} y} + y}
	addmsg {uli} {dli} LINK

	dup_protos {uli} {dli}
	call /edit/draw/tree RESET
end

function delete_link(uplink, downlink)
	echo in delete_link {uplink} {downlink}
end

/******************************************************************
      Stuff for editing linkinfo
******************************************************************/

function conv_name(path)
	str path

	int i = {findchar {substring {path} 1} /}
	return {substring {path} {i + 2}}
end

function do_update_linkinfo
	str linkinfo = {getfield /parmedit/linkinfo elmpath}
	str pa = {el {linkinfo}/..}
	str uplink
	str downlink
	str uli, dli

	if ({isa downlink {pa}})
		dli = linkinfo
		downlink = {pa}
		uli = {getmsg {linkinfo} -incoming -source 0}
		uplink = {getpath {uli} -head}
	else
		uli = linkinfo
		uplink = {pa}
		dli = {getmsg {linkinfo} -outgoing -dest 0}
		downlink = {getpath {dli} -head}
	end
	// echo uplink = {uplink} downlink = {downlink}

	pushe /parmedit/linkinfo
	// setfield path value {getpath {linkinfo} -head}
	// setfield name value {getpath {linkinfo} -tail}

	// If the downlink is a msgr, use the uplink notes.
	if ({getfield {dli}/proto nelm} == {getfield {dli}/proto npool})
		call {uli}/proto/notes TO_XTEXT notes
	else
		call {dli}/proto/notes TO_XTEXT notes
	end
	// setfield notes initialtext {getfield {linkinfo} notes}
	setfield uplink_name label {conv_name {uplink}}
	setfield downlink_name label {conv_name {downlink}}

	set_disp_range {uli}/proto/##[] /parmedit/linkinfo/udraw
	set_disp_range {dli}/proto/##[] /parmedit/linkinfo/ddraw
	pope
end

function drag_from_link(src, dest, srcval, x, y)
	str src, dest, srcval
	float x, y

	if ({strcmp {dest} {src}} == 0)
		position {srcval} {x} {y} I
		call {src} RESET
		call /edit/draw/tree RESET
		return
	end
end

// This function goes to the child proto for the notes elm.
function save_link_notes(widget)
	str widget
	str elm = {getfield {widget}/.. elmpath} @ "/proto"

	call {widget}/../notes PROCESS
	call {elm}/notes FROM_XTEXT {widget}/../notes
end

function make_xedit_linkinfo
	create xform /parmedit/linkinfo  [{EX},{EY},{EW},500]
	addfield /parmedit/linkinfo elmpath \
		-description "path of elm being edited"
	ce /parmedit/linkinfo
	setfield elmpath "/kinetics"
	create xlabel Uplink -wgeom 50% -bg cyan
	create xlabel Downlink [0:last,0:NULL,50%,] -bg cyan
	create xlabel uplink_name -wgeom 50%
	create xlabel downlink_name [0:last,0:Downlink,50%,]
	create xcoredraw udraw [0,0:uplink_name,50%,200]
	create xtree udraw/tree -treemode geometry -hlmode none \
		-script "edit_elm.D <v>; drag_from_link.w <s> <d> <S> <x> <y>"
	create xcoredraw ddraw [0:udraw,0:uplink_name,50%,200]
	create xtree ddraw/tree -treemode geometry -hlmode none \
		-script "edit_elm.D <v>; drag_from_link.w <s> <d> <S> <x> <y>"
	str name
	foreach name ({el /edit/draw/tree/shape[]})
		if ({getfield {name} index} > 0)
			copy {name} udraw/tree
			copy {name} ddraw/tree
		end
	end

	create xbutton savenotes -label NOTES [0,0:last,10%,130] \
		-script "save_link_notes <w>" -offbg gray
	create xtext notes [0:last,0:ddraw,90%,130] -editable 1
	create xbutton UPDATE [0%,0:notes,50%,30] \
		-script do_update_linkinfo
	create xbutton HIDE [50%,0:notes,50%,30] \
		-script "save_link_notes <w>; xhide /parmedit/linkinfo"
	ce /
end

/* Stuff for linkinfo visibility
** This may become generic stuff */

function edit_linkinfo(linkinfo)
	str linkinfo

	setfield /parmedit/linkinfo elmpath {linkinfo}
	do_update_linkinfo
	xshowontop /parmedit/linkinfo
end

/******************************************************************
      Stuff for graphing linkinfos
******************************************************************/

// This could be interesting. We might wish to have a field in the
// linkinfo editor which allows one to specify which of the linkinfoies
// gets displayed. For now, display the faithful old x coord

/******************************************************************
      Stuff for initializing linkinfos
******************************************************************/

function xlinkinfoproto
	ce /control/lib/tree
	create xshape shape -autoindex \
		-drawmode DrawLines \
		-fg yellow \
		-value "linkinfo" \
		-text "linkinfo" \
		-textmode fill \
		-textcolor blue \
		-pixflags v \
		-pixflags c \
		-script \
		"edit_linkinfo.D"
	
	copy {el ^} /edit/draw/tree -autoindex
	setfield ^ script "edit_linkinfo.D"
	ce /
end

function klinkinfoproto
	create neutral /linkinfo
//	create text /linkinfo/notes
    // addfield /linkinfo notes -description "string for linkinfo notes and refs"
    addfield /linkinfo editfunc -description "func for linkinfo edit"
    addfield /linkinfo xtree_fg_req -description "color for linkinfo icon"
    addfield /linkinfo xtree_textfg_req -description "text color for linkinfo icon"
    addfield /linkinfo plotfield -description "field to plot"
    addfield /linkinfo manageclass -description "Class that can manage it"
	addfield /linkinfo uplink -description "Path to uplink"
	addfield /linkinfo downlink -description "Path to downlink"
	addmsgdef /linkinfo LINK

	addobject linkinfo /linkinfo \
        -author "Upi Bhalla NCBS March 2001" \
        -description "linkinfo plus a notes field"
    setdefault linkinfo editfunc "edit_linkinfo"
    setdefault linkinfo xtree_fg_req "red"
    setdefault linkinfo xtree_textfg_req "blue"
    setdefault linkinfo plotfield "x"
    setdefault linkinfo manageclass "uplink"
    setdefault linkinfo uplink ""
    setdefault linkinfo downlink ""
    ce /
end


function init_xlinkinfo
    // set up the prototype linkinfo
    klinkinfoproto

	if (DO_X)
    	xlinkinfoproto

		// set up the calls to handle dragging things into a linkinfo
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[]" \
			"/kinetics/##[TYPE=linkinfo]" all all none -1 0 \
	"echo.p dragging <S> onto linkinfo <D>; thing_to_linkinfo_add.p <S> <D>" \
			"" ""
	
		// make linkinfo editor
		make_xedit_linkinfo
	end
	simobjdump linkinfo xtree_fg_req xtree_textfg_req \
	  uplink downlink \
	  x y z

end
/******************************************************************/
