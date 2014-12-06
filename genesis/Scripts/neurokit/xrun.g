//genesis

float dt = user_dt
float refresh_factor = user_refresh
int i
int col1 = 15
int col2 = 15

// we need a synactivator to activate synaptic channels on
// command.

include synactivator.g

function do_xcell_run_control
	for (i = 0; i < 10; i = i + 1)
		setclock {i} {dt}
	end
	setclock 9 {dt*refresh_factor}

	create x1form /cell_run_control [0,50,380,500] -nolabel
	ce ^
	disable .
	create x1label "SIMULATION CONTROL PANEL" [0,0,100%,40]
	create x1button stop [2%,45,30%,] -script finish_run
	create x1button reset [7:stop.right,45,30%,] -script reset
	create x1button run [7:reset.right,45,30%,] -script runsteps
	create x1button save [2%,72,30%,] -script save_sim
	create x1button restore [7:save.right,72,30%,] -script  \
	    restore_sim
	create x1button "run paradigm" [7:restore.right,72,30%,]  \
	    -script do_paradigm
	create x1dialog nstep -script "nstep "<widget> -value 10000
	create x1dialog runtime [8,5:nstep.bottom,64%,30] -script  \
	    "runtime "<widget> -value 0.1
	create x1dialog method [4:runtime.right,5:nstep.bottom,30%,30]  \
	    -script "intmethod "<widget> -value 0
	setfield /cell_run_control/method value {user_intmethod}

	/* If we change the clock used by a script_out element to one with
	** another period, we can get commands executed different rates. 
	** Here, clock 1 starts up at 1/10 the simulation clock rate so
	** the script is executed every 10 timesteps */

	create script_out /output/refresh_out
	create disk_out /output/field_out
	disable /output/field_out
	create asc_file /output/field_aout
	disable /output/field_aout
	create disk_out /output/plot_out
	disable /output/plot_out
	setfield /output/refresh_out command get_current_time
	useclock /output/refresh_out 9
	useclock /output/field_out 9
	useclock /output/field_aout 9
	useclock /output/plot_out 9

	create x1label /cell_run_control/"TIMING (sec)"
	create x1dialog current_time -script get_current_time -value  \
	    0.000
	create x1dialog clock [2%,5:current_time.bottom,50%,30] -value  \
	    {dt} -script "setdt <widget> "
	create x1dialog refresh_factor  \
	    [7:clock.right,5:current_time.bottom,44%,30] -script  \
	    "setrefresh <widget>" -value {refresh_factor}

	setfield runtime value {user_runtime}
	setfield nstep value {user_runtime/user_dt}

	create x1label "ELECTROPHYSIOLOGY"

	create x1toggle Record [0,4:ELECTROPHYSIOLOGY.bottom,16%,]  \
	    -script "do_run_clickmode "<widget>
	create x1toggle Iclamp [16%,4:ELECTROPHYSIOLOGY.bottom,16%,]  \
	    -script "do_run_clickmode "<widget>
	create x1toggle Vclamp [32%,4:ELECTROPHYSIOLOGY.bottom,16%,]  \
	    -script "do_run_clickmode "<widget>
	create x1toggle "Syn act" [48%,4:ELECTROPHYSIOLOGY.bottom,17%,] \
	     -script "do_run_clickmode "<widget>
	create x1toggle "Syn spike"  \
	    [65%,4:ELECTROPHYSIOLOGY.bottom,18%,] -script  \
	    "do_run_clickmode "<widget>
	create x1toggle "Syn rand"  \
	    [83%,4:ELECTROPHYSIOLOGY.bottom,17%,] -script  \
	    "do_run_clickmode "<widget>

	create neutral /stimulus

	// the rand element holds the rate and weight information
	// for synaptic spike input.  The rate and weight fields are
	// aliases for the y and z fields of the element

	create neutral /stimulus/rand
	addfield /stimulus/rand rate -indirect . y
	addfield /stimulus/rand weight -indirect . z
		setfield ^ rate {user_rate} weight {user_weight}

	// The spike_on_command element is used to activate
	// synaptic channels directly when the user clicks on
	// a compartment.  We will call the SEND_SPIKE action
	// to send the spike passing the channel path as an
	// argument.  The weight is determined by the weight field
	// which is set by the user through a dialog.

	// The synactivator object is defined in synactivator.g
	// which is included above.

	create synactivator /stimulus/spike_on_command
		setfield ^ weight {user_spike}

	create x1label dummy_dialog -label "Plant or remove recording electrodes" \
	    [2%,29:ELECTROPHYSIOLOGY.bottom,96%,30]           

	create x1dialog "inject (nanoAmps)"  \
	    [2%,29:ELECTROPHYSIOLOGY.bottom,96%,30] -script  \
	    "update_inject "<widget>
	setfield "inject (nanoAmps)" value {user_inject}

	create x1dialog "Clamp voltage (mV)"  \
	    [2%,29:ELECTROPHYSIOLOGY.bottom,96%,30] -script  \
	    "set_vclamp "<widget>
	setfield "Clamp voltage (mV)" value {user_clamp}

	create x1dialog "Spike rate (Hz)"  \
	    [2%,29:ELECTROPHYSIOLOGY.bottom,48%,30] -script  \
	    "set_rand_rate "<widget>
	setfield "Spike rate (Hz)" value {user_rate}

	create x1dialog "Spike weight"  \
	    [50%,29:ELECTROPHYSIOLOGY.bottom,48%,30] -script  \
	    "set_rand_wt "<widget>
	setfield "Spike weight" value {user_weight}

	create x1dialog "Amount of synaptic activation"  \
	    [2%,29:ELECTROPHYSIOLOGY.bottom,96%,30] -script  \
	    "set_act_dummy "<widget>
	setfield "Amount of synaptic activation" value {user_activ}

	create x1dialog "Weight of spike"  \
	    [2%,29:ELECTROPHYSIOLOGY.bottom,96%,30] -script \
	    "set_synspike_wt <widget>"
	setfield "Weight of spike" value {user_spike}

	create x1dialog click_site1 -script "click_site 1" -value none
	create x1dialog click_site2 -script "click_site 2" -value none
	create x1button {user_syntype1} [2%,3:click_site2.bottom,15%,30]  \
	    -script set_syn_type1
	create x1button {user_syntype2} [18%,3:click_site2.bottom,15%,30] \
	     -script set_syn_type2
	create x1dialog "Syn Type" [34%,3:click_site2.bottom,64%,30]  \
	    -value {user_syntype1}
	create x1button "Remove all electrophysiology probes" -script  \
	    remplot
	create x1toggle "FILE IO" [0,,50%,] -script  \
	    "popup_file_io "<widget>
		setfield ^ label0 "Show Simulation IO control"  \
		    label1 "Hide Simulation IO control"
	create x1toggle toggle_xout2 [50%,0:"Remove all electrophysiology probes",50%,] -script  \
	    "show_xout2 "<widget>
		setfield ^ label0 "Show extra cell window"  \
		    label1 "Hide extra cell window"

	create x1form /file_IO [0,555,380,350] -nolabel
	ce ^
	disable .
	create x1dialog filepath
	setfield filepath value {user_filepath}
	create x1dialog save_restore_file
	setfield save_restore_file value {user_restorefile}
	create x1dialog file_index -value 0

	create x1toggle save_run_info
		setfield ^ label1 "Save run info"  \
		    label0 "Do Not Save run info"
	create x1dialog info_file -value test
	create x1dialog notes -value " "

	create x1toggle save_plot
		setfield ^ label1 "Save Plots (graph1)"  \
		    label0 "Do Not Save Plots (graph1)"
	create x1dialog plot_file -value testplot

	create x1toggle save_field [2%,4:plot_file.bottom,46%,]
		setfield ^ label1 "Save Field"  \
		    label0 "Do Not Save Field"
	create x1toggle save_type [52%,4:plot_file.bottom,46%,]
		setfield ^ label1 "Ascii" label0 "Binary"
	create x1dialog field
	setfield field value {user_filefield}
	create x1dialog field_path
	setfield field_path value {cellpath}/{user_filefieldpath}
	create x1dialog field_file
	setfield field_file value {user_filename}
end

function get_current_time
	setfield /cell_run_control/current_time value {getstat -time}
end

function nstep(widget)
str widget
	float nsteps = ({getfield {widget} value})
	setfield /cell_run_control/runtime value {nsteps*dt}
end

function runtime(widget)
str widget
	float runtime = ({getfield {widget} value})
	int nsteps

	nsteps = runtime/dt + 0.5
	setfield /cell_run_control/nstep value {nsteps}
end

function intmethod(widget)
str widget
    int imethod = ({getfield {widget} value})
    if ((imethod) == 10)
	if (!({exists {cellpath}/solve}))
            create hsolve {cellpath}/solve
            if ({user_symcomps})
                setfield {cellpath}/solve path {cellpath}/##[][TYPE=symcompartment]
            else
                setfield {cellpath}/solve path {cellpath}/##[][TYPE=compartment]
            end
            setfield {cellpath}/solve chanmode 1
            setmethod {cellpath}/solve 10
	    call {cellpath}/solve SETUP
	    echo Warning : the previous integration scheme  \
	    will still apply for non-hsolvable elements
            echo Note : a reset may be necessary
            reset
	else
            setmethod {cellpath}/solve 10
	end
    end
    if ((imethod) == 11)
	if (!({exists {cellpath}/solve}))
            create hsolve {cellpath}/solve
            if ({user_symcomps})
                setfield {cellpath}/solve path {cellpath}/##[][TYPE=symcompartment]
            else
                setfield {cellpath}/solve path {cellpath}/##[][TYPE=compartment]
            end
            setfield {cellpath}/solve chanmode 1
            setmethod {cellpath}/solve 11
	    call {cellpath}/solve SETUP
	    echo Warning : the previous integration scheme  \
	    will still apply for non-hsolvable elements
            echo Note : a reset may be necessary
            reset
	else
            setmethod {cellpath}/solve 11
	end
    end

	if ((imethod) < -1)
		echo illegal entry, setting to exponential method
		imethod = 0
		setfield /cell_run_control/method value 0
	end
	if (((imethod) != 10) && ((imethod) != 11))
		if (({exists {cellpath}/solve}))
			delete {cellpath}/solve
			echo reset may be necessary
		end
		if ((imethod) > 7)
			echo illegal entry, setting to exponential  \
			    method
		end
	end

	setmethod {cellpath}/## {imethod}
	/*
**	This is what we ought to do, but method fields are not universal
	if ({user_symcomps})
		set {cellpath}/##[][TYPE=symcompartment] method {imethod}
	else
		set {cellpath}/##[][TYPE=compartment] method {imethod}
	end
*/
end

function setrefresh(widget)
str widget
	echo setting refresh_factor to {getfield {widget} value}
	refresh_factor = ({getfield {widget} value})
	//sec : for the refresh views
	setclock 9 {({getclock 0})*refresh_factor}
end

function showtoggle(widget, form)
str widget
str form
	echo {widget} {form}
	if (({getfield {widget} state}) == 1)
		xhide {form}
	else
		xshow {form}
	end
end

function do_nothing// a dummy function

end

function setdt(widget)
str widget
str cell
int clockno, nsteps

	dt = {getfield {widget} value}
	echo setting {cell} timesteps to {dt}
	echo a reset may be necessary.
	setclock {clockno} {dt} // sec
	if (clockno == 0)
		setclock 9 {dt*refresh_factor}
	end
	nsteps = ({getfield /cell_run_control/runtime value}/dt + 0.5)
	setfield /cell_run_control/nstep value {nsteps}
end

function run_func
	source cellparms.g
end

/*
function run_para
	set dummy_para script {get(paradigm_file,value)}


	if (strcmp({get(paradigm_file,value)},"cellparms") == 0)
		cellparms
		return
	end
	if (strcmp({get(paradigm_file,value)},"para1") == 0)
		para1
		return
	end
	if (strcmp({get(paradigm_file,value)},"para2") == 0)
		para2
		return
	end
	// Default op is to step as many as specified
	nstep /cell_run_control/nstep
end
*/

str info_file
str plot_file
str field_file

function start_run
	pushe /file_IO

	info_file = ({getfield filepath value}) @ "/" @ ({getfield info_file value}) @ ({getfield file_index value})
	plot_file = ({getfield filepath value}) @ "/" @ ({getfield plot_file value}) @ ({getfield file_index value})
	field_file = ({getfield filepath value}) @ "/" @ ({getfield field_file value}) @ ({getfield file_index value})

	if (({getfield save_run_info state}) == 1)
		echo saving run info in {info_file}
		openfile {info_file} "a"
		writefile {info_file}
		writefile {info_file} {getdate}
		/*
		writefile {info_file} paradigm : {get(paradigm_file,value)}
		*/
		writefile {info_file} Cell input res =  \
		    {calcRm {cellpath}/soma}
		writefile {info_file} Approx Cell cap =  \
		    {calcCm {cellpath}}
		writefile {info_file} notes : {getfield notes value}
		writefile {info_file}
    	if (({getfield save_plot state}) == 1)
        	writefile {info_file} storing plots in {plot_file}
    	end

    	if (({getfield save_field state}) == 1)
        	writefile {info_file} storing field  \
        	    '{getfield field value}' in {field_file}
    	end
	end

	/* \
	     This makes the data go to a new file with the new header info.
	** It should overwrite existing files with the same name, so
	** beware ! */
	if (({getfield save_plot state}) == 1)
		enable /output/plot_out
		setfield /output/plot_out filename {plot_file}  \
		    initialize 1
	end

	if (({getfield save_field state}) == 1)
		if (({getfield save_type state}) == 1)
			enable /output/field_aout
			setfield /output/field_aout  \
			    filename {field_file} initialize 1
			call /output/field_aout RESET
		else
			enable /output/field_out
			setfield /output/field_out filename {field_file} \
			     initialize 1
			call /output/field_out RESET
		end
	end

	pope
end

function make_output_messages
	str name
	str field
	str src
	int nmsgs
	int i

	pushe /file_IO
	if (({getfield save_plot state}) == 1)

		setfield /output/plot_out filename {plot_file} append 1  \
		    leave_open 1
		nmsgs = {getmsg {cellpath}graph1/graph -incoming -count}

		for (i = 0; i < nmsgs; i = i + 1)
			src = {getmsg {cellpath}graph1/graph -incoming -source {i}}
			addmsg {src} /output/plot_out SAVE Vm
		end
		// look at all messages going to the graph, and send them to
		// a file as well

		// openfile {plot_file} "a"
	end

	if (({getfield save_field state}) == 1)
		field = ({getfield field value})
		if (({getfield save_type state}) == 1)
			setfield /output/field_aout  \
			    filename {field_file} append 1 leave_open 1  \
			    flush 1
		else
			setfield /output/field_out filename {field_file} \
			     append 1 leave_open 1 flush 1
		end

		foreach name ({el {getfield field_path value}})
			if (({getfield save_type state}) == 1)
				addmsg {name} /output/field_aout SAVE  \
				    {field}
			else
				addmsg {name} /output/field_out SAVE  \
				    {field}
			end
		end
		reset
	end
	pope
end

function finish_run
	pushe /file_IO

	stop

	if (({getfield save_run_info state}) == 1)
		closefile {info_file}
	end
	pope
	disable /output/field_out
	disable /output/field_aout
	disable /output/plot_out
end

function clean_output_messages
	int nmsgs, i
	pushe /file_IO
	/* Getting rid of the messages */
	if (({getfield save_plot state}) == 1)
		nmsgs = {getmsg /output/plot_out -incoming -count}
		for (i = 0; i < nmsgs; i = i + 1)
			deletemsg /output/plot_out  0 -incoming
		end
	end

	if (({getfield save_field state}) == 1)
		if (({getfield save_type state}) == 1)
			nmsgs = {getmsg /output/field_aout -incoming -count}
			for (i = 0; i < nmsgs; i = i + 1)
				deletemsg /output/field_aout  0  \
				    -incoming
			end
		else
			nmsgs = {getmsg /output/field_out -incoming -count}
			for (i = 0; i < nmsgs; i = i + 1)
				deletemsg /output/field_out  0 -incoming
			end
		end
	end
	pope
end

function do_dummy
	click_site 1
end

function set_rand_rate(widget)
	setfield /stimulus/rand rate {getfield {widget} value}
end

function set_rand_wt(widget)
	setfield /stimulus/rand weight {getfield {widget} value}
end

function set_synspike_wt(widget)
	setfield /stimulus/spike_on_command weight {getfield {widget} value}
end

function do_run_clickmode(widget)
	str widget

	str widgname = {getfield {widget} name}
	int n
	str num, draww

	ce /cell_run_control
	if (({getfield Record state}) == 1)
		setfield Record state 0
	end
	if (({getfield Iclamp state}) == 1)
		setfield Iclamp state 0
	end
	if (({getfield Vclamp state}) == 1)
		setfield Vclamp state 0
	end
	if (({getfield "Syn act" state}) == 1)
		setfield "Syn act" state 0
	end
	if (({getfield "Syn spike" state}) == 1)
		setfield "Syn spike" state 0
	end
	if (({getfield "Syn rand" state}) == 1)
		setfield "Syn rand" state 0
	end
	setfield {widget} state 1

	for (n = 1; n <= 2; n = n + 1)
		num = n
		draww = (cellpath) @ "xout" @ (num) @ "/draw"
		if ({strcmp {widgname} "Record"} == 0)
			select_mouse {draww} add_plot "add_plot "{num}  \
			    drop_plot "drop_plot "{num} dummy do_dummy
			xraise  dummy_dialog
		end
		if ({strcmp {widgname} "Iclamp"} == 0)
			select_mouse {draww} inject "do_inject "{num}  \
			    unject "clear_inject "{num} dummy do_dummy
			xraise "inject (nanoAmps)"
		end
		if ({strcmp {widgname} "Vclamp"} == 0)
			select_mouse {draww} Vclamp "do_vclamp "{num}  \
			    unclamp "clear_vclamp "{num} dummy do_dummy
			xraise "Clamp voltage (mV)"
		end
		if ({strcmp {widgname} "Syn act"} == 0)
			xraise "Amount of synaptic activation"
			select_mouse {draww} SynAct "do_act "{num} UnAct \
			     "do_unact "{num} dummy do_dummy
		end
		if ({strcmp {widgname} "Syn spike"} == 0)
			xraise /cell_run_control/"Weight of spike"
			select_mouse {draww} SynSpike "do_spike "{num}  \
			    UnSpike "do_unspike "{num} dummy do_dummy
		end
		if ({strcmp {widgname} "Syn rand"} == 0)
			xraise /cell_run_control/"Spike rate (Hz)"
			xraise /cell_run_control/"Spike weight"
			select_mouse {draww} RandSyn "do_rand "{num}  \
			    UnRand "do_unrand "{num} dummy do_dummy
		end
	end
end


function set_act_dummy(widget)
	str widget

	int n
	float act
	str compt, channame, iname, num

	for (n = 1; n <= user_numxouts; n = n + 1)
		num = n
		compt = {getfield {cellpath}xout{num}/draw value}
		channame = {getfield /cell_run_control/"Syn Type" value}
		iname = {getfield {compt} name} @ {getfield {compt} index}
		act = {getfield {widget} value}
		if ( \
		    {exists {cellpath}xout{num}/draw/act{channame}{iname}/dummy} \
		    )
		 setfield  \
		     {cellpath}xout{num}/draw/act{channame}{iname}/dummy \
		      z {act}
		end
	end
end

function remplot
	int nmsgs, i, n
	str name, num

	for (n = 1; n <= user_numxouts; n = n + 1)
		num = n
		nmsgs = {getmsg {cellpath}graph{num}/graph -incoming -count}
		for (i = 0; i < nmsgs; i = i + 1)
			deletemsg {cellpath}graph{num}/graph  0  \
			    -incoming
		end
		foreach name ({el {cellpath}xout{num}/draw/Tr#})
			delete {name}
		end
		foreach name ({el {cellpath}xout{num}/draw/Inj#})
			delete {name}
		end
		foreach name ({el {cellpath}/#[]/Vclamp#})
			delete {name}
		end
		foreach name ({el {cellpath}xout{num}/draw/rand#})
			delete {name}
		end
		// This seems to be wrong! ---dhb
		//delete /stimulus/axon:0
		foreach name ({el {cellpath}xout{num}/draw/act#})
			delete {name}
		end
		xupdate {cellpath}xout{num}/draw
	end
	if ((user_symcomps))
		foreach name ({el {cellpath}/##[TYPE=symcompartment]})
			setfield {name} inject 0.0
		end
	else
		foreach name ({el {cellpath}/##[TYPE=compartment]})
			setfield {name} inject 0.0
		end
	end
	echo removed all electrophysiology probes
end

function update_inject(widget)
	str widget

	if (({strcmp {lastinjectsite1} "none"}) != 0)
	    setfield {lastinjectsite1}  \
	        inject {{getfield {widget} value}/1.0e9}
	    echo  \
	        "setting injection on "{lastinjectsite1}" to "{getfield {widget} value}" nA"
	end
	if (({strcmp {lastinjectsite2} "none"}) != 0)
	    setfield {lastinjectsite2}  \
	        inject {{getfield {widget} value}/1.0e9}
	    echo  \
	        "setting injection on "{lastinjectsite2}" to "{getfield {widget} value}" nA"
	end
end

function set_vclamp(widget)
	str widget
	float temp = {getfield {widget} value}
	str draww, iname

	draww = cellpath @ "xout1/draw"
	if (lastinjectsite1 != "none")
		iname = {getfield {lastinjectsite1} name} @ {getfield {lastinjectsite1} index}
		if ({exists {draww}/Vclamp{iname}})
			temp = temp/1.0e3
			/*
			set {draww}/Vclamp{iname}/Vclamp x {temp}
			*/
			setfield {lastinjectsite1}/Vclamp x {temp}
		end
	end
	draww = cellpath @ "xout2/draw"
	if (lastinjectsite2 != "none")
		iname = {getfield {lastinjectsite2} name} @ {getfield {lastinjectsite2} index}
		if ({exists {draww}/Vclamp{iname}})
			temp = temp/1.0e3
			/*
			set {draww}/Vclamp{iname}/Vclamp x {temp}
			*/
			setfield {lastinjectsite2}/Vclamp x {temp}
		end
	end
end

function runsteps
	echo doing {getfield /cell_run_control/nstep value} steps
	start_run
	make_output_messages
	resched
	step {getfield /cell_run_control/nstep value}
	finish_run
	clean_output_messages
	echo finished {getfield /cell_run_control/nstep value} steps
end

function set_syn_type1
	setfield /cell_run_control/"Syn Type" value {user_syntype1}
end

function set_syn_type2
	setfield /cell_run_control/"Syn Type" value {user_syntype2}
end

function show_xout2(widget)
	str widget

	if (({getfield {widget} state}) == 0)
    	disable {cellpath}graph2
    	disable {cellpath}xout2
    	xhide {cellpath}xout2
    	xhide {cellpath}graph2
		setfield {cellpath}xout1 width 600
		setfield {cellpath}graph1 width 600
		user_numxouts = 1
	else
		setfield {cellpath}xout1  \
		    width {(user_screenwidth - 380)/2}
		setfield {cellpath}graph1  \
		    width {(user_screenwidth - 380)/2}
    	enable {cellpath}graph2
    	enable {cellpath}xout2
    	xshowontop {cellpath}xout2
    	xshowontop {cellpath}graph2
		user_numxouts = 2
	end
end

function popup_file_io(widget)
	str widget
	if (({getfield {widget} state}) == 0)
		xhide /file_IO
	else
		xshowontop /file_IO
	end
end

function save_sim
	str  \
	    file_name = ({getfield /file_IO/save_restore_file value}) @ ({getfield /file_IO/file_index value})

	save {getfield /file_IO/field_path value}  {file_name}

end

function restore_sim
	str  \
	    file_name = ({getfield /file_IO/save_restore_file value}) @ ({getfield /file_IO/file_index value})

	restore  {file_name}
end
