//genesis
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: cell_path.g 1.8 Tue, 21 Feb 2006 16:51:16 +0100 hugo $
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


// cell_path.g : output functionality


extern CellPathDendriticTips
extern CellPathEscape
extern CellPathTerminals


int include_cell_path

if ( {include_cell_path} == 0 )

	include_cell_path = 1


///
/// SH:	CellPathDendriticTips
///
/// DE:	Return a list of all dendritic tips.
///
/// First call CellPathInitialize for the given path before applying this function.
///
/// RE: List of dendritic tips.
///

function CellPathDendriticTips(path)

str path

	str result = ""

	//- loop over all compartments of the cell

	str tip

	foreach tip ( {el {cellpath}/##[][TYPE=compartment][somatofugaltag=1]} )

		//- add the tip to the result

// 		tip = {CellPathEscape {tip}}

		result = {{result} @ " " @ {tip}}
	end

	//- return result

	return {result}
end


///
/// SH:	CellPathEscape
///
/// DE: Escape characters in a string to allow it to be used as a tag
/// by this library.
///
/// RE: Escaped characters.
///

function CellPathEscape(path)

str path

	//- escape characters

	str result = {path}

	result = {strsub {result} "[" "_" -all}

	result = {strsub {result} "]" "_" -all}

	//- return result

	return {result}
end


///
/// SH:	CellPathInitialize
///
/// DE:	Initialize compartments of the given cell for path tracking.
///

function CellPathInitialize(path)

str path

	//- loop over all compartments of the cell

	str compartment

	foreach compartment ( {el {cellpath}/##[][TYPE=compartment]} )

		//- by default we assume this is the soma

		addfield {compartment} \
			somatopetal -description "towards the soma"

		addfield {compartment} \
			somatopetaltag -description "towards the soma, tag"

		setfield {compartment} \
			somatopetal "none"

		setfield {compartment} \
			somatopetaltag "1"

		//- find incoming AXIAL message (somatopetal direction)

		int found = 0

		int count = {getmsg {compartment} -count -incoming}

		int msg_number

		for (msg_number = 0 ; msg_number < count ; msg_number = msg_number + 1)

			str type = {getmsg {compartment} -type {msg_number}}

			if ( { {type} == "AXIAL" } )

				found = 1

				break
			end
		end

		//- if an AXIAL has been found

		if ( {found} )

			//- get the source of the message

			str source = {getmsg {compartment} -source {msg_number}}

			//- register the source compartment

			setfield {compartment} \
				somatopetal {source}

			setfield {compartment} \
				somatopetaltag "0"
		end

		//- by default we assume this is a dendritic tip

		addfield {compartment} \
			somatofugal -description "away from the soma"

		addfield {compartment} \
			somatofugaltag -description "away from the soma, tag"

		setfield {compartment} \
			somatofugal ""

		setfield {compartment} \
			somatofugaltag ""

		//- find incoming RAXIAL message (somatofugal direction)

		int found = 0

		int count = {getmsg {compartment} -count -incoming}

		int msg_number

		for (msg_number = 0 ; msg_number < count ; msg_number = msg_number + 1)

			str type = {getmsg {compartment} -type {msg_number}}

			if ( { {type} == "RAXIAL" } )

				found = 1

				//- get the source of the message

				str source = {getmsg {compartment} -source {msg_number}}

				//- register the source compartment

				setfield {compartment} \
					somatofugal {{getfield {compartment} somatofugal} \
							@ " " \
							@ {source}}

				setfield {compartment} \
					somatofugaltag "0"
			end
		end

		//- if an RAXIAL has not been found

		if ( ! {found} )

			//- register this fact

			setfield {compartment} \
				somatofugal "none"

			setfield {compartment} \
				somatofugaltag "1"
		end
	end
end


///
/// SH:	CellPathTerminals
///
/// DE:	Search for the first compartment of the cell.
///
/// First call CellPathInitialize for the given path before applying this function.
///
/// RE: first compartment of the cell, for cell models without an
/// axon, this is the soma.
///

function CellPathTerminals(path)

str path

	str result = ""

	//- loop over all compartments of the cell

	str tip

	foreach tip ( {el {cellpath}/##[][TYPE=compartment][somatopetaltag=1]} )

		//- add the start compartment to the result

// 		tip = {CellPathEscape {tip}}

		result = {{result} @ " " @ {tip}}
	end

	//- return result

	return {result}
end


///
/// SH:	CellPathToTerminal
///
/// DE: Determine the compartments between the given compartment and
/// the terminal, including the given compartment and the terminal.
///
/// First call CellPathInitialize for the given path before applying
/// this function.  This function will only work if you have a single
/// terminal, mostly the soma.
///
/// RE: compartment list as said.
///

function CellPathToTerminal(path)

str path

	str result = {path}

	//- loop over the somatopetal compartments

	str somatopetal = {getfield {path} somatopetal}

	while ( {exists {somatopetal}} )

		//- add the compartment to the result

		result = {{result} @ " " @ {somatopetal}}

		//- go to next compartment

		somatopetal = {getfield {somatopetal} somatopetal}
	end

	//- return result

	return {result}
end


///
/// SH:	CellPathToTerminalAverageDiameter
///
/// DE: Determine the average diameter starting from the given
/// compartment ending at the terminal.
///
/// First call CellPathInitialize for the given path before applying
/// this function.  This function will only work if you have a single
/// terminal, mostly the soma.
///
/// RE: average diameter
///

function CellPathToTerminalAverageDiameter(path)

str path

	float result = 0

	int count = 0

	//- loop over the somatopetal compartments

	str compartment

	foreach compartment ( { arglist {CellPathToTerminal {path} } } )

		//- add diameter of compartment

		float diameter = {getfield {compartment} dia}

		result = {{result} + {diameter}}

		//- count compartments in the path

		count = {{count} + 1}
	end

	//- return average result

	return {{result} / {count}}
end


///
/// SH:	CellPathToTerminalLength
///
/// DE: Determine the segment distance starting from the given
/// compartment ending at the terminal.
///
/// First call CellPathInitialize for the given path before applying
/// this function.  This function will only work if you have a single
/// terminal, mostly the soma.
///
/// RE: distance
///

function CellPathToTerminalLength(path)

str path

	float result = 0

	//- loop over the somatopetal compartments

	str compartment

	foreach compartment ( { CellPathToTerminal {path} } )

		//- add length of compartment

		float length = {getfield {compartment} len}

		result = {{result} + {length}}
	end

	//- return result

	return {result}
end


end


