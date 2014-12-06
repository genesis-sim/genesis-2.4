//genesis  -  iznet.g
/*  A network of RS (regular spiking cortical pyramidal cell) izcells, with
    local excitatory connections
*/
 
/* for debugging and exploring - see comments in synapseinfo.g for details
   Usage:   synapse_info path_to_synchan
   Example: synapse_info /network/cell[5]/Ex_channel
*/
include synapseinfo.g

/* units here are V and sec (SI units); use uscale = 1000 for mv and msec */
float uscale = 1; str tunit = "sec"
// float uscale = 1000; str tunit = "msec"
    
float tmax = 2.0*uscale		// simulation time
float dt = (100e-6)*uscale	// simulation time step
setclock  0  {dt}		// set the simulation clock

float A, B, C, D, inject, Vmax
float SEP_X = 0.001 // 1 mm
float SEP_Y = 0.001  // piriform tutorial uses 0.00067
int NX = 25  // number of cells = NX*NY
int NY = 25

/* parameters for synaptic connections */

float cond_vel = 0.5 // m/sec - or 2 msec delay for nearest neighbors
float prop_delay = (SEP_X/cond_vel)*uscale
float syn_weight = 4 // synaptic weight, effectively multiplies gmax
float gmax = 20/uscale // maximum synchan conductance = 100*gdens (S/m^2)
float tau = 0.003*uscale // channel time constant

/* NOTE: because of instantaneous resetting of integrate and fire model Vm,
   a smaller absolute refractory period than usual is called for.  With too
   large of a value, spikes in a rapid burst can be lost.  However, the lack
   of a true absolute refractory period in this cell model can result in
   arbitrarily fast spiking under strong excitation, unless it is limited by
   the abs_refract parameter of the spikegen.  The value below is a compromise.
*/
float abs_refract = 0.003*uscale

// index of middle cell (or approximately, if NX, NY are even)
int middlecell = {round {(NY-1)/2}}*NX + {round {(NX -1)/2}}
int InjCell = middlecell // default current injection point

// default (initial) parameters for current injection
// for constant injection, use injwidth = tmax*uscale
float injwidth = 0.01*uscale // 10 msec
float injdelay = 0.2*uscale
float injinterval = 0.2*uscale
float injcurrent = 10

// =============================
//   Function definitions
// =============================

function step_tmax
    echo {NX*NY}" cells    dt = "{getclock 0}"   tmax = "{tmax}
    echo "START: " {getdate}
    step {tmax} -time
    echo "END  : " {getdate}
end

function set_weights(weight)
   float weight
   planarweight /network/cell[]/spike -fixed {weight}
end

function set_delays(delay)
   float delay
   planardelay /network/cell[]/spike -fixed {delay}
//   planardelay /network/cell[]/spike -radial {cond_vel}
end

function set_abs_refract(abs_refract)
    float abs_refract
    setfield /network/cell[]/spike abs_refract {abs_refract}
end

function set_inj_timing(delay, width, interval)
    float delay, width, interval
    setfield /injectpulse width1 {width} delay1 {delay}  \
        baselevel 0.0 trig_mode 0 delay2 {interval - delay} width2 0
    // free run mode with no second level2 pulse
    // level1 is set by the inj_toggle function
end

function set_frequency(frequency) // Frequency of random background activation
    float frequency
    setfield /network/cell[]/Ex_channel frequency {frequency}
end

/* file for synaptic channels from genesis/Scripts/neurokit/prototypes */
include synchans

/* Some parameters for spike patterns in Figure 1 - IEEE Trans NN Sept 2004
    and from Figure 2 - IEEE Trans NN Nov 2003.  These were originally in
    units of mV and msec and changed to V and sec.  Convert them back to mV
    and msec with uscale = 1000
*/

Vmax = 0.03*uscale // Vm is reset to C if Vm > Vmax

/* Use RS as the default -  Fig. 2 (RS) regular spiking - rat motor cortex
   pyramidal cell with adaptation
*/
A = 20/uscale; B = 200/uscale; C = -0.065*uscale; D = 8; inject = 10;

/* some other alternatives - uncomment the line with the set to be used */
/* Fig. 1 (A) tonic spiking */
// A = 20/uscale; B = 200/uscale; C = -0.065*uscale; D = 6; inject = 10;

/* Fig. 1 (C) tonic bursting - same as Fig. 2 CH*/
// A = 20/uscale; B = 200/uscale; C = -0.050*uscale; D = 2; inject = 10;

/* Fig. 1 (E) mixed mode - same as Fig. 2 IB */
// A = 20/uscale; B = 200/uscale; C = -0.055*uscale; D = 4; inject = 10;

/* Fig. 1 (F) spike frequency adaptation */
// A = 10/uscale; B = 200/uscale; C = -0.065*uscale; D = 8; inject = 30;

/* FS fast spiking - rat motor cortex inhibitory interneuron */
// A = 1000/uscale; B = 200/uscale; C = -0.065*uscale; D = 2; inject = 10;

/* my slower Class 2 excitable - rate increases with injection ramp */
// A = 200/uscale; B = 200/uscale; C = -0.065*uscale; D = 8; inject = 10;

/* Step 1 - create the prototype cell */
create neutral /library
disable /library
create izcell /library/cell
setfield /library/cell A {A} B {B} C {C} D {D} inject 0 Vmax {Vmax}
// apply scaling to coeff2 and coeff1 if using mV and msec
setfield /library/cell \
    coeff2 {{getfield /library/cell coeff2}/(uscale*uscale)} \
    coeff1 {{getfield /library/cell coeff1}/uscale}
pushe /library/cell
// Make a prototype excitatory channel, "Ex_channel"
make_Ex_channel     /* synchan with Ek = 0.045, tau1 = tau2 = 3 msec */
setfield Ex_channel tau1 {0.003*uscale} tau2 {0.003*uscale} Ek {0.045*uscale}
/* For future use, could make a prototype inhibitory channel, "Inh_channel" */
//make_Inh_channel     /* synchan with Ek = -0.082, tau1 = tau2 = 20 msec */

create spikegen spike
setfield spike thresh 0  abs_refract {abs_refract}  output_amp 1

addmsg  .  spike  INPUT Vm
setfield Ex_channel gmax {gmax}
// setfield Inh_channel gmax 40

/* connect them to the cell */
addmsg   Ex_channel  . CHANNEL Gk Ek
addmsg   .  Ex_channel VOLTAGE Vm
pope

/* Build network */
/* Step 2 - make a 2D array of cells with copies of /library/cell */
// createmap source dest Nx Ny -delta dx dy -origin x y
createmap /library/cell /network {NX} {NY} -delta {SEP_X} {SEP_Y}

/* Step 3 - Now connect them up with
 * planarconnect source-path destination-path
 *               [-relative]
 *               [-sourcemask {box,ellipse} xmin ymin xmax ymax]
 *               [-sourcehole {box,ellipse} xmin ymin xmax ymax]
 *               [-destmask   {box,ellipse} xmin ymin xmax ymax]
 *               [-desthole   {box,ellipse} xmin ymin xmax ymax]
 *               [-probability p]
 */

/* Connect each source spike generator to excitatory synchans on the 4
   nearest neighbors.  Set the ellipse axes or box size just higher than the
   cell spacing, to be sure cells are included.  To connect to nearest
   neighbors and the 4 diagonal neighbors, use a box:
    -destmask box {-SEP_X*1.01} {-SEP_Y*1.01} {SEP_X*1.01} {SEP_Y*1.01}
   For all-to-all connections with a 10% probability, set both the sourcemask
   and the destmask to have a range much greater than NX*SEP_X using options
    -destmask box -1 -1  1  1 \
    -probability 0.1
*/

planarconnect /network/cell[]/spike /network/cell[]/Ex_channel -relative \
    -sourcemask box -1 -1  1  1 \
    -destmask ellipse 0 0  {SEP_X*1.01} {SEP_Y*1.01} \
    -desthole box 0 0 0 0 \	// exclude source cell from self-connect
    -probability 1.1	// set probability > 1 to connect to all in destmask

/* Step 4 - set the axonal propagation delay and weight fields of the target
   synchan synapses for all spikegens.  To scale the delays according to
   distance instead of a fixed delay, use
       planardelay /network/cell[]/spike -radial {cond_vel}
   and change the dialogs in iznet_graphics.g to set cond_vel
*/
planardelay /network/cell[]/spike -fixed {prop_delay}
planarweight /network/cell[]/spike -fixed {syn_weight}

/* Set up the circuitry to provide injection pulses to the network */
create pulsegen /injectpulse // Make a periodic injection current step
set_inj_timing {injdelay} {injwidth} {injinterval}
// set the amplitude (current) with the gain of a differential amplifier
create diffamp /injectpulse/injcurr
setfield /injectpulse/injcurr saturation 10000 gain {injcurrent}
addmsg /injectpulse /injectpulse/injcurr PLUS output
addmsg /injectpulse/injcurr /network/cell[{InjCell}] INJECT output

// include the file defining the graphics functions
include iznet_graphics

make_control // make the control panel
make_Vmgraph // make the graphs and pass messages to the graphs

/* To eliminate net view and run about 40% faster, comment out the next line */
make_netview // Create an xview widget to display network Vm
colorize     // give the widgets some color

check
reset
echo "Network of "{NX}" by "{NY}" cells with separations "{SEP_X}" by "{SEP_Y}
