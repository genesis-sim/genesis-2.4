//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-1997 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/


/******************************************************************
      Stuff for connecting xtabs
	  For now we will restrict their action to pools and
	  other xtabs.
******************************************************************/

// Attaches the xtab to a pool
function xtab_to_pool_add(xtab,pool)
	str xtab,pool
	int is_running = {getfield {xtab} is_running}
	int slave_enable = {getfield {pool} slave_enable} & 4

	if (is_running == 3)
		slave_enable = slave_enable | 2
	end
	if (is_running == 1)
		slave_enable = slave_enable | 1
	end
		
	echo in xtab_to_pool_add with {xtab}, {pool}
	addmsg {xtab} {pool} SLAVE output
	setfield {pool} slave_enable {slave_enable}
	call /edit/draw/tree RESET
end

// Detaches the xtab from the pool
// This assumes that only 1 xtab at a time goes to a pool,
// since it disables slaving
function xtab_to_pool_drop(xtab,pool)
	str xtab,pool
	int slave_enable = {getfield {pool} slave_enable} & 4
	
	echo in xtab_to_pool_drop with {xtab}, {pool}
	deletemsg {pool} 0 -find {xtab} SLAVE
	setfield {pool} slave_enable {slave_enable}
	call /edit/draw/tree RESET
end

// Attaches the xtab to a xtab
function xtab_to_xtab_add(src,dest)
	str src,dest
	
	echo in xtab_to_xtab_add with {src}, {dest}
	addmsg {src} {dest} INPUT output
	call /edit/draw/tree RESET
end

// Detaches the xtab from the xtab
// since it disables slaving
function xtab_to_xtab_drop(src,dest)
	str src,dest
	
	echo in xtab_to_xtab_drop with {src}, {dest}
	deletemsg {dest} 0 -find {src} INPUT
	call /edit/draw/tree RESET
end

// Attaches a pool to a xtab for saving output for ascii dumps
function pool_to_xtab_add(src,dest)
	str src,dest
	str fieldname = {getfield {src} plotfield}
	
	echo in pool_to_xtab_add with {src}, {dest}
	addmsg {src} {dest} INPUT {fieldname}
	setfield {dest} step_mode 3 // This does buffering
	call /edit/draw/tree RESET
end

// Detaches the pool from the xtab
function pool_to_xtab_drop(src,dest)
	str src,dest
	
	echo in pool_to_xtab_drop with {src}, {dest}
	deletemsg {dest} 0 -find {src} INPUT
	setfield {dest} step_mode 1 // This loops. It is the default.
	call /edit/draw/tree RESET
end

/******************************************************************
      Stuff for editing xtab
******************************************************************/

function tab_autoscale_stimplot
	str plot = "/parmedit/xtab_stim/tabform/shape"
	int npts = {getfield {plot} npts}
	float xmin, xmax
	float ymin, ymax = 0

	xmin = 0
	// xmax = {getfield /parmedit/xtab_stim/tabform xmax}
	xmax = npts
	ymin = 0
	int i
	for (i = 0; i < npts; i = i + 1)
		if (ymax < {getfield {plot} ypts->table[{i}]})
			ymax = {getfield {plot} ypts->table[{i}]}
		end
	end
	if (ymax == 0)
		ymax = 1.0
	else
		ymax = ymax * 1.2 // a bit of headroom
	end
	setfield /parmedit/xtab_stim/tabform/shape xpts->table[0] {xmin}
	setfield /parmedit/xtab_stim/tabform/shape xpts =i={(xmax-xmin)/npts}
	float x = {getfield /parmedit/xtab_stim/X1 value}
	float y = {getfield /parmedit/xtab_stim/Y1 value}
	setfield /parmedit/xtab_stim/tabform/pt1 tx {x / xmax}
	setfield /parmedit/xtab_stim/tabform/pt1 ty {y / ymax}

	x = {getfield /parmedit/xtab_stim/X2 value}
	y = {getfield /parmedit/xtab_stim/Y2 value}
	setfield /parmedit/xtab_stim/tabform/pt2 tx {x / xmax}
	setfield /parmedit/xtab_stim/tabform/pt2 ty {y / ymax}
	setfield /parmedit/xtab_stim/tabform xmin {xmin} xmax {xmax} \
		ymin {ymin} ymax {ymax}
	xupdate /parmedit/xtab_stim/tabform
end

function make_xtab_sine
	float phase = {getfield /parmedit/xtab_stim/phase value}
	float period = {getfield /parmedit/xtab_stim/period value}
	float yoff = {getfield /parmedit/xtab_stim/sine_yoff value}
	float yscale = {getfield /parmedit/xtab_stim/sine_yscale value}
	int npts = {getfield /parmedit/xtab_stim/tabform/shape npts}
	int xmax = npts
	float dtheta = 2.0 * PI * xmax / (period * npts)
	int i
	float theta = PI * phase/180

	for (i = 0; i <= npts; i = i + 1)
		setfield /parmedit/xtab_stim/tabform/shape ypts->table[{i}] {{sin {theta}} * yscale + yoff}
		theta = theta + dtheta
	end
	tab_autoscale_stimplot
end

function make_straightline
	float base = {getfield /parmedit/xtab_stim/baselevel value}
	float slope = {getfield /parmedit/xtab_stim/slope value}
	float xmax = {getfield /parmedit/xtab_stim/tabform xmax}
	float npts = {getfield /parmedit/xtab_stim/tabform/shape npts}
	setfield /parmedit/xtab_stim/tabform/shape ypts =i={slope * xmax / npts}
	setfield /parmedit/xtab_stim/tabform/shape ypts =+={base}
	tab_autoscale_stimplot
end

function do_update_xtab_stim
	str xtab = {getfield /parmedit/xtab elmpath}

	int is_running = {getfield {xtab} is_running}

	ce /parmedit/xtab_stim
	// setfield loop_start value {getfield {xtab} input}
	setfield is_running state {is_running & 1}
	setfield n_or_conc state {1 - (is_running & 2) / 2 }
	ce /
end

/* assumes that the current directory is the same as the xtabinfo,
** and that the name is current */
function do_update_xtabinfo
	str xtab = {getfield /parmedit/xtab elmpath}
	float ymin, ymax, temp
	int mode = {getfield {xtab} step_mode}
	str name = {getfield /parmedit/xtab/namearray n{mode}}

	if ({getfield {xtab} alloced} == 0)
		call {xtab} TABCREATE 100 0 100
	end

	do_update_generic /parmedit/xtab

	setfield /parmedit/xtab/xdivs value {getfield {xtab} table->xdivs}
	setfield /parmedit/xtab/dx value {getfield {xtab} table->dx}
	int start = -{getfield {xtab} input}
	setfield /parmedit/xtab/loop_start value {start}
	setfield /parmedit/xtab/loop_duration value {getfield {xtab} table->xmax}
	setfield /parmedit/xtab/#[TYPE=xtoggle] state 0
	setfield /parmedit/xtab/{name} state 1


	// Calculate table ranges and assign plot.
	call {xtab} TABOP m
	ymin = {getfield {xtab} output}
	call {xtab} TABOP M
	ymax = {getfield {xtab} output}
	if (ymin >= ymax)
		temp = ymax
		ymax = 100.0 + ymin
		ymin = temp
	end

	setfield /parmedit/xtab/graph/plot \
		npts {getfield {xtab} table->xdivs}
	setfield /parmedit/xtab/graph/plot xpts ===0
	float dx = {getfield {xtab} table->dx}
	setfield /parmedit/xtab/graph/plot xpts =i={dx}
	setfield /parmedit/xtab/graph/plot ypts {xtab}/table

	setfield /parmedit/xtab/graph \
		xmin {getfield {xtab} table->xmin} \
		xmax {getfield {xtab} table->xmax} \
		ymin {ymin} \
		ymax {ymax}

	xupdate /parmedit/xtab/graph
	do_update_xtab_stim
end

function set_xtab_slave_enables
	str xtab = {getfield /parmedit/xtab elmpath}
	int do_stim = {getfield /parmedit/xtab_stim/is_running state}
	int do_num = {getfield /parmedit/xtab_stim/n_or_conc state}
	int slave_enable = do_num + 1
	int pool_s_e
	if (!do_stim)
		slave_enable = 0
	end
	setfield {xtab} is_running {do_stim + 2 - 2 * do_num}

	// setting the color of the xtab
	if (do_stim)
        setfield {xtab} xtree_textfg_req red
    else
        setfield {xtab} xtree_textfg_req green
    end

	/* going through the destination pools and setting their
	** slave_enable flags */
	int	i
	int nmsgs
	str src
	nmsgs = {getmsg {xtab} -outgoing -count}
	for (i = 0 ; i < nmsgs ; i = i + 1)
		src = {getmsg {xtab} -outgoing -destination {i}}
		if ({exists {src} slave_enable})
			pool_s_e = {getfield {src} slave_enable} & 4
			setfield {src} slave_enable {slave_enable | pool_s_e}
		end
	end
end

/* here we have to scale according to the graph limits */
function selectpt(x,y)
	float x, y

	str xtab = {getfield /parmedit/xtab elmpath}
	float xmin = {getfield /parmedit/xtab_stim/tabform xmin}
	float xmax = {getfield /parmedit/xtab_stim/tabform xmax}
	float ymin = {getfield /parmedit/xtab_stim/tabform ymin}
	float ymax = {getfield /parmedit/xtab_stim/tabform ymax}

	int rx = {round {xmin + (x - xmin) * (xmax - xmin)}}
	y = ymin + (y - ymin) * (ymax - ymin)

	if ((rx >= 0) && (rx <= 100))
		setfield {xtab} \
			table->table[{rx}] {y} \
			last_x {rx} \
			last_y {y}
		float x2 = {getfield /parmedit/xtab_stim/X1 value}
		float y2 = {getfield /parmedit/xtab_stim/Y1 value}
		setfield /parmedit/xtab_stim/tabform/shape ypts->table[{rx}] {y}
		setfield /parmedit/xtab_stim/X2 value {x2}
		setfield /parmedit/xtab_stim/Y2 value {y2}
		setfield /parmedit/xtab_stim/X1 value {rx}
		setfield /parmedit/xtab_stim/Y1 value {y}

		setfield /parmedit/xtab_stim/tabform/pt1 tx {rx/xmax}
		setfield /parmedit/xtab_stim/tabform/pt1 ty {y/ymax}
		setfield /parmedit/xtab_stim/tabform/pt2 tx {x2/xmax}
		setfield /parmedit/xtab_stim/tabform/pt2 ty {y2/ymax}
		xupdate /parmedit/xtab_stim/tabform
	end
end

function stepline
	float x1 = {round {getfield /parmedit/xtab_stim/X1 value}}
	float x2 = {round {getfield /parmedit/xtab_stim/X2 value}}
	float y = {getfield /parmedit/xtab_stim/Y1 value}
	int i

	if (x1 > x2)
		for (i = x2 + 1  ; i <= x1; i = i + 1)
			setfield /parmedit/xtab_stim/tabform/shape ypts->table[{i}] {y}
		end
		xupdate /parmedit/xtab_stim/tabform
	end
	if (x2 > x1)
		y = {getfield /parmedit/xtab_stim/Y2 value}
		for (i = x1 + 1  ; i <= x2; i = i + 1)
			setfield /parmedit/xtab_stim/tabform/shape ypts->table[{i}] {y}
		end
		xupdate /parmedit/xtab_stim/tabform
	end
end

function interpline
	// str xtab = {getfield /parmedit/xtab elmpath}
	float x1 = {round {getfield /parmedit/xtab_stim/X1 value}}
	float x2 = {round {getfield /parmedit/xtab_stim/X2 value}}
	float y1 = {getfield /parmedit/xtab_stim/Y1 value}
	float y2 = {getfield /parmedit/xtab_stim/Y2 value}
	int i
	float dy = y1 - y2
	float lasty
	int dx = 1

	if (x1 < x2)
		dx = -1;
	end

	dy = dy / (x1 - x2)
	lasty = y2
	for (i = x2  ; i != x1; i = i + dx)
		setfield /parmedit/xtab_stim/tabform/shape ypts->table[{i}] {lasty}
		lasty = lasty + dy * dx
	end
	// setfield /parmedit/xtab_stim/tabform/shape ypts {xtab}/table
	xupdate /parmedit/xtab_stim/tabform
end

function smoothline
	echo Smoothing not yet implemented
end

function set_tab_duration(value)
	float value
	if (value <= 0)
		do_warning 0 "Error: table loop duration must be greater than zero"
		return
	end
	str xtab = {getfield /parmedit/xtab elmpath}
	float xdivs = {getfield {xtab} table->xdivs}
	setfield {xtab} \
		table->xmax {value} \
		table->dx {value/xdivs} \
		table->invdx {xdivs / value}
end

function set_tab_start(value)
	float value
	str xtab = {getfield /parmedit/xtab elmpath}
	setfield {xtab} input {-value}
end

function set_tab_dx(dx)
	float dx
	str xtab = {getfield /parmedit/xtab elmpath}
	float xmin = {getfield {xtab} table->xmin}
	float xdivs = {getfield {xtab} table->xdivs}
	float xmax = xmin + xdivs * dx

	if (dx > 0)
		setfield {xtab} \
			table->xmax {xmax} \
			table->dx {dx} \
			table->invdx {1.0 / dx}
	else
		do_warning 0 "Cannot set dx to values < 0" 
	end
end

function set_tab_xdivs(xdivs)
	int xdivs
	str xtab = {getfield /parmedit/xtab elmpath}
	int alloced = {getfield {xtab} alloced}

	if (xdivs < 1)
		do_warning 0 "Cannot set xdivs to values < 1" 
		return
	end

	if (alloced)
		call {xtab} TABFILL {xdivs} 2
	else
		call {xtab} TABCREATE {xdivs} 0 100
	end
end

function xtab_increment(widget, incr)
	str widget
	int incr
	float xmax = {getfield /parmedit/xtab_stim/tabform xmax}
	
	int orig = {getfield /parmedit/xtab_stim/{widget} value}
	setfield /parmedit/xtab_stim/{widget} value {orig + incr}
	float x = (orig + incr) / xmax
	if ({strcmp {widget} X1} == 0)
		setfield /parmedit/xtab_stim/tabform/pt1 tx {x}
	else
		setfield /parmedit/xtab_stim/tabform/pt2 tx {x}
	end
end

function xtab_ypos(pt, value)
	int pt
	float value

	float ymax = {getfield /parmedit/xtab_stim/tabform ymax}
	setfield /parmedit/xtab_stim/tabform/pt{pt} ty {value}
end


function set_step_mode(mode)
	int mode

	str xtab = {getfield /parmedit/xtab elmpath}
	if (mode == 0)
		setfield {xtab} step_mode 2 // TAB_ONCE
	else
		setfield {xtab} step_mode 1 // TAB_LOOP
	end
end

function xtab_do_scale_y(value)
	float value
	setfield /parmedit/xtab_stim/tabform/shape ypts =*={value}
	tab_autoscale_stimplot
end

function xtab_do_offset_y(value)
	float value
	setfield /parmedit/xtab_stim/tabform/shape ypts =+={value}
	tab_autoscale_stimplot
end

/*
function xtab_do_scale_x(value)
	float value
	xmax = {getfield /parmedit/xtab_stim/tabform xmax}
	setfield /parmedit/xtab_stim/tabform xmax {xmax * value}
	tab_autoscale_stimplot
end

function xtab_do_scale(axis)
	str axis

	float x = {getfield /parmedit/xtab_stim/{axis}_scale_factor value}
	setfield /parmedit/xtab_stim/tabform/shape {axis}pts =*={x}
	tab_autoscale_stimplot
end

function xtab_do_offset(axis)
	str axis

	float x = {getfield /parmedit/xtab_stim/{axis}_offset value}
	setfield /parmedit/xtab_stim/tabform/shape {axis}pts =+={x}
	tab_autoscale_stimplot
end
*/

function xtab_do_log
	setfield /parmedit/xtab_stim/tabform/shape ypts \
		=l=/parmedit/xtab_stim/tabform/shape/ypts
	tab_autoscale_stimplot
end

function xtab_do_log10
	setfield /parmedit/xtab_stim/tabform/shape ypts \
		=L=/parmedit/xtab_stim/tabform/shape/ypts
	tab_autoscale_stimplot
end

function xtab_do_exp
	setfield /parmedit/xtab_stim/tabform/shape ypts \
		=e=/parmedit/xtab_stim/tabform/shape/ypts
	tab_autoscale_stimplot
end

function xtab_do_exp10
	setfield /parmedit/xtab_stim/tabform/shape ypts \
		=E=/parmedit/xtab_stim/tabform/shape/ypts
	tab_autoscale_stimplot
end

function tab_autoscale_plot(xtab)
	str xtab
	int xdivs = {getfield {xtab} table->xdivs}
	float xmin, xmax
	float ymin, ymax

	xmin = {getfield {xtab} table->xmin}
	xmax = {getfield {xtab} table->xmax}
	call {xtab} TABOP m
	ymin = {getfield {xtab} output}
	call {xtab} TABOP M
	ymax = {getfield {xtab} output}
	// setfield /parmedit/xtab/graph/plot allocated_pts {xdivs}
	setfield /parmedit/xtab/graph/plot npts {xdivs}
	setfield /parmedit/xtab/graph/plot xpts->table[0] {xmin}
	setfield /parmedit/xtab/graph/plot xpts =i={(xmax-xmin)/xdivs}
	setfield /parmedit/xtab/graph/plot ypts {xtab}/table

	setfield /parmedit/xtab/graph xmin {xmin} xmax {xmax} \
		ymin {ymin} ymax {ymax}
end

function xtab_apply_scale
	str xtab = {getfield /parmedit/xtab elmpath}
	float xmin = {getfield /parmedit/xtab_stim/tabform/shape xpts->table[0]}
	int xdivs = {getfield /parmedit/xtab_stim/tabform/shape npts}
	float xmax = \
		{getfield /parmedit/xtab_stim/tabform/shape xpts->table[{xdivs}]}
	setfield {xtab} table /parmedit/xtab_stim/tabform/shape/ypts
	setfield {xtab} table->xmin {xmin}
	setfield {xtab} table->xmax {xmax}
	tab_autoscale_plot {xtab}
	xhide /parmedit/xtab_stim
	xshow /parmedit/xtab
end

function xtab_undo_scale
	str xtab = {getfield /parmedit/xtab elmpath}
	setfield /parmedit/xtab_stim/tabform/shape ypts {xtab}/table
	tab_autoscale_stimplot
end

function xtab_load_file
	str filename = {getfield /parmedit/xtab_save_data/filename value}
	str xtab = {getfield /parmedit/xtab elmpath}
	int skiplines = {getfield /parmedit/xtab_save_data/skip_line value}
	int xdivs = {getfield /parmedit/xtab/xdivs value}
	int is_xy_file = {getfield /parmedit/xtab_save_data/fileformat state}
	float xmin, xmax

	if (is_xy_file)
	file2tab {filename} {xtab} table -skiplines {skiplines} -xy {xdivs}
	else
		file2tab {filename} {xtab} table -skiplines {skiplines}
	end
	// something here to update the graph
	tab_autoscale_plot {xtab}
	xshow /parmedit/xtab
	xhide /parmedit/xtab_save_data
	do_inform "Loaded file "{filename}" into "{xtab}
end

function xtab_save_file
	str filename = {getfield /parmedit/xtab_save_data/filename value}
	str xtab = {getfield /parmedit/xtab elmpath}
	int skiplines = {getfield /parmedit/xtab_save_data/skip_line value}
	int npts = {getfield {xtab} table->xdivs}
	int is_xy_file = {getfield /parmedit/xtab_save_data/fileformat state}
	float xmin, xmax

	if (is_xy_file)
		tab2file {filename} {xtab} table -mode xy -nentries {npts}
	else
		tab2file {filename} {xtab} table -mode y -nentries {npts}
	end
	do_inform "Saved table "{xtab}" into file "{filename}
end

function select_tabmode(mode, widget)
	int mode
	str widget
	str name = {getfield /parmedit/xtab/namearray n{mode}}
	str label = {getfield /parmedit/xtab/namearray l{mode}}

	str xtab = {getfield /parmedit/xtab elmpath}
	setfield {xtab} step_mode {mode}
	
	setfield {widget}/../#[TYPE=xtoggle] state 0
	setfield {widget} state 1

	// Call the update func for the particular mode
	xhide {widget}/..
	if ({strncmp {name} "stim" 4} == 0)
		setfield /parmedit/xtab_stim/loop_or_once state {mode - 1}
		setfield /parmedit/xtab_stim/title label {xtab}" : "{label}
		xtab_undo_scale
		xshow /parmedit/xtab_stim
	else
		setfield /parmedit/xtab_{name}/title label {xtab}" : "{label}
		xshow /parmedit/xtab_{name}
	end
end

function make_xtab_common(name, height)
	create xform /parmedit/{name} [{EX},{EY},{EW},{height}]
	ce ^
	create xlabel title
	int dh = 14

    create xbutton UPDATE [0,{height - dh - 30},30%,30] \
        -script "do_update_"{name}"info"
    create xbutton DISMISS [0:last,{height - dh - 30},30%,30] \
        -script {"xhide " @ {el .}} 
    create xbutton RETURN [0:last,{height - dh - 30},40%,30] \
        -script xtab_apply_scale
end

function make_xtab_lookup
	make_xtab_common xtab_lookup 400
	// Need to put in options a way of specifying lookup rather than
	// the default buffer mode when dragging pool to table.
end

function set_stim_loop(state)
	int state

	str xtab = {getfield /parmedit/xtab elmpath}

	if (state)
		setfield {xtab} step_mode 2 // once_mode
	else
		setfield {xtab} step_mode 1 // loop mode
	end
end

function make_xtab_stim
	make_xtab_common xtab_stim 670
	setfield /parmedit/xtab_stim y 0
	// Set up stim on/off, # vs conc, stepsize, start time, access to
	// plot editor shared with stim_once
	create xtoggle is_running [0,0:title,100%,] \
		-onlabel "Stimulus ON: click to stop" \
		-offlabel "Stimulus OFF: click to start" \
		-script "set_xtab_slave_enables"
	create xtoggle n_or_conc \
		-offlabel "Output is #" -onlabel "Output is conc" \
		-script set_xtab_slave_enables
	create xtoggle loop_or_once [0,0:n_or_conc,100%,] \
		-offlabel "Stimulus will loop" \
		-onlabel "Stimulus will be delivered once" \
		-script "set_stim_loop <v>"

	create xlabel stimsetup -label "Stimulus waveform" -bg cyan
	create xgraph tabform [0,0:last,100%,180] \
		-xmin 0 -xmax 100 -ymin 0 -ymax 1.0 \
		-script "selectpt.d1 <x> <y>" 
	create xplot tabform/shape -npts 100 -fg red -ysquish 0
	create xsphere tabform/pt1 -r 0.01 -fg green
	create xsphere tabform/pt2 -r 0.01 -fg blue

	setfield tabform/shape xpts =i=1
	setfield tabform/x_axis pixflags c
	setfield tabform/y_axis pixflags c

	create xlabel predef -label "Predefined waveforms" -bg cyan
	create xlabel flatline [0,0:predef,20%,30] -bg cyan \
		-label "Straight line"
	create xdialog baselevel -label "Baseline level" \
		[0:last,0:predef,40%,30] -value 0 \
		-script "make_straightline"
	create xdialog slope -label "Slope" \
		[0:last,0:predef,40%,30] -value 0 \
		-script "make_straightline"
	create xlabel sine -label "Sine Wave" [0,0:flatline,20%,60] -bg cyan
	create xdialog period -label "Period" [0:last,0:flatline,40%,30] \
		-script make_xtab_sine -value 100
	create xdialog phase -label "Phase (deg)" \
		[0:last,0:flatline,40%,30] \
		-script make_xtab_sine -value 0
	create xdialog sine_yoff [0:sine,0:period,40%,30] \
		-label "y offset" \
		-script make_xtab_sine -value 1
	create xdialog sine_yscale [0:last,0:period,40%,30] \
		-label "y scale" \
		-script make_xtab_sine -value 1
	create xlabel interplab -label "Interpolation options" -bg cyan
	create xdialog X1 [0,0:interplab,20%,30] -value 0 \
		 -script "xtab_increment X1 0"
	create xbutton x1up [0:last,0:interplab,5%,30] -title "+" \
		 -script "xtab_increment X1 1"
	create xbutton x1dn [0:last,0:interplab,5%,30] -title "-" \
		 -script "xtab_increment X1 -1"
	setfield ^ title "-" // this fools the parser on the create line
	create xdialog Y1 [0:last,0:interplab,20%,30] -value 0 \
		-script "xtab_ypos 1 <v>"
	create xdialog X2 [0:last,0:interplab,20%,30] -value 0 \
		 -script "xtab_increment X2 0"
	create xbutton x2up [0:last,0:interplab,5%,30] -title "+" \
		 -script "xtab_increment X2 1"
	create xbutton x2dn [0:last,0:interplab,5%,30] -title "-" \
		 -script "xtab_increment X2 -1"
	setfield ^ title "-" // this fools the parser on the create line
	create xdialog Y2 [0:last,0:interplab,20%,30] -value 0 \
		-script "xtab_ypos 2 <v>"
	create xbutton Step [0,0:Y2,33%,25] -script stepline
	create xbutton Interpolate [0:last,0:Y2,34%,25] -script interpline
	create xbutton Smooth [0:last,0:Y2,33%,25] -script smoothline
	create xlabel scale_title -label "Scaling options" -bg cyan
	create xdialog y_scale_factor [0,0:scale_title,50%,30] -value 1 \
		-script "xtab_do_scale_y <v>"
	create xdialog y_offset [50%,0:scale_title,50%,30] -value 0 \
		-script "xtab_do_offset_y <v>"
	/*
	create xdialog x_scale_factor [0,0:y_offset,50%,30] -value 1 \
		-script "xtab_do_scale x"
	create xdialog x_offset [50%,0:y_offset,50%,30] -value 0 \
		-script "xtab_do_offset x"
	*/
	create xbutton Log [0,0:y_offset,25%,25] -script "xtab_do_log"
	create xbutton Exp [0:last,0:y_offset,25%,25] -script "xtab_do_exp"
	create xbutton Log10 [0:last,0:y_offset,25%,25] -script "xtab_do_log10"
	create xbutton Exp10 [0:last,0:y_offset,25%,25] -script "xtab_do_exp10"
	create xbutton Apply [0,0:Log,50%,25] -script "xtab_apply_scale"
	create xbutton Undo [0:last,0:Log,50%,25] -script "xtab_undo_scale"
	ce /
end

function make_xtab_save
	make_xtab_common xtab_save_data 210
	// Need to set timestep for table so it fills up at a suitable rate.
	// Should have a little NOTES here that explains what it does.
	create xlabel fileinfo [0,25,,] -bg cyan -label "File I/O"
	create xdialog filename \
		-label "File name" -value "plot.data"
	create xtoggle fileformat \
		-offlabel "File format: y values only"  \
		-onlabel "File format: x y values"
	create xdialog skip_line \
		-label "Number of lines to skip at file start:" -value 0
	create xbutton Load [0,0:skip_line,50%,30] \
		-script xtab_load_file
	create xbutton Save [0:last,0:skip_line,50%,30] \
		-script xtab_save_file
end

function make_xtab_thresh
	make_xtab_common xtab_threshold 400
end

function make_xedit_xtab
	int i
	str name
	str label
	make_edit_common xtab 460

	create xlabel modelabel [0,0:path,20%,30] -label "Mode select:" -bg white
	// stepmodes: 0: lookup, 1: stim_loop, 2: stim_once, 3: buffer, 4: spike

	create neutral namearray
	for (i = 0; i < 5 ; i = i + 1)
		addfield namearray n{i}
		addfield namearray l{i}
	end
	setfield namearray n0 "lookup"
	setfield namearray n1 "stim_loop"
	setfield namearray n2 "stim_once"
	setfield namearray n3 "save_data"
	setfield namearray n4 "threshold"
	setfield namearray l0 "Lookup"
	setfield namearray l1 "Stim loop"
	setfield namearray l2 "Stim once"
	setfield namearray l3 "File I/O"
	setfield namearray l4 "Threshold"
	for (i = 0; i < 5 ; i = i + 1)
		name = {getfield namearray n{i}}
		label = {getfield namearray l{i}}
		create xtoggle {name} [0:last,0:path,16%,30] -label {label} \
			-script "select_tabmode "{i}" <w>"
	end
	
	create xgraph graph [0,0:lookup,100%,170]
	create xplot graph/plot -npts 1000
	create xdialog xdivs [0,0:graph,50%,30] -label "Table size" \
		-script "set_tab_xdivs <v>; do_update_xtabinfo"
	create xdialog dx [0:last,0:graph,50%,30] \
		-label "Table increment" \
		-script "set_tab_dx <v>; do_update_xtabinfo"
	create xdialog loop_duration [0,0:xdivs,50%,30] \
		-label "Loop Duration" \
		-script "set_tab_duration <v>; do_update_xtabinfo"
	create xdialog loop_start [0:last,0:xdivs,50%,30] \
		-label "Loop Start" \
		-script "set_tab_start <v>; do_update_xtabinfo"

	make_xtab_lookup
	make_xtab_stim
	make_xtab_save
	make_xtab_thresh
end

function edit_xtab(xtab)
	str xtab

	setfield /parmedit/xtab elmpath {xtab}
	do_update_xtabinfo
	xshowontop /parmedit/xtab
end

/******************************************************************
      Stuff for initializing xtabs
******************************************************************/

function ktabproto
	create table /xtab

	// need to extend the RESET action so that the input is zeroed too

	create text /xtab/notes
	addfield /xtab notes
    addfield /xtab editfunc -description "func for xtab edit"
    addfield /xtab xtree_fg_req -description "color for xtab icon"
    addfield /xtab xtree_textfg_req -description "color for xtab icon"
    addfield /xtab plotfield -description "field to plot"
    addfield /xtab manageclass -description "Class that can manage it"
    addfield /xtab baselevel -description "base level for xtab"
    addfield /xtab last_x -description "last x point clicked on"
    addfield /xtab last_y -description "last y point clicked on"
    addfield /xtab is_running -description "flag for state of xtab"
 
    addobject xtab /xtab \
        -author "Upi Bhalla Mt Sinai Apr 1994" \
        -description "xtab plus a notes field"
	setdefault xtab step_mode 1
	setdefault xtab stepsize 0
    setdefault xtab editfunc "edit_xtab"
    setdefault xtab xtree_fg_req "white"
    setdefault xtab xtree_textfg_req "red"
    setdefault xtab plotfield "output"
    setdefault xtab manageclass "group"
    setdefault xtab is_running "0"
	setdefault xtab baselevel 0
	setdefault xtab last_x 0
	setdefault xtab last_y 0
end

function xtabproto
	ce /control/lib/tree
	create xshape shape -autoindex \
		-fg red \
		-drawmode DrawLines \
		-linewidth 2 \
		-coords [-.6,0.5,0][-.2,.9,0][.6,.9,0][.4,.7,0] \
			[.6,0.5,0][.4,.7,0][.2,.5,0][0,.5,0][.2,.1,0] \
			[0,.5,0][-.4,.5,0][-.6,.1,0][-.4,.5,0][-.6,.5,0] \
		-text "xtab" -textmode draw -textcolor black \
		-value "xtab" \
		-pixflags v \
		-pixflags c \
		-script \
		"edit_xtab.D"

	copy {el ^} /edit/draw/tree -autoindex
	setfield ^ script "edit_xtab.D"
	ce /
end


function init_xtab
    // set up the prototype
	ktabproto

	if (DO_X)
	    xtabproto
	
		// Set up the calls used to handle dragging xtab to pool
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=xtab]" \
			"/kinetics/##[TYPE=kpool]" SLAVE kpool orange -1 0 \
			"echo.p dragging <S> to <D> for xtab" \
			"xtab_to_pool_add.p <S> <D>" \
			"xtab_to_pool_drop.p <S> <D>"
	
		// Set up the calls used to handle dragging xtab to xtab
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=xtab]" \
			"/kinetics/##[TYPE=xtab]" INPUT xtab orange -1 0 \
			"echo.p dragging <S> to <D> for xtab" \
			"xtab_to_xtab_add.p <S> <D>" \
			"xtab_to_xtab_drop.p <S> <D>"
	
		// Set up the calls used to handle dragging pool to xtab
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[TYPE=kpool]" \
			"/kinetics/##[TYPE=xtab]" INPUT xtab hotpink -1 0 \
			"echo.p dragging <S> to <D> for xtab" \
			"pool_to_xtab_add.p <S> <D>" \
			"pool_to_xtab_drop.p <S> <D>"
	
	
	    // make the editor for xtabs
	    make_xedit_xtab
	
	end
	// set up the dumping fields
	simobjdump xtab input output alloced step_mode stepsize notes \
    	editfunc xtree_fg_req xtree_textfg_req \
    	baselevel last_x last_y is_running x y z
end
