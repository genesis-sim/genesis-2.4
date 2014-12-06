//genesis
xcolorscale rainbow2
create xform /form [0,0,600,800]
ce /form
create xtext text [0,0,100%,20%]
	setfield text \
		fg black \
		font "7x13bold" \
		filename draws.txt

create xdraw /form/draw1 [0,0:text,100%,40%]
	setfield /form/draw1 xmin -10  xmax 10 ymin -5 ymax 15 \
	script "echo clicked on <w> at <x> <y> <z>; echo.p p; echo.w w"
ce /form/draw1
create xshape lines -coords "[0,0,0][1,1,2][-2,1,1][-2,-1,-1]" \
	-fg red -ty -1 -tx -8 \
	-drawmode DrawLines -text lines -textcolor red \
	-script "do_move.w <s> <d> <x> <y> <z>"

create xshape segs -coords "[0,0,0][1,1,1][-2,1,-1][-2,-1,2]" \
	-fg green -ty 2 -tx -7 \
	-drawmode DrawSegs -text segs -textcolor green \
	-script "do_move.w <s> <d> <x> <y> <z>"

create xshape rects -coords "[0,0,0][1,1,1][-2,1,2][-4,-1,3]" \
	-fg blue -ty 4 -tx -3 \
	-drawmode FillRects -text rects -textcolor blue \
	-script "do_move.w <s> <d> <x> <y> <z>"

create xshape arrows -coords "[0,0,0][1,1,-1][-2,1,2][-2,-1,-3]" \
	-fg yellow -ty 6 -tx 0 \
		-drawmode DrawArrows -text arrows -textcolor yellow \
		-script "do_move.w <s> <d> <x> <y> <z>"

create xshape poly -coords "[0,0,0][1,1,2][-2,1,5][-2,-1,-4]" \
	-fg white -ty 8 -tx 4 \
		-drawmode FillPoly -text poly -textcolor white \
		-script "do_move.w <s> <d> <x> <y> <z>"

create xshape xax -coords [-5,0,0][0,0,0] \
	-fg red -drawmode DrawArrows \
	-text x -tx 5 -ty 0 -tz 0

create xshape yax -coords [0,-5,0][0,0,0] \
	-fg green -drawmode DrawArrows \
	-text y -tx 0 -ty 5 -tz 0

create xshape zax -coords [0,0,-5][0,0,0] \
	-fg blue -drawmode DrawArrows \
	-text z -tx 0 -ty 0 -tz 5


function do_move(src,dest,x,y,z)
	if ({strcmp {dest} "/form/draw1"} != 0)
		echo Cannot drag one pix onto another in this example
	
	else
		setfield {src} tx {x} ty {y} tz {z}
		xupdate /form/draw1
	end
end

create xdraw /form/draw2 [0,0:draw1,100%,33%] -xmax 10 -ymax 15
	setfield /form/draw2 xmin -10 ymin -5 \
	script "echo clicked on <w> at <x> <y> <z>; echo.p p"
ce /form/draw2


// create xpix pix -fg red

create xsphere sphere -fg green -r 1 -tx 3

create xplot plot -fg blue -tx 5 -ty -4
	setfield plot \
		xmax 1000 xmin 0 \
		ymax 1 ymin 0 \
		wx 5 \
		wy 5
// addmsg to plot 

create xaxis axis -fg black -axx 1 -axy 1 -axz 1 \
	-axmax 100 -axmin 0  -axlength 3  -ty -3


create xgif gif -filename xodus.gif -tx -5 -ty 2 \
	-script "echo click on <w>"

create xvar var
	setfield var color_val 1 morph_val 2 tx 5 ty 5 
	setfield var/shape[0] drawmode FillPoly fg 0 \
	coords [0,0.3,0][0,.7,0][0.3,1,0][0.7,1,0][1,0.70][1,0.3,0][0.7,0,0][0.3,0,0]
	setfield var/shape[1] drawmode FillPoly fg 60 \
 coords [0,0,0][0,.5,0][0,1,0][0.5,1,0][1,1,0][1,0.5,0][1,0,0][0.5,0,0]

	setfield var sizescale 5


// addmsg to var so it does things
create table /tab
call /tab TABCREATE 1 0 100
setfield /tab \
	stepsize 1 \
	step_mode 1 \
	table->table[0] 0 \
	table->table[1] 1
addmsg /tab /form/draw2/var VAL1 output
addmsg /tab /form/draw2/plot PLOT output *plot *blue

create table /tab2
call /tab2 TABCREATE 1 0 50
setfield /tab2 \
	stepsize 1 \
	step_mode 1 \
	table->table[0] 1 \
	table->table[1] 0
addmsg /tab2 /form/draw2/var VAL2 output

create xbutton /form/runit -script "reset; step 1000"
create xbutton /form/quit -script "quit"

xshow /form
