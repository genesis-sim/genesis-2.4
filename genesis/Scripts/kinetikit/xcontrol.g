//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-1997 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/
/* graphics for upi's simulation of second messengers in LTP
*/

int IS_RUNNING = 0

function do_inform
end

// Dummy function, defined later.
function	do_save_all_plots
end

function do_stop
	IS_RUNNING = 0
	stop
end

function do_simple_run
	if (IS_RUNNING == 0)
		return
	end
	setfield /#graphs/#[TYPE=xgraph] xmax {{getstat -time} + MAXTIME}
	if (VARIABLE_DT_FLAG)
		setfield /control/dt label "dt = "{FASTDT}
		setclock {SIMCLOCK} {FASTDT}
		setclock {DUMMYCLOCK} {FASTDT}
		if (MAXTIME > TRANSIENT_TIME)
			step {TRANSIENT_TIME} -time
			setfield /control/dt label "dt = "{SIMDT}
			setclock {SIMCLOCK} {SIMDT}
			setclock {DUMMYCLOCK} {SIMDT}
			if (IS_RUNNING)
				step {MAXTIME - TRANSIENT_TIME} -time
			end
		else
			step {MAXTIME} -time
		end
	else
		setfield /control/dt label "dt = "{SIMDT}
		step {MAXTIME} -time
	end
end

// Dummy function. later overriden in xselectmodel.g
function do_stim_run(modelname)
end

function do_run
	IS_RUNNING = 1
	do_simple_run
end

function start_stimulus_run
	IS_RUNNING = 1
	str modelname
	if ({exists /select})
		modelname = {getfield /select modelname}
		if ({exists /modelstim/{modelname}})
			do_stim_run {modelname}
			return
		end
	end

	do_simple_run
end

function set_runtime
	MAXTIME = {getfield /control/runtime value}
	// setfield /#graphs/#[TYPE=xgraph] xmax {MAXTIME}
	// TABLEDT = MAXTIME/TABLESIZE
	// setclock {TABLECLOCK} {TABLEDT}
end

function update_time
	setfield /control/simtime value {getstat -time}
end

function do_reset
	str name

	/*
	foreach name ({el /kinetics/##[TYPE=xtab]})
		setfield {name} input 0
	end
	*/

	resetfastmsg
 	reset
end

function make_xcontrol
	create xform /control [1,1,{TW},{TH}]
	ce /control
	create xtoggle file [0,0,16%,30] \
		-script "xshow.d /file; xhide.u /file" -label "File"
	create xtoggle edit [0:last,0,17%,30] \
		-script "xshow.d /editcontrol; xhide.u /editcontrol" -label "Units"
	create xtoggle tools [0:last,0,17%,30] \
		-script "xshow.d /toolcontrol; xhide.u /toolcontrol" -label "Tools"
	create xtoggle simctl [0:last,0,17%,30] \
		-script "do_simctl_update.d; xshow.d /simcontrol; xhide.u /simcontrol" \
		-label "Options"
	create xtoggle graph [0:last,0,17%,30] \
		-script "xshow.d /plotcontrol; xhide.u /plotcontrol" -label "Graphs"
	create xtoggle simhelp [0:last,0,16%,30] \
		-script "xshow.d /simhelp; xhide.u /simhelp" \
		-label "Help"
	create xbutton start  [0,0:file,33%,30] \
		-label "Start" \
		-script "do_run" -offbg green
	create xbutton startstim  [0:last,0:file,34%,30] \
		-label "Start stimulus" \
		-script "start_stimulus_run" -offbg lightgreen
	create xbutton stop  [0:last,0:file,17%,30]  -label "Stop" \
		-script "do_stop" -offbg yellow
	create xbutton reset  [0:last,0:file,16%,30] -label "Reset" \
		 -script "do_reset" -offbg red
	create xdialog runtime -title "Runtime (sec)" -value {MAXTIME} \
		-script set_runtime [0,0:start,38%,30]
	create xlabel dt [0:last,0:start,24%,30] -title "dt = "{SIMDT}
	create xdialog simtime -title "Current time" -value 0 \
		[0:last,0:start,38%,30]
	addaction simtime PROCESS update_time
	useclock simtime {CONTROLCLOCK}
	disable /control/runtime
	disable /control/file
	disable /control/tools
	disable /control/graph
	disable /control/start
	disable /control/reset
	disable /control/edit
	disable /control/simctl
	disable /control/simhelp
	disable /control/stop
	// xshow /control
end

function do_clear
	str name
	foreach name ({el /#graphs/#/#[TYPE=xplot]})
		delete name
	end
	delete /kinetics
	create group /kinetics
	call /edit/draw/tree RESET
	reset
end

function do_reinit(path)
	str path

	str pool
	str enz

	do_inform "Warning: all initial concentrations on "{path}" have been changed"

	foreach pool ({el {path}/##[][TYPE=kpool]})
		setfield {pool} CoInit {getfield {pool} Co}
		setfield {pool} nInit {getfield {pool} n}
	end

	foreach enz ({el {path}/##[][TYPE=kenz]})
		setfield {enz} CoComplexInit {getfield {enz} CoComplex}
		setfield {enz} nComplexInit {getfield {enz} nComplex}
	end
end

function make_xfile
	create xform /quit [1,450,500,70]
	ce /quit
	create xlabel label -title "Did you save your work ?"
	create xbutton yes [0,0:label,33%,30] -title "Yes, Quit" -script quit
	create xbutton no [0:last,0:label,34%,30] -title "Oops! Dont quit" \
		-script "xhide /quit; xshow /file"
	create xbutton save [0:last,0:label,33%,30] -title "Save & Quit" \
		-script "do_save; quit"
	create xform /file [1,{TB},500,270]
	addfield /file is_visible -description "flag for visibility"
	setfield /file is_visible 0
	ce /file
	// see file save.g for open_reac and save_sim
	// create xbutton open -label "Open file..." -script "xhide /file; xshow /opencontrol"
	// create xbutton open -label "Select model..." -script "xhide /file; xshow /select"
	/*
	create xdialog open -value dumpreac.g
		setfield open script "load_sim <v>"
	*/
	create xdialog save -label "Save model:"  \
		-value dumpreac.g -script "do_save <v>"
	create xdialog modpath -label "Base path for modules:" \
		-value {BASEPATH}
	addfield modpath old_value -description "Previous value of modpath"
	setfield modpath old_value {BASEPATH}
	create xdialog modsave -label "Modular save:" -value mainmod.g
		setfield modsave script "save_modsim <v>"
	xhide modpath
	xhide modsave
	/*
	*/
	create xlabel notelab -ygeom "0:save" -label "NOTES" -bg cyan
	create xtext notes -hgeom 100 -editable 1
	create xbutton clear  -script do_clear \
		-label "Clear entire simulation"
	create xbutton Quit  -script "xhide /file; xshow /quit"
	create xbutton Cancel -label "Hide" \
		-script "xhide /file; setfield /control/file state 0"
	disable /file
	disable /quit
end

function make_opencontrol
	create xform /opencontrol [50,{TB},400,330]
	pushe /opencontrol
	create xtoggle allow_duplicated_msgs
	create xtoggle no_plots
	create xtoggle no_editcoords
	create xtoggle no_tab_or_stims
	create xtoggle dont_set_plotscales
	create xtoggle dont_set_drawscales
	create xtext instr_text -hgeom 100
	xtextload instr_text \
	"		Instructions:" \
	"In this version of Genesis, the parser cannot load" \
	"files specified in a dialog, so you must type the filename" \
	"at the command prompt and hit 'return' to load" \
	"in a script or dump file. Set the flags above" \
	"to control how the optional portions of a dumpfile" \
	"will be handled."
	create xbutton Cancel -label "Hide" \
		-script "xhide /opencontrol"
	pope
end


function do_set_all_graph_field(field,value)
	setfield /#graphs/#[TYPE=xgraph] {field} {value}
end

function do_delete_all_plots
	str name
	foreach name ({el /#graphs/conc#[]/##[TYPE=xplot] })
		delete {name}
	end
	call /#graphs/conc#[] RESET
end

function plotsave(filename)
	str filename
	do_save_all_plots {filename}
	do_inform "Plots saved in "{filename}
end

function do_convert_plots_to_tabs
	str name
	str src
	str field
	str srcname
	str tabname
	int xdivs = {MAXTIME / PLOTDT}

	if (!{exists /tabs})
		create neutral /tabs
	end

	foreach name ({el /#graphs/conc#[]/##[TYPE=xplot] })
		src = {getmsg {name} -source 0}
		srcname = {getpath {src} -tail}
		field = {getfield {src} plotfield}
		tabname = "/tabs/" @ {srcname} @ "_" @ {field}
		create xtab {tabname}
		setfield {tabname} step_mode 3
		call {tabname} TABCREATE {xdivs} 0 {xdivs}
		addmsg {src} {tabname} INPUT {field}
		useclock {tabname} {PLOTCLOCK}

		delete {name}
	end

	call /#graphs/conc#[] RESET
	call /tabs/# RESET
end

function do_set_enzyme_plotfield(state)
	int state

	if (state == 0) // use CoComplex to plot
		setfield /kinetics/##[][TYPE=kenz] plotfield CoComplex
	else
		setfield /kinetics/##[][TYPE=kenz] plotfield pA
	end
end

function do_plot_use_n(state)
	int state

	float vol = {getfield /editcontrol/vol value}
	float scale = {getfield /editcontrol/conc scalefactor}
	float newvol = NA * vol / scale

	if (state == 0) // use CoComplex to plot
		setfield /#graphs/conc#/##[TYPE=xplot] wy 1
	else
		setfield /#graphs/conc#/##[TYPE=xplot] wy {newvol}
	end

	if (state == 0) // use CoComplex to plot
		// setfield /#graphs/conc#/#.pA wy {1.0/newvol}
		setfield /#graphs/conc#/#.pA wy 1
	else
		setfield /#graphs/conc#/#.pA wy 1
	end
end

function make_plotcontrol
	create xform /plotcontrol [360,{TB},320,410]
	addfield /plotcontrol is_visible -description "flag for visibility"
	setfield /plotcontrol is_visible 0
	ce /plotcontrol
	create xlabel limits -label "Graph Axis Limits" -bg cyan
	create xdialog plotxmax -label "X-axis Max:" -value {MAXTIME} \
		-script "do_set_all_graph_field xmax <v>"
	create xdialog plotxmin -label "X-axis Min:" -value 0 \
		-script "do_set_all_graph_field xmin <v>"
	create xdialog plotymax -label "Y-axis Max:" -value 1 \
		-script "do_set_all_graph_field ymax <v>"
	create xdialog plotymin -label "Y-axis Min:" -value 0 \
		-script "do_set_all_graph_field ymin <v>"
	create xlabel plotdisp -label "Plot Display Options" -bg cyan
	create xtoggle moregraphs -label "Show more graphs" \
		-script "xshow.d /moregraphs; xhide.u /moregraphs"
	create xtoggle plotoverlay -onlabel "Overlay all plots" \
		-offlabel "Do not overlay plots" \
		-script "do_set_all_graph_field overlay <v>"
	create xtoggle enzrate -onlabel "Enzyme plots rate" \
		-offlabel "Enzyme plots CoComplex" \
		-script "do_set_enzyme_plotfield <v>"
	create xtoggle plotfield -onlabel "Plots display n" \
		-offlabel "Plots display Conc" \
		-script "do_plot_use_n <v>"
	create xtoggle disable -offlabel "Plots enabled" \
		-onlabel "Plots disabled" \
		-script "disable.d /graphs; disable.d /moregraphs; enable.u /graphs; enable.u /moregraphs"
	create xbutton delplot -label "Delete all plots" \
		-script "do_delete_all_plots"
	create xlabel plotopt -label "Other Graph Options" -bg cyan
	// create xbutton tabplot -label "Convert all plots to tables" \
		// -script "do_convert_plots_to_tabs"
	create xdialog saveplots -label "Save all plots to file:" \
		-script "plotsave <v>" -value "data.plot"
	create xbutton Cancel -label "Hide" \
		-script "xhide /plotcontrol; setfield /control/graph state 0"
	disable /plotcontrol
end

function do_munge_colors
	str name
	str color
	foreach name ({el /kinetics/##[]})
		if ({exists {name} xtree_fg_req})
			color = {getfield {name} xtree_fg_req}
			if ({strcmp {color} "white"} == 0)
				setfield {name} xtree_fg_req seagreen
			end
		end
	end
	call /edit/draw/tree RESET
end

function do_total_munge_colors
	str name
	str color
	int ncol
	foreach name ({el /kinetics/##[]})
		if ({exists {name} xtree_fg_req})
			ncol = 35 + {rand 0 20}
			setfield {name} xtree_fg_req {ncol}
			setfield {name} xtree_textfg_req black
		end
	end
	call /edit/draw/tree RESET
end

function do_bw_colors
	str name
	str color
	int ncol
	int i

	foreach name ({el /kinetics/##[]})
		if ({exists {name} xtree_fg_req})
			if ({isa pool {name}})
				setfield {name} xtree_fg_req cyan
			else
				setfield {name} xtree_fg_req black
			end
			setfield {name} xtree_textfg_req black
		end
	end
	setdefault kpool xtree_fg_req cyan
	setdefault kreac xtree_fg_req black
	setdefault xtab xtree_fg_req black
	setdefault xtab xtree_textfg_req black
	setdefault group xtree_fg_req black
	setdefault transport xtree_fg_req black
	setdefault kchan xtree_textfg_req black
	setdefault stim xtree_textfg_req black
	setdefault stim xtree_fg_req black

	setfield /graphs/conc1 bg gray90
	setfield /graphs/conc2 bg gray90
	setfield /moregraphs/conc3 bg gray90
	setfield /moregraphs/conc4 bg gray90
	setfield /graphs/conc#/# fg black
	setfield /moregraphs/conc#/# fg black

	setfield /control/#[TYPE=xtoggle] offbg gray90
	setfield /control/#[TYPE=xdialog] bg gray90
	setfield /control/#[TYPE=xlabel] bg gray90
	setfield /control/reset offbg pink

	setfield /parmedit/pool/#[TYPE=xdialog] bg gray90
	setfield /parmedit/pool/#[TYPE=xtoggle] offbg gray90
	setfield /parmedit/pool/#[TYPE=xbutton] offbg gray90
	setfield /parmedit/pool/#[TYPE=xtext] bg gray90

	setfield /edit/draw bg gray90
	setfield /control/lib bg gray90

	for (i = 0; i < 11; i = i + 1)
		setfield /edit/draw/tree msgarrows[{i}].msgfg 0
	end
	setfield /edit/draw/tree msgarrows[20].msgfg 0
	setfield /edit/draw/tree msgarrows[21].msgfg 0
	// setfield /edit/draw/tree msgarrows[12].msgfg 2016
	setfield /edit/draw/tree nmsgtypes 22
	setfield /edit/draw/tree msgarrows[3].msgsrc "/foo"
	// Also need to get rid of pool-to-pool CONSERVE msgs.
	setfield /edit/draw/tree/shape[4] coords \
	[.9,0,0][.9,.8,0][.9,.8,0][.6,.8,0][.6,.8,0][.4,.6,0][.4,.6,0][.2,.8,0][.2,.8,0][-.2,.8,0][-.2,.8,0][-.2,0,0][-.2,0,0][.9,0,0] \
	drawmode DrawSegments \
	textmode draw
	setfield /edit/draw/tree/shape[4] ypts =-=0.4
	setfield /edit/draw/tree/shape[4] xpts =-=0.3
	setfield /edit/draw/tree/shape[4] xpts =*=1.8


	setfield /edit/draw/tree/shape[2] textfont 9x15
	call /edit/draw/tree RESET
	call /control/lib/tree RESET
end

function make_pscontrol
	create xform /pscontrol [80,{TB},300,270]
	addfield /pscontrol is_visible -description "flag for visibility"
	setfield /pscontrol is_visible 0
	ce /pscontrol
	create xdialog maxgray -label "Max grayscale value:" -value 256 \
		-script "setpostscript -maxgray <v>"
	create xdialog mingray -label "Min grayscale value:" -value 0 \
		-script "setpostscript -mingray <v>"
	create xtoggle inverse -offlabel "Output not inverted" \
		-onlabel "Output inverted" \
		-script "setpostscript -inverse <v>"
	create xdialog psfile -label "File for postscript output:" \
		-value "display.ps" \
		-script "setpostscript -filename <v>"
	create xdialog gs -label "Preview file:" -value "display.ps" \
		-script "gs <v>"
	create xbutton munge -label "Replace white with green for output" \
		-script "do_munge_colors"
	create xbutton total_munge -label "Replace all colors for output " \
		-script "do_total_munge_colors"
	create xbutton bw -label "Use B/W for output " \
		-script "do_bw_colors"
	create xbutton Cancel -label "Hide" \
		-script "xhide /pscontrol"
	disable /pscontrol
end


function do_set_controldt(dt)
	float dt
	CONTROLDT = dt
	setclock {CONTROLCLOCK} {CONTROLDT}
end

function do_set_graph_dt(dt)
	float dt
	PLOTDT = dt
	setclock {PLOTCLOCK} {PLOTDT}
end

function do_start_psearch
	if (!{exists /psearch})
		pbegin
	end
	xshow /pedit
end

// Enables/disables variable dt runs.
function do_set_vardt(v)
	int v
	VARIABLE_DT_FLAG = v
	if (v)
		setfield /simcontrol/fastdt fg black
		setfield /simcontrol/transient_time fg black
		setfield /control/start label "start (variable dt)"
		setfield /control/dt label "dt = "{FASTDT}
	else
		setfield /simcontrol/fastdt fg darkgray
		setfield /simcontrol/transient_time fg darkgray
		setfield /control/start label "start"
		setfield /control/dt label "dt = "{SIMDT}
	end
end

function do_simctl_update
	setfield /simcontrol/simdt value {SIMDT}
	setfield /simcontrol/fastdt value {FASTDT}
	setfield /simcontrol/graphdt value {PLOTDT}
	setfield /simcontrol/controldt value {CONTROLDT}
	setfield /simcontrol/vardt state {VARIABLE_DT_FLAG}
	setfield /simcontrol/transient_time value {TRANSIENT_TIME}
	setfield /simcontrol/autodt state {USE_AUTOMATIC_DT}
	setfield /simcontrol/method# state 0
	setfield /simcontrol/method{INTEGMETHOD} state 1
	do_set_vardt {VARIABLE_DT_FLAG}
end

// dummy forward declaration
function find_dt(err, do_display)
	float err
	int do_display
end

// Intended eventually to round to multiples of 2, 5 and 10
function round_nice(x)
	float x

	return {x}
end

function do_auto_dt
	if (USE_AUTOMATIC_DT)
		if (INTEGMETHOD == 0)
			VARIABLE_DT_FLAG = 1
			FASTDT = {find_dt 0.01 0}
			SIMDT = {find_dt 0.05 0}
		end
		if (INTEGMETHOD == 2)
			VARIABLE_DT_FLAG = 1
			FASTDT = {find_dt 0.002 0}
			SIMDT = {find_dt 0.01 0}
		end
		if (INTEGMETHOD == 3 || INTEGMETHOD == 4 || INTEGMETHOD == 5)
			SIMDT = 0.1
			VARIABLE_DT_FLAG = 0
		end
		SIMDT = {round_nice {SIMDT}}
		FASTDT = {round_nice {FASTDT}}

		float faster = PLOTDT
		if (CONTROLDT < PLOTDT)
			faster = CONTROLDT
		end
		if (FASTDT > faster)
			FASTDT = faster
		end
		if (SIMDT > faster)
			SIMDT = faster
		end
		do_simctl_update
	end
end

function do_set_simdt(dt)
	float dt
	SIMDT = dt
	setclock {SIMCLOCK} {SIMDT}
	setclock {DUMMYCLOCK} {SIMDT}
	do_simctl_update
end

function set_disable(state)
	int state
	str name

	if (state)
		foreach name ({el ##[TYPE=kreac],##[TYPE=kenz]})
			disable {name}
		end
		reset
	else
		foreach name ({el ##[TYPE=kreac],##[TYPE=kenz],##[TYPE=kpool]})
			enable {name}
		end
	end
end

function do_set_integ_method(widget)
	str widget
	int len = {strlen {widget}}
	int option = {substring {widget} {len - 1}}
	int i

	if (INTEGMETHOD == option)
		return
	end

	if (DO_X)
		setfield /simcontrol/method# state 0
	end
	INTEGMETHOD = option
	if (DO_X)
		setfield /simcontrol/method{option} state 1
	end

	do_auto_dt

	if ({exists /solve})
		delete /solve
	end

	if (option == 0) // Exp Euler
		// set_disable 0
		setfield /kinetics/##[ISA=reac],/kinetics/##[ISA=enz],/kinetics/##[ISA=pool] stoch_flag 0
	end

	if (option == 1) // Runge Kutta
		do_inform "Runge-Kutta not yet working, using Exp Euler"
		// set_disable 0
		setfield /kinetics/##[ISA=reac],/kinetics/##[ISA=enz],/kinetics/##[ISA=pool] stoch_flag 0
	end

	if (option == 2) // Mixed Stochastic
		// set_disable 0
		setfield /kinetics/##[ISA=reac],/kinetics/##[ISA=enz],/kinetics/##[ISA=pool] stoch_flag 1
	end

	if (option == 3) // Gibson-Bruck
		// set_disable 1
		create gsolve /solve
		setfield /solve method 3
		reset
	end

	if (option == 4) // Gillespie 1
		// set_disable 1
		create gsolve /solve
		setfield /solve method 1
		reset
	end

	if (option == 5) // Gillespie 2
		// set_disable 1
		create gsolve /solve
		setfield /solve method 2
		reset
	end
end

function do_update_poolinfo
// dummy
end

function update_all_pool_realvol
// dummy
end

function update_all_pool_vol
// dummy
end

function do_update_reacinfo
// dummy
end

function do_update_transportinfo
// dummy
end

function do_update_enzinfo
// dummy
end

function update_all_enz_vol
// dummy
end

function select_conc_units(v)
	update_all_pool_realvol
	if (v == 2)
		setfield /editcontrol/mM state 1
		setfield /editcontrol/nM state 0
		setfield /editcontrol/uM state 0
		setfield /editcontrol/conc unitname mM	// mM is SI.
	else 
		setfield /editcontrol/nM state {1 - v}
		setfield /editcontrol/uM state {v}
		setfield /editcontrol/mM state 0
		if (v == 1)
			setfield /editcontrol/conc unitname uM
		else
			setfield /editcontrol/conc unitname nM
		end
	end
	setfield /editcontrol/conc units {v}
	setfield /editcontrol/conc scalefactor {pow 10 {(2 - v) * 3}}
	update_all_pool_vol
	update_all_enz_vol
	do_update_poolinfo
	do_update_enzinfo
	do_update_reacinfo
end

function select_time_units(v)
	setfield /editcontrol/Seconds state {1-v}
	setfield /editcontrol/Minutes state {v}
	setfield /editcontrol/time units {v}
	setfield /editcontrol/time scalefactor {v * 59 + 1} //1 and 60
	if (v)
		setfield /editcontrol/time unitname min
	else
		setfield /editcontrol/time unitname sec
	end
	do_update_enzinfo
	do_update_reacinfo
	do_update_transportinfo
end

// Converts volume in m^3 to dia in um
// calc_dia and calc_vol are used globally so they are found in ksim.g

function set_dia(dia)
	float dia
	setfield /editcontrol/vol value {calc_vol {dia}}
end

function set_vol(vol)
	float vol
	setfield /editcontrol/dia value {calc_dia {vol}}
end

function rescale_pool_vol(scale)
	echo dummy rescale_pool_vol function
end

function rescale_reac_vol(scale)
	echo dummy rescale_reac_vol function
end

function rescale_enz_vol(scale)
	echo dummy rescale_enz_vol function
end

function rescale_vol(vol) 
	float vol

	float scale = vol / {getfield /editcontrol/vol value}
	setfield /editcontrol/newdia value {calc_dia {vol}}
	setfield /editcontrol/dia value {calc_dia {vol}}
	setfield /editcontrol/vol value {vol}
	// select_conc_units 1

	
	rescale_pool_vol {scale}
	rescale_reac_vol {scale}
	rescale_enz_vol {scale}

	DEFAULT_VOL = vol
end

function rescale_dia(dia)
	float dia
	float vol = {calc_vol {dia}}
	float scale = vol / {getfield /editcontrol/vol value}
	echo newvol = {vol}, scale = {scale}

	setfield /editcontrol/newvol value {vol}
	setfield /editcontrol/vol value {vol}
	setfield /editcontrol/dia value {dia}

	rescale_pool_vol {scale}
	rescale_reac_vol {scale}
	rescale_enz_vol {scale}
	// rescale_transport_vol {scale}
	DEFAULT_VOL = vol
end

function make_editcontrol
	create xform /editcontrol [60,{TB},300,320]
	ce /editcontrol
	create xlabel time [0,0:NULL,50%,] -label "Time units" -bg cyan
	addfield time units
	addfield time unitname
	addfield time scalefactor
	create xtoggle Seconds [0,0:last,50%,] -script "select_time_units 0"
	create xtoggle Minutes [0,0:last,50%,] -script "select_time_units 1"
	create xlabel conc [50%,0:NULL,50%,] -label "Concentration units" -bg cyan
	addfield conc units
	addfield conc unitname
	addfield conc scalefactor
	setfield conc scalefactor 1000 // The default conc is uM
	create xtoggle nM [50%,0:last,50%,] -script "select_conc_units 0"
	create xtoggle uM [50%,0:last,50%,] -script "select_conc_units 1"
	create xtoggle mM [50%,0:last,50%,] -script "select_conc_units 2"

	create xlabel l2 -label "Default pool size" -bg cyan
	create xdialog vol -label "Volume (m^3)" \
		-value {DEFAULT_VOL} -script "set_vol <v>"
	create xdialog dia -label "Diameter (um)" \
		-script "set_dia <v>"
	create xlabel l3 -label "Rescale pool size" -bg cyan
	create xdialog newvol -label "New Volume (m^3)" \
		-value {DEFAULT_VOL} -script "rescale_vol <v>"
	create xdialog newdia -label "New Diameter (um)" \
		-script "rescale_dia <v>"


	create xbutton Cancel -label "Hide" \
		-script "xhide /editcontrol; setfield /control/edit state 0"

	ce /

	set_vol {DEFAULT_VOL}
	setfield /editcontrol/newdia value {getfield /editcontrol/dia value}
	select_time_units 0
	select_conc_units 1
end

function fix_pre_kkit8_notes
	str name
	str notes
	foreach name ({el kinetics/##[]})
		if ({exists {name} notes})
			notes = {getfield {name} notes}
			if ({strlen {notes}} > 0)
				setfield {name}/notes str {notes}
				setfield {name} notes ""
			end
		end
	end
end

function snap(v)
	float v

	if (GRID > 0.01)
		int ret
		if (v > 0)
			ret = (v + 0.5)/ GRID;
		else
			ret = (v - 0.5)/ GRID;
		end
		return ret * GRID;
	end

	return {v}
end

function snap_to_grid(v)
	float v
	str name

	GRID = {v}

	foreach name ({el /kinetics/##[]})
		setfield {name} x {snap {getfield {name} x}}
		setfield {name} y {snap {getfield {name} y}}
		setfield {name} z {snap {getfield {name} z}}
	end
	call /edit/draw/tree RESET
end

function do_model_size
	int npool = 0
	int nenz = 0
	int nreac = 0
	int nchan = 0
	int ntab = 0
	int nstim = 0
	int nplot = 0

	str name

	foreach name ({el /kinetics/##[][TYPE=kpool]})
		npool = npool + 1
	end

	foreach name ({el /kinetics/##[][TYPE=kreac]})
		nreac = nreac + 1
	end

	foreach name ({el /kinetics/##[][TYPE=kenz]})
		nenz = nenz + 1
	end

	foreach name ({el /kinetics/##[][TYPE=kchan]})
		nchan = nchan + 1
	end

	foreach name ({el /kinetics/##[][TYPE=tab]})
		ntab = ntab + 1
	end

	foreach name ({el /#graphs/##[][TYPE=xplot]})
		nplot = nplot + 1
	end

	do_inform "Simulation has "{npool}" pools, "{nreac}" reacs, "{nenz}" enzs, "{nchan}" chans, "{ntab}" tabs, "{nplot}" plots"
end

function make_toolcontrol
	create xform /toolcontrol [120,{TB},300,450]
	ce /toolcontrol
	create xbutton ps -label "Postscript..." \
		-script "xhide /toolcontrol; xshow /pscontrol"
	create xbutton doser -label "Dose Response..." \
		-script "xhide /toolcontrol; xshow /dosercontrol"
	create xbutton plot -label "Plot..." \
		-script "xhide /toolcontrol; xshow /plotcontrol"
	create xbutton compare -label "Compare models..." \
		-script "xhide /toolcontrol; xshow /comparecontrol"
	create xbutton diffuse -label "Diffusion..." \
		-script "xhide /toolcontrol; xshow /diffusion"
	create xdialog tabulate -label "Tabulate model parms" \
		-value tabfile \
		-script "xhide /toolcontrol; do_tabulate_model <v>"
	create xdialog eqns -label "Model Equations" \
		-value eqnfile \
		-script "xhide /toolcontrol; do_model_eqns <v>"
	create xbutton complex_copy -label "Copying tool..." \
		-script "xhide /toolcontrol; xshow /copy"
	create xbutton fixnotes \
		-label "Convert pre-kkit8 notes" \
		-script "fix_pre_kkit8_notes"
	create xbutton scalerates -label "Rate scaling tool..." \
		-script "xhide /toolcontrol; xshow /scalerate"
	create xbutton dbdump \
		-label "Dump model to database..." \
		-script "xhide /toolcontrol; show_dumpdb"
	// The show_dumpdb function is in model_to_db.g
	create xdialog grid \
		-label "Snap display to grid:" \
		-script "xhide /toolcontrol; snap_to_grid <v>" \
		-value 1
	create xbutton model_size -label "Calculate model size" \
		-script do_model_size
	create xbutton reinit -label "Reset simulation initial values" \
		-script "do_reinit /kinetics"
	create xlabel dtlab -label "Calculate timesteps for mixed stoch" \
		-bg cyan
	create xbutton finddt1 -wgeom 50% \
		-label "dt for 1% accuracy" -script "find_dt 0.01 1"
	create xbutton finddt2 [0:last,0:dtlab,50%,] \
		-label "dt for 5% accuracy" -script "find_dt 0.05 1"
	create xbutton Cancel -label "Hide" \
		-script "xhide /toolcontrol; setfield /control/tools state 0"
end


// Sets the fastclock for variable dt runs.
function do_set_fastdt(v)
	float v
	FASTDT = v
	do_simctl_update
end

// Sets the fastclock for variable dt runs.
function do_set_transient_time(v)
	float v
	TRANSIENT_TIME = v
end

function set_dt_selection_mode(state)
	int state

	USE_AUTOMATIC_DT = state
	do_auto_dt
end

function make_simcontrol
	create xform /simcontrol [240,{TB},450,520]
	addfield /simcontrol is_visible -description "flag for visibility"
	setfield /simcontrol is_visible 0
	ce /simcontrol
	create xlabel clocklabel -label "Timestep Control (dt in sec)" \
		-bg cyan
	create xdialog simdt [0,0:clocklabel,50%,30] \
		-label "Simulation dt" \
		-value {SIMDT} \
		-script "do_set_simdt <v>"
	create xtoggle autodt [0:last,0:clocklabel,50%,30] \
		-offlabel "Manual dt selection" \
		-onlabel "Automatic dt selection" \
		-script "set_dt_selection_mode <v>"
	create xdialog graphdt [0,0:simdt,50%,] -label "Plot dt" \
		-value {PLOTDT} \
		-script "do_set_graph_dt <v>"
	create xdialog controldt [0:last,0:simdt,50%,] -label "Control panel dt" \
		-value {CONTROLDT} \
		-script "do_set_controldt <v>"
	create xtoggle vardt -offlabel "Uniform dt after transients" \
		-onlabel "Variable dt for transients" \
		-script "do_set_vardt <v>"
	create xdialog fastdt [0,0:vardt,50%,] -label "Transient dt" \
		-value {FASTDT} -fg darkgray \
		-script "do_set_fastdt <v>"
	create xdialog transient_time [0:last,0:vardt,50%,] \
		-label "Settling time" \
		-value {TRANSIENT_TIME} -fg darkgray \
		-script "do_set_transient_time <v>"
	create xlabel reaclabel -label "Reaction Options" -bg cyan
	create xtoggle high_order -onlabel "Higher order reacs enabled" \
		-offlabel "Higher order reacs disabled"

	create xtoggle poolsumtot -offlabel "pool-to-pool uses SUMTOTAL" \
		-onlabel "pool-to-pool uses CONSERVE"

	create xtoggle enzconsv -onlabel "Enz-to-pool uses nComplex for CONSERVE" \
		-offlabel "Normal Enz-to-pool : not CONSERVE"

	create xtoggle enzsumtot -onlabel "Enz-to-pool uses nComplex for SUMTOTAL" \
		-offlabel "Normal Enz-to-pool : not SUMTOTAL"

	create xtoggle enzintramol -onlabel "pool-to-enz uses n for INTRAMOL" \
		-offlabel "Normal pool-to-enz : not INTRAMOL"
	create xtoggle kf -onlabel "pool-to-reac uses Co for kf" \
		-offlabel "Normal pool-to-reac : not KF" \
		-script "setfield /simcontrol/kb state 0"
	create xtoggle kb -onlabel "pool-to-reac uses Co for kb" \
		-offlabel "Normal pool-to-reac : not KB" \
		-script "setfield /simcontrol/kf state 0"
	create xtoggle autocat -onlabel "Autocatalysis Enabled" \
		-offlabel "Autocatalysis Disabled"

	create xlabel methlabel -bg cyan -label "Integration methods"
	create xtoggle method0 -wgeom 50% -label "Exponential Euler" \
		-script "do_set_integ_method <w>" -state 1 -onbg white
	create xtoggle method1 [0:last,0:methlabel,50%,] \
		-label "Runge-Kutta" \
		-script "do_set_integ_method <w>" -onbg white -offfg gray
	create xtoggle method2 -wgeom 50% -label "Mixed Stochastic" \
		-script "do_set_integ_method <w>" -onbg white
	create xtoggle method3 [0:last,0:method0,50%,] \
		-label "Gibson-Bruck" \
		-script "do_set_integ_method <w>" -onbg white
	create xtoggle method4 -wgeom 50% -label "Gillespie 1" \
		-script "do_set_integ_method <w>" -onbg white
	create xtoggle method5 [0:last,0:method2,50%,] \
		-label "Gillespie2" \
		-script "do_set_integ_method <w>" -onbg white

	create xlabel graphs -label "Graph options" -bg cyan
	create xtoggle overlay -onlabel "Overlay plots" \
		-offlabel "Do not overlay plots" \
		-script "do_set_all_graph_field overlay <v>"
	create xbutton Cancel -label "Hide" \
		-script "xhide /simcontrol; setfield /control/simctl state 0"

	disable /simcontrol
end

function make_simhelp
	create xform /simhelp [{EX},0,{EW},{WINHT + 10}]
	create xbutton /simhelp/about -label "About kkit" \
		-script "xshow /about; xhide /simhelp"
	create xtext /simhelp/text -hgeom {WINHT - 60} \
		-filename "Kkit.help"
	create xbutton /simhelp/Cancel -label "Hide" \
		-script "xhide /simhelp; setfield /control/simhelp state 0"
	disable /simhelp
end
