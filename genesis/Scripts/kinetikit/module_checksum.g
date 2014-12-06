// Checks that the message source  src  lies within the module  mod
// but not on any uplinks or downlinks.
function check_mod_msgsrc(src, mod)
	str name
	
	if ({isachild {src} {mod}})
		foreach name ({el {mod}/#[][ISA=uplink]/#[][ISA=linkinfo]})
			if ({isachild {src} {name}})
				return 0
			end
		end
		return 1
	end
	return 0
end


// This is asymmetrical as it only looks at incoming msgs.
// This should be fine as it restricts itself to the messages within a
// specified module.

function essential_module_msgs(elm, mod)
	str elm
	str mod

	int i
	str src
	int nmsgs = {getmsg {elm} -incoming -count}
	for (i = 0; i < nmsgs; i = i + 1)
		src = {getmsg {elm} -incoming -source {i}}
		if ({check_mod_msgsrc {src} {mod}} == 1)
			echo msg {src} {elm} {getmsg {elm} -incoming -type {i}} >> /tmp/kkit_modif
		end
	end
end

function append_to_md5sum_file(mod)
    str mod 

    str name
	str lp
	foreach name ( {el {mod},{mod}/#[],{mod}/#[][ISA!=uplink]/##[],{mod}/#[][ISA=uplink]/proto,{mod}/#[][ISA=uplink]/proto/##[]} )
		if ({exists {name} linkparms})
			lp = {getfield {name} linkparms}
			showfield {name} {arglist {lp}} >> /tmp/kkit_modif
		end
		if ({exists {name}/notes})
			call {name}/notes SAVE "/tmp/kkit_modif"
		end
		essential_module_msgs {name} {mod}
	end
end

// This function computes an md5sum to characterize a module. This
// sum includes essential parameters and all message in the module
// EXCEPT those which pertain to links.
function calc_md5sum(mod)
    str mod

	echo Starting md5sum_file for module {mod} > /tmp/kkit_modif
	append_to_md5sum_file {mod}

	sh md5sum /tmp/kkit_modif > /tmp/kkit_modif_sum
//	sh rm /tmp/kkit_modif

	openfile /tmp/kkit_modif_sum r
	str ret = {getarg {readfile /tmp/kkit_modif_sum} -arg 1}
	closefile /tmp/kkit_modif_sum
	sh rm /tmp/kkit_modif_sum

	return {ret}
end

// This function computes an md5sum to characterize the structure
// of a module or a link. It includes no names or parameters,
// but the order of listing of the elms is critical as this is 
// required for filling in the parms.
// Still incomplete: Uses names.
function calc_struct_md5sum(mod)
	str mod

	str name
	sh /bin/rm /tmp/kkit_modif
	foreach name ({el {mod}/##[]})
		echo {getfield {name} object->name} >> /tmp/kkit_modif
		essential_module_msgs {name} {mod}
	end

	sh md5sum /tmp/kkit_modif > /tmp/kkit_modif_sum
//	sh rm /tmp/kkit_modif

	openfile /tmp/kkit_modif_sum r
	str ret = {getarg {readfile /tmp/kkit_modif_sum} -arg 1}
	closefile /tmp/kkit_modif_sum
	sh /bin/rm /tmp/kkit_modif_sum

	return {ret}
end
