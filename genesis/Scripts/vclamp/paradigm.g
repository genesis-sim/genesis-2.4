//genesis

function do_paradigm
int vclamp

setfield /cell_run_control/"Clamp voltage (mV)" value -80
call /cell_run_control/"Clamp voltage (mV)" B1DOWN
step 10 /* kludge to make sure that the reset voltage propagates. */
reset
setfield {cellpath}graph1/graph/overlay state 1
setfield {cellpath}graph1/graph overlay 1
setfield {cellpath}graph2/graph/overlay state 1
setfield {cellpath}graph2/graph overlay 1

for (vclamp = 20 ; vclamp > -35 ; vclamp = vclamp - 10) 

	setfield /cell_run_control/"Clamp voltage (mV)" value -80
	call /cell_run_control/"Clamp voltage (mV)" B1DOWN
	reset
	step 5000
	setfield /cell_run_control/"Clamp voltage (mV)" value {vclamp}
	call /cell_run_control/"Clamp voltage (mV)" B1DOWN
	step 7000
	setfield /cell_run_control/"Clamp voltage (mV)" value -80
	call /cell_run_control/"Clamp voltage (mV)" B1DOWN
	step 3000
	reset
	color_plot 1
	color_plot 2

end

end
