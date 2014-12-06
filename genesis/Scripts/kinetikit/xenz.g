//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-1997 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

/******************************************************************
      Stuff for connecting enzs
******************************************************************/
function calc_mm(enz)
// dummy
end

// called from the enz when a proto of it is dropped onto a pool
// Must be called immediately after the enz has been created
function enzproto_to_pool_link(enz,pool,x,y,z)
	str enz,pool
	float x,y,z

		// call /edit/draw/tree MOVECUSTOM {enz} {x} {y + 1} {z} 1
		position {enz} {x} {y + 1} {z}
		setfield {enz} xtree_textfg_req {getfield {pool} xtree_fg_req}
		setfield {enz} vol {getfield {pool} vol}

		// numerically attach the enz to the pool
		addmsg {pool} {enz} ENZYME n
		addmsg {enz} {pool} REAC eA B
end

// Attaches the pool to the enz as a substrate
function pool_to_enz_add(pool,enz)
	str pool,enz
	int is_autocat = {getfield /simcontrol/autocat state}

	if ( (is_autocat == 0 ) && ( {getmsg {enz} -incoming -find {pool} ENZYME} >= 0 ) )
		do_inform "Error: Attempting to make '"{pool}"' a substrate of itself"
		return
	end

	if ({getfield /simcontrol/enzintramol state} == 1)
		addmsg {pool} {enz} INTRAMOL n
		setfield /simcontrol/enzintramol state 0
	else
		addmsg {enz} {pool} REAC sA B
		addmsg {pool} {enz} SUBSTRATE n
	end
	call /edit/draw/tree RESET
	calc_mm {enz}
end

// Detaches the pool from the enz as a substrate
function pool_to_enz_drop(pool,enz)
	str pool,enz
// Check if high_order reactions are allowed
	if ({getfield /simcontrol/high_order state} == 1)
		pool_to_enz_add {pool} {enz}
		setfield /simcontrol/high_order state 0
		return
	end
	echo in pool_to_enz_drop with {enz} {pool}
	deletemsg {pool} 0 -find {enz} REAC
	deletemsg {enz} 0 -find {pool} SUBSTRATE
	call /edit/draw/tree RESET
	calc_mm {enz}
end

// Attaches the pool to the enz as a product
function enz_to_pool_add(enz,pool)
	str enz,pool
	int is_autocat = {getfield /simcontrol/autocat state}

	if ({exists {enz}} == 0)
		echo bailing out, src is a mirror
		return
	end

	if (( is_autocat == 0 ) && ( {getmsg {enz} -incoming -find {pool} ENZYME} >= 0) )
		do_inform "Error: Attempting to make '"{pool}"' a product of its own enzyme activity"
		return
	end


// Check if CONSVS are enabled
	if ({getfield /simcontrol/enzconsv state} == 1)
		addmsg {enz} {pool} CONSERVE nComplex nComplexInit
		setfield /simcontrol/enzconsv state 0
		call /edit/draw/tree RESET
		return
	end
// Check if SUMTOTALS are enabled
	if ({getfield /simcontrol/enzsumtot state} == 1)
		addmsg {enz} {pool} SUMTOTAL nComplex nComplexInit
		setfield /simcontrol/enzconsv state 0
		call /edit/draw/tree RESET
		return
	end
	echo in enz_to_pool_add with {enz} {pool}
	addmsg {enz} {pool} MM_PRD pA
	call /edit/draw/tree RESET
end

// Detaches the pool from the enz as a product
function enz_to_pool_drop(enz,pool)
	str enz,pool
	if ({exists {enz}} == 0)
		echo bailing out, src is a mirror
		return
	end
// Check if high_order reactions are allowed
	if ({getfield /simcontrol/high_order state} == 1)
		enz_to_pool_add {enz} {pool}
		setfield /simcontrol/high_order state 0
		call /edit/draw/tree RESET
		return
	end
// Check if CONSVS are enabled
	if ({getfield /simcontrol/enzconsv state} == 1)
		addmsg {enz} {pool} CONSERVE nComplex nComplexInit
		setfield /simcontrol/enzconsv state 0
		call /edit/draw/tree RESET
		return
	end
// Check if SUMTOTS are enabled
	if ({getfield /simcontrol/enzsumtot state} == 1)
		addmsg {enz} {pool} SUMTOTAL nComplex nComplexInit
		setfield /simcontrol/enzconsv state 0
		call /edit/draw/tree RESET
		return
	end
	echo in enz_to_pool_drop with {enz} {pool}
	deletemsg {pool} 0 -find {enz} MM_PRD
	call /edit/draw/tree RESET
end

function find_enz_pa(enz)
	str enz

	int n
	int i
	str pa = "/kinetics"

	n = {getmsg {enz} -in -count}
	for (i = 0 ; i < n; i = i + 1)
		if ({strcmp {getmsg {enz} -in -type {i}} "ENZYME"} == 0)
			pa = {getmsg {enz} -in -source {i}}
			return {pa}
		end
	end
	return {pa}
end

// Fixes problem cases where an enzyme pool is connected to its 
// enz activity by the REAC sA B message rather than REAC eA B.

function fix_enz_parent_bug
	str enz
	str pa
	int i
	float j
	int num_bad = 0

	reset
	foreach enz ({el /kinetics/##[][TYPE=kenz]})
		pa = {find_enz_pa {enz}}
		setfield {enz} sA 1 pA 2 eA 3
		// echo {pa}, {enz}
	end
	foreach enz ({el /kinetics/##[][TYPE=kenz]})
		pa = {find_enz_pa {enz}}
		i = {getmsg {pa} -incoming -find {enz} REAC}
		if (i >= 0)
			// echo {getmsg {pa} -incoming -slot {i} 0}
			setfield {enz} sA 1 pA 2 eA 3
			// showmsg {pa}
			j = {getmsg {pa} -incoming -slot {i} 0}
			// echo j is {j}, {pa}, {i}
			if ( j < 2.5 ) // avoid equality comparisons on floats
				// showmsg {pa}
				if (FIX_ENZ_PARENT_BUG)
					echo WARNING: enz_parent bug: fixing up {pa} to {enz}
					echo Set FIX_ENZ_PARENT_BUG in PARMS.g to change behaviour.
					deletemsg {pa} {i} -incoming
					addmsg {enz} {enz}/.. REAC eA B
				else 
					echo WARNING: enz_parent bug: detected but did not fix {pa} to {enz}
					echo Set FIX_ENZ_PARENT_BUG in PARMS.g to change behaviour.
				end
				num_bad = num_bad + 1
			end
		end
	end
	if (num_bad > 0)
		if (FIX_ENZ_PARENT_BUG)
			do_inform "Warning: "{num_bad}" substrate messages found between enzyme and parent, fixed"
		else
			do_inform "Warning: "{num_bad}" substrate messages found between enzyme and parent, not fixed. "
		end
	end
end

/******************************************************************
      Stuff for editing enz
******************************************************************/

function update_all_enz_vol
	str name
	float vol
	float scale = {getfield /editcontrol/conc scalefactor}

	foreach name ({el /kinetics/##[TYPE=kenz]})
		setfield {name} vol {getfield {name}/.. vol}
	end
end

function find_enz_order(enz)
	str enz

	int order = 0
	int i
	int nmsg = {getmsg {enz} -incoming -count}
	str type

	for (i = 0 ; i < nmsg; i = i + 1)
		type = {getmsg {enz} -incoming -type {i}}
		if ({strcmp {type} "SUBSTRATE"} == 0)
			order = order + 1
		end
		if ({strcmp {type} "INTRAMOL"} == 0)
			order = order - 1
		end
	end
	// echo order = {order}
	return {order}
end

function find_enz_vol(enz)
	str  enz
	int n
	int i

	n = {getmsg {enz} -in -count}
	for (i = 0 ; i < n; i = i + 1)
		if ({strcmp {getmsg {enz} -in -type {i}} "ENZYME"} == 0)
			str pa = {getmsg {enz} -in -source {i}}
			return {getfield {pa} vol}
		end
	end

	echo failed to find enzyme parent.
	return 1
end

function rescale_enz_vol(scale)
	float scale

	int enz_order
	str name
	foreach name ({el /kinetics/##[TYPE=kenz]})
		enz_order = {find_enz_order {name}}
		setfield {name} \
			k1 {{getfield {name} k1} * {pow {scale} {-enz_order}}} \
			vol { {getfield {name} vol} * {scale} } \
			nComplex { {getfield {name} nComplex} * {scale} } \
			nComplexInit { {getfield {name} nComplexInit} * {scale} }
	end
	do_update_enzinfo
end


// Handles messy conversion to specified units of conc and time
function calc_mm(enz)
	str enz

	float scale = {getfield /editcontrol/conc scalefactor}

	float vol = {find_enz_vol {enz}}
	float realvol = scale * vol / NA
	int order = {find_enz_order {enz}}

	float k1 = {getfield {enz} k1}
    float k2 = {getfield {enz} k2}
    float k3 = {getfield {enz} k3}

	if (order == 0)
		setfield /parmedit/enz/Km value {(k3 + k2)/k1}
		setfield /parmedit/enz/uKm label "No units/intramol"
	end
	if (order == 1)
		setfield /parmedit/enz/Km value {((k3 + k2)/k1) / vol}
		setfield /parmedit/enz/uKm label {getfield /editcontrol/conc unitname}
	end
	if (order == 2)
		setfield /parmedit/enz/Km value {((k3 + k2)/k1) / (vol * vol)}
		setfield /parmedit/enz/uKm label {getfield /editcontrol/conc unitname}
	end

	// setfield /parmedit/enz/Km value {(k3 + k2)/k1}
	setfield /parmedit/enz/Vmax value \
		{k3 * {getfield /editcontrol/time scalefactor}}
	setfield /parmedit/enz/uVmax label 1/{getfield /editcontrol/time unitname}
	setfield /parmedit/enz/ratio value {k2 / k3}
end

function set_mm
	str enz = {getfield /parmedit/enz elmpath}
	int order = {find_enz_order {enz}}
	float volconv = {find_enz_vol {enz}}

	float vmax = {getfield /parmedit/enz/Vmax value}
	float km = {getfield /parmedit/enz/Km value}
	float ratio = {getfield /parmedit/enz/ratio value}
	float k3 = vmax / {getfield /editcontrol/time scalefactor}

	setfield {enz} k3 {k3}
	setfield {enz} k2 {k3 * ratio}
	setfield {enz} k1 {k3 * (1.0 + ratio) / (km * {pow {volconv} {order}})}

    setfield /parmedit/enz/k1 value {getfield {enz} k1}
    setfield /parmedit/enz/k2 value {getfield {enz} k2}
    setfield /parmedit/enz/k3 value {getfield {enz} k3}
end

function do_update_enzinfo
	if (!{exists /parmedit/enz})
		return
	end
	str enz = {getfield /parmedit/enz elmpath}
	if ({strlen {enz}} == 0)
		return
	end
	if (!{exists {enz}})
		return
	end

	// echo in do_update_enzinfo
	float scale = {getfield /editcontrol/conc scalefactor}

	float vol = {find_enz_vol {enz}}
	float realvol = scale * vol / NA

	do_update_generic /parmedit/enz

    setfield /parmedit/enz/vol label "Volume = "{realvol}" m^3"
    setfield /parmedit/enz/dia label \
		"Diameter = "{calc_dia {realvol}}" um"
    setfield /parmedit/enz/usecomplex state {getfield {enz} usecomplex}
    setfield /parmedit/enz/nComplex value {getfield {enz} nComplex }
    setfield /parmedit/enz/nComplexInit value {getfield {enz} nComplexInit}
    setfield /parmedit/enz/CoComplex value {getfield {enz} CoComplex }
    setfield /parmedit/enz/CoComplexInit value {getfield {enz} CoComplexInit}
    setfield /parmedit/enz/k1 value {getfield {enz} k1}
    setfield /parmedit/enz/k2 value {getfield {enz} k2}
    setfield /parmedit/enz/k3 value {getfield {enz} k3}
    setfield /parmedit/enz/stoch_flag state {getfield {enz} stoch_flag}

	calc_mm {enz}
end

function set_usecomplex(w,v)
	str w
	int v
	str enz = {getfield {w}/.. elmpath}
	str epa = {find_enz_pa {enz}}
	if (v)
		/* We need to get rid of the enz-to-pool REAC msg and set the 
		** usecomplex flag to 1 */
		deletemsg {epa} 0 -find {enz} REAC
		setfield {enz} usecomplex 1
	else
		// We need to add the enz-to-pool REAC msg and set usecomplex 0
		// Check if by some mischance the message already exists
		// This func returns -1 if it fails to find the msg
		if ({getmsg {epa} -find {enz} REAC} == -1)
			addmsg {enz} {epa} REAC eA B
		end
		setfield {enz} usecomplex 0
	end
end

function make_xedit_enz
	create xform /parmedit/enz [{EX},{EY},{EW},480]
	addfield /parmedit/enz elmpath \
		-description "path of elm being edited"
	ce /parmedit/enz
	create xdialog path -wgeom 60% -title "Parent"
	create xdialog name -xgeom 60% -ygeom 0 -wgeom 40% -title "Name" \
		-script "set_item_name <w> <v>"
	create xlabel Rates [0,0:name,30%,] -bg cyan
	create xlabel mm [0:last,0:name,40%,] \
		-label "Michaelis-Menten consts" -bg cyan
	create xlabel units [0:last,0:name,30%,] \
		-label "MM Units" -bg cyan
	create xdialog k1 [0,0:Rates,30%,30] -script "set_field <w> <v>"
	create xdialog k2 [0,0:last,30%,30] -script "set_field <w> <v>"
	create xdialog k3 [0,0:last,30%,30] -script "set_field <w> <v>"
	create xdialog Km [30%,0:mm,40%,30] -script "set_mm"
	create xdialog ratio [30%,0:last,40%,30] -script "set_mm"
	create xdialog Vmax [30%,0:last,40%,30] -label kcat -script "set_mm"
	create xlabel uKm [70%,0:mm,30%,30] -label "uM"
	create xlabel uratio [70%,0:last,30%,30] -label "dimensionless"
	create xlabel uVmax [70%,0:last,30%,30] -label "1/sec"

	create xlabel Settings [0,0:last,100%,] -bg cyan
	/*
	create xdialog vol [0%,0:Settings,50%,30] \
		-script "set_field <w> <v>; do_update_enzinfo"
	*/
	create xlabel vol [0%,0:Settings,50%,]
	create xlabel dia [0:last,0:Settings,50%,]
	create xtoggle stoch_flag -script "set_field <w> <v>" \
		-onlabel "Stochastic mode ON" -offlabel "Stochastic mode OFF"
	// create xdialog Units [50%,0:Settings,50%,30]
	/*
	create xdialog color [0%,0:vol,50%,30] \
		-script "set_item_color <w> <v>"
	make_colorbar /parmedit/enz dia
	*/

	create xlabel Complex [0,0:last,100%,] -bg cyan
	create xtoggle usecomplex \
		-script "set_usecomplex <w> <v>" \
		-offlabel "Explicit Enz complex" \
		-onlabel "Classical Michaelis Menten"
	create xdialog nComplex [0%,0:last,50%,30] \
		-script "set_field <w> <v>; do_update_enzinfo"
	create xdialog nComplexInit [50%,0:usecomplex,50%,30] \
		-script "set_field <w> <v>; do_update_enzinfo"
	create xdialog CoComplex [0%,0:nComplex,50%,30] \
		-script "set_field <w> <v>; do_update_enzinfo"
	create xdialog CoComplexInit [50%,0:nComplex,50%,30] \
		-script "set_field <w> <v>; do_update_enzinfo"
	create xbutton savenotes -label NOTES [0,0:last,10%,130] \
		-script "save_item_notes <w>" -offbg gray
	create xtext notes [0:last,0:CoComplex,90%,130] -editable 1
	create xbutton UPDATE [0%,0:notes,50%,30] \
		-script "do_update_enzinfo"
	create xbutton HIDE [50%,0:notes,50%,30] \
		-script "save_item_notes <w>; xhide /parmedit/enz"
	ce /
end

function edit_enz(reac)
	str reac

	setfield /parmedit/enz elmpath {reac}
	do_update_enzinfo
	xshowontop /parmedit/enz
end

/******************************************************************
      Stuff for initializing enzs
******************************************************************/

function kenzproto
	create enz /enz
		setfield /enz \
			nComplex 0 \
			nComplexInit 0 \
			k1 0.1	\
			k2 0.4	\
			k3 0.1	\
			vol {DEFAULT_VOL_CONVERSION}

	create text /enz/notes
	addfield /enz notes
    addfield /enz editfunc -description "func for enz edit"
    addfield /enz xtree_fg_req -description "color for enz icon"
    addfield /enz xtree_textfg_req -description "textcolor for enz icon"
    addfield /enz plotfield -description "field to plot"
    addfield /enz manageclass -description "Class that can manage it"
	addfield /enz link_to_manage -descript "func to link enz to manager"
	addfield /enz link -descript "func to link enz to manager"
	addfield /enz linkparms -description "Fields used for link parms"
 
    addobject kenz /enz \
        -author "Upi Bhalla Mt Sinai Apr 1994" \
        -description "enz plus a notes field"
    setdefault kenz editfunc "edit_enz"
    setdefault kenz xtree_fg_req "red"
    setdefault kenz xtree_textfg_req "white"
    setdefault kenz plotfield "CoComplex"
    setdefault kenz manageclass "kpool"
    setdefault kenz link_to_manage "enzproto_to_pool_link"
    setdefault kenz link ""
    setdefault kenz linkparms "k1 k2 k3"
end

function xenzproto
	ce /control/lib/tree
	create xshape shape -autoindex \
		-fg black \
		-text "--MM-->" -textfont 7x13bold \
		-textmode fill -textcolor white \
		-value "kenz" \
		-pixflags v \
		-pixflags c \
		-script \
		"edit_enz.D"

	copy {el ^}  /edit/draw/tree -autoindex
	setfield ^ \
		script "edit_enz.D"

    ce /
end


function init_xenz
    // set up the prototype
	kenzproto
	if (DO_X)
	    xenzproto
	
		// Set up the calls used to handle dragging enz to pool
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=kenz]" \
			"/kinetics/##[TYPE=kpool]" MM_PRD kpool red -1 0 \
			"echo.p dragging <S> to <D> for enz product" \
			"enz_to_pool_add.p <S> <D>" \
			"enz_to_pool_drop.p <S> <D>"
	
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=kpool]" \
			"/kinetics/##[TYPE=kenz]" SUBSTRATE kenz red -1 0 \
			"echo.p dragging <S> to <D> for enz substrate" \
			"pool_to_enz_add.p <S> <D>" \
			"pool_to_enz_drop.p <S> <D>"
	
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=kenz]" \
			"/kinetics/##[TYPE=kpool]" SUMTOTAL kpool darkgreen -1 0 \
			"" "" ""
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=kenz]" \
			"/kinetics/##[TYPE=kpool]" CONSERVE kpool blue -1 0 \
			"" "" ""

		// This one is for setting up intramol reac'ns
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=kpool]" \
			"/kinetics/##[TYPE=kenz]" INTRAMOL kenz white -1 0 \
			"" "" ""
	
	    // make the editor for enzs
	    make_xedit_enz
	end
	simobjdump kenz  CoComplexInit CoComplex nComplexInit nComplex vol \
		k1 k2 k3 keepconc usecomplex notes xtree_fg_req \
		xtree_textfg_req link  x y z
end

/******************************************************************/
