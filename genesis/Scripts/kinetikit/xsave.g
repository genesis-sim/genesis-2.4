//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-2003 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/
// xsave.g : implements the save/restore capabilities of kkit
int dump_x = 1

function save_header(filename, is_modular)
	str filename
	int is_modular

	do_inform {"Saving file" @ {filename}}
	openfile {filename} "w"
	writefile {filename} "//genesis"
	if (is_modular)
		writefile {filename} "// kkit Version 11 modular dumpfile"
	else
		writefile {filename} "// kkit Version 11 flat dumpfile"
	end
	writefile {filename} " "
	writefile {filename} {"// Saved on " @ {getdate}}
	writefile {filename} " "
	writefile {filename} "include kkit {argv 1}"
	writefile {filename} " "

	// Note that from kkit7 FASTDT is used for transient calculations
	writefile {filename} "FASTDT = "{FASTDT}
	writefile {filename} "SIMDT = "{SIMDT}
	writefile {filename} "CONTROLDT = "{CONTROLDT}
	writefile {filename} "PLOTDT = "{PLOTDT}
	writefile {filename} "MAXTIME = "{MAXTIME}

	// Below here, the parms are post-version 6.
	writefile {filename} "TRANSIENT_TIME = "{TRANSIENT_TIME}
	writefile {filename} "VARIABLE_DT_FLAG = "{VARIABLE_DT_FLAG}
	writefile {filename} "DEFAULT_VOL = "{getfield /editcontrol/vol value}
	writefile {filename} "VERSION = 11.0"
	writefile {filename} "setfield /file/modpath value "{getfield /file/modpath value}
	writefile {filename} "kparms"
	writefile {filename} " "

	closefile {filename}
end

function find_geom_matching_vol(scalevol)
	float scalevol

	float v
	str name
	foreach name ({el /kinetics/##[][TYPE=geometry]})
		v = {getfield {name} scalesize}
		if ({{abs {v - scalevol}} / scalevol} < 0.001) // 0.1% is pretty good
			return {name}
		end
	end

	create geometry /kinetics/geometry -autoindex
	name = {el ^}
	setfield {name} size {scalevol * 1.6666667e-21}
	setfield {name} scalesize {scalevol}
	setfield {name} dim 3
	setfield {name} shape "sphere"
	setfield {name} outside ""
	return {name}
end

// This function associates each pool with a geometry. It creates
// new geometries as needed.
// This function is safe to run even if all geometries already exist:
// it will not modify legal-looking geomname references.
function convert_vols_to_geometry
	str name
	str geom
	float vol

	foreach name ({el /kinetics/##[][TYPE=kpool]})
		if (!{exists {getfield {name} geomname}})
			vol = {getfield {name} vol}
			geom = {find_geom_matching_vol {vol}}
			setfield {name} geomname {geom}
		end
	end
end

function complete_loading
	if (DO_X == 1)
		setfield /file/modpath old_value {getfield /file/modpath value}
		if (VERSION >= 8.0)
			FIXCOORDS = 0
			setfield /edit/draw/tree treemode geometry
			setfield /inform/ok script "xhide /inform"
			do_model_size
	//		xhide /inform
		end
	end
	fix_transp_msgs
	fix_enz_parent_bug
	if (DO_X == 1)
		set_disp_path
	end

	// Scan volumes of all molecules and assign
	// into geometry.
	if (VERSION < 11.0)
		convert_vols_to_geometry
	end
	reset
end

function save_table_info(file, path)
	str file
	str path

	str name

	// echo saving table {file} {path}

	openfile {file} "a"

	foreach name ({el {path}})
		writefile {file} setfield {name} table->dx {getfield {name} table->dx}
		writefile {file} setfield {name} table->invdx {getfield {name} table->invdx}
	end

	closefile {file}
end

function save_sim(filename, group)
	str filename
	str group
	int group_only
	if ({exists {group}} == 1)
		group_only = 1
	else
		group_only = 0
	end

	// Here we check if geometries have been set up in Version 11 models
	convert_vols_to_geometry

	save_header {filename} 0

	// swap in the current simulation context for dumps
	swapdump
	str name
	if (dump_x)
		if (group_only)
			simdump {filename} -path {group} \
				-path {group}/##[] \
				-path /kinetics/#[TYPE=kpool] \
				-path /kinetics/#[]/#[TYPE=kenz] \
				-path /kinetics/#[TYPE=kreac] \
				-path /edit/draw \
				-path /edit/draw/tree \
				-path /file/notes
			foreach name ({el {group}/#[][TYPE=uplink]})
				dumpuplink {filename} {name}
			end
			foreach name ({el {group}/#[][TYPE=downlink]})
				dumpdownlink {filename} {name}
			end
			save_table_info {filename} {group}/#[][TYPE=xtab]

			foreach name ({el {group}/##[],kinetics/#[][TYPE=kpool],/kinetics/#[][TYPE=kreac],/kinetics/#[]/#[TYPE=kenz]})
				if ({exists {name}/notes})
					call {name}/notes SAVE {filename}
				end
			end
		else
			simdump {filename} \
				-path /kinetics/##[][TYPE=geometry] \
				-path /kinetics/##[][TYPE!=geometry] \
				-path /psearch/##[] \
				-path /graphs/#[TYPE=xgraph] \
				-path /graphs/#[]/#[TYPE=xplot] \
				-path /moregraphs/#[TYPE=xgraph] \
				-path /moregraphs/#[]/#[TYPE=xplot] \
				-path /edit/draw \
				-path /edit/draw/tree \
				-path /file/notes
			save_table_info {filename} /kinetics/##[][TYPE=xtab]

			foreach name ({el /kinetics/##[]})
				if ({exists {name}/notes})
					call {name}/notes SAVE {filename}
				end
			end
		end
	else
		if (group_only)
			simdump {filename} -path {group} \
				-path {group}/##[] \
				-path /kinetics/#[TYPE=kpool] \
				-path /kinetics/#[]/#[TYPE=kenz]
		else
			simdump {filename} -path /kinetics/##[] \
				-path /psearch/##[]
		end
	end
	// swap out the current simulation context for dumps
	swapdump

	openfile {filename} "a"
	writefile {filename} "complete_loading"
	closefile {filename}

	do_inform {"Finished saving file " @ {filename}}
	xflushevents
	xhide /inform
	xhide /file

end

function load_sim(filename)
	str filename
	str foo = filename

	do_inform {"Loading file " @ {foo}}
	/*
	echo {filename}
	source {filename}
	*/

	do_inform {"Finished loading file " @ {filename}}
	xflushevents
	xhide /inform
	xhide /file
end

function fix_pre_kkit8_coords
	int i
	str name
	float x, y, z
	i = 0
	foreach name ({el /kinetics/##[]})
		x = {getfield /edit/draw/tree ptlist[{i}].xpts}
		y = {getfield /edit/draw/tree ptlist[{i}].ypts}
		z = {getfield /edit/draw/tree ptlist[{i}].zpts}
		position {name} {x} {y} {z}
		i = i + 1
	end
	setfield /edit/draw/tree treemode geometry
	call /edit/draw/tree RESET
end

function convert_prekkit8
	fix_pre_kkit8_coords
	fix_pre_kkit8_notes
	fix_enz_parent_bug
	setfield /inform/ok script "xhide /inform"
	xhide /inform
end

function do_save
	if (FIXCOORDS == 1)
		fix_pre_kkit8_coords
		FIXCOORDS = 0
	end
    str dest = {getfield /file/save value}
    save_sim {dest} __nothere
end

function save_modular_modelfile
	if (FIXCOORDS == 1)
		fix_pre_kkit8_coords
		FIXCOORDS = 0
	end
    str dest = {getfield /file/save value}
    save_modsim {dest}
end

// Saves the module data to the specified file.
// Does not worry about decisions about whether or not to save.
function save_module_data(modfile, module)
	str modfile
	str module

	echo saving module {module} as {modfile}
	openfile {modfile} "w"
	writefile {modfile} "//genesis"
	writefile {modfile} "// Module dumpfile for kinetikit version 10"
	writefile {modfile} " "
	writefile {modfile} "// Saved on "{getdate}
	writefile {modfile} " "
	closefile {modfile}
	swapdump
	// echo {el {module},{module}/#[],{module}/#[][ISA!=uplink]/##[],{module}/#[][ISA=uplink]/proto,{module}/#[][ISA=uplink]/proto/##[]}
	simdump {modfile} -path {module},{module}/#[],{module}/#[][ISA!=uplink]/##[],{module}/#[][ISA=uplink]/proto,{module}/#[][ISA=uplink]/proto/##[]
	swapdump


	openfile {modfile} "a"
	writefile {modfile} " "
	writefile {modfile} "///////////////////////////////////////////"
	writefile {modfile} "//            Notes for objects          //"
	writefile {modfile} "///////////////////////////////////////////"
	closefile {modfile}

	str name
	foreach name ({el {module},{module}/#[],{module}/#[][ISA!=uplink]/##[],{module}/#[][ISA=uplink]/proto,{module}/#[][ISA=uplink]/proto/##[]})
		if ({exists {name}/notes})
			call {name}/notes SAVE {modfile}
		end
	end
end

// possible modes:
// 0: Save only if changed, automatically generate version #s.
// 1: Save to generated new version# regardless of whether changed
// 2: Save only if changed, use provided version # and filename
// 3: Save as specified filename even if file exists.
// In modes 0, 1, 2: it complains if file exists, and does not save,
// and returns 0. In all other cases returns 1.

function mode0_save_module(filename, module)
	str dirpath = {getfield /file/modpath value}
	str olddirpath = {getfield /file/modpath value}
	int do_save = 0

	// Has the directory changed ? Find lowest modversion +1 and save.
	if ({strcmp {getfield /file/modpath value} {getfield /file/modpath old_value}} != 0)
		do_save = 1
		echo directory changed
	end

	// Has the md5sum changed ? Find lowest modversion +1 and save
	str Gmd5sum = {calc_md5sum {module}}
	if ({strcmp {Gmd5sum} {getfield {module} md5sum}} != 0)
		do_save = 1
		echo md5sum changed
	end

	// Has original file vanished ? Find lowest modversion +1 and save
	update_modfilename {module}
	str modfile = {getfield {module} file}
	if ( {fileexists {modfile}} == 0 )
		do_save = 1
		echo orig file {modfile} vanished
	end

	if (do_save)
		str modname = {getfield {module} savename} @ "_"
		int modversion = {maxfileversion {dirpath} {modname}} + 1
		setfield {module} version {modversion}
		setfield {module} md5sum {Gmd5sum}
		update_modfilename {module}
		modfile = {getfield {module} file}
		if ( {fileexists {modfile}} )
			do_warning 0 "Attempting to overwrite file "{modfile}". File not saved."
			return 0
		end
		save_module_data {modfile} {module}
	else
		echo Module {module} unchanged, still using file {modfile}
	end
	// In both cases, need to write out include statement for modfile
	writefile {filename} include {modfile}
end

function mode1_save_module(filename, module)
	// Find lowest modversion +1 and save.
end

function mode2_save_module(filename, module)
	// Has directory changed ? Use provided version and filename if poss
	// Has md5sum changed ? Use provided
end

function mode3_save_module(filename, module)
	// Save in specified filename.
end

function check_and_save_module(filename, module, mode)
	str filename	// filename refers to the model file name.
	str module		// module is the path to the module
	int mode		// mode is defined above

	if (mode == 0)
		mode0_save_module {filename} {module}
	end
	if (mode == 1)
		mode1_save_module {filename} {module}
	end
	if (mode == 2)
		mode2_save_module {filename} {module}
	end
	if (mode == 3)
		mode3_save_module {filename} {module}
	end

	return 1
end

function save_all_modules(filename, start)
	str filename
	str start

	str mod
	foreach mod ({el {start}/#[][TYPE=group]})
		if ({getfield {mod} mod_save_flag} == 1)
			check_and_save_module {filename} {mod} 0
			save_all_modules {filename} {mod}
		end
	end
end

function save_module_links(filename)
	str filename

	str ul
	str dl
	int nmsgs
	int i
	str mtype
	foreach ul ({el /kinetics/##[][TYPE=uplink]})
		nmsgs = {getmsg {ul} -out -count}
		for (i = 0; i < nmsgs; i = i + 1)
			mtype = {getmsg {ul} -out -type {i}}
			if ({strcmp {mtype} "INPUT"} == 0)
				dl = {getmsg {ul} -out -dest {i}}
				writefile {filename} uplink_to_downlink_add {ul} {dl}
			end
		end
	end
end

function save_module_plots(filename)
	str filename

	str plot
	str src
	str dest
	foreach plot ({el /#raphs/#/#[][TYPE=xplot]})
		src = {getmsg {plot} -in -source 0}
		dest = {el {plot}/..}
		writefile {filename} xadd_plot {dest} {src}
	end
end

function save_module_position(filename)
	str filename
	str name

	foreach name ({el /kinetics/##[][TYPE=group]})
		writefile {filename} position {name} {getfield {name} x} {getfield {name} y} {getfield {name} z}
	end
end

// Designed to take a duplicated proto and dump all its parameters.
// Signature of proto: path to parent linkinfo, md5sum, date ?
// Currently there is a problem with dumping the notes.
function dump_linkparms(proto, filename)
	str proto
	str filename

	if ({getfield {proto} nelm} == {getfield {proto} npool})
		return // No need to save msgr stuff
	end
	str name
	int i, n
	str lp
//	echo {getfield {name} {getarg {lp} -arg {i}}} >> filename
	echo load_linkparms {el {proto}/..} \
		{calc_struct_md5sum {proto}} " " -n >> {filename}
		
	foreach name ({el {proto}/##[]})
		if ({exists {name} linkparms})
			lp = {getfield {name} linkparms}
			n = {getarg {arglist {lp}} -count}
			for (i = 0; i < n ; i = i + 1)
				echo {getfield {name} {getarg {arglist {lp}} -arg {i + 1}}} " " -n >> {filename}
			end
		else
			echo No linkparms for {getfield {name} object->name}: {name}
		end
	end
	echo >> {filename}
	/*
	echo >> {filename}
	echo "/////////////////////////////////////" >> {filename}
	*/
		
	foreach name ({el {proto},{proto}/##[]})
		if ({exists {name}/notes})
			call {name}/notes SAVE {filename}
		end
	end

end

// Saves all the link parameters in a model.
function save_link_parms(filename)
	str filename

	str proto

	// modstr = ""
	foreach proto ({el /kinetics/##[][ISA=linkinfo]/proto})
		dump_linkparms {proto} {filename}
	end
end

function load_linkparms
	int n = {argc}
	str proto = {argv 1} @ "/proto"
	
// Check consistency of proto structure here.
//	str mfs = {calc_struct_md5sum {proto}/../../proto}
//	if ({strcmp {argv 2} {mfs}} != 0)
// 		do_warning 0 "discrepancy in proto structure for "{proto}
// 		return
//	end

	str name
	str lp
	int i
	int j = 3
	int n
	foreach name ({el {proto}/##[]})
		if ({exists {name} linkparms})
			lp = {getfield {name} linkparms}
			n = {getarg {arglist {lp}} -count}
			for (i = 0; i < n ; i = i + 1)
				setfield {name} {getarg {arglist {lp}} -arg {i + 1}} {argv {j}}
				j = j + 1
			end
		end
	end
end

function save_modsim(filename)
	str filename
	do_inform {"Saving modular file " @ {filename}}

	save_header {filename} 1

	openfile {filename} "a"
	writefile {filename} "///////////////////////////////////////////"
	writefile {filename} "//           Module specification        //"
	writefile {filename} "///////////////////////////////////////////"
	save_all_modules {filename} /kinetics
	setfield /file/modpath old_value {getfield /file/modpath value}

	writefile {filename} " "
	writefile {filename} "///////////////////////////////////////////"
	writefile {filename} "//            Link specification         //"
	writefile {filename} "///////////////////////////////////////////"
	save_module_links {filename}

	writefile {filename} " "
	writefile {filename} "///////////////////////////////////////////"
	writefile {filename} "//      Link parameter specification     //"
	writefile {filename} "///////////////////////////////////////////"
	closefile {filename}
	save_link_parms {filename}

	openfile {filename} "a"
	writefile {filename} " "
	writefile {filename} "///////////////////////////////////////////"
	writefile {filename} "//            Plot specification         //"
	writefile {filename} "///////////////////////////////////////////"
	save_module_plots {filename}

	writefile {filename} " "
	writefile {filename} "///////////////////////////////////////////"
	writefile {filename} "//       Position module display         //"
	writefile {filename} "///////////////////////////////////////////"
	save_module_position {filename}

	writefile {filename} " "
	writefile {filename} "///////////////////////////////////////////"
	writefile {filename} "//            Wrapping up save           //"
	writefile {filename} "///////////////////////////////////////////"
	writefile {filename} "complete_loading"

	closefile {filename}
	xflushevents
	xhide /inform
	xhide /file
end
