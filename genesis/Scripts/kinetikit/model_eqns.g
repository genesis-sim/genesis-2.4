//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-2000 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

// This function prints out units given the # of molecules involved
function dump_units(i)
	int i

	if (i < 2)
		return "s^-1"
	end

	if (i >= 2)
		return "uM^-" @ {i - 1} @ ".s^-1"
	end
end

function dump_num_units(i)
	int i

	if (i < 2)
		return "s^-1"
	end

	if (i >= 2)
		return "#^-" @ {i - 1} @ ".s^-1"
	end
end

// This function dumps equations for reacs
function reaceqns(filename, group)
	str filename
	str group

	float kf, kb, Kf, Kb, Kd, tau
	int i, j

	str reac
	str fwd_units
	str bwd_units
	str fwd_num_units
	str bwd_num_units
	if ({getarg {el {group}/#[][TYPE=kreac]} -count} == 0)
		return
	end
	echo Reactions for group {group} >> {filename}
	echo "Reaction	kf	kb	Kf	Kb" >> {filename}
	foreach reac ({el {group}/#[][TYPE=kreac]})
		int nmsg = {getmsg {reac} -incoming -count}
		str lastsrc = ""
		str src = ""
		str type
		int nmol = 1
		int nreactant = 0
		for (i = 0; i < nmsg; i = i + 1)
			type = {getmsg {reac} -incoming -type {i}}
			if ({strcmp {type} "SUBSTRATE"} == 0)
				nreactant = nreactant + 1
				src = {getmsg {reac} -incoming -source {i}}
				if ({strcmp {src} {lastsrc}} == 0)
					nmol = nmol + 1
				else
					if ({strlen {lastsrc}} > 0)
						if (nmol > 1)
							echo {nmol} " " -n >> {filename}
						end
						echo {getpath {lastsrc} -tail} "+ " -n >> {filename}
						nmol = 1
					end
					lastsrc = src
				end
			end
		end
		fwd_units = {dump_units {nreactant}}
		fwd_num_units = {dump_num_units {nreactant}}
		if ({strlen {lastsrc}} > 0)
			if (nmol > 1)
				echo {nmol} " " -n >> {filename}
			end
			echo {getpath {lastsrc} -tail} "<===> " -n >> {filename}
		end

		nmol = 1
		nreactant = 0

		str lastprd = ""
		str prd = ""

		for (i = 0; i < nmsg; i = i + 1)
			type = {getmsg {reac} -incoming -type {i}}
			if ({strcmp {type} "PRODUCT"} == 0)
				nreactant = nreactant + 1
				prd = {getmsg {reac} -incoming -source {i}}
				if ({strcmp {prd} {lastprd}} == 0)
					nmol = nmol + 1
				else
					if ({strlen {lastprd}} > 0)
						if (nmol > 1)
							echo {nmol} " " -n >> {filename}
						end
						echo {getpath {lastprd} -tail} "+ " -n >> {filename}
						nmol = 1
					end
					lastprd = prd
				end
			end
		end
		bwd_units = {dump_units {nreactant}}
		bwd_num_units = {dump_num_units {nreactant}}
		if ({strlen {lastprd}} > 0)
			if (nmol > 1)
				echo {nmol} " " -n >> {filename}
			end
			echo {getpath {lastprd} -tail} -n >> {filename}
		end

		kf = {getfield {reac} kf}
		kb = {getfield {reac} kb}
		echo "	" {kf} {fwd_num_units} "	" {kb} {bwd_num_units} -n >> {filename}

		setfield /parmedit/reac elmpath {reac}
		do_update_reac_scaling
		float skf = {getfield /parmedit/reac/skf value}
		float skb = {getfield /parmedit/reac/skb value}
		echo "	" {skf} {fwd_units} "	" {skb} {bwd_units} >> {filename}

	end
	echo >> {filename}
end

function chaneqns(filename, group)
	str filename
	str group

	float perm
	int i, j

	if ({getarg {el {group}/#[]/#[][TYPE=kchan]} -count} == 0)
		return
	end
	str name
	str type
	str sub
	str prd
	echo Channels for group {group} >> {filename}
	echo name perm >> {filename}
	foreach name ({el {group}/#[]/#[][TYPE=kchan]})
		int nmsg = {getmsg {name} -incoming -count}
		for (i = 0; i < nmsg; i = i + 1)
			type = {getmsg {name} -incoming -type {i}}
			if ({strcmp {type} "SUBSTRATE"} == 0)
				sub = {getmsg {name} -incoming -source {i}}
			end
			if ({strcmp {type} "PRODUCT"} == 0)
				prd = {getmsg {name} -incoming -source {i}}
			end
		end
		perm = {getfield {name} perm}
		echo {getpath {sub} -tail} "<---chan="{getpath {el {name}/..} -tail}"--->" {getpath {prd} -tail} "	" {perm} >> {filename}
	end
	echo >> {filename}
end

function enzeqns(filename, group)
	str filename
	str group

	float k1, k2, k3
	int i, j

	str enz
	if ({getarg {el {group}/#[]/#[][TYPE=kenz]} -count} == 0)
		return
	end
	echo Enzymes for group {group} >> {filename}
	echo "Enzyme-reaction	k1	k2	k3	Km	kcat	ratio" >> {filename}
	foreach enz ({el {group}/#[]/#[][TYPE=kenz]})
		int nmsg = {getmsg {enz} -incoming -count}
		str lastsrc = ""
		str src = ""
		str type
		int nmol = 1
		for (i = 0; i < nmsg; i = i + 1)
			type = {getmsg {enz} -incoming -type {i}}
			if ({strcmp {type} "SUBSTRATE"} == 0)
				src = {getmsg {enz} -incoming -source {i}}
				if ({strcmp {src} {lastsrc}} == 0)
					nmol = nmol + 1
				else
					if ({strlen {lastsrc}} > 0)
						if (nmol > 1)
							echo {nmol} " " -n >> {filename}
						end
						echo {getpath {lastsrc} -tail} "+ " -n >> {filename}
						nmol = 1
					end
					lastsrc = src
				end
			end
		end
		if ({strlen {lastsrc}} > 0)
			if (nmol > 1)
				echo {nmol} " " -n >> {filename}
			end
			echo {getpath {lastsrc} -tail} "---"{getpath {el {enz}/..} -tail}"--> " -n >> {filename}
		end
		nmol = 1

		str lastprd = ""
		str prd = ""

		int nmsg = {getmsg {enz} -outgoing -count}
		for (i = 0; i < nmsg; i = i + 1)
			type = {getmsg {enz} -outgoing -type {i}}
			if ({strcmp {type} "MM_PRD"} == 0)
				prd = {getmsg {enz} -outgoing -dest {i}}
				if ({strcmp {prd} {lastprd}} == 0)
					nmol = nmol + 1
				else
					if ({strlen {lastprd}} > 0)
						if (nmol > 1)
							echo {nmol} " " -n >> {filename}
						end
						echo {getpath {lastprd} -tail} "+ " -n >> {filename}
						nmol = 1
					end
					lastprd = prd
				end
			end
		end
		if ({strlen {lastprd}} > 0)
			if (nmol > 1)
				echo {nmol} " " -n >> {filename}
			end
			echo {getpath {lastprd} -tail} -n >> {filename}
		end

		k1 = {getfield {enz} k1}
		k2 = {getfield {enz} k2}
		k3 = {getfield {enz} k3}
		echo "	" {k1} "#^-1.s^-1	" {k2} "s^-1	" {k3} "s^-1" -n >> {filename}

		calc_mm {enz}
		float km = {getfield /parmedit/enz/Km value}
		float vmax = {getfield /parmedit/enz/Vmax value}
		float ratio = {getfield /parmedit/enz/ratio value}

		echo "	" {km} "uM	" {vmax} "s^-1	" {ratio} >> {filename}
	end
	echo >> {filename}
end

// This function dumps all eqns for a group
function groupeqns(filename, group)
	echo >> {filename}
	echo >> {filename}
	echo Equations for group {group} >> {filename}
	reaceqns {filename} {group}
	enzeqns {filename} {group}
	chaneqns {filename} {group}
	dumppools {filename} {group}
	echo >> {filename}
	echo "-------------------------------------------" >> {filename}
end


function do_model_eqns(filename)
	str filename

	str name

// This function scans through the entire simulation in stages, and
// dumps out all rates, initial values, and the order of reactions.
	
	dumpunits {filename}

	groupeqns {filename} /kinetics
	foreach name ({el /kinetics/##[][TYPE=group]})
		groupeqns {filename} {name}
	end

	echo "-------------------------------------------" >> {filename}
	echo "-------------------------------------------" >> {filename}
	echo >> {filename}
	echo "Same data, sorting by data type:" >>  {filename}
	echo >> {filename}
	groupeqns {filename} /##[]

	do_inform "Finished dumping simulation equations to "{filename}
end

