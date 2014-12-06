//genesis
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: simulation_sequence.g 1.3.1.4 Tue, 21 Feb 2006 16:51:16 +0100 hugo $
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


// simulation_sequence.g : simulation sequence abstraction

int include_simulation_sequence

if ( {include_simulation_sequence} == 0 )

	include_simulation_sequence = 1


int simulation_sequence_count = 0


///
/// SH:	SimulationSequence
///
/// DE:	Create a new simulation sequence for the given source.
///
/// Note : if you want the output element to be scheduled, a 'reset'
/// might be necessary.
///

function SimulationSequence(label,path,field)

str label

str path

str field

	//t protect path '/' or create subdirectories

	//- increment simulation sequence count

	simulation_sequence_count = {simulation_sequence_count + 1}

	//- create simulation sequence asc_file element

	create asc_file /simulation_sequences/{simulation_sequence_count}

	//! not sure yet about the options here

	setfield ^ \
		append 0 \
		filename {"simulation_sequences" @ {label}} \
		flush 1 \
		leave_open 1

	//- link the source with the asc_file element

	addmsg {path} /simulation_sequences/{simulation_sequence_count} SAVE {field}

	//- add the element to the simulation schedule

	call /simulation_sequences/{simulation_sequence_count} RESET

	resched

	//- return simulation identifier

	return {simulation_sequence_count}
end


///
/// SH:	SimulationSequence
///
/// PA: ss....:	simulation sequence identifier.
///
/// DE:	Create a new simulation sequence for the given source.
///
/// Note : if you want the output element to be scheduled, a 'reset'
/// might be necessary.
///

function SimulationSequenceStop(ss)

str ss

	//- delete the simulation sequence

	//! saves the file

	delete /simulation_sequences/{ss}
end


//- create the simulation sequence archive

create neutral /simulation_sequences

//- create archive in the file system

sh "mkdir simulation_sequences"


end


