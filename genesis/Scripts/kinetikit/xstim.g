//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-1997 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/


/******************************************************************
      Stuff for connecting stims
	  Stims are unusual since they can affect many objects.
	  For now we will restrict their action to pools and
	  other stims.
******************************************************************/

// Attaches the stim to a pool
function stim_to_pool_add(stim,pool)
	str stim,pool
	int is_running = {getfield {stim} is_running}
	int pool_s_e = {getfield {pool} slave_enable} & 4
	
	echo in stim_to_pool_add with {stim}, {pool}
	addmsg {stim} {pool} SLAVE output
	if (is_running == 3)
		setfield {pool} slave_enable {2 | pool_s_e}
	else
		if (is_running == 1)
			setfield {pool} slave_enable {1 | pool_s_e}
		else
			setfield {pool} slave_enable {pool_s_e}
		end
	end
	call /edit/draw/tree RESET
end

// Detaches the stim from the pool
// This assumes that only 1 stim at a time goes to a pool,
// since it disables slaving
function stim_to_pool_drop(stim,pool)
	str stim,pool
	int pool_s_e = {getfield {pool} slave_enable} & 4
	
	echo in stim_to_pool_drop with {stim}, {pool}
	deletemsg {pool} 0 -find {stim} SLAVE
	setfield {pool} slave_enable {pool_s_e}
	call /edit/draw/tree RESET
end

// Attaches the stim to a stim
function stim_to_stim_add(src,dest)
	str src,dest
	
	echo in stim_to_stim_add with {src}, {dest}
	addmsg {src} {dest} INPUT output
	call /edit/draw/tree RESET
end

// Detaches the stim from the stim
// since it disables slaving
function stim_to_stim_drop(src,dest)
	str src,dest
	
	echo in stim_to_stim_drop with {src}, {dest}
	deletemsg {dest} 0 -find {src} INPUT
	call /edit/draw/tree RESET
end

/******************************************************************
      Stuff for editing stim
******************************************************************/

/* assumes that the current directory is the same as the stiminfo,
** and that the name is current */
function do_update_stiminfo
	str stim = {getfield /parmedit/stim elmpath}
	do_update_generic /parmedit/stim

	setfield /parmedit/stim/baselevel value {getfield {stim} baselevel}
	setfield /parmedit/stim/level1 value {getfield {stim} level1}
	setfield /parmedit/stim/width1 value {getfield {stim} width1}
	setfield /parmedit/stim/delay1 value {getfield {stim} delay1}
	setfield /parmedit/stim/is_running state {({getfield {stim} is_running} & 1) > 0}
	setfield /parmedit/stim/n_or_conc state {({getfield {stim} is_running} & 2) > 0}
end

function set_slave_enables(stim,slave_enable)
	/* going through the destination pools and setting their
	** slave_enable flags */
	int	i
	int nmsgs
	str src
	int pool_s_e
	nmsgs = {getmsg {stim} -outgoing -count}
	for (i = 0 ; i < nmsgs ; i = i + 1)
		src = {getmsg {stim} -outgoing -destination {i}}
		if ({exists {src} slave_enable})
			pool_s_e = {getfield {src} slave_enable} & 4
			setfield {src} slave_enable {slave_enable | pool_s_e}
		end
	end
end

function set_stim_state
	str stim = {getfield /parmedit/stim elmpath}

	int n_or_conc = {getfield /parmedit/stim/n_or_conc state}
	int is_running = {getfield /parmedit/stim/is_running state}

    int old_is_running = {getfield {stim} is_running}
	int new_is_running = is_running + 2 * n_or_conc

	// setting the color of the stim
	if (is_running)
        setfield {stim} xtree_textfg_req red
		setfield {stim} trig_mode 0 // free run
    else
        setfield {stim} xtree_textfg_req red
		setfield {stim} trig_mode 1 // ext trig
    end

	if (old_is_running != new_is_running)
		setfield {stim} is_running {new_is_running}
		/* if the new_is_running is 0 or 1, it is OK as is */
		if (new_is_running == 2) // convert to the slave_enable form
			new_is_running = 0
		end
		if (new_is_running == 3) // convert to the slave_enable form
			new_is_running = 2
		end
		set_slave_enables {stim} {new_is_running}
	end
end

function make_xedit_stim
	create xform /parmedit/stim [{EX},{EY},{EW},325]
	addfield /parmedit/stim elmpath \
		-description "path of elm being edited"
	ce /parmedit/stim

	create xdialog path -wgeom 60% -title "Parent"
	create xdialog name -xgeom 60% -ygeom 0 -wgeom 40% \
		-title "Name" \
		-script "set_item_name <w> <v>"
	create xdialog width1 [0,0:name,50%,30] -label "Stim Width" \
		-script "set_field <w> <v>"
	create xtoggle n_or_conc [50%,0:name,50%,30] \
		-onlabel "Level uses concentration units" \
		-offlabel "Level uses # units" \
		-script "set_stim_state"
	create xdialog baselevel [0,0:width1,50%,30] \
		-label "Baseline level" \
		-script "set_field <w> <v>"
	create xdialog level1 [50%,0:width1,50%,30] -label "Stim Level" \
		-script "set_field <w> <v>"
	create xdialog delay1 [0%,0:baselevel,50%,30] \
		-label "Interpulse Delay" -script "set_field <w> <v>"

	create xtoggle is_running [0:last,0:baselevel,50%,30] \
		-onlabel "Stimulus ON: click to stop" \
		-offlabel "Stimulus OFF: click to start" \
		-script "set_stim_state"

	 make_colorbar /parmedit/stim delay1

	create xbutton savenotes -label NOTES [0,0:last,10%,130] \
		-script "save_item_notes <w>" -offbg gray
	create xtext notes [0:last,0:Color,90%,130] -editable 1
	create xbutton UPDATE [0%,0:notes,50%,30] \
		-script "do_update_stiminfo"
	create xbutton HIDE [50%,0:notes,50%,30] \
		-script "xhide /parmedit/stim"
	ce /
end

function edit_stim(stim)
	str stim

	setfield /parmedit/stim elmpath {stim}
	do_update_stiminfo
	xshowontop /parmedit/stim
end

/******************************************************************
      Stuff for initializing stims
******************************************************************/

function kstimproto
	create pulsegen /stim
	setfield /stim	\
		width1 1	\
		level1 1	\
		delay1 1	\
		trig_mode 1

	create text /stim/notes
	addfield /stim notes
    addfield /stim editfunc -description "func for stim edit"
    addfield /stim xtree_fg_req -description "color for stim icon"
    addfield /stim xtree_textfg_req -description "color for stim icon"
    addfield /stim plotfield -description "field to plot"
    addfield /stim manageclass -description "Class that can manage it"
    addfield /stim is_running -description "flag for state of stim"
 
    addobject stim /stim \
        -author "Upi Bhalla Mt Sinai Apr 1994" \
        -description "stim plus a notes field"
    setdefault stim editfunc "edit_stim"
    setdefault stim xtree_fg_req "white"
    setdefault stim xtree_textfg_req "red"
    setdefault stim plotfield "output"
    setdefault stim manageclass "group"
    setdefault stim is_running "0"
end

function xstimproto
	ce /control/lib/tree
	create xshape shape -autoindex \
		-fg green \
		-drawmode DrawLines \
		-linewidth 2 \
		-coords [0,1,0][.5,.2,0][.25,0,0][.5,-0.8,0][0,0,0] \
			[.25,.2,0][0,1,0] \
		-text "stim" -textmode draw -textcolor black \
		-value "stim" \
		-pixflags v \
		-pixflags c \
		-script \
		"edit_stim.D"

	copy {el ^} /edit/draw/tree -autoindex
	setfield ^ script "edit_stim.D"

	ce /
end


function init_xstim
    // set up the prototype
	kstimproto

	if (DO_X)
	    xstimproto
	
		// Set up the calls used to handle dragging stim to pool
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=stim]" \
			"/kinetics/##[TYPE=kpool]" SLAVE kpool orange -1 0 \
			"echo.p dragging <S> to <D> for stimulus" \
			"stim_to_pool_add.p <S> <D>" \
			"stim_to_pool_drop.p <S> <D>"
	
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=stim]" \
			"/kinetics/##[TYPE=stim]" INPUT stim orange -1 0 \
			"echo.p dragging <S> to <D> for stim" \
			"stim_to_stim_add.p <S> <D>" \
			"stim_to_stim_drop.p <S> <D>"
	
	
	    // make the editor for stims
	    make_xedit_stim
	end
	simobjdump stim level1 width1 delay1 level2 width2 delay2 \
		baselevel trig_time trig_mode notes xtree_fg_req \
	    xtree_textfg_req is_running  x y z

end
