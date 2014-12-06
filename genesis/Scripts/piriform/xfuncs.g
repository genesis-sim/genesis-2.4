// genesis

int y = -1
float y_f = 0.0


function mode

	if (({getfield /menu/draw/mode state}) == 1)
		if (({exists /playback}) == 0)
			create neutral /playback
		end
		deactivate
	else
		delete /playback
		y_f = 0.0
		y = -1
		create neutral /playback
		activate
	end
end

create neutral /playback

str directory = ""


function cont_sim
	setclock 0 {getfield /x_interface/simulate/step_size value}
	setclock 1 {getfield /x_interface/simulate/out_step value}
	adjustspike
reset
	echo START TIME:
	echo | date
	step {getfield /x_interface/simulate/sim_dur value} -time
	echo END TIME:
	echo | date
end


function start_sim
str str_el, file, parm_path
	if (({getfield /menu/draw/mode state}) != 1)
		test_dir  \
		    {getfield /x_interface/simulate/directory value}
		openfile dir_exist r
		if ({readfile dir_exist} == 0.0)
			directory = ({getfield /x_interface/simulate/directory value})
			mkdir {directory}
			pushe /output
			setfield soma_Vm filename {directory}/soma_Vm
			setfield Ia_Vm filename {directory}/Ia_Vm
			setfield supIb_Vm filename {directory}/supIb_Vm
			setfield deepIb_Vm  \
			    filename {directory}/deepIb_Vm
			setfield III_Vm filename {directory}/III_Vm
			setfield fb_Vm filename {directory}/fb_Vm
			setfield ff_Vm filename {directory}/ff_Vm
			setfield pyr_gAff filename {directory}/pyr_gAff
			setfield pyr_gRA filename {directory}/pyr_gRA
			setfield pyr_gCA filename {directory}/pyr_gCA
			setfield pyr_gLocal  \
			    filename {directory}/pyr_gLocal
			setfield pyr_gFB filename {directory}/pyr_gFB
			setfield pyr_gFF filename {directory}/pyr_gFF
			setfield ff_g filename {directory}/ff_g
			setfield fb_g filename {directory}/fb_g
			setfield soma_Im filename {directory}/soma_Im
			setfield Ia_Im filename {directory}/Ia_Im
			setfield supIb_Im filename {directory}/supIb_Im
			setfield deepIb_Im  \
			    filename {directory}/deepIb_Im
			setfield III_Im filename {directory}/III_Im
			setfield efield_Ia_Na  \
			    filename {directory}/efield_Ia_Na
			setfield efield_Ia_K  \
			    filename {directory}/efield_Ia_K
			setfield efield_supIb  \
			    filename {directory}/efield_supIb
			setfield efield_deepIb  \
			    filename {directory}/efield_deepIb
			setfield efield_soma  \
			    filename {directory}/efield_soma
			setfield efield_III  \
			    filename {directory}/efield_III
			setfield efield_total  \
			    filename {directory}/efield_total
			pope
			parm_path = (directory) @ "/parms"
			openfile {parm_path} w
			pushe /x_interface
			writefile {parm_path}  \
			    {getfield Aff_syn/wt_bulb_Ia value}
			writefile {parm_path}  \
			    {getfield Aff_syn/Na value}
			writefile {parm_path}  \
			    {getfield ff_syn/wt_ff_Ia value}
			writefile {parm_path} {getfield ff_syn/K value}
			writefile {parm_path}  \
			    {getfield CA_syn/wt_CA_pyr value}
			writefile {parm_path} {getfield CA_syn/Na value}
			writefile {parm_path}  \
			    {getfield RA_syn/wt_pyr_RA value}
			writefile {parm_path} {getfield RA_syn/Na value}
			writefile {parm_path}  \
			    {getfield fb_syn/wt_fb_pyr value}
			writefile {parm_path} {getfield fb_syn/Cl value}
			writefile {parm_path}  \
			    {getfield LOCAL_syn/wt_pyr_local value}
			writefile {parm_path}  \
			    {getfield LOCAL_syn/Na value}
			writefile {parm_path}  \
			    {getfield Aff_ff_syn/wt_Aff_ff value}
			writefile {parm_path}  \
			    {getfield fb_ff_syn/wt_pyr_ff value}
			writefile {parm_path}  \
			    {getfield pyr_fb_syn/wt_pyr_fb value}
			writefile {parm_path}  \
			    {getfield Aff_fb_syn/wt_Aff_fb value}
			closefile {parm_path}
			cont_sim
			closefile dir_exist
		else
			echo "Directory" {directory} "already exists."
			xshow /x_interface/dir_error
		end
	else
		xshow /x_interface/not_sim
		echo "NOT IN SIMULATE MODE"
	end
end

int flag = 0
function update
	delete /x_interface/sim_stat/sim_time
	delete /x_interface/sim_stat/percent
	create xdialog /x_interface/sim_stat/sim_time [1%,21,98%,30]  \
	    -title "simulation time (msec)" -value  \
	    {{getstat -time} + .1}
	create xdialog /x_interface/sim_stat/percent [1%,51,98%,30]  \
	    -title "percent completed" -value  \
	    {100*({getstat -time} + .1)/{getfield /x_interface/simulate/sim_dur value}}
end

function sim_stat

	if (flag)
		update
	else
		create xform /x_interface/sim_stat [18,230,300,156]
		create xlabel /x_interface/sim_stat/label -title \
		    "sim_stat" -bg pink 
		create xdialog /x_interface/sim_stat/sim_time  \
		    [1%,21,98%,30] -title "simulation time (msec)"  \
		    -value {{getstat -time} + .1}
		create xdialog /x_interface/sim_stat/percent  \
		    [1%,51,98%,30] -title "percent completed" -value  \
		    {100*({getstat -time} + .1)/{getfield /x_interface/simulate/sim_dur value}}
		create xbutton /x_interface/sim_stat/update  \
		    [1%,83,98%,25] -title "update" -script "update"
		create xbutton /x_interface/sim_stat/cancel  \
		    [1%,111,98%,25] -title "cancel" -script  \
		    "xhide /x_interface/sim_stat"
	end
	xshow /x_interface/sim_stat
	flag = 1
end


function set_parameters
str str_el

	foreach str_el ({el /pyr/pyramidal[]/soma/Cl_channel})
		setfield {str_el}  \
		    tau2 {getfield /x_interface/fb_syn/Cl value}
	end
	foreach str_el ({el /pyr/pyramidal[]/soma/Ia_dend/Aff_Na_channel})
		setfield {str_el}  \
		    tau2 {getfield /x_interface/Aff_syn/Na value}
	end
	foreach str_el ({el /pyr/pyramidal[]/soma/supIb_dend/CA_Na_channel})
		setfield {str_el}  \
		    tau2 {getfield /x_interface/CA_syn/Na value}
	end
	foreach str_el ({el /pyr/pyramidal[]/soma/deepIb_dend/RA_Na_channel} \
	    )
		setfield {str_el}  \
		    tau2 {getfield /x_interface/RA_syn/Na value}
	end
	foreach str_el ({el /pyr/pyramidal[]/soma/Ia_dend/K_channel})
		setfield {str_el}  \
		    tau2 {getfield /x_interface/ff_syn/K value}
	end
	foreach str_el ({el /pyr/pyramidal[]/soma/III_dend/Local_Na_channel} \
	    )
		setfield {str_el}  \
		    tau2 {getfield /x_interface/LOCAL_syn/Na value}
	end
	echo Channel Open Times Adjusted

	adjust_wts {getfield /x_interface/RA_syn/wt_pyr_RA value}  \
	    {getfield /x_interface/CA_syn/wt_CA_pyr value}  \
	    {getfield /x_interface/fb_syn/wt_fb_pyr value} 0.0  \
	    {getfield /x_interface/ff_syn/wt_ff_Ia value}  \
	    {getfield /x_interface/Aff_syn/wt_bulb_Ia value}  \
	    {getfield /x_interface/Aff_ff_syn/wt_Aff_ff value}  \
	    {getfield /x_interface/Aff_fb_syn/wt_Aff_fb value}  \
	    {getfield /x_interface/pyr_fb_syn/wt_pyr_fb value}  \
	    {getfield /x_interface/fb_ff_syn/wt_pyr_ff value}  \
	    {getfield /x_interface/LOCAL_syn/wt_pyr_local value}

end

function run_playback
str file

	if (({getfield /menu/draw/mode state}) == 1)
		file = ({getfield /x_interface/playback/dir value}) @ "/parms"
		openfile {file} r
		setfield /x_interface/Aff_syn/wt_bulb_Ia  \
		    value {readfile {file}}
		setfield /x_interface/Aff_syn/Na value {readfile {file}}
		setfield /x_interface/ff_syn/wt_ff_Ia  \
		    value {readfile {file}}
		setfield /x_interface/ff_syn/K value {readfile {file}}
		setfield /x_interface/CA_syn/wt_CA_pyr  \
		    value {readfile {file}}
		setfield /x_interface/CA_syn/Na value {readfile {file}}
		setfield /x_interface/RA_syn/wt_pyr_RA  \
		    value {readfile {file}}
		setfield /x_interface/RA_syn/Na value {readfile {file}}
		setfield /x_interface/fb_syn/wt_fb_pyr  \
		    value {readfile {file}}
		setfield /x_interface/fb_syn/Cl value {readfile {file}}
		setfield /x_interface/LOCAL_syn/wt_pyr_local  \
		    value {readfile {file}}
		setfield /x_interface/LOCAL_syn/Na  \
		    value {readfile {file}}
		setfield /x_interface/Aff_ff_syn/wt_Aff_ff  \
		    value {readfile {file}}
		setfield /x_interface/fb_ff_syn/wt_pyr_ff  \
		    value {readfile {file}}
		setfield /x_interface/pyr_fb_syn/wt_pyr_fb  \
		    value {readfile {file}}
		setfield /x_interface/Aff_fb_syn/wt_Aff_fb  \
		    value {readfile {file}}
		closefile {file}
		setclock 0  \
		    {getfield /x_interface/playback/step_size value}
		step {getfield /x_interface/playback/step_num value}  \
		    -time
	else
		xshow /x_interface/not_play
		echo "NOT IN PLAYBACK MODE"
	end
end

create compartment /library/comp
setfield /library/comp Rm 1.0


function not_exists(path)

	if (!({exists {path}}))
		return 1
	else
		echo {path} already exists
		return 0
	end
end


function map(parameter, pyr_ny)

	createmap /library/comp map_{parameter} {PYR_NX} {pyr_ny} -delta \
	     {PYR_DX} {PYR_DY}
end

function position
	if (y == 3)
		y = 0
	else
		y = y + 1
	end
end

function dk_in(filename)
	str filename
	str directory

	directory = ({getfield /x_interface/playback/dir value})
	create disk_in dk_{filename}.{directory}
       	enable ^
        setfield ^ filename {directory}/{filename} fileformat 1  \
            leave_open 1
	reset
end


function create_xview(abs_path,parameter, segment, offset, increment)
str abs_path
float offset, increment
     	create xview net_{parameter}/draw/{segment} -path  \
     	    {abs_path}/map_{segment}/comp[] -field Em \
	    -color_val 1 -morph_val 0 -sizescale 30 \
	    -tx -4.2  -ty  {offset + increment} -autoscale 1
end

// -value_min[0] 1e-10 -value_max[0] 5.0e-07 \


function create_xform(parameter, y_scale)
int pos_x, pos_y
float scale, scaled
str directory

	pos_x = 885
	pos_y = y*195
	create xform net_{parameter} [{pos_x},{pos_y},280,170]
	create xlabel net_{parameter}/label -title "net_"{parameter} -bg pink
	create xdraw net_{parameter}/draw [2%,15%,96%,96%] \
	       -wx 11 -wy {y_scale} -bg ghostwhite
	scaled = ((-2.6*y_scale)/10)
	create xshape net_{parameter}/draw/rostral -tx -3.7 -ty  \
	    {scaled} -text "rostral"
	create xshape net_{parameter}/draw/caudal -tx 3.7 -ty {scaled}  \
	    -text "caudal"
	create xbutton net_{parameter}/cancel [74%,12%,25%,15%] -title  \
	    "cancel" -script "delete "/playback/net_{parameter}

end


function net_view(flag, parameter)
	str str_el, parameter, el_name
	int flag, k

	el_name = (parameter) @ "." @ ({getfield /x_interface/playback/dir value})
	if (!flag)
		delete /playback/{el_name}
	else
		if (({not_exists /playback/net_{el_name}}))
			create neutral /playback/net_{el_name}
			ce ^
			dk_in {parameter}
			k = 0
			map {el_name} {PYR_NY}
			foreach str_el ({el map_{el_name}/comp[]})
				addmsg dk_{el_name} {str_el} EREST  \
				    val[0][{k}]
				k = k + 1
			end
			position
			create_xform {el_name} 10
			create_xview /playback/net_{el_name} {el_name} {el_name} -1.75 0
	     		xshow net_{el_name}
			reset
		end
	end
end


function side_play(str_el, i)
int row, k
str comp_el

	directory = ({getfield /x_interface/playback/dir value})
	row = ({getfield /x_interface/full_cell_view/row value})
	k = (row*15)
	dk_in {str_el}
	map {str_el} 1
	foreach comp_el ({el map_{str_el}/comp[]})
		addmsg dk_{str_el}.{directory} {comp_el} EREST val[0][{k}]
		k = k + 1
	end
end


function side_view(flag)
	str directory, str_el, parameter, comp_el
	int flag, k, i, row

	parameter = ({getfield /x_interface/full_cell_view/parameter value})
	directory = ({getfield /x_interface/playback/dir value})
	row = ({getfield /x_interface/full_cell_view/row value})
	if ((row > 0) && (row < 10))
		if (!flag)
			delete  \
			    /playback/full_cell_{parameter}_{row}.{directory}
		else
			if ( \
			    ({not_exists /playback/full_cell_{parameter}_{row}.{directory}}) \
			    )
				create neutral  \
				    /playback/net_full_cell_{parameter}_{row}.{directory}
				ce ^
				position
				create_xform  \
				    full_cell_{parameter}_{row}.{directory} \
				     6.1
				i = 0
				if ((parameter) == "Vm")
					foreach str_el (III_Vm soma_Vm  \
					    deepIb_Vm supIb_Vm Ia_Vm)
						i = i + 1
						side_play {str_el} {i}
						create_xview /playback/net_full_cell_{parameter}_{row}.{directory}   \
						    full_cell_{parameter}_{row}.{directory} \
						     {str_el} -1.6 {i*.69}

					end
				end
				if ((parameter) == "Im")
					foreach str_el (III_Im soma_Im  \
					    deepIb_Vm supIb_Vm Ia_Vm)
						i = i + 1
						side_play {str_el} {i}
						create_xview /playback/net_full_cell_{parameter}_{row}.{directory}   \
						    full_cell_{parameter}_{row}.{directory} \
						     {str_el} -1.6 {i*.69}
					end
				end
				if ((parameter) == "Gk")
					foreach str_el (pyr_gLocal pyr_gFB  \
					    pyr_gCA pyr_gRA pyr_gFF  \
					    pyr_gAff)
						i = i + 1
						side_play {str_el} {i}
						create_xview /playback/net_full_cell_{parameter}_{row}.{directory}   \
						    full_cell_{parameter}_{row}.{directory} \
						     {str_el} -1.6 {i*.69}
					end
				end
			end
		end
	else
		echo "INVALID ROW NUMBER, VALID ROW NUMBERS: 1-9"
	end
	xshow net_full_cell_{parameter}_{row}.{directory}
end


function layer(path, plot_name, filename)
str path, plot_name, filename, directory

str str_el
int k

	directory = ({getfield /x_interface/playback/dir value})
	dk_in {filename}
	k = 0
	foreach str_el ({el {path}})
		addmsg dk_{filename}.{directory} pseudo_{plot_name}  \
		    CURRENT val[0][{k}] 0.0
		k = k + 1
	end
end


function scale_graph(path)
str path

	setfield {path} xmin {getfield {path}/scale/xmin value}  \
	    xmax {getfield {path}/scale/xmax value}  \
	    ymin {getfield {path}/scale/ymin value}  \
	    ymax {getfield {path}/scale/ymax value}
end



function make_xplot(plot_name, xmin, xmax, ymin, ymax, y_unit)
str plot_name, y_unit
float xmin, xmax, ymin, ymax

float y_pos

	y_pos = y_f*195
	y_f = y_f + 1.0
	if (y_f == 4)
		y_f = 0
	end

	create xform form [518,{y_pos},367,170]
	create xgraph form/graph [0,0%,100%,90%] -bg ghostwhite -title {plot_name}
		setfield ^ xgeom 0 ygeom 0 wgeom 100% hgeom 100% xmin 0  \
		    ymin {ymin}  \
		    xmax {getfield /x_interface/playback/step_num value} \
		    ymax {ymax} XUnits msec YUnits {y_unit} 


// If the next two lines puzzle you, don't worry.  This is a hack done to
// insure that the xgraph's size corresponds to the xform.  As XODUS is
// improved these hacks will no longer be necessary.
/*
	setfield form xgeom 375 ygeom 170
	setfield form/graph xmax {xmax}
*/
	pushe form/graph

		create xform scale [518,{y_pos},220,204]
		create xlabel scale/label -title "scale" -bg pink
		create xdialog scale/xmin -title "xmin" -value {xmin}
		create xdialog scale/xmax -title "xmax" -value {xmax}
		create xdialog scale/ymin -title "ymin" -value {ymin}
		create xdialog scale/ymax -title "ymax" -value {ymax}
		create xbutton scale/apply -title "apply" -script  \
		    "scale_graph "/playback/{plot_name}/form/graph" "  \
		    <widget>
		create xbutton scale/cancel -title "cancel" -script  \
		    "xhide "/playback/{plot_name}/form/graph/scale" "  \
		    <widget>

	pope

	create xbutton form/scale_on [83%,0%,17%,15%] -title "scale"  \
	    -script "xshow "/playback/{plot_name}/form/graph/scale" "  \
	    <widget>
	create xbutton form/cancel [83%,15%,17%,15%] -title "cancel"  \
	    -script "delete "/playback/{plot_name}" " <widget>

	xshow form
	

	addmsg pseudo_{plot_name} form/graph PLOT field *curvename \
	    *black

	reset	

end


function pseudo_field(plot_name, avg)
str plot_name
float avg

	create efield pseudo_{plot_name}
		setfield ^ x 1 y 0 z 0 scale {avg}

end


function field_view(flag)
	str str_el, plot_name, short_x, directory
	int flag
	float avg, y_pos, k


	directory = ({getfield /x_interface/playback/dir value})
	plot_name = "field"
	short_x = "/x_interface/field_pot"
	foreach str_el ({el {short_x}/#[TYPE=xtoggle]})
		if (({getfield {str_el} state}) == 1)
			plot_name = (plot_name) @ "_" @ ({getfield {str_el} onlabel})
		end
	end
	plot_name = (plot_name) @ "_" @ (directory)

	if (flag == 2)
		plot_name = "field_eeg" @ "_" @ (directory)
	end
	if (!flag)
		delete /playback/{plot_name}
	else
		if (({not_exists /playback/{plot_name}}))
			create neutral /playback/{plot_name}
			ce /playback/{plot_name}
			k = 0.0
			foreach str_el ({el /field/efield_soma[]})
				k = k + 1.0
			end
			avg = 1.0/(k)
			pseudo_field {plot_name} {avg}

			if (flag == 2)
				layer /field/efield_total[] {plot_name}  \
				    efield_total
			else
				if ( \
				    ({getfield {short_x}/Ia_Na state}) == 1 \
				    )
					layer /field/efield_Ia_Na[]  \
					    {plot_name} efield_Ia_Na
				end
				if ( \
				    ({getfield {short_x}/Ia_K state}) == 1 \
				    )
					layer /field/efield_Ia_K[]  \
					    {plot_name} efield_Ia_K
				end
				if ( \
				    ({getfield {short_x}/supIb state}) == 1 \
				    )
					layer /field/efield_supIb[]  \
					    {plot_name} efield_supIb
				end
				if ( \
				    ({getfield {short_x}/deepIb state}) == 1 \
				    )
					layer /field/efield_deepIb[]  \
					    {plot_name} efield_deepIb
				end
				if ( \
				    ({getfield {short_x}/soma state}) == 1 \
				    )
					layer /field/efield_soma[]  \
					    {plot_name} efield_soma
				end
				if ( \
				    ({getfield {short_x}/III state}) == 1 \
				    )
					layer /field/efield_III[]  \
					    {plot_name} efield_III
				end
			end

			make_xplot {plot_name} 0 500 -500 100 mV
		end
	end
end


function conduct_view(flag, short_chan)
int flag
str short_chan

str str_el, chan, short_x, plot_name, directory
float avg, k

	directory = ({getfield /x_interface/playback/dir value})
	plot_name = "conductance_" @ (short_chan) @ "." @ (directory)
	short_x = "/x_interface/conduct"

	if (!flag)
		delete /playback/{plot_name}
	else
		if (({not_exists /playback/{plot_name}}))
			create neutral /playback/{plot_name}
			ce ^

			if (short_chan == "pyr_gAff")
				chan = "Ia_dend/Aff_Na_channel"
			end
			if (short_chan == "pyr_gFF")
				chan = "Ia_dend/K_channel"
			end
			if (short_chan == "pyr_gCA")
				chan = "supIb_dend/CA_Na_channel"
			end
			if (short_chan == "pyr_gRA")
				chan = "deepIb_dend/RA_Na_channel"
			end
			if (short_chan == "pyr_gFB")
				chan = "Cl_channel"
			end
			if (short_chan == "pyr_gLocal")
				chan = "III_dend/Local_Na_channel"
			end

			k = 0.0
			foreach str_el ({el /pyr/pyramidal[]/soma/{chan}})
				k = k + 1.0
			end
			avg = 1.0/(k)

			pseudo_field {plot_name} {avg}

 			layer /pyr/pyramidal[]/soma/{chan} {plot_name}  \
 			    {short_chan}
			if ( \
			    (short_chan == "pyr_gAff") || (short_chan == "pyr_gFB") \
			    )
			    make_xplot {plot_name} 0 500 -1.0e-03  \
			        1.0e-03 mS
			else
			    make_xplot {plot_name} 0 500 -1.0e-04  \
			        1.0e-04 mS
			end
		end
	end
end


function spike_view(flag, cell)
int flag
str cell

str str_el, cell, plot_name, short_x, file, directory
int k
float avg

	directory = ({getfield /x_interface/playback/dir value})
	plot_name = "spike_activity_" @ (cell) @ "_" @ (directory)
	short_x = "/x_interface/playback/spike"

	if (!flag)
		delete /playback/{plot_name}
	else
		if (({not_exists /playback/{plot_name}}))
			create neutral /playback/{plot_name}
			ce ^
			if (cell == "pyr")
				file = "soma_Vm"
			end
			if (cell == "fb")
				file = "fb_Vm"
			end
			if (cell == "ff")
				file = "ff_Vm"
			end

			k = 0
			foreach str_el ({el /pyr/pyramidal[]})
				copy /library/spike pseudo_spike[{k}]
				setfield pseudo_spike[{k}] output_amp 1
				k = k + 1
			end
			avg = 1.0/(k)

			pseudo_field {plot_name} 1.0

			dk_in {file}
			k = 0
			foreach str_el ({el /pyr/pyramidal[]})
				addmsg dk_{file}.{directory}  \
				    pseudo_spike[{k}] INPUT val[0][{k}]
				addmsg pseudo_spike[{k}]  \
				    pseudo_{plot_name} CURRENT state 0.0
				k = k + 1
			end
			make_xplot {plot_name} 0 500 0.0 135 spike/cell
		end
	end
end


function time
        setfield /x_interface/playback/clock  \
            value {{getstat -time} + .1}
end

function get_current_time
        create script_out /x_interface/playback/clock/script_clock
        setfield ^ command "time"
end

