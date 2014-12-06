//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-1997 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

/******************************************************************
      Stuff for connecting chans
******************************************************************/

// called from the chan when a proto of it is dropped onto a pool
// Must be called immediately after the chan has been created
function chanproto_to_pool_link(chan,pool,x,y,z)
	str chan,pool
	float x,y,z

		// call /edit/draw/tree MOVECUSTOM {chan} {x} {y + 1} {z} 1
		position {chan} {x} {y + 1} {z}
		setfield {chan} xtree_textfg_req {getfield {pool} xtree_fg_req}

		// numerically attach the chan to the pool
		addmsg {pool} {chan} NUMCHAN n
end

// Attaches the pool to the chan as a substrate
function pool_to_chan_add(pool,chan)
	str pool,chan

	echo in pool_to_chan_add with {chan} {pool}
	addmsg {chan} {pool} REAC A B
	addmsg {pool} {chan} SUBSTRATE n vol
	call /edit/draw/tree RESET
end

// Detaches the pool from the chan as a substrate
function pool_to_chan_drop(pool,chan)
	str pool,chan

	echo in pool_to_chan_drop with {chan} {pool}
	deletemsg {pool} 0 -find {chan} REAC
	deletemsg {chan} 0 -find {pool} SUBSTRATE
	call /edit/draw/tree RESET
end

// Attaches the pool to the chan as a product
function chan_to_pool_add(chan,pool)
	str chan,pool

	echo in chan_to_pool_add with {chan} {pool}
	addmsg {chan} {pool} REAC B A
	addmsg {pool} {chan} PRODUCT n vol
	call /edit/draw/tree RESET
end

// Detaches the pool from the chan as a product
function chan_to_pool_drop(chan,pool)
	str chan,pool

	echo in chan_to_pool_drop with {chan} {pool}
	deletemsg {pool} 0 -find {chan} REAC
	deletemsg {chan} 0 -find {pool} PRODUCT
	call /edit/draw/tree RESET
end

/******************************************************************
      Stuff for editing chan
******************************************************************/


function do_update_chaninfo
	str chan = {getfield /parmedit/chan elmpath}
	do_update_generic /parmedit/chan

    setfield /parmedit/chan/n value {getfield {chan} n }
    setfield /parmedit/chan/perm value {getfield {chan} perm}
    setfield /parmedit/chan/gmax value {getfield {chan} gmax }
end

function make_xedit_chan
	create xform /parmedit/chan [{EX},{EY},{EW},270]
	addfield /parmedit/chan elmpath \
		-description "path of elm being edited"
	ce /parmedit/chan
	create xdialog path -wgeom 60% -title "Parent"
	create xdialog name -xgeom 60% -ygeom 0 -wgeom 40% -title "Name" \
		-script "set_item_name <w> <v>"
	create xdialog color [0%,0:name,50%,30] \
		-script "set_item_color <w> <v>"
	create xdialog n [50%,0:name,50%,30] \
		-script "set_field <w> <v>"
	create xdialog perm [0%,0:n,50%,30] \
		-script "set_field <w> <v>"
	create xdialog gmax [50%,0:n,50%,30] \
		-script "set_field <w> <v>"
	create xbutton savenotes -label NOTES [0,0:last,10%,130] \
		-script "save_item_notes <w>" -offbg gray
	create xtext notes [0:last,0:gmax,90%,130] -editable 1
	create xbutton UPDATE [0%,0:notes,50%,30] \
		-script "do_update_chaninfo"
	create xbutton HIDE [50%,0:notes,50%,30] \
		-script "save_item_notes <w>; xhide /parmedit/chan"
	ce /
end

function edit_chan(reac)
	str reac

	setfield /parmedit/chan elmpath {reac}
	do_update_chaninfo
	xshowontop /parmedit/chan
end

/******************************************************************
      Stuff for initializing chans
******************************************************************/

function kchanproto
	create concchan /chan
		setfield /chan \
			perm 0.1	\
			gmax 0.1

	create text /chan/notes
	addfield /chan notes
    addfield /chan editfunc -description "func for chan edit"
    addfield /chan xtree_fg_req -description "color for chan icon"
    addfield /chan xtree_textfg_req -description "textcolor for chan icon"
    addfield /chan plotfield -description "field to plot"
    addfield /chan manageclass -description "Class that can manage it"
	addfield /chan link_to_manage -descript "func to link chan to manager"
	addfield /chan linkparms -description "Fields used for link parms"
 
    addobject kchan /chan \
        -author "Upi Bhalla Mt Sinai July 1994" \
        -description "concchan plus a notes field"
    setdefault kchan editfunc "edit_chan"
    setdefault kchan xtree_fg_req "brown"
    setdefault kchan xtree_textfg_req "white"
    setdefault kchan plotfield "n"
    setdefault kchan manageclass "kpool"
    setdefault kchan link_to_manage "chanproto_to_pool_link"
    setdefault kchan linkparms "perm"
end

function xchanproto
	ce /control/lib/tree
	create xshape shape -autoindex \
		-fg black \
		-npts 30 \
		-coords  [1,0,0][1,.8,0][1,.8,0][.6,.8,0][.6,.8,0][.4,.6,0] \
		[.4,.6,0][.2,.8,0][.2,.8,0][-.2,.8,0][-.2,.8,0][-.2,0,0] \
		[-.2,0,0][1,0,0][.6,1,0][.2,1,0][.2,1,0][.4,.8,0][.4,.8.0] \
		[.6,1,0][-1,0,0][-1,.8,0][-1,.8,0][-.6,.8,0][-.6,.8,0] \
		[-.6,0,0][-.6,0,0][-1,0,0] \
		-linewidth 2 \
		-drawmode DrawSegments \
		-value "kchan" \
		-pixflags v -pixflags c \
		-script "edit_chan.D"

	copy {el ^}  /edit/draw/tree -autoindex
	setfield ^ \
		script "edit_chan.D"

    ce /
end


function init_xchan
    // set up the prototype
	kchanproto

	if (DO_X)
	
	    xchanproto
	
		// Set up the calls used to handle dragging chan to pool
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=kchan]" \
			"/kinetics/##[TYPE=kpool]" REAC kpool none -1 0 \
			"echo.p dragging <S> to <D> for chan product" \
			"chan_to_pool_add.p <S> <D>" \
			"chan_to_pool_drop.p <S> <D>"
		// This draws the arrow for the product, but flipped:
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=kpool]" \
			"/kinetics/##[TYPE=kchan]" PRODUCT kchan orange -1 1 \
			"" "" ""
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=kpool]" \
			"/kinetics/##[TYPE=kchan]" SUBSTRATE kchan orange -1 0 \
			"echo.p dragging <S> to <D> for chan substrate" \
			"pool_to_chan_add.p <S> <D>" \
			"pool_to_chan_drop.p <S> <D>"
	
	    // make the editor for chans
	    make_xedit_chan
	end
	simobjdump kchan perm gmax Vm is_active use_nernst notes \
	  xtree_fg_req xtree_textfg_req  x y z

end	

/******************************************************************/
