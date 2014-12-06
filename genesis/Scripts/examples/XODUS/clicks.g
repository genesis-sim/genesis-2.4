//genesis
// clicks.g: demo program for Xodus events.
// By Upinder S. Bhalla Mt Sinai School of Medicine June 1995

create xform /foo [0,0,600,800]
xshow /foo
create xbutton /foo/but -script "echo test <w>"
create xtoggle /foo/tog -script "echo test <w> <v>; echo.u widget <w> is up with state <v>; echo.d <w> is down ie state = <v> "
create xdialog /foo/dia -value hello -script "echo testing <w> whose value is <v> ; echo.ka keypress a"
create xdraw /foo/draw -hgeom 100 -xmin -10 -xmax 10 -ymin -10 -ymax 10\
-script "echo test on <w> at <x> <y> <z>; echo.D D; echo.e entering draw widget; echo.l l; echo.c c; echo.drag dragging <w>; echo.drop dropping <s> onto <d> in <w>"
create xshape /foo/draw/shape -coords [-2,-2,0][2,-2,1][0,2,2][-2,-2,3]\
	-fg blue -drawmode DrawLines -text shape -value shapespal -tx -4 \
	-script "echo clicked on <w>; echo.D D; echo.c calling an XOCOMMAND in <w> with args: <v> ; echo.drag dragging <w>; echo.drop dropping <s> onto <d>; echo.w <s> was dropped onto <d>"

create xshape /foo/draw/othershape -text othershape -tx 4 \
	-coords [-3,-5,4][3,-5,4][3,5,0][-3,5,0] -drawmode FillPoly \
	-fg yellow -textcolor green -value otherval

create xbutton /foo/but2 -label "Click me to call XOCOMMAND on shape" \
	-script "call /foo/draw/shape XOCOMMAND this is a test"

create xtext /foo/text -editable 1 -hgeom 500
xtextload /foo/text \
"This is an xtext with editing enabled. " \
"You can move the cursor to the selected point using "\
"the mouse. You can also use most of the emacs commands" \
"for editing."\
"" \
"To try out other parts of this demo:" \
"Click on the button, toggle and dialog to see what they do." \
"Change the text in the dialog using the same mouse/emacs"\
"options as for this text widget"\
"Try out clicks, double-clicks, clicks with different mouse"\
"buttons, clicks and drags, etc on the draw widget and its" \
"contents. Then compare the effects with the commands used" \
"to set them up in this script file" \
"Also try using the arrow, angle-bracket, shift-arrow, "\
"x, y, z, o, p keys on the draw widget. The draw"\
"documentation explains these transformations."

create xbutton /foo/quit -script quit
