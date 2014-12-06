// genesis

//
// params.g: setting up the parameter table.
//

int NPARAMS = 5     // number of parameters
int NTABS   = 100   // number of parameter tables

// This is the main parameter search object:
create paramtableGA /par

call /par TABCREATE {NTABS} {NPARAMS}

setfield /par preserve              1     // preserve the best table

// Reseed the parameter tables after 25 unproductive generations,
// preserving the best table.

setfield /par do_restart            1
setfield /par restart_after         25

// A crossover_probability of x means that x proportion of the original
// tables will be crossed over (0 <= x <= 1).

setfield /par crossover_probability 0.7
setfield /par mutation_probability  0.02  // per bit

// crossover_type = 1 means have an *average* of N crossover per pair
// of parameter sets that are chosen for crossovers, where N is the
// crossover_number value.  crossover_type = 0 means have *exactly* N
// crossovers per selected pair.  I prefer the former method because then
// you get a mix of single, double etc. crossovers which increases the
// possibilities for useful variants.

setfield /par crossover_type        1
setfield /par crossover_number      1


// Set the starting values and ranges of all the parameters.
// usage: initparamGA path param type range center label

initparamGA /par 0  1   4.0    1.0  "Na Gbar scaling"
initparamGA /par 1  1   4.0    1.0  "Kdr Gbar scaling"
initparamGA /par 2  1   4.0    1.0  "KM Gbar scaling"
initparamGA /par 3  1   4.0    1.0  "KM tau scaling"
initparamGA /par 4  0   0.010  0.0  "KM minf offset"


// The following function has to be re-written for each different
// parameter search.  I like to keep all parameters for a given
// channel adjacent to one another.  I also like to keep all channels
// corresponding to a given ion next to one another.  This is only
// really important for the genetic algorithm method, where you
// usually don't want crossovers to split up parameters corresponding
// to the same channel.

function adjust_parameters(table)
    int table         // index of table to use for adjusting parameters

    // Adjust Gbars of ion channels:
    adjust_Gbar /par  {table} 0     /cell/soma/Na_hip_traub91
    adjust_Gbar /par  {table} 1     /cell/soma/Kdr_hip_traub91
    adjust_Gbar /par  {table} 2     /cell/soma/KM_bsg_yka

    // Adjust kinetics of KM channel:
    adjust_tau  /par  {table} 3  X  /cell/soma/KM_bsg_yka
    adjust_minf /par  {table} 4  X  /cell/soma/KM_bsg_yka
end


