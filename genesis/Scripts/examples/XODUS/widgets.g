//genesis
// widgets.g example
// By Upinder S. Bhalla Mt. Sinai School of Medicine 1995
// this example demonstrates the simple widgets and how to
// use their script calls with the argument passing methods

function do_args(s1,s2,s3,s4,s5,s6,s7,s8,s9)
	echo args are: {s1} {s2} {s3} {s4} {s5} {s6} {s7} {s8} {s9}
end

create xform /form -xgeom 10% -ygeom 0 -wgeom 600 -hgeom 660
ce /form

create xlabel label 
	setfield label \
	fg black \
	font 9x15bold \
	label "This label does not do anything when you click on it"

create xbutton button1
setfield button1 \
	label "This button executes a script when you press it" \
	onfg seagreen \
	onbg yellow \
	offfg blue \
	script \
	"do_args on all; do_args.d1 on 1; do_args.d2 on 2; do_args.d3 on 3"

create xbutton button2 -hgeom 50
setfield button2 \
	script "do_args; do_args a b c d e ; do_args xyzzy" \
	offfont 9x15 \
	onfont 12x24 \
	onlabel "KABOOM!!!!" \
	offlabel "I am another button. Press me to see what happens..."

create xtoggle toggle
	setfield toggle script \
	"do_args on all; do_args.d1 on 1; do_args.u1 on u1; do_args.u on any up"
	setfield toggle onlabel "This toggle is now on, press to turn off" \
			offlabel "This toggle is now off, press to turn on" \
			onfg white onbg black \
			offfg blue offbg white


create xdialog dialog
	setfield dialog \
	fg black \
	font 7x13bold \
	label "This is a dialog for entering text" \
	script \
	"do_args value of <w> is <v>; do_args.d1 on 1; do_args.u1 on u1; do_args.d2 on d2"

create xdialog msgdialog
	setfield msgdialog \
	fg blue \
	label "This dialog displays a msg value. Press me to run." \
	script do_step


create table /table
call /table TABCREATE 1 0 1
setfield /table \
	step_mode 1 \
	stepsize 0.001 \
	table->table[0] 0 \
	table->table[1] 1

addmsg /table /form/msgdialog INPUT output

create ximage image 
	setfield image \
		filename xodus.gif \
		hgeom 220 wgeom 300
create xlabel bf_label -xgeom 0:image -ygeom 0:msgdialog -wgeom 300
	setfield bf_label \
		label "<--- This is an ximage displaying a gif file."

create xtext text -hgeom 200 -ygeom 0:image
	setfield text \
		fg black \
		font "7x13bold" \
		filename widgets.txt

function do_quit
	quit
end

function do_step
	step 100
end

create xbutton quit -script "echo Goodbye!! ; do_quit"
	setfield quit label "Press me to quit this example"

reset
xshow /form
