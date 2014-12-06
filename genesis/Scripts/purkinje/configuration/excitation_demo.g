//genesis - Purkinje cell version M9 genesis2 master script
// Copyright: Theoretical Neurobiology, Born-Bunge Foundation - UA, 1998-1999.
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: excitation_demo.g 1.8 Tue, 22 Aug 2006 00:00:53 +0200 hugo $
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

//- give header

echo "--------------------------------------------------------------------------"
echo
echo "Purkinje tutorial, version " -n
// $Format: "echo \"$ProjectVersion$ ($ProjectDate$)\""$
echo "Release2-2.17 (Wed, 14 Mar 2007 22:13:17 -0500)"
echo "                   Excitation demo configuration script"
echo
echo "--------------------------------------------------------------------------"


//- local parallel fiber synchronous firing

bSynchroLocal = 1

// //- show the control panel

// xshow /control

//- show the output panel

xshow /output/panel

//- plot soma membrane potential

XCellSimulateClick soma

OutputSetVisible /output/panel/graph_vm 1

//- show some of the graphics : membrane potential

XCellCreate "/comp" "default"

XCellSetupElectrodes /xcells/views/cell_view_1/electrodes 1

//- show some of the graphics : excitatory channels conductance

XCellCreate "/excitatory_channels" "Gk"

XCellSetupElectrodes /xcells/views/cell_view_2/electrodes 1

//- create excitatory input form

create xform /configuration/excitatory_inputs [0,520,420,390]

xshow /configuration/excitatory_inputs

//- make it the current element

pushe /configuration/excitatory_inputs

//- create title label

create xlabel heading \
	-label "Excitatory Inputs"

//- and help the user further by explaining how it works

create xlabel info0 \
	-label ""

create xlabel info1 \
	-label "In this demo, the excitatory inputs to the PC fall apart"

create xlabel info2 \
	-label "in parallel fibers and the climbing fiber.  Parallel"

create xlabel info3 \
	-label "fibers are assumed to give a excitatory background"

create xlabel info4 \
	-label "drive.  The climbing fiber gives very strong"

create xlabel info5 \
	-label "excitation and a very strong calcium influx."

create xlabel sep1 \
	-label ""

//- climbing fiber

create xlabel climbing_fiber_header \
	-label "Climbing Fiber"

//- create button for local/distribute firing

create xbutton climbing_fiber \
	-title {"Activate Climbing Fiber (along smooth dendrite)"} \
	-script "ToggleAction 3"

create xlabel sep2 \
	-label ""

//- background
//-
//- background / no background
//- activate local site PF activation (b3s44)

create xlabel background_activity \
	-label "Parallel Fiber and Stellate Cell background activity"

//- create button for local/distribute firing

function background_toggle

	ToggleMode 1

	if (iVVMode == 0)

		setfield /configuration/excitatory_inputs/background_state title "Activity off (in vitro)"
	else

		setfield /configuration/excitatory_inputs/background_state title "Activity on (in vivo)"
	end
end

create xbutton background_toggle \
	-ygeom 0:background_activity \
	-wgeom 50% \
	-title "Toggle background activity" \
	-script "background_toggle"

create xlabel background_state \
	-xgeom 0:background_toggle \
	-ygeom 2:background_activity \
	-wgeom 50% \
	-title "Activity off (in vitro)"

create xbutton local_activation \
	-ygeom 0:background_toggle \
	-title {"Activate local parallel fibers (site " @ {synchroBranch} @ ")"} \
	-script "ToggleAction 1"

//- simulation control

create xlabel sep3 \
	-label ""

create xlabel control_label \
	-label "Simulation Control"

create xbutton reset \
	-ygeom 0:control_label \
	-wgeom 25% \
	-title "RESET" \
	-script "PurkinjeReset"

create xbutton run \
	-xgeom 0:last.right \
	-ygeom 0:control_label \
	-wgeom 25% \
	-title "RUN" \
	-script "PurkinjeRun"

create xbutton stop \
	-xgeom 0:last.right \
	-ygeom 0:control_label \
	-wgeom 25% \
	-title "STOP" \
	-script "echo Stopping simulation ; stop"

create xbutton quit \
	-xgeom 0:last.right \
	-ygeom 0:control_label \
	-wgeom 25% \
	-title "QUIT" \
	-script "PurkinjeExit"

pope


//- add a graph for total currents

function GraphTotalCurrentCreate
// (unit,outputSource,outputValue,outputFlags,outputDescription,iChannelMode)

// str unit
// str outputSource
// str outputValue
// str outputFlags
// str outputDescription
// int iChannelMode

	str unit = "totals"

	//- create the graph container if it does not exist yet

	if ( ! {exists /configuration/xgraphs} )

		create neutral /configuration/xgraphs
	end

	//- create container form

	int x1 = 410
	int y1 = 520
	int x2 = 500
	int y2 = 420

	create xform /configuration/xgraphs/{unit} [{x1}, {y1}, {x2}, {y2}]

	//- make the form the current element

	pushe /configuration/xgraphs/{unit}

	//- create graph within the form

	create xgraph graph \
		-xgeom 1% \
		-ygeom 2% \
		-wgeom 99% \
		-hgeom 90% \
		-title {"Total dendritic currents"}

	//- set appropriate dimensions for voltage

	setfield ^ \
		xmin 0 \
		xmax {runtime / 1000} \
		ymin -3e-8 \
		ymax 3e-8 \
		bg white \
		XUnits "t (sec)" 

	//- create the info area

	create xlabel area \
		-xgeom 1% \
		-wgeom 99% \
		-label "Tip : use the 'a' key at any time to see all plots."

	//- set clock to use for graph

	useclock /configuration/xgraphs/{unit}/graph 9

	//- add some plots for this graph, that I think are interesting

	//! change if you think this is necessary ...

// storing leak in itotal[0]
// storing par in itotal[1]
// storing CaT in itotal[2]
// storing CaP in itotal[3]
// storing KM in itotal[4]
// storing KC in itotal[5]
// storing K2 in itotal[6]
// storing Ca_pool in itotal[7]
// storing Ca_nernst in itotal[8]
// storing stell in itotal[9]
// storing stell1 in itotal[10]
// storing stell2 in itotal[11]
// storing climb in itotal[12]
// storing KA in itotal[13]
// storing Kdr in itotal[14]
// storing basket in itotal[15]
// storing NaF in itotal[16]
// storing NaP in itotal[17]
// storing h1 in itotal[18]
// storing h2 in itotal[19]

	addmsg {cellpath}/solve /configuration/xgraphs/{unit}/graph \
		PLOT \
		itotal[1] \
		*"I_PFs" \
		*"red"

	addmsg {cellpath}/solve /configuration/xgraphs/{unit}/graph \
		PLOT \
		itotal[3] \
		*"I_CaP" \
		*"blue"

	addmsg {cellpath}/solve /configuration/xgraphs/{unit}/graph \
		PLOT \
		itotal[5] \
		*"I_KC" \
		*"green"

	//- show the graph

	xshow /configuration/xgraphs/{unit}

	//- switch to previous element

	pope
end

GraphTotalCurrentCreate

//- add plot for soma to dendrite current

//- for each time step

create script_out /configuration/xgraphs/soma_to_dendrite

setfield ^ command "soma_to_dendrite"

addfield ^ \
	soma_to_dendrite_current -description "Soma-to-dendrite current"

function soma_to_dendrite

	//- get membrane potential of soma and first dendritic segment

	float soma_vm = {getfield {cellpath}/solve {findsolvefield {cellpath}/solve soma Vm}}

	float main_0_vm = {getfield {cellpath}/solve {findsolvefield {cellpath}/solve main[0] Vm}}

	//- get axial resistance

	float ra = 772813

	//- compute current

	float current = {{{main_0_vm} - {soma_vm}} / ra}

// 	echo {"(" @ {soma_vm} @"-" @ {main_0_vm} @ ") /" @ {ra} @ " = " @ {current}}

	//- set output field

	//! can bring it to scale of other total currents if necessary

	setfield /configuration/xgraphs/soma_to_dendrite \
		soma_to_dendrite_current {{current}}

end

addmsg /configuration/xgraphs/soma_to_dendrite /configuration/xgraphs/totals/graph \
	PLOT \
	soma_to_dendrite_current \
	*"I_s-d" \
	*"purple"


//- reset to schedule plots and output elements

reset


