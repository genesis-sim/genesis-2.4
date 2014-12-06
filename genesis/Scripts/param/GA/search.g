// genesis

//
// search.g: functions for genetic algorithm-based parameter searches
//

// Print out a single parameter table; mainly for debugging.

function print_parameters(index)
    int index, i

    for (i = 0; i < NPARAMS; i = i + 1)
        echo table {index}, parameter {i} = {getparamGA /par {index} {i}}
    end
end


function init_fitness_file(file)
  str path, file
  openfile  {file} w
  writefile {file} "% generation best_fitness"
  closefile {file}
end


function output_fitness(path, file, gen)
  str path, file
  int gen

  openfile {file} a
  writefile {file} {gen} {getfield {path} max_fitness}
  closefile {file}
end


// This function regenerates the original Vm output file and
// spike time file.

function make_orig
    silent 2
    if (!{exists /currtable})
        make_fItable
    end

    newsim {origcellfile} 0 0
    runfI
    gen2spk {sim_output_file} {delay} {current_duration} \
            {total_duration}
    mv Vm Vm.data
    mv Vm.spk.sim Vm.spk.data
    silent 0
end


// This function runs one iteration of the parameter search
// and calculates a match value.

function one_run(adjust_flag, table)
    int adjust_flag   // if 1, then adjust parameters
    int table         // index of table to use for adjusting parameters

    float match, fitness

    silent 2
    // Create a new cell and set its parameters.
    newsim {cellfile} {adjust_flag} {table}

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

    // The fitness is the inverse of the square root of the match
    // value.  It's the inverse because low match values mean
    // high fitness.  The sqrt is to reduce the selection pressure
    // somewhat, but it isn't essential.

    fitness   = 1.0 / {sqrt {match}}
    silent 0
    return {fitness}
end


// This function simulates all the parameter sets in one generation
// of the genetic algorithm object.

function generation
    int   gen, i, bestloc
    float fitness, best, maxfitness
    str   param_file_for_round

    gen    = {getfield /par generation} + 1

    if (restore)
        maxfitness = {getfield /par best_match}
    else
        maxfitness = -9999  // An absurdly low value.
    end

    echo
    echo "Generation: " {gen}
    echo

    for (i = 0; i < NTABS; i = i + 1)
        fitness = {one_run 1 {i}}

        if (fitness > maxfitness)
            maxfitness = fitness
            sh mv {sim_output_file} {bestfile}
        end

        floatformat %7.2f
        echo {fitness} -n
        floatformat %g

        if ({i} % 10 == 9)  // 10 fitness values per column
            echo
        end

        setfield /par fitness[{i}] {fitness}
    end

    call /par FITSTATS

    best    = {getfield /par max_fitness}
    bestloc = {getfield /par max_fitness_index}
    output_fitness /par {matchfile} {gen}

    echo
    echo
    echo **********************************************
    floatformat %7.2f
    echo "***   Best fitness: " {best} " in table " -n
    floatformat %4f
    echo {bestloc} " *** "
    echo **********************************************
    echo
    floatformat %g

    call /par UPDATE_PARAMS
    call /par SAVE {restore_file}
    call /par SAVEBEST {best_param_file}
end



// The following function does a full parameter search.

function do_search(num_rounds)
    int num_rounds, i

    silent 2
    make_fItable

    if (restore)
        call /par RESTORE {restore_file}
    else
        init_fitness_file {matchfile}
        call /par INITSEARCH random
    end

    for (i = 0; i < num_rounds; i = i + 1)
        generation
    end

    // Store the number of generations simulated in the
    // sim_status file.

    generations_so_far = generations_so_far + num_rounds
    openfile sim_status w
    writefile sim_status {generations_so_far}
    closefile sim_status

    silent 0
end

