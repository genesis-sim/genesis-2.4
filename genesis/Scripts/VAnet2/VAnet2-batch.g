//genesis  -  VAnet2-batch.g

/*======================================================================

  Improved GENESIS implementation of the dual exponential conductance
  version of the Vogels-Abbott (J. Neurosci. 25: 10786--10795 (2005))
  network model with Hodgkin-Huxley neurons.  Details are given in Brette
  et al. (2007) Simulation of networks of spiking neurons: A review of
  tools and strategies. J. Comput. Neurosci. 23: 349-398.
  http://senselab.med.yale.edu/SenseLab/ModelDB/ShowModel.asp?model=83319

  
  A network of simplified Regular Spiking neocortical neurons providing
  excitatory connections and Fast Spiking interneurons providing inhibitory
  connections.  Based on the RSnet.g example from the GENESIS Neural
  Modeling Tutorials (http://www.genesis-sim.org/GENESIS/Tutorials/)

  ======================================================================*/

/* Customize these strings and parameters to modify this simulation for
   another cell.
*/

str script_name =  "VAnet2-batch.g"
str RUNID = "1000"        // default ID string for output file names
// Booleans indicating the type of calculations or output
int debug = 1 // Provide additional output to console
int batch = 1 // if (batch) run the default simulation
int graphics = 0     // display control panel, graphs, optionally net view
                     // This version has no graphics.
int netview_output = 1	// Record network output (soma Vm) to a file
int write_asc_header = 1 // write header information to ascii file
int EPSC_output = 0  // output Ex_ex EPS currents to file
int calc_EPSCsum = 0 // calculate summed excitatory post-synaptic currents

int Vm_output = 1 // record soma Vm of LL corner, R edge, middle cells
int connect_network = 1  // Set to 0 for testing with unconnected cells
int hflag = 1    // use hsolve if hflag = 1
int hsolve_chanmode = 4  // chanmode to use if hflag != 0
int use_sprng = 1 // Use SPRNG random number generator, rather than default RNG
                  // SPRNG provides higher quality platform-independent
                  // random numbers, and is recommended over the default.
str Ex_cellfile = "VA_HHcell.p"  // name of the cell parameter file
str Inh_cellfile = "VA_HHcell.p"  // name of the cell parameter file

str Ex_cell_name = "Ex_cell"   // name of the excitatory cell
str Inh_cell_name = "Inh_cell" // name of the inhibitory cell

str Ex_ex_synpath = "soma/Ex_channel" // compartment-name/synchan-name
str Ex_inh_synpath = "soma/Inh_channel" // compartment-name/synchan-name
str Inh_ex_synpath = "soma/Ex_channel" // compartment-name/synchan-name
str Inh_inh_synpath = "soma/Inh_channel" // compartment-name/synchan-name

float frequency = 70.0    // default initial random background frequency

float tmax = 0.05		// simulation time for the first steps with input
// This is the time step used in the original script, which is rather large
//
float dt = 50e-6		// default simulation time step
float out_dt = 0.0001   // output every 0.1 msec (also used to clock input)

// Set random number seed. If seed is 0, set randseed from clock,
// else from given seed
// int seed = 0  // Simulation will give different random numbers each time
int seed = 1404706052 // This is to insure that it will reproduce the results
                      // Change to explore effect of different randomizations
if (use_sprng)
    setrand -sprng
end

if (seed)
    randseed {seed}
else
    seed = {randseed}
end

int Ex_NX = 64; int Ex_NY = 50   // 3200 excitatory cells
int Inh_NX = 32; int Inh_NY = 25 // 800 inhibitory cells

/* In this version of the RSnet script, there will be a layer of
   excitatory cells on a grid, and another layer of inhibitory cells,
   with twice the grid spacing, in order to have a 4:1 ratio of
   excitatory to inhibitory cells.
*/

/* 
   The neurons in the Vogels-Abbott model have no location in space,
   and their connections are purely random. In this GENESIS
   implmentation, neurons will be placed on a two dimensional NX by NY
   grid, with points SEP_X and SEP_Y apart in the x and y directions.
   This allows the model to be generalized to more realistic
   connectivities, as well as other more detailed cell models.
   For example, the 'ACnet2' model of primary auditory cortex layer IV
   (Beeman D (2013) A modeling study of cortical waves in primary
   auditory cortex. BMC Neuroscience, 14(Suppl 1):P23
   doi:10.1186/1471-2202-14-S1-P23.)  is based on the original
   dualexpVA-HHnet.g script.  The scripts and tutorial for the
   'ACnet2' model may be viewed at
   http://genesis-sim.org/GENESIS/ACnet2-GENESIS or from ModelDB at
   http://senselab.med.yale.edu/ModelDB/ShowModel.asp?model=150678

   Cortical networks typically have pyramidal cell separations on the
   order of 10 micrometers, and can have local pyramidal cell axonal
   projections of up to a millimeter or more.  For small network
   models, one sometimes uses a larger separation, so that the model
   represents a larger cortical area.  In this case, the neurons in
   the model are a sparse sample of the those in the actual network,
   and each one of them represents many in the biological network.  To
   compensate for this, the conductance of each synapse can be scaled
   by a synaptic weight factor, to represent the increased number of
   neurons that would be providing input in the actual network.  Here,
   we use a larger separation of 40e-6 meters in order to scale down
   a large cortical model with long range connections between cells.
*/

float Ex_SEP_X = 40e-6
float Ex_SEP_Y = 40e-6 
float Inh_SEP_X = 2*Ex_SEP_X  // There are 1/4 as many inihibitory neurons
float Inh_SEP_Y = 2*Ex_SEP_Y

/* parameters for synaptic connections */

float syn_weight = 1.0 // synaptic weight, effectively multiplies gmax
float input_weight = 5.0 // weight for input randomspikes

// Typical conduction velocity for myelinated pyramidal cell axon
// float cond_vel = 0.25 // m/sec
// With these parameters, there is 4 msec delay for a 1 mm separation
// float prop_delay = SEP_X/cond_vel

/* Here, prop_delay is the delay per meter, or 1/cond_vel.  The
   default value is zero, as the benchmark uses no axonal propagation
   delays.  If a non-zero value is used, the set_delays(prop_delay)
   function will use the conduction velocity to calculate delays based
   on radial distance to the target synapse.
*/

float prop_delay = 0.0 //  changed to be delay per meter, or 1/cond_vel

float Ex_ex_gmax = 6e-9
float Ex_inh_gmax = 67e-9
float Inh_ex_gmax = 6e-9
float Inh_inh_gmax = 67e-9

// index of middle cell (or approximately, if NX, NY are even)
int middlecell = {round {(Ex_NY-1)/2}}*Ex_NX + {round {(Ex_NX -1)/2}}
int LLcell = 0
int Redgecell = {{round {(Ex_NY-1)/2}}*Ex_NX}
 
/* for debugging and exploring - see comments in file for details
   Usage:   synapse_info path_to_synchan
   Example: synapse_info /Ex_layer/Ex_cell[5]/Ex_channel
*/
include synapseinfo.g

// =============================
//   Function definitions
// =============================

function set_ex_gmax(value)
   float value
   Ex_ex_gmax = {value}
   Inh_ex_gmax = {value}
   setfield /Ex_layer/Ex_cell[]/{Ex_ex_synpath} gmax {Ex_ex_gmax}
   setfield /Inh_layer/Inh_cell[]/{Inh_ex_synpath} gmax {Inh_ex_gmax}
end

function set_inh_gmax(value)
   float value
   Ex_inh_gmax = {value}
   Inh_inh_gmax = {value}
   setfield /Ex_layer/Ex_cell[]/{Ex_inh_synpath} gmax {Ex_inh_gmax}
   setfield /Inh_layer/Inh_cell[]/{Inh_inh_synpath} gmax {Inh_inh_gmax}
end

/* Used in Step 5:  Connect the cells with planarconnect.  Usage:
 * planarconnect source-path destination-path
 *               [-relative]
 *               [-sourcemask {box,ellipse} xmin ymin xmax ymax]
 *               [-sourcehole {box,ellipse} xmin ymin xmax ymax]
 *               [-destmask   {box,ellipse} xmin ymin xmax ymax]
 *               [-desthole   {box,ellipse} xmin ymin xmax ymax]
 *               [-probability p]
 */

/* Connect each source spike generator to target synchans within the
   specified range.  Set the ellipse axes or box size just higher than the
   cell spacing, to be sure cells are included.  To connect to nearest
   neighbors and the 4 diagonal neighbors, use a box:
     -destmask box {-SEP_X*1.01} {-SEP_Y*1.01} {SEP_X*1.01} {SEP_Y*1.01}
   For all-to-all connections with a 10% probability, set both the sourcemask
   and the destmask to have a range much greater than NX*SEP_X using options
     -destmask box -1 -1  1  1 \
     -probability 0.1
   Set desthole to exclude the source cell, to prevent self-connections.
*/

function connect_cells
    // Ex_layer cells connect to excitatory synchans
    planarconnect /Ex_layer/Ex_cell[]/soma/spike \
        /Ex_layer/Ex_cell[]/{Ex_ex_synpath} \
        -relative \	 // Destination coordinates are measured relative to source
        -sourcemask box -1 -1  1  1 \   // Larger than source area ==> all cells
        -destmask box -1 -1  1  1 \ 
        -desthole box {-Ex_SEP_X*0.5} {-Ex_SEP_Y*0.5} {Ex_SEP_X*0.5} {Ex_SEP_Y*0.5} \
        -probability 0.02

    planarconnect /Ex_layer/Ex_cell[]/soma/spike \
        /Inh_layer/Inh_cell[]/{Inh_ex_synpath} \
        -relative \	    // Destination coordinates are measured relative to source
        -sourcemask box -1 -1  1  1 \   // Larger than source area ==> all cells
        -destmask box -1 -1  1  1  -probability 0.02

    // Inh_layer cells connect to inhibitory synchans
    planarconnect /Inh_layer/Inh_cell[]/soma/spike \
        /Inh_layer/Inh_cell[]/{Inh_inh_synpath} \
        -relative \	    // Destination coordinates are measured relative to source
        -sourcemask box -1 -1  1  1 \   // Larger than source area ==> all cells
        -destmask box -1 -1  1  1 \ 
        -desthole box {-Inh_SEP_X*0.5} {-Inh_SEP_Y*0.5} {Inh_SEP_X*0.5} {Inh_SEP_Y*0.5} \
        -probability 0.02

   planarconnect /Inh_layer/Inh_cell[]/soma/spike \
      /Ex_layer/Ex_cell[]/{Ex_inh_synpath} \
      -relative \	    // Destination coordinates are measured relative to source
      -sourcemask box -1 -1  1  1 \   // Larger than source area ==> all cells
      -destmask box -1 -1  1  1  -probability 0.02
end

function set_weights(weight)
   float weight
   syn_weight = weight
   planarweight /Ex_layer/Ex_cell[]/soma/spike -fixed {weight}
   planarweight /Inh_layer/Inh_cell[]/soma/spike -fixed {weight}
   echo "All synaptic weights set to "{weight}
end


function set_input_weights(weight)
   float weight
   input_weight = weight
   planarweight /randomspike[]  -fixed {weight}
   echo "All input weights set to "{weight}
end

/* If the delay is zero, set it as the fixed delay, else use the conduction
   velocity to calculate delays based on radial distance to the target.
*/

function set_delays(delay)
    float delay
    prop_delay = delay
    if (delay == 0.0)
        planardelay /Ex_layer/Ex_cell[]/soma/spike -fixed {delay}
        planardelay /Inh_layer/Inh_cell[]/soma/spike -fixed {delay}

    else
        planardelay /Ex_layer/Ex_cell[]/soma/spike -radial {1/delay}
        planardelay /Inh_layer/Inh_cell[]/soma/spike -radial {1/delay}
    end
    echo "All propagation delays set to "{delay}" sec/m"
end

function print_avg_syn_number
    int n
    int num_Ex_ex = 0
    int num_Ex_inh = 0
    for (n=0; n < Ex_NX*Ex_NY; n=n+1)
        num_Ex_ex = num_Ex_ex + {getsyncount \
            /Ex_layer/{Ex_cell_name}[{n}]/{Ex_ex_synpath}}
        num_Ex_inh = num_Ex_inh + {getsyncount \
            /Ex_layer/{Ex_cell_name}[{n}]/{Ex_inh_synpath}}
    end
    num_Ex_ex = num_Ex_ex/(Ex_NX*Ex_NY)
    num_Ex_inh = num_Ex_inh/(Ex_NX*Ex_NY)
    int num_Inh_ex = 0
    int num_Inh_inh = 0
    for (n=0; n < Inh_NX*Inh_NY; n=n+1)
        num_Inh_ex = num_Inh_ex + {getsyncount \
            /Inh_layer/{Inh_cell_name}[{n}]/{Inh_ex_synpath}}
        num_Inh_inh = num_Inh_inh + {getsyncount \
            /Inh_layer/{Inh_cell_name}[{n}]/{Inh_inh_synpath}}
    end
    num_Inh_ex = num_Inh_ex/(Inh_NX*Inh_NY)
    num_Inh_inh = num_Inh_inh/(Inh_NX*Inh_NY)
    echo "Average number of Ex_ex synapses per cell: " {num_Ex_ex}
    echo "Average number of Ex_inh synapses per cell: " {num_Ex_inh}
    echo "Average number of Inh_ex synapses per cell: " {num_Inh_ex}
    echo "Average number of Inh_inh synapses per cell: " {num_Inh_inh}
end // print_avg_syn_number

/**** create input to network ****/
function makeinput
    // use the slower output clock for inputs also
    setclock 1 {out_dt}
    int n
    for (n=0; n < Ex_NX*Ex_NY; n=n+1)
      create randomspike /randomspike[{n}]
      useclock /randomspike[{n}] 1
      setfield  /randomspike[{n}] rate 70 abs_refract 0.001 // max freq = 1 kHz
      // Use for random poisson activation of every excitatory cell
      addmsg /randomspike[{n} /Ex_layer/Ex_cell[{n}]/{Ex_ex_synpath} SPIKE
    end
end

function set_frequency(value)
    float value
	  frequency = value
    setfield  /randomspike[] rate {frequency}
end


/**** functions to output results ****/

function make_output(rootname) // asc_file to {rootname}_{RUNID}.txt
    str rootname, filename
    if ({exists {rootname}})
        call {rootname} RESET // this closes and reopens the file
        delete {rootname}
    end
    filename = {rootname} @ "_" @ {RUNID} @ ".txt"
    create asc_file {rootname}
    setfield ^    flush 1    leave_open 1 filename {filename}
    setclock 1 {out_dt}
    useclock {rootname} 1
end

function setup_output
    int i
    str name, cell_name
    make_output "Vm_out"
    if({hflag} && {hsolve_chanmode > 1})
        addmsg /Ex_layer/Ex_cell[{middlecell}]/solver /Vm_out SAVE \
            {findsolvefield /Ex_layer/Ex_cell[{middlecell}]/solver \
                /Ex_layer/Ex_cell[{middlecell}]/soma Vm}

        addmsg /Ex_layer/Ex_cell[{Redgecell}]/solver /Vm_out SAVE \
            {findsolvefield /Ex_layer/Ex_cell[{Redgecell}]/solver \
                /Ex_layer/Ex_cell[{Redgecell}]/soma Vm}

        /* LLcell = 0, and a hack is needed for findsolvefield with
           cell 0.  Instead of 'Ex_cell[0]' it has to be 'Ex_cell'
        */
        addmsg /Ex_layer/Ex_cell/solver /Vm_out SAVE \
            {findsolvefield /Ex_layer/Ex_cell/solver \
                /Ex_layer/Ex_cell/soma Vm}
    else
         foreach cell_name ({arglist "middlecell Redgecell LLcell"})
            name = "/Ex_layer/Ex_cell[" @ {getglobal {cell_name}} @ "]"
            addmsg {name}/soma /Vm_out SAVE Vm
         end
    
    end
end


function step_tmax
  echo "dt = "{getclock 0}"   tmax = "{tmax}
  if (debug)
    echo "Network of "{Ex_NX}" by "{Ex_NY}" excitatory cells with separations" \
        {Ex_SEP_X}" by "{Ex_SEP_Y}
    echo "and "{Inh_NX}" by "{Inh_NY}" inhibitory cells with separations" \
         {Inh_SEP_X}" by "{Inh_SEP_Y}
    echo "Random number generator seed initialized to: " {seed}
    echo
    print_avg_syn_number
    echo "Script name: " {script_name}
    echo "RUNID: " {RUNID}
  end
  echo "average input frequency: " {frequency}
  echo "input weight: " {input_weight}
  echo "START: " {getdate}
  step {tmax} -time
  echo "END  : " {getdate}
end

//===============================
//    Main simulation section
//===============================

setclock  0  {dt}		// set the simulation clock

/* Make the network
   Step 1: Assemble the components to build the prototype cell under the
	   neutral element /library.
*/

// create prototype definitions used by the cell parameter file 'cellfile'

include protodefs.g  // This creates /library with the cell components

// Now /library contains prototype channels, compartments, spikegen

/* Step 2: Create the prototype cell specified in 'cellfile', using readcell.
	   Then, set up the excitatory synchan in the appropriate compartment,
	   and the spikegen attached to the soma
*/
readcell {Ex_cellfile} /library/Ex_cell
setfield /library/Ex_cell/{Ex_ex_synpath} gmax {Ex_ex_gmax}
setfield /library/Ex_cell/{Ex_inh_synpath} gmax {Ex_inh_gmax}
setfield /library/Ex_cell/soma/spike thresh 0  abs_refract 0.005  output_amp 1

// In the VA_HH model Inh_cell and Ex_cell will be identical
readcell {Inh_cellfile} /library/Inh_cell
setfield /library/Inh_cell/{Inh_ex_synpath} gmax {Inh_ex_gmax}
setfield /library/Inh_cell/{Inh_inh_synpath} gmax {Inh_inh_gmax}
setfield /library/Inh_cell/soma/spike thresh 0  abs_refract 0.005  output_amp 1

/* Step 3 - make a 2D array of cells with copies of /library/cell */
// usage: createmap source dest Nx Ny -delta dx dy [-origin x y]

/* There will be NX cells along the x-direction, separated by SEP_X,
   and  NY cells along the y-direction, separated by SEP_Y.
   The default origin is (0, 0).  This will be the coordinates of cell[0].
   The last cell, cell[{NX*NY-1}], will be at (NX*SEP_X -1, NY*SEP_Y-1).
*/

createmap /library/Ex_cell /Ex_layer {Ex_NX} {Ex_NY} \
    -delta {Ex_SEP_X} {Ex_SEP_Y}

// Displace the /Inh_layer origin to be in between Ex_cells
createmap /library/Inh_cell /Inh_layer {Inh_NX} {Inh_NY} \
    -delta {Inh_SEP_X} {Inh_SEP_Y} -origin {Ex_SEP_X/2}  {Ex_SEP_Y/2}

/* Step 4: Setting up hsolve for a network requires setting up a
   solver for one cell of each type in the network and then
   duplicating the solvers for the remaining cells.  The procedure is
   described in the advanced tutorial 'Simulations with GENESIS using
   hsolve by Hugo Cornelis' at
   genesis-sim.org/GENESIS/UGTD/Tutorials/advanced-tutorials
*/

if(hflag)
    pushe /Ex_layer/Ex_cell[0]
    create hsolve solver
    setmethod solver 11 // Use Crank-Nicholson
    setfield solver chanmode {hsolve_chanmode} path "../soma"
    call solver SETUP
    int i
    for (i = 1 ; i < {Ex_NX*Ex_NY} ; i = i + 1)
        call solver DUPLICATE \
        /Ex_layer/Ex_cell[{i}]/solver  ../soma
        setfield /Ex_layer/Ex_cell[{i}]/solver \
            x {getfield /Ex_layer/Ex_cell[{i}]/soma x} \
            y {getfield /Ex_layer/Ex_cell[{i}]/soma y} \
            z {getfield /Ex_layer/Ex_cell[{i}]/soma z}
    end
    pope
    pushe /Inh_layer/Inh_cell[0]
    create hsolve solver
    setmethod solver 11
    setfield solver chanmode {hsolve_chanmode} path "../[][TYPE=compartment]"
    call solver SETUP
    int i
    for (i = 1 ; i < {Inh_NX*Inh_NY} ; i = i + 1)
  	call solver DUPLICATE \
            /Inh_layer/Inh_cell[{i}]/solver	 ../##[][TYPE=compartment]
    setfield /Inh_layer/Inh_cell[{i}]/solver \
        x {getfield /Inh_layer/Inh_cell[{i}]/soma x} \
        y {getfield /Inh_layer/Inh_cell[{i}]/soma y} \
        z {getfield /Inh_layer/Inh_cell[{i}]/soma z}
    end
    pope
end

// Do an overall reset, just to be sure hsolve is properly set up
reset

/* Step 5: Now connect them up with planarconnect */
if (connect_network)
    connect_cells // connect up the cells in the network layers
end


/* Step 6: Set the axonal propagation delay and weight fields of the target
   synchan synapses for all spikegens.  To scale the delays according to
   distance instead of using a fixed delay, use
       planardelay /network/cell[]/soma/spike -radial {cond_vel}
   and change the cond_vel to a non-zero value.  This would be
   appropriate when connections are made to more distant cells.  Other
   options of planardelay and planarweight allow some randomized variations
   in the delay and weight.
*/

planarweight /Ex_layer/Ex_cell[]/soma/spike -fixed {syn_weight}
planarweight /Inh_layer/Inh_cell[]/soma/spike -fixed {syn_weight}

set_input_weights {input_weight}

/* If the delay is zero, set it as the fixed delay, else use the conduction
   velocity and calculate delays based on radial distance to target
*/
if (prop_delay == 0.0)
    planardelay /Ex_layer/Ex_cell[]/soma/spike -fixed {prop_delay}
    planardelay /Inh_layer/Inh_cell[]/soma/spike -fixed {prop_delay}
else
    planardelay /Ex_layer/Ex_cell[]/soma/spike -radial {1/prop_delay}
    planardelay /Inh_layer/Inh_cell[]/soma/spike -radial {1/prop_delay}
end

/* Step 7: Set up the inputs */
makeinput
set_frequency 70.0

/* step 8: Set up the outputs */

if (Vm_output)
    setup_output
end

reset

/* Step 9: Make any necessary parameter changes between runs
   Step 10: Run the simulation for a specified time

   In the original simulation, this was done through the GUI.
*/
if(batch)
    echo "RUNID: " {RUNID}
    set_frequency 70
    set_input_weights 10.0
    reset
    reset
    tmax = 0.05
    step_tmax
    set_frequency 0
     tmax = 4.95
    step_tmax
end
 