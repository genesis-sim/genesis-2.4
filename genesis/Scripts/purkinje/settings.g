//genesis
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: settings.g 1.8.2.2.1.1.1.1.1.1.3.4.1.1.1.1.1.13 Mon, 21 Aug 2006 23:07:20 +0200 hugo $
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


// settings.g : control panel settings


extern SettingsIClampSetTarget


int include_settings

if ( {include_settings} == 0 )

	include_settings = 1


include actions.g
include input.g


///
/// SH:	SettingsBasket
///
/// DE:	display and handle settings for basket firing
///	synaptic strength
///

function SettingsBasket

	//- give an informational message

	echo "Settings for basket axon"

	//- show settings form

	xshow /settings/basket
end


///
/// SH:	SettingsBasketHide
///
/// DE:	Hide settings form for basket axon experiment
///

function SettingsBasketHide

	//- give an informational message

	echo "Hiding settings for basket axons"

	//- show settings form

	xhide /settings/basket
end


///
/// SH:	SettingsBasketSetStrength
///
/// PA:	value.:	new strength/level for synapses
///
/// DE:	Set the strength/level for basket synapses
///

function SettingsBasketSetStrength(value)

float value

	//- give an informational message

	echo "Setting strength for basket synapses to "{value}

	//- set global variable

	basketLevel = {value}
end


///
/// SH:	SettingsBasketCreate
///
/// PA:	path..:	path of parent to contain the form
///
/// DE:	create the form for the settings for basket axon firing
///

function SettingsBasketCreate(path)

str path

	//- give an informational message

	echo "Creating form for basket axons"

	//- retreive registered values for geometry

	int xgeom = {getfield /settings/globals xgeom}
	int ygeom = {getfield /settings/globals ygeom}
	int wgeom = {getfield /settings/globals wgeom}
	int hgeom = {getfield /settings/globals hgeom}

	//- create form

	create xform {path}basket [{xgeom},{ygeom},360,175]

	//- make it the current element

	pushe {path}basket

	//- create title label

	create xlabel heading \
		-label "Basket axon settings"

	//- and help the user further by explaining how it works

	create xlabel info0 \
		-label ""

	create xlabel info1 \
		-label "The basket axon gives a fast and strong"

	create xlabel info2 \
		-label "feedforward inhibition coming from the parallel"

	create xlabel info3 \
		-label "fiber system to the Purkinje cell soma."

	//- create dialog for synaptic strength

	create xdialog strength \
		-value {basketLevel} \
		-title "Relative synaptic strength" \
		-script "SettingsBasketSetStrength <v>"

	//- create a button for closing the window

	create xbutton close \
		-title "Done" \
		-script "SettingsBasketHide"

	//- go to previous current element

	pope
end


///
/// SH:	SettingsClimbing
///
/// DE:	display and handle settings for climbing fiber firing
///	delay parameter
///	synaptic strength
///

function SettingsClimbing

	//- give an informational message

	echo "Settings for climbing fiber"

	//- show settings form

	xshow /settings/climbing
end


///
/// SH:	SettingsClimbingHide
///
/// DE:	Hide settings form for climbing fiber experiment
///

function SettingsClimbingHide

	//- give an informational message

	echo "Hiding settings for climbing fiber"

	//- show settings form

	xhide /settings/climbing
end


///
/// SH:	SettingsClimbingSetDelay
///
/// PA:	value.:	new delay between synapses (in msec)
///
/// DE:	Set the delay between consecutive synapses for climbing fiber volley
///

function SettingsClimbingSetDelay(value)

float value

	//- give an informational message

	echo "Setting delay between climbing fiber synapses to "{value}" msec"

	//- set global variable

	climbingDelay = {value * 1.0e-3}
end


///
/// SH:	SettingsClimbingSetStrength
///
/// PA:	value.:	new strength for synapses
///
/// DE:	Set the strength for climbing fiber synapses
///

function SettingsClimbingSetStrength(value)

float value

	//- give an informational message

	echo "Setting strength for climbing fiber synapses to "{value}

	//- set global variable

	strength = {value}
end


///
/// SH:	SettingsClimbingCreate
///
/// PA:	path..:	path of parent to contain the form
///
/// DE:	create the form for the settings for climbing fiber firing
///

function SettingsClimbingCreate(path)

str path

	//- give an informational message

	echo "Creating form for climbing fiber"

	//- retreive registered values for geometry

	int xgeom = {getfield /settings/globals xgeom}
	int ygeom = {getfield /settings/globals ygeom}
	int wgeom = {getfield /settings/globals wgeom}
	int hgeom = {getfield /settings/globals hgeom}

	//- create form

	create xform {path}climbing [{xgeom},{ygeom},350,230]

	//- make it the current element

	pushe {path}climbing

	//- create title label

	create xlabel heading \
		-label "Climbing fiber settings"

	//- and help the user further by explaining how it works

	create xlabel info0 \
		-label ""

	create xlabel info1 \
		-label "The climbing fiber is a single axon per"

	create xlabel info2 \
		-label "Purkinje cell, that gives a very strong"

	create xlabel info3 \
		-label "excitation and calcium influx in the smooth"

	create xlabel info4 \
		-label "part of the dendritic tree."

	//- create dialog for delay between synapses

	create xdialog delay \
		-value {climbingDelay * 1000} \
		-title "Delay between synapses (msec) : " \
		-script "SettingsClimbingSetDelay <v>"

	//- create dialog for synaptic strength

	create xdialog strength \
		-value {strength} \
		-title "Relative synaptic strength" \
		-script "SettingsClimbingSetStrength <v>"

	//- create a button for closing the window

	create xbutton close \
		-title "Done" \
		-script "SettingsClimbingHide"

	//- go to previous current element

	pope
end


///
/// SH:	SettingsIClamp
///
/// DE:	display and handle settings for current clamp
///

function SettingsIClamp

	//- give an informational message

	echo "Settings for current clamp experiment"

	//- show the form for current clamping

	//! this still uses a hard coded path

	xshow /settings/iClamp
end


///
/// SH: SettingsIClampHide
///
/// DE:	hide the IClamp settings window
///

function SettingsIClampHide

	//- hide the window

	//! this uses still a hard coded path

	xhide /settings/iClamp
end


///
/// SH:	SettingsIClampSetCurrent
///
/// PA:	level.:	value in nA for current
///
/// DE:	set the current for the pulse generator and for cte current
///

function SettingsIClampSetCurrent(level)

float level

	if (level <= 50)

		//- give diagnostics

		echo "Setting base current to "{level}" nA"

		//- set global variable

		iClampCurrentBase = {level}

		//- set the fields for current clamp to registered globals

		ActionIClampSetGenerator /actions/iClamp/

	else
		//- give diagnostics

		echo "Base current above 10 nA not allowed."

		//- reset dialog

		setfield /settings/iClamp/level \
			value {iClampCurrentBase}
	end
end


///
/// SH:	SettingsIClampSetDelay1
///
/// PA:	delay1:	value in msec for pulses first delay.
///
/// DE:	set first delay for pulses.
///

function SettingsIClampSetDelay1(delay1)

float delay1

	if (delay1 >= 0)

		//- give diagnostics

		echo "Setting first pulse delay to "{delay1}" msec"

		//- set global variable

		iClampCurrentDelay1 = {delay1}

		//- set the fields for current clamp to registered globals

		ActionIClampSetGenerator /actions/iClamp/

	else
		//- give diagnostics

		echo "Pulses first delay must be positive."

		//- reset dialog

		setfield /settings/iClamp/delay1 \
			value {iClampCurrentDelay1}
	end
end


///
/// SH:	SettingsIClampSetOffset
///
/// PA:	offset:	value in nA for pulse offset
///
/// DE:	set offset for current pulses
///

function SettingsIClampSetOffset(offset)

float offset

	if (offset <= 10)

		//- give diagnostics

		echo "Setting first pulse offset to "{offset}" nA"

		//- set global variable

		iClampCurrentOffset = {offset}

		//- set the fields for current clamp to registered globals

		ActionIClampSetGenerator /actions/iClamp/

	else
		//- give diagnostics

		echo "Pulse Amplitude above 10 nA not allowed."

		//- reset dialog

		setfield /settings/iClamp/offset \
			value {iClampCurrentOffset}
	end
end


///
/// SH:	SettingsIClampSetOffset2
///
/// PA:	offset2: value in nA for pulse offset
///
/// DE:	set offset for current pulses
///

function SettingsIClampSetOffset2(offset2)

float offset2

	if (offset2 <= 10)

		//- give diagnostics

		echo "Setting second pulse offset to "{offset2}" nA"

		//- set global variable

		iClampCurrentOffset2 = {offset2}

		//- set the fields for current clamp to registered globals

		ActionIClampSetGenerator /actions/iClamp/

	else
		//- give diagnostics

		echo "Pulse Amplitude above 10 nA not allowed."

		//- reset dialog

		setfield /settings/iClamp/offset2 \
			value {iClampCurrentOffset2}
	end
end


///
/// SH:	SettingsIClampSetPeriod
///
/// PA:	period:	value in msec for period
///
/// DE:	set period for current pulses
///

function SettingsIClampSetPeriod(period)

float period

	//- give diagnostics

	echo "Setting second pulse delay to "{period}" msec"

	//- set global variable

	iClampCurrentPeriod = {period}

	//- set the fields for current clamp to registered globals

	ActionIClampSetGenerator /actions/iClamp/
end


///
/// SH:	SettingsIClampSetTarget
///
/// PA:	target:	target for current injection protocol.
///
/// DE:	Set target for current injection protocol.
///

function SettingsIClampSetTarget(target)

str target

	//- give diagnostics

	echo {"Setting current clamp target to compartment " @ {target}}

	//- if exists 

	if ( {exists {target}} )

		//- remove the old value from the solver

		//t this should be done in a way that automatically enforces synchronization between
		//t modeling elements and hsolve.

		setfield {cellpath}/solve \
			{findsolvefield {cellpath}/solve {iClampCurrentTarget} inject} 0

		//- set global variable

		iClampCurrentTarget = {target}

		//- reinitialize current injection protocol

		ActionIClampSetGenerator /actions/iClamp/

		int state = {getfield /settings/iClamp/pulse state}

		ActionIClampStop
		ActionIClampStart {state}

		setfield /settings/iClamp/target \
			title {"Change target compartment (" @ {iClampCurrentTarget} @ ")"}

	//- else 

	else
		//- notice the user

		echo "Compartment not found"
	end
end


///
/// SH:	SettingsIClampSetWidth
///
/// PA:	width.:	value in msec for width of pulses
///
/// DE:	set width of first pulse
///

function SettingsIClampSetWidth(width)

float width

	//- give diagnostics

	echo "Setting first pulse width to "{width}" msec"

	//- set global variable

	iClampCurrentWidth = {width}

	//- set the fields for current clamp to registered globals

	ActionIClampSetGenerator /actions/iClamp/
end


///
/// SH:	SettingsIClampSetWidth2
///
/// PA:	width2:	value in msec for width of pulses
///
/// DE:	set width of second pulse
///

function SettingsIClampSetWidth2(width2)

float width2

	//- give diagnostics

	echo "Setting second pulse width to "{width2}" msec"

	//- set global variable

	iClampCurrentWidth2 = {width2}

	//- set the fields for current clamp to registered globals

	ActionIClampSetGenerator /actions/iClamp/
end


///
/// SH:	SettingsIClampToggleMode
///
/// DE:	toggle pulse mode for the current injection
///	If the current mode is not active (iCurrentMode is zero), 
///	nothing is done
///

function SettingsIClampToggleMode

	//- get state of toggle button

	int state = {getfield /settings/iClamp/pulse state}


	//! why again was this synchronization needed ?

// 	//- check if current mode is enabled

// 	if ( {iCurrentMode} != 0)

// 		//- stop current mode

// 		ActionIClampStop

// 		//- start new current with appropriate state

// 		ActionIClampStart {state}
// 	end

	//- check state of toggle button

	if ( {state} == 1)

		//- hide the delay1, offset, width and period labels

		xhide /settings/iClamp/nooffset
		xhide /settings/iClamp/nooffset2
		xhide /settings/iClamp/nowidth
		xhide /settings/iClamp/nodelay1
		xhide /settings/iClamp/nowidth2
		xhide /settings/iClamp/noperiod

		//- show the delay1, offset, width and period dialogs

		xshow /settings/iClamp/offset
		xshow /settings/iClamp/offset2
		xshow /settings/iClamp/width
		xshow /settings/iClamp/delay1
		xshow /settings/iClamp/width2
		xshow /settings/iClamp/period
	else

		//- hide the delay1, offset, width and period dialogs

		xhide /settings/iClamp/offset
		xhide /settings/iClamp/offset2
		xhide /settings/iClamp/width
		xhide /settings/iClamp/delay1
		xhide /settings/iClamp/width2
		xhide /settings/iClamp/period

		//- show the delay1, offset, width and period labels

		xshow /settings/iClamp/nooffset
		xshow /settings/iClamp/nooffset2
		xshow /settings/iClamp/nowidth
		xshow /settings/iClamp/nodelay1
		xshow /settings/iClamp/nowidth2
		xshow /settings/iClamp/noperiod
	end
end


///
/// SH:	SettingsIClampCreate
///
/// PA:	path..:	path of parent to contain the form (ending in /)
///
/// DE:	create the form for the settings for current clamp
///	create the pulse generator for current clamp
///

function SettingsIClampCreate(path)

str path

	//- give an informational message

	echo "Creating form for current clamping"

	//- retreive registered values for geometry

	int xgeom = {getfield /settings/globals xgeom}
	int ygeom = {getfield /settings/globals ygeom}
	int wgeom = {getfield /settings/globals wgeom}
	int hgeom = {getfield /settings/globals hgeom}

	//- create form

	create xform {path}iClamp [{xgeom},{ygeom},350,400]

	//- make it the current element

	pushe {path}iClamp

	//- create title label

	create xlabel heading \
		-label "Current clamp settings"

	//- and help the user further by explaining how it works

	create xlabel info0 \
		-label ""

	create xlabel info1 \
		-label "The current clamp protocol injects a fixed"

	create xlabel info2 \
		-label "amount of current in a dendritic segment"

	create xlabel info3 \
		-label "or the soma"

	//- create dialog for current injection

	create xdialog level \
		-value {iClampCurrentBase} \
		-title "Base  level  (nA) : " \
		-script "SettingsIClampSetCurrent <v>"

	//- create button to popup a requester to change the target element

	create xbutton target \
		-title {"Change target compartment (" @ {iClampCurrentTarget} @ ")"} \
		-script "InputTargetAsk <w>"

	//- create toggle button for pulse or constant

	create xlabel pulse_label \
		-ygeom 2:target \
		-wgeom 50% \
		-label "Toggle cte/pulses : "

	create xtoggle pulse \
		-xgeom 50% \
		-ygeom 0:target \
		-wgeom 50% \
		-title "" \
		-onlabel "Current pulses" \
		-offlabel "Constant current" \
		-script "SettingsIClampToggleMode"

	//- create dialog for pulse offset

	create xdialog offset \
		-value {iClampCurrentOffset} \
		-title "First  pulse amplitude (nA) : " \
		-script "SettingsIClampSetOffset <v> "

	//- create dialog for first delay

	create xdialog delay1 \
		-value {iClampCurrentDelay1} \
		-title "First  pulse  delay  (msec) : " \
		-script "SettingsIClampSetDelay1 <v> "

	//- create dialog for pulse width

	create xdialog width \
		-value {iClampCurrentWidth} \
		-title "First  pulse  width  (msec) : " \
		-script "SettingsIClampSetWidth <v>"

	//- create dialog for pulse offset

	create xdialog offset2 \
		-value {iClampCurrentOffset2} \
		-title "Second pulse amplitude (nA) : " \
		-script "SettingsIClampSetOffset2 <v> "

	//- create dialog for period between pulses

	create xdialog period \
		-value {iClampCurrentPeriod} \
		-title "Second pulse  delay  (msec) : " \
		-script "SettingsIClampSetPeriod <v>"

	//- create dialog for pulse width

	create xdialog width2 \
		-value {iClampCurrentWidth2} \
		-title "Second pulse  width  (msec) : " \
		-script "SettingsIClampSetWidth2 <v>"

	//- create a button for closing the window

	create xbutton close \
		-title "Done" \
		-script "SettingsIClampHide"

	//- create a label for no offset

	create xlabel nooffset \
		-ygeom 3:pulse \
		-title "First pulse amplitude not applicable"

	//- hide the offset dialog

	xhide offset

	//- create a label for no delay1

	create xlabel nodelay1 \
		-ygeom 3:offset \
		-title "First pulse delay not applicable"

	//- hide the delay1 dialog

	xhide delay1

	//- create a label for no width

	create xlabel nowidth \
		-ygeom 3:delay1 \
		-title "First pulse width not applicable"

	//- hide the width dialog

	xhide width

	//- create a label for no offset

	create xlabel nooffset2 \
		-ygeom 3:width \
		-title "Second pulse amplitude not applicable"

	//- hide the offset dialog

	xhide offset2

	//- create label for no period

	create xlabel noperiod \
		-ygeom 3:offset2 \
		-title "Second pulse delay not applicable"

	//- hide the period dialog

	xhide period

	//- create a label for no width

	create xlabel nowidth2 \
		-ygeom 3:period \
		-title "Second pulse width not applicable"

	//- hide the width dialog

	xhide width2

	//- pop previous current element

	pope
end


///
/// SH: SettingsParallel
///
/// DE:	display and handle settings for parallel synchro firing
///	handles localization within xcell
///	handles settings for uniform distribution
///	handles amplitude (strength of synapses)
///

function SettingsParallel

	//- give an informational message

	echo "Settings for synchronous parallel fiber firing"

	//- show the form for parallel fiber

	//! this still uses a hard coded path

	xshow /settings/parallel
end


///
/// SH: SettingsParallelHide
///
/// DE:	hide the parallel settings window
///

function SettingsParallelHide

	//- hide the window

	//! this uses still a hard coded path

	xhide /settings/parallel
end


///
/// SH: SettingsParallelLocalSet
///
/// PA:	branch:	dendrite clicked on within xcell
///
/// DE:	Set local dendrites for synchronous firing
///	Resets script field for xcell widget
///

function SettingsParallelLocalSet(solver,branch,color,cell)

str solver
str branch
int color
str cell

	//- default we give a popup

	int bPopup = 1

	//- reset field for xcell script

	XCellGlobalElectrodePopCallback
	XCellGlobalElectrodePopCallback

	//- isolate tail from clicked membrane

	str branchTail = {getpath {branch} -tail}

	//- find opening brace for array

	int openBrace = {findchar {branchTail} "["}

	//- if the argument is from an array

	if (openBrace != -1)

		//- cut off the index and set global variable

		str branchNoIndex = {substring {branchTail} 0 {openBrace - 1}}

		//- if the branch has an associated mapping

		if ({exists /mappings/{branchNoIndex}})

			//- set synchro global

			synchroBranch = {branchNoIndex}

			//- update the label field

			setfield /settings/parallel/labelArea \
				label "Branch : "{synchroBranch}

			//- we should not give a popup

			bPopup = 0

			//- give diagnostics

			echo \
				"Synchronous parallel fiber set to" \
				{synchroBranch}
		end
	end

	//- if we should give a popup

	if (bPopup)

		//- show the popup

		xshow /settings/localFailed

	//- else the action completed

	else
		//- hide the local set action window

		xhide /settings/localSet
	end
end


///
/// SH: SettingsParallelLocalChoose
///
/// DE:	Initiate choose branch for local synchronous firing
///	This function messes with the xcell call back script
///

function SettingsParallelLocalChoose

	//- set xcell callback

	XCellGlobalElectrodeAddCallback {"ISOLATE"}

	XCellGlobalElectrodeAddCallback \
		{"SettingsParallelLocalSet" \
			@ "_" \
			@ {cellpath}}

	//- show the local set action window

	//! this still uses a hard coded path

	xshow /settings/localSet
end


///
/// SH: SettingsParallelLocalCancel
///
/// DE:	Cancel an choose area for local activation
///

function SettingsParallelLocalCancel

	//- hide the local set action window

	//! this still uses a hard coded path

	xhide /settings/localSet

	//- restore xcell callback

	XCellGlobalElectrodePopCallback
	XCellGlobalElectrodePopCallback

end


///
/// SH: SettingsParallelLocalFail
///
/// DE:	Hide the popup and action window for parallel activation
///

function SettingsParallelLocalFail

	//- hide the popup

	//! this still uses a hard coded path

	xhide /settings/localFailed

	//- hide the local set action window

	//! this still uses a hard coded path

	xhide /settings/localSet
end


///
/// SH: SettingsParallelSetLevel
///
/// PA:	level.:	new level for synchro activation
///
/// DE:	Set the level for synchronous parallel activation
///

function SettingsParallelSetLevel(level)

float level

	//- if setting local activation

	if (bSynchroLocal)

		//- give descriptive message

		echo "Setting local parallel fiber activation level to "{level}

		//- set new activation level

		synchroLocalLevel = {level}

	//- else setting distributed activation

	else
		//- give descriptive message

		echo "Setting distributed parallel fiber activation level to "{level}

		//- set new activation level

		synchroDistrLevel = {level}
	end
end


///
/// SH: SettingsParallelUpdateLevel
///
/// DE:	Update parallel activation level according to mode of synchro firing
///

function SettingsParallelUpdateLevel

	//- if we are in local parallel fiber mode

	if (bSynchroLocal)

		//- set value to local activation strength

		setfield /settings/parallel/level \
			value {synchroLocalLevel}

	//- else distributed firing

	else
		//- set value to distributed activation strength

		setfield /settings/parallel/level \
			value {synchroDistrLevel}
	end
end


///
/// SH: SettingsParallelUpdateWindow
///
/// DE:	Update the settings window after a change in distributed / local mode
///

function SettingsParallelUpdateWindow

	//- if local mode is active

	if (bSynchroLocal)

		//- hide the number dialog

		xhide /settings/parallel/number

		//- show the cte number label

		xshow /settings/parallel/ctenumber

		//- set label of distributed / local

		setfield /settings/parallel/labelLocalDistr \
			label "Local"

	//- else distributed mode is active

	else

		//- show the number dialog

		xshow /settings/parallel/number

		//- hide the cte number label

		xhide /settings/parallel/ctenumber

		//- set label of distributed / local

		setfield /settings/parallel/labelLocalDistr \
			label "Distributed"
	end

	//- update parallel activation strength

	SettingsParallelUpdateLevel
end


///
/// SH: SettingsParallelToggleMode
///
/// DE:	Toggle the mode for parallel fiber synchro firing
///

function SettingsParallelToggleMode

	//- if local mode is active

	if (bSynchroLocal)

		//- give descriptive message

		echo "Setting parallel fiber mode to distributed firing"

		//- switch to distributed mode

		bSynchroLocal = 0
	else

		//- give descriptive message

		echo "Setting parallel fiber mode to local firing"

		//- switch to local mode

		bSynchroLocal = 1
	end

	//- update the settings window

	SettingsParallelUpdateWindow
end


///
/// SH: SettingsParallelSetSynapses
///
/// PA:	number:	number of synapses to activate
///
/// DE:	Set the number of synapses for synchronous parallel activation
///

function SettingsParallelSetSynapses(number)

int level

	//- give descriptive message

	echo "Setting number of synchronous parallel fibers to "{number}

	//- set new activation level

	synchroSynapses = {number}
end


///
/// SH:	SettingsParallelCreate
///
/// PA:	path..:	path of parent to contain the form
///
/// DE:	create the form for the settings for synchro parallel fiber firing
///

function SettingsParallelCreate(path)

str path

	//- give an informational message

	echo "Creating form for parallel fiber settings"

	//- retreive registered values for geometry

	int xgeom = {getfield /settings/globals xgeom}
	int ygeom = {getfield /settings/globals ygeom}
	int wgeom = {getfield /settings/globals wgeom}
	int hgeom = {getfield /settings/globals hgeom}

	//- create form

	create xform {path}parallel [{xgeom},{ygeom},350,280]

	//- make it the current element

	pushe {path}parallel

	//- create title label

	create xlabel heading \
		-label "Synchronous parallel fiber firing settings"

	//- and help the user further by explaining how it works

	create xlabel info0 \
		-label ""

	create xlabel info1 \
		-label "The parallel fiber system provides the main"

	create xlabel info2 \
		-label "excitatory drive to the Purkinje cell."

	create xlabel info3 \
		-label "The settings below allow to fire multiple"

	create xlabel info4 \
		-label "parallel fibers synchronously."

	//- create dialog for distributed synaptic activation

	create xdialog number \
		-title "Number   of   synapses     : " \
		-value {synchroSynapses} \
		-script "SettingsParallelSetSynapses <v>"

	//- create label for local synaptic activation

	create xlabel ctenumber \
		-ygeom 3:info4 \
		-title "Number of synapses is set to 20"

	//- hide the label

	xhide ctenumber

	//- create dialog for synaptic activation strength

	//! value will be updated later on

	create xdialog level \
		-ygeom 0:number \
		-title "Relative synaptic strength : " \
		-script "SettingsParallelSetLevel <v>"

	//- create button for local/distribute firing

	create xbutton localDistr \
		-wgeom 60% \
		-title "Toggle distributed/local" \
		-script "SettingsParallelToggleMode"

	//! the label field in following widget is not relevant,
	//! it will be updated later on

	//- create label to indicate current state of firing

	create xlabel labelLocalDistr \
		-xgeom 0:localDistr \
		-ygeom 1:level \
		-wgeom 40% \
		-title "Distributed"

	//- create button to user choose an area in the xcell

	create xbutton chooseArea \
		-ygeom 0:localDistr \
		-wgeom 60% \
		-title "Change area from xcell" \
		-script "SettingsParallelLocalChoose"

	//- create a label to show the local branch

	create xlabel labelArea \
		-xgeom 0:chooseArea \
		-ygeom 1:localDistr \
		-wgeom 40% \
		-title "Branch : "{synchroBranch}

	//- create a button for closing the window

	create xbutton close \
		-ygeom 0:chooseArea \
		-title "Done" \
		-script "SettingsParallelHide"

	//- update the status of the local / distr label

	SettingsParallelUpdateWindow

	//- pop previous current element

	pope

	//- create form for when local set is active

	create xform {path}localSet [500,250,350,110]

	//- make it the current element

	pushe {path}localSet

	//- create title label

	create xlabel heading1 \
		-label "Click on Purkinje cell dendrite "

	create xlabel heading2 \
		-label "to select next activation site"

	create xlabel heading3 \
		-label "for local parallel fiber input"

	//- create a button to cancel the action

	create xbutton cancel \
		-title "Cancel" \
		-script "SettingsParallelLocalCancel"

	//- pop previous current element

	pope

	//- create form for when local set failed

	create xform {path}localFailed [100,250,350,90]

	//- make it the current element

	pushe {path}localFailed

	//- create title label

	create xlabel heading \
		-label "This branch does not contain enough synapses"

	//- create label with question

	create xlabel question \
		-label "Do you want to choose another branch ?"

	// following code would read better with the '@' operator but
	// that does not work very well
	// perhaps if surrounded with '{', '}' ?

	//- create 'Yes' button

	create xbutton yes \
		-wgeom 50% \
		-title "Yes" \
		-script "SettingsParallelLocalFail;SettingsParallelLocalChoose"

	//- create 'No' button

	create xbutton no \
		-xgeom 0:yes \
		-ygeom 0:question \
		-wgeom 50% \
		-title "No" \
		-script "SettingsParallelLocalFail"

	//- pop previous current element

	pope
end


///
/// SH:	SettingsVivo
///
/// DE:	display and handle settings for vivo mode
///	is dependent on vivo / vitro mode as registered in /mode element
///	vitro : 
///		no parameters
///	vivo : 
///		firing rate in Hz for parallel fibers
///		firing rate in Hz for basket cells
///

function SettingsVivo

	//- give an informational message

	echo "Settings for vivo simulation"

	//- show the form for current clamping

	//! this still uses a hard coded path

	xshow /settings/vivoVitro
end


///
/// SH: SettingsVivoHide
///
/// DE:	hide the vivo settings window
///

function SettingsVivoHide

	//- hide the window

	//! this uses still a hard coded path

	xhide /settings/vivoVitro
end


///
/// SH: UpdateFrequencies
///
/// DE:	update the frequencies for parallel fibers and stellate axons
///	The new frequencies depend on the global {iVVMode} for setting
///	or deleting the frequencies and the globals {phertz} and {ihertz}
///
/// NO: This function uses hsolve's HSAVE / HRESTORE actions to update 
///	the internals of hsolve. This means that fields of all compartments
///	and concentrations should be consistent between 
///	hsolve - normal elements.
///

function UpdateFrequencies

	//- give diagnostics

	if (iVVMode == 0)

		echo "New firing frequencies are "
		echo "  parallel fibers : "{0}
		echo "  stellate cells  : "{0}
	else
		echo "New firing frequencies are "
		echo "  parallel fibers : "{phertz}
		echo "  stellate cells  : "{ihertz}
	end

	echo "Updating synaptic firing frequencies..." -n

	//- show message to user

	xshow /settings/updateFrequencies

	//- update the update freq form

	xupdate /settings/updateFrequencies

	//- synchronize X events

	xflushevents

	echo "..." -n

	//- put the state of the solve element into the originals

	call {cellpath}/solve HSAVE

	//- loop over all channels that should be adjusted

	str element

	foreach element ( {el {cellpath}/##[][TYPE=synchan]} )

		//- for vitro mode

		if (iVVMode == 0)

			//- set element frequency to zero

			setfield {element} frequency 0

		//- for vivo mode

		else
			//- for excitatory synaptic channel

			if ( {getfield {element} synmode} == "ex")

				//- change frequency for vivo mode

				setfield {element} frequency {phertz}

			//- for inhibitory synaptic channel

			elif ( {getfield {element} synmode} == "in")

				//- change frequency for vivo mode

				setfield {element} frequency {ihertz}
			end
		end
	end

	//- update the solve object

	call {cellpath}/solve HRESTORE

	//- hide the message to the user (if it has ever been shown)

	xhide /settings/updateFrequencies

	//- update the update freq form

	xupdate /settings/updateFrequencies

	//- synchronize X events

	xflushevents

	echo "..." -n

	//- give diagnostics

	echo "done"
end


///
/// SH:	SettingsVivoPfFrequencies
///
/// PA:	paraHz:	new parallel firing frequency
///
/// DE:	set the average firing frequency in Hz for asynchronous parallel
///	firing
///	sets the globals {phertz} and calls UpdateFrequencies
///

function SettingsVivoPfFrequencies(paraHz)

float paraHz

	//- set phertz variable

	phertz = {paraHz}

	//- update the firing frequencies

	UpdateFrequencies
end


///
/// SH:	SettingsVivoScFrequencies
///
/// PA:	stelHz:	new stellate firing frequency
///
/// DE:	sets global {ihertz} and calls UpdateFrequencies
///

function SettingsVivoScFrequencies(stelHz)

float stelHz

	//- set ihertz variable

	ihertz = {stelHz}

	//- update the firing frequencies

	UpdateFrequencies
end


///
/// SH:	SettingsVivoCreate
///
/// PA:	path..:	path of parent to contain the form
///
/// DE:	create the form for the settings for vivo mode.
///	Create the form for update frequencies message.
///

function SettingsVivoCreate(path)

str path

	//- give an informational message

	echo "Creating form for vivo settings"

	//- retreive registered values for geometry

	int xgeom = {getfield /settings/globals xgeom}
	int ygeom = {getfield /settings/globals ygeom}
	int wgeom = {getfield /settings/globals wgeom}
	int hgeom = {getfield /settings/globals hgeom}

	//- create form

	create xform {path}vivoVitro [{xgeom},{ygeom},350,190]

	//- make it the current element

	pushe {path}vivoVitro

	//- create title label

	create xlabel heading \
		-label " In vivo settings"

	//- and help the user further by explaining how it works

	create xlabel info0 \
		-label ""

	create xlabel info1 \
		-label "The settings provide the background inhibitory"

	create xlabel info2 \
		-label "and excitatory drive for the Purkinje cell."

	//- create dialog for parallel fiber firing rate

	create xdialog parallel \
		-value {phertz} \
		-title "Parallel fiber rate (Hz) : " \
		-script "SettingsVivoPfFrequencies <v> "

	//- create dialog for stellate cell firing rate

	create xdialog stellate \
		-value {ihertz} \
		-title "Stellate cell  rate (Hz) : " \
		-script "SettingsVivoScFrequencies <v>"

	create xbutton close \
		-title "Done" \
		-script "SettingsVivoHide"

	//- pop previous current element

	pope

	//- create update frequencies form

	create xform {path}updateFrequencies [150,180,200,60]

	//- make it the current element

	pushe {path}updateFrequencies

	//- create title label

	create xlabel heading \
		-label "Reconfiguring setup"

	//- create additional message

	create xlabel message \
		-label "Please wait..."

	//- pop previous current element

	pope
end


///
/// SH:	SettingsCreate
///
/// DE: create all settings forms but keep them hidden
///	creates a neutral element named globals that has fields for all
///	settings forms
///

function SettingsCreate

	//- give an informational message

	echo "Creating all setting forms"

	//- create a parent for all setting forms

	create neutral /settings

	//- create globals element

	create neutral /settings/globals

	//- add fields for positions

	addfield ^ xgeom
	addfield ^ ygeom
	addfield ^ wgeom
	addfield ^ hgeom

	//- give some reasonable defaults for positions

	setfield ^ \
		xgeom 500 \
		ygeom 430 \
		wgeom 300 \
		hgeom 320

	//- create the form for the basket axon firing settings

	SettingsBasketCreate /settings/

	//- create the form for the climbing fiber firing settings

	SettingsClimbingCreate /settings/

	//- create the form for current clamping

	SettingsIClampCreate /settings/

	//- create the form for parallel fiber firing settings

	SettingsParallelCreate /settings/

	//- create the form for vivo settings

	SettingsVivoCreate /settings/
end


end


