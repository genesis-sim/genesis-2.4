//genesis
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: purkinje_eds1994.g 1.10.1.3.2.4.1.2.1.12 Wed, 09 Aug 2006 21:07:02 +0200 hugo $
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

int include_purkinje_eds1994

if ( {include_purkinje_eds1994} == 0 )

	include_purkinje_eds1994 = 1


//- include default definitions from neurokit, overwrites cellpath, so
//- we reset the cellpath afterwards ... hack hack, one hour lost :-)

include defaults.g

//- set cellpath

str cellpath = {argv 1}

//- include Purkinje cell constants

include Purk_const

//- ensure that all elements are made in the library

ce /library

/* userprefs is for loading the preferred set of prototypes into
** the library and assigning new values to the defaults.
** A customised copy of userprefs.g usually lives in  the local
** directory where the simulation is going to be run from */

//- include scripts to create the prototypes

include Purk_chan
include Purk_cicomp
include Purk_syn


// multiple cell files defined with last one that will be read in
// make no assumptions a good working of the tutorial when changing the cell
// file. Try and see.

// cell without spines

str cellfile = "library/cells/purkinje_eds1994/Purk2M9.p"

// cell with spines

str cellfile = "library/cells/purkinje_eds1994/Purk2M9s.p"

// cut off from previous cell
// this cell can automatically fire even without external activation

//str cellfile = "library/cells/purkinje_eds1994/psmall.p"

//- make prototypes of channels and synapses

make_Purkinje_chans
make_Purkinje_syns

//- set the firing frequencies at the library level
//- this forces the hsolve object to reserve space for these fields

setfield /library/GABA frequency {ihertz}
setfield /library/non_NMDA frequency {phertz}

//- add fields to the library elements to respect the vivo / vitro mode

addfield /library/GABA freqmode -description "frequency operation mode"
addfield /library/non_NMDA freqmode -description "frequency operation mode"

//- set the added fields to the current vivo/vitro mode

setfield /library/GABA freqmode {iVVMode}
setfield /library/non_NMDA freqmode {iVVMode}

//- add fields to distinguish between asynchronous inhibition and excitation
//- and synchronous activation

addfield /library/GABA synmode -description "synaptic mode"
addfield /library/non_NMDA synmode -description "synaptic mode"
addfield /library/GABA2 synmode -description "synaptic mode"
addfield /library/non_NMDA2 synmode -description "synaptic mode"

//- set field to "in" for asynchronous inhibition

//! normally the stellate synapses

setfield /library/GABA synmode "in"

//- set field to "ex" for asynchronous excitation

//! normally the parallel fiber synapses

setfield /library/non_NMDA synmode "ex"

//- set field for synchronous activation

//! normally the basket axon synapses

setfield /library/GABA2 synmode "synchro"

//! normally the climbing fiber

setfield /library/non_NMDA2 synmode "synchro"

//- make prototypes of compartements and spines

make_Purkinje_comps
make_Purkinje_spines

//- switch to the path where the cell will be created

create neutral {cellpath}

ce {cellpath}


// from here on : more high level things related to the experiments

include actions.g

include bounds.g


///
/// SH:	CellInit
///
/// DE:	Initialize interactive actions for this particular cell.
///	Initializes messages for climbing fiber and for synchronous firing 
///	of parallel fibers.
///

function CellInit

	//- initialize actions : all of them

	ActionsInit 0 0 0 0 0 0

	//- initialize boundaries

	BoundsInit "bounds.txt"

end


//- set global variables that signal xcell what is possible

addglobal int "cell_supports_exc_ige" 1
addglobal int "cell_supports_inh_ige" 1

addglobal str "xcell_transform_mode" "y"


end


