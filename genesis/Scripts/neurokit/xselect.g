//genesis

function do_xselect_funcs
	int channo

	create x1form /compt_display [480,50,520,370]
	// disable /compt_display
	//DHB create xdraw /compt_display/draw [0,2,100%,335] -transform z2d  \
	    //DHB -wx 18 -wy 18 -cx 9 -cy 9 -refresh_flag FALSE
	create x1draw /compt_display/draw [0,2,100%,335] -transform z2d  \
	    -xmin 0 -xmax 18 -ymin 0 -ymax 18
	ce ^
	copy /xproto/draw/hhna.icon hh.icon
		setfield hh.icon pixcolor Orange linewidth 2
	copy /xproto/draw/excsyn.icon syn.icon
		setfield syn.icon pixcolor Green linewidth 2
	copy /xproto/draw/soma.icon rec.icon
	copy /xproto/draw/fillbox.icon vdep.icon
	copy /xproto/draw/hilight hilight

	setfield # tx -1000 ty -1000   // move proto pixes out of view

	copy /xproto/draw/comptshape compt
	setfield compt pixflags clickable_not
	create x1shape dummy.icon
	create x1shape comptpath -tx 10 -ty 10
	setfield comptpath pixflags clickable_not
	/* This is a brutish hack. We need to handle all this much better  */
	for (channo = 0; channo < 20; channo = channo + 1)
		create x1shape chan{channo} -tx {6 + 2.2*(channo%5)} -ty  \
		    {3.5 + 0.82*channo}
	end

	// turn off automatic select highlighting
	setfield chan# hldispmode invert hlhistmode lastone
        setfield #[ISA=xshape] pixflags hlt1 pixflags ~hlt2

	create x1toggle /compt_display/mouse_mode -script  \
	    "set_compt_disp_mode "<widget>
		setfield /compt_display/mouse_mode label0 "Select"  \
		    label1 "Cut-n-Paste"
	setup_mouse /compt_display/draw 0 0 paste do_chanpaste cut  \
	    do_chancut copy do_chancopy
	setup_mouse /compt_display/draw 0 0 select  \
	    "do_channelselect /compt_display/draw" dummy do_dummy dummy  \
	    do_dummy


	create x1form /compt_select [0,440,480,450]
	// disable /compt_select
	create x1draw /compt_select/draw [0,2,100%,400] -transform  \
	    ortho3d -wx 0.0015 -wy 0.0015 -cz 0.2e-3 -vx 10 -vy -15 -vz  \
	    7
	//DHB setfield ^ wx {user_wx} wy {user_wy} cx {user_cx} cy {user_cy}  \
	    //DHB cz {user_cz}
	setfield ^ xmin {user_cx - user_wx/2} xmax {user_cx + user_wx/2} \
		   ymin {user_cy - user_wy/2} ymax {user_cy + user_wy/2}
	create x1toggle /compt_select/scope [0,5:draw.bottom,49%,25]
		setfield /compt_select/scope  \
		    label0 "Select One Compartment"  \
		    label1 "Select Sub Tree"
	create x1toggle /compt_select/mouse_mode  \
	    [51%,5:draw.bottom,49%,25] -script  \
	    "set_compt_sel_mode "<widget>
		setfield /compt_select/mouse_mode label0 "Edit"  \
		    label1 "Cut-n-Paste"
	setup_mouse /compt_select/draw 0 0 paste do_comptpaste cut  \
	    do_comptcut copy do_comptcopy
	setup_mouse /compt_select/draw 0 0 select do_comptselect stretch \
	     do_stretch rotate do_rotate
// hack added to allow stretch and rotate functions to get coords - DEB 8/3/95
	setfield /compt_select/draw script \
          "do_comptselect.d1; do_stretch.d2 <x> <y> <z>; do_rotate.d3 <x> <y> <z>"

	create x1cell /compt_select/draw/cell -colfield activation  \
	    -colmin -1 -colmax 2 -autocol FALSE
	setfield /compt_select/draw/cell fatrange {user_fatrange1}  // DEB 8/2/95

	/* create xcell /compt_select/draw/cell -colfield activation  \
	    -colmin -1 -colmax 2 -fatfield dia -autocol FALSE
	    */
end

function sub_hilight(name)
	str name

	int nmsgs
	int i
	str child

	setfield {name} activation 1

	nmsgs = {getmsg {name} -incoming -count}

	for (i = 0; i < nmsgs; i = i + 1)
		if ( \
		    {strcmp {getmsg {name} -incoming -type {i}} RAXIAL} == 0 \
		    )
			child = {getmsg {name} -incoming -source {i}}
			sub_hilight {child}
		end
	end
end

function hilight_compt(name)
	str name

	int scope = {getfield /compt_select/scope state}

	if ((user_symcomps))
		setfield {cellpath}/##[TYPE=symcompartment] activation 0
	else
		setfield {cellpath}/##[TYPE=compartment] activation 0
	end
	if ((scope) == 0)	// local

		setfield {name} activation 1
	else
		sub_hilight {name}
	end

	call /compt_select/draw/cell PROCESS
end

function display_compt(name)
	str name
	str chan
	int channo = 0

	foreach chan ({el /compt_display/draw/chan#})
		//DHB
		setfield {chan} text "" iconname dummy.icon
		/*
		setfield {chan} text "" &&xpts &/compt_display/draw/dummy.icon/xpts \
				        &&ypts &/compt_display/draw/dummy.icon/ypts \
				        &&zpts &/compt_display/draw/dummy.icon/zpts \
					npts 0
	*/
	end

	pushe /compt_display/draw

	foreach chan ({el {name}/#[TYPE=tabchannel]})
		//DHB
		setfield chan{channo} text {getfield {chan} name}  \
		    iconname hh.icon
		/*
		setfield chan{channo} text {getfield {chan} name}  \
		    &&xpts &hh.icon/xpts \
		    &&ypts &hh.icon/ypts \
		    &&zpts &hh.icon/zpts \
		    npts {getfield hh.icon npts}
		*/
		channo = channo + 1
	end
	foreach chan ({el {name}/#[TYPE=tab2Dchannel]})
		setfield chan{channo} text {getfield {chan} name}  \
		    iconname hh.icon
		channo = channo + 1
	end
	foreach chan ({el {name}/#[TYPE=hh_channel]})
		//DHB
		setfield chan{channo} text {getfield {chan} name}  \
		    iconname hh.icon
		/*
		setfield chan{channo} text {getfield {chan} name}  \
		    &&xpts &hh.icon/xpts \
		    &&ypts &hh.icon/ypts \
		    &&zpts &hh.icon/zpts \
		    npts {getfield hh.icon npts}
		*/
		channo = channo + 1
	end
	foreach chan ({el {name}/#[TYPE=channelC2]})
		//DHB 
		setfield chan{channo} text {getfield {chan} name}  \
		    iconname syn.icon
		/*
		setfield chan{channo} text {getfield {chan} name}  \
		    &&xpts &syn.icon/xpts \
		    &&ypts &syn.icon/ypts \
		    &&zpts &syn.icon/zpts \
		    npts {getfield syn.icon npts}
		*/
		channo = channo + 1
	end
	foreach chan ({el {name}/#[TYPE=synchan]})  // DEB 8/3/95
		setfield chan{channo} text {getfield {chan} name}  \
		    iconname syn.icon
		channo = channo + 1
	end
	foreach chan ({el {name}/#[TYPE=vdep_channel]})
		//DHB 
		setfield chan{channo} text {getfield {chan} name}  \
		    iconname vdep.icon
		/*
		setfield chan{channo} text {getfield {chan} name}  \
		    &&xpts &vdep.icon/xpts \
		    &&ypts &vdep.icon/ypts \
		    &&zpts &vdep.icon/zpts \
		    npts {getfield vdep.icon npts}
		*/
		channo = channo + 1
	end
	foreach chan ({el {name}/#[TYPE=receptor2]})
		//DHBi
		setfield chan{channo} text {getfield {chan} name}  \
		    iconname rec.icon
		/*
		setfield chan{channo} text {getfield {chan} name}  \
		    &&xpts &rec.icon/xpts \
		    &&ypts &rec.icon/ypts \
		    &&zpts &rec.icon/zpts \
		    npts {getfield rec.icon npts}
		*/
		channo = channo + 1
	end

	setfield comptpath text {name}

	foreach chan ({el /compt_display/draw/chan#})
		if ({strcmp {getfield {chan} text} {channelname}} == 0)
			setfield hilight tx {getfield {chan} tx}  \
			    ty {getfield {chan} ty}
			xupdate /compt_display/draw
			pope
			return
		end
	end
	setfield hilight tx -1000 ty -1000

	xupdate .
	pope
end


function do_comptcut
	str src = {getfield /compt_select/draw value}

	hilight_compt {src}
	cut_to_lib compt_lib {src} {getfield /compt_select/scope state}
	if ((user_symcomps))
		setfield /compt_select/draw/cell  \
		    path {cellpath}/##[TYPE=symcompartment]
	else
		setfield /compt_select/draw/cell  \
		    path {cellpath}/##[TYPE=compartment]
	end
end


function do_comptcopy
	str src = {getfield /compt_select/draw value}

	hilight_compt {src}
	copy_to_lib compt_lib {src} {getfield /compt_select/scope state}
end

function do_comptpaste
	str dst = {getfield /compt_select/draw value}
	str elm = {getfield /compt_lib_form/sel value}
	str  \
	    elmname = {getfield {elm} name} @ "[" @ {getfield {elm} index} @ "]"
	str src = (cellpath) @ "/" @ (elmname)
	float x, y, z
	int rename_flag = {getfield /compt_lib_form/auto_rename state}
	str newname

	if ( \
	    {strcmp {getfield {elm}/{elmname} object->name} "compartment"} != 0 \
	    )
		echo Only compartments may be pasted onto the cell.
		return
	end

	hilight_compt {dst}

	if ((!rename_flag) && ({exists {cellpath}/{elmname}}))
		echo Element {elmname} already exists. Paste not done.
		return
	end
	if ({exists /buffer/temp2})
		delete /buffer/temp2
	end
	copy {elm} /buffer/temp2
	if (rename_flag)
		newname = {do_child_rename /buffer/temp2/{elmname} {dst}}
		src = (cellpath) @ "/" @ (newname)
		subtree_move /buffer/temp2/{newname} {cellpath}
	else
		subtree_move /buffer/temp2/{elmname} {cellpath}; 
	end

	addmsg {dst} {src} AXIAL Vm
	addmsg {src} {dst} RAXIAL Ra Vm
	x = {getfield {dst} x}
	y = {getfield {dst} y}
	z = {getfield {dst} z}
	position {src} R{x} R{y} R{z}
	// refresh the cell display

	if ((user_symcomps))
		setfield /compt_select/draw/cell  \
		    path {cellpath}/##[TYPE=symcompartment]
		setfield {cellpath}xout1/draw/cell  \
		    path {cellpath}/##[TYPE=symcompartment]
		setfield {cellpath}xout2/draw/cell  \
		    path {cellpath}/##[TYPE=symcompartment]
	else
		setfield /compt_select/draw/cell  \
		    path {cellpath}/##[TYPE=compartment]
		setfield {cellpath}xout1/draw/cell  \
		    path {cellpath}/##[TYPE=compartment]
		setfield {cellpath}xout2/draw/cell  \
		    path {cellpath}/##[TYPE=compartment]
	end
end


function do_channelselect(widget)
	str widget

	str icon = {getfield {widget} pixchoose}

	channelname = ({getfield {widget}/{icon} text})
	channelpath = (comptpath) @ "/" @ (channelname)
	hilight_channel
	if (({exists {channelpath}}))
		if (({strcmp {analysis_level} neuron}) == 0)
			update_neuron_params
		end
		/*
		if ({strcmp({analysis_level},compt)} == 0)
			update_compt_params
		end
		*/
		if (({strcmp {analysis_level} channel}) == 0)
			update_channel_params
		end
	end
end


function do_chancut
	do_channelselect /compt_display/draw

	hilight_channel
	cut_to_lib compt_lib {channelpath}  \
	    {getfield /compt_select/scope state}
	display_compt {comptpath}
end


function do_chancopy
	do_channelselect /compt_display/draw

	hilight_channel
	copy_to_lib compt_lib {channelpath}  \
	    {getfield /compt_select/scope state}
end

function do_chanpaste
	str elm = {getfield /compt_lib_form/sel value}
	str  \
	    elmname = {getfield {elm} name} @ "[" @ {getfield {elm} index} @ "]"
	str src = "/compt_lib/" @ (elmname) @ "/" @ (elmname)
	str chtype = {getfield {src} object->name}
	str name

	if ({strcmp {chtype} "compartment"} == 0)
		echo element {elmname} is not a channel
		return
	end

	if (({getfield /compt_select/scope state}) == 1)	// global

		foreach name ({el {cellpath}/#[][activation>0.99]})
			if ({exists {name}/{elmname}})
				echo Element {elmname} already exists.  \
				    Paste not done.
			else
				pastechannel {src} {name}
			end
		end
	else	// local

		if ({exists {comptpath}/{elmname}})
			echo Element {elmname} already exists. Paste not \
			     done.
			return
		end
		pastechannel {src} {comptpath}
	end

	hilight_compt {comptpath}

	display_compt {comptpath}
end


function do_comptselect
	str name

	name = {getfield /compt_select/draw value}
	if (({exists {name}}))
		comptpath = name
		comptname = {getfield {name} name}
		channelpath = (comptpath) @ "/" @ (channelname)
		hilight_compt {comptpath}
		// hilight_compt({comptpath},{get(/compt_select/scope,state)})
		if (({strcmp {analysis_level} neuron}) == 0)
			update_neuron_params
		end
		if (({strcmp {analysis_level} compt}) == 0)
			update_compt_params
		end
		if (({strcmp {analysis_level} channel}) == 0)
			update_channel_params
		end
		display_compt {comptpath}
	end
end

function set_compt_sel_mode(widget)
    str widget

    int state = {getfield {widget} state}

    if (state == 1)
        select_mouse /compt_select/draw paste do_comptpaste cut  \
            do_comptcut copy do_comptcopy
    else
        select_mouse /compt_select/draw select do_comptselect stretch  \
            do_stretch rotate do_rotate
// hack added to allow stretch and rotate functions to get coords - DEB 8/3/95
	setfield /compt_select/draw script \
	"do_comptselect.d1; do_stretch.d2 <x> <y> <z>; do_rotate.d3 <x> <y> <z>"

    end
end

function set_compt_disp_mode(widget)
    str widget

    int state = {getfield {widget} state}

    if (state == 1)
        select_mouse /compt_display/draw paste do_chanpaste cut  \
            do_chancut copy do_chancopy
    else
        select_mouse /compt_display/draw select  \
            "do_channelselect /compt_display/draw" dummy do_dummy dummy  \
            do_dummy
    end
end
