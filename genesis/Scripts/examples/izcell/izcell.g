//genesis  -  izcell.g
/* A demonstration of the Izhikevich simple spiking neuron model. */
/* units here are V and sec (SI units); use uscale = 1000 for mv and msec */

float uscale = 1; str tunit = "sec"
// float uscale = 1000; str tunit = "msec"

float tmax = 0.25*uscale		// simulation time
float dt = (100e-6)*uscale	// simulation time step
setclock  0  {dt}		// set the simulation clock

float A, B, C, D, inject, Vmax
// EREST normally is C parameter, but allows for different voltage on reset
float EREST

float injwidth = 0.9*tmax
// for constant injection, use injwidth = tmax*uscale
float injdelay = 0.1*tmax
float injinterval = 1.0*tmax
float injcurrent = 10

str cell = "/cell"

// =============================
//   Function definitions
// =============================

function step_tmax
    echo "START: " {getdate}
    step {tmax} -time
    echo "END  : " {getdate}
end

function set_inj_timing(delay, width, interval)
    float delay, width, interval
    setfield /injectpulse level1 1 width1 {width} delay1 {delay}  \
        baselevel 0.0 trig_mode 0 delay2 {interval - delay} width2 0
    // free run mode with no second level2 pulse
end

/* parameters for spike patterns in Figure 1 - IEEE Trans NN Sept 2004 */
/* Originally in units of mV and msec and changed to V and sec */
/* convert them back to mV and msec with uscale = 1000 */

Vmax = 0.03*uscale // Vm is reset to C if Vm > Vmax

/* uncomment the line with the set to be used */
/* (A) tonic spiking */
// A = 20/uscale; B = 200/uscale; C = -0.065*uscale; D = 6; inject = 14;

/* (B) phasic spiking */
// A = 20/uscale; B = 250/uscale; C = -0.065*uscale; D = 6; inject = 0.5;

/* (C) tonic bursting */
// A = 20/uscale; B = 200/uscale; C = -0.050*uscale; D = 2; inject = 15;

/* (D) phasic bursting */
// A = 20/uscale B = 250/uscale; C = -0.055*uscale; D = 0.05; inject = 0.6;

/* (E) mixed mode */
// A = 20/uscale; B = 200/uscale; C = -0.055*uscale; D = 4; inject = 10;

/* (F) spike frequency adaptation */
// A = 10/uscale; B = 200/uscale; C = -0.065*uscale; D = 8; inject = 30;

/* (G) Class 1 excitable - rate increases with ramp, inject = 0 - 20 */
/* also need to set coeff1 = 4100, coeff0 = 108 */
// A = 20/uscale; B = -100/uscale; C = -0.055*uscale; D = 6; inject = 10;

/* (H) Class 2 excitable - no firing at low end of ramp, inject = -0.5 - 3.55 */
// A = 200/uscale; B = 260/uscale; C = -0.065*uscale; D = 0; inject = 0;

/* (I) spike latency - same as Fig. 1 (A) with 3 msec pulse of inject = 7 */

/* my slower firing version of Class 2 excitable */
// A = 200/uscale; B = 200/uscale; C = -0.065*uscale; D = 8; inject = 0;

/* Other parameters from Figure 2 - IEEE Trans NN Nov 2003 */
/* RS regular spiking - rat motor cortex pyramidal cell with adaptation */
A = 20/uscale; B = 200/uscale; C = -0.065*uscale; D = 8; inject = 10;

/* FS fast spiking - rat motor cortex inhibitory interneuron */
// A = 1000/uscale; B = 200/uscale; C = -0.065*uscale; D = 2; inject = 10;

/* IB intrinsically bursting motor cortex - same as Fig. 1 (E) */

/* CH chattering - same as Fig. 1 (C) */

/* TC thalamocortical - fires a burst for injection step from -10 to 0 */
// A = 20/uscale; B = 250/uscale; C = -0.065*uscale; D = 0.05; inject = 10;

/* RZ resonator */
// A = 100/uscale; B = 280/uscale; C = -0.065*uscale; D = 2; inject = -1.35;

/*  LTS low threshold spiking */
// A = 20/uscale; B = 250/uscale; C = -0.065*uscale; D = 2; inject = 10;

/* create the cell */
create izcell {cell}

/* The following parameters are for the layer 5 RS cortical pyramidal cell
   shown in Chapter 10, Fig. 10.12 of Izhikevich (2006), Dynamical Systems
   in Neuroscience.  Note that the coefficients for the dVm/dt equation
   must also be changed after the cell is created.
*/
//  A = 30/uscale; B = -20/uscale; C = -0.05*uscale; D = 1; inject = 1;
//  float coeff2 = 7000; float coeff1 = 700; float coeff0 = 18
//  setfield {cell} coeff2 {coeff2} coeff1 {coeff1} coeff0 {coeff0}

/* Injection will be with an INJECT message, so set inject to 0 */
setfield {cell} A {A} B {B} C {C} D {D} inject 0 Vmax {Vmax}
EREST = C // reset_sim will initialize Vm to EREST
injcurrent = inject // This will be the amplitude of the injection pulses

// apply scaling to cell parameters coeff2 and coeff1 if using mV and msec
setfield {cell} coeff2 {{getfield {cell} coeff2}/(uscale*uscale)} \
    coeff1 {{getfield {cell} coeff1}/uscale}

/* Set up the circuitry to provide injection pulses to the cell */
create pulsegen /injectpulse // Make a periodic injection current step
set_inj_timing {injdelay} {injwidth} {injinterval}
// set the amplitude (current) with the gain of a differential amplifier
create diffamp /injectpulse/injcurr
setfield /injectpulse/injcurr saturation 10000 gain {injcurrent}
addmsg /injectpulse /injectpulse/injcurr PLUS output
addmsg /injectpulse/injcurr {cell} INJECT output

// include the graphics functions
include izgraphics.g

make_control // make the control panel
make_Vmgraph // make the graphs and pass messages to the graphs
colorize     // give the widgets some color

check
reset_sim   // do a reset, with color cycling if overlay on

