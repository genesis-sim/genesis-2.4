// genesis

//
// paramfuncs.g: helper functions for parameter searches.
//

// This function adjusts the maximal conductance of a channel:

function adjust_Gbar(table, i, path)
    str   table
    int   i
    str   path

    float old_Gbar = {getfield {path} Gbar}
    float new_Gbar = old_Gbar * {getfield {table} current[{i}]}
    setfield {path} Gbar {new_Gbar}
end


// This function adjusts the midpoint of an activation curve:

function adjust_minf(table, i, gate, path)
    str   table
    int   i
    str   gate
    str   path

    float offset = {getfield {table} current[{i}]}

    if ({strcmp {gate} "X"} == 0)
        scaletabchan {path} X minf 1.0 1.0 {offset} 0.0
    elif ({strcmp {gate} "Y"} == 0)
        scaletabchan {path} Y minf 1.0 1.0 {offset} 0.0
    elif ({strcmp {gate} "Z"} == 0)
        scaletabchan {path} Z minf 1.0 1.0 {offset} 0.0
    end
end


// This function scales the tau(V) values of a channel
// activation curve by some amount.

function adjust_tau(table, i, gate, path)
    str   table
    int   i
    str   gate
    str   path

    float scale = {getfield {table} current[{i}]}

    if ({strcmp {gate} "X"} == 0)
         scaletabchan {path} X tau 1.0 {scale} 0.0 0.0
    elif ({strcmp {gate} "Y"} == 0)
        scaletabchan {path} Y tau 1.0 {scale} 0.0 0.0
    elif ({strcmp {gate} "Z"} == 0)
        scaletabchan {path} Z tau 1.0 {scale} 0.0 0.0
    end
end


