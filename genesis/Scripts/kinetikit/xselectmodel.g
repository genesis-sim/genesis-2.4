//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-2004 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

str old_model_name = "nil"

function do_stim_run(modelname)
	str modelname
	str stimname
	float time
	float value

	foreach stimname ({el /modelstim/{modelname}/stim[]})
		time = {getfield {stimname} time}
	end
	setfield /control/runtime value {time}

	reset
	float currtime = 0
	foreach stimname ({el /modelstim/{modelname}/stim[]})
		time = {getfield {stimname} time}
		if (time > currtime)
			MAXTIME = time - currtime
			do_simple_run
			currtime = time
		end
		if (IS_RUNNING == 0)
			sh echo t={getstat -time}"	simulation halted" >> /tmp/comment
			setfield /comment/text filename /tmp/comment
			return
		end
		setfield {getfield {stimname} path} {getfield {stimname} field} {getfield {stimname} value}
		sh echo t={time}"	"{getfield {stimname} comment} >> /tmp/comment
		setfield /comment/text filename /tmp/comment
	end
	IS_RUNNING = 0
end

function read_stim_line
	if ({argc} < 5)
		return 0
	end

	if ({strncmp {argv 1} "//" 2} == 0)
		return 1
	end

	create neutral stim -autoindex
	pushe ^

	addfield . time
	setfield . time {argv 1}
	addfield . path
	setfield . path {argv 2}
	addfield . field
	setfield . field {argv 3}
	addfield . value
	setfield . value {argv 4}
	addfield . comment

	str comment = {argv 5}
	int i
	for (i = 6; i <= {argc}; i = i + 1)
		comment = comment @ " " @ {argv {i}}
	end
	setfield . comment {comment}
	pope

	return 1
end

function read_stimulus(mname)
	create neutral /modelstim/{mname}
	pushe /modelstim/{mname}
	str fname = "models/" @ {mname} @ ".stim"
	if ({fileexists {fname}})
		openfile {fname} "r"

		while ({read_stim_line {readfile {fname}}})
		end

		closefile {fname}
	else
		read_stim_line 0 / x 0 "No stimulus file available"
	end
	pope
end

function store_current_model_state(mname)
	str mname
	if ({strcmp {mname} "nil"} != 0)
		setfield /models/{mname} fastdt {FASTDT}
		setfield /models/{mname} simdt {SIMDT}
		setfield /models/{mname} controldt {CONTROLDT}
		setfield /models/{mname} plotdt {PLOTDT}
		setfield /models/{mname} variable_dt_flag {VARIABLE_DT_FLAG}
		setfield /models/{mname} transient_time {TRANSIENT_TIME}
		setfield /models/{mname} default_vol {DEFAULT_VOL}
		setfield /models/{mname} use_automatic_dt {USE_AUTOMATIC_DT}
		setfield /models/{mname} integmethod {INTEGMETHOD}
	end
end

function do_select_model(mname)
	str mname
	if ({strcmp {mname} {old_model_name}} == 0)
		return
	end
	if (IS_RUNNING)
		do_inform "Model is still running. Please stop simulation before changing demos."
		return
	end
	IS_RUNNING = 0
	stop
	reset
	xhide /select
	xshow /edit
	xhide /parmedit/pool
	xhide /parmedit/reac
	xhide /parmedit/enz
	xhide /parmedit/chan
	xhide /parmedit/group
	setfield /select modelname {mname}


	move /kinetics /models/{old_model_name}
	store_current_model_state {old_model_name}

	move /models/{mname} /kinetics
	call /kinetics/notes TO_XTEXT /file/notes
	sh echo "Selecting model: "{mname} > /tmp/comment
	setfield /comment/text filename /tmp/comment
	if ({exists /edit/{old_model_name}})
		xhide /edit/{old_model_name}
	end

	SIMDT = {getfield /kinetics simdt}
	FASTDT = {getfield /kinetics fastdt}
	PLOTDT = {getfield /kinetics plotdt}
	CONTROLDT = {getfield /kinetics controldt}
	VARIABLE_DT_FLAG = {getfield /kinetics variable_dt_flag}
	TRANSIENT_TIME = {getfield /kinetics transient_time}
	DEFAULT_VOL = {getfield /kinetics default_vol}
	setfield /editcontrol/vol value {DEFAULT_VOL}
	setfield /editcontrol/newvol value {DEFAULT_VOL}
	// setfield /file/notes initialtext {getfield /kinetics notes}
	USE_AUTOMATIC_DT = {getfield /kinetics use_automatic_dt}
	INTEGMETHOD = {getfield /kinetics integmethod}
	// do_set_integ_method /simctl/method{INTEGMETHOD}

	do_set_simdt {SIMDT}
	setclock {PLOTCLOCK} {PLOTDT}
	setclock {CONTROLCLOCK} {CONTROLDT}
	do_simctl_update
	
	set_disp_gif 1
	xshow /edit/{mname}
	old_model_name = mname
	setfield /parmedit/group elmpath "/kinetics"
	set_disp_path

	str plot
	str name
	foreach plot ({el /#graphs/conc#/#[TYPE=xplot]})
		delete {plot}
	end
	foreach plot ({el /modelplots/{mname}/plot[]}) 
		name = {getfield {plot} path}
		create xplot {name}
		addmsg {getfield {plot} msgsrc} {name} PLOT Co *{getpath {name} -tail} *{getfield {plot} fg}
		setfield {name} do_slope {getfield {plot} do_slope}
		setfield {name}/.. xmin 0 ymin 0 ymax {getfield {plot} ymax}
		useclock {name} {PLOTCLOCK}
	end
	// Set off simulation here
	do_set_all_graph_field overlay 0
	setfield /simcontrol/overlay state 0
	setfield /plotcontrol/plotoverlay state 0
	reset
	start_stimulus_run
end

function repackage_graphs(mname)
	str mname

	str plot
	str plotname
	int i = 0

	create neutral /modelplots/{mname}
	foreach plot ({el /#graphs/conc#/#[TYPE=xplot]})
		create neutral /modelplots/{mname}/plot[{i}]
		pushe /modelplots/{mname}/plot[{i}]
		i = i + 1
		addfield . path
		setfield . path {plot}
		addfield . msgsrc
		setfield . msgsrc {getmsg {plot} -incoming -source 0}
		addfield . do_slope
		setfield . do_slope {getfield {plot} do_slope}
		addfield . fg
		setfield . fg {getfield {plot} fg}
		addfield . ymax
		setfield . ymax {getfield {plot}/.. ymax}
		pope
		delete {plot}
	end
end

function repackage(modelfile)
	int len = {strlen {modelfile}}
	str mname

	mname = {substring {modelfile} 0 {len - 3}}

	read_stimulus {mname}
	repackage_graphs {mname}

	if (!{exists /kinetics/notes})
		create text /kinetics/notes
	end
	call /kinetics/notes FROM_XTEXT /file/notes
	move /kinetics /models/{mname}
	addfield /models/{mname} fastdt
	addfield /models/{mname} simdt
	addfield /models/{mname} controldt
	addfield /models/{mname} plotdt
	addfield /models/{mname} variable_dt_flag
	addfield /models/{mname} transient_time
	addfield /models/{mname} default_vol
	addfield /models/{mname} use_automatic_dt
	addfield /models/{mname} integmethod

	store_current_model_state {mname}

	create neutral /kinetics
	// create xform /edit/{mname} -nested [0,0:zoom,100%,0:bottom]
	create xform /edit/{mname} [0,0:zoom,100%,0:NULL.bottom] -nested
	create ximage /edit/{mname}/image
	if ({fileexists models/{mname}.gif})
		setfield /edit/{mname}/image filename "models/"{mname}".gif"
	else
		setfield /edit/{mname}/image filename "models/nogiffound.gif"
	end
	int w = {getfield /edit/{mname}/image wgeom}
	int h = {getfield /edit/{mname}/image hgeom}
	//setfield /edit/{mname}/image xgeom {(EDW - w)/2} ygeom {(EDH - h)/2}
	xhide /edit/{mname}
end

function make_xselect_model
	str modelname
	str mname
	int len
	int ht = 30 + WMH + BORDER * 4
	create xform /select [1,{TB},{EW},70]
	ce /select
		create xlabel /select/label -bg cyan -title "Select a model"
		foreach modelname ({el /models/#})
			mname = {getpath {modelname} -tail}
			create xbutton {mname} -script "do_select_model "{mname}
			ht = ht + 25
		end
	setfield /select hgeom {ht}
	create xbutton Cancel -label "Hide" -script "xhide /select"
	ce /
	disable /select
	addfield /select modelname
	setfield /select modelname "nil"
	xshow /select
	create xform /comment [{EW},{GH + WMH},{EW},{360 - WMH}]
	create xtext /comment/text [0,0,100%,100%] \
		-initialtext "Please select a model to run"
	xshow /comment
end
