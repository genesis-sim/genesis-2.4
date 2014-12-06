//genesis
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: xcell.g 1.1.1.6.1.5.2.6.1.1.2.1.2.2.1.25 Mon, 21 Aug 2006 23:40:45 +0200 hugo $
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


// xcell.g : xcell functionality

int include_xcell

if ( {include_xcell} == 0 )

	include_xcell = 1


extern XCellCallback
extern XCellCallbacksTrigger
extern XCellAllSwitchChanMode
extern XCellElectrodeColor
extern XCellElectrodeColorNext
extern XCellElectrodeName
extern XCellRemoveElectrode


include stack.g
include xcell_name_requester.g


//v first color for electrodes

int iXCellElectrodeStartColor = 38

//v basename for library file of all channels

str strXCLibrary = "XCLib"

//v number of cell views created so far

int iCreatedCells = 0


///
/// SH:	XCellAllAddElectrodes
///
/// PA:	path..:	path to the clicked compartment
///	name..:	name for electrode
///	color.:	color for the electrode
///
/// RE:	1 if successfull
///	0 if failed (the electrode already exists)
///
/// DE:	Associate an electrode with {path} for registered xcell parameters
///
/// Electrode prototype is /electrodes/draw/proto {xshape}
///
/// The electrode is created within 
///	/electrodes/draw/{name}.......:	always
///	{form}/draw/{name}............:	when electrodes visible
///					in xcell display
///

function XCellAllAddElectrodes(path,name,color)

str path
str name
int color

	//- loop over all xcell displays

	str form

	foreach form ( {el /xcells/views/#} )

		//- add electrode to this xcell display

		XCellAddElectrode {form} {path} {name} {color}
	end

end


///
/// SH:	XCellAllReset
///
/// DE:	Set the default state for all xcell views
///

function XCellAllReset

	//- loop over all xcell displays

	str form

	foreach form ( {el /xcells/views/#} )

		//- reset display

		XCellReset {form} "default" "default"
	end

end


///
/// SH:	XCellAllSwitchChanMode
///
/// PA:	state.: new chanmode.
///
/// DE:	Switch between normalized and absolute channel mode.
///	Sets the min/max color values for the xcell display.
///

function XCellAllSwitchChanMode(state)

int state

	//- loop over all xcell displays

	str form

	foreach form ( {el /xcells/views/#} )

		//- switch chanmode

		XCellSwitchChanMode {form} {state}
	end

end


///
/// SH:	XCellCallback
///
/// PA:	widget.: clicked widget, needed to computer container form.
///	comp...: compartname of clicked compartment.
///
/// RE:	1 if successfull
///	0 if failed (the electrode already exists)
///
/// DE: Main call back, computes a color and calls all other
/// callbacks.
///

function XCellCallback(widget,comp)

str widget
str comp

	str form

	if ( {widget} != "" )

		form = {{widget} @ "/../.."}

	else

		form = ""
	end

	//- set default result

	int bResult = 0

	//- get color for the electrode

	//- get allocated color

	XCellElectrodeColorNext {form}

	int color = {getfield /xcells/electrode_colors cNextColor}

	//- call the callbacks

	XCellCallbacksTrigger {form} {comp} {color}

	//- set success

	bResult = 1

	//- return result

	return {bResult}
end


///
/// SH:	XCellCallbacksTrigger
///
/// PA:	form..:	path to the xcell display
///	comp..:	path to compartment
///	color.:	color of electrode of the compartment
///
/// DE:	Call all callbacks after an electrode has been added.
///

function XCellCallbacksTrigger(form,comp,color)

str form
str comp
str color

	//- get path to the stack

	str callbacks = {StackElementValues /xcells/stack/}

// 	echo {callbacks}

	//- loop over all callbacks in reverse order

	str callback

	foreach callback ( {arglist {Reverse {arglist {callbacks}} } } )

		//- if the callback is a stop command

		if ( {callback} == "ISOLATE" )

			//- break callback loop

			break
		end

		//- replace underscores

		int pos = {findchar {callback} "_"}

		while ({pos} != -1)

			str head = {substring {callback} 0 {{pos} - 1}}

			str tail = {substring {callback} {{pos} + 1}}

			callback = {{head} @ " " @ {tail}}

			pos = {findchar {callback} "_"}
			
		end

		echo {"Calling electrode callback (" @ {callback} @ ", for compartment " @ {comp} @ " with color code " @ {color} @ ")"}

		callfunc {arglist {callback}} {comp} {color} {form}
	end
end


///
/// SH:	XCellElectrodeAdd
///
/// PA:	comp...: compartment name.
///	color..: color to be used.
///	form...: xcell view.
///
/// RE:	1 if successfull
///	0 if failed (the electrode already exists)
///
/// DE:	Associate an electrode with {comp} for registered xcell parameters
///
/// Electrode prototype is /electrodes/draw/proto {xshape}
///
/// The electrode is created within 
///	/electrodes/draw/{name}.......:	always
///	/xcell/draw/{name}............:	when electrodes visible
///					in xcell display
///

function XCellElectrodeAdd(comp,color,form)

str comp
int color
str form

	//- set default result

	int bResult = 0

	//- get a name for the electrode

	str electrode = {XCellElectrodeName {comp}}

	//- give diagnostics

	echo {"Adding electrode for " @ {electrode}}

	//- copy the prototype electrode

	copy /electrodes/draw/proto /electrodes/draw/{electrode}

	//- set field for identification and color

	setfield ^ \
		ident "record" \
		pixcolor {color}

	//- set the translation of the electrode

	setfield ^ \
		tx {getfield {comp} x} \
		ty {getfield {comp} y} \
		tz {getfield {comp} z}

	//- if the electrodes toggle is set

	if ( {form} != "" )

		if ( {getfield {form}/electrodes state} )

			//- copy the electrode to the xcell window

			copy ^ {form}/draw/{electrode}
		end
	end

	//- set success

	bResult = 1

	//- return result

	return {bResult}
end


///
/// SH:	XCellGlobalElectrodeAddCallback
///
/// PA:	cback.: arguments to be called by the callback.
///
/// DE: Add a callback to the xcell displays.
///
/// The callback will be called whenever new electrodes are added.
/// The callback is a single string without whitespaces.  Underscores
/// will be replaced by spaces whenever it is called.
///

function XCellGlobalElectrodeAddCallback(cback)

str cback

	//- push the call back on the stack

	StackPush /xcells/stack/ {cback}
end


///
/// SH:	XCellGlobalElectrodePopCallback
///
/// PA:	cback.: arguments to be called by the callback.
///
/// DE: Pop a callback to the xcell displays.
///

function XCellGlobalElectrodePopCallback(cback)

str cback

	//- pop the call back on the stack

	StackPop /xcells/stack/ {cback}
end


///
/// SH:	XCellElectrodeColor
///
/// PA:	form..:	path to the xcell.
///
/// RE:	color for current plot
///
/// DE:	Give color for current plot
///

function XCellElectrodeColor(form)

str form

	//- get next color

	int color = {getfield /xcells/electrode_colors cNextColor}

	//- modulo 64 to get a legal value

	color = {color} % 64

	//- return result

	return {color}
end


///
/// SH:	XCellElectrodeColorNext
///
/// PA:	form..:	path to the xcell.
///
/// RE:	color for current plot
///
/// DE:	Give color for next plot
///

function XCellElectrodeColorNext(form)

str form

	//- get next available color

	int color = {getfield /xcells/electrode_colors cNextColor}

	//- modulo 64 to get a legal value

	color = {color + 19} % 64

	//- increment the color count

	setfield /xcells/electrode_colors cNextColor {color}

	//- return result

	return {color}
end


///
/// SH:	XCellElectrodeName
///
/// PA:	path..:	path to the clicked compartment
///
/// RE:	name for electrode
///
/// DE:	Associate an electrode name with {path} for registered xcell parameters
///	The electrode name will have a fairly descriptive name (ie from the 
///	electrode name you can make out which field from which compartment is
///	recorded.
///

function XCellElectrodeName(path)

str path

	//- get tail of compartment

	str compTail = {getpath {path} -tail}

	//- default plot title is without index

	str result = {compTail}

	//- default index is none

	str index = ""

	//- find beginning of index

	int iBegin = {findchar {compTail} "["}

	//- if compartment has an index

	if (iBegin != -1)

		//- get index from registered compartement

		index = {substring \
				{compTail} \
				{iBegin + 1} \
				{{findchar {compTail} "]"} - 1}}

		//- set title for electrode

		result = {substring {compTail} 0 {iBegin - 1}} \
				@ "_" \
				@ {index}
	end

	//- return electrode name

	return {result}
end


///
/// SH:	XCellAddElectrode
///
/// PA:	form..: container form.
///	path..:	path to the clicked compartment
///	name..:	name for electrode
///	color.:	color for the electrode
///
/// RE:	1 if successfull
///	0 if failed (the electrode already exists)
///
/// DE:	Associate an electrode with {path} for registered xcell parameters
///
/// Electrode prototype is /electrodes/draw/proto {xshape}
///
/// The electrode is created within 
///	/electrodes/draw/{name}.......:	always
///	{form}/draw/{name}............:	when electrodes visible
///					in xcell display
///

function XCellAddElectrode(form,path,name,color)

str form
str path
str name
int color

	//- set default result

	int bResult = 0

	//- give diagnostics

	echo "Adding electrode for "{name}

	//- copy the prototype electrode

	copy /electrodes/draw/proto /electrodes/draw/{name}

	//- set field for identification and color

	setfield ^ \
		ident "record" \
		pixcolor {color}

	//- set the translation of the electrode

	setfield ^ \
		tx {getfield {path} x} \
		ty {getfield {path} y} \
		tz {getfield {path} z}

	//- if the electrodes toggle is set

	if ( {getfield {form}/electrodes state} )

		//- copy the electrode to the xcell window

		copy ^ {form}/draw/{name}
	end

	//- set success

	bResult = 1

	//- return result

	return {bResult}
end


///
/// SH:	XCellPrepareElectrodes
///
/// DE:	Prepare electrodes
///

function XCellPrepareElectrodes

	//- create a container

	create xform /electrodes

	//- disable the form

	disable ^

	//- create a draw

	create xdraw /electrodes/draw

	//- create an electrode prototype shape

	create xshape /electrodes/draw/proto \
		-linewidth 1 \
		-textmode nodraw \
		-pixcolor red \
		-coords [-40e-7,0,90e-7][0,0,0][-20e-7,0,100e-7][-30e-7,0,95e-7][-40e-7,0,150e-7][-140e-7,0,100e-7]

	//- add a field for identification

	addfield ^ \
		ident -description "Identification"

	//- set the field

	setfield ^ \
		ident "prototype"

	//- add the callback for adding electrodes

	XCellGlobalElectrodeAddCallback {"XCellElectrodeAdd"}

	//- create color container

	create neutral /xcells/electrode_colors

	disable ^

	pushe ^

	//- add an initialized field for allocated colors

	addfield . cNextColor -descr "next color to allocate"
	setfield . cNextColor {iXCellElectrodeStartColor}

	pope

end


///
/// SH:	XCellRemoveElectrode
///
/// PA:	form..: container form.
///	path..:	path to the clicked compartment
///
/// DE:	Remove the electrode associated with {path}
///

function XCellRemoveElectrode(form,path)

str form
str path

	//- get the registered xcell output source

	str xcOutputSource = {getfield {form} outputSource}

	//- get the registered xcell output value

	str xcOutputValue = {getfield {form} outputValue}

	//- get the registered xcell output flags

	int xcOutputFlags = {getfield {form} outputFlags}

	//- get the electrode title

	str plotTitle = {XGraphPlotTitle \
				{path} \
				{xcOutputSource} \
				{xcOutputValue} \
				{xcOutputFlags}}

	//- if the electrode exists

	if ( {exists /electrodes/draw/{plotTitle}} )

		//- give diagnostics

		echo "Removing electrode "{plotTitle}

		//- remove the electrode

		delete /electrodes/draw/{plotTitle}

		//- if the electrodes toggle is set

		if ( {getfield {form}/electrodes state} )

			//- delete the electrode from the xcell window

			delete {form}/draw/{plotTitle}

			//- update the draw widget

			//xflushevents
			xupdate {form}/draw

			//! to get around a bug that does not update 
			//! the deleted electrodes :
			//! hide and show the parent form

			xhide {form}
			xshow {form}
		end

	//- else

	else
		//- give diagnostics

		echo "No electrode named "{plotTitle}
	end
end


///
/// SH:	XCellRemoveElectrodes
///
/// PA:	form..: container form.
///	unit..:	unit of the electrodes to be removed.
///
/// DE:	Remove all electrodes
///

function XCellRemoveElectrodes(form,unit)

str form
str unit

	//- give diagnostics

	echo "Removing all electrodes, units ignored ----------------------"

	//- loop over all registered electrode

	str electr

	foreach electr ( {el /electrodes/draw/#[][ident=record]} )

		//! because of a bug in the wildcard parsing
		//! we can stil get the prototype here.
		//! we must check this by name of the element

		//- if it is not the prototype

		if ( {getpath {electr} -tail} != "proto" )

			//- delete the electrode to the xcell window

			delete {electr}
		end
	end

	//- update the state of the electrodes

	callfunc XCellSetupElectrodes {form} {getfield {form}/electrodes state}
end


///
/// SH:	XCellSetupElectrodes
///
/// PA:	widget.: clicked widget, needed to computer container form.
///	state.:	0 if electrodes should be invisible
///		1 if electrodes should be visible
///
/// DE:	Show/hide the electrodes
///

function XCellSetupElectrodes(widget,state)

str widget
int state

	str form = {{widget} @ "/.."}

	//- loop over all electrodes in the xcell window

	str electr

	foreach electr ( {el {form}/draw/#[][ident=record]} )

		//- remove the electrode

		delete {electr}
	end

	//- if the electrodes should be visible

	if (state)

		//- give diagnostics

		echo "Showing electrodes"

		//- loop over all registered electrode

		foreach electr ( {el /electrodes/draw/#[][ident=record]} )

			//- get the tail of the name

			str tail = {getpath {electr} -tail}

			//! because of a bug in the wildcard parsing
			//! we can stil get the prototype here.
			//! we must check this by name of the element

			if ( {tail} != "proto" )

				//- copy the electrode to the xcell window

				copy {electr} {form}/draw/{tail}
			end
		end

	//- else

	else

		//- give diagnostics

		echo "Hiding electrodes"
	end

	//- synchronize the GUI button state

	setfield {form}/electrodes state {state}

	//- update the draw widget

	xflushevents
	xupdate {form}/draw

	//! to get around a bug that does not update the deleted electrodes :
	//! hide and show the parent form
	//! for some reason it is not necessary here, but it is above 
	//! (removal of electrodes).

//	xhide {form}
//	xshow {form}
end


///
/// SH:	XCellSwitchChanMode
///
/// PA:	form..: container form.
///	state.: new chanmode state.
///
/// DE:	Switch between normalized and absolute channel mode.
///	Sets the min/max color values for the xcell display.
///

function XCellSwitchChanMode(form,state)

str form
int state

	//- get name for boundary element

	str bound = {BoundElementName \
			{getfield {form} outputSource} \
			{getfield {form} outputValue} \
			{getglobal iChanMode}}

	//- set field for boundaries

	setfield {form} \
		boundElement {bound}

	//- set new boundaries from element

	callfunc XCellSetBoundaries {form} {bound}
end


///
/// SH:	XCellDeleteMessages
///
/// PA:	form..: container form.
///
/// DE:	Delete the messages from the xcell
///	If no messages are setup, none will be deleted and the function
///	will cleanly return.
///

function XCellDeleteMessages(form)

str form

	//- count the number of incoming messages

	int iCount = {getmsg {form}/draw/xcell1 -incoming -count}

	//- if the count is not zero

	if (iCount)

		//- retreive the number of elements from the config

		int iElements = {getfield /config xCellElements}

		//- loop for the number of elements / messages

		int i

		for (i = 0; i < iElements; i = i + 1)

			//- delete the first message

			deletemsg {form}/draw/xcell1 {0} -incoming
		end
	end
end


///
/// SH:	XCellSetupMessages
///
/// PA:	form..: container form.
///	source:	message source in {cellpath}
///	value.:	message value within {source}
///
/// DE:	Setup the messages between the solver and xcell
///	The solver is assumed to be {cellpath}/solve .
///

function XCellSetupMessages(form,source,value)

str form
str source
str value

	//- retreive the wildcard from the config file

	str wPath = {getfield /config xCellPath}

	//- give diagnostics

	echo "Setting up messages to xcell for "{source}", "{value}

	str element
//
//	foreach element ( { el { wPath } } )

//		if ( {exists {element}/{source}} )

//			echo Exists : {element}/{source}

//		else

//			echo Non existent : {element}/{source}
//		end
//		break
//	end

	//- loop over all elements in the xcell object

	str element

	foreach element ( { el { wPath } } )

		//- if the source elements exists

		if ( {exists {element}/{source}} )

			//echo Exists : {element}/{source}

			//- find solve field and add the message

			addmsg {cellpath}/solve {form}/draw/xcell1 \
				COLOR {findsolvefield \
					{cellpath}/solve \
					{element}/{source} \
					{value}}

		//- else the element does not exist

		else

			//echo Non existent : {element}/{source}

			//- add a dummy message

			addmsg /config {form}/draw/xcell1 COLOR z
		end
	end

	//- set number of compartments in the xcell object

	setfield {form}/draw/xcell1 \
		nfield {getfield /config xCellElements}

	//- give diagnostics

	echo "Messages to xcell ok."
end


///
/// SH:	XCellSetupCompMessages
///
/// PA:	form..: container form.
///	source:	message source in {cellpath} (not used)
///	value.:	message value within {source}
///
/// DE:	Setup the messages between the solver and xcell for compartments
///	The solver is assumed to be {cellpath}/solve .
///

function XCellSetupCompMessages(form,source,value)

str form
str source
str value

	//- retreive the wildcard from the config file

	str wPath = {getfield /config xCellPath}

	//- give diagnostics

	echo "Setting up messages to xcell for (compartments), "{value}

	//- loop over all elements in the xcell object

	str element

	foreach element ( { el { wPath } } )

		//- if the source elements exists

		if ( {exists {element}} )

			//echo Exists : {element}

			//- find solve field and add the message

			addmsg {cellpath}/solve {form}/draw/xcell1 \
				COLOR {findsolvefield \
					{cellpath}/solve \
					{element} \
					{value}}

		//- else the element does not exist

		else

			//echo Non existent : {element}

			//- add a dummy message

			addmsg /config {form}/draw/xcell1 COLOR z
		end
	end

	//- set number of compartments in the xcell object

	setfield {form}/draw/xcell1 \
		nfield {getfield /config xCellElements}

	//- give diagnostics

	echo "Messages to xcell ok."
end


///
/// SH:	XCellSetupExcIGEMessages
///
/// PA:	form..: container form.
///	source:	message source in {cellpath}
///	value.:	message value within {source}
///
/// DE:	Setup the messages between the solver and xcell
///	The solver is assumed to be {cellpath}/solve .
///

function XCellSetupExcIGEMessages(form,source,value)

str form
str source
str value

	//- this works only for Erik's Purkinje cell model

	//t have the cell document what is possible :
	//t
	//t register names of actions with the cell,
	//t call the actions at appropriate times.
	//t
	//t here we should call the action exc_ige
	//t

	//! note this depends critically on the fact that addglobal
	//! does not recreate already existing variables.

	addglobal int "cell_supports_exc_ige"

	if ( {getglobal "cell_supports_exc_ige"} == 0)

		echo "This cell does not offer support to link the xcell" -n
		echo "display with excitatory synaptic channels"

		return
	end

	//- retreive the wildcard from the config file

	str wPath = {getfield /config xCellPath}

	//- give diagnostics

	echo "Setting up messages to xcell for " \
		{source}", "{value}

	//- loop over all elements in the xcell object

	str element

	foreach element ( { el { wPath } } )

		//- get the spine that gives messages to the element

		str spine = {getmsg {element} -outgoing -destination 7}

		//- get tail of spine

		str spineTail = {getpath {spine} -tail}

		//- get head of spine for use with solver's flat space

		str spineHead = {getpath {spine} -head}

		//- if we are handling a spine

		if ( {strncmp {spineTail} "spine" 5} == 0 )

			//- default index is zero

			source = "head[0]/par"

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

			//- find solve field and add the message

			addmsg {cellpath}/solve {form}/draw/xcell1 \
				COLOR {findsolvefield \
					{cellpath}/solve \
					{spineHead}{source} \
					{value}}

		//- else if we can find a climbing fiber input

		elif ( {exists {element}/climb } )

			//- find solve field and add the message

			addmsg {cellpath}/solve {form}/draw/xcell1 \
				COLOR {findsolvefield \
					{cellpath}/solve \
					{element}/climb \
					{value}}

		//- else the element does not exist

		else
			//- add a dummy message

			addmsg /config {form}/draw/xcell1 COLOR z
		end
	end

	//- set number of compartments in the xcell object

	setfield {form}/draw/xcell1 \
		nfield {getfield /config xCellElements}

	//- give diagnostics

	echo "Messages to xcell ok."
end


///
/// SH:	XCellSetupInhIGEMessages
///
/// PA:	form..: container form.
///	source:	message source in {cellpath}
///	value.:	message value within {source}
///
/// DE:	Setup the messages between the solver and xcell
///	The solver is assumed to be {cellpath}/solve .
///

function XCellSetupInhIGEMessages(form,source,value)

str form
str source
str value

	//- this works only for Erik's Purkinje cell model

	//t have the cell document what is possible :
	//t
	//t register names of actions with the cell,
	//t call the actions at appropriate times.
	//t
	//t here we should call the action inh_ige
	//t

	//! note this depends critically on the fact that addglobal
	//! does not recreate already existing variables.

	addglobal int "cell_supports_inh_ige"

	if ( {getglobal "cell_supports_inh_ige"} == 0)

		echo "This cell does not offer support to link the xcell" -n
		echo "display with inhibitory synaptic channels"

		return
	end

	//- retreive the wildcard from the config file

	str wPath = {getfield /config xCellPath}

	//- give diagnostics

	echo "Setting up messages to xcell for " \
		{source}", "{value}

	//- loop over all elements in the xcell object

	str element

	foreach element ( { el { wPath } } )

		//- if we are handling a stellate cell

		if ( {exists {element}/stell} )

			//- find solve field and add the message

			addmsg {cellpath}/solve {form}/draw/xcell1 \
				COLOR {findsolvefield \
					{cellpath}/solve \
					{element}/stell \
					{value}}

		//- else if we can find a stellate 1 cell

		elif ( {exists {element}/stell1 } )

			//- find solve field and add the message

			addmsg {cellpath}/solve {form}/draw/xcell1 \
				COLOR {findsolvefield \
					{cellpath}/solve \
					{element}/stell1 \
					{value}}

		//- else if we can find a basket cell

		elif ( {exists {element}/basket } )

			//- find solve field and add the message

			addmsg {cellpath}/solve {form}/draw/xcell1 \
				COLOR {findsolvefield \
					{cellpath}/solve \
					{element}/basket \
					{value}}

		//- else no inhibitory channel exists

		else
			//- add a dummy message

			addmsg /config {form}/draw/xcell1 COLOR z
		end
	end

	//- set number of compartments in the xcell object

	setfield {form}/draw/xcell1 \
		nfield {getfield /config xCellElements}

	//- give diagnostics

	echo "Messages to xcell ok."
end


///
/// SH:	XCellSetupSpineVmMessages
///
/// PA:	form..: container form.
///	source:	message source in {cellpath}
///	value.:	message value within {source}
///
/// DE:	Setup the messages between the solver and xcell
///	The solver is assumed to be {cellpath}/solve .
///

function XCellSetupSpineVmMessages(form,source,value)

str form
str source
str value

	//- retreive the wildcard from the config file

	str wPath = {getfield /config xCellPath}

	//- give diagnostics

	echo "Setting up spine compartment messages to xcell for " \
		{source}", "{value}

	//- loop over all elements in the xcell object

	str element

	foreach element ( { el { wPath } } )

		//- get the spine that gives messages to the element

		str spine = {getmsg {element} -outgoing -destination 7}

		//- get tail of spine

		str spineTail = {getpath {spine} -tail}

		//- get head of spine for use with solver's flat space

		str spineHead = {getpath {spine} -head}

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

			//echo {spineHead}{source} {value}

			//- find solve field and add the message

			addmsg {cellpath}/solve {form}/draw/xcell1 \
				COLOR {findsolvefield \
					{cellpath}/solve \
					{spineHead}{source} \
					{value}}

		//- else the element does not exist

		else
			//- add a dummy message

			addmsg /config {form}/draw/xcell1 COLOR z
		end
	end

	//- set number of compartments in the xcell object

	setfield {form}/draw/xcell1 \
		nfield {getfield /config xCellElements}

	//- give diagnostics

	echo "Messages to xcell ok."
end


///
/// SH:	XCellSetupButtons
///
/// PA:	form..: container form.
///	widget:	name of toggled widget
///	mode..:	output mode of xcell
///		1	comp. Vm
///		2	channel with IGE
///		3	excitatory channel with IGE
///		4	spine comp. Vm
///		5	nernst E
///		6	Calcium concen Ca
/// 		7	inhibitory channel with IGE
///
/// DE:	Display the buttons according to the output mode
///

function XCellSetupButtons(form,widget,mode)

str form
str widget
int mode

	//echo setupbuttons : {widget} {mode}

	//- set the heading for the xcell form

	setfield {form}/heading \
		title {getfield {form} outputDescription}

	//- comp. Vm
	//- or spine comp. Vm
	//- or nernst E
	//- or Calcium concen Ca

	if (mode == 1 || mode == 4 || mode == 5 || mode == 6)

		//- hide I,G toggles

		xhide {form}/Ik
		xhide {form}/Gk

		//- show I,G labels

		xshow {form}/noIk
		xshow {form}/noGk

		//- show E label

		xhide {form}/Ek
		xshow {form}/noEk

	//- channel with IGE
	//- or excitatory channel with IGE
	//- or inhibitory channel with IGE

	elif (mode == 2 || mode == 3 || mode == 7)

		//- hide I,G labels

		xhide {form}/noIk
		xhide {form}/noGk

		//- show I,G toggles

		xshow {form}/Ik
		xshow {form}/Gk

		//- get widget tail

		str channel = {getpath {widget} -tail}

		//- for a calcium channel

		if ( {channel} == "CaP" || {channel} == "CaT")

			//- show Ek toggle

			xhide {form}/noEk
			xshow {form}/Ek

		//- else 

		else
			//- hide Ek toggle

			xshow {form}/noEk
			xhide {form}/Ek
		end

	//- else there is something wrong

	else
		//- give diagnostics

		echo "XCellSetupButtons : Wrong output mode for XCell"
	end

	//- loop over all toggle buttons in the xcell

	str toggle

	foreach toggle ( {el {form}/#[][TYPE=xtoggle]} )

		//- isolate the tail

		str toggleTail = {getpath {toggle} -tail}

		//- if the toggle is not for graph or electrodes

		if (toggleTail != "graph" \
			&& toggleTail != "electrodes")

			//- unset the toggle

			setfield {toggle} \
				state 0
		end
	end

	//- set the toggle that has been pressed

	setfield {widget} \
		state 1

	//- set the toggle for the channel mode

	setfield {form}/{getfield {form} channelMode} \
		state 1

end


///
/// SH:	XCellSetOutput
///
/// PA:	form..: container form.
///	widget:	name of toggled widget
///
/// RE: output mode
///	1	comp. Vm
///	2	channel with IGE
///	3	excitatory channel with IGE
///	4	spine comp. Vm
///	5	nernst E
///	6	Calcium concen Ca
///	7	inhibitory channel with IGE
///
/// DE:	Setup messages for update of xcell, setup buttons, do a reset
///

function XCellSetOutput(form,widget)

str form
str widget

	//- set the field for output

	setfield {form} \
		output {widget}

	//- delete all messages from the xcell

	XCellDeleteMessages {form}

	//- default we should not continue

	int bContinue = 0

	//v strings for construction of messages

	str msgSource
	str msgValue
	str msgDescription

	//= mode for setting up buttons
	//=
	//= 1	comp. Vm
	//= 2	channel with IGE
	//= 3	excitatory channel with IGE
	//= 4	spine comp. Vm
	//= 5	nernst E
	//= 6	Calcium concen Ca
	//= 7	inhibitory channel with IGE

	int flButtons = 0

	//- get the parameter attribute for the widget

	str parameters = {getfield {widget} parameters}

	//- if we are dealing with compartmental membrane potential

	if (parameters == "Vm")

		//- the description is compartmental voltage

		msgDescription = "Compartmental voltage"

		//- the source is empty

		msgSource = ""

		//- the value is Vm

		msgValue = "Vm"

		//- remember to continue

		bContinue = 1

		//- set flags for buttons

		flButtons = 1

	//- if we are dealing with compartmental current

	elif (parameters == "Im")

		//- the description is compartmental voltage

		msgDescription = "Compartmental current"

		//- the source is empty

		msgSource = ""

		//- the value is Vm

		msgValue = "Im"

		//- remember to continue

		bContinue = 1

		//- set flags for buttons

		flButtons = 1

	//- if we are dealing with compartmental leak

	elif (parameters == "leak")

		//- the description is compartmental voltage

		msgDescription = "Compartmental leak current"

		//- the source is empty

		msgSource = ""

		//- the value is Vm

		msgValue = "leak"

		//- remember to continue

		bContinue = 1

		//- set flags for buttons

		flButtons = 1

	//- if we are dealing with spine compartments

	elif (parameters == "spineVm")

		//- the description is spiny voltage

		msgDescription = "Spiny voltage"

		//- the source is the head of the spine

		msgSource = "head"

		//- the value is Vm

		msgValue = "Vm"

		//- remember to continue

		bContinue = 1

		//- set flags for buttons

		flButtons = 4

	//- if we are dealing with nernst

	elif (parameters == "E")

		//- the description is Nernst

		msgDescription = "Nernst"

		//- the source is Ca_nernst

		msgSource = "Ca_nernst"

		//- the value is E

		msgValue = "E"

		//- remember to continue

		bContinue = 1

		//- set flags for buttons

		flButtons = 5

	//- if we are dealing with concentration

	elif (parameters == "Ca")

		//- the description is compartmental Ca conc.

		msgDescription = "Compartmental [Ca2+]"

		//- the source is Ca_pool

		msgSource = "Ca_pool"

		//- the value is Ca

		msgValue = "Ca" 

		//- remember to continue

		bContinue = 1

		//- set flags for buttons

		flButtons = 6

	//- if we are dealing with channels

	elif (parameters == "IGE")

		//- the description is the channel with registered mode

		msgDescription \
			= {getpath {widget} -tail} \
				@ " " \
				@ {getfield \
					{form}/{getfield {form} channelMode} \
					description}

		//- the source is slash + the widget tail

		msgSource = {getpath {widget} -tail}

		//- the value is registered in {form}

		msgValue = {getfield {form} channelMode}

		//- remember to continue

		bContinue = 1

		//- set flags for buttons

		flButtons = 2

	//- if we are dealing with exc channels

	elif (parameters == "excIGE")

		//- the description is the channel with registered mode

		msgDescription \
			= "Excitatory " \
				@ {getfield \
					{form}/{getfield {form} channelMode} \
					description}

		//- the source is not relevant

		msgSource = "excitatory"

		//- the value is registered in {form}

		msgValue = {getfield {form} channelMode}

		//- remember to continue

		bContinue = 1

		//- set flags for buttons

		flButtons = 3

	//- if we are dealing with inh channels

	elif (parameters == "inhIGE")

		//- the description is the channel with registered mode

		msgDescription \
			= "Inhibitory " \
				@ {getfield \
					{form}/{getfield {form} channelMode} \
					description}

		//- the source is not relevant

		msgSource = "inhibitory"

		//- the value is registered in {form}

		msgValue = {getfield {form} channelMode}

		//- remember to continue

		bContinue = 1

		//- set flags for buttons

		flButtons = 7

	//- else somebody messed up the code

	else
		//- give diagnostics

		echo "Somebody messed up the code"
		echo "XCell module bug"
	end

	//- if we should continue

	if (bContinue)

		//- if we are handling compartments

		if (flButtons == 1)

			//- setup messages for compartments

			XCellSetupCompMessages {form} {msgSource} {msgValue}

		//- else if we are handling spine compartments

		elif (flButtons == 4)

			//- setup messages for spines

			XCellSetupSpineVmMessages {form} {msgSource} {msgValue}

		//- else if we are handling exc channels

		elif (flButtons == 3)

			//- setup messages for those channels

			XCellSetupExcIGEMessages {form} {msgSource} {msgValue}

		//- else if we are handling inh channels

		elif (flButtons == 7)

			//- setup messages for those channels

			XCellSetupInhIGEMessages {form} {msgSource} {msgValue}

		//- else we are handling normal messages

		else
			//- setup messages

			XCellSetupMessages {form} {msgSource} {msgValue}
		end
	end

	//- register output description

	setfield {form} \
		outputDescription {msgDescription}

	//- set up buttons correctly

	XCellSetupButtons {form} {widget} {flButtons}

	//- get name of boundary element

	str bound = {BoundElementName {msgSource} {msgValue} {getglobal iChanMode}}

	//- register the output parameters and boundary element

	setfield {form} \
		outputSource {msgSource} \
		outputValue {msgValue} \
		outputFlags {flButtons} \
		outputDescription {msgDescription} \
		boundElement {bound}

	//- set boundaries for xcell

	callfunc XCellSetBoundaries {form} {bound}

	//- reset the simulation

	reset

// 	//- notify graph of switched output units

// 	callfunc XGraphNextPlotMode {msgValue}

	//- return the output mode

	return {flButtons}
end


///
/// SH:	XCellSetBoundaries
///
/// PA:	form..: container form.
///	bound.:	boundary element
///
/// RE:	Success of operation
///
/// DE:	Set boundaries from the given element, update color widgets
///

function XCellSetBoundaries(form,bound)

str form
str bound

	//v result var

	int bResult

	//- if the element with the boundaries exists

	if ( {exists {bound}} )

		//- give diagnostics

		echo "Setting xcell color boundaries from "{bound}

		//- set the fields for dimensions

		setfield {form}/draw/xcell1 \
			colmin {getfield {bound} xcellmin} \
			colmax {getfield {bound} xcellmax}

		//- set config values in color widgets

		callfunc XCellShowConfigure {form}

		//- set result : ok

		bResult = 1

	//- else

	else
		//- set result : failure

		bResult = 0
	end

	//- return result

	return {bResult}
end


///
/// SH:	XCellSetChannelMode
///
/// PA:	form..: container form.
///	widget:	name of toggled widget
///
/// DE:	Set the channel mode
///

function XCellSetChannelMode(form,widget)

str form
str widget

	//- isolate the tail of the toggled widget

	str widgetTail = {getpath {widget} -tail}

	//- set the channelmode field

	setfield {form} \
		channelMode {widgetTail}

	//- update the output messages

	XCellSetOutput {form} {getfield {form} output}
end


///
/// SH:	XCellCancelConfigure
///
/// PA:	form..: container form.
///
/// DE:	Hide the configure window
///

function XCellCancelConfigure(form)

str form

	//- hide the configure window

	xhide {form}/configure
end


///
/// SH:	XCellSetConfigure
///
/// PA:	form..: container form.
///
/// DE:	Set xcell config as in the configuration window
///

function XCellSetConfigure(form)

str form

	//- set color min

	setfield {form}/draw/xcell1 \
		colmin {getfield {form}/colormin value}

	//- set color max

	setfield {form}/draw/xcell1 \
		colmax {getfield {form}/colormax value}
end


///
/// SH:	XCellSimulateClick
///
/// PA:	comp...: compartname of clicked compartment.
///
/// RE:	1 if successfull
///	0 if failed (the electrode already exists)
///
/// DE: Simulate a click on the given compartment.
///

function XCellSimulateClick(comp)

str comp

	return {XCellCallback "" {comp}}

end


///
/// SH:	XCellShowCompartment
///
/// PA:	form........: container form.
///	compartment.: compartment name
///
/// DE:	Popup compartment namer window.
///	This function messes with the xcell call back script
///

function XCellShowCompartment(solver,compartment,color,cell)

str solver
str compartment
int color
str cell

	//- protect relative pathnames

	if ( ! {exists {compartment} } )

		//! hamper, unrelated globals here

		if ( {exists {{cellpath} @ {compartment}} } )

			compartment = {{cellpath} @ {compartment}}

		end
	end

	//- construct an appropriate message text

	str message

	if ( {exists {compartment} } )

		message = {"This is compartment " @ {compartment}}

		message = {{message} \
				@ "(dia:" \
				@ {getfield {compartment} dia} \
				@ ",len:" \
				@ {getfield {compartment} len} \
				@ ")"}

	else
		message = {"This compartment does not exist : " @ {compartment}}

	end

	//- echo the compartment to the terminal

	echo {message}

	//- display the message in the requester

	str requestername = {getfield {cell} requestername}

	XCellNameRequesterSetWarning {requestername} {message}

end


///
/// SH:	XCellShowCompartmentHideWindow
///
/// PA:	form..: container form.
///
/// DE:	Hide the compartment namer window.
///

function XCellShowCompartmentHideWindow(form)

str form

	//- restore field for xcell script

	XCellGlobalElectrodePopCallback
	XCellGlobalElectrodePopCallback

	//- hide add plot window

	str requestername = {getfield {form} requestername}

	XCellNameRequesterHide {requestername}
end


///
/// SH:	XCellShowCompartmentShowWindow
///
/// PA:	form..: container form.
///
/// DE:	Popup compartment namer window.
///	This function messes with the xcell call back script
///

function XCellShowCompartmentShowWindow(form)

str form

	//- set field for xcell script

	XCellGlobalElectrodeAddCallback {"ISOLATE"}

	XCellGlobalElectrodeAddCallback \
		{"XCellShowCompartment" \
			@ "_" \
			@ {cellpath}}

	//- pop compartment namer

	str requestername = {getfield {form} requestername}

	XCellNameRequesterShow {requestername}
end


///
/// SH: XCellShowConfigure
///
/// PA:	form..: container form.
///
/// DE:	Show configuration window for xcell
///

function XCellShowConfigure(form)

str form

	//- set color min value

	setfield {form}/colormin \
		value {getfield {form}/draw/xcell1 colmin}

	//- set color max value

	setfield {form}/colormax \
		value {getfield {form}/draw/xcell1 colmax}
/*
	//- pop up the configuration window

	xshow {form}/configure
*/
end


///
/// SH:	XCellCloseWindow
///
/// PA:	form..: container form.
///
/// DE:	Close xcell display.
///

function XCellCloseWindow(form)

	//- hide all requesters

	XCellShowCompartmentHideWindow {form}

	//- just remove from memory

	delete {form}

	//! let's hope Genesis does not crash ...

	// xhide {form}
end


///
/// SH:	XCellCreateControlButtons
///
/// PA:	form..: container form.
///
/// DE:	Create control buttons.
///

function XCellCreateControlButtons(form)

str form

	//- create button to show compartment names

	create xbutton {form}/closer \
		-xgeom 90% \
		-ygeom -2:info1.bottom \
		-wgeom 10% \
		-title "Close window" \
		-script {"XCellCloseWindow " @ {form}}

end


///
/// SH:	XCellCreateChannelLibrary
///
/// DE:	Create library of Purkinje channels in /library
///

function XCellCreateChannelLibrary

	//v number of channels

	int iChannels = 0

	//- create neutral container

	create neutral /tmp

	//- loop over all purkinje channels found in the library

	str channel

	foreach channel ( {el /library/##[][TYPE=compartment]/##[][TYPE=tabchannel]} )

		//- isolate the name of the channel

		str tail = {getpath {channel} -tail}

		//- if the element does not exist

		if ( ! {exists /tmp/{tail}} )

			//- create library element

			create neutral /tmp/{tail}

			//- increment number of channels

			iChannels = {iChannels + 1}
		end
	end

	//- open the library file

	openfile {strXCLibrary}".u" w

	//- loop over all create elements

	foreach channel ( {el /tmp/#[]} )

		//- write tail of channel to the lib file

		writefile {strXCLibrary}".u" {getpath {channel} -tail}

		//- delete the neutral element

		delete {channel}
	end

	//- close the lib file

	closefile {strXCLibrary}".u"

	//- sort the lib file

	sh "sort <"{strXCLibrary}".u >"{strXCLibrary}".s"

	//- open the library file

	openfile {strXCLibrary}".s" r

	//- loop over all channels

	int i

	for (i = 0; i < iChannels; i = i + 1)

		//- read a channel

		channel = {readfile {strXCLibrary}".s" -linemode}

		//- create a neutral element

		create neutral /tmp/{channel}
	end

	//- close the lib file

	closefile {strXCLibrary}".s"
end


///
/// SH:	XCellCreateColorDialogs
///
/// PA:	form..: container form.
///
/// DE:	Create the xcell color dialogs at the bottom
///

function XCellCreateColorDialogs(form)

str form

	//- create color min dialog

	create xdialog {form}/colormax \
		-xgeom 0:parent.left \
		-ygeom 5:draw.bottom \
		-wgeom 70% \
		-title "Color maximum (red)  : " \
		-script {"XCellSetConfigure " @ {form}}

	//- create color max dialog

	create xdialog {form}/colormin \
		-xgeom 0:parent.left \
		-ygeom 0:last.bottom \
		-wgeom 70% \
		-title "Color minimum (blue) : " \
		-script {"XCellSetConfigure " @ {form}}
end


///
/// SH:	XCellCreateInfoArea
///
/// PA:	form..: container form.
///
/// DE:	Create the xcell info area.
///

function XCellCreateInfoArea(form)

str form

	//- create color min dialog

	create xlabel {form}/info1 \
		-xgeom 0:parent.left \
		-ygeom 15:colormin.bottom \
		-wgeom 90% \
		-title "Initialized"

	create xlabel {form}/info2 \
		-xgeom 0:parent.left \
		-ygeom 0:last.bottom \
		-wgeom 90% \
		-title " "
end


///
/// SH:	XCellCreateHeadings
///
/// PA:	form.: container form.
///
/// DE:	Create the xcell headings for the draw and buttons
///

function XCellCreateHeadings(form)

str form

	//- create header label

	create xlabel {form}/heading [0,0,70%,5%] \
		-title "Comp. voltage"

	//- create buttons label

	create xlabel {form}/outputs \
		-xgeom 0:last.right \
		-ygeom 0 \
		-wgeom 30% \
		-hgeom 5% \
		-title "Possible outputs"
end


///
/// SH:	XCellCreateDraw
///
/// PA:	form..: container form.
///
/// DE:	Create the xcell draw
///

function XCellCreateDraw(form)

str form

	//- create draw within form

	create xdraw {form}/draw [0,5%,70%,70%] \
		-wx 2e-3 \
		-wy 2e-3 \
		-transform ortho3d \
		-bg white

	//- set dimensions for draw

	setfield {form}/draw \
		xmin -1.5e-4 \
		xmax 1.5e-4 \
		ymin -0.4e-4 \
		ymax 3.1e-4

	//- set projection mode

	addglobal str "xcell_transform_mode"

	setfield {form}/draw \
		transform {xcell_transform_mode}

	//- retreive the wildcard from the config file

	str wPath = {getfield /config xCellPath}

	//- create cell display

	create xcell {form}/draw/xcell1 \
	        -path {wPath} \
	        -colmin -0.09 \
	        -colmax 0.02 \
	        -diarange -20 

	//- set clock to use

	useclock {form}/draw/xcell1 9
end


///
/// SH:	XCellCreateOutputButtons
///
/// PA:	form..: container form.
///
/// DE:	Create the xcell buttons and toggles.
///
/// Looks at the channel library to check which tabchannels are
/// present.
///

function XCellCreateOutputButtons(form)

str form

	//- create toggle buttons per compartment

	create xtoggle {form}/comp \
		-xgeom 70% \
		-ygeom 5% \
		-wgeom 20% \
		-title "Comp. Vm" \
		-script {"XCellSetOutput " @ {form} @ " <w>"}
	addfield ^ \
		parameters -description "parameters for messages"
	setfield ^ \
		parameters "Vm"

// 	create xtoggle {form}/Im \
// 		-xgeom 70% \
// 		-wgeom 20% \
// 		-title "Comp. Im" \
// 		-script {"XCellSetOutput " @ {form} @ " <w>"}
// 	addfield ^ \
// 		parameters -description "parameters for messages"
// 	setfield ^ \
// 		parameters "Im"

// 	create xtoggle {form}/leak \
// 		-xgeom 70% \
// 		-wgeom 20% \
// 		-title "Comp. Leak" \
// 		-script {"XCellSetOutput " @ {form} @ " <w>"}
// 	addfield ^ \
// 		parameters -description "parameters for messages"
// 	setfield ^ \
// 		parameters "leak"

	create xtoggle {form}/Caconcen \
		-title "Comp. Ca" \
		-xgeom 70% \
		-wgeom 20% \
		-script {"XCellSetOutput " @ {form} @ " <w>"}
	addfield ^ \
		parameters -description "parameters for messages"
	setfield ^ \
		parameters "Ca"

	create xtoggle {form}/excitatory_channels \
		-xgeom 70% \
		-wgeom 20% \
		-title "Exc. chan." \
		-script {"XCellSetOutput " @ {form} @ " <w>"}
	addfield ^ \
		parameters -description "parameters for messages"
	setfield ^ \
		parameters "excIGE"
	create xtoggle {form}/inhibitory_channels \
		-xgeom 70% \
		-wgeom 20% \
		-title "Inh. chan." \
		-script {"XCellSetOutput " @ {form} @ " <w>"}
	addfield ^ \
		parameters -description "parameters for messages"
	setfield ^ \
		parameters "inhIGE"
//	create xtoggle {form}/compSpines \
//		-xgeom 70% \
//		-wgeom 20% \
//		-title "Spine comp." \
//		-script {"XCellSetOutput " @ {form} @ " <w>"}
//	addfield ^ \
//		parameters -description "parameters for messages"
//	setfield ^ \
//		parameters "spineVm"
//	create xtoggle {form}/nernst \
//		-xgeom 70% \
//		-wgeom 20% \
//		-script {"XCellSetOutput " @ {form} @ " <w>"}
//	addfield ^ \
//		parameters -description "parameters for messages"
//	setfield ^ \
//		parameters "E"

	//- create a label as seperator

	create xlabel {form}/sep1 \
		-xgeom 70% \
		-ygeom 3:last.bottom \
		-wgeom 20% \
		-title ""

	//- create toggle buttons for Ik,Gk,Ek

	create xtoggle {form}/Ik \
		-xgeom 70% \
		-ygeom 1:sep1 \
		-wgeom 20% \
		-script {"XCellSetChannelMode " @ {form} @ " <w>"}
	create xtoggle {form}/Gk \
		-xgeom 70% \
		-wgeom 20% \
		-script {"XCellSetChannelMode " @ {form} @ " <w>"}
	create xtoggle {form}/Ek \
		-xgeom 70% \
		-wgeom 20% \
		-script {"XCellSetChannelMode " @ {form} @ " <w>"}

	//- add descriptions

	addfield {form}/Ik \
		description -description "Description of output"
	addfield {form}/Gk \
		description -description "Description of output"
	addfield {form}/Ek \
		description -description "Description of output"

	//- set descriptions

	setfield {form}/Ik \
		description "current"
	setfield {form}/Gk \
		description "conductance"
	setfield {form}/Ek \
		description "reversal potential"

	create xlabel {form}/noIk \
		-xgeom 70% \
		-ygeom 4:sep1 \
		-wgeom 20% \
		-title "No Ik"
	create xlabel {form}/noGk \
		-xgeom 70% \
		-ygeom 3:last.bottom \
		-wgeom 20% \
		-title "No Gk"
	create xlabel {form}/noEk \
		-xgeom 70% \
		-ygeom 3:last.bottom \
		-wgeom 20% \
		-title "No Ek"

	//- loop over all purkinje channels found in the library

	str channel

	foreach channel ( {el /tmp/#[]} )

		//- isolate the name of the channel

		str tail = {getpath {channel} -tail}

		//- create a toggle if necessary

		XCellCreateToggle {form} {tail}
	end

	//- create toggle to show recording electrodes

	create xtoggle {form}/electrodes \
		-xgeom 70% \
		-ygeom 6:draw.bottom \
		-wgeom 30% \
		-title "" \
		-onlabel "Electrodes" \
		-offlabel "No Electrodes" \
		-script "XCellSetupElectrodes <w> <v>"

	//- create button to show compartment names

	create xbutton {form}/naming \
		-xgeom 70% \
		-ygeom 5:last.bottom \
		-wgeom 30% \
		-title "Compartment Namer" \
		-script {"XCellShowCompartmentShowWindow " @ {form}}
end


///
/// SH:	XCellReset
///
/// PA:	form..: container form.
///	source: output source, "default" for default.
///	mode..: default channel mode, "default" for default.
///
/// DE:	Set the default state for the xcell
///

function XCellReset(form,source,mode)

str form
str source
str mode

	//- apply default source if necessary

	if ( {source} == "default" )

		source = "/comp"

	end

	//! source is a ref to a subelement, add path separator

	//- set output

	setfield {form} \
		output {{form} @ {source}}

	//- apply default channel mode if necessary

	if ( {mode} == "default" || {mode} == "")

		mode = "Gk"

	end

	//- set channel mode

	setfield {form} \
		channelMode {mode}

	//- default : electrodes are not visible

	setfield {form}/electrodes \
		state 0

	//- update all output (buttons, colors)

	//! this just simulates a click on the comp. volt. button

	XCellSetOutput {form} {getfield {form} output}
end


///
/// SH:	XCellCreateToggle
///
/// PA:	form..: container form.
///	name..:	name of widget to create
///
/// DE:	Create a channel toggle button in the xcell form.
///	The button is only created if it does not exist yet.
///

function XCellCreateToggle(form,name)

str form
str name

	//- if the widget does not exist yet

	if ( ! {exists {form}/{name}} )

		//- if there are already channels created

		if ({getfield {form} bButtonsCreated} == 1)

			//- create a toggle button beneath previous

			create xtoggle {form}/{name} \
				-xgeom 90% \
				-wgeom 10% \
				-script {"XCellSetOutput " @ {form} @ " <w>"}

		//- else 

		else
			//- create toggle button at upper right

			create xtoggle {form}/{name} \
				-xgeom 90% \
				-ygeom 5% \
				-wgeom 10% \
				-script {"XCellSetOutput " @ {form} @ " <w>"}

			//- remember that buttons are created

			setfield {form} \
				bButtonsCreated 1
		end

		//- add field for parameters

		addfield ^ \
			parameters -description "parameters for messages"

		//- set the parameter field to channels

		setfield ^ \
			parameters "IGE"
	end
end


// ///
// /// SH:	XCellCBRemovePlot
// ///
// /// PA:	path..:	path to the clicked compartment
// ///
// /// DE:	Callback to remove compartment from graph
// ///

// function XCellCBRemovePlot(path)

// str path

// 	//- remove the electrode

// 	XCellRemoveElectrode {path}

// 	//- remove plot from the clicked compartment

// 	callfunc "XGraphRemoveCompartment" /Purkinje {path}
// end


///
/// SH:	XCellCreate
///
/// PA:	source...: requested output source (button name).
///	chanmode.: default channel mode.
///
/// DE:	Create the xcell display widget with all buttons.
///	Also sets the update clock to clock 9
///

function XCellCreate(source,chanmode)

str source
str chanmode

	//- create container form

	int x1 = 0 + {iCreatedCells * 500}
	int y1 = 0 + {iCreatedCells * 0}
	int x2 = 500
	int y2 = 500

	iCreatedCells = {iCreatedCells + 1}

	//- create form

	str container = {"/xcells/views"}

	str form = {{container} @ "/cell_view_" @ {iCreatedCells}}

	create xform {form} [{x1}, {y1}, {x2}, {y2}]

	//- add field for output

	addfield {form} \
		output -description "Output (toggled widget)"

	//- add field for output source

	addfield {form} \
		outputSource \
		-description "Output source (compartment subelement)"

	//- add field for output value

	addfield {form} \
		outputValue -description "Output value (Vm, Ik, Gk, Ek, Ca)"

	//- add field for output flags

	addfield {form} \
		outputFlags -description "Output flags (1-7)"

	//- add field for output description

	addfield {form} \
		outputDescription -description "Output description (Title)"

	//- add field for channel mode

	addfield {form} \
		channelMode -description "Channel display mode (Ik, Gk, Ek)"

	//- add field for registering boundary element

	addfield {form} \
		boundElement -description "Element with display boundaries"

	//- add field to indicate first toggle for tabchannel has been created

	addfield {form} \
		bButtonsCreated -description "First channel button created indicator"

	setfield {form} \
		bButtonsCreated 0

	//- add a field to link with an xcell name requester

	addfield {form} \
		requestername -description "xcell name requester"

	//- create the heading at the top

	XCellCreateHeadings {form}

	//- create the draw

	XCellCreateDraw {form}

	//- create color dialog widgets at the bottom

	XCellCreateColorDialogs {form}

	//- create the buttons and toggles

	XCellCreateOutputButtons {form}

	//- create the information area

	XCellCreateInfoArea {form}

	//- create the close button

	XCellCreateControlButtons {form}

	//- enable adding electrodes and callback triggering

	setfield {form}/draw/xcell1 \
		script "XCellCallback.d1 <w> <v>"

	//- create the form to request a compartment name

	str requestername \
		= {XCellNameRequester \
			"" \
			"Show compartment names" \
			"Select a compartment from the Purkinje cell" \
			"to know its name, or type a compartment " \
			"name to check its existence:" \
			"Compartment name : " \
			{"XCellShowCompartment " @ {form} @ " <v>"} \
			"" \
			"Done" \
			{"XCellShowCompartmentHideWindow " @ {form}}}

	setfield {form} \
		requestername {requestername}

	//- set an informational message in the info area

	setfield {form}/info1 \
		label "Select an output variable for the xcell view, next click"
	setfield {form}/info2 \
		label "on the dendrite to plot the selected variable in a graph."

	xshow {form}

	XCellReset {form} {source} {chanmode}

	//- reset simulation

	step

	reset
end


//- create the cell view container if it does not exist yet

if ( ! {exists /xcells} )

	create neutral /xcells
end

//- create form container

str container = {"/xcells/views"}

if ( ! {exists {container}} )

	create neutral {container}
end

//- create a stack for the electrode callback

create neutral /xcells/stack

StackCreate /xcells/stack/

//- prepare the electrodes

XCellPrepareElectrodes

//- create a library of all channels

XCellCreateChannelLibrary


end


