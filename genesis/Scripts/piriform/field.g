//genesis

function field_electrode(channel_current, path, target, layer, x, y, z,  \
    r, rz)
str path
str target
float x, y, z, r, rz
int channel_currrent
    create efield {path}
    setfield ^ scale -3.33e4 x {x} y {y} z {z}
    if (channel_current == 1)
	    addmsg  \
	        {target}/{layer}[x>={x - r}][y>={y - r}][x<={x + r}][y<={y + r}] \
	         {path} CURRENT Ik 0.0
    else
	    addmsg  \
	        {target}/#[TYPE=compartment][x>={x - r}][y>={y - r}][x<={x + r}][y<={y + r}] \
	         {path} CURRENT Im 0.0
    end
    call ^ RECALC
end

function move_electrode(path, x, y, z)
str path
float x, y, z
    setfield {path} x {x} y {y} z {z}
    call {path} RECALC
end

function electrode_array(path, xmin, ymin, xmax, ymax, xspacing,  \
    yspacing, z)
str path
float xmin, ymin
float xmax, ymax
float xspacing, yspacing
float z
float x, y
int count = 0

    create neutral /field
    for (y = ymin; y < ymax; y = y + yspacing)
	for (x = xmin; x < xmax; x = x + xspacing)
	    field_electrode 1 /field/efield_soma[{count}] {path}  \
	        soma/Cl_channel {x} {y} {z} {{abs z} + 0.5} 1
	    field_electrode 1 /field/efield_Ia_Na[{count}] {path}  \
	        soma/Ia_dend/Aff_Na_channel {x} {y} {z} {{abs z} + 0.5}  \
	        1
	    field_electrode 1 /field/efield_Ia_K[{count}] {path}  \
	        soma/Ia_dend/K_channel {x} {y} {z} {{abs z} + 0.5} 1
	    field_electrode 1 /field/efield_supIb[{count}] {path}  \
	        soma/supIb_dend/CA_Na_channel {x} {y} {z}  \
	        {{abs z} + 0.5} 1
            field_electrode 1 /field/efield_deepIb[{count}] {path}  \
                soma/deepIb_dend/RA_Na_channel {x} {y} {z}  \
                {{abs z} + 0.5} 1
            field_electrode 1 /field/efield_III[{count}] {path}  \
                soma/III_dend/Local_Na_channel {x} {y} {z}  \
                {{abs z} + 0.5} 1
            field_electrode 0 /field/efield_total[{count}] {path} soma  \
                {x} {y} {z} {{abs z} + 0.5} 1
	    echo . -nonewline
	    count = count + 1
	end
    end
    echo
    return (count)
end

if ({FIELD} && {MULTI})
    // construct a 10x6mm electrode array
    echo setting up {CORTEX_X} x {CORTEX_Y}mm field electrode array
    float dxtrodes = {round {1.0/{PYR_DX}}}*{PYR_DX}
    int nxtrodes = {trunc {{CORTEX_X}/dxtrodes}}
    float dytrodes = {round {1.0/{PYR_DY}}}*{PYR_DY}
    int nytrodes = {trunc {{CORTEX_Y}/dytrodes}}
    echo dxtrodes: {dxtrodes} dytrodes: {dytrodes}
    echo {nxtrodes} x {nytrodes} electrode array
    echo  \
        {electrode_array /pyr/pyramidal[] {{PYR_DX}/2.0} {{PYR_DY}/2.0} {CORTEX_X} {CORTEX_Y} {dxtrodes} {dytrodes} 0} \
         electrodes
end

