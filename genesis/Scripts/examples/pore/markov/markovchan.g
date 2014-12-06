// genesis - markovchan.g

/* This script defines functions to create the extended objects
   Na_squid_markov and K_squid_markov from the pore objects Napores and
   Kpores.  These objects (described in the reference manual) use Markov
   kinetics to simulate a finite population of ion channel pores.  Instead of
   a Gbar field, there is Gmax (the open state conductance of a single pore)
   and the total number of pores, Nt.  This is set to SOMA_A * pores/area =
   SOMA_A * gdens(S/m^2)/Gmax, where gdens is an added field, giving the
   maximum conductance per unit area (Gbar/SOMA_A) and the global variable
   SOMA_A is the area of the parent compartment.

   As the cell reader does not recognize these channels, the "*compt" option
   should be used with readcell to create copies of a prototype compartment
   that contains the prototype channels.  Then, we need a way to determine the
   area of the parent compartment and set Nt appropriately, as this will not
   be done by readcell.  This is done by the extended RESET, and CREATE
   actions, and the SET action, when gdens is set.  This script contains a
   function make_markov_compt for creating such a compartment.

   An example script that uses these prototypes with the cell reader may
   be found in Scripts/examples/pore/markov.

   NOTE: When using these channels, it may be necessary to give the reset
   command twice, or to call the RESET action for the channels before doing a
   general reset, in order to be sure that compartments are reset after the
   channels have been reset.
*/

// CONSTANTS
float EREST_ACT = -0.070 // volts
float ENA = 0.045
float EK = -0.090
float RM = 0.33333              // specific membrane resistance (ohms m^2)
float CM = 0.01                 // specific membrane capacitance (farads/m^2)
float RA = 0.3                  // specific axial resistance (ohms m)

/* These are the values typical for squid axon.  EREST_ACT, RM, CM, and RA can
be overridden by values defined before invoking make_markov_compt.  If
readcell is used, then any values given in the cell parameter file will
have the final say.
*/

/* prototype compartment area in square meters, used to set the channel Nt
   from the conductance density, gdens.  This is useful to give Nt an initial
   value.  SOMA_A may be changed to an appropriate value.
*/

float SOMA_A = 30e-6*30e-6*3.14159

/* function used to extend the RESET action of the squid_markov prototypes */
function squid_markov_RESET(action)
    float area, len, dia
    float PI = 3.14159
    call . RESET -parent
    if ({isa compartment ..})
        len = {getfield .. len}
        dia = {getfield .. dia}

        if (len > 1.0e-12)
            area = {PI*dia*len}
        else
            area = {PI*dia*dia}
        end

        // Set Nt to Gbar/Gmax, where Gmax is max conductance per pore
        setfield . Nt {{getfield . gdens}*area/{getfield . Gmax}}
    end
    return 0
end

/* function used to extend the CREATE action of the squid_markov prototypes */
function squid_markov_CREATE(action, parent, object, elm)
    float area, len, dia
    float PI = 3.14159
        if ({isa compartment ..})
            len = {getfield .. len}
            dia = {getfield .. dia}

            if (len > 1.0e-12)
                area = {PI*dia*len}
            else
                area = {PI*dia*dia}
            end
            // Set Nt to Gbar/Gmax, where Gmax is max conductance per pore
            setfield . Nt {{getfield . gdens}*area/{getfield . Gmax}}
        end
    return 1    // indicate that CREATE action is complete
end

/* function used to extend the SET action of the squid_markov prototypes */
function squid_markov_SET(action, field, newvalue)
    float area, len, dia, newvalue
    float PI = 3.14159

    if ((field == "gdens") || (field == "Gmax"))
        if ({isa compartment ..})
            len = {getfield .. len}
            dia = {getfield .. dia}

            if (len > 1.0e-12)
                area = {PI*dia*len}
            else
                area = {PI*dia*dia}
            end
        end
        // Set Nt to Gbar/Gmax, where Gmax is max conductance per pore
        setfield . Nt {{getfield . gdens}*area/{getfield . Gmax}}
    end
    return 0    // indicate that SET action isn't yet complete
end

// stochastic Na channel, with parameters like those in hhchan.g Na_squid_hh
function make_Na_squid_markov
    str chanpath = "Na_squid_markov"
    if ({argc} == 1)
       chanpath = {argv 1}
    end
    if ({exists {chanpath}})
       return
    end
    float NaGmax = 20.0e-12      // 20 pS/channel
    float gdens = 1200           // S/m^2

    create Napores {chanpath}
    addfield {chanpath} gdens
    setfield {chanpath} Ek {ENA} Gmax {NaGmax} gdens {gdens} \
        Nt {SOMA_A*gdens/NaGmax} \
        malpha_A {0.1e6*(0.025 + EREST_ACT)}  malpha_B -0.1e6 malpha_C -1.0 \
        malpha_D {-1.0*(0.025 + EREST_ACT)} malpha_F -0.01  \
        mbeta_A 4.0e3  mbeta_B 0.0 mbeta_C 0.0 mbeta_D {-1.0*EREST_ACT}  \
        mbeta_F 18e-3 halpha_A 70.0 halpha_B 0.0 halpha_C 0.0 \
        halpha_D  {-1.0*EREST_ACT} halpha_F 0.02 \
        hbeta_A 1.0e3 hbeta_B 0.0 hbeta_C 1.0 \
        hbeta_D {-1.0*(0.030 + EREST_ACT)} hbeta_F -10.0e-3
    // Extend actions to properly scale Nt from gdens and compt area
    addaction {chanpath} RESET squid_markov_RESET
    addaction {chanpath} SET squid_markov_SET
    addaction {chanpath} CREATE squid_markov_CREATE
    // Make an extended object
    addobject {chanpath} {chanpath}  -author "Dave Beeman" \
    -description "Markov HH squid channel based on the Napore object"
end

// stochastic K channel, with parameters like those in hhchan.g K_squid_hh
function make_K_squid_markov
    str chanpath = "K_squid_markov"
    if ({argc} == 1)
       chanpath = {argv 1}
    end
    if ({exists {chanpath}})
       return
    end
    float KGmax = 20.0e-12      // 20 pS/channel
    float gdens = 360           // S/m^2

    create Kpores {chanpath}
    addfield {chanpath} gdens
    setfield {chanpath} Ek {EK} Gmax {KGmax} gdens {gdens} \
        Nt {SOMA_A*gdens/KGmax} \
        alpha_A {10e3*(0.01 + EREST_ACT)} alpha_B -10.0e3 alpha_C -1.0 \
        alpha_D {-1.0*(0.01 + EREST_ACT)} alpha_F -0.01 beta_A 125.0 \
        beta_B 0.0 beta_C 0.0 beta_D {-1.0*EREST_ACT} beta_F 80.0e-3
    // Extend actions to properly scale Nt from gdens and compt area
    addaction {chanpath} RESET squid_markov_RESET
    addaction {chanpath} SET squid_markov_SET
    addaction {chanpath} CREATE squid_markov_CREATE
    // Make an extended object
    addobject {chanpath} {chanpath}  -author "Dave Beeman" \
    -description "Markov HH squid channel based on the Kpore object"
end

/*  make_markov_compt creates a prototype compartment markov_compt in the
    current working element (usually it will be /library), containing the
    channels markov_compt/Na_squid_markov and markov_compt/K_squid_markov.
    This allows these channels to be used with readcell and the "*compt"
    option.  This is necessary when using channel objects that are not
    recognized by readcell (such as the pore objects), or to simplify the cell
    parameter file when creating many compartments that have the same set of
    channels.

    The compartment fields len, dia, Cm, Rm, Ra, and Em are set from the
    prototype dimensions and the global variables CM, RM, RA, and EREST_ACT.
    GENESIS does not declare these globals when started, although they will be
    declared when readcell is invoked if they do not already exist.
    Default values of CM, RM, RA, and EREST_ACT are given at the start
    of this file, in order to intialize the prototype compartment, and can be
    overridden by values specified in the cell parameter file.

    make_markov_compt also assumes that the extended objects used to create
    the Markov channels have been created by invoking make_Na_squid_markov and
    make_K_squid_markov.  As readcell will not scale the maximum conductance
    of these channels, the gdens field of these channels should be set if it
    is different than the defaults given here.  Then, the extended RESET
    action of the channels will set Nt (proportional to the maximum
    conductance) using gdens and the compartment area that is assigned by
    readcell.
*/

function make_markov_compt
    str comptpath = "markov_compt"
    if ({argc} == 1)
       comptpath = {argv 1}
    end
    if ({exists {comptpath}})
       return
    end
    float len, dia, surf
    float PI = 3.14159
    // default dimensions of the compartment
    len = 30.0e-6
    dia = 30.0e-6
    surf = {len*dia*PI}
    create compartment {comptpath}
    setfield {comptpath} Cm {CM*surf}  Rm {RM/surf} \
        Ra {4.0*RA*len/(dia*dia*PI)} \
        Em {EREST_ACT} dia {dia} len {len}

    /* Create the prototype channels with the properly scaled
       Nt and messages to/from the compartment */

    create Na_squid_markov {comptpath}/Na_squid_markov
    addmsg {comptpath} {comptpath}/Na_squid_markov VOLTAGE Vm
    addmsg {comptpath}/Na_squid_markov {comptpath} CHANNEL Gk Ek
    setfield {comptpath}/Na_squid_markov gdens 1200

    create K_squid_markov {comptpath}/K_squid_markov
    addmsg {comptpath} {comptpath}/K_squid_markov VOLTAGE Vm
    addmsg {comptpath}/K_squid_markov {comptpath} CHANNEL Gk Ek
    setfield {comptpath}/K_squid_markov gdens 360
end
