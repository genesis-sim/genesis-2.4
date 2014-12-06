//genesis
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: xcell_name_requester.g 1.7 Tue, 21 Feb 2006 16:51:16 +0100 hugo $
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


// xcellnamerequester : requester asking the user for a name of an element displayed in an xcell

int include_xcellnamerequester

if ( {include_xcellnamerequester} == 0 )

	include_xcellnamerequester = 1


int xcell_name_requester_count = 0


///
/// SH:	XCellNameRequester
///
/// DE:	Construct a name requester.
///

function XCellNameRequester(xcell, header, info1, info2, info3, name, namescript, warning, done, donescript)

str xcell
str header
str info1
str info2
str info3
str name
str namescript
str warning
str done
str donescript

	//- create a name for the requester

	str requestername = {"namerequester_" @ {xcell_name_requester_count}}

	//- create a form for setting scales

	pushe /requesters

	//- compute position of the new requester

	int x = 600

	int y = 150

	x = {{x + 0} % 1024}

	y = {{y + 100 * {xcell_name_requester_count}} % 768}

	create xform {requestername} [{x},{y},350,190] \
		-title {header}

	//- make it the current element

	pushe ^

	//- create labels with info msgs and dialog for compartment name input

	create xlabel info1 \
		-ygeom 4:last.bottom \
		-title {info1}
	create xlabel info2 \
		-ygeom 2:last.bottom \
		-title {info2}
	create xlabel info3 \
		-ygeom 2:last.bottom \
		-wgeom 100% \
		-title {info3}
	create xdialog compartmentname \
		-wgeom 100% \
		-label {name} \
		-script {namescript}
	create xlabel info4 \
		-ygeom 2:last.bottom \
		-title {warning}

	//- create cancel button

	create xbutton done \
		-title {done} \
		-script {donescript}

	//- pop previous element

	pope

	//- return result : name of requester

	pope

	//- increment the requester count

	xcell_name_requester_count = {xcell_name_requester_count + 1}

	return {requestername}
end


///
/// SH:	XCellNameRequesterHide
///
/// DE:	Hide the requester.
///

function XCellNameRequesterHide(name)

str name

	xhide /requesters/{name}

end


///
/// SH:	XCellNameRequesterSetWarning
///
/// DE:	Set warning in the requester.
///

function XCellNameRequesterSetWarning(name, warning)

str name
str warning

	setfield /requesters/{name}/info4 \
		title {warning}

end


///
/// SH:	XCellNameRequesterShow
///
/// DE:	Show the requester.
///

function XCellNameRequesterShow(name)

str name

	xshow /requesters/{name}

end


//- create the library of requesters

if (!{exists /requesters})

	create neutral /requesters
end


end


