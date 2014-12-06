// This file has a hack for copying notes from one .g file to
// the running model.
// It works by using the framework of the xcomparemodel.g system,
// where the new model file is edited and loaded into /tempkin.
// The trick is to replace the 
// 		do_actual_compare(start1, start2)
// function with a dummy version that does the copying of the notes.

// Notes are copied to any and all elements where the names match.
// regardless of the grouping.

// There is also a utility function for deleting notes from all
// existing elements.


function delete_all_notes
	str name
	setfield /parmedit/pool/notes initialtext ""
	foreach name ({el /kinetics/##[]})
		if ({exists {name}/notes})
			call {name}/notes FROM_XTEXT /parmedit/pool/notes
		end
	end
end

function do_actual_compare(start1, start2)
	str name
	str group
	str tail
	echo "//genesis" > all__notes.g
	foreach group ({el {start1}/##[]})
		foreach name ({el {start2}/##[]})
			tail = {getpath {name} -tail}
			if ({exists {group}/{tail}} && {exists {name}/notes})
				echo {name}
				echo "" > notes1.txt
				echo "" > notes2.txt
				call {name}/notes SAVE notes2.txt
				call {group}/{tail}/notes SAVE notes1.txt

				sh "sed -f sscript1 notes1.txt > notes3.txt"
			//	sh "sed -e 's/\"$/ \\/' notes1.txt > notes1.txt"
		//		sh sed -e "/LOAD/d" notes2.txt > notes2.txt
				sh "sed -f sscript2 notes2.txt >> notes3.txt"
//				cat notes2.txt >> notes1.txt
				sh "cat notes3.txt >> all__notes.g"
			end
		end
	end
	echo "All done, please type all__notes.g"
end
