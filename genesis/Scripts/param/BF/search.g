// genesis

//
// search.g: functions for brute-force parameter searches
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


// This function records the current parameter values and the
// current match value to a file.

function record_params
    float match  = {getfield /par current_match}
    str filename = "parameter_space.out"
    float param0, param1, param2, param3, param4

    param0 = {getfield /par current[0]}
    param1 = {getfield /par current[1]}
    param2 = {getfield /par current[2]}
    param3 = {getfield /par current[3]}
    param4 = {getfield /par current[4]}

    openfile {filename} a
    writefile {filename} {match} " " -n
    writefile {filename} {param0} {param1} {param2} {param3} {param4}
    closefile {filename}
end



// This function actually runs the parameter search:

function do_search
    int   i, j, iteration, done
    float ndivs

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

        // Also initialize the parameter space output file.
        openfile  parameter_space.out w
        writefile parameter_space.out "% match param1 ..."
        closefile parameter_space.out
    end


    if (restore)
        echo
        echo Restoring parameter search object...
        echo

        call /par RESTORE {restore_file}
        best = {getfield /par best_match}
    else
        // The INITSEARCH action for the paramtableBF object
        // is different from that of the other paramtable object.
        // It expects its arguments to be in the form of
        // <param_number> <npts> <scale> triplets, where
        //   <param_number> is the index of the parameter to be varied
        //   <npts> is the number of equally-spaced points along the
        //     parameter axis to test
        //   <scale> is the proportion of the total range to use;
        //     1.0 means use the entire range
        //
        // Note that the range is relative to a center point which is
        // where the parameter search starts from.
        // We use 5 divisions for the first four parameters, which are
        // multiplicative, and 6 divisions for the fifth parameter, which
        // is additive.  This turns out to make the parameter values
        // come out on nice even numbers, but has no other significance.

        call /par INITSEARCH \
            0 5 1.0 \
            1 5 1.0 \
            2 5 1.0 \
            3 5 1.0 \
            4 6 1.0

        // The INITSEARCH action also initializes best_match to a huge
        // starting value.

        best = {getfield /par best_match}
    end

    if (!restore)
        echo
        echo Starting brute force search...
        echo
    else
        echo
        echo Continuing brute force search...
        echo
    end

    while (!done && (sim_num < max_iterations))
        iteration = {getfield /par iteration_number}

        match = {one_run 1}

        echo iteration {iteration}: match = {match} best match = {best}
        echo

        // EVALUATE prints a message when it finds a new best
        // match if silent < 2.  This message is unnecessary
        // here since we're monitoring this ourselves.

        silent 2
        call /par EVALUATE {match}
        silent 0

        record_params

        if (match < best)
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



