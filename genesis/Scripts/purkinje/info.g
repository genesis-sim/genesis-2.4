//genesis
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: info.g 1.4.2.1.2.1.1.1.1.5 Tue, 21 Feb 2006 16:51:16 +0100 hugo $
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


// info.g : help/credits functionality

int include_info

if ( {include_info} == 0 )

	include_info = 1


///
/// SH:	InfoHelp
///
/// DE:	Show help
///

function InfoHelp

	//- show the help form

	xshow /info/help
end


///
/// SH:	InfoHelpCreate
///
/// PA:	parent:	parent to create help form in (ending in '/')
///
/// DE:	Create help widgets
///

function InfoHelpCreate(parent)

str parent

	//- create a form for help

	create xform {parent}help \
		-xgeom 200 \
		-ygeom 0 \
		-wgeom 600 \
		-hgeom 800

	//- make created form current element

	pushe {parent}help

	//- create title label

	create xlabel heading \
		-label "Purkinje tutorial help"

	//- create a button for closing the window

	create xbutton close \
		-ygeom 0:parent.bottom \
		-title "Close" \
		-script "xhide "{parent}"help"

	//- create text widget for help

	create xtext help \
		-ygeom 0:heading.bottom \
		-hgeom 0:close.top \
		-filename "help.txt"

	//- go to previous current element

	pope
end


///
/// SH:	InfoCredits
///
/// DE:	Show the credits
///

function InfoCredits

	//- show the credits form

	xshow /info/credits
end


///
/// SH:	InfoCreditsCreate
///
/// PA:	parent:	parent to create credits form in (ending in '/')
///
/// DE:	Create credits widgets
///

function InfoCreditsCreate(parent)

str parent

	//- create a form for the credits

	create xform {parent}credits \
		-xgeom 200 \
		-ygeom 330 \
		-wgeom 450 \
		-hgeom 230

	//- make created form current element

	pushe {parent}credits

	//- create title label

	create xlabel heading \
		-label "Genesis script credits"

	//- create a button for closing the window

	create xbutton close \
		-ygeom 0:parent.bottom \
		-title "Close" \
		-script "xhide "{parent}"credits"

	//- create text widget for credits

	create xtext credits \
		-ygeom 0:heading.bottom \
		-hgeom 0:close.top \
		-filename "credits.txt"

	//- go to previous current element

	pope
end


///
/// SH:	InfoCreate
///
/// DE:	Create help and credits widgets
///

function InfoCreate

	//- create parent element for all information

	create neutral /info

	//- create widgets for help

	InfoHelpCreate /info/

	//- create widgets for credits

	InfoCreditsCreate /info/
end


end


