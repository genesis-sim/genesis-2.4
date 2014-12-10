// genesis

// pga.g: a parallel genetic algorithm demonstration script

// echo all parameters
echo Genesis started at {getdate}
echo "n_nodes = " {n_nodes}
echo "# of workers = " {n_nodes - 1}
echo "simulation time per individual = " {simulation_time}
echo "individuals = " {individuals}
echo "population = " {population}
echo ""

// variables for the parameter search
float param_a			// parameters
float param_b
float param_c
float param_d
float bit_mutation_prob	= 0.02  // probability of mutating a bit when
				// generating a new individual

int actual_population = 0	// number of individuals that are "alive"
int least_fit			// index of least-fit individual in current
				//   population
float min_fitness		// fitness of the least-fit individual
int most_fit			// index of the most-fit individual in current
				//   population
float max_fitness		// fitness of the most-fit individual

int print_count = 0		// counts how many individuals' fitness values
				//    we have printed out

// subordinate files
include farm.g		   // support for task farming

// search functions

// evaluate is dummy function used to demonstrate the parameter search method.
// This would typically be replaced in a real neural-modeling parameter
// search with a simulation of a neural model for a number of timesteps,
// followed by an evaluation of the results according to some match
// criteria.  The function returns a fitness value indicating the quality of
// the match, with higher values signifying better matches.
function evaluate (a, b, c, d)
    float a, b, c, d
    float match, fit
    int i

    if (simulation_time != 0)
	    // pretend we are really doing a simulation
	    for (i = 0; i < 800000; i = i + 1)
	    end
    end

    // this just assigns a fitness value based on how
    //   closely the parameters match the set (3, 5, 7, 11)
    match = ({a} - 3.0) * ({a} - 3.0) + ({b} - 5.0) * ({b} - 5.0) + ({c} - 7.0) * ({c} - 7.0) + ({d} - 11.0) * ({d} - 11.0)
    if (match != 0.0)
      fit = 1.0/{sqrt {match}} 
    else
	  fit = 1e+9
    end
    return {fit}
end

// recompute_fitness_extremes recomputes which individuals in the population
// are the most and least fit
function recompute_fitness_extremes
	int i

	min_fitness = 1e+10
	max_fitness = -1e+10
	for (i = 0; i < actual_population; i = i + 1)
	    if ({getfield /population[{i}] fitness} < min_fitness)
		least_fit = i
		min_fitness = {getfield /population[{i}] fitness}
	    end
	    if ({getfield /population[{i}] fitness} > max_fitness)
		most_fit = i
		max_fitness = {getfield /population[{i}] fitness}
	    end
	end
end

function gray_decode (x)
    int x
    int y
    y = x
    y = y ^ (y / 2)
    y = y ^ (y / 4)
    y = y ^ (y / 16)
    y = y ^ (y / 256)
    y = y ^ (y / 65536)
    return {y}
end

// worker_task performs the task that the master has doled out to this node
function worker_task (index, bs_a, bs_b, bs_c, bs_d)
	int index
	int bs_a, bs_b, bs_c, bs_d;
	float a, b, c, d;
	float fit

	// compute the real-valued parameters from the bit-strings (the
	//    "genes")
	a = ({gray_decode {bs_a}} - 32768.0) / 1000.0;
	b = ({gray_decode {bs_b}} - 32768.0) / 1000.0;
	c = ({gray_decode {bs_c}} - 32768.0) / 1000.0;
	d = ({gray_decode {bs_d}} - 32768.0) / 1000.0;

	// determine that fitness value for this individual
    fit = {evaluate {a} {b} {c} {d}}

	// return result to the master (node 0 in zone 0)
	return_result@0.0 {mytotalnode} {index} {bs_a} {bs_b} {bs_c} {bs_d} {fit}
end


function return_result (node, index, bs_a, bs_b, bs_c, bs_d, fit)
	int node, index, bs_a, bs_b, bs_c, bs_d
	float fit
	int added = 0

	if (actual_population < population)
	    // if we are underpopulated, each new individual stays "alive"
	    least_fit = actual_population
	    min_fitness = -1e+10
	    actual_population = actual_population + 1
	end
	if (fit > min_fitness)
	    // replace the least fit individual with the current individual
	    setfield /population[{least_fit}] fitness {fit}
	    setfield /population[{least_fit}] a_value {bs_a}
	    setfield /population[{least_fit}] b_value {bs_b}
	    setfield /population[{least_fit}] c_value {bs_c}
	    setfield /population[{least_fit}] d_value {bs_d}
	    recompute_fitness_extremes
	    added = 1
	end

	// that node is now free, and can be assigned a new task
	setfield /free[{node}] value 1

	// print out the fitness value of the individual we just evaluated
        floatformat %12.2f
        echo {fit} -n
	if (added)
	    echo "*" -n
	else
	    echo " " -n
	end
        floatformat %g
        if({print_count} % 6 == 5)  // 6 fitness values per column
          echo
        end
	print_count = {print_count} + 1
end


// mutate alters the given bit-string based on the mutation probability,
//	and returns the possibly-mutated string
function mutate (v)
    int v
    int i, b

    b = 1
    for (i = 0; i < 16; i = i + 1)
	if ({rand 0 1} < bit_mutation_prob)
	    v = v ^ b
	end
	b = b + b
    end
    return {v}
end

// initializes the search
function init_search
	create neutral /gs
	disable /gs
	create neutral /gs/population
	addfield /gs/population a_value
	addfield /gs/population b_value
	addfield /gs/population c_value
	addfield /gs/population d_value
	addfield /gs/population fitness
	createmap /gs/population / {population} 1
	actual_population = 0
end

// this is the function that actually runs the search
function search
    int   i, chosen
    int	  bs_a, bs_b, bs_c, bs_d
    float a, b, c, d

    init_search
    init_farm

    echo Starting search at {getdate}
    echo
    echo "   Fitness of individuals as they are evaluated:"
    echo "       (* indicates that this individual was added to the population)"
    echo

    for (i = 0; i < individuals; i = i + 1)
	// pick random individual from population
	chosen = {rand 0 population}
	if (chosen >= actual_population)
	    // pick a random bit string
	    bs_a = {rand 0 65536}
	    bs_b = {rand 0 65536}
	    bs_c = {rand 0 65536}
	    bs_d = {rand 0 65536}
	else
	    // mutate an existing bit string
	    bs_a = {mutate {getfield /population[{chosen}] a_value}}
	    bs_b = {mutate {getfield /population[{chosen}] b_value}}
	    bs_c = {mutate {getfield /population[{chosen}] c_value}}
	    bs_d = {mutate {getfield /population[{chosen}] d_value}}
	end
	// assign this task to a worker
	delegate_task {i} {bs_a} {bs_b} {bs_c} {bs_d}
    end 

    finish
    echo
    echo "Finished search at " {getdate}

    bs_a = {getfield /population[{most_fit}] a_value}
    bs_b = {getfield /population[{most_fit}] b_value}
    bs_c = {getfield /population[{most_fit}] c_value}
    bs_d = {getfield /population[{most_fit}] d_value}
    a = ({gray_decode {bs_a}} - 32768.0) / 1000.0;
    b = ({gray_decode {bs_b}} - 32768.0) / 1000.0;
    c = ({gray_decode {bs_c}} - 32768.0) / 1000.0;
    d = ({gray_decode {bs_d}} - 32768.0) / 1000.0;
    echo "Best match was with a = " {a} ", b = " {b} ", c = " {c} ", d = " {d}
    echo "which had a fitness of " {max_fitness}
end

// every node will run these script commands
paron -farm -silent 0 -nodes {n_nodes} -output o.out \
	-executable nxpgenesis
echo I am node {mytotalnode}

// first wait for every node to start up
barrierall
echo Completed startup at {getdate}

if ({mytotalnode} == 0)
	search {individuals} {population} // the master (on node 0) runs
					  //   the parameter search
end
barrierall 7 1000000	// workers will sit here waiting for task
			// assignments from the master
quit

// $Log: pga.g,v $
// Revision 1.2  2006/02/03 17:28:27  ghood
// Changed to Gray-encoded parameter values with a more realistic evaluation function
//
// Revision 1.1.1.1  2005/06/14 04:38:39  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.1  1999/12/18 03:02:08  mhucka
// Added PGENESIS from GENESIS 2.1 CDROM distribution.
//
// Revision 1.4  1997/01/21 22:19:31  ngoddard
// paragon port
//
// Revision 1.3  1996/09/20 20:03:25  ghood
// Fixed altsimrc misspecification.
//
// Revision 1.2  1996/09/20 18:58:54  ghood
// Cleaned up for PSC release
//
// Revision 1.1  1996/08/16 19:04:25  ghood
// Initial revision
//
