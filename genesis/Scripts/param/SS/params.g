// genesis

//
// params.g: setting up the parameter table.
//

int NPARAMS = 5  // number of parameters

// This is the main parameter search object:
create paramtableSS /par

// Set parameters for variance contraction.
// The contraction rate selected ensures that one contraction
// cycle will take exactly 100 iterations.
setfield /par contract 0.9616 maxvariance 0.5 minvariance 0.01
call /par TABCREATE {NPARAMS}


// Set the starting values and ranges of all the parameters.
// usage: initparamSS path param type range center label

initparamSS /par 0  1   4.0    1.0  "Na Gbar scaling"
initparamSS /par 1  1   4.0    1.0  "Kdr Gbar scaling"
initparamSS /par 2  1   4.0    1.0  "KM Gbar scaling"
initparamSS /par 3  1   4.0    1.0  "KM tau scaling"
initparamSS /par 4  0   0.010  0.0  "KM minf offset"


// The following function has to be re-written for each different
// parameter search.  I like to keep all parameters for a given
// channel adjacent to one another.  I also like to keep all channels
// corresponding to a given ion next to one another.  This is only
// really important for the genetic algorithm method, where you
// usually don't want crossovers to split up parameters corresponding
// to the same channel.

function adjust_parameters
    // Adjust Gbars of ion channels:
    adjust_Gbar /par  0     /cell/soma/Na_hip_traub91
    adjust_Gbar /par  1     /cell/soma/Kdr_hip_traub91
    adjust_Gbar /par  2     /cell/soma/KM_bsg_yka

    // Adjust kinetics of KM channel:
    adjust_tau  /par  3  X  /cell/soma/KM_bsg_yka
    adjust_minf /par  4  X  /cell/soma/KM_bsg_yka
end


