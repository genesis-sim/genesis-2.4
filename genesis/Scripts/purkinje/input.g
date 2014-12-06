//genesis
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: input.g 1.9 Thu, 10 Aug 2006 21:08:48 +0200 hugo $
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


// input.g : input functionality


extern InputRecordToFile


int include_input

if ( {include_input} == 0 )

	include_input = 1


include xcell_name_requester.g


///
/// SH:	InputInitialize
///
/// DE:	Initialize input facilities.
///

function InputInitialize

	//- create input menu

	create neutral /input

	create neutral /input/panel

	create neutral /input/panel/ascii

	//- make it the current element

	pushe /input/panel/ascii

	//- create the form to request a compartment name

	str requestername \
		= {XCellNameRequester \
			"" \
			"Select compartment for current clamp" \
			"Select a compartment from the Purkinje cell" \
			"to inject current," \
			"or type a compartment name below :" \
			"Location for current injection : " \
			{"InputTargetSet " @ {cellpath} @ " <v> -1 -1"} \
			{"Current site is (" @ {getglobal iClampCurrentTarget} @ ")"} \
			"Done" \
			"InputTargetHideRequester"}

	addfield /input/panel/ascii \
		requestername -description "xcell name requester"

	setfield /input/panel/ascii \
		requestername {requestername}

	//- pop previous current element from stack

	pope
end


///
/// SH:	InputTargetAsk
///
/// DE: Ask for a compartment name to record in an ascii file.
///	This function messes with the xcell call back script
///

function InputTargetAsk(widget)

str widget

	//- set xcell callback

	XCellGlobalElectrodeAddCallback {"ISOLATE"}

	XCellGlobalElectrodeAddCallback \
		{"InputTargetSet" \
			@ "_" \
			@ {cellpath}}

	//- pop add plot form

	str requestername = {getfield /input/panel/ascii requestername}

	XCellNameRequesterShow {requestername}

end


///
/// SH:	InputTargetHideRequester
///
/// DE:	Hide the requester
///	Resets the xcell call back
///

function InputTargetHideRequester

	//- restore xcell callbacks

	XCellGlobalElectrodePopCallback
	XCellGlobalElectrodePopCallback

	//- hide add plot window

	str requestername = {getfield /input/panel/ascii requestername}

	XCellNameRequesterHide {requestername}
end


///
/// SH:	InputTargetSet
///
/// DE: Convert a given compartment name and record its simulation
/// sequence.
///

function InputTargetSet(solver,name,color,cell)

str solver
str name
int color
str cell

	//- for empty given name

	if ( {name} == "" )

		//- just return

		return
	end

	//- update the target name in the requester

	str requestername = {getfield /input/panel/ascii requestername}

	XCellNameRequesterSetWarning {requestername} {"Current site is (" @ {name} @ ")"}

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

	SettingsIClampSetTarget {name}
end


InputInitialize


end


