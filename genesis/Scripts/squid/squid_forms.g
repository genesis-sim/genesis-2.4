// genesis

str quit_form = "Do you really want to quit ?"

function add_squid_forms
    pushe

    //-------------------------
    // SIMULATION CONTROL FORM
    //-------------------------
    // button added by Ed Vigmond 11/10/94 to clear graphs 

    create neutral /forms
    disable /forms
    ce /forms

    create xform /forms/control [215,500,290,260] -notitle
    ce /forms/control
    create xlabel clabel [10,5,255,25] -label "Simulation control"
    create xbutton reset [10,45,60,25] -title "RESET" -script  \
        "squid_reset"
    create xbutton run [75,45,60,25] -title "RUN" -script "squid_step"
    create xbutton stop [140,45,60,25] -title "STOP" -script "stop"
    create xbutton quit [205,45,60,25] -title "QUIT" -script popup_quit

    create xdialog tstep [10,80,255,25] -value 15.0 -title  \
        "time (msec)" -script "set_step"
    create xdialog dt [10,110,255,25] -value 0.020 -title "dt (msec)"  \
        -script "set_dt"
    create xbutton clamp_mode [10,140,255,25] -title  \
        "Toggle Vclamp/Iclamp Mode" -script "toggle_clamp_mode"
    create xtoggle overlay [10,170,255,25] -title "" -script  \
        "toggle_overlay "<widget>
    setfield overlay label0 "Overlay OFF" label1 "Overlay ON" state 0

    create xbutton clrgrph [10,200,255,25] -title "Clear Graphs"  \
        -script "cleargraph / "


    // The iclamp form now has a "Base Current Level" and
    // the vclamp form has a Pre-pulse level and time
    //-------------------------
    // CURRENT CLAMP FORM
    //-------------------------
    // Modified  by Ed Vigmond 11/10/93 to allow two different pulses
    // to be applied
    create xform /forms/iclamp [510,500,290,255] -notitle
    ce /forms/iclamp
    create xlabel ilabel [10,,255,25] -label "Current Clamp Mode"
    create xdialog baselevel [10,,255,25] -value 0.0 -title  \
        "Base Current Level (uA)" -script  \
        "dset /pulsegen baselevel "<widget>
    create xdialog level1 [10,,255,25] -value 0.1 -title  \
        "Pulse 1 Current (uA)" -script "dset /pulsegen level1 "<widget>
    create xdialog delay1 [10,,255,25] -value 5 -title  \
        "Onset Delay 1 (msec)" -script "dset /pulsegen delay1 "<widget>
    create xdialog width1 [10,,255,25] -value 40 -title  \
        "Pulse 1 Width (msec)" -script "dset /pulsegen width1 "<widget>
    create xdialog level2 [10,,255,25] -value 0.0 -title  \
        "Pulse 2 Current (uA)" -script "dset /pulsegen level2 "<widget>
    create xdialog delay2 [10,,255,25] -value 0 -title  \
        "Onset Delay 2 (msec)" -script "dset /pulsegen delay2 "<widget>
    create xdialog width2 [10,,255,25] -value 0 -title  \
        "Pulse 2 Width (msec)" -script "dset /pulsegen width2 "<widget>
    create xtoggle trigmode [10,,255,25] -title "" -script  \
        "toggle_trigmode "<widget>
    setfield trigmode label0 "Pulse Train" label1 "Single Pulse" state 1

    // added by Ed Vigmond: 10/10/93
    // form to control external Na and K concentrations

    //-------------------------
    // External Concentration Form
    //-------------------------

    create xform /forms/exconcen [800,500,200,145] -notitle
    ce /forms/exconcen
    create xlabel exlabel [10,5,170,25] -label "External Concentrations"
    create xdialog kout [10,40,170,25] -value 10.0 -title "[K] ( mM )"  \
        -script "concset 1 "<widget>
    create xdialog naout [10,70,170,25] -value 460 -title "[Na] ( mM )" \
         -script "concset 2  "<widget>
    create xbutton defconc [10,100,170,25] -title "Default Values"  \
        -script "concdefault"

    //-------------------------
    // VOLTAGE CLAMP FORM
    //-------------------------

    create xform /forms/vclamp [560,500,290,240] -notitle
    ce /forms/vclamp
    create xlabel vlabel [10,5,255,25] -label "Voltage Clamp Mode"
    create xdialog vhold [10,40,255,25] -value {0.0 + EREST_ACT} -title \
         "Holding Voltage (mV)" -script  \
        "dset /pulsegen baselevel "<widget>
    create xdialog thold [10,70,255,25] -value 10 -title  \
        "Holding Time (msec)" -script "dset /pulsegen delay1 "<widget>

    create xdialog vpre [10,100,255,25] -value {0.0 + EREST_ACT} -title \
         "Pre-pulse Voltage (mV)" -script  \
        "dset /pulsegen level1 "<widget>
    create xdialog tpre [10,130,255,25] -value 0.0 -title  \
        "Pre-pulse Time (msec)" -script "set_tpre /pulsegen "<widget>

    create xdialog vclamp [10,160,255,25] -value {50 + EREST_ACT}  \
        -title "Clamp Voltage (mV)" -script  \
        "dset /pulsegen level2 "<widget>
    create xdialog tclamp [10,190,255,25] -value 20 -title  \
        "Clamp Time (msec)" -script "set_tclamp /pulsegen "<widget>

    //-------------------------
    // GRAPH SCALE FORM
    //-------------------------

    create xform /forms/gscale [300,100,235,235] -title "Graph Scale"
    ce /forms/gscale

    create xdialog xmin [10,45,200,25] -value "???" -script  \
        "gset xmin "<widget>
    create xdialog xmax [10,80,200,25] -value "???" -script  \
        "gset xmax "<widget>
    create xdialog ymin [10,115,200,25] -value "???" -script  \
        "gset ymin "<widget>
    create xdialog ymax [10,150,200,25] -value "???" -script  \
        "gset ymax "<widget>
    create xdialog overlay [10,185,200,25] -value "???" -script  \
        "gset overlay "<widget>
    create xbutton DONE [10,10,50,25] -script "xhide /forms/gscale"

    //--------------------------
    // QUIT FORM
    // and its related functions are taken from NEUROKIT
    //--------------------------
    create xform /{quit_form} [400,270,300,95]
    ce ^
    disable .
    create xlabel quit -label "Quit?"
    create xbutton yes -script quit
    create xbutton no -script popdown_quit

    //----------------------------------------
    // INITIALIZE SOME ADDED FIELDS
    //----------------------------------------

    addfield  /forms/gscale gpath
    addfield  /forms/control clamp_mode
    setfield  /forms/gscale gpath "???"
    setfield  /forms/control clamp_mode "vclamp"

    pope
end

function set_step
    float tstep
    str graph
    tstep = {getfield /forms/control/tstep value}
    echo next step will be for {tstep} msec
    foreach graph ({el /axon/##[TYPE=xgraph]})
	setfield {graph} xmax {tstep}
    end
    // don't forget the HH parameter graph
    setfield /output/phase_plot/act_grf xmax {tstep}
end

function set_dt
    float dt
    dt = {getfield /forms/control/dt value}
    echo dt = {dt} msec
    setclock 0 {dt}
end

function squid_step
    int nstep
    float dt, tstep
    call /forms/control/tstep B1DOWN
    tstep = {getfield /forms/control/tstep value}
    call /forms/control/dt B1DOWN
    dt = {getfield /forms/control/dt value}
    nstep = tstep/dt
    step {nstep}
end

function squid_reset
    // temporary solution (hack) to 
    // accomplish reset scheduling

    reset

    call /axon RESET
    setfield /axon Vm {Vrest_squid}

    call /axon/Na/m RESET
    call /axon/Na/h RESET
    call /axon/Na RESET

    call /axon/K/n RESET
    call /axon/K RESET

    // changes by Ed Vigmond to allow for proper behaviour when
    // external concentrations are changed
    setfield /axon Em {Vleak_squid}
    setfield /pulsegen trig_time 0
end


function toggle_trigmode(widget)
    str widget
    float state = {getfield {widget} state}
    setfield /pulsegen trig_mode {state}
    echo Setting /pulsegen trig_mode {state}
end


function toggle_clamp_mode
    str mode, dbut, graph
    mode = {getfield  /forms/control clamp_mode}
    if (mode == "iclamp")
	// toggle to voltage clamp
	xhide /forms/iclamp
	xshow /forms/vclamp
	setfield  /forms/control clamp_mode "vclamp"

        setfield /forms/control/tstep value 50
	setfield /forms/control/dt value 0.01
	foreach graph ({el /axon/##[TYPE=xgraph]})
	    setfield {graph} xmax 50
	end

	// injection current graph needs to be rescaled
	setfield /axon/graphs/inj ymin -10 ymax 10

	//set-up electronics for voltage clamp
	setfield /Iclamp gain 0
	setfield /Vclamp gain 1
	setfield /PID gain 0.5
	setfield /forms/iclamp/trigmode state 1		//ensure single pulse
	toggle_trigmode /forms/iclamp/trigmode
		
	foreach dbut ({el /forms/vclamp/#[TYPE!=xlabel]})
	    call {dbut} B1DOWN
	end
    else
	// toggle to current clamp
	xhide /forms/vclamp
	xshow /forms/iclamp
	setfield  /forms/control clamp_mode "iclamp"

        setfield /forms/control/tstep value 50
	setfield /forms/control/dt value 0.1
	foreach graph ({el /axon/##[TYPE=xgraph]})
	    setfield {graph} xmax 50
	end

	// injection current graph needs to be rescaled
	setfield /axon/graphs/inj ymin -0.5 ymax 0.5


	//set-up electronics for current clamp
	setfield /Iclamp gain 1
	setfield /Vclamp gain 0
	setfield /PID gain 0
	// restore pulsgen fields set in vclamp mode
	setfield /pulsegen width2 0.0
	setfield /pulsegen level2 0.0
	foreach dbut ({el /forms/iclamp/#[TYPE!=xlabel]})
	    call {dbut} B1DOWN
	end
    end
end

function dset(path, field, dialog)
    float val
    val = {getfield {dialog} value}
    echo Setting {path} {field} {val}
    setfield {path} {field} {val}
end

function gset(field, dialog)
    str path
    float val
    path = {getfield  /forms/gscale gpath}
    val = {getfield {dialog} value}
    echo Setting {path} {field} {val}
    setfield {path} {field} {val}
end

function set_pulse_interval(path, dialog)
    float interval, delay1, delay2
    interval = {getfield {dialog} value}
    delay1 = {getfield {path} delay1}
    delay2 = interval - delay1
    echo Setting {path} delay2 {delay2}
    setfield {path} delay2 {delay2}
end

function set_tpre(path, dialog)
    float width1
    // Pre-pulse time
    width1 = {getfield {dialog} value}
    echo Setting delay2 {width1}
    echo Setting {path} width1 {width1}
    setfield {path} delay2 {width1}
    setfield {path} width1 {width1}
end

function set_tclamp(path, dialog)
    float width1, width2
    width2 = {getfield {dialog} value}
    width1 = {getfield {path} width1}
    echo Setting delay2 {width1}
    echo Setting {path} width2 {width2}
    setfield {path} delay2 {width1}
    setfield {path} width2 {width2}
end

function toggle_overlay(widget)
    str overlay = ({getfield {widget} state})
    setfield /##[TYPE=xgraph] overlay {overlay}
end

//The next 5 functions were added by Ed Vigmond 10/10/93
// set new values for external concentration and accordingly change the 
// Nerst potential for the ion
//	ion = 1 -> K
//	      2 -> Na

function concset(ion, dialog)
    float ion
    float val
    val = {getfield {dialog} value}
	if (ion == 1)
	    K_out_squid = (val)
	    echo Setting K_out_squid {val}
	    VK_squid = ((R*T/F)*1000*{log {K_out_squid/K_in_squid}} + 70.0 + EREST_ACT)
	    echo Setting /axon/K Ek {VK_squid}
	    setfield /axon/K Ek {VK_squid}
	else
	    Na_out_squid = (val)
	    echo Setting Na_out_squid {val}
	    VNa_squid = ((R*T/F)*1000*{log {Na_out_squid/Na_in_squid}} + 70.0 + EREST_ACT)
	    echo Setting /axon/Na Ek {VNa_squid}
	    setfield /axon/Na Ek {VNa_squid}
	end
end

// return external concentrations and Nernst potentials to default values 
//	[Na]_e = 460 mM
//	[K]_e  =  10 mM
function concdefault
    setfield /forms/exconcen/kout value {10.0}
    K_out_squid = (10.0)
    echo Setting K_out_squid 10.0
    VK_squid = ((R*T/F)*(1000)*{log {K_out_squid/K_in_squid}} + 70.0 + EREST_ACT)
    echo Setting /axon/K Ek {VK_squid}
    setfield /axon/K Ek {VK_squid}
    setfield /forms/exconcen/naout value {460.0}
    Na_out_squid = (460)
    echo Setting Na_out_squid 460
    VNa_squid = ((R*T/F)*(1000)*{log {Na_out_squid/Na_in_squid}} + 70.0 + EREST_ACT)
    echo Setting /axon/Na Ek {VNa_squid}
    setfield /axon/Na Ek {VNa_squid}
end


function popdown_quit
	xhide /{quit_form}
end

function popup_quit
	xshowontop /{quit_form}
end

