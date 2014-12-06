//genesis
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: passive.g 1.5 Wed, 09 Aug 2006 21:07:02 +0200 hugo $
//

//////////////////////////////////////////////////////////////////////////////
//'
//' Purkinje tutorial
//'
//' (C) 1998-2002 BBF-UIA
//'
//' see our site at http://www.tnb.ua.ac.be/ for more information regarding
//' the Purkinje cell and genesis simulation software.
//'
//'
//' functional ideas ... Erik De Schutter, erik@tnb.ua.ac.be
//' genesis coding ..... Hugo Cornelis, hugo@tnb.ua.ac.be
//'
//' general feedback ... Reinoud Maex, Erik De Schutter, Dave Beeman, Volker Steuber, Dieter Jaeger, James Bower
//'
//////////////////////////////////////////////////////////////////////////////

int hines_cell

if ( {hines_cell} == 0 )

	hines_cell = 1


//- include default definitions from neurokit, overwrites cellpath, so
//- we reset the cellpath afterwards ... hack hack, one hour lost :-)

include defaults.g

//- set cellpath

str cellpath = {argv 1}

//- set constants

float RA = 0.5
float RM = 10
float CM = 0.01
float EREST_ACT = -0.065

float dt = 2.0e-5

//- create prototype library

include compartments

ce /library

make_cylind_compartment			/* makes "compartment" */
make_sphere_compartment			/* makes "compartment_sphere" */


// cell with spines

str cellfile = "library/cells/example/example.p"


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
///

function CellInit

	//- initialize actions : iclamp switch always there

	ActionsInit 1 1 0 1 1 1

	//- initialize boundaries

	BoundsInit "bounds.txt"

end


//- set global variables that signal xcell what is possible

addglobal int "cell_supports_exc_ige" 0
addglobal int "cell_supports_inh_ige" 0

addglobal str "xcell_transform_mode" "y"


end


