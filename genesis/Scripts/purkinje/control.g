//genesis
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: control.g 1.14.1.1.2.1.1.1.1.1.1.1.1.2.2.3.1.1.1.14 Mon, 21 Aug 2006 23:40:45 +0200 hugo $
//

//////////////////////////////////////////////////////////////////////////////
//'
//' Purkinje tutorial
//'
//' (C) 1998-2002 BBF-UIA
//' (C) 2005-2006 UTHSCSA
//'
//' functional ideas ... Erik De Schutter, erik@tnb.ua.ac.be
//' genesis coding ..... Hugo Cornelis, hugo.cornelis@gmail.com
//'
//' general feedback ... Reinoud Maex, Erik De Schutter, Dave Beeman, Volker Steuber, Dieter Jaeger, James Bower
//'
//////////////////////////////////////////////////////////////////////////////


// control.g : control panel functionality

int include_control

if ( {include_control} == 0 )

	include_control = 1


include actions.g
include xcell.g


//! I define the vars first before including any scripts because else 
//! genesis starts complaining about syntax errors when functions in the 
//! scripts reference them. 



// include scripts that handle settings and actions

include actions.g
include settings.g


///
/// SH:	CreateRefresh
///
/// PA: path..........:	path for refresh elements (ending in '/')
///	fnRefresh.....:	function to call at each refresh step
///
/// DE:	Create refresh elements
///	Refresh elements use clock 10
///

function CreateRefresh(path,fnRefresh)

str path
str fnRefresh

	//- give diagnostics

	echo "Creating refresh elements for "{path}

	//- create script_out element in given path

	create script_out {path}refresh

	//- set clock for refresh element

	useclock ^ 10

	//- set command field

	setfield ^ \
		command {fnRefresh}
end


///
/// SH:	PurkinjeExit
///
/// DE:	exit from simulation
///

function PurkinjeExit

	//- set simulation status : none

	echo 0 > simulation.status

	//- give diagnostics

	echo "Exiting simulation"

	//- exit genesis

	exit
end


///
/// SH:	PurkinjeReset
///
/// DE:	resets the simulation
///	all compartments and graphs are reset
///

function PurkinjeReset

	//- switch off any pending current clamp

	ActionIClampStop

	//- if current clamp is enabled

	if (iCurrentMode != 0)

		//- start current clamp with registered pulse mode

		ActionIClampStart {getfield \
					/settings/iClamp/pulse \
					state}

	//- else

	else
	end

	//- update the firing frequencies for stellate and parallel fibers

	UpdateFrequencies

// 	//! the single argument call form of silent reports the actual value
// 	//! instead of the previous one.  So I need two calls to unsilence hsolve.

// 	str previous_silent = {silent}

// 	silent -1

	//- reset all

	//t normally all modules should receive their own reset

	reset

// 	silent {previous_silent}

end


///
/// SH:	PurkinjeRun
///
/// DE:	do a run for the amount of time steps as set in the time step dialog
///

function PurkinjeRun

	//- force a button down click on the time dialog

	call /control/time B1DOWN

	//- retrieve time

	float time = {runtime}

	//- determine number of steps to perform

	int steps = time / 1000 / dt

	//- give an informational message

	echo "Running simulation for "{steps}" steps"

	//- do the simulation steps

	step {steps} -background
end


///
/// SH:	SetOutputRate
///
/// PA:	rate..:	new output rate
///
/// DE:	act after change in relative output rate
///	calculates clock 9, set global outputRate
///

function SetOutputRate(rate)

int rate

	//- give a diagnostic message to the user

	echo "Relative output rate changed to "{rate}

	//- set global variable

	outputRate = rate

	//- set output clock

	setclock 9 {dt * outputRate}
end


///
/// SH:	SetRunTime
///
/// PA:	time..:	new time in msec
///
/// DE:	act after change in run time
///	calculates clock 9, set global outputRate
///

function SetRunTime(time)

float time

	//- if given time is not negative

	if ( time > 0 )

		//- give a diagnostic message to the user

		echo "Setting simulation time to "{time}" msec"

		//- set global variable

		runtime = {time}

	//- else

	else
		//- give diagnostics

		echo "You cannot reverse time"

		//- reset the value of the widget

		setfield /control/time \
			value {runtime}
	end
end


///
/// SH:	ClearGraphs
///
/// PA:	path..:	top of hierarchy
///
/// DE:	clear graphs in el and all sub elements
///

function ClearGraphs(path)

str path

	//- clear all graphs

	XGraphClear({path})
end


///
/// SH: ToggleAction
///
/// PA: action:	1 synchronous parallel fiber
///		2 basket axon activation
///		3 climbing fiber activation
///
/// DE:	do a simulation action
///

function ToggleAction(action)

int action

	//- for parallel fiber

	if (action == 1)

		//- fire parallel fibers

		if (bSynchroLocal)

			ActionParallelStart \
				{bSynchroLocal} \
				{synchroBranch} \
				{synchroSynapses} \
				{synchroLocalLevel}
		else
			ActionParallelStart \
				{bSynchroLocal} \
				{synchroBranch} \
				{synchroSynapses} \
				{synchroDistrLevel}
		end

	//- for basket axon

	elif (action == 2)

		// give a discriptive message

		echo "Firing basket axon"

		//- fire basket axon

		ActionBasketStart {basketLevel}

	//- for climbing fiber

	elif (action == 3)

		//- give a discriptive message

		echo "Firing climbing fiber"

		//- fire climbing fiber

		ActionClimbingStart {strength}

	//- else we have an unkown action

	else

		//- give a very discriptive message

		echo "Somebody has messed up the code"
	end
end


///
/// SH: ToggleMode
///
/// PA: mode..:	1 change in vivo / vitro mode
///		2 change in current injection
///
/// DE:	toggle the simulation mode
///	reflects changes in the toggle buttons
///	calls UpdateToMode() to update the displayed forms
///	to reflect the simulation mode
///

function ToggleMode(mode)

int mode

	//- if we have a change in vivo / vitro

	if (mode == 1)

		//- for vivo mode

		if (iVVMode == 1)

			//- give a discriptive message

			echo "Switching to vitro mode"

			//- set the control panel label

			setfield /control/vivoVitroToggle \
				title "In vitro"

			//- show the no settings label

			xshow /control/noVitroSettings

			//- hide the settings button

			xhide /control/vivoSettings

			//- go to vitro mode

			iVVMode = 0

			//- perform the switch in frequencies

			UpdateFrequencies

		//- else (for vitro mode)

		else

			//- give a discriptive message

			echo "Switching to vivo mode"

			//- set the control panel label

			setfield /control/vivoVitroToggle \
				title "In vivo"

			//- hide the no settings label

			xhide /control/noVitroSettings

			//- show the settings button

			xshow /control/vivoSettings

			//- go to vivo mode

			iVVMode = 1

			//- perform the switch in frequencies

			UpdateFrequencies

		end

	//- else if we have a change in current injection

	elif (mode == 2)

		//- if we are in current injection mode

		if (iCurrentMode == 1)

			//- give a discriptive message

			echo "Switching off current injection"

			//- switch off current mode

			iCurrentMode = 0

			//- switch off pulse generator

			//! still interface with settings needed

			ActionIClampStop {getfield \
						/settings/iClamp/pulse \
						state}

		//- else (we are not in current injection mode)

		else

			//- give a discriptive message

			echo "Switching on current injection"

			//- switch on current mode

			iCurrentMode = 1

			//- switch on pulse generator

			//! still interface with settings needed

			ActionIClampStart {getfield \
						/settings/iClamp/pulse \
						state}
		end

	//- else no mode is possible

	else

		//- give a very discriptive message

		echo "Somebody has messed up the code"
	end
end


///
/// SH:	ControlRefresh
///
/// DE:	Refresh the control panel
///	Does update for simulation time widget in the control panel
///

function ControlRefresh

	//- set new label

	setfield /control/simulationTimeOutput \
		title {substring {getstat -time}".000" 0 4}
end


///
/// SH:	CurrentTimeSwitch
///
/// DE:	Switch simulation time display on/off
///

function CurrentTimeSwitch

	//- if simulation time was off

	if ({getfield /control/simulationTime state} == 0)

		//- give diagnostics

		echo "Showing simulation time"

		//- enable refresh action

		setfield /control/refresh \
			command "ControlRefresh"

		//- do RESET on refresh element

		call /control/refresh RESET

		//- set new state

		setfield /control/simulationTime \
			state 1

	//- else

	else
		//- give diagnostics

		echo "Hiding simulation time"

		//- set label for simulation time

		setfield /control/simulationTimeOutput \
			title "off"

		//- disable refresh

		setfield /control/refresh \
			command ""

		//- do RESET on refresh element

		call /control/refresh RESET

		//- set new state

		setfield /control/simulationTime \
			state 0
	end
end


///
/// SH:	ControlPanelCreate
///
/// DE:	create the control panel with all buttons and dialogs
///	All settings must already be created
///

function ControlPanelCreate

	//- create form for control panel

	create xform /control [0, 520, 300, 360]

	//! Xodus contains a couple of bugs regarding layout of
	//! widgets using reference edges.  The offset logic is a workaround,
	//! but is specific to this control panel.

	int offset = 0

	//- make it the current element

	pushe /control

	//- create a label with a header

	create xlabel heading \
		-label "Simulation control"

	//- create the reset button

	create xbutton reset \
		-ygeom 0:heading \
		-wgeom 25% \
		-title "RESET" \
		-script "PurkinjeReset"

	//- create the run button

	create xbutton run \
		-xgeom 0:reset \
		-ygeom 0:heading \
		-wgeom 25% \
		-title "RUN" \
		-script "PurkinjeRun"

	//- create the stop button

	create xbutton stop \
		-xgeom 0:run \
		-ygeom 0:heading \
		-wgeom 25% \
		-title "STOP" \
		-script "echo Stopping simulation ; stop"

	//- create the quit button

	create xbutton quit \
		-xgeom 0:stop \
		-ygeom 0:heading \
		-wgeom 25% \
		-title "QUIT" \
		-script "PurkinjeExit"

	//- create the time dialog

	create xdialog time \
		-value {runtime} \
		-title "Time (msec) : " \
		-script "SetRunTime <v>"

	//- create label for simulation time

	create xlabel simulationTimeLabel \
		-wgeom 70% \
		-title "Simulation time (s) :"

	//- create label for output of simulation time

	create xlabel simulationTimeOutput \
		-xgeom 0:last.right \
		-ygeom 0:time.bottom \
		-wgeom 30% \
		-title "0.0000"

	//- create the time step dialog

	create xdialog outputRate \
		-value {outputRate} \
		-title "Output rate : " \
		-script "SetOutputRate <v>"

	//- create label with normalized / absolute description

	create xlabel chanlabel \
		-ygeom 2:outputRate.bottom \
		-wgeom 70% \
		-title "Output mode :"

	//- create toggle to change normalized / absolute output

	create xtoggle chanmode \
		-xgeom 70% \
		-ygeom 0:outputRate.bottom \
		-wgeom 30% \
		-title "" \
		-onlabel "Normalized" \
		-offlabel "Absolute" \
		-script "ControlSwitchChanmode <v>"

	//- if chanmode is 5

	if ({getglobal iChanMode} == 5)

		//- set widget to normalized output

		setfield chanmode \
			state 1

	//- else

	else
		//- set widget to absolute output

		setfield chanmode \
			state 0
	end

	//- create a simulation mode label

	create xlabel mode \
		-label "Simulation mode"

	//- create the vivo/vitro toggle

	if ({exists /actions/vivovitro})

		create xtoggle vivoVitroToggle \
			-wgeom 70% \
			-ygeom {offset}:mode \
			-title "In Vitro" \
			-script "ToggleMode 1"

		//- create vivo settings button

		create xbutton vivoSettings \
			-xgeom 0:last.right \
			-ygeom {offset}:mode \
			-wgeom 30% \
			-title "Settings" \
			-script "SettingsVivo"

		//- hide this button

		xhide vivoSettings

		//- create a no vitro settings label

		create xlabel noVitroSettings \
			-xgeom 0:vivoVitroToggle \
			-ygeom {offset + 2}:mode \
			-wgeom 30% \
			-title "No settings"

		offset = {offset + 25}
	else
		setfield /control \
			hgeom {{getfield /control hgeom} - 25}
	end

	//- create the injection clamp toggle button

	if ({exists /actions/iClamp})

		create xtoggle iClamp \
			-ygeom {offset}:mode \
			-wgeom 70% \
			-state {iCurrentMode} \
			-title "" \
			-script "ToggleMode 2"

		//- set on/off labels for current injection

		setfield iClamp \
			offlabel "Current injection off" \
			onlabel "Current injection on"

		//- create settings button

		create xbutton iClampSettings \
			-xgeom 0:last.right \
			-ygeom {offset}:mode \
			-wgeom 30% \
			-title "Settings" \
			-script "SettingsIClamp"

		offset = {offset + 25}
	else
		setfield /control \
			hgeom {{getfield /control hgeom} - 25}
	end

	if ({exists /actions/parallel})

		//- create the parallel fiber button

		create xbutton parallel \
			-ygeom {offset}:mode \
			-wgeom 70% \
			-title "Activate parallel fibers" \
			-script "ToggleAction 1"

		//- create settings button

		create xbutton parallelSettings \
			-xgeom 0:last.right \
			-ygeom {offset}:mode \
			-wgeom 30% \
			-title "Settings" \
			-script "SettingsParallel"

		offset = {offset + 25}
	else
		setfield /control \
			hgeom {{getfield /control hgeom} - 25}
	end

	if ({exists /actions/basket})

		//- create the basket fiber button

		create xbutton basket \
			-ygeom {offset}:mode \
			-wgeom 70% \
			-title "Activate basket axon" \
			-script "ToggleAction 2"

		//- create the settings button

		create xbutton basketSettings \
			-xgeom 0:last.right \
			-ygeom {offset}:mode \
			-wgeom 30% \
			-title "Settings" \
			-script "SettingsBasket"

		offset = {offset + 25}
	else
		setfield /control \
			hgeom {{getfield /control hgeom} - 25}
	end

	if ({exists /actions/climbing})

		//- create the climbing fiber button

		create xbutton climbing \
			-ygeom {offset}:mode \
			-wgeom 70% \
			-title "Activate climbing fiber" \
			-script "ToggleAction 3"

		//- create settings button

		create xbutton climbingSettings \
			-xgeom 0:last.right \
			-ygeom {offset}:mode \
			-wgeom 30% \
			-title "Settings" \
			-script "SettingsClimbing"

		offset = {offset + 25}
	else
		setfield /control \
			hgeom {{getfield /control hgeom} - 25}
	end

	//- create information label

	create xlabel info \
		-title "Simulation information"

	//- create help button

	create xbutton help \
		-ygeom 0:info.bottom \
		-wgeom 50% \
		-title "Help" \
		-script "InfoHelp"

	//- create the credits button

	create xbutton credits \
		-xgeom 0:last.right \
		-ygeom 0:info.bottom \
		-wgeom 50% \
		-title "Credits" \
		-script "InfoCredits"

	//- create refresh funtionality for control panel

	CreateRefresh /control/ ControlRefresh

	//- pop previous current element from stack

	pope

end


///
/// SH:	ControlSwitchChanmode
///
/// PA:	state.: new chanmode state (0 or 1).
///
/// DE:	Refresh the control panel
///	Does update for simulation time widget in the control panel
///

function ControlSwitchChanmode(state)

int state

	//- if state is not zero

	if (state)

		//- switch to chanmode 5

		setglobal iChanMode 5

		setfield {cellpath}/solve \
			chanmode {getglobal iChanMode}

	//- else

	else
		//- switch to chanmode 4

		setglobal iChanMode 4

		setfield {cellpath}/solve \
			chanmode {getglobal iChanMode}
	end

	//- synchronize xcell views

	XCellAllSwitchChanMode {state}

end


end


