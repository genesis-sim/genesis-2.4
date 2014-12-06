// genesis

//
// search.g: functions for stochastic-search parameter searches
//

// This function prints the current parameter values.
// It's useful for debugging.

function print_params
    int i

    echo
    for (i = 0; i < NPARAMS; i = i + 1)
        echo Param {i} value: {getfield /par current[{i}]}
    end
    echo
end


// This function regenerates the original Vm output file and
// spike time file.

function make_orig
    silent 2
    if (!{exists /currtable})
        make_fItable
    end

    newsim {origcellfile} 0
    runfI
    gen2spk {sim_output_file} {delay} {current_duration} \
            {total_duration}
    mv Vm Vm.data
    mv Vm.spk.sim Vm.spk.data
    silent 0
end


// This function runs one iteration of the parameter search
// and calculates a match value.

function one_run(adjust_flag)
    int adjust_flag   // if 1, then adjust parameters

    float match

    silent 2
    // Create a new cell and set its parameters.
    newsim {cellfile} {adjust_flag}

    // Simulate the cell with several different input currents.
    runfI

    // gen2spk converts the membrane potentials in the output file
    // to a list of spike times.

    gen2spk {sim_output_file} {delay} {current_duration} \
            {total_duration}

    // spkcmp compares the list of spike times to the desired spike
    // times and calculates a numerical error value.  An error of
    // 0 would be a perfect match.

    match = {spkcmp {real_spk_file} {sim_spk_file} \
        -pow1 0.1 -pow2 0.6 -msp 0.5 -nmp 200.0}

    silent 0
    return {match}
end


// This function actually runs the parameter search:

function do_search
    float accept = 1e-6     // criteria for acceptance of match
    int   done   = 0
    int   iteration, round, rounds_this_run, last_round_this_run
    float best, match, var

    silent 2
    make_fItable
    rounds_this_run = 10

    if (status == 0)  // New search.
        openfile  {matchfile} w
        writefile {matchfile} "% iteration match"
        closefile {matchfile}
        call /par INITSEARCH
        call /par RANDOMIZE
        match = {one_run 1}

        best = match
        call /par ACCEPT {best}

        echo iteration: {iteration}  match = {match}   best = {best}
        echo
        call /par DISPLAY
        echo

        call /par SAVEBEST {best_param_file}
        call /par UPDATE_PARAMS

        sh cp {sim_output_file} {bestfile}
        sh cp {sim_spk_file} {sim_spk_file_best}
    else
        call /par RESTORE {restore_file}
        best = {getfield /par best_match}
    end

    iteration           = {getfield /par iteration_number}
    round               = {getfield /par round_number}
    last_round_this_run = round + rounds_this_run - 1
    var                 = {getfield /par variance}

    while (round <= last_round_this_run)
        match = {one_run 1}

        echo Simulation: {iteration}  match = {match}   best = {best}

        if (match < best)
            best = match
            call /par ACCEPT {best}

            echo
            echo >>>>>>>>>>>>> Accepting new parameters...
            echo

            call /par DISPLAY

            echo

            call /par SAVEBEST {best_param_file}
            cp {sim_output_file} {bestfile}
            cp {sim_spk_file} {sim_spk_file_best}
        end

        call /par UPDATE_PARAMS
        iteration = {getfield /par iteration_number}
        var       = {getfield /par variance}
        round     = {getfield /par round_number}

        if (iteration % 100 == 0)
            openfile  {matchfile} a
            writefile {matchfile} {getfield /par iteration_number} {best}
            closefile {matchfile}
        end
    end

    echo Simulation: {iteration}  match = {match}   best = {best}

    sh rm {sim_output_file}
    sh rm {sim_spk_file}

    echo
    echo Saving paramtable object...
    echo

    call /par SAVE {restore_file}
    openfile sim_status w
    writefile sim_status {getfield /par iteration_number}
    closefile sim_status
end





