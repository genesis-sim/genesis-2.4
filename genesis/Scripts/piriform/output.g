//genesis
str name

echo setting up disk outputs

function disk(diskpath, srcpath, field)
    create x1viewdata /output/{diskpath}
    setfield /output/{diskpath} filename {diskpath} leave_open 1 flush 1
    addmsg {srcpath} /output/{diskpath} SAVE {field}
    useclock /output/{diskpath} 1
    echo {diskpath}
end

function disk2(diskpath, srcpath, field)
    create disk_out /output/{diskpath}
    setfield /output/{diskpath} filename {diskpath} leave_open 1 flush 1
    addmsg {srcpath} /output/{diskpath} SAVE {field}
    useclock /output/{diskpath} 1
    echo {diskpath}
end

function disable_Vm
    disable /output/soma_Vm
    disable /output/Ia_Vm
    disable /output/supIb_Vm
    disable /output/deepIb_Vm
    disable /output/III_Vm
    disable /output/fb_Vm
    disable /output/ff_Vm
end

function enable_Vm
    enable /output/soma_Vm
    enable /output/Ia_Vm
    enable /output/supIb_Vm
    enable /output/deepIb_Vm
    enable /output/III_Vm
    enable /output/fb_Vm
    enable /output/ff_Vm
end

function disable_conductance_output
    foreach name (Aff RA CA Local FB FF)
	disable /output/pyr_g{name}
    end
end

function enable_conductance_output
    foreach name (Aff RA CA Local FB FF)
	enable /output/pyr_g{name}
    end
end


/*
** DISK OUTPUT
** write out the intracellular membrane potentials to disk
** for all elements in /pyr
*/

function set_out
	disk2 soma_Vm /pyr/pyramidal[]/soma Vm
	disk2 fb_Vm /fb/interneuron[]/soma Vm
	disk2 ff_Vm /ff/interneuron[]/soma Vm
	if ({MULTI})
		disk2 Ia_Vm /pyr/pyramidal[]/soma/Ia_dend Vm
		disk2 supIb_Vm /pyr/pyramidal[]/soma/supIb_dend Vm
		disk2 deepIb_Vm /pyr/pyramidal[]/soma/deepIb_dend Vm
		disk2 III_Vm /pyr/pyramidal[]/soma/III_dend Vm
	end

	/*
** CONDUCTANCES
*/
	if ({MULTI})
		str base = "/pyr/pyramidal[]/soma"
		disk2 pyr_gAff {base}/Ia_dend/Aff_Na_channel Gk
		disk2 pyr_gRA {base}/deepIb_dend/RA_Na_channel Gk
		disk2 pyr_gCA {base}/supIb_dend/CA_Na_channel Gk
		disk2 pyr_gLocal {base}/III_dend/Local_Na_channel Gk
		disk2 pyr_gFB {base}/Cl_channel Gk
		disk2 pyr_gFF {base}/Ia_dend/K_channel Gk
		disk2 ff_g /ff/interneuron[]/SNa_channel Gk
		disk2 fb_g /fb/interneuron[]/SNa_channel Gk
	else
		disk2 pyr_gAff /pyr/pyramidal[]/Aff_Na_channel Gk
    		disk2 pyr_gRA /pyr/pyramidal[]/RA_Na_channel Gk
    		disk2 pyr_gCA /pyr/pyramidal[]/CA_Na_channel Gk
    		disk2 pyr_gLocal /pyr/pyramidal[]/Local_Na_channel Gk
    		disk2 pyr_gFB /pyr/pyramidal[]/Cl_channel Gk
    		disk2 pyr_gFF /pyr/pyramidal[]/K_channel Gk
	end

	/*
** CURRENTS
*/
	if ({MULTI})
		str base
	    	disk2 soma_Im /pyr/pyramidal[]/soma Im
    		foreach base (Ia supIb deepIb III)
			disk2 {base}_Im  \
			    /pyr/pyramidal[]/soma/{base}_dend Im
	    	end
	end



	/*
** FIELD POTENTIALS
*/
	if ({FIELD} && {MULTI})
		disk2 efield_Ia_Na /field/efield_Ia_Na[] field
		disk2 efield_Ia_K /field/efield_Ia_K[] field
		disk2 efield_supIb /field/efield_supIb[] field
		disk2 efield_deepIb /field/efield_deepIb[] field
		disk2 efield_soma /field/efield_soma[] field
		disk2 efield_III /field/efield_III[] field
		disk2 efield_total /field/efield_total[] field
	end

end

set_out


