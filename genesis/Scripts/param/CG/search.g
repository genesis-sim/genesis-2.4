// genesis

//
// search.g: functions for conjugate-gradient parameter searches
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
    int   i, j, iteration, done

    int   preiterations        = 100
    int   iterations_per_round = 100

    // max_iterations = the maximum number of iterations before saving and
    // restarting, *not* necessarily the maximum number of iterations in
    // the parameter search as a whole.

    int   max_iterations = 1000
    int   sim_num        = 0     // Simulations this round.

    float match, best

    make_fItable

    if (!restore)
        // Initialize match file.  This holds a list of match
        // values obtained at various points in the search.
        openfile  {matchfile} w
        writefile {matchfile} "% iteration best_match"
        closefile {matchfile}
    end


    if (restore)
        echo
        echo Restoring parameter search object...
        echo

        call /par RESTORE {restore_file}
        best = {getfield /par best_match}
    else
        call /par INITSEARCH

        // ==========================================================
        // Pick a preset number of random points and start the
        // search at the best of these.
        // ==========================================================

        echo
        echo Simulating {preiterations} random points...
        echo

        for (i = 0; i < {preiterations}; i = i + 1)
            iteration = {getfield /par iteration_number}
            call /par RANDOMIZE // pick a random starting point in range
            match = {one_run 1}

            echo iteration {iteration}: match = {match}
            echo

            // EVALUATE prints a message when it finds a new best
            // match if silent < 2.  This message is unnecessary
            // here since we're monitoring this ourselves.

            silent 2
            call /par EVALUATE {match}
            silent 0

            if ({getfield /par new_best_match} == 1)
                best = match

                echo
                echo >>>>>>>>>>>>> Accepting new parameters...
                echo
                call /par DISPLAY
                echo

                call /par SAVEBEST {best_param_file}

                sh cp {sim_output_file} {bestfile}
                sh cp {sim_spk_file} {sim_spk_file_best}
            end

            // The UPDATE_PARAMS2 action just increments the iteration
            // number.  The UPDATE_PARAMS action is only used when doing
            // a true CG search, not a random search like we're doing now.

            call /par UPDATE_PARAMS2
            sim_num = sim_num + 1

            // Save best match to matchfile every so often.

            if ((sim_num % iterations_per_round) == 0)
                openfile  {matchfile} a
                writefile {matchfile} {getfield /par iteration_number} {best}
                closefile {matchfile}
            end
        end

        call /par LOADBEST  // Copies best parameter values to current values.
    end

    if (!restore)
        echo
        echo Starting conjugate gradient search...
        echo
    else
        echo
        echo Continuing conjugate gradient search...
        echo
    end

    // ============================================================
    // Now we do the real conjugate gradient search.  Use the best
    // parameter values found as the starting point for the search.
    // If restoring a previously-saved simulation just keep on
    // going.
    // ============================================================

    while (!done && (sim_num < max_iterations))
        iteration = {getfield /par iteration_number}

        match = {one_run 1}

        echo iteration {iteration}: match = {match} best match = {best}
        // Note that state 1 = gradient and state 2 = line minimization.
        // Uncomment the next line if you're interested in such things.
        //echo state = {getfield /par state}
        echo

        call /par EVALUATE {match}

        if ({getfield /par new_best_match} == 1)
            best = match

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
        sim_num = sim_num + 1

        done  = {getfield /par done}

        // Save best match to matchfile every so often.

        if ((sim_num % iterations_per_round) == 0)
            openfile  {matchfile} a
            writefile {matchfile} {getfield /par iteration_number} {best}
            closefile {matchfile}
        end
    end

    if (done)
        echo
        echo DONE! Best match is: {getfield /par best_match}
        echo

        // Save results to match file; this is important
        // since not all simulations last long enough
        // to have the match values recorded above.

        openfile  {matchfile} a
        writefile {matchfile} {getfield /par iteration_number} {best}
        closefile {matchfile}

        openfile sim_status w
        writefile sim_status 2
        closefile sim_status

        sh rm {sim_output_file}
        sh rm {sim_spk_file}
    else
        echo
        echo Saving paramtable object...
        echo

        call /par SAVE {restore_file}
        openfile sim_status w
        writefile sim_status 1
        closefile sim_status
    end
end



