//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-1997 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/
// Creates the introductory panel with info about the program and the 
// opening statement.

function a_inform(txt)
	str txt
	setfield /about/inform label {txt}
	xflushevents
end

function make_xabout
	create xform /about [10,10,780,760]
	create xtext /about/text [0,0,100%,60:NULL.bottom] \
		-filename "Kkit.COPYRIGHT"
	create xlabel /about/inform -label "Loading kinetikit version 11"
	create xbutton /about/Dismiss -script "xhide /about"
end

