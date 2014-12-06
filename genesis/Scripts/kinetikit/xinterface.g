//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-1997 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

/* By Upinder S. Bhalla, Mount Sinai School of Medicine */
// this file sets up the basic X interface for kkit

int auto_plot = 1
 
int EX = WINWID/2 - BORDER // x position for edit windows
int GH = WINHT/3 - BORDER  // height of graphs
int EY = GH + WMH // y position for edit windows
int EW = WINWID/2 - BORDER // width of edit windows
int TW = WINWID/2 - BORDER // Width of title bar.
int TH = 200 // height of title bar
int TB = TH + WMH + 2 * BORDER 
int EDW = WINWID/2 - BORDER // width of edit/draw window
int EDH = WINHT - TB - BORDER - WMH // height of edit/draw window

include xcontrol.g
include xcomparemodel.g
include xcopy.g
include xdoser.g
include xscalerates.g
include xdiffusion.g
include xselectmodel.g

function do_warning(is_cline,text)
	int is_cline
	str text
	if (is_cline)
		echo {text}
	else 
		setfield /warning/label label {text}
		xshowontop /warning
		xflushevents
	end
end

function edit_object(elm)
	str elm

	echo in edit_object {elm}
end

function drag_from_lib(dest,destval,srcval,x,y,z)
	str dest,destval,srcval
	float x,y,z

	str destclass
	str elm

	if ({strncmp {dest} "/edit/draw" 10} == 0)
		if ({strlen {destval}} == 0)
			destclass = "group"
			destval = "/kinetics"
		else
			destclass = {getfield {destval} object->name}
		end
		if ({strcmp {getdefault {srcval} manageclass} {destclass}} == 0)
			create {srcval} {destval}/{srcval} -autoindex
			elm = {el ^}
			call /edit/draw/tree RESET

			if ({exists {elm} "link_to_manage"})
				callfunc {getfield {elm} link_to_manage} \
					{elm} {destval} {snap {x}} {snap {y}} {snap {z}}
			else
				// call /edit/draw/tree MOVECUSTOM {elm} {x} {y} {z} 1
				position {elm} {snap {x}} {snap {y}} {snap {z}}
			end
			if ({exists {elm} "createfunc"})
				callfunc {getfield {elm} createfunc} {elm}
			end
		else
			do_warning 0 {{srcval} @ "s must be created on a " @ \
				{getdefault {srcval} manageclass}}
				return
		end
		call /edit/draw/tree RESET
		/* Set the clock for the new element */
		useclock {elm} {SIMCLOCK}
		/* Edit the new element */
		callfunc {getfield {elm} editfunc} {elm}
	end
end

function drag_from_edit(dest,srcval,x,y,z)
	str dest,srcval
	float x,y,z

	int movekids = 1

	if ({exists {srcval} movealone})
		movekids = 1 - {getfield {srcval} movealone}
	end
		
	if ({strcmp {dest} "/edit/draw"} == 0)
		if (movekids)
			position {srcval} {snap {x}} {snap {y}} {snap {z}}
		else 
			setfield {srcval} \
				x {snap {x}} \
				y {snap {y}} \
				z {snap {z}}
		end
		call /edit/draw/tree RESET
	end
end

function edit_elm(elm)
	callfunc {getfield {elm} editfunc} {elm}
end

function edit_obj(obj)
	echo in edit_obj with {obj}
end

function set_disp_range(path, target)
	str path
	str target

	// echo in set_disp_range path = {path}

	str name
	float x, y
	float xmin = 10000, ymin = 10000 
	float xmax = -10000, ymax = -10000
	
	foreach name ({el {path}})
		x = {getfield {name} x}
		y = {getfield {name} y}
		if (xmin > x)
			xmin = x
		end
		if (ymin > y)
			ymin = y
		end
		if (xmax < x)
			xmax = x
		end
		if (ymax < y)
			ymax = y
		end
	end
	if (xmin == 10000)
		xmin = -3
	end
	if (ymin == 10000)
		ymin = -3
	end
	if (xmax == -10000)
		xmax = 3
	end
	if (ymax == -10000)
		ymax = 3
	end

	setfield {target}/tree path {path}
	call {target}/tree RESET

	setfield {target} \
		xmin {xmin - 2} \
		xmax {xmax + 2} \
		ymin {ymin - 2} \
		ymax {ymax + 2}
end

function mod_path_str(start)
	str start

	str ret = ""
	str name
	foreach name ({el {start}/##[][TYPE=group]})
		if ({getfield {name} mod_save_flag} == 1)
			ret = ret @ "," @ {name}
			echo ret = {ret}
		end
	end
			echo ret = {ret}
	return {ret}
end

function set_disp_path
	int showmod = {getfield /edit/dispmodule state}
	int dozoom = {getfield /edit/zoom state}
	str	mod = {getfield /parmedit/group elmpath}
	str path

	if ({strcmp {mod} "/kinetics"} == 0)
		dozoom = 0
	end

	if (showmod)
		if (dozoom)
			path = {mod} @ "/##[TYPE=uplink]," @ \
				{mod} @ "/##[TYPE=downlink]," @ \
				{mod} @ "/#[TYPE=group]," @ {mod}
//				{mod} @ {mod_path_str {mod}}
		else
			path = "/kinetics/##[TYPE=uplink]," @ \
				"/kinetics/##[TYPE=downlink]," @ \
				"/kinetics/#[TYPE=group]"
//				{mod_path_str "/kinetics"}
		end
	else
		if (dozoom)
			// path = {mod} @ "/##[]," @ {mod}
			path = {mod} @ "/#[]," @ {mod} @ "/#[]/#[][TYPE!=proto]," @\
				{mod} @ "/#[]/#[][TYPE!=linkinfo]/##[]," @ {mod} @\
				",/kinetics/#[TYPE!=group],/kinetics/#/#[TYPE=kenz]"
		else
		//	path = "/kinetics/##[]"
			path = "/kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[]"
		end
	end

	set_disp_range {path} "/edit/draw"
end

function disp_model_only
	setfield /edit/draw/tree path \
"/kinetics/##[][TYPE=kpool],/kinetics/##[][TYPE=kenz],/kinetics/##[][TYPE=kreac],/kinetics/##[][TYPE=kchan],/kinetics/##[][TYPE=stim],/kinetics/##[][TYPE=xtab],/kinetics/##[][TYPE=transport],/kinetics/##[][TYPE=group],/kinetics/##[][TYPE=mirror],/kinetics/##[][ISA=uplink]"
	call /edit/draw/tree RESET
end

function set_fullpage(state)
	int state
		
	if (state)
		setfield /edit xgeom 1% ygeom 1% wgeom 98% hgeom 98%
		xshowontop /edit
	else
		setfield /edit xgeom 0 ygeom {TB} wgeom {EDW} hgeom {EDH}
	end
end

function set_snap_to_grid(state)
	int state
end

function set_disp_gif(state)
	int state
	if (!{exists /select})
		setfield /edit/dispgif state 0
		return
	end
	str mname = {getfield /select modelname}
	if (!{exists /edit/{mname}})
		setfield /edit/dispgif state 0
		return
	end
	setfield /edit/dispgif state {state}
	if (state)
		xshow /edit/{mname}
		xshow /edit/showdemoselect
		setfield /edit/dispmodule onfg grey offfg grey
		setfield /edit/zoom onfg grey offfg grey
		setfield /edit/fullpage onfg grey offfg grey
	else
		xhide /edit/{mname}
		xhide /edit/showdemoselect
		setfield /edit/dispmodule onfg black offfg black
		setfield /edit/zoom onfg black offfg black
		setfield /edit/fullpage onfg black offfg black
	end
end

function make_xedit
	/* Create the editor form */
    create xform /edit [0,{TB},{EDW},{EDH}]
	create xtoggle /edit/dispgif [0,0,25%,] \
		-script "set_disp_gif <v>" \
		-onlabel "Schematic" \
		-offlabel "Edit window" 
	create xtoggle /edit/dispmodule [0:last,0,25%,] \
		-script "set_disp_path" \
		-onlabel "Module view" \
		-offlabel "Reaction view" 
	create xtoggle /edit/zoom [0:last,0,25%,] \
		-script set_disp_path \
		-onlabel "Current module" \
		-offlabel "Entire model" 
	create xbutton /edit/showdemoselect [0:zoom,0,25%,] \
		-script "xshow /select" \
		-label "Select demo..."
	xhide /edit/showdemoselect
	create xtoggle /edit/fullpage [0:zoom,0,25%,] \
		-script "set_fullpage <v>" \
		-onlabel "Fullpage ON" \
		-offlabel "Fullpage OFF" 
	create xcoredraw /edit/draw [0,0:last,100%,0:NULL.bottom] \
		-xmin -5 -xmax 5 -ymin -5 -ymax 5 -fg black
	create xtree /edit/draw/tree
		setfield ^ \
			treemode geometry \
			path "/kinetics" \
			hlmode none \
		script "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>"

	if (FIXCOORDS == 1)
		setfield /edit/draw/tree treemode custom
	end

		/* get rid of the default settings */
		call ^ ADDMSGARROW "" "" "" "" none 0 0 "" "" ""
	disable /edit
//    xshow /edit
end

function make_xlibrary
	/* creating the library form */
	/* The lib is now located in /control */
	// create xform /lib [1,115,250,200]
	create xcoredraw /control/lib [0,0:runtime,100%,0:NULL.bottom] \
		-xmin -1 -xmax 12 -ymin -1 -ymax 1
	create xtree /control/lib/tree
// path "kpool kreac kenz kchan transport stim xtab group uplink downlink geometry mirror" \
	setfield /control/lib/tree \
		path "kpool kreac kenz kchan transport stim xtab group geometry" \
		treemode obj_grid \
		orientation s \
		sizescale 0.4 \
		script "drag_from_lib.w <d> <D> <S> <x> <y> <z>; edit_obj.D <v>"
	disable /control/lib
	// xshow /lib
end

function make_xclipboard
	create xform /clipboard [255,115,250,200]
	create xcoredraw /clipboard/draw [0,0,100%,100%] \
		-xmin -10 -xmax 10  -ymin -3 -ymax 1.5 \
		-fg black
	create xtree /clipboard/draw/tree
	setfield /clipboard/draw/tree \
		path "/kclip/##[]" \
		treemode custom \
		sizescale 0.4 \
		script "drag_from_clip.w <d> <S> <x> <y> <z>; edit_elmD <v>"
	disable /clipboard
end

function do_delete(src,elm)
	str src,elm
	if (({strncmp {src} "/edit/draw/" 11} == 0))
		// Start out by deleting any attached plots
		int nmsgs = {getmsg {elm} -outgoing -count}
		int i
		str msgtype
		for (i = 0 ; i < nmsgs; i = i + 1)
			msgtype = {getmsg {elm} -outgoing -type {i}}
			if ({strcmp {msgtype} PLOT} == 0)
				str dest = {getmsg {elm} -outgoing -destination {i}}
				str pa = {el {dest}/..}
				delete {getmsg {elm} -outgoing -destination {i}}
				xupdate {pa}
				nmsgs = nmsgs - 1
				i = i - 1
			end
		end
		delete  {elm}
		call /edit/draw/tree RESET
	end
end

function make_xedittools
// This also uses /control/lib as the parent draw widget
	create xgif /control/lib/del -filename del.gif \
		-tx 9.6 \
		-script "do_delete.p <s> <S>"
	create xgif /control/lib/dup -filename dup.gif \
		-tx 11.2 \
		-script "do_simple_copy.p <s> <S>"
end


function make_xwarning
    create xform /warning [100,400,700,100]
    create xlabel /warning/label -title "Warning" -hgeom 50%
    create xbutton /warning/done [40%,60,20%,30%] -label "OK" \
        -script "xhide /warning"
	disable /warning
end

function make_xinform
    create xform /inform [200,500,700,100]
    create xlabel /inform/label -title "hello" [0,0,100%,100%]
	create xbutton /inform/ok -title "OK" [40%,40%,20%,40%] \
    	 -script "xhide /inform"
	disable /inform
end

function do_inform(text)
	str text
	if (DO_X)
		setfield /inform/label label {text}
		xshowontop /inform
		xflushevents
	else
		echo {text}
	end
end

function set_field(widget,value)
	str widget,value

	str elm = {getfield {{widget} @ "/.."} elmpath}
	str field = {getfield {widget} name}
	setfield {elm} {field} {value}
end

// This function now screens names to eliminate dangerous chars
function set_item_name(widget,value)
	str widget,value

	str elm = {getfield {widget}/.. elmpath}
	str nm = {getpath {elm} -tail}
	str goodname = value
	str temp = ""
	while ({strcmp {temp} {goodname}} != 0)
		temp = goodname
		goodname = {strsub {goodname} " " "_"}
		goodname = {strsub {goodname} "'" "*"}
		//goodname = {strsub {goodname} "-" "_"}
		goodname = {strsub {goodname} "	" "_"}
		goodname = {strsub {goodname} "+" "_"}
		goodname = {strsub {goodname} "!" "_"}
		goodname = {strsub {goodname} "&" "_"}
		goodname = {strsub {goodname} "#" "_"}
		goodname = {strsub {goodname} "^" "_"}
		goodname = {strsub {goodname} "$" "_"}
		goodname = {strsub {goodname} "@" "_"}
		goodname = {strsub {goodname} "%" "_"}
		// The next term causes problems because it is interpreted
		// by SLI as a redirection to a file. 
		// goodname = {strsub {goodname} ">" "_"}
		goodname = {strsub {goodname} "<" "_"}
		goodname = {strsub {goodname} "?" "_"}
		goodname = {strsub {goodname} "," "."}
		goodname = {strsub {goodname} "~" "_"}
		goodname = {strsub {goodname} "=" "_"}
		goodname = {strsub {goodname} ":" "_"}
		goodname = {strsub {goodname} ";" "_"}
		goodname = {strsub {goodname} ")" "_"}
		goodname = {strsub {goodname} "(" "_"}
		goodname = {strsub {goodname} "{" "_"}
		goodname = {strsub {goodname} "}" "_"}
		goodname = {strsub {goodname} "/" "_"}
	end

	if ({strcmp {goodname} {value}} != 0)
		do_warning 0 {"Original name '" @ {value} @ "' has illegal chars. Renamed to '" @ {goodname}}
		setfield {widget} value {goodname}
	end

	if ({strcmp {nm} {goodname}} == 0)
		return
	end


	if ({exists {elm}/../{goodname}})
		do_warning 0 {"Rename failed. Element " @ {nm} @ \
			" already exists."}
		return
	end
	str pa = {el {elm}/..}
	move {elm} {elm}/../{goodname}
	callfunc {getfield {pa}/{goodname} editfunc} {pa}/{goodname}
	call /edit/draw/tree RESET
end

function save_item_notes(widget)
	str widget
	str elm = {getfield {widget}/.. elmpath}

	call {widget}/../notes PROCESS
	if ({exists {elm}/notes})
		call {elm}/notes FROM_XTEXT {widget}/../notes
	end
end

function do_update_generic(widget)
	str elm = {getfield {widget} elmpath}
	setfield {widget}/path value {getpath {elm} -head}
	setfield {widget}/name value {getpath {elm} -tail}
	if ({exists {widget}/Color})
		setfield {widget}/Color value {getfield {elm} xtree_fg_req}
	end
	call {elm}/notes TO_XTEXT {widget}/notes
end

function set_item_color(widget,color)
	str widget,color

	str elm = {getfield {{widget} @ "/.."} elmpath}
	str kids
	str manager
	str elmclass = {getfield {elm} object->name}

/* We need to change the plot colors here */

/* Changing the color of the elm */
	setfield {elm} xtree_fg_req {color}

/* Changing the textcolor of the elm kids */
	foreach kids ({el {elm}/#[]})
		manager = {getfield {kids} manageclass}
		if ({strcmp {manager} {elmclass}} == 0)
			setfield {kids} xtree_textfg_req {color}
		end
	end
	call /edit/draw/tree RESET
end

function set_colorbar_color(w, x)
	str w
	int x
	set_item_color {w} {x}
	setfield {w}/../Color value {x}
end

function make_colorbar(parent, above)
	str parent
	pushe {parent}
	create xdialog Color [0,0:{above},30%,30] \
		-script "set_item_color <w> <v>"
	create xcoredraw cdraw [0:last,0:{above},70%,30] \
		-xmin 0 -xmax 63 -ymin -0.5 -ymax 0.5
	create neutral scale
	createmap scale cdraw 64 1 -delta 1 1 -origin 0 0
	create xview cdraw/view -path {parent}/cdraw/scale[] -field x \
		-viewmode colorview -autoscale 1 -sizescale 1.2 \
		-script "set_colorbar_color "{parent}"/cdraw <x>"
	call cdraw/view RESET
	call cdraw/view PROCESS
	pope
end

// This function makes the form and the standard buttons 
//  for the edit popup panels
function make_edit_common(name,height)
    str name
    int height
    create xform /parmedit/{name} [{EX},{EY},{EW},{height}]
    ce ^
    create xlabel mylabel -label {name}" editor" -bg cyan
    addfield . elmpath -description "path of "{name} 
    create xdialog path [0,0:mylabel,60%,30] -title "Parent"
    create xdialog name [0:last,0:mylabel,40%,30] -title "Name" \
        -script "set_item_name <w> <v>"

    create xbutton savenotes -label NOTES [0,{height - 135},10%,100] \
        -script "save_item_notes <w>" -offbg gray 
    create xtext notes [10%,{height - 135},90%,90] -editable 1
    create xbutton UPDATE [0,0:notes,30%,30] \
        -script "do_update_"{name}"info"
    create xbutton DISMISS [0:last,0:notes,30%,30] \
        -script {"xhide " @ {el .}} 
    create xdialog color [0:last,0:notes,40%,30] \
        -script "set_item_color <w> <v>"
end


function xbegin
	/* Set the global flag for the interface to 1 */

	/* build the colorscale */
	xcolorscale rainbow2

	/* set up the postscript stuff */
	setpostscript -filename "display.ps" -inverse 0

	make_xedit
	// make_xclipboard
	create neutral /parmedit
	disable /parmedit
	make_xwarning
	make_xinform
	make_xcontrol
	make_xlibrary
	make_xfile
	//make_xgraphs
	make_opencontrol
	make_plotcontrol
	make_pscontrol
	make_comparecontrol
	make_copyform
	make_simcontrol
	make_simhelp
	make_xedittools
	make_editcontrol
	make_toolcontrol
	make_dosercontrol
	make_scalerate
	make_xdiffusion

	simobjdump xtree path script namemode sizescale -noDUMP
	simobjdump xcoredraw xmin xmax ymin ymax
	simobjdump xtext editable
end

function xmiddle
end

function xend
	xhide /about
	xshow /edit
	xshow /control
	xshow /graphs
	set_disp_path
	call /control/lib/tree RESET
	setfield /edit/draw/tree sizescale 0.6
	setfield /control/lib/tree sizescale 0.4
end
