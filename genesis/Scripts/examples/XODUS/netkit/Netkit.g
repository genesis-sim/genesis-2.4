//genesis -  Netkit example from Chapter 20 of "The Book of GENESIS"
create xform /edit [0,150,500,550]
xshow /edit
create xcoredraw /edit/lib [0,0,100%,30%] \
	-xmin -3 -xmax 3 -ymin -1  -ymax 2
create xtree /edit/lib/tree -path "/proto/##[TYPE=compartment]" \
	-treemode grid \
	-script "create_cell.w <d> <S> <x> <y>"
include cellproto.g
make_cell // This loads a cell into /proto
move /proto/cell /proto/ecell
addfield /proto/ecell transmitter
addfield /proto/ecell xtree_fg_req
setfield /proto/ecell transmitter glu  xtree_fg_req blue
copy /proto/ecell /proto/icell
setfield /proto/icell transmitter GABA xtree_fg_req orange
call /edit/lib/tree RESET
create xshape /edit/lib/tree/shape -autoindex \
	-value compartment -pixflags v -pixflags c \
	-coords [-0.5,0,0][0,0.5,0][0.5,0,0] -drawmode FillPoly
call /edit/lib/tree RESET
create xcoredraw /edit/work [0,5:lib,100%,65%] \
	-xmin -4 -xmax 4 -ymin -4  -ymax 4
create xtree /edit/work/tree -path "/net/#[TYPE=compartment]" \
	-treemode geometry \
	-script "edit_cell.D <v>; move_cell.w <d> <S> <x> <y>"
copy /edit/lib/tree/shape[1] /edit/work/tree
create neutral /net // The new cells are created on top of /net
function create_cell(dest,srcval,x,y)
	str dest,srcval
	float x,y
	// Make sure that the destination is the work window 
	if ({strcmp {dest} "/edit/work"} == 0)
		copy {srcval} /net -autoindex
		position ^ {x} {y} I
		call /edit/work/# RESET
	end
end
function move_cell(dest,srcval,x,y)
	str dest,srcval
	float x,y
	// Make sure that the destination is the work window 
	if ({strcmp {dest} "/edit/work"} == 0)
		position {srcval} {x} {y} I
		call /edit/work/# RESET
	end
end
function make_xedit_cell
	create xform /parmedit/cell [550,550,500,150]
	addfield /parmedit/cell elmpath -description "path of elm"
	ce /parmedit/cell
	create xdialog name -title "Name"
	create xdialog Em [0,0:name,50%,30] \
		-script "set_named_field <w> <v>"
	create xdialog Vm [50%,0:name,50%,30] \
		-script "set_named_field <w> <v>"
	create xdialog inject -script "set_named_field <w> <v>"
	create xbutton UPDATE [0%,0:inject,50%,30] \
		-script "do_update_cellinfo"
	create xbutton HIDE [50%,0:inject,50%,30] \
		-script "xhide /parmedit/cell"
	ce /
end
create neutral /parmedit
make_xedit_cell // actually make the cell param editor

function set_named_field(widget,value)
	str widget,value
	str elm = {getfield {widget}/.. elmpath}
	str field = {getfield {widget} name}
	setfield {elm} {field} {value}
end

function do_update_cellinfo
	str cell = {getfield /parmedit/cell elmpath}
	setfield /parmedit/cell/name value {cell}
    setfield /parmedit/cell/Em value {getfield {cell} Em }
    setfield /parmedit/cell/Vm value {getfield {cell} Vm }
    setfield /parmedit/cell/inject value {getfield {cell} inject}
end

function edit_cell(reac)
	str reac
	setfield /parmedit/cell elmpath {reac}
	do_update_cellinfo
	xshowontop /parmedit/cell
end
function connect_cells(A,B)
	str A,B
	addmsg {A}/axon {B}/{getfield {A} transmitter} SPIKE
	call /edit/work/# RESET
end
call /edit/work/tree ADDMSGARROW "/net/##[TYPE=compartment]" \
	"/net/##[TYPE=compartment]" all compartment green 0 0 \
	"connect_cells.p <S> <D>" "" ""
create xtree /edit/work/arrows -path "/net/#[]/##[]" \
	-treemode geometry \
	-namemode "none" \
	-pixflags c	
call /edit/work/arrows ADDMSGARROW \
	all all all all red 0 0 "" "" ""
setfield /edit/work/arrows/shape[0] npts 0 drawmode DrawPoints
create xform /graph [550,0,500,500]
create xgraph /graph/graph -hgeom 100% -ymin -0.1 -ymax 0.25 \
	-xmin 0 -xmax 0.5 -yoffset 0.15 \
	-script "add_plot.p <S>"
xshow /graph

function add_plot(src)
	str src
	str srcname = {getfield {src} name} @ "_" \
		@ {getfield {src} index} @ ".Vm"
	echo {srcname}
	create xplot /graph/graph/{srcname} -pixflags f
	addmsg {src} /graph/graph/{srcname} PLOT \
		Vm *{srcname} *{getfield {src} xtree_fg_req}
	useclock /graph/graph/{srcname} 2
end
setclock 0 50e-6 // a 50 usec time step
setclock 1 1e-3
setclock 2 0.2e-3
function update_time
	setfield /control/currtime value {getstat -time}
end
create xform /control [0,0,500,100]
create xbutton /control/start [0,0,33%,30] \
	-script "step 0.5 -time"
create xbutton /control/reset [0:last,0,34%,30] \
	 -script reset
create xbutton /control/stop [0:last,0,34%,30] -script stop
create xdialog /control/currtime \
	-label "Current time (sec)" -value 0
addaction /control/currtime PROCESS update_time
useclock /control/# 1
create xbutton /control/quit -script quit
xshow /control
