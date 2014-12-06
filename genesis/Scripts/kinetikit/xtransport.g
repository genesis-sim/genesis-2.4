//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 2000 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

/******************************************************************
      Stuff for connecting transports
******************************************************************/
// Delays are implemented as a reac to limit on-rate, which feeds
// into a table set up in TAB_DELAY step_mode. The reac has no
// backward flow of molecules. At the outlet of the transport there is
// just an emerging flow of molecules which needs to be added into
// a target pool.
// transports are created directly on groups, which is handled by the
// xdrag functions.

// called from the transport when a pool is dropped on it
// Attaches the pool to the reac of the transport as a substrate, and
// removes the corresponding amount of stuff from the pool.
function pool_to_transport_add(pool,transport)
	str pool,transport
	addmsg {transport}/reac {pool} REAC A B
	addmsg {pool} {transport} SUBSTRATE n
	call /edit/draw/tree RESET
end


function pool_to_transport_drop(pool,transport)
	str pool,transport
	deletemsg {pool} 0 -find {transport}/reac REAC
	deletemsg {transport}/reac 0 -find {pool} SUBSTRATE
	call /edit/draw/tree RESET
end

// Attaches the transport to the pool as a supplier of translocated mols.
function transport_to_pool_add(transport, pool)
	str transport, pool
	addmsg {transport} {pool} MM_PRD output
	call /edit/draw/tree RESET
end

function transport_to_pool_drop(transport,pool)
	str transport,pool
	deletemsg {transport} 0 -find {transport} MM_PRD
	call /edit/draw/tree RESET
end


/******************************************************************
      Stuff for editing transport
******************************************************************/

// This function is called when transports are recreated from dump files
function fix_transp_msgs
	echo fixing transport msgs
	str name
	int msgcount
	str msgsrc
	foreach name ({el /kinetics/##[TYPE=transport})
		msgsrc = {getmsg {name}/reac -incoming -source 0}
		msgcount = {getmsg {name}/reac -outgoing -count}
		if (msgcount == 1) // The return msg needs sending
			addmsg {name}/reac {msgsrc} REAC A B
		end
		echo {name}
		setclock {getfield  {name} clock} {getfield {name} dt}
		useclock {name} {getfield {name} clock}
		setfield {name}/reac kf {getfield {name} kf}
	end
end

function set_transp_rate_scale(rate)
	float rate
	str transport = {getfield /parmedit/transport elmpath}

	float timescale = {getfield /editcontrol/time scalefactor}
	str timename = {getfield /editcontrol/time unitname}

	str l1

	if ({strcmp {ratename} "kf"} == 0)
		l1 = "No substrate"
	else
		l1 = "No product"
	end
	
	if ({getmsg {transport}/reac -incoming -count}  == 0)
		setfield {dname} value 0
		setfield {uname} label {l1}
	else
		setfield {dname} value  {rate * timescale}
		setfield {uname} label "1/"{timename}
	end

end

function do_update_transportinfo
	if (!{exists /parmedit/transport})
		return
	end
	str transport = {getfield /parmedit/transport elmpath}

	if (!{exists {transport} stepsize})
		return
	end

	float timescale = {getfield /editcontrol/time scalefactor}
	str timename = {getfield /editcontrol/time unitname}

	float dt = {getfield {transport} dt}
    float transporttime = {getfield {transport} table->xdivs} * dt
	float kf = {getfield {transport}/reac kf}

	do_update_generic /parmedit/transport

    setfield /parmedit/transport/kf \
		value {kf * timescale} \
		label "input rate (1/"{timename}")"
    setfield /parmedit/transport/delay \
		value {transporttime / timescale} \
		label "Delay duration ("{timename}")"
    setfield /parmedit/transport/dt \
		value {dt / timescale} \
		label "Delay Timestep ("{timename}")"

end

// Sets the rate. Internally it is always stored in seconds.
function set_transport_kf(v)
	float v
	str t = {getfield /parmedit/transport elmpath}
	float timescale = {getfield /editcontrol/time scalefactor}

	setfield {t}/reac kf {v/timescale}
	setfield {t} kf {v/timescale}
end

// Note that the transporter always stores numbers in seconds.
function set_transport_delay(v)
	float v
	float timescale = {getfield /editcontrol/time scalefactor}
	v = v * timescale
	str t = {getfield /parmedit/transport elmpath}
	float dt = {getfield {t} dt}
	int divs = v / dt

	if (v > 0)
		if (divs > 1000000)
			do_warning 0 "Warning: Transport size = "{divs}". Try larger dt"
			return
		end
		call {t} TABCREATE {divs} 0 {divs}
		setfield {t} delay {v}
	end
end

function set_transport_dt(v)
	float v
	float timescale = {getfield /editcontrol/time scalefactor}
	v = v * timescale
	str t = {getfield /parmedit/transport elmpath}
	int divs = {getfield {t} table->xdivs}
	float delay = {getfield {t} delay}
	int newdivs = delay / v

	if (newdivs > 1000000)
		do_warning 0 "Warning: Transport size = "{newdivs}". Try larger dt"
		return
	end

	if (newdivs > divs)
		call {t} TABCREATE {newdivs} 0 {newdivs}
	end

	setfield {t} dt {v}
	setclock {getfield {t} clock} {v}
end

function set_transport_resettog(v)
	int v
	setfield {getfield /parmedit/transport elmpath} resettog {v}
end

function make_xedit_transport
	create xform /parmedit/transport [{EX},{EY},{EW},420]
	addfield /parmedit/transport elmpath \
		-description "path of elm being edited"
	addfield /parmedit/transport nclocks \
		-description "Number of transport clocks defined"
	setfield /parmedit/transport nclocks 0
	ce /parmedit/transport
	create xdialog path -wgeom 60% -title "Parent" 
	create xdialog name -xgeom 60% -ygeom 0 -wgeom 40% -title "Name" \
		-script "set_item_name <w> <v>"
	create xdialog kf \
		-script "set_transport_kf <v>" \
		-label "Input rate (1/sec)"

	create xdialog delay -script "set_transport_delay <v>" \
		-label "Delay duration (sec)"
	create xdialog dt -script "set_transport_dt <v>" \
		-label "Delay Timestep (sec)"

	create xtoggle resettog -script "set_transport_resettog <v>" \
		-offlabel "Zero out transport buffer values on reset" \
		-onlabel "Guess transport values using input reaction on reset"

	make_colorbar /parmedit/transport resettog
	create xbutton savenotes -label NOTES [0,0:last,10%,130] \
		-script "save_item_notes <w>" -offbg gray
	create xtext notes [0:last,0:Color,90%,130] -editable 1
	create xbutton UPDATE [0%,0:notes,50%,30] \
		-script "do_update_transportinfo"
	create xbutton HIDE [50%,0:notes,50%,30] \
		-script "save_item_notes <w>; xhide /parmedit/transport"
	ce /
end

function edit_transport(transport)
	str transport

	setfield /parmedit/transport elmpath {transport}
	do_update_transportinfo
	xshowontop /parmedit/transport
end

function get_next_clock
	int i
	for (i = 3; i < 20; i = i + 1)
		if ({getclock {i}} == 0)
			return {i}
		end
	end
	return 0
end

function transport_create_func(t)
	str t

	call {t} TABCREATE 100 0 100
	setfield  {t} dt {SIMDT}
	int clockno = {get_next_clock}
	setclock {clockno} {SIMDT}
	setfield {t} clock {clockno}
	useclock {t} {clockno}
	// useclock {t}/reac {clockno}
end

/******************************************************************
      Stuff for initializing transports
******************************************************************/
function transport_MSGIN(action, msgnum)
	str msgtype

	msgtype = {getmsg . -incoming -type {msgnum}}

	if (msgtype == "SUBSTRATE")
		if (action == "ADDMSGIN")
			addforwmsg . {msgnum} reac
		else // DELETEMSGIN
			deleteforwmsg . {msgnum} reac
		end
	end
end

function transport_RESET
	useclock . {getfield . clock}
	if ({getfield . resettog} == 0)
		setfield table ===0
	else 
		call reac PROCESS
		call . PROCESS
		setfield table ==={getfield table->table[0]}
		setfield input 0
	end
end

function transportproto
	create table /transport
	create reac /transport/reac
	setfield /transport/reac kf 0.1 kb 0
	addmsg /transport/reac /transport INPUT B
	setfield /transport step_mode 6

 	addfield /transport dt -description "timestep for transport"
 	addfield /transport delay -description "Total transport delay"
 	addfield /transport clock -description "Clock number used by transport"
 	addfield /transport kf -description "rate const (seconds) for input to transport"
	 addfield /transport resettog -description "Flag: use 0 or input reac msg to fill table on reset"

	create text /transport/notes
 	addfield /transport editfunc -description "name of editfunc for transport"
 	addfield /transport createfunc -description "name of createfunc for transport"
 	addfield /transport xtree_fg_req -description "color for transport icon"
 	addfield /transport xtree_textfg_req -description "text color for transport icon"
 	addfield /transport plotfield -description "field of transport to plot"
 	addfield /transport manageclass -description "Class to manage transports"
	addfield /transport linkparms -description "Fields used for link parms"
	addmsgdef /transport SUBSTRATE n
	addaction /transport ADDMSGIN transport_MSGIN
	addaction /transport DELETEMSGIN transport_MSGIN
	addaction /transport RESET transport_RESET

	addobject transport /transport \
		-author "Upi Bhalla NCBS Bangalore" \
		-description "Table plus reac to implement translocation transport"
	setdefault transport kf 0.1
	setdefault transport resettog 0
	setdefault transport editfunc "edit_transport"
	setdefault transport createfunc "transport_create_func"
	setdefault transport xtree_fg_req "yellow"
	setdefault transport xtree_textfg_req "black"
	setdefault transport plotfield "output"
	setdefault transport manageclass "group"
	setdefault transport linkparms "kf dt delay"
end

function xtransportproto
	ce /control/lib/tree
	create xshape shape -autoindex \
		-fg yellow \
		-drawmode DrawSegs \
		-coords "[-1,0.2,0][1,0.2,0][-0.5,0.4,0][-0.8,0.7,0][-0.8,0.7,0][-0.5,1,0][-0.5,1,0][-0.2,0.7,0][-0.2,0.7,0][-0.5,0.4,0][0,0.7,0][0.9,0.7,0][0.9,0.7,0][0.6,1,0][0.9,0.7,0][0.6,0.4,0]" \
		-text "transport" \
		-textfont 6x10 \
		-textmode draw \
		-textcolor black \
		-linewidth 2 \
		-pixflags v \
		-pixflags c \
		-value "transport" \
		-script "edit_lib.D"

	copy {el ^} /edit/draw/tree -autoindex
	setfield ^ \
		script \
		"edit_transport.D; graph_transport.w"

	ce /
end

function init_xtransport
    // set up the prototype object and icon
	transportproto

	if (DO_X)
	    xtransportproto
	
		// Set up the calls used to handle dragging transports to pools
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=transport]" \
			"/kinetics/##[TYPE=kpool]" "MM_PRD" "kpool" "yellow" -1 0 \
			"" \
			"transport_to_pool_add.p <S> <D>" \
			"transport_to_pool_drop.p <S> <D>"
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=kpool]" \
			"/kinetics/##[TYPE=transport]" SUBSTRATE "transport" "yellow" \
			-1 0 \
			"" \
			"pool_to_transport_add.p <S> <D>" \
			"pool_to_transport_drop.p <S> <D>"

	
	    // make the editor for transports
	    make_xedit_transport
	end
	simobjdump transport input output alloced step_mode stepsize dt \
		delay clock kf xtree_fg_req xtree_textfg_req notes x y z
end

/******************************************************************/
