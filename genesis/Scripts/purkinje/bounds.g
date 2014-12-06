//genesis
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: bounds.g 1.3.1.6 Tue, 21 Feb 2006 16:51:16 +0100 hugo $
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


// bounds.g : boundaries for xcell and xgraph

int include_bounds

if ( {include_bounds} == 0 )

	include_bounds = 1


//v last read boundary file

str boundsLastFile = ""


///
/// SH:	BoundsInit
///
/// PA:	path..:	boundary file
///
/// DE:	Initialize boundaries for xcell and xgraph
///	boundaries are set as specified in {path}
///	{path} is registered in boundsLastFile if successfull
///

function BoundsInit(path)

str path

	//- give diagnostics

	echo "Setting up boundaries from file "{path}

	//- create neutral container

	create neutral /bounds

	//- make it the current element

	pushe /bounds

	//! create the boundary tab file if it does not exist

	//sh "touch bounds.tab"

	//- open the tab file

	openfile {path} r

	//- read first line from file

	str line = {readfile {path} -line}

	//- loop over all lines in the file

	//! I do not know when EOF is read (undocumented), but this seems to
	//! work

	while (line != "")

		//- extract element name

		str element = {getarg {arglist {line}} -arg 1}

		//- if element does not start with comment char

		if ({substring {element} 0 0} != "#")

			//- create element for absolute boundaries

			create neutral {element}_abs

			//- addfields for minima and maxima

			addfield {element}_abs \
				xcellmin -description \
					"XCell minimum, absolute mode"
			addfield {element}_abs \
				xcellmax -description \
					"XCell maximum, absolute mode"
			addfield {element}_abs \
				xgraphmin -description \
					"XGraph minimum, absolute mode"
			addfield {element}_abs \
				xgraphmax -description \
					"XGraph maximum, absolute mode"

			//- set the fields for maxima and minima as in the file

			setfield {element}_abs \
				xcellmin {getarg {arglist {line}} -arg 2} \
				xcellmax {getarg {arglist {line}} -arg 3} \
				xgraphmin {getarg {arglist {line}} -arg 4} \
				xgraphmax {getarg {arglist {line}} -arg 5}

			//- create element for normalized values

			create neutral {element}_norm

			//- addfields for minima and maxima

			addfield {element}_norm \
				xcellmin -description \
					"XCell minimum, normalized mode"
			addfield {element}_norm \
				xcellmax -description \
					"XCell maximum, normalized mode"
			addfield {element}_norm \
				xgraphmin -description \
					"XGraph minimum, normalized mode"
			addfield {element}_norm \
				xgraphmax -description \
					"XGraph maximum, normalized mode"

			//- set the fields for maxima and minima as in the file

			setfield {element}_norm \
				xcellmin {getarg {arglist {line}} -arg 6} \
				xcellmax {getarg {arglist {line}} -arg 7} \
				xgraphmin {getarg {arglist {line}} -arg 8} \
				xgraphmax {getarg {arglist {line}} -arg 9}
		end

		//- read next line from the file

		line = {readfile {path} -line}
	end

	//- register last boundary file

	boundsLastFile = {path}

	//- close the tab file

	closefile {path}

	//- pop previous current element

	pope
end


///
/// SH:	BoundElementName
///
/// PA:	source:	subelement of compartment
///	value.:	field of subelement
///	iMode.:	4 : absolute channel mode
///		5 : normalized channel mode
///
/// RE:	name of boundary element
///
/// DE:	Give the name for a boundary element in the given context
///

function BoundElementName(source,value,iMode)

str source
str value
int iMode

	//v name of boundary element

	str bound

	//- if chanmode is 4

	if (iMode == 4)
	
		//- construct element name with boundaries for absolute mode

		bound = "/bounds/" \
				@ {source} \
				@ "_" \
				@ {value} \
				@ "_abs"

	//- else if chanmode is 5

	elif (iMode == 5)

		//- construct element name with boundaries for normalized mode

		bound = "/bounds/" \
				@ {source} \
				@ "_" \
				@ {value} \
				@ "_norm"
	//- else

	else
		//- give diagnostics

		echo "Illegal chanmode for boundary name creation ("{iMode}")"

		//- set result : empty

		bound = ""
	end

	//- return result

	return {bound}
end


///
/// SH:	BoundsReread
///
/// DE:	Reread the boundary file registered in {boundsLastFile}
///

function BoundsReread

	//- delete root boundary element

	delete /bounds

	//- read registered boundary file

	BoundsInit {boundsLastFile}
end


end


