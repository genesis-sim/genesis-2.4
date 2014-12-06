// genesis

//
// siminit.g: basic definitions for simulation.
//

randseed
floatformat %g

int restore       = 0  // flag for whether to restore old parameter values

//
// Check to see if the simulation is in progress.
// The "sim_status" file consists of a single integer, which is
// the count of simulations done so far.  Once it is >= 10000,
// the search stops.
//

int status

openfile sim_status r
status = {readfile sim_status}
closefile sim_status

if (status >= 10000)
    echo
    echo Search is finished!
    quit
end

// simulation timings:

setclock    0    0.00002        // sec
setclock    1    0.0001        // output interval

float delay             = 0.050  // sec
float current_duration  = 0.500
float total_duration    = 0.600

// file names:

str cellfile          = "cell.p"
str origcellfile      = "origcell.p"
str sim_output_file   = "Vm"
str sim_spk_file      = {sim_output_file} @ ".spk.sim"
str bestfile          = {sim_output_file} @ ".best"
str sim_spk_file_best = {sim_spk_file} @ ".best"
str matchfile         = "match.values"
str restore_file      = "param.save"

// Where the best values of the parameters are stored:
str best_param_file   = "best.params"

// This is what we want our parameter search to match.  It consists of
// spike times from the "real" cell.  In fact, other forms of data,
// such as membrane potential traces, can also be used, but they
// require changes to the matching function.

str real_spk_file      = "Vm.spk.data"

