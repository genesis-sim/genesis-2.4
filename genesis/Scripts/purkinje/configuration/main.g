//genesis - Purkinje cell version M9 genesis2 master script
// Copyright: Theoretical Neurobiology, Born-Bunge Foundation - UA, 1998-1999.
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: main.g 1.2 Fri, 11 Aug 2006 00:32:42 +0200 hugo $
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

//- give header

echo "--------------------------------------------------------------------------"
echo
echo "Purkinje tutorial, version " -n
// $Format: "echo \"$ProjectVersion$ ($ProjectDate$)\""$
echo "Release2-2.17 (Wed, 14 Mar 2007 22:13:17 -0500)"
echo "                       Main configuration script"
echo
echo "--------------------------------------------------------------------------"


//- show the control panel

xshow /control

//- show the output panel

xshow /output/panel

//- show some of the graphics

XCellCreate "/comp" "default"

XCellCreate "/Caconcen" "default"

//- switch on current clamp

call /control/iClamp B1DOWN

//! need to synchronize, xtoggles do not follow automatically

setfield /control/iClamp state 1

