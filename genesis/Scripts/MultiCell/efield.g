//genesis

/*
** repositions an existing extracellular electrode
*/
function position_electrode(path, x, y, z)
    position {path} {x} {y} {z}
    call {path} RECALC
end

/*
** creates an extracellular recording electrode named 'path' with
** coordinates x,y,z
*/
function extracellular_electrode(path, x, y, z)
str path
float x, y, z
str s
    create efield {path}
    setfield {path} scale 1
    foreach s ({el /##[CLASS=channel]})
	addmsg {s} {path} CURRENT Ik 0.0
    end
    position_electrode {path} {x} {y} {z}

    if ({GRAPHICS})
	create x1form {path}/graph1
	create x1graph {path}/graph1/graph
	addmsg {path} {path}/graph1/graph PLOT field *field *black
    end
end

