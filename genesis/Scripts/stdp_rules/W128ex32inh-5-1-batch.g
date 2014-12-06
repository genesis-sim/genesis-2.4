//genesis  - GENESIS Version 2.4beta

/*======================================================================
An implementation of the Song, Miller, and Abbot (2000) model with
the postsynaptic cell being the VA_HHcell model that was used in
the Brette et al. (2006) review of network simulators.
======================================================================*/

str script_name = "W128ex32inh-5-1-batch.g" 
str RUNID = "5100BA"        // default ID string for output file names

// minimal graphics

/* Some Booleans indicating the type of calculations or output */
int batch = 1        // if (batch) then run a default simulation
int graphics = 0     // if = 1, display control panel minimal graphs
                     // if = 2, show more detailed output (much slower)
int file_out = 0     // write output to a file
int weights_out = 1  // output weights to a file
int hflag = 1      // GENESIS 2 - use hsolve if hflag = 1
int hsolve_chanmode = 4    // hsolve mode for GENESIS 2
int use_sprng = 1 // Use SPRNG random number generator, rather than default RNG
int rand_delay = 0  // Assign delays from 0 - 2*delay
int rand_weight = 1  // Assign weights  min_weight*weight - max_weight*weight
int normalize_weights = 0 // perform synaptic scaling to average weight
int debug = 0     //Provide debugging output -  0 = no messages,
                  // 1 = some messages, 2 = more messages  > 2, even more
int change_weights = 1 // Allow weight changes. if = 0, stdp_update will be
                       // called, but return.  If < 0, stdp_applier not created.
int use_update_object = 1   // Create stdp_rules element to do update
int use_update_function = 0 // Use SLI stdp_update function with script_out
                            // These are mutually exclusive
int Ninputs = 128  // number of presynaptic Excitatory connections
int ninh_syns = 32 // number of presynaptic Inhibitory connections
int Ncells = 1   // Number of cells to make

// Set randdom number seed. If seed is 0, set randseed from clock,
// else from given seed
int seed = 0  // Simulation will give different random numbers each time
int seed = 1369497795

if (use_sprng)
    setrand -sprng
end
if (seed)
    randseed {seed}
else
    seed = {randseed}
end

// Parameters used later for STDP update
float Aplus, Aminus, dAplus, dAminus, minus_plus_ratio

float gmax = 0.23e-9
//float gmax = 6.0e-9 // maximum Ex synaptic conductance (Siemens)
//float Inh_gmax   = 67.0e-9 // max Inh conductance
float Inh_gmax   = 0.2e-9 // max Inh conductance
float injcurrent = 0.0
// injcurrent = 0.3e-9 // Typical for simplecell
float avg_weight = 0.5 // Desired average synaptic weight per target synchan
float delay  = 0.00 // no  delay
// float delay = 0.003 // assume a 3 msec conduction delay
float frequency = 10 // freq of random activation of cell[0]

str cell_file = "VA_HHcell.p" // VAcell parameter file
str cell_name = "/cell"
str Ex_synpath = "soma/Ex_channel" // location of AMPA
str Inh_synpath = "soma/Inh_channel" // location of GABA synchan
str spikepath = "soma/spike"  // location of the cell spikegen

str precell_name = "precell"

// Root name for the output files - with "_RUNID.txt" to be added
str Vm_file = "W1280ex32inh_Vm"
str syn_file = "W128ex32inh_Gk_I"
str weights_file = "W128ex32inh_weights"
str sum_file = "run_summary"

float tmax = 400.0                // simulation time in sec

/***** Setting of clocks *****/

float dt = 0.00002              // simulation time step in sec
setclock  0  {dt}               // set the simulation clock
float out_dt = 0.001           // time step for Vm, Gk/I plots and file I/O
setclock 1 {out_dt}
float stdp_dt = 0.001           // time step for weight update

setclock 2 {stdp_dt}
float weights_out_dt = 0.10      // time step to output and plot the weights
setclock 3 {weights_out_dt}
float wtscale_dt = 0.010        // time step for weight normalization
setclock 4 {wtscale_dt}
float min_dt = 0.002 // minimum delta_t that a precell spike can affect postcell

// For displaying and debugging connections - e.g.
// synapse_info /cell[0]/dend/Ex_channel
// See comments in the file for more details
include synapseinfo.g


//===============================
//      Function Definitions
//===============================

// create prototype input in /library/{precell_name}
function make_precell_proto
        if (!{exists /library})     // But, only if it doesn't already exist
            create neutral /library
        end
        pushe /library
        create neutral {precell_name}
        // fake a soma compartment with a randomspike
        create randomspike {precell_name}/soma
        setfield  {precell_name}/soma min_amp 1.0 max_amp 1.0 \
             reset 1 reset_value 0  rate {frequency}
        create spikegen {precell_name}/soma/spike
        setfield {precell_name}/soma/spike thresh 0.5 abs_refract 0.001
        addmsg {precell_name}/soma {precell_name}/soma/spike INPUT state

        if (ninh_syns > 0)
            create neutral Inh_input
            // fake a soma compartment with a randomspike                                
            create randomspike Inh_input/soma
            setfield  Inh_input/soma min_amp 1.0 max_amp 1.0 \
                reset 1 reset_value 0  rate {frequency}
            create spikegen Inh_input/soma/spike
            setfield Inh_input/soma/spike thresh 0.5 abs_refract 0.001
            addmsg Inh_input/soma Inh_input/soma/spike INPUT state
        end      
        pope
end

function makeinputs
    // uses globals 'precell_name' and 'Ninputs'
    int i
    // usage: createmap source dest Nx Ny -delta dx dy [-origin x y]
    create neutral /inputs
    createmap /library/{precell_name} /inputs {Ninputs} 1 -delta 1 0
    if (ninh_syns > 0)
        createmap /library/Inh_input /inputs {ninh_syns} 1 -delta 1 0
    end
end

function connect_inputs
  int i, j
  for (j=0; j<Ncells; j=j+1)
	for (i=0; i<Ninputs; i=i+1)
        addmsg  /inputs/{precell_name}[{i}]/{spikepath} \
            {cell_name}[{j}]/{Ex_synpath} SPIKE
    end

    if (ninh_syns > 0)
    	for (i=0; i<ninh_syns; i=i+1)
          addmsg  /inputs/Inh_input[{i}]/{spikepath} \
            {cell_name}[{j}]/{Inh_synpath} SPIKE
        end
    end
  end
end

function set_tmax(value)
    float value
    tmax = value
    if(graphics)
        setfield /##[ISA=xgraph] xmax {tmax}
    end
end

extern setup_stdp_update // defined later below

function set_stdp_dt(value)
    float value
    stdp_dt = value
    setclock 2 {stdp_dt}
    if(use_update_object)
        setup_stdp_update
    end
    if(use_update_function)
        useclock /STDP-update 2
    end
end

function set_min_dt(value)
    float value
    min_dt = value
    if(use_update_object)
        setup_stdp_update
    end
end

function set_dAplus(value)
    float value
    dAplus = value
    dAminus = minus_plus_ratio*dAplus
    setup_stdp_update
end

function set_minus_plus_ratio(value)
    float value
    minus_plus_ratio = value
    dAminus = minus_plus_ratio*dAplus
    setup_stdp_update
end

function set_inject(value)
    float value
    injcurrent = value
    setfield {cell_name}/soma inject {injcurrent}
end

function set_gmax(value)
    float value
    gmax = value
    setfield {cell_name}/{Ex_synpath} gmax {gmax}
end

function set_Inh_gmax(value)
    float value
    Inh_gmax = value
    if (ninh_syns > 0)
       setfield {cell_name}/{Inh_synpath} gmax {Inh_gmax}
    end
end

function set_frequency(value) // set input cell average firing frequency
    float value
    frequency = value
    setfield /inputs/{precell_name}[]/soma rate {frequency}
end

function set_weight(value)
    float value
    avg_weight = value
   str chanpath = {cell_name} @ "/" @ {Ex_synpath}
   int i
   // reseed the RNG with to give the same weights
   randseed {seed}   
     for (i=0; i < {getfield {chanpath} nsynapses}; i = i + 1)
        if (rand_weight == 1)
           value = {rand 0 {2*avg_weight}}
        end
        setfield {chanpath} synapse[{i}].weight {value}
     end
end

function set_delays(value)
    float value
    delay = value
    str chanpath = {cell_name} @ "/" @ {Ex_synpath}
    int i
    for (i=0; i < {getfield {chanpath} nsynapses}; i = i + 1)
        if (rand_delay == 1)
           value = {rand 0 {2*delay}}
        end
        setfield {chanpath} synapse[{i}].delay {value}
    end
end

function do_network_out
    if (file_out == 0)
       return
    end
    str filename
    filename = {Vm_file}  @ "_" @ {RUNID} @ ".txt"
    if ({exists /Vm_out})
        call /Vm_out RESET // this closes and reopens the file
        delete /Vm_out
    end
    create asc_file /Vm_out
    setfield /Vm_out  leave_open 1 flush 0 filename {filename}

    filename = {syn_file}  @ "_" @ {RUNID} @ ".txt"
    if ({exists /syn_out})
        call /syn_out RESET // this closes and reopens the file
        delete /syn_out
    end
    create asc_file /syn_out
    setfield /syn_out  leave_open 1 flush 0 filename {filename}

    if({hflag} && {hsolve_chanmode > 1})
        addmsg {cell_name}/solver  /Vm_out SAVE \
          {findsolvefield  {cell_name}/solver soma Vm}
        addmsg  {cell_name}/solver   /syn_out SAVE \
          {findsolvefield  {cell_name}/solver {Ex_synpath} Gk}
        addmsg  {cell_name}/solver   /syn_out SAVE \
          {findsolvefield  {cell_name}/solver {Ex_synpath} Ik}
    else
        addmsg {cell_name}/soma/ /Vm_out SAVE Vm
        addmsg {cell_name}/{Ex_synpath} /syn_out SAVE Gk
        addmsg {cell_name}/{Ex_synpath} /syn_out SAVE Ik
    end
    useclock /Vm_out 1
    useclock /syn_out 1

end

function do_weights_out
    if (weights_out == 0)
       return
    end
    str filename
    filename = {weights_file}  @ "_" @ {RUNID} @ ".txt"
    if ({exists /weights_out})
        call /weights_out RESET // this closes and reopens the file
        delete /weights_out
    end
    create asc_file /weights_out
    setfield /weights_out  leave_open 1 flush 0 filename {filename}
    int i
    for (i=0; i<Ninputs; i=i+1)
        addmsg {cell_name}/{Ex_synpath} /weights_out SAVE synapse[{i}].weight
    end
    useclock /weights_out 3
end

// Output the final weights to a file in a single column
function do_final_weights_out
    str filename
    float fwt
    int i
    float avgwt = 0.0
    filename = {weights_file}  @ "-final_" @ {RUNID} @ ".txt"
    openfile {filename} w
    for (i = 0; i < {Ninputs}; i = i + 1)
         fwt = {getfield  {cell_name}/{Ex_synpath} synapse[{i}].weight }
//         writefile {filename} {fwt}
         // put them all on one line
         writefile {filename} {fwt} " " -n
         avgwt = avgwt + fwt
    end
    avgwt = avgwt/Ninputs
    writefile {filename}  // give a final newline
    closefile {filename}
    echo "Final weights output to file: " {filename}
    echo  " Average weight = " {avgwt}
end

/* -------------------------------------------------------------------------

 stdp_update uses the following global variables (with default values)

int Ninputs = 5  // number of presynaptic Excitatory connections

str cell_name = "/cell"
str Ex_synpath = "soma/Ex_channel" // location of AMPA
str spikepath = "soma/spike"  // location of the cell spikegen

and the additional ones defined below.  Here "pre" represents an event
where a presynaptic spike arrives before the postsynaptic spike is generated.
"post refers to the case where the postsynaptic spike is generated before
the presynaptic spike arrives.  With synaptic (axonal conduction) delays,
the arrival of a presynaptic spike may be delayed until after a postsynaptic
spike is generated.
--------------------------------------------------------------------------- */


float tau_pre	= 0.020	 // decay constant for pre before post spike
float tau_post = 0.020 //  decay constant for post before pre spike
// float max_dt = 0.2 //	maximum time difference	for a weight change.
float min_dt = 0.002  // minimum time diff to account for latency
float max_weight = 2.0*avg_weight
float min_weight = 0.0
float dAplus = 0.010  // Song, Miller, Abbott used 0.005
float minus_plus_ratio = -1.05
float dAminus = minus_plus_ratio*dAplus

/***** For debugging *****/
function print_spike_info
    float last_post_spike, last_pre_spike, lastupdate
    float c_weight, c_delay, delta_t
    int refractory, Nsyns, i
    Nsyns = 5 // number of synapses to inspect
    echo ; echo "current time = " {getstat -time}
    last_post_spike = {getfield {cell_name}/{spikepath} lastevent}
    echo "last postcell spike time = " {last_post_spike}

    // Now loop over the synapses                                               
    for (i=0; i<Nsyns; i=i+1)
        last_pre_spike = {getfield  \
            {cell_name}/{Ex_synpath} synapse[{i}].last_spike_time}
        lastupdate  = {getfield \
            {cell_name}/{Ex_synpath} synapse[{i}].lastupdate}
        delta_t = {getstat -time} - lastupdate
        echo; echo "---------- synapse " {i} " ----------"
        echo "last precell spike time = " {last_pre_spike}
        echo "lastupdate = " {lastupdate} "  delta_t = " {delta_t}
        c_weight={getfield {cell_name}/{Ex_synpath} synapse[{i}].weight}
        c_delay={getfield {cell_name}/{Ex_synpath} synapse[{i}].delay}
        echo "connection delay = " {c_delay} "; weight = " {c_weight}
    end
end

/* Create the stdp_update object if it does not exist and
   fill the fields with needed values
*/
function setup_stdp_update
    str postpath
    str relsynpath
    str relspikepath
    if (change_weights < 0)
        return
    end
    if (!{exists /stdp_applier})
        create stdp_rules /stdp_applier
    end

    /* 
       In this script, 'synpath' and 'spikepath' are relative to the cell.
    */

    postpath = {cell_name} @ "[]"
    relsynpath = {Ex_synpath}
    relspikepath =  {spikepath}
    setfield /stdp_applier cellpath {postpath}
    setfield /stdp_applier synpath {relsynpath}
    setfield /stdp_applier spikepath {relspikepath}
    setfield /stdp_applier tau_pre {tau_pre}
    setfield /stdp_applier tau_post {tau_post}
    setfield /stdp_applier min_dt {min_dt} // min time diff (account for latency)
    setfield /stdp_applier min_weight {min_weight}
    setfield /stdp_applier max_weight {max_weight}
    setfield /stdp_applier dAplus {dAplus}
    setfield /stdp_applier dAplus {dAplus}
    setfield /stdp_applier dAminus {dAminus}
    setfield /stdp_applier change_weights {change_weights}
    setfield /stdp_applier debug_level {debug}
    setclock 2 {stdp_dt}
    useclock  /stdp_applier 2
end // function stdp_update

/* Alternatively, define a script function to perform the
   update when called by a clocked script_out object
*/

// This function should be called with clock interval stdp_dt
function apply_stdp_rules
    str postcell = {cell_name}
    str precell
    float curr_time
    int curr_step
    int post_fired = 0 // flag to see if post cell fired on this step
    float c_weight, c_delay // conduction weight and delay for synapse i
    float last_post_spike // time of most recent firing of cell[1]
    float last_pre_spike // time of most recent firing of cell[0]
    float arrival_time  // arrival time of spike from cell[0]
    float Aplus  // synaptic variable that adds to weight
    float Aminus  // synaptic variable that subtracts from weight
    float lastupdate  // last time synapse was updated from pre or post spike
    float prev_lastupdate // temporary variable for previous lastupdate    
float delta_t       // time difference between pre and post synaptic spikes
    int i // synapse number

    curr_time = {getstat -time} 
    curr_step = {getstat -step} 

    // get postsynaptic cell info
    last_post_spike = {getfield {postcell}/{spikepath} lastevent}
    // if the cell has never fired, try again later
    if(last_post_spike < 0.0)
        return
    end

    // Has the postsynaptic cell fired since the last check?
    if (curr_step < {round {(last_post_spike + stdp_dt)/dt}})
         post_fired = 1
    else
         post_fired = 0
    end
        
    // Now loop over the synapses
    for (i=0; i<{Ninputs}; i=i+1)
        prev_lastupdate = -1.0 // initialize
        // check for arrival of presynaptic spike
        last_pre_spike = \
             {getfield {postcell}/{Ex_synpath} synapse[{i}].last_spike_time}
        if (last_pre_spike > 0.0) \\ otherwise skip the rest - never fired
            lastupdate  = \
                {getfield {postcell}/{Ex_synpath} synapse[{i}].lastupdate}
            if (lastupdate < 0.0) // initialize lastupdate and go on to next synapse
              setfield {postcell}/{Ex_synpath} synapse[{i}].lastupdate {last_pre_spike}
            else // perform update
              delta_t = curr_time - lastupdate
              // was it received within current stdp_dt step?
              if (curr_step < {round {(last_pre_spike + stdp_dt)/dt}} &&  \
                    curr_step >= {round {last_pre_spike/dt}})
                  Aplus={getfield {postcell}/{Ex_synpath} synapse[{i}].Aplus}
                  Aminus={getfield {postcell}/{Ex_synpath} synapse[{i}].Aminus}
                  c_weight={getfield {postcell}/{Ex_synpath} synapse[{i}].weight}
                  Aplus = Aplus*{exp {-delta_t/tau_pre}} + dAplus
                  Aminus = Aminus*{exp {-delta_t/tau_post}}
                  c_weight =  {max {c_weight + Aminus} {min_weight}}
                  setfield {postcell}/{Ex_synpath} synapse[{i}].weight {c_weight}
                  setfield {postcell}/{Ex_synpath} synapse[{i}].Aplus {Aplus}
                  setfield {postcell}/{Ex_synpath} synapse[{i}].Aminus {Aminus}
                  setfield {postcell}/{Ex_synpath} synapse[{i}].lastupdate {curr_time}
                  prev_lastupdate = lastupdate
              end // in time window
            end // if-else
        end // test for existence of last presynaptic spike

        // check for generation of postsynaptic spike
        if(post_fired)  // It has fired within current stdp_dt step
            lastupdate  = \
                {getfield {postcell}/{Ex_synpath} synapse[{i}].lastupdate}
            if (lastupdate < 0.0) // initialize lastupdate and go on to next synapse
                setfield {postcell}/{Ex_synpath} synapse[{i}].lastupdate {last_post_spike}
            else // perform update
                  if (prev_lastupdate > 0.0) // if it was just updated, use prev_lastupdate
                      lastupdate = prev_lastupdate
                  end
                  delta_t = curr_time - lastupdate
                  if (delta_t < min_dt)
                      // do an update, but don't count this event in Aplus
                      delta_t =  curr_time - prev_lastupdate
                  end
                  Aplus={getfield {postcell}/{Ex_synpath} synapse[{i}].Aplus}
                  Aminus={getfield {postcell}/{Ex_synpath} synapse[{i}].Aminus}
                  c_weight={getfield {postcell}/{Ex_synpath} synapse[{i}].weight}
                  Aplus = Aplus*{exp {-delta_t/tau_pre}}
                  Aminus = Aminus*{exp {-delta_t/tau_post}} + dAminus
                  c_weight =  {min {c_weight + Aplus} {max_weight}}
                  setfield {postcell}/{Ex_synpath} synapse[{i}].weight {c_weight}
                  setfield {postcell}/{Ex_synpath} synapse[{i}].Aplus {Aplus}
                  setfield {postcell}/{Ex_synpath} synapse[{i}].Aminus {Aminus}
                  setfield {postcell}/{Ex_synpath} synapse[{i}].lastupdate {curr_time}
            end // if-else
        end // test for postsynaptic spike
    end // loop over synapses
    if((debug > 1) && curr_time > (tmax - 0.05))
        print_spike_info
    end
end // function stdp_update



/* Optionally, provide homeostatic plasticity with synaptic scaling
   to the average weight "avg_weight".  This only applies to excitatory
   connections to {cell_name}/{Ex_synpath}

The syntax is

    normalizeweights source target -fixed scale

*/

function scale_weights
         normalizeweights /inputs/{precell_name}[]/{spikepath}  \
              {cell_name}/{Ex_synpath} -fixed {avg_weight*Ninputs}
end // function scale_weights

function do_run_summary

    str summary = ""  // will not be used for console output ?????
    str line
    line = "Script name: " @ {script_name}
    summary = {summary}  @ {line} @ " \n "
    echo {line}
    line =  "RUNID: " @ {RUNID} @ " date: " @ {getdate} @ " tmax:" @ {tmax} @ " dt:" @ {dt}
    summary = {summary}  @ {line} @ " \n "
    echo {line}
    line =  "Ex gmax: " @ {gmax} @ " Inh gmax: " @ {Inh_gmax}  @ " input_rate: " @ {frequency}
    summary = {summary}  @ {line} @ " \n "
    echo {line}
    line =  "dAplus: " @ {dAplus} @ " dAminus: " @ {dAminus}
    summary = {summary}  @ {line} @ " \n "
    echo {line}
    line =  "stdp_dt = " @ {stdp_dt} @ " min_dt = " @ {min_dt}
    summary = {summary}  @ {line} @ " \n "
    echo {line}
    if(normalize_weights)
        line = "weight normalization to average weight = " @ {avg_weight} \
            @ ", wtscale_dt = " @ {wtscale_dt}
        summary = {summary}  @ {line} @ " \n "
        echo {line}
    end
//    echo {summary}
    echo 
end

function change_RUNID(value)
    str value
    RUNID =  value
    // Set up new file names for output
    do_network_out
    do_weights_out
end

function step_tmax
    echo "RUNID: " {RUNID}
    echo "START: " {getdate}
    step {tmax} -time
    echo "END  : " {getdate}
    do_final_weights_out
    do_run_summary
    print_spike_info
end

//===============================
//    Graphics Functions
//===============================

/*  A subset of the functions defined in genesis/startup/xtools.g
    These are used to provide a "scale" button to graphs.
    "makegraphscale path_to_graph" creates the button and the popup
     menu to change the graph scale.
*/

function setgraphscale(graph)
    str graph
    str form = graph @ "_scaleform"
    str xmin = {getfield {form}/xmin value}
    str xmax = {getfield {form}/xmax value}
    str ymin = {getfield {form}/ymin value}
    str ymax = {getfield {form}/ymax value}
    setfield {graph} xmin {xmin} xmax {xmax} ymin {ymin} ymax {ymax}
    xhide {form}
end

function showgraphscale(form)
    str form
    str x, y
    // find the parent form
    str parent = {el {form}/..}
    while (!{isa xform {parent}})
        parent = {el {parent}/..}
    end
    x = {getfield {parent} xgeom}
    y = {getfield {parent} ygeom}
    setfield {form} xgeom {x} ygeom {y}
    xshow {form}
end

function makegraphscale(graph)
    if ({argc} < 1)
        echo usage: makegraphscale graph
        return
    end
    str graph
    str graphName = {getpath {graph} -tail}
    float x, y
    str form = graph @ "_scaleform"
    str parent = {el {graph}/..}
    while (!{isa xform {parent}})
        parent = {el {parent}/..}
    end

    x = {getfield {graph} x}
    y = {getfield {graph} y}

    create xbutton {graph}_scalebutton  \
        [{getfield {graph} xgeom},{getfield {graph} ygeom},50,25] \
           -title scale -script "showgraphscale "{form}
    create xform {form} [{x},{y},180,170] -nolabel

    disable {form}
    pushe {form}
    create xbutton DONE [10,5,55,25] -script "setgraphscale "{graph}
    create xbutton CANCEL [70,5,55,25] -script "xhide "{form}
    create xdialog xmin [10,35,160,25] -value {getfield {graph} xmin}
    create xdialog xmax [10,65,160,25] -value {getfield {graph} xmax}
    create xdialog ymin [10,95,160,25] -value {getfield {graph} ymin}
    create xdialog ymax [10,125,160,25] -value {getfield {graph} ymax}
    pope
end


function colorize
    setfield /##[ISA=xlabel] fg white bg blue3
    setfield /##[ISA=xbutton] offbg rosybrown1 onbg rosybrown1
//    setfield /##[ISA=xtoggle] onfg red offbg cadetblue1 onbg cadetblue1
    setfield /##[ISA=xdialog] bg palegoldenrod
    setfield /##[ISA=xgraph] bg white
end

// function to return a color name from an index into the colorlist
// Usage example: str color = {colors 3}
function colors(col_num)
    int col_num
    str colorlist = "black blue cyan green magenta red orange gold"
    str color
    // convert col_num to range 1 though 8
    col_num = col_num - {trunc {col_num/8.0}}*8 + 1
    color = {getarg {arglist {colorlist}} -arg {col_num}}
    return {color}
end

/***** Create the simulation GUI *****/

function make_control
    create xform /control [10,50,305,350]
    create xlabel /control/label -bg cyan -label "CONTROL PANEL"
    create xbutton /control/RESET -wgeom 33%       -script reset
    create xbutton /control/RUN  -xgeom 0:RESET -ygeom 0:label -wgeom 33% \
         -script step_tmax
    create xbutton /control/QUIT -xgeom 0:RUN -ygeom 0:label -wgeom 34% \
        -script quit
    create xdialog /control/RUNID -title "RUNID string:" -value {RUNID} \
                -script "change_RUNID <value>"
    create xdialog /control/tmax -label "tmax (sec):" \
    	-value {tmax} -script "set_tmax <v>"

    create xdialog /control/stdp_dt -wgeom 50% -label "STDP dt:" \
    	-value {stdp_dt} -script "set_stdp_dt <v>"
    create xdialog /control/min_dt -ygeom 0:tmax -xgeom 0:stdp_dt -wgeom 50% \
        -label "min_dt:" -value {min_dt} -script "set_min_dt <v>"
    create xdialog /control/dAplus -wgeom 50% -label "dAplus:" \
    	-value {dAplus} -script "set_dAplus <v>"
    create xdialog /control/plus_minus -ygeom 0:stdp_dt -wgeom 50% \
        -xgeom 0:dAplus -label "ratio:" \
    	-value {minus_plus_ratio} -script "set_minus_plus_ratio <v>"
    create xdialog /control/Injection -label "Cell Injection (Amp)" \
		-value {injcurrent} -script "set_inject <value>"

    create xdialog /control/frequency -label "Pre-cells freq (Hz)" \
		-value {frequency} -script "set_frequency <value>"

    create xdialog /control/gmax -label "Ex gmax (S)" -value {gmax} \
        -script "set_gmax <value>"
        create xdialog /control/inh_gmax -label "Inh gmax (S)" -value {Inh_gmax} \
        -script "set_Inh_gmax <value>"
    create xdialog /control/weight -label "Weight" \
		-value {avg_weight} -script "set_weight <value>"
    create xdialog /control/delay -label "Delay" \
		-value {delay} -script "set_delays <value>"
    xshow /control
end

function make_Vmgraph
    float vmin = -0.100
    float vmax = 0.06
    create xform /data [665,530,575,350]
    create xlabel /data/label -hgeom 10% -label "post synaptic cell Vm"
    create xgraph /data/voltage  -hgeom 90%  -title "Membrane Potential"
    setfield ^ XUnits sec YUnits Volts
    setfield ^ xmax {tmax + 0.025} ymin {vmin} ymax {vmax}
    makegraphscale /data/voltage
    useclock /data 1 // out_dt
    xshow /data
end

function make_condgraph
    create xform /condgraphs [315,325,345,350]
    pushe /condgraphs
    create xgraph channel_Gk -hgeom 100% -title "Channel Conductance"
    // Note that the scale for the maximum Gk is hard-coded below
    setfield channel_Gk xmin 0 xmax {tmax} ymin -0.5e-9 ymax 5e-9
    setfield channel_Gk XUnits "sec" YUnits "Gk (Siemens)"
    pope
    makegraphscale /condgraphs/channel_Gk
    useclock 1 /condgraph // out_dt
    xshow /condgraphs
end

function make_stdp_weightgraph
    create xform /stdp_weightgraph [665,50,575,475]
    pushe /stdp_weightgraph
    create xgraph channel_stdp_weight -hgeom 70% -title "synaptic weight"
    setfield ^ xmin 0 xmax {tmax} ymin 0 ymax {max_weight} \
	       XUnits "sec"
    create xgraph channel_Aplus_Aminus -hgeom 30% -title "Aplus/Aminus variables"
    setfield ^ xmin 0 xmax {tmax} ymin -0.02 ymax 0.05 XUnits "sec"
    pope
    useclock /stdp_weightgraph 3
    makegraphscale /stdp_weightgraph/channel_stdp_weight
    makegraphscale /stdp_weightgraph/channel_Aplus_Aminus
    xshow /stdp_weightgraph
end

function make_freqmon
    float tmin = 0.5       // default "window" for the frequency monitor
    float exponent = 0      // default exponent used for weighting
    float ymax = 50
    create spikegen /freq_input
    setfield /freq_input abs_refract 0.001
    create freq_monitor /freq_input/freqmon
    setfield /freq_input/freqmon tmin {tmin}
    addmsg /freq_input /freq_input/freqmon  POSITIVE_TRIG state
    create xform /freq_data [315,50,345,245]  -title "spike frequency monitor"
    create xgraph /freq_data/frequency -hgeom 95%  -title "Post cell Spike Rate"
    setfield ^ xmax {tmax} ymax {ymax} XUnits sec YUnits Freq
//    useclock /freq_input 1
//    useclock /freq_data 1
    makegraphscale /freq_data/frequency
    xshow /freq_data

    addmsg /freq_input/freqmon /freq_data/frequency PLOT frequency *frequency *black

end // function make_freqmon

//===============================
//         Main Script
//===============================

// Create a library of prototype elements to be used by the cell reader
include protodefs_VAcell.g // creates prototype elements for VAcell model

// Set a more realistic abs_refract for the spikegens
setfield /library/spike abs_refract 0.002

// Build the cell from a parameter file using the cell reader
readcell {cell_file} {cell_name}

if (Ncells > 1)  // make copies
    int i
    for (i = 1; i < Ncells; i=i+1)
        copy  {cell_name} {cell_name}[{i}]
    end
end

/* Step 3: Create an array of cells for the input */
 
// create the prototype input cell /library/{precell_name}
make_precell_proto

// Create the input cells
makeinputs

// set the random background excitation frequency of input precells
set_frequency {frequency}

/* Step 4: Setting up hsolve for a network requires setting up a
   solver for one cell of each type in the network and then
   duplicating the solvers for the remaining cells.  The procedure is
   described in the advanced tutorial 'Simulations with GENESIS using
   hsolve by Hugo Cornelis' at
   genesis-sim.org/GENESIS/UGTD/Tutorials/advanced-tutorials
*/

if(hflag)
    // set up a solver for /cell
    pushe {cell_name}
    create hsolve solver
    setmethod solver 11 // Use Crank-Nicholson
    setfield solver chanmode {hsolve_chanmode} path "../[][TYPE=compartment]"
    call solver SETUP

    if (Ncells > 1)  // make copies
        int i
        for (i = 1; i < Ncells; i=i+1)
            call solver DUPLICATE \
                {cell_name}[{i}]/solver  ../##[][TYPE=compartment]
            setfield {cell_name}[{i}]/solver \
            x {getfield {cell_name}[{i}]/soma x} \
            y {getfield {cell_name}[{i}]/soma y} \
            z {getfield {cell_name}[{i}]/soma z}
        end
    end
    pope
end

/* Step 5: Connect the cells in the network */
connect_inputs

// set values of injection, synchan gmax and connection weight
set_inject {injcurrent}
set_gmax {gmax}
set_Inh_gmax {Inh_gmax}
set_weight {avg_weight}
set_delays {delay}

/* step 8: Set up the outputs */

// Create and set up asc_file elements for output
if (file_out)
    do_network_out
end

    do_weights_out


if (graphics)
    // make the control panel
    make_control

    // make the graph to display soma Vm and pass messages to the graph
    make_Vmgraph

    make_stdp_weightgraph
end

if (graphics > 1)
    // Make synaptic conductance graph and pass message to the graph
    make_condgraph
    make_freqmon
end

if (graphics)
    colorize

// Add the messages to the graphs
    if({hflag} && {hsolve_chanmode > 1})
        addmsg  {cell_name}/solver /data/voltage PLOT \
          {findsolvefield  {cell_name}/solver soma Vm} *postcell *red
    else
      addmsg  {cell_name}/soma /data/voltage PLOT Vm *postcell *red
    end
    addmsg  /inputs/{precell_name}/{spikepath} /data/voltage PLOTSCALE state \
        *precell0 *blue 0.05 0.0
    addmsg /inputs/{precell_name}[1]/{spikepath} /data/voltage PLOTSCALE state \
        *precell1 *green 0.05 -0.02
    addmsg /inputs/{precell_name}[2]/{spikepath} /data/voltage PLOTSCALE state \
        *precell2 *magenta 0.05 -0.04
    addmsg /inputs/{precell_name}[3]/{spikepath} /data/voltage PLOTSCALE state \
        *precell3 *cyan 0.05 -0.06
    addmsg /inputs/{precell_name}[4]/{spikepath} /data/voltage PLOTSCALE state \
        *precell3 *gold 0.05 -0.08
  int i
//  Hack to prevent core dump when number of messages is too large
//  for (i=0; i<{Ninputs}; i=i+2)
  for (i=0; i<{Ninputs}; i=i+1)
      addmsg {cell_name}/{Ex_synpath} /stdp_weightgraph/channel_stdp_weight \
          PLOT synapse[{i}].weight *" " *{colors {i}} 
  end
end

if (graphics > 1)
    if({hflag} && {hsolve_chanmode > 1})
        addmsg  {cell_name}/solver /freq_input INPUT \
          {findsolvefield  {cell_name}/solver soma Vm}
        addmsg  {cell_name}/solver  /condgraphs/channel_Gk PLOT \
          {findsolvefield  {cell_name}/solver \
          {cell_name}/{Ex_synpath} Gk} *Ex_Gk *blue
        addmsg  {cell_name}/solver  /condgraphs/channel_Gk PLOT \
          {findsolvefield  {cell_name}/solver \
          {cell_name}/{Inh_synpath} Gk} *Inh_Gk *magenta
        addmsg  {cell_name}/solver  /condgraphs/channel_Gk PLOT \
          {findsolvefield  {cell_name}/solver {Ex_synpath} Ik} *Ex_Ik *red
        addmsg  {cell_name}/solver  /condgraphs/channel_Gk PLOT \
          {findsolvefield  {cell_name}/solver {Inh_synpath} Ik} *Inh_Ik *green
    else
      // trigger if > 0
      addmsg   {cell_name}/soma  /freq_input INPUT Vm
      addmsg  {cell_name}/{Ex_synpath} /condgraphs/channel_Gk PLOT \
        Gk *Ex_Gk *black
      addmsg  {cell_name}/{Inh_synpath} /condgraphs/channel_Gk PLOT \
        Gk *Inh_Gk *magenta
      addmsg  {cell_name}/{Ex_synpath} /condgraphs/channel_Gk PLOT \
        Ik *Ex_Ik *red
      addmsg  {cell_name}/{Ex_synpath} /condgraphs/channel_Gk PLOT \
        Ik *Inh_Ik *green
    end

    //  Hack to prevent core dump when number of messages is too large
    //  for (i=0; i<{Ninputs}; i=i+2)
    for (i=0; i<{Ninputs}; i=i+1)
      addmsg {cell_name}/{Ex_synpath} /stdp_weightgraph/channel_Aplus_Aminus \
          PLOT synapse[{i}].Aplus *" " *{colors {i}}
      addmsg {cell_name}/{Ex_synpath} /stdp_weightgraph/channel_Aplus_Aminus \
          PLOT synapse[{i}].Aminus *" " *{colors {i}}
    end
end

/*** Provide Spike Timing Dependent Plasticity ***/

if ((change_weights > 0) && (use_update_object))
        setup_stdp_update
        echo "Using stdp_update with step "  {getclock 2} ", delay " {delay}
        useclock /stdp_applier 2
end

// These are mutually exclusive
if ((change_weights > 0) && (use_update_function))
    create script_out /STDP-update
    setfield /STDP-update command apply_stdp_rules
    echo "Using SLI stdp_update with step "  {getclock 2} ", delay " {delay}
    useclock /STDP-update 2
end

/* Optionally, provide homeostatic plasticity with synaptic scaling
   to the average weight "avg_weight"
*/

if(normalize_weights)
    create script_out /wtscale
    setfield /wtscale command scale_weights
    setclock 4 {wtscale_dt}
    useclock /wtscale 4
    echo "Using scale_weights with step "  {getclock 4} \
        ", Average Wt = " {avg_weight}
end

reset

if (batch)
   change_RUNID "5146BA"
//   print_spike_info
   tmax = 400
   set_gmax 0.15e-9
   set_Inh_gmax 0.3e-9
   set_frequency 10.0
   debug = 0
   setup_stdp_update
   reset
   reset
   step_tmax
end
