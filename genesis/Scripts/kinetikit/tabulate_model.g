//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-2000 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

// This function dumps all the info for channels
function dumpchans(filename, group)
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
function dumppools(filename, group)
	str filename
	str group

	float CoInit, buffered
	float vol
	int i, j

	str name

	if ({getarg {el {group}/#[][TYPE=kpool]} -count} == 0)
		return
	end

	echo Pools for group {group} >> {filename}
	echo "name	InitialConc	buffered	Volume" >> {filename}
	foreach name ({el {group}/#[][TYPE=kpool]})
		CoInit = {getfield {name} CoInit}
		buffered = {getfield {name} slave_enable}
		vol = {{getfield {name} vol} / 600}
		if ({{buffered} & 4} == 4)
			buffered = 1
		else
			buffered = 0
		end

		echo {getpath {name} -tail}"	"{CoInit}" uM	"{buffered}"	"{vol}" fl" >> {filename}
	end
	echo >> {filename}
end

// This function dumps all the info for enzs
function dumpenzs(filename, group)
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
	echo name k1 k2 k3 Km Vmax nsub >> {filename}
	foreach name ({el {group}/#[]/#[][TYPE=kenz]})
		k1 = {getfield {name} k1}
		k2 = {getfield {name} k2}
		k3 = {getfield {name} k3}
		calc_mm {name}

		Vmax = {getfield /parmedit/enz/Vmax value}
		Km  = {getfield /parmedit/enz/Km value}
		nsub = {find_enz_order {name}}

		echo {getpath {el {name}/..} -tail}/{getpath {name} -tail} {k1} {k2} {k3} {Km} {Vmax} {nsub} >> {filename}
	end
	setfield /parmedit/enz elmpath {orig_name}
	do_update_enzinfo
	echo >> {filename}
end

// This function dumps all the info for reacs
function dumpreacs(filename, group)
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
function dumpgroup(filename, group)
	echo >> {filename}
	echo >> {filename}
	echo Data for group {group} >> {filename}
	dumpreacs {filename} {group}
	dumpenzs {filename} {group}
	dumpchans {filename} {group}
	dumppools {filename} {group}
	echo >> {filename}
	echo "-------------------------------------------" >> {filename}
end

function dumpunits(filename)
	str filename

	echo >> {filename}
	echo Concentration units: {getfield /editcontrol/conc unitname} >> {filename}
	echo Time units: {getfield /editcontrol/time unitname} >> {filename}
	echo Default {getfield /editcontrol/vol label} :  {getfield /editcontrol/vol value} >> {filename}
	echo >> {filename}
end

function do_tabulate_model(filename)
	str filename

	str name

// This function scans through the entire simulation in stages, and
// dumps out all rates, initial values, and the order of reactions.
	
	dumpunits {filename}

	dumpgroup {filename} /kinetics
	foreach name ({el /kinetics/##[][TYPE=group]})
		dumpgroup {filename} {name}
	end

	echo "-------------------------------------------" >> {filename}
	echo "-------------------------------------------" >> {filename}
	echo >> {filename}
	echo "Same data, sorting by data type:" >>  {filename}
	echo >> {filename}
	dumpgroup {filename} /##[]

	do_inform "Finished dumping simulation data to "{filename}
end

function make_comparecontrol
	create xform /comparecontrol [{EX},0,{EW},{WINHT - WMH - BORDER}]
	ce /comparecontrol
	create xbutton help -label "Instructions for running comparisons" \
		-script "compare_load_instructions"
	// create xtext instructions -hgeom 250 -bg white

	create xlabel settings -bg cyan -label "Settings for comparison"
	create xdialog start1 -value "/kinetics" \
		-label "Reference group:" -script "comp_get_group.d3 <w>"
	create xdialog start2 -value "/kinetics" \
		-label "Comparison group:" -script "comp_get_group.d3 <w>"
	create xlabel running -bg cyan -label "Running comparison"
	create xdialog filename -label "Compare against file:" \
		-script "do_compare_file_model <v>"
	create xbutton comp_within_model -script "do_compare_within_model" \
		-label "Compare within model"
	create xtext compareinfo [0,0:last,100%,30:NULL.bottom] -bg white
	create xbutton HIDE -script "xhide /comparecontrol" -ygeom 2:last
	compare_load_instructions
end

