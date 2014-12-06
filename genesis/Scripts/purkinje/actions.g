//genesis
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: actions.g 1.13.1.1.1.5.1.10 Mon, 21 Aug 2006 23:07:20 +0200 hugo $
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


// actions.g : parallel fiber and climbing fiber actions

int include_actions

if ( {include_actions} == 0 )

	include_actions = 1


include mapping.g


//v climbing step delay while climbing fiber is firing

int climbingDelaySteps = 0

//v climbing fiber active synapse

int climbingSynapse = 0

///
/// SH:	ActionBasket
///
/// PA:	level.:	level of activation
///
/// DE:	Simulate a basket axon firing
///

function ActionBasket(level)

float level

	//- if the state is inactive

	if ({getfield {cellpath}/basket_presyn state} == "INACT")

		//- set the level of activation

		setfield {cellpath}/basket_presyn z {level}

		//- remember we the basket axon is active

		setfield {cellpath}/basket_presyn \
			state "ACT"

	//- else the state is inactive

	else

		//- reset the level of activation

		setfield {cellpath}/basket_presyn z 0

		//- remember we are inactive

		setfield {cellpath}/basket_presyn \
			state "INACT"

		//- stop the action

		setfield /actions/basket \
			command ""
	end
end


///
/// SH:	ActionBasketInit
///
/// PA:	path..:	path to contain action (ending in '/')
///
/// DE:	Initialize basket axons for firing
///

function ActionBasketInit(path)

str path

	//- create a script object

	create script_out {path}basket

	//- make basket axon presynaptic elements 
	//- and add messages to appropriate channels

	create neutral {cellpath}/basket_presyn
	disable ^
	setfield ^ z 0
	addmsg {cellpath}/basket_presyn {cellpath}/#[]/basket \
		ACTIVATION z

	//- add status field to the element

	addfield ^ \
		state -description "Status of basket axon"

	//- initialize status

	setfield ^ \
		state "INACT"

end


///
/// SH:	ActionBasketStart
///
/// PA:	level.:	level of activation
///
/// DE:	Fire basket axon
///

function ActionBasketStart(level)

float level

	//- set field in script object

	setfield /actions/basket \
		command "ActionBasket "{level / dt}
end


///
/// SH:	ActionClimbing
///
/// DE:	Simulate a climbing fiber firing
///

function ActionClimbing(strength)

float strength

	//- decrement the delay

	climbingDelaySteps = {climbingDelaySteps - 1}

	//- if there is no synapse active

	if (climbingSynapse == 0)

		//- activate the first climbing fiber synapse

		setfield {cellpath}/climb_presyn1 z {strength}

		//- set active synapse

		climbingSynapse = 1

	//- if the first synapse is active

	elif (climbingSynapse == 1)

		//- if we are coming from the first synapse

		if (climbingDelaySteps == -1)

			//- unset activation of first synapse

			setfield {cellpath}/climb_presyn1 z 0

			//- set delay

			climbingDelaySteps = {delaysteps}

		//- if we reached the next synapse

		elif (climbingDelaySteps == 0)

			//- activate second climbing fiber synapse

			setfield {cellpath}/climb_presyn2 z {strength}

			//- set active synapse

			climbingSynapse = 2
		end

	//- if the second synapse is active

	elif (climbingSynapse == 2)

		//- check if we are coming from the first synapse

		if (climbingDelaySteps == -1)

			//- unset activation of second synapse

			setfield {cellpath}/climb_presyn2 z 0

			//- set delay

			climbingDelaySteps = {delaysteps}

		//- if we reached the next synapse

		elif (climbingDelaySteps == 0)

			//- activate third climbing fiber synapse

			setfield {cellpath}/climb_presyn3 z {strength}

			//- set active synapse

			climbingSynapse = 3
		end

	//- if the third synapse is active

	elif (climbingSynapse == 3)

		//- check if we are coming from the first synapse

		if (climbingDelaySteps == -1)

			//- unset activation of third synapse

			setfield {cellpath}/climb_presyn3 z 0

			//- set delay

			climbingDelaySteps = {delaysteps}

		//- if we reached the next synapse

		elif (climbingDelaySteps == 0)

			//- activate fourth climbing fiber synapse

			setfield {cellpath}/climb_presyn4 z {strength}

			//- set active synapse

			climbingSynapse = 4
		end

	//- if the fourth synapse is active

	elif (climbingSynapse == 4)

		//- check if we are coming from the first synapse

		if (climbingDelaySteps == -1)

			//- unset activation of fourth synapse

			setfield {cellpath}/climb_presyn4 z 0

			//- set delay

			climbingDelaySteps = {delaysteps}

		//- if we reached the next synapse

		elif (climbingDelaySteps == 0)

			//- activate fifth climbing fiber synapse

			setfield {cellpath}/climb_presyn5 z {strength}

			//- set active synapse

			climbingSynapse = 5
		end

	//- if the fifth synapse is active

	elif (climbingSynapse == 5)

		//- check if we are coming from the first synapse

		if (climbingDelaySteps == -1)

			//- unset activation of fourth synapse

			setfield {cellpath}/climb_presyn5 z 0

			//- set delay

			climbingDelaySteps = {delaysteps}

		//- if we reached the next synapse (but there is none)

		elif (climbingDelaySteps == 0)

			//- initialize delay

			climbingDelaySteps = 1

			//- unset active synapse

			climbingSynapse = 0

			//- stop the action

			setfield /actions/climbing \
				command ""
		end
	end
end


///
/// SH:	ActionClimbingInit
///
/// PA:	path..:	path to contain action (ending in '/')
///
/// DE:	Initialize climbing fiber messages
///

function ActionClimbingInit(path)

str path

	//- create a script object

	create script_out {path}climbing

	//- initialize number of delay steps

	climbingDelaySteps = 1

	//- make climbing fiber presynaptic elements 
	//- and add messages to appropriate channels

	create neutral {cellpath}/climb_presyn1
	disable {cellpath}/climb_presyn1
	setfield {cellpath}/climb_presyn1 z 0
	addmsg {cellpath}/climb_presyn1 {cellpath}/main[0-2]/climb \
		ACTIVATION z

	create neutral {cellpath}/climb_presyn2
	disable {cellpath}/climb_presyn2
	setfield {cellpath}/climb_presyn2 z 0
	addmsg {cellpath}/climb_presyn2 {cellpath}/main[3-8]/climb \
		ACTIVATION z

	create neutral {cellpath}/climb_presyn3
	disable {cellpath}/climb_presyn3
	setfield {cellpath}/climb_presyn3 z 0
	addmsg {cellpath}/climb_presyn3 {cellpath}/br1[0-6]/climb \
		ACTIVATION z
	addmsg {cellpath}/climb_presyn3 {cellpath}/br2[0-6]/climb \
		ACTIVATION z

	create neutral {cellpath}/climb_presyn4
	disable {cellpath}/climb_presyn4
	setfield {cellpath}/climb_presyn4 z 0
	addmsg {cellpath}/climb_presyn4 {cellpath}/br1[7-12]/climb \
		ACTIVATION z
	addmsg {cellpath}/climb_presyn4 {cellpath}/br2[7-12]/climb \
		ACTIVATION z
	addmsg {cellpath}/climb_presyn4 {cellpath}/br3[0-8]/climb \
		ACTIVATION z

	create neutral {cellpath}/climb_presyn5
	disable {cellpath}/climb_presyn5
	setfield {cellpath}/climb_presyn5 z 0
	addmsg {cellpath}/climb_presyn5 {cellpath}/br1[13-20]/climb \
		ACTIVATION z
	addmsg {cellpath}/climb_presyn5 {cellpath}/br2[13-21]/climb \
		ACTIVATION z
	addmsg {cellpath}/climb_presyn5 {cellpath}/br3[9-16]/climb \
		ACTIVATION z

end


///
/// SH:	ActionClimbingStart
///
/// DE:	Fire climbing fiber
///

function ActionClimbingStart(strength)

float strength

	//- set field in script object

	setfield /actions/climbing \
		command "ActionClimbing "{strength / dt}
end


///
/// SH:	ActionIClamp
///
/// PA: elSource.: source element
///	fiSource.: source field
///	elTarget.: target element
///	fiTarget.: target field
///
/// DE:	Simulate current injection
///
///	Copies elSource->fiSource to elTarget->fiTarget.
///

function ActionIClamp(elSource,fiSource,elTarget,fiTarget)

str elSource
str fiSource
str elTarget
str fiTarget

	//echo "current clamp : "{elTarget}"."{fiTarget}" = "{getfield {elSource} {fiSource}}

	//- set current injection as given in parameters

	setfield {elTarget} {fiTarget} {getfield {elSource} {fiSource}}
end


///
/// SH:	ActionIClampSetGenerator
///
/// PA:	path..:	path of parent to contain the object
///
/// DE:	Set the generator settings to respect the global variables
///	vars :	{iClampCurrentBase}
///		{iClampCurrentOffset}
///		{iClampCurrentWidth}
///		{iClampCurrentOffset2}
///		{iClampCurrentWidth2}
///		{iClampCurrentPeriod}
///		{iClampCurrentTarget}
///		{iClampCurrentDelay1}
///

function ActionIClampSetGenerator(path)

str path

	//- set fields of generator

	setfield {path}current \
		level1 {{iClampCurrentOffset} * 1.0e-9} \
		baselevel {iClampCurrentBase * 1.0e-9} \
		width1 {iClampCurrentWidth * 1.0e-3} \
		delay2 {iClampCurrentPeriod * 1.0e-3} \
		ctecurrent {iClampCurrentBase * 1.0e-9} \
		target {iClampCurrentTarget} \
		delay1 {iClampCurrentDelay1 * 1.0e-3} \
		level2 {{iClampCurrentOffset2} * 1.0e-9} \
		width2 {iClampCurrentWidth2 * 1.0e-3} \
		trig_mode 0
end


///
/// SH:	ActionIClampCreateGenerator
///
/// PA:	path..:	path of parent to contain the object
///
/// DE:	create the pulse generator for current injection experiments
///

function ActionIClampCreateGenerator(path)

str path

	//- go to the given element

	pushe {path}

	//- create the pulsegen object

	create pulsegen current

	//- set fields for pulse generator

	setfield ^ \
		level1 2.0e-9 \
		width1 30 \
		delay1 0 \
		level2 0 \
		width2 0 \
		delay2 0 \
		baselevel 0 \
		trig_mode 0

	//- add a field for a constant current

	addfield ^ ctecurrent \
		-description "Constant current"

	//- set this field to something sensible

	setfield ^ ctecurrent 2.0e-9

	//- add a field for current target element

	addfield ^ target \
		-description "Target element"

	//- set this field to something sensible

	setfield ^ target "soma"

	//- set the fields for current clamp to registered globals

	ActionIClampSetGenerator {path}

	//- go to previous current element

	pope
end


///
/// SH:	ActionIClampInit
///
/// PA:	path..:	path to contain action (ending in '/')
///
/// DE:	Create current injection elements and messages
///

function ActionIClampInit(path)

str path

	//- create a script object

	create script_out {path}iClamp

	//- create elements

	ActionIClampCreateGenerator {path}iClamp/
end


///
/// SH:	ActionIClampStart
///
/// PA:	mode..:	0 : constant current
///		1 : current pulses
///
/// DE:	Start injection of current with given parameters
///
/// NO: This setup interferes with settings.g/UpdateFrequencies, see notes 
///	overthere to understand.
///

function ActionIClampStart(mode)

int mode

	//- if pulse train requested

	if (mode == 1)

		//- give some info

		echo "Switching to current pulses"

		//- set command for script object

		//! two commands : for hsolve and original element

		setfield /actions/iClamp \
			command {"ActionIClamp " \
				 @ "/actions/iClamp/current " \
				 @ "output " \
				 @ {cellpath} \
				 @ "/solve " \
				 @ {findsolvefield \
					{cellpath}/solve \
					{getfield \
						/actions/iClamp/current \
						target} \
					inject} \
				 @ " ; " \
				 @ "ActionIClamp " \
				 @ "/actions/iClamp/current " \
				 @ "output " \
				 @ {cellpath} \
				 @ "/" \
				 @ {getfield \
					/actions/iClamp/current \
					target} \
				 @ " inject" \
				}

	//- if constant current requested

	elif (mode == 0)

		//- give some info

		echo "Switching to constant current"

		//- set command for script object

		//! two commands : for hsolve and original element

		setfield /actions/iClamp \
			command {"ActionIClamp " \
				 @ "/actions/iClamp/current " \
				 @ "ctecurrent " \
				 @ {cellpath} \
				 @ "/solve " \
				 @ {findsolvefield \
					{cellpath}/solve \
					{getfield \
						/actions/iClamp/current \
						target} \
					inject} \
				 @ " ; " \
				 @ "ActionIClamp " \
				 @ "/actions/iClamp/current " \
				 @ "ctecurrent " \
				 @ {cellpath} \
				 @ "/" \
				 @ {getfield \
					/actions/iClamp/current \
					target} \
				 @ " inject" \
				}

	//- else (illegal mode)

	else
		//- give diagnostics

		echo "Somebody messed up the code"
	end

	//- reset the pulse generator

	call /actions/iClamp/current RESET
end


///
/// SH:	ActionIClampStop
///
/// DE:	Stop ongoing current injection
///

function ActionIClampStop

	//- set field in script object

	setfield /actions/iClamp \
		command ""

	//- clear inject field of target compartment

	setfield \
		{cellpath}/solve \
		{findsolvefield \
			{cellpath}/solve \
			{getfield \
				/actions/iClamp/current \
				target} \
			inject} \
		0

	setfield {getfield \
			/actions/iClamp/current \
			target} \
		inject 0

	//! in earlier versions of the tutorial :
	//! after these commands the original soma->inject field can still
	//! be non-zero. Take care : hsolve uses that field to initialize its
	//! own compartment injection fields (e.g. during reset).
end


///
/// SH:	ActionParallelCreateBigDistr
///
/// DE:	Create presynaptic neutral elements for distributed parallel activation
///	Within {cellpath} neutral elements are created for distributed 
///	synchronous activation of parallel fibers
///

function ActionParallelCreateBigDistr

// 100 : 3,6,9,12 + 14
//  50 : 5 + 28
//  25 : 11 + 56

	//- create neutral elements

	create neutral {cellpath}/ds100_synchro[0]
	create neutral {cellpath}/ds100_synchro[1]
	create neutral {cellpath}/ds100_synchro[2]
	create neutral {cellpath}/ds100_synchro[3]
	create neutral {cellpath}/ds050_synchro
	create neutral {cellpath}/ds025_synchro

	//- disable the elements from simulation

	disable {cellpath}/ds100_synchro[0]
	disable {cellpath}/ds100_synchro[1]
	disable {cellpath}/ds100_synchro[2]
	disable {cellpath}/ds100_synchro[3]
	disable {cellpath}/ds050_synchro
	disable {cellpath}/ds025_synchro

	//- set messaged value to zero

	setfield {cellpath}/ds100_synchro[0] \
		z 0
	setfield {cellpath}/ds100_synchro[1] \
		z 0
	setfield {cellpath}/ds100_synchro[2] \
		z 0
	setfield {cellpath}/ds100_synchro[3] \
		z 0
	setfield {cellpath}/ds050_synchro \
		z 0
	setfield {cellpath}/ds025_synchro \
		z 0

	//- add messages to elements

	int i

	for (i = 0; i < 100; i = i + 1)

		// calculate indices

		int i1 = {3+i*14}
		int i2 = {6+i*14}
		int i3 = {9+i*14}
		int i4 = {12+i*14}

		// genesis 2.1

//		i1 = 0
//		i2 = 0
//		i3 = 0
//		i4 = 0

		addmsg {cellpath}/ds100_synchro[0] \
			{cellpath}/spine[{i1}]/head[{i1}]/par \
			ACTIVATION z

		addmsg {cellpath}/ds100_synchro[1] \
			{cellpath}/spine[{i2}]/head[{i2}]/par \
			ACTIVATION z

		addmsg {cellpath}/ds100_synchro[2] \
			{cellpath}/spine[{i3}]/head[{i3}]/par \
			ACTIVATION z

		addmsg {cellpath}/ds100_synchro[3] \
			{cellpath}/spine[{i4}]/head[{i4}]/par \
			ACTIVATION z

		if (i % 2 == 0)

			int i5 = {5+i*14}

			// genesis 2.1

//			i5 = 0

			addmsg {cellpath}/ds050_synchro \
				{cellpath}/spine[{i5}]/head[{i5}]/par \
				ACTIVATION z

			if (i % 4 == 0)

				int i6 = {11+i*14}

				// genesis 2.1

//				i6 = 0

				addmsg {cellpath}/ds025_synchro \
					{cellpath}/spine[{i6}]/head[{i6}]/par \
					ACTIVATION z
			end
		end
	end
end


///
/// SH:	ActionParallelCreateSmallDistr
///
/// DE:	Create distributed activation msgs for small cell
///

function ActionParallelCreateSmallDistr

// 10 : 3,6,9,12 + 14
//  5 : 5 + 28
//  3 : 11 + 56

	//- create neutral elements

	create neutral {cellpath}/ds010_synchro[0]
	create neutral {cellpath}/ds010_synchro[1]
	create neutral {cellpath}/ds010_synchro[2]
	create neutral {cellpath}/ds010_synchro[3]
	create neutral {cellpath}/ds005_synchro
	create neutral {cellpath}/ds003_synchro

	//- disable the elements from simulation

	disable {cellpath}/ds010_synchro[0]
	disable {cellpath}/ds010_synchro[1]
	disable {cellpath}/ds010_synchro[2]
	disable {cellpath}/ds010_synchro[3]
	disable {cellpath}/ds005_synchro
	disable {cellpath}/ds003_synchro

	//- set messaged value to zero

	setfield {cellpath}/ds010_synchro[0] \
		z 0
	setfield {cellpath}/ds010_synchro[1] \
		z 0
	setfield {cellpath}/ds010_synchro[2] \
		z 0
	setfield {cellpath}/ds010_synchro[3] \
		z 0
	setfield {cellpath}/ds005_synchro \
		z 0
	setfield {cellpath}/ds003_synchro \
		z 0

	//- add messages to elements

	int i

	for (i = 0; i < 10; i = i + 1)

		// calculate indices

		int i1 = {3+i*14}
		int i2 = {6+i*14}
		int i3 = {9+i*14}
		int i4 = {12+i*14}

		// genesis 2.1

//		i1 = 0
//		i2 = 0
//		i3 = 0
//		i4 = 0

		addmsg {cellpath}/ds010_synchro[0] \
			{cellpath}/spine[{i1}]/head[{i1}]/par \
			ACTIVATION z

		addmsg {cellpath}/ds010_synchro[1] \
			{cellpath}/spine[{i2}]/head[{i2}]/par \
			ACTIVATION z

		addmsg {cellpath}/ds010_synchro[2] \
			{cellpath}/spine[{i3}]/head[{i3}]/par \
			ACTIVATION z

		addmsg {cellpath}/ds010_synchro[3] \
			{cellpath}/spine[{i4}]/head[{i4}]/par \
			ACTIVATION z

		if (i % 2 == 0)

			int i5 = {5+i*14}

			// genesis 2.1

//			i5 = 0

			addmsg {cellpath}/ds005_synchro \
				{cellpath}/spine[{i5}]/head[{i5}]/par \
				ACTIVATION z

			if (i % 4 == 0)

				int i6 = {11+i*14}

				// genesis 2.1

//				i6 = 0

				addmsg {cellpath}/ds003_synchro \
					{cellpath}/spine[{i6}]/head[{i6}]/par \
					ACTIVATION z
			end
		end
	end
end


///
/// SH:	ActionParallelCreateDistr
///
/// DE:	Create presynaptic neutral elements for distributed parallel activation
///	Within {cellpath} neutral elements are created for distributed 
///	synchronous activation of parallel fibers
///
/// NO:	Expanding a wildcard list with as result a very long string gives a
///	segmentation fault. So we must use an entry in the config file to
///	determine the number of spines in the read cell.
///

function ActionParallelCreateDistr

	//- create status element

	create neutral {cellpath}/dssts_synchro

	//- disable the element from simulation

	disable {cellpath}/dssts_synchro

	//- add field for status

	addfield {cellpath}/dssts_synchro \
		state -description "Status for distributed firing"

	//- default status is inactive

	setfield {cellpath}/dssts_synchro \
		state "INACT"

	//- add field for number of firings

	addfield {cellpath}/dssts_synchro \
		iFirings -description "Number of synchro activated synapses"

	//- determine number of spines

	// following line gives a segmentation fault

	//int iSpines = {NumberOfElements {cellpath}/spine}

	// that's why the number of spines is set up in the config

	int iSpines = {getfield /config iSpines}

	//- give diagnostics

	echo Setting up distribution for {iSpines} spines

	//- if we are working with the big model

	if (iSpines == 1474)

		//- setup distributions for big model

		ActionParallelCreateBigDistr

	//- if we are working with the small model

	elif (iSpines == 142)

		//- setup distributions for small model

		ActionParallelCreateSmallDistr
	end
end


///
/// SH:	ActionParallelCreateLocal
///
/// DE:	Create presynaptic neutral elements for local parallel activation
///	Searches /mappings/ for dendrites that need simultanous activation
///	For every dendrite branch 20 messages are set up (so we need at least
///	20 spines per mapping).
///	The neutral elements are created within {cellpath} and are named to
///	the dendrite branchlets they activate.
///

function ActionParallelCreateLocal

	//- loop over all elements that have been mapped

	str mappedDend

	foreach mappedDend ( {el /mappings/#} )

		//- cut of path to element

		str mappedDendTail = {getpath {mappedDend} -tail}

		//- get number of synapses on this branch

		int synapses = {getfield {mappedDend} synapseCount}

		//- determine synapse interval

		float interval = {synapses / 20.0}

		//- create a neutral presynaptic element

		create neutral {cellpath}/{mappedDendTail}_synchro

		//- disable the element from calculations

		disable ^

		//- set messaged value to zero

		setfield {cellpath}/{mappedDendTail}_synchro z 0

		//- initialize synapse index

		float index = {getfield {mappedDend} synapseBegin}

		//- loop over required inputs (total of 20)

		int i

		for (i = 0; i < 20; i = i + 1)

			//- calculate rounded index

			int k = {round {index}}

			// genesis 2.1

//			k = 0

			//- setup message to spine with calculated index

			addmsg {cellpath}/{mappedDendTail}_synchro \
				{cellpath}/spine[{k}]/head[{k}]/par \
				ACTIVATION z

			//- increment the index with interval

			index = {index + interval}
		end
	end
end


///
/// SH:	ActionParallelInit
///
/// PA:	path..:	path to contain action (ending in '/')
///
/// DE:	Initialize parallel fiber synchronous firing action
///	Initializes local synchronous firing
///	Initializes distribute synchronous firing
///

function ActionParallelInit(path)

str path

	//- create a script object

	create script_out {path}parallel

	//- create mapping for the purkinje cell

	MappingCreate {cellpath}

	//- create elements for local activation

	ActionParallelCreateLocal

	//- create elements for distributed activation

	ActionParallelCreateDistr
end


///
/// SH:	ActionParallelDistr
///
/// PA:	number:	number of synapses to activate
///	level.:	level of activation
///
/// RE: number of activated parallel fibers
///
/// DE:	Simulate distributed synchro parallel fiber firing
///     Number of parallel fibers is rounded to 25 and has a maximum of
///     475 for the big model, 48 for the small model.
///

function ActionParallelDistr(number,level)

int number
float level

        //v number of parallel firings

        int iFirings

	//echo ActionParallelDistr {number} {level}

	//- if state was inactivating

	if ( {getfield {cellpath}/dssts_synchro state} == "INACT")

		//- set state : activating

		setfield {cellpath}/dssts_synchro \
			state "ACT"

		//- retrieve number of spines

		int iSpines = {getfield /config iSpines}

		//- for the big model

		if (iSpines == 1474)

			//- if number of spines is bigger than 475

			if (number > 475)

				//- set number to 475

				number = 475
			end

			//- determine rest 100

			int rest100 = {number % 100}

			//- determine times 100

			int times100 = {{number - rest100} / 100}

			//- determine times 50

			int times50 = {rest100 >= 50}

			//- determine times 25

			int times25 = {rest100 - times50 * 50 >= 25}

			//- loop over number of 100 times

			int i

			for (i = 0; i < times100; i = i + 1)

				//- activate the synchro element

				setfield {cellpath}/ds100_synchro[{i}] \
					z {level}
			end

			//- if we should activate the 50 element

			if (times50)

				//- activate the 50 element

				setfield {cellpath}/ds050_synchro \
					z {level}
			end

			//- if we should activate the 25 element

			if (times25)

				//- activate the 25 element

				setfield {cellpath}/ds025_synchro \
					z {level}
			end

                        //- set number of parallel firings

                        iFirings = {times100 * 100 \
					+ times50 * 50 \
					+ times25 * 25}

		//- for the small model

		elif (iSpines == 142)

			//- if number is bigger than 48

			if (number > 48)

				//- set number to 48

				number = 48
			end

			//- determine rest 10

			int rest10 = {number % 10}

			//- determine times 10

			int times10 = {{number - rest10} / 10}

			//- determine times 5

			int times5 = {rest10 >= 5}

			//- determine times 3

			int times3 = {rest10 - times5 * 5 >= 3}

			//- loop over number of 10 times

			int i

			for (i = 0; i < times10; i = i + 1)

				//- activate the synchro element

				setfield {cellpath}/ds010_synchro[{i}] \
					z {level}
			end

			//- if we should activate the 5 element

			if (times5)

				//- activate the 5 element

				setfield {cellpath}/ds005_synchro \
					z {level}
			end

			//- if we should activate the 3 element

			if (times3)

				//- activate the 3 element

				setfield {cellpath}/ds003_synchro \
					z {level}
			end

                        //- set number of parallel firings

                        iFirings = {times10 * 10 + times5 * 5 + times3 * 3}
		end

	//- else we are inactivating

	else
		//- set status to inactive

		setfield {cellpath}/dssts_synchro \
			state "INACT"

		//- loop over all distr synchro elements

		str synchro

		foreach synchro ( {el {cellpath}/ds#_synchro[]} )

			//echo clearing {synchro}

			//- clear messaged value

			setfield {synchro} z 0
		end

		//- reset the script field

		setfield /actions/parallel \
			command ""

                //- set number of parallel firings

		iFirings = 0
	end

	//- set field in script object

	setfield {cellpath}/dssts_synchro \
		iFirings {iFirings}

	//- return result

	return {iFirings}
end


///
/// SH:	ActionParallelLocal
///
/// PA:	branch:	branch for local activation
///	level.:	level of activation
///
/// DE:	Simulate local synchro parallel fiber firing
///

function ActionParallelLocal(branch,level)

str branch
float level

	//- if we have no activation level

	if ({getfield {cellpath}/{branch}_synchro z} == 0)

		//- set activation level

		setfield {cellpath}/{branch}_synchro z {level}

		//! here we could retreive the number of synapses from a field 
		//! from the branch's synchro element. 
		//! The field should be set with a value that comes from the
		//! mapping's elements/procedures

	 //- else (activation level is set on the previous time step)

	else
		//-  reset the activation level

		setfield {cellpath}/{branch}_synchro z 0

		//- reset the script field

		setfield /actions/parallel \
			command ""

		//! set number of activated synapses to zero
	end

	//! set number of activated synapses to make it global
end


///
/// SH:	ActionParallelStart
///
/// PA:	local.:	0 means distributed mode
///		1 means local mode
///	branch:	branch for local activation
///	number:	number of synapses to activate
///	level.:	level of activation
///
/// DE:	Start local synchro firing of parallel fibers
///

function ActionParallelStart(local,branch,number,level)

int local
str branch
int number
float level

	//- if local mode is selected

	if (local)

		// give a discriptive message

		echo "Firing 20 synchronous parallel fibers on branch "{branch}

		//- set field of script object

		setfield /actions/parallel \
			command "ActionParallelLocal "{branch}" "{level / dt}

	//- else distributed mode is asked

	else
		//! of course this does not work...

		//int iFirings = {getfield {cellpath}/dssts_synchro iFirings}

		//! It works if ActionParallelDistr is executed exactly once
		//! but I do not know how to figure this out in this proc to
		//! get the number of firings.
		//! As a result the code to calc. number of firings is 
		//! replicated here.

		//- if number of spines is bigger than 475

		if (number > 475)

			//- set number to 475

			number = 475
		end

		//- determine rest 100

		int rest100 = {number % 100}

		//- determine times 100

		int times100 = {{number - rest100} / 100}

		//- determine times 50

		int times50 = {rest100 >= 50}

		//- determine times 25

		int times25 = {rest100 - times50 * 50 >= 25}

		//- set number of parallel firings

		int iFirings = {times100 * 100 + times50 * 50 + times25 * 25}

		//- give a discriptive message

		echo "Firing "{iFirings}" synchronous parallel fibers"

		//- set field of script object

		setfield /actions/parallel \
			command "ActionParallelDistr "{number}" "{level / dt}
	end
end


///
/// SH:	ActionsInit
///
/// PA:	no_vivovitro..:	0 vivo - vitro switch possible
///			1 vivo - vitro switch not possible
///	no_basket.....:	basket axon activation
///	no_climbing...:	climbing fiber activation
///	no_iclamp.....:	current clamp
///	no_parallel...:	parallel fiber activation
///
/// DE:	Initialize interactive actions module
///	Initializes messages for climbing fiber and for synchronous firing 
///	of parallel fibers.
///

function ActionsInit(no_basket,no_climbing,no_iclamp,no_parallel,no_vivovitro)

int no_basket
int no_climbing
int no_iclamp
int no_parallel
int no_vivovitro

	//- create a container

	create neutral /actions

	//- initialize basket axons

	if (!no_basket)

		ActionBasketInit /actions/
	end

	//- initialize climbing fiber synapses

	if (!no_climbing)

		ActionClimbingInit /actions/
	end

	//- initialize current clamp

	if (!no_iclamp)

		ActionIClampInit /actions/
	end

	//- initialize synchronous parallel fiber

	if (!no_parallel)

		ActionParallelInit /actions/
	end

	//- initialize vivo vitro switch

	if (!no_vivovitro)

		create neutral /actions/vivovitro
	end
end


end


