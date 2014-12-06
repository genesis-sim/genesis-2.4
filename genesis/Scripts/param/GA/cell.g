// genesis

//
// cell.g: sets up a simulation which is the basis for the parameter search.
//

// Note that a new cell is defined for each iteration of the parameter
// search.  This is necessary because the tabchannel tables for the KM
// channel are manipulated in destructive ways by the parameter search,
// so it is safest to recreate the cell from scratch each time.  It also
// makes the bookkeeping somewhat simpler, and doesn't consume much
// simulation time.  If the cell model was very large and took a long
// time to load up, this approach might not be feasible.

function newsim(cell, adjust_flag, table)
    str cell
    int adjust_flag   // if 1, then adjust parameters.
    int table         // index of table to use for adjusting parameters

    str ascfile, file

    // Delete objects from previous simulation if any.

    if ({exists /cell})
        delete /Iin
        delete /cell
        delete /out/{sim_output_file}
        delete /out
        reclaim
    end

    // Define new channel library and load in the cell.

    make_channel_library
    readcell {cell} /cell

    // Provide current injection to cell 1.

    create pulsegen /Iin
    setfield ^  \
        delay1 {delay}             \      // sec
        level1 0.0                 \
        width1 {current_duration}  \
        delay2 9999.0
    addmsg /Iin /cell/soma INJECT output

    // Save output of simulations to disk.

    clearfile {sim_output_file}

    ce /
    create neutral /out
    create asc_file /out/{sim_output_file}
    setfield ^ leave_open 1 append 1 flush 0 filename {sim_output_file}
    addmsg /cell/soma /out/{sim_output_file} SAVE Vm
    useclock /out/{sim_output_file} 1

    if (adjust_flag)
        adjust_parameters {table}
    end
end
