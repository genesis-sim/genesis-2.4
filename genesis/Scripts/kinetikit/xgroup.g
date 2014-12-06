//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-1997 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

/******************************************************************
      Stuff for connecting groups
******************************************************************/

function save_sim // Dummy function defined in xsave.g
end

// note that libs cannot be dragged onto groups yet.

function ungroup(thing)
	str thing

	str nm = {getpath {el {thing} } -tail}
	str newicon

	if ({exists /kinetics/{nm}})
		do_warning 0 "Warning: cannot ungroup "{thing}": name conflict"
		return(0)
	end
	setfield {thing} xtree_textfg_req {getfield /kinetics xtree_fg_req}
	move {thing} /kinetics/{nm}
	call /edit/draw/tree RESET
	return(1)
end

// This is called by the group when something is dropped onto it
function thing_to_group_add(thing,group)
	str thing,group

	str srcpa, newicon

	// Check that the thing can be put onto a group
	if ({strcmp {getfield {thing} "manageclass"} "group"} != 0)
		do_warning 0 "Warning: Element '"{src}"' cannot be moved onto a group"
		return
	end

	// Check that the src is not already grouped to self
	if ({strcmp {group} {el {thing}/.. }} == 0)
		// we might want to do an ungroup here
		ungroup {thing}
		return 
	end

	// Check that the src isnt named the same as an existing elm
	if ({exists {group}/{getpath {thing} -tail }})
		do_warning 0 "cannot move "{thing}" onto existing element of same name"
		return
	end


	// Set the new textcolor of the thing
	setfield {thing} xtree_textfg_req {getfield {group} xtree_fg_req }

	// Move the thing into the group
	move {thing} {group}

	// update the display
	call /edit/draw/tree RESET

	// we need to update the edit info here 
end


/******************************************************************
      Stuff for cleaning out non-group stuff
******************************************************************/



// Returns 1 if the 'name' is in the group. Also 
// returns 1 if there are any messages from 'name' to 'group'
// To be conservative it checks both for incoming and outgoing msgs
function connects_to_group(name, group)
	str name
	str group

	int len = {strlen {group}}
	str src
	str dest
	int nmsg
	int i

	if ({strncmp {name} {group} {len}} == 0)
		return 1
	end

	nmsg = {getmsg {name} -incoming -count}

	for (i = 0 ; i < nmsg; i = i + 1)
		src = {getmsg {name} -incoming -source {i}}
		if ({strncmp {src} {group} {len}} == 0)
			return 1
		end
	end

	nmsg = {getmsg {name} -outgoing -count}

	for (i = 0 ; i < nmsg; i = i + 1)
		dest = {getmsg {name} -outgoing -destination {i}}
		if ({strncmp {dest} {group} {len}} == 0)
			return 1
		end
	end

	return 0
end

function is_occupied(name)
	str kids
	int i = 0
	foreach kids ({el {name}/##})
		i = i + 1
	end
	return {i}
end

function zap_stuff_not_connected_to_group
	str group = {getfield /parmedit/group elmpath}
	int len = {strlen {group}}

	str name

	if (len < 11) // strlen("/kinetics/x") = 11
		return
	end

	// Have to order the search to avoid deleting parents whose
	// children connect up.
	foreach name ({el /kinetics/##[][TYPE=kenz],/kinetics/##[][TYPE=kreac]})
		if ({connects_to_group {name} {group}} == 0)
			delete {name}
		end
	end

	foreach name ({el /kinetics/##[][TYPE=kpool]})
		if ({connects_to_group {name} {group}} == 0)
			if ({is_occupied {name}} == 0)
				delete {name}
			end
		end
	end

	foreach name ({el /kinetics/##[][TYPE=group]})
		if ({strncmp {name} {group} {len}} != 0)
			if ({is_occupied {name}} == 0)
				delete {name}
			end
		end
	end

	foreach name ({el /#graphs/##[][TYPE=xplot]})
		if ({getmsg {name} -incoming -count} < 1)
			delete {name}
		end
	end

	call /#graphs/# RESET
	call /edit/draw/tree RESET
end


/******************************************************************
      Stuff for editing group
******************************************************************/

function update_modfilename(mod)
	str mod

	str dirpath = {getfield {mod} savename}
	str modfilename
	int version = {getfield {mod} version}
	if ({findchar {dirpath} /} == 0)
		modfilename = {dirpath} @ "_" @ {version} @ ".g"
	else
		modfilename = {getfield /file/modpath value} @ "/" @ \
			{dirpath} @ "_" @ {version} @ ".g"
	end
	setfield /parmedit/group/save_group value {modfilename}
	setfield {mod} file {modfilename}
end

function set_mod_saves(state)
	int state

	str group = {getfield /parmedit/group elmpath}
	setfield {group} mod_save_flag {state}

	str col
	if (state)
		col = "black"
		setfield /parmedit/group/save_group label "File for module saves"
		update_modfilename {group}
	else
		col = "darkgray"
		setfield /parmedit/group/save_group label "File for saves" \
			value {getpath {group} -tail}.g
	end
	setfield /parmedit/group/savename fg {col}
	setfield /parmedit/group/version fg {col}
	setfield /parmedit/group/md5sum fg {col}
	setfield /parmedit/group/Redo offfg {col}
end

function do_update_groupinfo
	str group = {getfield /parmedit/group elmpath}
	do_update_generic /parmedit/group

	setfield /parmedit/group/expand state {getfield {group} expanded}
	setfield /parmedit/group/movekids state {getfield {group} movealone}
	setfield /parmedit/group/savename value {getfield {group} savename}
	setfield /parmedit/group/version label "Version # "{getfield {group} version}
	setfield /parmedit/group/md5sum label "md5sum: "{getfield {group} md5sum}
	setfield /parmedit/group/mod_ops state {getfield {group} mod_save_flag}
	set_mod_saves {getfield {group} mod_save_flag}
end

function make_xedit_group
	create xform /parmedit/group  [{EX},{EY},{EW},430]
	addfield /parmedit/group elmpath \
		-description "path of elm being edited"
	ce /parmedit/group
	setfield elmpath "/kinetics"
	create xdialog path -wgeom 60% -title "Parent"
	create xdialog name -xgeom 60% -wgeom 40% -ygeom 0 -title "Name" \
		-script "set_item_name <w> <v>; set_field /parmedit/group/savename <v>; do_update_groupinfo"
	create xtoggle expand
		setfield expand \
		onlabel "Contracted : click to expand" \
		offlabel "Expanded : click to contract" \
		script "toggle_group_visibility <v>"
	// Also put in a radio button toggle for showing only links
	create xtoggle movekids [0,0:expand,50%,]
		setfield movekids \
		onlabel "Move group alone" \
		offlabel "Move children with group" \
		script "toggle_group_movekids <v>"
	create xbutton reinit [0:last,0:expand,50%,] \
		-label "Set nInit for child pools to n" \
		-script "do_group_reinit"

	create xbutton prune \
	 	-label "Delete all objects not connected to group" \
		-script zap_stuff_not_connected_to_group
	// create xdialog color -script "set_item_color <w> <v>"
	create xtoggle mod_ops \
		-onlabel "Module saves: ON" -offlabel "Module saves: OFF" \
		-onbg cyan -offbg gray -script "set_mod_saves <v>"
	create xdialog savename -label "Name for module saves" \
		-script "set_field <w> <v>; do_update_groupinfo"
	create xlabel version [0,0:savename,30%,30]
	create xlabel md5sum [0:last,0:savename,60%,30]
	create xbutton Redo [0:last,0:savename,10%,30] \
		-script "update_md5sum; do_update_groupinfo"
	create xdialog save_group -script "do_save_group <v>" \
		-label "File for saves"

	make_colorbar /parmedit/group save_group
	create xbutton savenotes -label NOTES [0,0:last,10%,130] \
		-script "save_item_notes <w>" -offbg gray
	create xtext notes [0:last,0:Color,90%,130] -editable 1
	create xbutton UPDATE [0%,0:notes,50%,30] \
		-script do_update_groupinfo
	create xbutton HIDE [50%,0:notes,50%,30] \
		-script "save_item_notes <w>; xhide /parmedit/group"
	ce /
end

/* Stuff for group visibility
** This may become generic stuff */

function toggle_group_visibility(state)
	int state
	str group = {getfield /parmedit/group elmpath}

	call /edit/draw/tree TRUNCATE {group} {state}
	setfield {group} expanded {state}
end

function update_md5sum(mod)
	str mod = {getfield /parmedit/group elmpath}

	setfield {mod} md5sum {calc_md5sum {mod}}
end

function toggle_group_movekids(state)
	int state
	str group = {getfield /parmedit/group elmpath}

	setfield {group} movealone {state}
end

function edit_group(group)
	str group

	setfield /parmedit/group elmpath {group}
	do_update_groupinfo
	xshowontop /parmedit/group
end

function do_save_group(destfile)
	str destfile
	str group = {getfield /parmedit/group elmpath}
	// assume for now we do not have multiple nesting of groups
	// We save all the base-level elms on kinetics for now, so
	// as to easily manage interactions.

	update_md5sum {group}

	save_sim {destfile} {group}
end

function do_group_reinit
	str group = {getfield /parmedit/group elmpath}

	do_reinit {group}
end

/******************************************************************
      Stuff for graphing groups
******************************************************************/

// This could be interesting. We might wish to have a field in the
// group editor which allows one to specify which of the groupies
// gets displayed. For now, display the faithful old x coord

/******************************************************************
      Stuff for initializing groups
******************************************************************/

function xgroupproto
	ce /control/lib/tree
	create xshape shape -autoindex \
		-drawmode DrawLines \
		-fg yellow \
		-coords [0,2,0][-2,0,0][0,-2,0][2,0,0][0,2,0] \
		-linewidth 3 \
		-value "group" \
		-text "group" \
		-pixflags v \
		-pixflags c \
		-textcolor "black" \
		-script \
		"edit_group.D"
	
	copy {el ^} /edit/draw/tree -autoindex
	setfield ^ script "edit_group.D"
end

function kgroupproto
	create neutral /group
	create text /group/notes
	addfield /group notes
    addfield /group editfunc -description "func for group edit"
    addfield /group xtree_fg_req -description "color for group icon"
    addfield /group xtree_textfg_req -description "text color for group icon"
    addfield /group plotfield -description "field to plot"
    addfield /group manageclass -description "Class that can manage it"
    addfield /group expanded -description "Flag for expansion of group"
    addfield /group movealone -description "Flag moving group without kids"
	addfield /group link -description "Path to link tied to this module"
	addfield /group savename -description "Name to give module file"
	addfield /group file -description "Full path for module file"
	addfield /group version -description "Module version number (integer)"
	addfield /group md5sum -description "md5sum signature for module file"
	addfield /group mod_save_flag -description "Flag for saving group as a module"

	addobject group /group \
        -author "Upi Bhalla NCBS March 2001" \
        -description "group plus a notes field"
    setdefault group editfunc "edit_group"
    setdefault group xtree_fg_req "yellow"
    setdefault group xtree_textfg_req "black"
    setdefault group plotfield "x"
    setdefault group manageclass "group"
    setdefault group expanded 0
    setdefault group movealone 0
    setdefault group link ""
    setdefault group savename "defaultfile"
    setdefault group file "defaultfile.g"
    setdefault group version 0
    setdefault group md5sum 0
    setdefault group mod_save_flag 0
    ce /
end


function init_xgroup
    // set up the prototype group
    kgroupproto
	// change /kinetics to a group
	if ({exists /kinetics})
		delete /kinetics
	end
	create group /kinetics

	if (DO_X)
    	xgroupproto

		// set up the calls to handle dragging things into a group
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[]" \
			"/kinetics/##[TYPE=group]" all all none -1 0 \
	"echo.p dragging <S> onto group <D>; thing_to_group_add.p <S> <D>" \
			"" ""
	
		// make group editor
		make_xedit_group
	end
	simobjdump group xtree_fg_req xtree_textfg_req plotfield \
	  expanded movealone link savename file version md5sum \
	  mod_save_flag x y z

end

/******************************************************************/










