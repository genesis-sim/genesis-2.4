//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-1997 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/


/******************************************************************
      Stuff for connecting pools : this is done by the reacs etc
	  except for the case of setting up SUMTOTAL messages between
	  pools, which is done here
******************************************************************/

function pool_to_pool_add_sumtot(srcpool,destpool)
	str srcpool,destpool

	if ({getfield /simcontrol/poolsumtot state} == 0)
		echo connecting {srcpool} to {destpool} for SUMTOTAL
		addmsg {srcpool} {destpool} SUMTOTAL n nInit
		call /edit/draw/tree RESET
	end
end

function pool_to_pool_drop_sumtot(srcpool,destpool)
	str srcpool,destpool

	if ({getfield /simcontrol/poolsumtot state} == 0)
		echo disconnecting {srcpool} from {destpool} for SUMTOTAL
		deletemsg {destpool} 0 -find {srcpool} SUMTOTAL
		call /edit/draw/tree RESET
	end
end

function pool_to_pool_add_consv(srcpool,destpool)
	str srcpool,destpool

	if ({getfield /simcontrol/poolsumtot state} == 1)
		echo connecting {srcpool} to {destpool} for CONSERVE
		addmsg {srcpool} {destpool} CONSERVE n nInit
		call /edit/draw/tree RESET
	end
end

function pool_to_pool_drop_consv(srcpool,destpool)
	str srcpool,destpool

	if ({getfield /simcontrol/poolsumtot state} == 1)
		echo disconnecting {srcpool} from {destpool} for CONSERVE
		deletemsg {destpool} 0 -find {srcpool} CONSERVE
		call /edit/draw/tree RESET
	end
end

function make_mirror(src, window, pool)
	str src
	str window
	str pool

	echo in make_mirror {src},  {window}, {pool}
	if ({strcmp {window} "/edit/draw/tree"} == 0)
		if ({isa kpool {pool}})
			create kpool {pool}_mirror -autoindex
			str newpool = {el ^}
			addmsg {newpool} {pool} MIRROR n
			setfield {newpool} \
				xtree_shape_req mirror \
				y {{getfield {pool} y} + 1} \
				x {getfield {pool} x} \
				n {getfield {pool} n} \
				nInit {getfield {pool} nInit} \
				vol {getfield {pool} vol}
			setfield {newpool}/notes str {getfield {pool}/notes str}

			call /edit/draw/tree RESET
			return
		end
	end
	do_inform "Warning: mirrors must be made on existing pools in the edit window" 0
end


/******************************************************************
      Stuff for editing pool
******************************************************************/

/* assumes that the current directory is the same as the poolinfo,
** and that the name is current */
function do_update_poolinfo
	if (!{exists /parmedit/pool})
		return
	end
	str pool = {getfield /parmedit/pool elmpath}
	if ({strlen {pool}} == 0)
		return
	end
	if (!{exists {pool}})
		return
	end
	float vol = {getfield {pool} vol}
	float realvol
	float dia
	float scale = {getfield /editcontrol/conc scalefactor}

	realvol = scale * vol / NA
	dia = {calc_dia {realvol}}
	setfield {pool} realvol {realvol}

	setfield /parmedit/pool/Co \
		label "Co ("{getfield /editcontrol/conc unitname}")"
	setfield /parmedit/pool/CoInit \
		label "CoInit ("{getfield /editcontrol/conc unitname}")"

	do_update_generic /parmedit/pool
    setfield /parmedit/pool/geomname value {getfield {pool} geomname}
    setfield /parmedit/pool/vol value {realvol}
    setfield /parmedit/pool/dia value {dia}
    // setfield /parmedit/pool/keepconc state {getfield {pool} keepconc}
    setfield /parmedit/pool/n value {getfield {pool} n}
    setfield /parmedit/pool/nInit value {getfield {pool} nInit}
//    setfield /parmedit/pool/nTotal value {getfield {pool} nTotal}
//    setfield /parmedit/pool/nRemaining value {getfield {pool} nRemaining}
    setfield /parmedit/pool/Co value {getfield {pool} Co}
    setfield /parmedit/pool/CoInit value {getfield {pool} CoInit}
//    setfield /parmedit/pool/CoTotal value {getfield {pool} CoTotal}
//    setfield /parmedit/pool/CoRemaining value {getfield {pool} CoRemaining}
    setfield /parmedit/pool/DiffConst value \
		{{getfield {pool} DiffConst} * 1e12}
    setfield /parmedit/pool/mwt value {getfield {pool} mwt}
	setfield /parmedit/pool/buffer_enable state {({getfield {pool} slave_enable} & 4) != 0}
	// setfield /parmedit/pool/stoch_flag state {getfield {pool} stoch_flag}
end


function set_buffering(state)
	int state

	str pool = {getfield /parmedit/pool elmpath}
	int oldslave_enable = {getfield {pool} slave_enable}

	if (state)
		setfield {pool} slave_enable {oldslave_enable | 4}
	else
		setfield {pool} slave_enable {oldslave_enable & 3}
	end
end


function rescale_pool_vol(scale)
	float scale

    float vol
    str name
	float scale
	float concscale = {getfield /editcontrol/conc scalefactor}

    foreach name ({el /kinetics/##[TYPE=kpool]})
		vol = {getfield {name} vol} * scale
        setfield {name} \
			keepconc 0 \
        	vol {vol} \
        	realvol {concscale * vol / NA} \
        	n {scale * {getfield {name} n}} \
        	nInit {scale * {getfield {name} nInit}} \
        	nTotal {scale * {getfield {name} nTotal}} \
        	nRemaining {scale * {getfield {name} nRemaining}}
    end
	do_update_poolinfo
end

function update_all_pool_realvol
    str name
    float vol
    float scale = {getfield /editcontrol/conc scalefactor}

    foreach name ({el /kinetics/##[TYPE=kpool]})
        vol = {getfield {name} vol}
        setfield {name} keepconc 0 realvol {scale * vol / NA}
    end
end

function update_all_pool_vol
    str name
    float realvol
    float scale = {getfield /editcontrol/conc scalefactor}

    foreach name ({el /kinetics/##[TYPE=kpool]})
        realvol = {getfield {name} realvol}
        setfield {name} vol {NA * realvol / scale}
    end
end

function set_pool_vol(w, vol)
	str w
	float vol
	str pool = {getfield /parmedit/pool elmpath}
	float scale = {getfield /editcontrol/conc scalefactor}

	setfield {w}/../dia value {calc_dia {vol}}
	setfield {pool} realvol {vol}
	setfield {pool} vol {NA * vol / scale}
end

function set_pool_dia(w, dia)
	str w
	float dia

	float vol = {calc_vol {dia}}
	str pool = {getfield /parmedit/pool elmpath}
	float scale = {getfield /editcontrol/conc scalefactor}

	setfield {w}/../vol value {vol}
	setfield {pool} realvol {vol}
	setfield {pool} vol {NA * vol / scale}
end

function set_pool_diff_const(w, D)
	str w
	float D

	str pool = {getfield /parmedit/pool elmpath}

	setfield {pool} DiffConst {D * 1e-12}
end

function update_pool_conc
	str pool = {getfield /parmedit/pool elmpath}
	if ({exists {pool} Co})
		setfield /parmedit/pool/Co value {getfield {pool} Co}
		setfield /parmedit/pool/n value {getfield {pool} n}
	end
end

function make_xedit_pool
	create xform /parmedit/pool [{EX},{EY},{EW},450]
	addfield /parmedit/pool elmpath -description "path of pool"
	ce /parmedit/pool
	create xdialog path -wgeom 60% -title "Parent"
	create xdialog name -xgeom 60% -ygeom 0 -wgeom 40% -title "Name" \
		-script "set_item_name <w> <v>"
	create xdialog geomname \
		-label "Compartment" \
		-script "set_field <w> <v>; do_update_poolinfo"
	create xdialog vol [0,0:geomname,50%,30] \
		-label "Volume (m^3)" \
		-script "set_pool_vol <w> <v>"
	create xdialog dia [0:last,0:geomname,50%,30] \
		-label "Diameter (um)" \
		-script "set_pool_dia <w> <v>"
	create xlabel nl1 -bg cyan -wgeom 50% \
		-label "Run-time amounts"
	create xlabel nl2 -bg cyan [0:last,0:dia,50%,] \
		-label "Initial/buffered amounts"
	create xdialog n [0,0:nl1,50%,30] \
		-script "set_field <w> <v>; do_update_poolinfo"
	create xdialog nInit [50%,0:nl1,50%,30] \
		-script "set_field <w> <v>; do_update_poolinfo"
		/*
	create xdialog nTotal [0,0:n,50%,30] \
		-script "set_field <w> <v>; do_update_poolinfo"
	create xdialog nRemaining [50%,0:n,50%,30] \
		-script "set_field <w> <v>; do_update_poolinfo"
		*/
	create xdialog Co [0,0:n,50%,30] \
		-label "Co (uM)" \
		-script "set_field <w> <v>; do_update_poolinfo"
	create xdialog CoInit [50%,0:n,50%,30] \
		-label "CoInit (uM)" \
		-script "set_field <w> <v>; do_update_poolinfo"

	create xtoggle buffer_enable \
		-script "set_buffering <v>" \
		-onlabel "Buffering ON" \
		-offlabel "Buffering OFF"

	create xlabel wl -bg cyan \
		-label "Molecule properties"
	create xdialog DiffConst [0,0:wl,50%,30] \
		-label "Diff Const (um^2/sec)" \
		-script "set_pool_diff_const <w> <v>; do_update_poolinfo"
	create xdialog mwt [0:last,0:wl,50%,30] \
		-label "Molec Wt (Da)" \
		-script "set_field <w> <v>; do_update_poolinfo"

		/*
	create xdialog CoTotal [0,0:Co,50%,30] \
		-script "set_field <w> <v>; do_update_poolinfo"
	create xdialog CoRemaining [50%,0:Co,50%,30] \
		-script "set_field <w> <v>; do_update_poolinfo"
		*/
	/*
	create xtoggle keepconc [0,0:CoTotal,50%,] \
		-onlabel "Conc fixed, n changes" \
		-offlabel "n fixed, conc changes" \
		-script "set_field <w> <v>"
	*/
	/*
	create xtoggle stoch_flag [0:last,0:CoRemaining,50%,] \
		-script "set_field <w> <v>" \
		-onlabel "Stochastic mode ON" \
		-offlabel "Stochastic mode OFF"
		*/
	// create xdialog color -script "set_item_color <w> <v>"
	make_colorbar /parmedit/pool mwt
	create xbutton savenotes -label NOTES [0,0:last,10%,130] \
		-script "save_item_notes <w>" -offbg gray
	create xtext notes [0:last,0:Color,90%,130] -editable 1
	create xbutton UPDATE [0%,0:notes,50%,30] \
		-script "do_update_poolinfo"
	create xbutton HIDE [50%,0:notes,50%,30] \
		-script "save_item_notes <w>; xhide /parmedit/pool"
	ce /
	create script_out /control/update_pool_conc
	setfield /control/update_pool_conc command update_pool_conc
	useclock /control/update_pool_conc {CONTROLCLOCK}
end

function edit_pool(pool)
	str pool

	setfield /parmedit/pool elmpath {pool}
	do_update_poolinfo
	xshowontop /parmedit/pool
end


/******************************************************************
      Stuff for initializing pools
*****************************************************************/

function pool_create_func(pool)
	str pool
	
	float vol = {getfield /editcontrol/vol value}
	float scale = {getfield /editcontrol/conc scalefactor}
	setfield {pool} vol {NA * vol / scale}
	setfield {pool} realvol {vol}
	setfield {pool} nTotal 0
end

function nonxpoolproto
	create pool /pool
	setfield /pool \
		Co 0 \
		CoInit 0 \
		CoTotal 0 \
		vol 0 \
		keepconc 0
	create text /pool/notes
	addfield /pool notes
	addfield /pool realvol -description "actual vol, not a scale factor"
	addfield /pool mwt -description "Molecular wt in Daltons"
	addfield /pool DiffConst -description "Diffusion const in m^2/sec"
	addfield /pool geomname -description "Geometrical/cellular compt"
	addfield /pool editfunc -description "func for pool edit"
	addfield /pool createfunc -description "func for pool edit"
	addfield /pool xtree_fg_req -description "color for pool icon"
	addfield /pool xtree_textfg_req \
		-description "textcolor for pool icon"
	addfield /pool xtree_shape_req -description "icon for pool"
	addfield /pool plotfield -description "field to plot"
	addfield /pool manageclass -description "Class that can manage it"
	addfield /pool linkparms -description "Fields used for link parms"

	addobject kpool /pool \
		-author "Upi Bhalla Mt Sinai March 1994" \
	-description "pool plus a notes field"
	setdefault kpool mwt 0
	setdefault kpool DiffConst 0
	setdefault kpool geomname "default_compartment"
	setdefault kpool editfunc "edit_pool"
	setdefault kpool createfunc "pool_create_func"
	setdefault kpool xtree_fg_req "blue"
	setdefault kpool xtree_shape_req "kpool"
	setdefault kpool xtree_textfg_req "black"
	setdefault kpool plotfield "Co"
	setdefault kpool manageclass "group"
	setdefault kpool linkparms "vol slave_enable nInit"
end

function xpoolproto
	ce /control/lib/tree
	create xshape shape -autoindex \
		-drawmode FillRects \
		-fg blue \
		-coords [-.9,-.5,0][.9,.5,0] \
		-text "pool" \
		-textfont 7x13bold \
		-pixflags v \
		-pixflags c \
		-value "kpool" \
		-script \
		"edit_pool.D"

	copy {el ^} /edit/draw/tree -autoindex
	setfield ^ \
		script "edit_pool.D"

	/*
	copy /control/lib/mirror /edit/draw/tree/shape[{newindex + 1}]
	setfield ^ script "edit_pool.D" \
		pixflags v \
		pixflags c \
		tx 0
	ce /control/lib
	create xshape mirror \
		-drawmode DrawLines \
		-fg blue \
		-coords [-.8,.8,0][1,-1,0][1,1,0][-1,1,0][.8,-.8,0][-1,1,0][-1,-1,0][1,-1,0] \
		-text "mirror" \
		-textfont 7x13bold \
		-pixflags v \
		-pixflags c \
		-value "mirror" \
		-tx 12 -ty 0

	setfield ^ \
		script "make_mirror.w <d> <D> <S>"
	*/
	ce /
end

function init_xpool
    // set up the prototype
	nonxpoolproto
	if (DO_X)
    	xpoolproto

		// Set up the calls to handle dragging pools to pools
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=kpool]" \
			"/kinetics/##[TYPE=kpool]" "SUMTOTAL" "kpool" "darkgreen" -1 0 \
			"echo.p dragging <S> to <D> for sumtotal" \
			"pool_to_pool_add_sumtot.p <S> <D>" \
			"pool_to_pool_drop_sumtot.p <S> <D>"

		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=kpool]" \
			"/kinetics/##[TYPE=kpool]" "CONSERVE" "kpool" "blue" -1 0 \
			"echo.p dragging <S> to <D> for conserve" \
			"pool_to_pool_add_consv.p <S> <D>" \
			"pool_to_pool_drop_consv.p <S> <D>"
	
    	// make the editor for pools
    	make_xedit_pool
	end
	// Set up the dumping fields
	// Note that we have replaced CoTotal with DiffConst
	// and nTotal with mwt
	simobjdump kpool DiffConst CoInit Co n nInit mwt \
		nMin vol slave_enable geomname \
		xtree_fg_req xtree_textfg_req x y z
end

/******************************************************************/
