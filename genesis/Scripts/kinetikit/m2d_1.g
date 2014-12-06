//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-2001 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

int pathway_id

// This function dumps the notes
function db_dumpnotes(filename, elm)
	call {elm}/notes SAVE textsave
	sed -f /home/bhalla/scripts/kkit8/sfile textsave >> {filename}
//	sed -f /home/bhalla/scripts/kkit8/sfile textsave
	echo > textsave
	echo ");" >> {filename}
end

// This function dumps all the info for channels
function db_dumpchans(filename, group)
	str filename
	str group

	float perm
	int i, j

	if ({getarg {el {group}/#[]/#[][TYPE=kchan]} -count} == 0)
		return
	end
	str name
	foreach name ({el {group}/#[]/#[][TYPE=kchan]})
		perm = {getfield {name} perm}
		echo {getpath {el {name}/..} -tail}/{getpath {name} -tail} {perm} >> {filename}
	end
	echo >> {filename}
end

// This function dumps all the info for pools
function db_dumppools(filename, group)
	str filename
	str group

	float CoInit, buffered
	int i, j

	str name

	if ({getarg {el {group}/#[][TYPE=kpool]} -count} == 0)
		return
	end

	foreach name ({el {group}/#[][TYPE=kpool]})
		CoInit = {getfield {name} CoInit}
		buffered = {getfield {name} slave_enable}
		if ({{buffered} & 4} == 4)
			buffered = 1
		else
			buffered = 0
		end
		echo insert into molecule (name, pathway, is_buffered, initial_conc, notes) >> {filename}
		echo VALUES ('{getpath {name} -tail}', {pathway_id}, {buffered}, {CoInit},  >> {filename}
		db_dumpnotes {filename} {name}
	end
end
// This function makes a string of the names of all msg dest of the
// given msgtype
function find_outputs(elm, mtype)
	str elm
	str mtype

	int i
	int j = 0
	int nmsg = {getmsg {elm} -outgoing -count}
	str type
	str dest
	str ret = ""

	for (i = 0; i < nmsg; i = i + 1)
		type = {getmsg {elm} -outgoing -type {i}}
		if ({strcmp {type} {mtype}} == 0)
			dest = {getmsg {elm} -outgoing -dest {i}}
			dest = {getpath {dest} -tail}
			ret = ret @ "'" @ {dest} @ "', "
			j = j + 1
		end
	end
	for (i = j; j < 5; j = j + 1)
		ret = ret @ "'', "
	end
	return {ret}
end

// This function makes a string of the names of all msg src of the
// given msgtype
function find_inputs(elm, mtype)
	str elm
	str mtype

	int i
	int j = 0
	int nmsg = {getmsg {elm} -incoming -count}
	str type
	str src
	str ret = ""

	for (i = 0; i < nmsg; i = i + 1)
		type = {getmsg {elm} -incoming -type {i}}
		if ({strcmp {type} {mtype}} == 0)
			src = {getmsg {elm} -incoming -source {i}}
			src = {getpath {src} -tail}
			ret = ret @ "'" @ {src} @ "', "
			j = j + 1
		end
	end
	for ( i = j; j < 5; j = j + 1)
		ret = ret @ "'', "
	end
	return {ret}
end

// This function dumps all the info for enzs
function db_dumpenzs(filename, group)
	str filename
	str group

	float k1, k2, k3, Km, Vmax
	int i, j
	int nsub
	str orig_name = {getfield /parmedit/enz elmpath}
	str name

	if ({getarg {el {group}/#[]/#[][TYPE=kenz]} -count} == 0)
		return
	end

	foreach name ({el {group}/#[]/#[][TYPE=kenz]})
		k1 = {getfield {name} k1}
		k2 = {getfield {name} k2}
		k3 = {getfield {name} k3}
		calc_mm {name}

		Vmax = {getfield /parmedit/enz/Vmax value}
		Km  = {getfield /parmedit/enz/Km value}
		nsub = {find_enz_order {name}}

		echo insert into enzyme (name, pathway, parent, Km, Vmax, ratio, nsub, nprd, sub1, sub2, sub3, sub4, sub5, prd1, prd2, prd3, prd4, prd5, notes) >> {filename}
		echo VALUES ('{getpath {name} -tail}', {pathway_id}, '{getpath {el {name}/..} -tail}', {Km}, {Vmax}, {k2/k3}, {nsub}, 0, >> {filename}
		echo {find_inputs {name} "SUBSTRATE"} >> {filename}
		echo {find_outputs {name} "MM_PRD"} >> {filename}

		db_dumpnotes {filename} {name}
	end

	setfield /parmedit/enz elmpath {orig_name}
	do_update_enzinfo
end

// This function dumps all the info for reacs
function db_dumpreacs(filename, group)
	str filename
	str group

	float kf, kb, Kf, Kb, Kd, tau
	int i, j
	str sub_order
	str prd_order

	str name
	str orig_name = {getfield /parmedit/reac elmpath}

	if ({getarg {el {group}/#[][TYPE=kreac]} -count} == 0)
		return
	end
	foreach name ({el {group}/#[][TYPE=kreac]})
		setfield /parmedit/reac elmpath {name}
		do_update_reac_scaling

		kf = {getfield {name} kf}
		kb = {getfield {name} kb}

		Kf = {getfield /parmedit/reac/skf value}
		Kb = {getfield /parmedit/reac/skb value}
		Kd = {getfield /parmedit/reac/Kd value}
		tau = {getfield /parmedit/reac/Tau value}
		sub_order = {getfield /parmedit/reac/forder label}
		prd_order = {getfield /parmedit/reac/border label}
		echo insert into reaction (name, pathway, Kf, Kb, nsub, nprd, sub1, sub2, sub3, sub4, sub5, prd1, prd2, prd3, prd4, prd5, notes) >> {filename}
		echo VALUES ('{getpath {name} -tail}', {pathway_id}, {Kf}, {Kb}, {sub_order}, {prd_order}, >> {filename}

		echo {find_inputs {name} "SUBSTRATE"} >> {filename}
		echo {find_inputs {name} "PRODUCT"} >> {filename}
		db_dumpnotes {filename} {name}
	end
	setfield /parmedit/reac elmpath {orig_name}
	do_update_reacinfo
end

// This function dumps all info for a group
function db_dumpgroup(filename, group)
	str pathname = {getpath {group} -tail}
	echo replace into pathway (accessno, name, entrydate, developer, transcriber, notes) >> {filename}
	
	echo VALUES ({pathway_id}, '{pathname}', CURDATE(), 'Upi Bhalla', 'Upi Bhalla', >> {filename}
	db_dumpnotes {filename} {group}

	echo "update pathway set figure=LOAD_FILE('/tmp/"{pathname}".png') where accessno = "{pathway_id}";" >> {filename}
	echo "update pathway set kkitfile=LOAD_FILE('/tmp/"{pathname}".g') where accessno = "{pathway_id}";" >> {filename}


	db_dumpreacs {filename} {group}
	db_dumpenzs {filename} {group}
	// dumpchans {filename} {group}
	db_dumppools {filename} {group}

	echo done dumping {group}
end

function dumpunits(filename)
	str filename

	echo >> {filename}
	echo Concentration units: {getfield /editcontrol/conc unitname} >> {filename}
	echo Time units: {getfield /editcontrol/time unitname} >> {filename}
	echo Default {getfield /editcontrol/vol label} :  {getfield /editcontrol/vol value} >> {filename}
	echo >> {filename}
end

function dump_db(filename, pid)
	str filename
	int pid

	pathway_id = pid

	str name

// This function scans through the entire simulation in stages, and
// dumps out all rates, initial values, and the order of reactions.
	
//	dumpunits {filename}
	echo "use doqcs;" >> {filename}

	db_dumpgroup {filename} /kinetics
	pathway_id = pathway_id + 1
	foreach name ({el /kinetics/##[][TYPE=group]})
		db_dumpgroup {filename} {name}
		pathway_id = pathway_id + 1
	end
	/*
	echo "-------------------------------------------" >> {filename}
	echo "-------------------------------------------" >> {filename}
	echo >> {filename}
	echo "Same data, sorting by data type:" >>  {filename}
	echo >> {filename}
	dumpgroup {filename} /##[]
	*/

	do_inform "Finished dumping simulation data to "{filename}
end

