//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-1997 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

/******************************************************************
      Stuff for connecting reacs
******************************************************************/
// reacs are created directly on groups, which is handled by the
// xdrag functions.

// These two functions are for the special situations where we
// add/remove rate constant controls to reacs.
function pool_to_reac_kf(pool,reac)
	str pool,reac
	if ({getmsg {reac} -find {pool} KF} == -1)
		addmsg {pool} {reac} KF Co
		setfield {reac} oldmsgmode 1
	else
		deletemsg {reac} 0 -find {pool} KF
		setfield {reac} oldmsgmode 0
	end
	call /edit/draw/tree RESET
end

function pool_to_reac_kb(pool,reac)
	str pool,reac
	if ({getmsg {reac} -find {pool} KB} == -1)
		addmsg {pool} {reac} KB Co
		setfield {reac} oldmsgmode 1
	else
		deletemsg {reac} 0 -find {pool} KB
		setfield {reac} oldmsgmode 0
	end
	call /edit/draw/tree RESET
end

// called from the reac when a pool is dropped on it
// Attaches the pool to the reac as a substrate
function pool_to_reac_add(pool,reac)
	str pool,reac
// think about this. ADDMSGARROW should call this case
	if ({getfield /simcontrol/kf state} == 1)
		pool_to_reac_kf {pool} {reac}
		setfield /simcontrol/kf state 0
		return
	end
	if ({getfield /simcontrol/kb state} == 1)
		pool_to_reac_kb {pool} {reac}
		setfield /simcontrol/kb state 0
		return
	end
	addmsg {reac} {pool} REAC A B
	addmsg {pool} {reac} SUBSTRATE n
	call /edit/draw/tree RESET
end


function pool_to_reac_drop(pool,reac)
	str pool,reac
// Check if high_order reactions are allowed
	if ({getfield /simcontrol/high_order state} == 1)
		pool_to_reac_add {pool} {reac}
		setfield /simcontrol/high_order state 0
		return
	end
	if ({getfield /simcontrol/kf state} == 1)
		pool_to_reac_kf {pool} {reac}
		setfield /simcontrol/kf state 0
		return
	end
	if ({getfield /simcontrol/kb state} == 1)
		pool_to_reac_kb {pool} {reac}
		setfield /simcontrol/kb state 0
		return
	end
	deletemsg {pool} 0 -find {reac} REAC
	deletemsg {reac} 0 -find {pool} SUBSTRATE
	call /edit/draw/tree RESET
end

// Attaches the reac to the pool as a substrate
function reac_to_pool_add(reac,pool)
	str reac,pool
// think about this. ADDMSGARROW should call this case
	addmsg {reac} {pool} REAC B A
	addmsg {pool} {reac} PRODUCT n
	call /edit/draw/tree RESET
end

function reac_to_pool_drop(reac,pool)
	str reac,pool
// Check if high_order reactions are allowed
	if ({getfield /simcontrol/high_order state} == 1)
		reac_to_pool_add {reac} {pool}
		setfield /simcontrol/high_order state 0
		return
	end
	deletemsg {reac} 0 -find {pool} PRODUCT
	deletemsg {pool} 0 -find {reac} REAC
	call /edit/draw/tree RESET
end


/******************************************************************
      Stuff for editing reac
******************************************************************/

// name could be either SUBSTRATE or PRODUCT
function find_reac_order(reac, name)
	str reac
	str name
	int i
	int order = 0
	int nmsg = {getmsg {reac} -incoming -count}
	str type

	for (i = 0; i < nmsg; i = i + 1)
		type = {getmsg {reac} -incoming -type {i}}
		if ({strcmp {type} {name}} == 0)
			order = order + 1
		end
	end

	return {order}
end

// returns the vol field of either a substrate or product pool,
// whichever it finds first. Note that the vol field is NOT true
// volume, it is NA * realvol / concscale
function find_reac_vol
	str reac = {getfield /parmedit/reac elmpath}
	if (!{exists {reac} kf})
		return 0
	end
	int nmsg = {getmsg {reac} -incoming -count}
	int i
	str type
	str pool = ""

	for (i = 0; i < nmsg; i = i + 1)
		type = {getmsg {reac} -incoming -type {i}}
		if ({strcmp {type} SUBSTRATE} == 0)
			pool = {getmsg {reac} -incoming -source {i}}
			break
		end
		if ({strcmp {type} PRODUCT} == 0)
			pool = {getmsg {reac} -incoming -source {i}}
			break
		end
	end
	if ({strlen {pool}} == 0)
		return 0
	else
		return {getfield {pool} vol}
	end
end

function set_rate_scale(ratename, rate, order)
	str ratename
	float rate
	int order

	float timescale = {getfield /editcontrol/time scalefactor}
	float concscale = {find_reac_vol}
	str timename = {getfield /editcontrol/time unitname}
	str concname = {getfield /editcontrol/conc unitname}
	// Really need to get hold of volscale here.

	str dname = "/parmedit/reac/s" @ ratename
	str uname = "/parmedit/reac/u" @ ratename
	str l1

	if ({strcmp {ratename} "kf"} == 0)
		l1 = "No substrate"
	else
		l1 = "No product"
	end
	
	if (order == 0)
		setfield {dname} value 0
		setfield {uname} label {l1}
	end
	if (order == 1)
		setfield {dname} value  {rate * timescale}
		setfield {uname} label "1/"{timename}
	end
	if (order == 2)
		setfield {dname} value {rate * timescale * concscale}
		setfield {uname} label "1/"{timename}"/"{concname}
	end
	if (order == 3)
		setfield {dname} \
			value {rate * timescale * concscale * concscale}
		setfield {uname} label "1/"{timename}"/"{concname}"^2"
	end
	if (order == 4) // This is the largest likely.
		setfield {dname} \
			value {rate * timescale * concscale * concscale * concscale}
		setfield {uname} label "1/"{timename}"/"{concname}"^3"
	end

end

function rescale_reac_vol(scale)
	float scale

	int sub_order
	int prd_order
	float kf
	float kb

	str name

	foreach name ({el /kinetics/##[TYPE=kreac]})
		sub_order = {find_reac_order {name} SUBSTRATE}
		prd_order = {find_reac_order {name} PRODUCT}
		kf = {getfield {name} kf}
		kb = {getfield {name} kb}
		kf = kf * {pow {scale} {1 - sub_order} }
		kb = kb * {pow {scale} {1 - prd_order} }
		setfield {name} kf {kf}
		setfield {name} kb {kb}
	end
	do_update_reacinfo
end

function do_update_reac_scaling
	pushe /parmedit/reac
	str reac = {getfield elmpath}
	str concname = {getfield /editcontrol/conc unitname}
	int sub_order = {find_reac_order {reac} SUBSTRATE}
	int prd_order = {find_reac_order {reac} PRODUCT}
	float kf = {getfield {reac} kf}
	float kb = {getfield {reac} kb}

	setfield forder label {sub_order - 1}
	setfield border label {prd_order - 1}

	set_rate_scale "kf" {kf} {sub_order}
	set_rate_scale "kb" {kb} {prd_order}

	float kd = {{getfield skb value} / {getfield skf value}}
	// setfield ukd label {concname}"^"{sub_order - prd_order}

	if (sub_order == prd_order)
		setfield Kd value {1.0/kd} label Keq
		setfield ukd label "Keq: no units"
	else
		setfield Kd value {pow {kd} {1.0/(sub_order - prd_order)}} \
			label Kd
		setfield ukd label {concname}
	end

	/*
	if ((sub_order == 2) && (prd_order == 1))
		setfield ukd label {concname}
	else
		if (sub_order == prd_order)
			setfield ukd label "Keq: no units"
		else
			setfield ukd label {concname}"^"{sub_order - prd_order}
		end
	end
	setfield Kd value {{getfield skb value} / {getfield skf value}}
	*/

	float timescale = {getfield /editcontrol/time scalefactor}
	str timename = {getfield /editcontrol/time unitname}
	setfield Tau value {1.0 / ((kf + kb) * timescale)}
	setfield utau label {getfield /editcontrol/time unitname}
	pope
end

function do_update_reacinfo
	if (!{exists /parmedit/reac})
		return
	end
	str reac = {getfield /parmedit/reac elmpath}
	if ({strlen {reac}} == 0)
		return
	end
	if (!{exists {reac}})
		return
	end
	// echo in do_update_reacinfo

	do_update_generic /parmedit/reac
    setfield /parmedit/reac/kf value {getfield {reac} kf}
    setfield /parmedit/reac/kb value {getfield {reac} kb}
    setfield /parmedit/reac/stoch_flag state {getfield {reac} stoch_flag}

	do_update_reac_scaling
end

function set_srate(ratename, v)
	str ratename
	float v

	float concscale = {find_reac_vol}
	float timescale = {getfield /editcontrol/time scalefactor}

	str reac = {getfield /parmedit/reac elmpath}
	int order

	if ({strcmp {ratename} "kf"} == 0)
		order = {find_reac_order {reac} SUBSTRATE}
	else
		order = {find_reac_order {reac} PRODUCT}
	end

	if ( (order == 0) || (order == 1) )
		setfield {reac} {ratename} {v / timescale}
	else
		setfield {reac} {ratename} {v / (timescale * {pow {concscale} {order - 1}})}
	end
	do_update_reacinfo
end

function set_kd(v)
	float v
end

function set_tau(v)
	float v
end

function make_xedit_reac
	create xform /parmedit/reac [{EX},{EY},{EW},420]
	addfield /parmedit/reac elmpath \
		-description "path of elm being edited"
	ce /parmedit/reac
	create xdialog path -wgeom 60% -title "Parent" 
	create xdialog name -xgeom 60% -ygeom 0 -wgeom 40% -title "Name" \
		-script "set_item_name <w> <v>"
	create xlabel Rates [0,0:name,30%,] -bg cyan \
		-label "Raw rates (#/sec)"
	create xlabel lorder [0:last,0:name,20%,] -bg cyan \
		-label "Reac Order"
	create xlabel srates [0:last,0:name,30%,] -bg cyan \
		-label "Scaled rates"
	create xlabel units [0:last,0:name,20%,] -bg cyan \
		-label "Units"
	create xdialog kf [0,0:Rates,30%,30] -script "set_field <w> <v>; do_update_reac_scaling"
	create xlabel forder [0:last,0:Rates,20%,30] -label "0"
	create xdialog skf [0:last,0:Rates,30%,30] \
		-label "Kf" -script "set_srate kf <v>"
	create xlabel ukf [0:last,0:Rates,20%,30] -label "No substrate"
	create xdialog kb [0,0:kf,30%,30] -script "set_field <w> <v>; do_update_reac_scaling"
	create xlabel border [0:last,0:kf,20%,30] -label "0"
	create xdialog skb [0:last,0:kf,30%,30] \
		-label "Kb" -script "set_srate kb <v>"
	create xlabel ukb [0:last,0:kf,20%,30] -label "No product"

	create xlabel lequiv [0,0:kb,70%,] \
		-label "Experimental constraints" -bg cyan
	create xlabel lequivunits [0:last,0:kb,30%,] \
		-label "Units" -bg cyan
	create xdialog Kd [0,0:lequiv,70%,30] -script "set_kd <v>"
	create xlabel ukd [0:last,0:lequiv,30%,30] -label "uM"
	create xdialog Tau [0,0:Kd,70%,30] -script "set_tau <v>"
	create xlabel utau [0:last,0:Kd,30%,30] -label "sec"
	create xtoggle stoch_flag -script "set_field <w> <v>" \
		-onlabel "Stochastic mode ON" -offlabel "Stochastic mode OFF"

	// create xdialog color -script "set_item_color <w> <v>"
	make_colorbar /parmedit/reac stoch_flag
	create xbutton savenotes -label NOTES [0,0:last,10%,130] \
		-script "save_item_notes <w>" -offbg gray
	create xtext notes [0:last,0:Color,90%,130] -editable 1
	create xbutton UPDATE [0%,0:notes,50%,30] \
		-script "do_update_reacinfo"
	create xbutton HIDE [50%,0:notes,50%,30] \
		-script "save_item_notes <w>; xhide /parmedit/reac"
	ce /
end

function edit_reac(reac)
	str reac

	setfield /parmedit/reac elmpath {reac}
	do_update_reacinfo
	xshowontop /parmedit/reac
end


/******************************************************************
      Stuff for graphing reacs
******************************************************************/

// might wish to graph all the substrates and prds here
function graph_reac(self,value,dest)
	str self,value,dest
	if ({strncmp {dest} "/graphs/" 8} == 0)
		do_warning 0 "Warning: Nothing to graph in reacs"
	end
end

/******************************************************************
      Stuff for initializing reacs
******************************************************************/
function kreacproto
	create reac /reac
	setfield /reac \
		kf 0.1	\
		kb 0.1
	create text /reac/notes
	addfield /reac notes
 	addfield /reac editfunc -description "name of editfunc for reac"
 	addfield /reac xtree_fg_req -description "color for reac icon"
 	addfield /reac xtree_textfg_req -description "text color for reac icon"
 	addfield /reac plotfield -description "field of reac to plot"
 	addfield /reac manageclass -description "Class to manage reacs"
	addfield /reac linkparms -description "Fields used for link parms"
	addobject kreac /reac \
		-author "Upi Bhalla Mt Sinai" \
		-description "reac plus a field for a description string"
	setdefault kreac editfunc "edit_reac"
	setdefault kreac xtree_fg_req "white"
	setdefault kreac xtree_textfg_req "black"
	setdefault kreac plotfield "A"
	setdefault kreac manageclass "group"
	setdefault kreac linkparms "kf kb"
end

function xreacproto
	ce /control/lib/tree
	create xshape shape -autoindex \
		-fg white \
		-drawmode DrawArrows \
		-coords "[-1,0.4,0][1,0.4,0][1,-0.4,0][-1,-0.4,0]" \
		-text "reac" \
		-textfont 6x10 \
		-textmode draw \
		-textcolor black \
		-linewidth 2 \
		-pixflags v \
		-pixflags c \
		-value "kreac" \
		-script "edit_lib.D"

	copy {el ^} /edit/draw/tree -autoindex
	setfield ^ \
		script \
		"edit_reac.D; reac_to_pool.w; pool_to_reac.p; graph_reac.w"

	ce /
end



function init_xreac
    // set up the prototype object and icon
	kreacproto

	if (DO_X)
	    xreacproto
	
		// Set up the calls used to handle dragging reacs to pools
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=kreac]" \
			"/kinetics/##[TYPE=kpool]" "REAC" "kpool" "none" -1 0 \
			"" \
			"reac_to_pool_add.p <S> <D>" \
			"reac_to_pool_drop.p <S> <D>"
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=kpool]" \
			"/kinetics/##[TYPE=kreac]" "SUBSTRATE" "kreac" "green" \
			-1 0 \
			"" \
			"pool_to_reac_add.p <S> <D>" \
			"pool_to_reac_drop.p <S> <D>"
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=kpool]" \
			"/kinetics/##[TYPE=kreac]" "KF" "kreac" "purple" \
			-1 0 \
			"" "" ""
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=kpool]" \
			"/kinetics/##[TYPE=kreac]" "KB" "kreac" "hotpink" \
			-1 0 \
			"" "" ""
		/* This draws the other arrow, but flipped */
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=kpool]" \
			"/kinetics/##[TYPE=kreac]" "PRODUCT" "kreac" "green" -1 1 \
			"" "" ""
	
	    // make the editor for reacs
	    make_xedit_reac
	end
	simobjdump kreac kf kb notes xtree_fg_req xtree_textfg_req  x y z

end

/******************************************************************/
