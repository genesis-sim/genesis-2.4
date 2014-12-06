//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-1997 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

/******************************************************************
	Stuff for drag-drop operations involving axes
******************************************************************/


/******************************************************************
	function for editing axes
******************************************************************/

function do_update_axisinfo
	str axis = {getfield /parmedit/axis elmpath}

	setfield /parmedit/axis/path value {getpath {axis} -head}
    setfield /parmedit/axis/name value {getpath {axis} -tail}
    setfield /parmedit/axis/xmin value {getfield {axis} xmin}
    setfield /parmedit/axis/xmax value {getfield {axis} xmax}
    setfield /parmedit/axis/ymin value {getfield {axis} ymin}
    setfield /parmedit/axis/ymax value {getfield {axis} ymax}
end

function make_xedit_axis
	create xform /parmedit/axis [{EX},{EY},{EW},245]
	addfield /parmedit/axis elmpath \
		-description "path of axis being edited"
	ce /parmedit/axis
    create xdialog path -wgeom 60% -title "Parent"
    create xdialog name -xgeom 60% -ygeom 0 -wgeom 40% -title "Name"
    create xdialog xmin [0%,0:name,50%,30] \
        -script "set_field <w> <v>; update_axis"
    create xdialog xmax [50%,0:name,50%,30] \
        -script "set_field <w> <v>; update_axis"

    create xdialog ymin [0%,0:xmin,50%,30] \
        -script "set_field <w> <v>; update_axis"
    create xdialog ymax [50%,0:xmin,50%,30] \
        -script "set_field <w> <v>; update_axis"

    create xbutton UPDATE [0%,0:ymin,50%,30] \
        -script "do_update_axisinfo"
    create xbutton HIDE [50%,0:ymin,50%,30] \
        -script "xhide /parmedit/axis"
end

function update_axis
	/*
	str axis = {getfield /parmedit/axis elmpath}

	xupdate {axis}/..
	*/
end

function edit_axis(axis)
	str axis
	echo editing {axis}
	setfield /parmedit/axis elmpath {el {axis}/..}
	do_update_axisinfo
	xshowontop /parmedit/axis
end

/******************************************************************
	function for actually making the graphs
******************************************************************/
// See the xgraphs.g file
