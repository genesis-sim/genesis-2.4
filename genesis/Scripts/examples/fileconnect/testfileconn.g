//genesis

int nx = 5
int ny = 5
int destx = 5
int desty = 5

float inj = 1e-10
float wt = 0.01

str filename = "testfileconn.wts"


setclock 0 1e-5
setclock 1 1e-4

include cellproto.g


function make_network
	createmap /proto/cell /lgn {nx} {ny} -delta 1 1 -origin 0 0
	createmap /proto/cell /v1 {destx} {desty} -delta 1 1 -origin 0 0
	fileconnect /lgn/##[TYPE=spikegen] /v1/cell[]/glu  \
		{filename} -wtscale {wt}
	useclock /lgn/## 0
	useclock /v1/## 0
end


function do_stim(v)
	str v

	// echo stimulating cell {v}

	if ({getfield {v} inject} < 1e-20)
		setfield {v} inject {inj}
	else
		setfield {v} inject 0
	end
end

function do_update_time
	setfield /control/time value {getstat -time}
end

function make_display
	create xform /lgnform [0,20%,50%,50%] -title LGN
	create xform /v1form  [50%,20%,50%,50%] -title V1
	
	create xdraw /lgnform/draw [0,30,100%,100%] \
		-xmin -1 -xmax {nx} -ymin -1 -ymax {ny}
	create xview /lgnform/draw/view -path /lgn/cell[] -field Vm \
		-script "do_stim <v>" \
		-viewmode colorview \
		-value_min[0] -0.08 -value_max[0] 0.05 \
		-sizescale 0.8
	
	create xdraw /v1form/draw [0,30,100%,100%] \
		-xmin -1 -xmax {destx} -ymin -1 -ymax {destx}

	create xview /v1form/draw/view -path /v1/cell[] -field Vm \
		-viewmode colorview \
		-value_min[0] -0.08 -value_max[0] 0.05 \
		-sizescale 0.8
	
	create xform /control [0,0,50%,20%]
	create xbutton /control/start [0,0,33%,30] \
		-script "step 1 -t" -offbg green
	create xbutton /control/stop [0:last,0,34%,30] \
		-script stop -offbg yellow
	create xbutton /control/reset [0:last,0,33%,30] \
		-script reset -offbg red
	create xbutton /control/clear \
		-label "Clear all inputs" \
		-script "setfield /lgn/#[] inject 0"
	create xdialog /control/time \
		-label "Current time"
	create script_out /output/update_time
	create xbutton /control/quit -script quit
	setfield /output/update_time command do_update_time
	useclock /output/update_time 1
	
	xshow /lgnform
	xshow /v1form
	xshow /control
	xcolorscale hot
	disable /control
	useclock /lgnform/## 1
	useclock /v1form/## 1
end

make_cell
make_network

make_display
reset
