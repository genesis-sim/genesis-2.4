//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-1997 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

/******************************************************************
	Stuff for drag-drop operations involving graphs/plots
******************************************************************/

function xadd_plot(graph,src)
	str graph,src

	str col
	int srcindex = {getfield {src} index}
	str srcname
	str fieldname

	/* Check that the src is a simulated element */
	if ({strncmp {src} "/kinetics/" 10} != 0)
		return
	end

	if ({exists {src} xtree_fg_req})
		col = {getfield {src} xtree_fg_req}
	end

	fieldname = {getfield {src} plotfield}

	if (srcindex == 0)
		srcname = {getfield {src} name} @ "." @ {fieldname}
	else
		srcname = {getfield {src} name} @ "_" @ \
			{getfield {src} index} @ "." \
			@ {fieldname}
	end

	if ({exists {{graph} @ "/" @ {srcname}}})
		do_warning 0 \
			{"Warning: plot for '" @ {srcname} @ "' already exists"}
		return
	end

	create xplot {graph}/{srcname}
	addmsg {src} {graph}/{srcname} PLOT {fieldname} *{srcname} *{col}
	if ({strcmp {fieldname} "pA"} == 0)
		if ({getfield /plotcontrol/plotfield state} == 0)
			float vol = {getfield {src} vol}
			setfield {graph}/{srcname} wy {1 / vol}
		end
	end

	setfield {graph}/{srcname} \
		pixflags f \
		script "delete_plot.w <s> <d>; edit_plot.D <w>" \
		fg {col} \
		memory_mode expand
	useclock {graph}/{srcname} {PLOTCLOCK}
end

function delete_plot(plot,dest)
	str plot,dest

	if ({strcmp {dest} "/control/lib/del"} == 0)
		delete {plot}
		xupdate {getpath {plot} -head}
	end
end

function move_plot(plot,dest)
	str plot,dest
end

/******************************************************************
	function for editing plots
******************************************************************/

function do_update_plotinfo
	str plot = {getfield /parmedit/plot elmpath}
	int pixflags = {getfield {plot} pixflags}

	setfield /parmedit/plot/path value {getpath {plot} -head}
    setfield /parmedit/plot/name value {getpath {plot} -tail}
    setfield /parmedit/plot/Color value {getfield {plot} fg}
    setfield /parmedit/plot/ysquish value {getfield {plot} ysquish}
    setfield /parmedit/plot/hideme state {pixflags & 1}
    setfield /parmedit/plot/slope state {getfield {plot} do_slope}

    if ({exists {plot}/.. ymin})
    	setfield /parmedit/plot/yoffset value \
			{{getfield {plot}/.. ymin} - {getfield {plot} ymin}}
    else
    	setfield /parmedit/plot/yoffset value \
			{{getfield {plot}/../.. ymin} - {getfield {plot} ymin}}
    end
    setfield /parmedit/plot/yscale value 1
end

function do_save_plot(file)
	str file
	str plot = {getfield /parmedit/plot elmpath}
	int x

	x = {getfield {plot} npts} // Sachin's bug fixed.
    tab2file {file} {plot} xpts -table2 ypts \
       	-nentries {x - 1}

	xhide /parmedit/plot
end

function do_save_named_plot(plot, file)
	str plot
	str file
	int x

	if ({exists {plot} output}) // we are actually using tables
		x = {getfield {plot} output} // Sachin's bug fix
       	tab2file {file} {plot} table -nentries {x - 1}
	else
		x = {getfield {plot} npts} // Sachin's bug fix
       	tab2file {file} {plot} xpts -table2 ypts -nentries {x - 1}
		xhide /parmedit/plot
	end
end

function do_save_all_plots(filename)
    str filename
    str plot
	int x
 
    foreach  plot ({el /#graphs/#[]/#.Co,/#graphs/#[]/#.CoComplex})
        echo >> {filename}
        echo /newplot >> {filename}
        echo /plotname {plot} >> {filename}
		if ({exists {plot} output}) // we are actually using tables
			x = {getfield {plot} output} // This solves a really
			// obscure bug, owing its discovery to Sachin. If 
			// we don't do the assignment to an int, we get the
			// result in sci notation every time npts is large.
			// The atoi in tab2file cannot handle that, and we
			// get garbage in the output.
        	tab2file {filename} {plot} table -nentries {x - 1}
		else
			x = {getfield {plot} npts} // Sachin's bug fixed.
        	tab2file {filename} {plot} xpts -table2 ypts \
            	-nentries {x - 1}
		end
    end
end

function set_plot_color(v)
	int v
	str plot = {getfield /parmedit/plot elmpath}

	setfield {plot} fg {v}
	setfield /parmedit/plot/Color value {v}
end

function set_plot_colorname(v)
	str v
	str plot = {getfield /parmedit/plot elmpath}
	setfield {plot} fg {v}
end

function make_xedit_plot
	create xform /parmedit/plot [{EX},{EY},{EW},245]
	addfield /parmedit/plot elmpath \
		-description "path of plot being edited"
	ce /parmedit/plot
    create xdialog path -wgeom 60% -title "Parent"
    create xdialog name -xgeom 60% -ygeom 0 -wgeom 40% -title "Name"
	create xlabel dopts -label "Display options" -bg cyan
	/*
    create xdialog fg [0%,0:name,50%,30] -label "color" \
        -script "set_field <w> <v>; update_graph"
*/

	make_colorbar /parmedit/plot dopts
	setfield Color script "set_plot_colorname <v>"
	setfield cdraw/view script "set_plot_color <x>"

	create xtoggle slope [0,0:Color,50%,25] \
		-offlabel "Slope calculation mode: OFF" \
		-onlabel "Slope calculation mode: ON" \
		-script "do_slope <v>"
	create xtoggle hideme [50%,0:Color,50%,25] \
		-offlabel "Plot visibility: ON" \
		-onlabel "Plot visibility: OFF" \
		-script "do_hideme <v>"
	create xdialog yoffset [0,0:hideme,50%,30] -label "Y Offset" \
        -script "setplotscale <v> 0"
	create xdialog yscale [50%,0:hideme,50%,30] -label "Y Scale" \
        -script "setplotscale <v> 1"
    create xdialog ysquish \
		-label "Compression cutoff" \
        -script "set_field <w> <v>"
	create xdialog save -label "Save to file:" \
		-script "do_save_plot <v>"
    create xbutton UPDATE [0%,0:save,50%,30] \
        -script "do_update_plotinfo"
    create xbutton HIDE [50%,0:save,50%,30] \
        -script "xhide /parmedit/plot"
end

function update_graph
	str plot = {getfield /parmedit/plot elmpath}

	xupdate {plot}/..
end

function edit_plot(plot)
	str plot
	echo editing {plot}
	setfield /parmedit/plot elmpath {plot}
	do_update_plotinfo
	xshowontop /parmedit/plot
end

function do_hideme(state)
	int state
	str plot = {getfield /parmedit/plot elmpath}
	if (state)
		setfield {plot} pixflags v
	else
		setfield {plot} pixflags ~v
	end
end

function do_slope(state)
	int state
	str plot = {getfield /parmedit/plot elmpath}
	setfield {plot} do_slope {state}
end

function setplotscale
	str plot = {getfield /parmedit/plot elmpath}

	float offset = {getfield /parmedit/plot/yoffset value}
	float scale = {getfield /parmedit/plot/yscale value}
	float ymin = {getfield {plot}/.. ymin}
	float ymax = {getfield {plot}/.. ymax}
	float yrange = ymax - ymin
	if ((scale < 1e-9) && (scale > -1e-9))
		do_warning 0 "Scale must be more than 1e-9 absolute"
		return
	end

	setfield {plot} \
		ymin {ymin - offset} \
		ymax {ymin - offset + yrange / scale}

	update_graph
end

/* This function converts an existing plot to a phaseplot. It
** asks for the x-axis plot to be dragged onto the current plot */
function do_phaseplot
	do_inform "Drag the x-axis plot onto the current plot"
end


/******************************************************************
	function for actually making the graphs
******************************************************************/

function make_xgraphs

	create xform /graphs [{EX},0,{EW},{GH}]
	ce /graphs

	create xgraph conc1 -hgeom 100% -wgeom 50% \
		-ymax 1 -title "Concs 1"\
		-xlabel Time -XUnits Sec -YUnits uM \
		-script "xadd_plot.p <d> <S>"
	create xgraph conc2 [50%,0,50%,100%] -ymax 1 \
		-title "Concs 2" -xlabel Time -XUnits Sec -YUnits uM \
		-script "xadd_plot.p <d> <S>"

	create xform /moregraphs [{EX},{EY},{EW},{GH + 30}]
	ce /moregraphs
	create xgraph conc3 -hgeom 85% -wgeom 50% \
		-ymax 1 -title "Concs 3"\
		-xlabel Time -XUnits Sec -YUnits uM \
		-script "xadd_plot.p <d> <S>"
	create xgraph conc4 [50%,0,50%,85%] -ymax 1 \
		-title "Concs 4" -xlabel Time -XUnits Sec -YUnits uM \
		-script "xadd_plot.p <d> <S>"
	create xbutton HIDE -ygeom 0:last -script "xhide /moregraphs"
	ce /

	make_xedit_plot
	make_xedit_axis
	setfield /graphs/#[]/#[TYPE=xaxis] script "edit_axis.D <w>"
	setfield /moregraphs/#[]/#[TYPE=xaxis] script "edit_axis.D <w>"
	useclock /graphs/#[]/#[] {PLOTCLOCK}

	simobjdump xgraph xmin xmax ymin ymax overlay
	simobjdump xplot pixflags script fg ysquish do_slope wy -noDUMP
end

