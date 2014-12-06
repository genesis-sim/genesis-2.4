//genesis
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: output.g 1.19 Mon, 21 Aug 2006 23:40:45 +0200 hugo $
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


// output.g : output functionality

int include_output

if ( {include_output} == 0 )

	include_output = 1


extern OutputRecordToFile


include simulation_sequence.g
include xcell_name_requester.g
include xgraph.g


///
/// SH:	OutputInitialize
///
/// DE:	Initialize output facilities.
///

function OutputInitialize

	//- create form for output menu

	create xform /output/panel [300, 520, 200, 220] \
		-title "Output Menu"

	//- make it the current element

	pushe /output/panel

// 	//- create a label with a header

// 	create xlabel heading \
// 		-label "Output Menu"

	//- create toggles to show graphs

	create xtoggle /output/panel/graph_vm \
		-offlabel "Display Graph (Vm)" \
		-onlabel "Hide Graph (Vm)" \
		-script "OutputSetVisible <w> <v>"

	addfield ^ \
		linked_form -description "Form linked to this toggle"

	setfield ^ \
		linked_form "/xgraphs/Vm"

// 	create xtoggle /output/panel/graph_im \
// 		-offlabel "Display Graph (Im)" \
// 		-onlabel "Hide Graph (Im)" \
// 		-script "OutputSetVisible <w> <v>"

// 	addfield ^ \
// 		linked_form -description "Form linked to this toggle"

// 	setfield ^ \
// 		linked_form "/xgraphs/Im"

// 	create xtoggle /output/panel/graph_leak \
// 		-offlabel "Display Graph (Leak)" \
// 		-onlabel "Hide Graph (Leak)" \
// 		-script "OutputSetVisible <w> <v>"

// 	addfield ^ \
// 		linked_form -description "Form linked to this toggle"

// 	setfield ^ \
// 		linked_form "/xgraphs/leak"

	create xtoggle /output/panel/graph_ca \
		-offlabel "Display Graph (Ca)" \
		-onlabel "Hide Graph (Ca)" \
		-script "OutputSetVisible <w> <v>"

	addfield ^ \
		linked_form -description "Form linked to this toggle"

	setfield ^ \
		linked_form "/xgraphs/Ca"

	create xtoggle /output/panel/graph_ik \
		-offlabel "Display Graph (Ik)" \
		-onlabel "Hide Graph (Ik)" \
		-script "OutputSetVisible <w> <v>"

	addfield ^ \
		linked_form -description "Form linked to this toggle"

	setfield ^ \
		linked_form "/xgraphs/Ik"

	create xtoggle /output/panel/graph_gk \
		-offlabel "Display Graph (Gk)" \
		-onlabel "Hide Graph (Gk)" \
		-script "OutputSetVisible <w> <v>"

	addfield ^ \
		linked_form -description "Form linked to this toggle"

	setfield ^ \
		linked_form "/xgraphs/Gk"

	create xtoggle /output/panel/graph_em \
		-offlabel "Display Graph (Em)" \
		-onlabel "Hide Graph (Em)" \
		-script "OutputSetVisible <w> <v>"

	addfield ^ \
		linked_form -description "Form linked to this toggle"

	setfield ^ \
		linked_form "/xgraphs/Em"

	//- create toggle to show cell

	create xbutton /output/panel/cell \
		-title "Add Cell View (+ resets)" \
		-script "OutputAddVisible <w> <v>"

	//- create button to allow ascii output

	create xbutton /output/panel/ascii \
		-title "Record to file" \
		-script "OutputRecorderAsk <w>"

	//- create the form to request a compartment name

	str requestername \
		= {XCellNameRequester \
			"" \
			"Add an ascii file plot" \
			"Select a compartment from the Purkinje cell" \
			"to plot its output value in an ascii file," \
			"or type a compartment name below :" \
			"Compartment name to save : " \
			"OutputRecorderToFile <v>" \
			"" \
			"Done" \
			"OutputRecorderHideRequester"}

	addfield /output/panel/ascii \
		requestername -description "xcell name requester"

	setfield /output/panel/ascii \
		requestername {requestername}

	//- pop previous current element from stack

	pope

end


///
/// SH:	OutputRecorderAsk
///
/// DE: Ask for a compartment name to record in an ascii file.
///	This function messes with the xcell call back script
///

function OutputRecorderAsk(widget)

str widget

	XCellGlobalElectrodeAddCallback {"ISOLATE"}

	XCellGlobalElectrodeAddCallback \
		{"OutputRecorderToFile" \
			@ "_" \
			@ {cellpath}}

	//- pop add plot form

	str requestername = {getfield /output/panel/ascii requestername}

	XCellNameRequesterShow {requestername}

end


///
/// SH:	OutputRecorderHideRequester
///
/// DE:	Hide the requester
///	Resets the xcell call back
///

function OutputRecorderHideRequester

	XCellGlobalElectrodePopCallback
	XCellGlobalElectrodePopCallback

	//- hide add plot window

	str requestername = {getfield /output/panel/ascii requestername}

	XCellNameRequesterHide {requestername}
end


///
/// SH:	OutputRecorderToFile
///
/// DE: Convert a given comparment name and record its simulation
/// sequence.
///

function OutputRecorderToFile(solver,name,color,cell)

str solver
str name
int color
str cell

	//- for empty given name

	if ( {name} == "" )

		//- just return

		return
	end

	//- if name is relative

	if ( {substring {name} 0 0} != "/" )

		//- add cell path to compartment name

		name = {{cellpath} @ "/" @ {name}}
	end

	//- get tail of string

	str tail = {getpath {name} -tail}

	//- find opening bracket

	int iOpen = {findchar {tail} "["}

	//- if name has index

	if ( iOpen != -1 )

		//- find closing bracket

		int iClose = {findchar {tail} "]"}

		//- get index from tail of name

		int iIndex = {substring {tail} {iOpen + 1} {iClose - 1}}

		//- if index count of tail is zero

		if ( iIndex == 0 )

			//- delete index of tail of name

			tail = {substring {tail} 0 {iOpen - 1}}

			//- construct new name

			name = {{getpath {name} -head} @ {tail}}
		end
	end

	//- call callback script for xcell

	//! hmm... hardcoded /Purkinje path

	//! check how input.g and settings.g deal with it,
	//! checkout SettingsIClampSetTarget().

	OutputRecordToFile {cellpath} {name} {cell}
end


///
/// SH:	OutputAddVisible
///
/// DE:	Add a new form for output.
///
/// The form to show is registered in the clicked widget using the
/// field 'linked_form'.
///

function OutputAddVisible(widget,value)

str widget

str value

	XCellCreate "/comp" "default"

end


///
/// SH:	OutputSetVisible
///
/// DE:	Show a form for output.
///
/// The form to show is registered in the clicked widget using the
/// field 'linked_form'.
///

function OutputSetVisible(widget,value)

str widget

str value

	//- get form to operate on

	str form = {getfield {widget} linked_form}

	//- operate depending on value -- show/hide

	if ({value})

		xshow {form}

	else
		xhide {form}
	end

	//- synchronize the state with widget look

	setfield {widget} state {value}
end


///
/// SH:	OutputRecordToFile
///
/// DE:	Record the simulation sequence of the given compartment.
///

function OutputRecordToFile(path,comp,cell)

str path
str comp
str cell

	//- set default result : success

	int bResult = 1

	//- get the registered xcell output

	str xcOutput = {getfield {cell} output}

	//- get the registered xcell output source

	str xcOutputSource = {getfield {cell} outputSource}

	//- get the registered xcell output value

	str xcOutputValue = {getfield {cell} outputValue}

	//- get the registered xcell output flags

	int xcOutputFlags = {getfield {cell} outputFlags}

	//- get the registered xcell output description

	str xcOutputDescription = {getfield {cell} outputDescription}

	//- get the registered xcell channel mode

	str xcChannelMode = {getfield {cell} channelMode}

	//- get xcell field for registering boundary element

	str xcBoundElement = {getfield {cell} boundElement}

	//- get tail of compartment

	str compTail = {getpath {comp} -tail}

	//- get a plot title

	str plotTitle = {XGraphPlotTitle \
				{comp} \
				{xcOutputSource} \
				{xcOutputValue} \
				{xcOutputFlags}}

	//- if flags say compartmental output

	if (xcOutputFlags == 1)

		//- if the specified element exists

		if ( {exists {comp} {xcOutputValue}} )

			//- give an informational message

			echo "Adding ascii plot for "{comp}", " \
				"source is '(compartment)', " \
				"value is '"{xcOutputValue}"'."

			//- create the simulation sequence for compartmental output

			str id \
				= {SimulationSequence \
					{comp}_{xcOutputValue} \
					{path}/solve {findsolvefield \
							{path}/solve \
							{comp} \
							{xcOutputValue}}}

		//- else

		else
			//- give an informational message

			echo "No plot for "{comp}", " \
				"source is '(compartment)', " \
				"value is '"{xcOutputValue}"'."
			echo "The element or field does not exist."

			//- set result

			bResult = 0
		end

	//- else if flags say spiny Vm output

	elif (xcOutputFlags == 4)

		//- get the spine that gives messages to the element

		str spine = {getmsg {comp} -outgoing -destination 7}

		//- get tail of spine

		str spineTail = {getpath {spine} -tail}

		//- if we are handling a spine

		if ( {strncmp {spineTail} "spine" 5} == 0 )

			//- default index is zero

			source = "head[0]"

			//- if an index is available

			if ( {strlen {spineTail}} != 5 )

				//- get index of synapse

				int synapseIndex \
					= {substring \
						{spineTail} \
						6 \
						{{strlen {spineTail}} - 1}}

				//- make source string with index

				source = "head[" @ {synapseIndex} @ "]"
			end

			//- give an informational message

			echo "Adding ascii plot for "{comp}", " \
				"source is '"{source}"', " \
				"value is '"{xcOutputValue}"'."

			//- create the simulation sequence for spiny Vm

			str id \
				= {SimulationSequence \
					{comp}_{source}_{xcOutputValue} \
					{path}/solve {findsolvefield \
							{path}/solve \
							{path}/{source} \
							{xcOutputValue}}}

		//- else inform the user

		else
			//- give an informational message

			echo "No plot for "{comp}", " \
				"source is '"{xcOutputSource}"', " \
				"value is '"{xcOutputValue}"'."
			echo "The element or field does not exist."

			//- set result

			bResult = 0
		end

	//- else if flags say excitatory channel output

	elif (xcOutputFlags == 3)

		//- get the spine that gives messages to the element

		str spine = {getmsg {comp} -outgoing -destination 7}

		//- get tail of spine

		str spineTail = {getpath {spine} -tail}

		//- get head of spine for use with solver's flat space

		str spineHead = {getpath {spine} -head}

		//- if we are handling a spine

		if ( {strncmp {spineTail} "spine" 5} == 0 )

			//- default index is zero

			str source = "head[0]/par"

			//- if an index is available

			if ( {strlen {spineTail}} != 5 )

				//- get index of synapse

				int synapseIndex \
					= {substring \
						{spineTail} \
						6 \
						{{strlen {spineTail}} - 1}}

				//- make source string with index

				source = "head[" @ {synapseIndex} @ "]/par"
			end

			//- give an informational message

			echo "Adding ascii plot for "{comp}", " \
				"source is '"{spineHead}{source}"', " \
				"value is '"{xcOutputValue}"'."

			//- create the simulation sequence for excitatory channel output

			str id \
				= {SimulationSequence \
					{comp}_{spineHead}{source}_{xcOutputValue} \
					{path}/solve {findsolvefield \
							{path}/solve \
							{spineHead}{source} \
							{xcOutputValue}}}

		//- else if we can find a climbing fiber input

		elif ( {exists {comp}/climb} )

			//- give an informational message

			echo "Adding ascii plot for "{comp}", " \
				"source is 'climb', " \
				"value is '"{xcOutputValue}"'."

			//- create the simulation sequence for climbing fiber input

			str id \
				= {SimulationSequence \
					{comp}_climb_{xcOutputValue} \
					{path}/solve {findsolvefield \
							{path}/solve \
							{comp}/climb \
							{xcOutputValue}}}

		//- else inform the user

		else
			//- give an informational message

			echo "No plot for "{comp}", " \
				"source is '"{xcOutputSource}"', " \
				"value is '"{xcOutputValue}"'."
			echo "The element or field does not exist."

			//- set result

			bResult = 0
		end

	//- else if flags say inhibitory channel output

	elif (xcOutputFlags == 7)

		//- if we can find a stellate cell

		if ( {exists {comp}/stell} )

			//- give an informational message

			echo "Adding ascii plot for "{comp}", " \
				"source is 'stell', " \
				"value is '"{xcOutputValue}"'."

			//- create the simulation sequence for inhibitory channel output

			str id \
				= {SimulationSequence \
					{comp}_stell_{xcOutputValue} \
					{path}/solve {findsolvefield \
							{path}/solve \
							{comp}/stell \
							{xcOutputValue}}}

		//- else if we can find a stellate 1 cell

		elif ( {exists {comp}/stell1} )

			//- give an informational message

			echo "Adding ascii plot for "{comp}", " \
				"source is 'stell1', " \
				"value is '"{xcOutputValue}"'."

			//- create the simulation sequence for inhibitory channel output

			str id \
				= {SimulationSequence \
					{comp}_stell1_{xcOutputValue} \
					{path}/solve {findsolvefield \
							{path}/solve \
							{comp}/stell1 \
							{xcOutputValue}}}

		//- else if we can find a basket cell

		elif ( {exists {comp}/basket} )

			//- give an informational message

			echo "Adding ascii plot for "{comp}", " \
				"source is 'basket', " \
				"value is '"{xcOutputValue}"'."

			//- create the simulation sequence for inhibitory channel output

			str id \
				= {SimulationSequence \
					{comp}_basket_{xcOutputValue} \
					{path}/solve {findsolvefield \
							{path}/solve \
							{comp}/basket \
							{xcOutputValue}}}

		//- else inform the user

		else
			//- give an informational message

			echo "No plot for "{comp}", " \
				"source is '"{xcOutputSource}"', " \
				"value is '"{xcOutputValue}"'."
			echo "The element or field does not exist."

			//- set result

			bResult = 0
		end

	//- else we have 'normal' compartmental output

	else

		//- if the specified element exists

		if ( {exists {comp}/{xcOutputSource} {xcOutputValue}} )

			//- give an informational message

			echo "Adding ascii plot for "{comp}", " \
				"source is '"{xcOutputSource}"', " \
				"value is '"{xcOutputValue}"'."

			//- create the simulation sequence for inhibitory channel output

			str id \
				= {SimulationSequence \
					{comp}_{xcOutputSource}_{xcOutputValue} \
					{path}/solve {findsolvefield \
							{path}/solve \
							{comp}/{xcOutputSource} \
							{xcOutputValue}}}

		//- else

		else
			//- give an informational message

			echo "No plot for "{comp}", " \
				"source is '"{xcOutputSource}"', " \
				"value is '"{xcOutputValue}"'."
			echo "The element or field does not exist."

			//- set result

			bResult = 0
		end
	end

	//- return result

	return {bResult}
end


end


