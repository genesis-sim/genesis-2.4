//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 2005 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

/******************************************************************
      Stuff for connecting groups
******************************************************************/

function save_sim // Dummy function defined in xsave.g
end

// This is called by the geom when something is dropped onto it
function mol_to_geom_add(mol,geom)
	str mol,geom

	// Check that the thing can be put onto a geom
	if (!{isa kpool {mol}})
		do_warning 0 "Warning: Attempt to put non-molecule '"{mol}"' onto a geometry."
		return
	end

	// Move the thing into the geom
	setfield {mol} geomname {geom}
	float vol = {getfield {geom} size}
	float scale = {getfield /editcontrol/conc scalefactor}
	setfield {mol} realvol {vol}
	setfield {mol} vol {NA * vol / scale}
	// Check if the mol edit window is currently displayed.
	if ({strcmp {getfield /parmedit/pool elmpath} {mol}} == 0)
		setfield /parmedit/pool/geomname value {geom}
		setfield /parmedit/pool/dia value {calc_dia {vol}}
		setfield /parmedit/pool/vol value {vol}
	end
end

/******************************************************************
      Stuff for editing geom
******************************************************************/

function do_set_dim(dim)
	int dim

	str geom = {getfield /parmedit/geometry elmpath}

	setfield /parmedit/geometry/dim# state 0
	setfield /parmedit/geometry/dim{dim} state 1
	if (dim == 0)
		setfield /parmedit/geometry/size label "Point size(?)"
	end
	if (dim == 1)
		setfield /parmedit/geometry/size label "Length"
	end
	if (dim == 2)
		setfield /parmedit/geometry/size label "Area"
	end
	if (dim == 3)
		setfield /parmedit/geometry/size label "Volume"
	end
end

function set_geom_size( size )
	float size
	str geomname = {getfield /parmedit/geometry elmpath}
	setfield /parmedit/geometry/size value {size}
	float scale = {getfield /editcontrol/conc scalefactor}
	float vol = NA * size / scale
	setfield {geomname} size {size}

	str name
	foreach name ( { el /kinetics/##[][TYPE=kpool] } )
		if ( { strcmp {getfield {name} geomname} {geomname} } == 0)
			// Later do this according to geometry dimensions
			setfield {name} vol { vol }
			setfield {name} realvol { size }
		end
	end
	do_update_poolinfo
end

function set_geom_name( w, newgeom )
	str w
	str newgeom

	str oldgeom = {getfield /parmedit/geometry elmpath}
	set_item_name {w} {newgeom}
	newgeom = {getfield /parmedit/geometry elmpath}


	// Scan all pools to see if they have the same geometry name as
	// the previous one. If so, replace name.
	str mol = {getfield /parmedit/pool elmpath}
	str molgeom = {getfield {mol} geomname}
	if ( {strcmp {molgeom} {oldgeom} } == 0 )
		setfield /parmedit/pool/geomname value {newgeom}
	end
	str name
	foreach name ( { el /kinetics/##[][TYPE=kpool] } )
		if ( { strcmp {getfield {name} geomname} {oldgeom} } == 0)
			setfield {name} geomname { newgeom }
		end
	end
end

function do_update_geometryinfo
	str geom = {getfield /parmedit/geometry elmpath}
	do_update_generic /parmedit/geometry

	do_set_dim {getfield {geom} dim}
	setfield /parmedit/geometry/size value {getfield {geom} size}

	setfield /parmedit/geometry/shape value {getfield {geom} shape}
	setfield /parmedit/geometry/outside value {getfield {geom} outside}
end

function make_xedit_geometry
	create xform /parmedit/geometry  [{EX},{EY},{EW},430]
	addfield /parmedit/geometry elmpath \
		-description "path of elm being edited"
	ce /parmedit/geometry
	setfield elmpath "/kinetics"
	create xdialog path -wgeom 60% -title "Parent"
	create xdialog name -xgeom 60% -wgeom 40% -ygeom 0 -title "Name" \
		-script "set_geom_name <w> <v>; do_update_geometryinfo"

	create xlabel dl -label "Number of dimensions:" -bg cyan
	create xtoggle dim0 [0,0:dl,25%,] -label 0 -onbg cyan\
		-script "do_set_dim 0"
	create xtoggle dim1 [0:last,0:dl,25%,] -label 1 -onbg cyan \
		-script "do_set_dim 1"
	create xtoggle dim2 [0:last,0:dl,25%,] -label 2 -onbg cyan \
		-script "do_set_dim 2"
	create xtoggle dim3 [0:last,0:dl,25%,] -label 3 -onbg cyan \
		-script "do_set_dim 3"
	create xdialog size -label "Volume" -script "set_geom_size <v>"
	create xdialog shape -label "Shape" -script "set_field <w> <v>"
	create xdialog outside -label "Outside" -script "set_field <w> <v>"

	make_colorbar /parmedit/geometry outside
	create xbutton savenotes -label NOTES [0,0:last,10%,130] \
		-script "save_item_notes <w>" -offbg gray
	create xtext notes [0:last,0:Color,90%,130] -editable 1
	create xbutton UPDATE [0%,0:notes,50%,30] \
		-script do_update_geometryinfo
	create xbutton HIDE [50%,0:notes,50%,30] \
		-script "save_item_notes <w>; xhide /parmedit/geometry"
	ce /
end

function edit_geometry(geometry)
	str geometry

	setfield /parmedit/geometry elmpath {geometry}
	do_update_geometryinfo
	xshowontop /parmedit/geometry
end

/******************************************************************
      Stuff for graphing geometrys
******************************************************************/
// Not yet implemented

/******************************************************************
      Stuff for initializing geometrys
******************************************************************/

function xgeometryproto
	ce /control/lib/tree
	create xshape shape -autoindex \
		-drawmode DrawLines \
		-fg yellow \
		-coords [1,0.8,0][-1,0.8,0][-1,-0.8,0][1,-0.8,0][1,0.8,0] \
		-linewidth 3 \
		-value "geometry" \
		-text "geometry" \
		-pixflags v \
		-pixflags c \
		-textcolor "black" \
		-script \
		"edit_geometry.D"
	
	copy {el ^} /edit/draw/tree -autoindex
	setfield ^ script "edit_geometry.D"
end

function kgeometryproto
	create neutral /geometry
	create text /geometry/notes
	addfield /geometry notes
    addfield /geometry editfunc -description "func for geometry edit"
    addfield /geometry xtree_fg_req -description "color for geometry icon"
    addfield /geometry xtree_textfg_req -description "text color for geometry icon"
    addfield /geometry plotfield -description "field to plot"
    addfield /geometry manageclass -description "Class that can manage it"
    addfield /geometry size -description "Length, Area, Volume"
    addfield /geometry scalesize -description "size/NA with SI scaling"
    addfield /geometry dim -description "# of dimensions of geom"
	addfield /geometry shape -description "Shape of geom"
	addfield /geometry outside -description "Compt outside this"

	addobject geometry /geometry \
        -author "Upi Bhalla NCBS March 2001" \
        -description "geometry plus a notes field"
    setdefault geometry editfunc "edit_geometry"
    setdefault geometry xtree_fg_req "white"
    setdefault geometry xtree_textfg_req "black"
    setdefault geometry plotfield "x"
    setdefault geometry manageclass "group"
    setdefault geometry size 1.666667e-21
    setdefault geometry scalesize 1
    setdefault geometry dim 3
    setdefault geometry shape "sphere"
    setdefault geometry outside ""
    ce /
end


function init_xgeometry
    // set up the prototype geometry
    kgeometryproto

	if (DO_X)
    	xgeometryproto

		// set up the calls to handle dragging things into a geometry
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[]" \
			"/kinetics/##[TYPE=geometry]" all all none -1 0 \
	"echo.p dragging <S> onto geometry <D>; mol_to_geom_add.p <S> <D>" \
			"" ""
	
		// make geometry editor
		make_xedit_geometry
	end
	simobjdump geometry \
	  size dim shape outside \
	  xtree_fg_req xtree_textfg_req x y z
end

/******************************************************************/
