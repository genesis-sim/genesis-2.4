//genesis  -  dualexpVA-HHnet.g

/*======================================================================

  The GENESIS implementation of the dual exponential conductance version of
  the Vogels-Abbott (J. Neurosci. 25: 10786--10795 (2005)) network model
  with Hodgkin-Huxley neurons.  Details are given in Destexe et al. (2006)
  Simulation of networks of spiking neurons: A review of tools and
  strategies.  Submitted to JCNS.

  A network of simplified Regular Spiking neocortical neurons providing
  excitatory connections and Fast Spiking interneurons providing inhibitory
  connections.  Based on the RSnet.g example from the GENESIS Neural
  Modeling Tutorials (http://www.genesis-sim.org/GENESIS/Tutorials/)

  ======================================================================*/

/* Customize these strings and parameters to modify this simulation for
   another cell.
*/

str Ex_cellfile = "VA_HHcell.p"  // name of the cell parameter file
str Inh_cellfile = "VA_HHcell.p"  // name of the cell parameter file
str Ex_ex_synpath = "soma/Ex_channel" // compartment-name/synchan-name
str Ex_inh_synpath = "soma/Inh_channel" // compartment-name/synchan-name
str Inh_ex_synpath = "soma/Ex_channel" // compartment-name/synchan-name
str Inh_inh_synpath = "soma/Inh_channel" // compartment-name/synchan-name

// Label to appear on the graph
str graphlabel = "Vogels-Abbott model with HH neurons"

float tmax = 2.0		// simulation time
float dt = 50e-6		// simulation time step

int Ex_NX = 64; int Ex_NY = 50   // 3200 excitatory cells
int Inh_NX = 32; int Inh_NY = 25 // 800 inhibitory cells

/* In this version of the RSnet script, there will be a layer of
   excitatory cells on a grid, and another layer of inhibitory cells,
   with twice the grid spacing, in order to have a 4:1 ratio of
   excitatory to inhibitory cells.
*/

/* Neurons will be placed on a two dimensional NX by NY grid, with points
   SEP_X and SEP_Y apart in the x and y directions.

   Cortical networks typically have pyramidal cell separations on the order
   of 10 micrometers, and can have local pyramidal cell axonal projections
   of up to a millimeter or more.  For small network models, one sometimes
   uses a larger separation, so that the model represents a larger cortical
   area.  In this case, the neurons in the model are a sparse sample of the
   those in the actual network, and each one of them represents many in the
   biological network.  To compensate for this, the conductance of each
   synapse is scaled by a synaptic weight factor, to represent the
   increased number of neurons that would be providing input in the actual
   network.  Here, we use a very large separation of 1 mm that would be
   appropriate for a piriform cortex model with long range connections
   between cells.
*/


float Ex_SEP_X = 0.001  // 1 mm
float Ex_SEP_Y = 0.001 
float Inh_SEP_X = 2*Ex_SEP_X  // There are 1/4 as many inihibitory neurons
float Inh_SEP_Y = 2*Ex_SEP_Y

/* parameters for synaptic connections */

float syn_weight = 1.0 // synaptic weight, effectively multiplies gmax

// Typical conduction velocity for myelinated pyramidal cell axon
// float cond_vel = 0.5 // m/sec
// With these separations, there is 2 msec delay between nearest neighbors
// float prop_delay = SEP_X/cond_vel

/* In this version of RSnet, prop_delay is the delay per meter, or
   1/cond_vel.  The default value is zero, as the benchmark uses no
   axonal propagation delays.  If a non-zero value is used, the
   set_delays(prop_delay) function will use the conduction velocity to
   calculate delays based on radial distance to the target synapse.
*/

float prop_delay = 0.0 //  changed to be delay per meter, or 1/cond_vel

float Ex_ex_gmax = 6e-9
float Ex_inh_gmax = 67e-9
float Inh_ex_gmax = 6e-9
float Inh_inh_gmax = 67e-9

// index of middle cell (or approximately, if NX, NY are even)
int middlecell = {round {(Ex_NY-1)/2}}*Ex_NX + {round {(Ex_NX -1)/2}}
int InjCell = middlecell // default current injection point
// default (initial) parameters for current injection

// Pulsed current injection -- for constant injection, use injwidth >= tmax
float injwidth = 0.05 // 50 msec
float injdelay = 0
float injinterval = 1
float injcurrent = 1.0e-9

/* for debugging and exploring - see comments in file for details
   Usage:   synapse_info path_to_synchan
   Example: synapse_info /network/cell[5]/Ex_channel
*/
include synapseinfo.g

// =============================
//   Function definitions
// =============================

function step_tmax
    echo "dt = "{getclock 0}"   tmax = "{tmax}
    echo "START: " {getdate}
    step {tmax} -time
    echo "END  : " {getdate}
end

function set_ex_gmax(value)
   float value
   Ex_ex_gmax = {value}*1e-9
   Inh_ex_gmax = {value}*1e-9
   setfield /Ex_layer/Ex_cell[]/{Ex_ex_synpath} gmax {Ex_ex_gmax}
   setfield /Inh_layer/Inh_cell[]/{Inh_ex_synpath} gmax {Inh_ex_gmax}
end

function set_inh_gmax(value)
   float value
   Ex_inh_gmax = {value}*1e-9
   Inh_inh_gmax = {value}*1e-9
   setfield /Ex_layer/Ex_cell[]/{Ex_inh_synpath} gmax {Ex_inh_gmax}
   setfield /Inh_layer/Inh_cell[]/{Inh_inh_synpath} gmax {Inh_inh_gmax}
end


function set_weights(weight)
   float weight
   planarweight /Ex_layer/Ex_cell[]/soma/spike -fixed {weight}
   planarweight /Inh_layer/Inh_cell[]/soma/spike -fixed {weight}
   echo "All synaptic weights set to "{weight}
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


function set_inj_timing(delay, width, interval)
    float delay, width, interval
    setfield /injectpulse width1 {width} delay1 {delay}  \
        baselevel 0.0 trig_mode 0 delay2 {interval - delay} width2 0
    // free run mode with very long delay for 2nd pulse (non-repetitive)
    // level1 is set by the inj_toggle function
end

function set_frequency(frequency)
    float frequency
    setfield /Ex_layer/Ex_cell[]/{Ex_ex_synpath} frequency {frequency}
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

// 
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

/* Step 4: Now connect them up with planarconnect.  Usage:
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

// Ex_layer cells connect to excitatory synchans
planarconnect /Ex_layer/Ex_cell[]/soma/spike \
    /Ex_layer/Ex_cell[]/{Ex_ex_synpath} \
    -relative \	    // Destination coordinates are measured relative to source
    -sourcemask box -1 -1  1  1 \   // Larger than source area ==> all cells
    -destmask box -1 -1  1  1 \ 
    -desthole box {-Ex_SEP_X*0.5} {-Ex_SEP_Y*0.5} {Ex_SEP_X*0.5} {Ex_SEP_Y*0.5} \
    -probability 0.02
planarconnect /Ex_layer/Ex_cell[]/soma/spike \
    /Inh_layer/Inh_cell[]/{Inh_ex_synpath} \
    -relative \	    // Destination coordinates are measured relative to source
    -sourcemask box -1 -1  1  1 \   // Larger than source area ==> all cells
    -destmask box -1 -1  1  1 \ 
    -probability 0.02

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
    -destmask box -1 -1  1  1 \ 
    -probability 0.02

/* Step 5: Set the axonal propagation delay and weight fields of the target
   synchan synapses for all spikegens.  To scale the delays according to
   distance instead of using a fixed delay, use
       planardelay /network/cell[]/soma/spike -radial {cond_vel}
   and change dialogs in graphics.g to set cond_vel.  This would be
   appropriate when connections are made to more distant cells.  Other
   options of planardelay and planarweight allow some randomized variations
   in the delay and weight.
*/
planarweight /Ex_layer/Ex_cell[]/soma/spike -fixed {syn_weight}
planarweight /Inh_layer/Inh_cell[]/soma/spike -fixed {syn_weight}


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


/* Set up the circuitry to provide injection pulses to the network */
create pulsegen /injectpulse // Make a periodic injection current step
set_inj_timing {injdelay} {injwidth} {injinterval}
// set the amplitude (current) with the gain of a differential amplifier
create diffamp /injectpulse/injcurr
setfield /injectpulse/injcurr saturation 10000 gain {injcurrent}
addmsg /injectpulse /injectpulse/injcurr PLUS output
addmsg /injectpulse/injcurr /Ex_layer/Ex_cell[{InjCell}]/soma INJECT output

// include the graphics functions
include graphics

// make the control panel
make_control

// make the graph to display soma Vm and pass messages to the graph
make_Vmgraph

/* To eliminate net view and run faster, comment out the next line */
make_netview
colorize
check
reset
echo "Network of "{Ex_NX}" by "{Ex_NY}" excitatory cells with separations "{Ex_SEP_X}" by "{Ex_SEP_Y}
echo "and "{Inh_NX}" by "{Inh_NY}" inhibitory cells with separations "{Inh_SEP_X}" by "{Inh_SEP_Y}
