//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-2001 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

int pathway_id

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
	echo Channels for group {group} >> {filename}
	echo name perm >> {filename}
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

	echo "insert into molecules" >> {filename}
	echo "values( " >> {filename}
	echo "name"

	echo Pools for group {group} >> {filename}
	echo "name	CoInit	buffered" >> {filename}
	foreach name ({el {group}/#[][TYPE=kpool]})
		CoInit = {getfield {name} CoInit}
		buffered = {getfield {name} slave_enable}
		if ({{buffered} & 4} == 4)
			buffered = 1
		else
			buffered = 0
		end

		echo {getpath {name} -tail}"	"{pathway_id}"	"{CoInit}"	"{buffered} >> {filename}
	end
	echo >> {filename}
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
	echo Enzymes for group {group} >> {filename}
	echo name Km Vmax ratio nsub nprd  >> {filename}
	foreach name ({el {group}/#[]/#[][TYPE=kenz]})
		k1 = {getfield {name} k1}
		k2 = {getfield {name} k2}
		k3 = {getfield {name} k3}
		calc_mm {name}

		Vmax = {getfield /parmedit/enz/Vmax value}
		Km  = {getfield /parmedit/enz/Km value}
		nsub = {find_enz_order {name}}

		echo {getpath {el {name}/..} -tail}/{getpath {name} -tail} {Km} {Vmax} {k2 / k3} {nsub} {nprd} >> {filename}
	end
	setfield /parmedit/enz elmpath {orig_name}
	do_update_enzinfo
	echo >> {filename}
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
	echo Reactions for group {group} >> {filename}
	echo name kf kb Kf Kb Kd tau sub_order prd_order >> {filename}
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
		echo {getpath {name} -tail} {kf} {kb} {Kf} {Kb} {Kd} {tau} {sub_order} {prd_order} >> {filename}
	end
	setfield /parmedit/reac elmpath {orig_name}
	do_update_reacinfo
	echo >> {filename}
end

// This function dumps all info for a group
function db_dumpgroup(filename, group)
	echo replace into pathway (accessno, name, entrydate, notes) >> {filename}
	
	echo VALUES ({pathway_id}, {getpath {group} -tail}, CURDATE(), >> {filename}
	call {group}/notes SAVE textsave
	sed -e "/LOAD/d" textsave > temp
	// echo "/^/ a \\" > sfile
	// echo " " >> sfile

	sed -f /home/bhalla/scripts/kkit8/sfile temp >> {filename}
	echo > textsave
	echo ");" >> {filename}


/*
	echo >> {filename}
	echo >> {filename}
	echo Data for group {group} >> {filename}
	dumpreacs {filename} {group}
	dumpenzs {filename} {group}
	dumpchans {filename} {group}
	dumppools {filename} {group}
	echo >> {filename}
	echo "-------------------------------------------" >> {filename}
	*/
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
	
	dumpunits {filename}

	db_dumpgroup {filename} /kinetics
	foreach name ({el /kinetics/##[][TYPE=group]})
		db_dumpgroup {filename} {name}
	end

	echo "-------------------------------------------" >> {filename}
	echo "-------------------------------------------" >> {filename}
	echo >> {filename}
	echo "Same data, sorting by data type:" >>  {filename}
	echo >> {filename}
	dumpgroup {filename} /##[]

	do_inform "Finished dumping simulation data to "{filename}
end

