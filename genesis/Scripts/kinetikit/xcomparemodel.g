//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-1997 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/
/* graphics for upi's simulation of second messengers in LTP
*/


function cmpdmy
// a dummy function for the filtered file to get rid of awkward
// commands
end

function do_compare_file_model(filename)
	str filename

//	This function works in the following steps:
// 1. Filters the specified filename to another without the
//		addmsgs and the loadtabs etc, and changing all instances of
//		/kinetics to /tempkin
// 2. Creates a group /tempkin
// 3. Prompts the user to type in the new file name for loading
// 4. Does the parm comparison and echoes the results into a file
// 5. Deletes /tempkin
// 6. Reads in the parm comparison result file into the comparinfo xtext widget.

// 1. Filters the specified filename
	echo 1,14d > /tmp/mungedump.sed
	echo s.call.cmpdmy. >> /tmp/mungedump.sed
	echo s.loadtab.cmpdmy. >> /tmp/mungedump.sed
	echo s.xtextload.cmpdmy. >> /tmp/mungedump.sed
	echo s/^simundump xgraph/cmpdmy/ >> /tmp/mungedump.sed
	echo s/^simundump xplot/cmpdmy/ >> /tmp/mungedump.sed
	echo s/^simundump xtree/cmpdmy/ >> /tmp/mungedump.sed
	echo s/^simundump xcoredraw/cmpdmy/ >> /tmp/mungedump.sed
	echo /addmsg/d >> /tmp/mungedump.sed
	echo s/kinetics/tempkin/ >> /tmp/mungedump.sed
	echo /reset/d >> /tmp/mungedump.sed

	sed -f /tmp/mungedump.sed {filename} > /tmp/munged.g

	str start1 = {getfield /comparecontrol/start1 value}
	str start2 = {getfield /comparecontrol/start2 value}
	start2 = {strsub {start2} kinetics tempkin}

	echo do_actual_compare {start1} {start2} >> /tmp/munged.g
	echo delete /tempkin >> /tmp/munged.g

// 2. Creates a group /tempkin
	create group /tempkin

// 3. Prompts the user to type in the new file name for loading
	do_inform "Please type:   'include /tmp/munged'    to load file"
end

function do_compare_within_model
	str start1 = {getfield /comparecontrol/start1 value}
	str start2 = {getfield /comparecontrol/start2 value}
	do_actual_compare {start1} {start2}
end

function do_actual_compare(start1,start2)
	str start1,start2

// 4. Does the parm comparison and echoes the results into a file
	str name
	str tempname
	float origval , compareval
	int ndiff = 0
	str fout = "/tmp/res"
	echo > {fout}

	echo Comparing {start1} (reference) with {start2} (comparison) >> {fout}

	echo Elements present in reference but not comparison model: >> {fout}
	foreach name ({el {start1}/##[TYPE=kpool],{start1}/##[TYPE=kenz],{start1}/##[TYPE=kreac]})
		tempname = {strsub {name} {start1} {start2}}
		if (!{exists {tempname}})
			ndiff = ndiff + 1
			echo {ndiff}. {name} >> {fout}
		end
	end
	echo >> {fout}

	echo Elements present in comparison but not reference model: >> {fout}
	foreach name ({el {start2}/##[TYPE=kpool],{start2}/##[TYPE=kenz],{start2}/##[TYPE=kreac]})
		tempname = {strsub {name} {start2} {start1}}
		if (!{exists {tempname}})
			ndiff = ndiff + 1
			echo {ndiff}. {name} >> {fout}
		end
	end
	echo >> {fout}

	echo Comparing pool values >> {fout}

	foreach name ({el {start1}/##[TYPE=kpool]})
		tempname = {strsub {name} {start1} {start2}}
		if ({exists {tempname}})
			origval = {getfield {name} CoInit} 
			compareval = {getfield {tempname} CoInit} 
			if (origval != compareval)
				ndiff = ndiff + 1
				echo {ndiff}. {name} CoInit={origval} != {compareval} >> {fout}
			end

			origval = {getfield {name} vol} 
			compareval = {getfield {tempname} vol} 
			if (origval != compareval)
				ndiff = ndiff + 1
				echo {ndiff}. {name} vol={origval} != {compareval} >> {fout}
			end

			origval = {getfield {name} slave_enable} 
			compareval = {getfield {tempname} slave_enable} 
			if (origval != compareval)
				ndiff = ndiff + 1
				echo {ndiff}. {name} slave_enable={origval} != {compareval} >> {fout}
			end
		end
	end
	echo >> {fout}
	echo Comparing enzyme values >> {fout}
	foreach name ({el {start1}/##[TYPE=kenz]})
		tempname = {strsub {name} {start1} {start2}}
		if ({exists {tempname}})

			origval = {getfield {name} k1} 
			compareval = {getfield {tempname} k1} 
			if (origval != compareval)
				ndiff = ndiff + 1
				echo {ndiff}. {name} k1={origval} != {compareval} >> {fout}
			end

			origval = {getfield {name} k2} 
			compareval = {getfield {tempname} k2} 
			if (origval != compareval)
				ndiff = ndiff + 1
				echo {ndiff}. {name} k2={origval} != {compareval} >> {fout}
			end

			origval = {getfield {name} k3} 
			compareval = {getfield {tempname} k3} 
			if (origval != compareval)
				ndiff = ndiff + 1
				echo {ndiff}. {name} k3={origval} != {compareval} >> {fout}
			end
		end
	end
	echo >> {fout}
	echo Comparing reac values >> {fout}
	foreach name ({el {start1}/##[TYPE=kreac]})
		tempname = {strsub {name} {start1} {start2}}
		if ({exists {tempname}})

			origval = {getfield {name} kf} 
			compareval = {getfield {tempname} kf} 
			if (origval != compareval)
				ndiff = ndiff + 1
				echo {ndiff}. {name} kf={origval} != {compareval} >> {fout}
			end

			origval = {getfield {name} kb} 
			compareval = {getfield {tempname} kb} 
			if (origval != compareval)
				ndiff = ndiff + 1
				echo {ndiff}. {name} kb={origval} != {compareval} >> {fout}
			end
		end
	end
	echo >> {fout}

	if (ndiff == 0)
		echo No differences found between models {start1} and {start2} >> {fout}
	else
		echo {ndiff} differences between models {start1} and {start2} >> {fout}
	end

// 6. Reads in the result file into the comparinfo xtext widget.
	setfield /comparecontrol/compareinfo filename {fout}
	// xhide /comparecontrol/run_comparison
end

function comp_get_group(widget)
	str widget
	setfield {widget} value {getfield /parmedit/group elmpath}
end

function compare_load_instructions
	xtextload /comparecontrol/compareinfo \
		"       Instructions for the comparison tool:" \
		"This tool is used to compare the kinetic parameters of two" \
		"models. The models can be parts of the same large model if "\
		"it has already been loaded in, or there can be a pre-loaded" \
		"model and a comparison model in a file." \
		"" \
		"Comparisons are done only on the kinetic parameters, not on" \
		"the variables or graphical values. These are:" \
		"pools:        CoInit, vol, slave_enable" \
		"reacs:        kf, kb" \
		"enzs:         k1, k2, k3, vol" \
		"channels:     perm" \
		"" \
		"All the children of a group are compared. By default, the" \
		"starting group is /kinetics so that the entire model is" \
		"compared. More selective comparisons can be done by entering" \
		"the group path in the 'Reference' and 'Comparison' dialogs." \
		"As a shortcut, you can double-click on the relevant group" \
		"and then right-click on the appropriate dialog to set it." \
		"" \
		"If you are doing a comparison within a pre-loaded model," \
		"just hit the 'Compare within model' button." \
		"" \
		"If you are comparing against model saved in a file, enter" \
		"the file name in the 'Compare against file:' dialog and" \
		"hit return." \
		"Due to a much-lamented restriction on re-entrant parsing" \
		"in Genesis, one cannot load a script file from within" \
		"Xodus. So at this point you have to type in the name of" \
		"a temporary comparison file '/tmp/munged'." \
		"" \
		"The results of the comparison are displayed in this text" \
		"window."
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
