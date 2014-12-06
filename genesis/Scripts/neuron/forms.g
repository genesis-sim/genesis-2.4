//genesis
// forms.g for Neuron tutorial

include xtools.g
include helpforms.g 

// =================================================
//   Functions used for setting dendrite inputs
// =================================================
function togglesource(channel, widget)
// toggles channel input between source A/B
    str channel, widget
    if ({getfield {widget} state} == 1)    // source B

        setfield /cell/{channel} \
            synapse[1].weight {getfield /cell/{channel} synapse[0].weight}
        setfield /cell/{channel} synapse[0].weight 0
    else    // source A
        setfield /cell/{channel} \
            synapse[0].weight {getfield /cell/{channel} synapse[1].weight}
        setfield /cell/{channel} synapse[1].weight 0
    end
end

function setweight(channel, widget)
    str channel, widget
    if ({getfield {widget}/source state} == 1)    // source B
    setfield /cell/{channel} \
        synapse[1].weight {getfield {widget} value}
    else
    setfield /cell/{channel} \
        synapse[0].weight {getfield {widget} value}
    end
end

// =================================================
//   Functions used for popup parameter menus
// =================================================
function make_comp_menu(compartment)
    str compartment
    str formpath = "/output/popups/" @ compartment
    str comppath = "/cell/" @ compartment
    create xform {formpath} [720,50,255,395] -title  \
        {compartment}" parameters"
    disable {formpath}
    pushe {formpath}
    create xbutton DONE [0,40,78,30] -script "xhide "{formpath}
    create xbutton Ex_button [80,40,78,30] -title "Exc. Ch." -script  \
        "xshowontop "{formpath}"/Ex_channel"
    create xbutton Inh_button [160,40,78,30] -title "Inh. Ch." -script  \
        "xshowontop "{formpath}"/Inh_channel"
    create xdialog Em -title "Em (mv)" -value {getfield {comppath} Em}  \
        -script "fieldbutton "{comppath}" Em "{formpath}/Em
    create xdialog Rm -title "Rmem (KOhm)" -value  \
        {getfield {comppath} Rm} -script "set_comp_fields "{compartment}
    create xdialog Cm -title "Cmem (uF)" -value  \
        {getfield {comppath} Cm} -script "set_comp_fields "{compartment}
    create xdialog Ra -title "Raxial (KOhm)" -value  \
        {getfield {comppath} Ra} -script "set_comp_fields "{compartment}
    create xlabel specific_rc -title "Specific Resistance/Capacitance"
    create xdialog RM -title "RM (Kohm*cm^2)" -value {rm} -script  \
        "set_comp_params "{compartment}
    create xdialog CM -title "CM (uF/cm^2)" -value {cm} -script  \
        "set_comp_params "{compartment}
    create xdialog RA -title "RA (Kohm*cm)" -value {ra} -script  \
        "set_comp_params "{compartment}
    create xlabel sizes -title "Compartment length/diameter"
    create xdialog length -title "length (cm)" -value {dend_l} -script  \
        "set_comp_params "{compartment}
    create xdialog diam -title "diam (cm)" -value {dend_d} -script  \
        "set_comp_params "{compartment}
    if (compartment == "soma")
	setfield length value {soma_l}
	setfield diam value {soma_d}
    end
    pope
end

function make_channel_menu(compartment, channel)
    str compartment, channel
    // e. g. "soma/Ex_channel"
    str comp_and_chan = compartment @ "/" @ channel
    str formpath = "/output/popups/" @ comp_and_chan
    str chanpath = "/cell/" @ comp_and_chan
    create xform {formpath} [770,90,255,230] -title  \
        {comp_and_chan}" parameters"
    disable {formpath}
    pushe {formpath}
    create xbutton DONE [0,40,240,30] -script "xhide "{formpath}
    if ((compartment) == "soma")
       create xdialog Ek [0,75,240,30] -title "Eequil (mV)" -value  \
           {getfield {chanpath} Ek} -script  \
           "set_ch_params "{compartment}" "{channel}
       create xdialog gbar [0,110,240,30] -title "Gbar (mS)" -value  \
           {getfield {chanpath} Gbar} -script  \
           "set_ch_params "{compartment}" "{channel}
    else
       create xdialog Ek [0,75,240,30] -title "Esyn (mV)" -value  \
           {getfield {chanpath} Ek} -script  \
           "set_ch_params "{compartment}" "{channel}
       create xdialog gmax [0,110,240,30] -title "gmax (mS)" -value  \
           {getfield {chanpath} gmax} -script  \
           "set_ch_params "{compartment}" "{channel}

       create xdialog tau1 [0,145,240,30] -title "Tau 1 (msec)" -value  \
           {getfield {chanpath} tau1} -script  \
           "set_ch_params "{compartment}" "{channel}
       create xdialog tau2 [0,180,240,30] -title "Tau 2 (msec)" -value  \
           {getfield {chanpath} tau2} -script  \
           "set_ch_params "{compartment}" "{channel}
    end
    pope
end

function set_ch_params(compartment, channel)
    str compartment, channel
    // e. g. "soma/Ex_channel"
    str comp_and_chan = compartment @ "/" @ channel
    str formpath = "/output/popups/" @ comp_and_chan
    str chanpath = "/cell/" @ comp_and_chan
    fieldbutton {chanpath} Ek {formpath}/Ek
    if ((compartment) == "soma")
       fieldbutton {chanpath} Gbar {formpath}/gbar
    else
       fieldbutton {chanpath} gmax {formpath}/gmax
       fieldbutton {chanpath} tau1 {formpath}/tau1
       fieldbutton {chanpath} tau2 {formpath}/tau2
    end
end

function set_comp_fields(compartment)
    str compartment
    str formpath = "/output/popups/" @ compartment
    str comppath = "/cell/" @ compartment
    float length, diam
    length = ({getfield {formpath}/length value})
    diam = ({getfield {formpath}/diam value})
    setfield {formpath}/RM  \
        value {{getfield {formpath}/Rm value}*(length*diam*PI)}
    setfield {formpath}/CM  \
        value {{getfield {formpath}/Cm value}/(length*diam*PI)}
    setfield {formpath}/RA  \
        value {{getfield {formpath}/Ra value}*(diam*diam*PI)/(4*length)}
    fieldbutton {comppath} Rm {formpath}/Rm
    fieldbutton {comppath} Cm {formpath}/Cm
    fieldbutton {comppath} Ra {formpath}/Ra
end

function set_comp_params(compartment)
    str compartment
    str formpath = "/output/popups/" @ compartment
    str comppath = "/cell/" @ compartment
    float length, diam
    length = ({getfield {formpath}/length value})
    diam = ({getfield {formpath}/diam value})
    setfield {formpath}/Rm  \
        value {{getfield {formpath}/RM value}/(length*diam*PI)}
    setfield {formpath}/Cm  \
        value {{getfield {formpath}/CM value}*(length*diam*PI)}
    setfield {formpath}/Ra  \
        value {{getfield {formpath}/RA value}*(4*length)/(diam*diam*PI)}
    fieldbutton {comppath} Rm {formpath}/Rm
    fieldbutton {comppath} Cm {formpath}/Cm
    fieldbutton {comppath} Ra {formpath}/Ra
end

function set_input_timing// uses setspikes from inputs.g

   str dialog = "/output/popups/inputs"
   setinjection {getfield {dialog}/Idelay value}  \
       {getfield {dialog}/Iwidth value}
   setspikes Apulse {getfield {dialog}/Adelay value}  \
       {getfield {dialog}/Awidth value}  \
       {getfield {dialog}/Ainterval value}
   setspikes Bpulse {getfield {dialog}/Bdelay value}  \
       {getfield {dialog}/Bwidth value}  \
       {getfield {dialog}/Binterval value}
   // set spike amplitudes to 1.0/dt
end

// functions used in MAIN CONTROL PANEL

// change_stepsize  -  sets clock 0 and properly normalizes spike plots

function change_stepsize
    float dt = ({getfield /output/control/stepsize value})
    setclock 0 {dt}
    echo dt = {dt}
end

// =======================================================
//  The two main functions, loadgraphs and loadcontrol
//  set up the forms used for graphs and control buttons
// =======================================================
function loadgraphs
    // activate XODUS
    
    // While forms are being loaded, put up some credits
     echo " "
     echo "*************************************************"
     echo "*                                               *"
     echo "*                   NEURON                      *"
     echo "*                                               *"
     echo "*        A simulation of a simple neuron        *"
     echo "*                                               *"
     echo "*                 D. Beeman                     *"
     echo "*                                               *"
     echo "*            Harvey Mudd College                *"
     echo "*                                               *"
     echo "*************************************************"
     echo " "
    // give it color
    xcolorscale hot

    // =================================================
    //                     GRAPHICS
    // =================================================
    /* \
         NOTE - Another undocumented feature: The neutral element /output is
   prexisting, so I can create a child "/output/dend1graphs" without explictly
   creating the parent. */

    create xform /output/dend1graphs [10,10,500,675] -nolabel
    pushe /output/dend1graphs
    //parameter to makegraphscale
    str graphicspath = "/output/dend1graphs"
    // =================================================
    //      make input_grf for Input/Output Voltages
    // =================================================
    create xgraph input_grf [0,0,100%,24%] -title "Inputs to Cell"
    setfield input_grf xmin 0 xmax 80 ymin 0 ymax 6 XUnits "t (msec)"
    useclock input_grf 0    // spikes need faster clock than other plots
    makegraphscale {graphicspath}/input_grf

    // =================================================
    //       make dend1Gk_grf for synaptic conductances
    // =================================================
    create xgraph dend1Gk_grf [0,0:input_grf.bottom,100%,38%] -title  \
        "Dendrite #1 Gex and Ginh (mS)"
    setfield dend1Gk_grf yoffset 0 YUnits "G (mS)"
    setfield dend1Gk_grf xmin 0 xmax 80 ymin -1e-6 ymax 5e-6  \
        XUnits "t (msec)"
    useclock dend1Gk_grf 2
    makegraphscale {graphicspath}/dend1Gk_grf
    // =================================================
    //      make dend1Vm_grf for intracellular potentials
    // =================================================
    create xgraph dend1Vm_grf [0,0:dend1Gk_grf.bottom,100%,38%] -title  \
        "Dendrite #1 membrane potential Vm (mV)"
    setfield dend1Vm_grf xmin 0 xmax 80 ymin -100 ymax 50  \
        XUnits "t (msec)" YUnits "Vm (mV)"
    useclock dend1Vm_grf 1     // possibly  use fast clock for Vm
    makegraphscale {graphicspath}/dend1Vm_grf
    pope

    // =================================================
    //  make graphs for soma Vm and conductances
    // =================================================
    create xform /output/somagraphs [510,10,500,675] -nolabel
    pushe /output/somagraphs
    create xgraph activation_grf [0,0,100%,24%] -title  \
        "Hodgkin-Huxley Activation Parameters"
    setfield activation_grf yoffset 2
    setfield activation_grf xmin 0 xmax 80 ymin 0 ymax 6  \
        XUnits "t (msec)"
    useclock activation_grf 2
    makegraphscale /output/somagraphs/activation_grf

    create xgraph somaGk_grf [0,0:activation_grf.bottom,100%,38%] -title  \
        "Soma GNa and GK (mS)"
    setfield somaGk_grf yoffset 0 YUnits "G (mS)"
    setfield somaGk_grf xmin 0 xmax 80 ymin 0 ymax 1e-3  \
        XUnits "t (msec)"
    useclock somaGk_grf 2
    makegraphscale /output/somagraphs/somaGk_grf

    create xgraph somaVm_grf [0,0:somaGk_grf.bottom,100%,38%] -title  \
        "Soma membrane potential Vm (mV)"
    setfield somaVm_grf xmin 0 xmax 80 ymin -100 ymax 50  \
        XUnits "t (msec)" YUnits "Vm (mV)"
    useclock somaVm_grf 1
    makegraphscale /output/somagraphs/somaVm_grf
    pope

    // =================================================
    //       make dend2Gk_grf for synaptic conductances
    // =================================================
    create xform /output/dend2graphs [510,10,500,675] -nolabel
    pushe /output/dend2graphs
    //parameter to makegraphscale
    str graphicspath = "/output/dend2graphs"
    create xgraph dend2Gk_grf [0,24%,100%,38%] -title  \
        "Dendrite #2 Gex and Ginh (mS)"
    setfield dend2Gk_grf yoffset 0 YUnits "G (mS)"
    setfield dend2Gk_grf xmin 0 xmax 80 ymin -1e-6 ymax 5e-6  \
        XUnits "t (msec)"
    useclock dend2Gk_grf 2
    makegraphscale {graphicspath}/dend2Gk_grf
    // =================================================
    //      make dend2Vm_grf for intracellular potentials
    // =================================================
    create xgraph dend2Vm_grf [0,0:dend2Gk_grf.bottom,100%,38%] -title  \
        "Dendrite #2 membrane potential Vm (mV)"
    setfield dend2Vm_grf xmin 0 xmax 80 ymin -100 ymax 50  \
        XUnits "t (msec)" YUnits "Vm (mV)"
    useclock dend2Vm_grf 1
    makegraphscale {graphicspath}/dend2Vm_grf
    pope

    xshow /output/dend1graphs
    xshow /output/somagraphs
end// loadgraphs


    // =================================================
    //                MAIN CONTROL PANEL
    // =================================================
function loadcontrol
    create xform /output/control [10,710,1010,155] -nolabel
    pushe /output/control
    create xbutton HELP [0,5,90,30] -script  \
        "xshowontop /output/popups/helpmenu"
    // in cell.g
    create xbutton reset [90,5,90,30] -title RESET -script reset_kludge
    create xbutton stop [180,5,90,30] -title STOP -script stop
    create xbutton quit [270,5,90,30] -title QUIT -script  \
        "quitbutton <widget>"
    makeconfirm quit "Quit Neuron?" "quit" "cancelquit <widget>" 91 730
    // make_cable.g
    create xdialog nsections [180,75,180,30] -title "Cable Compts."  \
        -value 0 -script "add_cable"
    create xtoggle overlay [180,40,90,30] -title "" -script  \
        "overlaytoggle <widget>"
    setfield overlay label0 "Overlay OFF" label1 "Overlay ON" state 0
    overlaytoggle /output/control/overlay
    // be sure that it is in state 0 after resetting to Defaults
    create xbutton inputs [0,40,90,30] -title "Inputs" -script  \
        "xshowontop /output/popups/inputs"
    create xbutton resetdefaults [90,40,90,30] -title "Defaults"  \
        -script "ResetDefaults"
    create xdialog step [0,75,180,30] -title "STEP (msec)" -script  \
        "stepbutton <widget>" -value 80
    create xtoggle plotsoma [270,40,90,30] -title "" -script  \
        "formtoggle <widget> /output/somagraphs /output/dend2graphs"
    setfield plotsoma label0 "Plot Soma" label1 "Plot Dend2" state 0
    create xdialog stepsize [0,110,360,30] -title "dt (msec)" -script  \
        "change_stepsize" -value {getclock 0}
    create xlabel cellparms [835,5,160,30] -title "Cell Parameters"
    create xbutton somaparms [835,40,160,30] -title "Soma" -script  \
        "xshowontop /output/popups/soma"
    create xbutton dend1parms [835,75,160,30] -title "Dendrite 1"  \
        -script "xshowontop /output/popups/dend1"
    create xbutton dend2parms [835,110,160,30] -title "Dendrite 2"  \
        -script "xshowontop /output/popups/dend2"
    pope

    // dialog boxes to set dendrite connections
    str inparms = "/output/control/inparms"
    create xlabel {inparms} [552,5,280,25] -title "Dendrite Inputs"
    pushe {inparms}

    // I need a more general way to do this when there are several dendrites
    // The dialog boxes are initialized to 0 - check inputs.g for consistency

    create xdialog dend1_exwt [552,32,200,25] -title "Dend #1 Exc. Wt." \
         -script "setweight dend1/Ex_channel <widget>" -value 0
    create xtoggle dend1_exwt/source [755,32,77,25] -title "" -script  \
        "togglesource dend1/Ex_channel <widget>"
    setfield dend1_exwt/source label0 "Source A" label1 "Source B"  \
        state 0

    create xdialog dend1_inhwt [552,59,200,25] -title  \
        "Dend #1 Inh. Wt." -script  \
        "setweight dend1/Inh_channel <widget>" -value 0
    create xtoggle dend1_inhwt/source [755,59,77,25] -title "" -script  \
        "togglesource dend1/Inh_channel <widget>"
    // Initialize inhibitory connections to source B
    setfield dend1_inhwt/source label0 "Source A" label1 "Source B"  \
        state 1

    create xdialog dend2_exwt [552,86,200,25] -title "Dend #2 Exc. Wt." \
         -script "setweight dend2/Ex_channel <widget>" -value 0
    create xtoggle dend2_exwt/source [755,86,77,25] -title "" -script  \
        "togglesource dend2/Ex_channel <widget>"
    setfield dend2_exwt/source label0 "Source A" label1 "Source B"  \
        state 0

    create xdialog dend2_inhwt [552,113,200,25] -title  \
        "Dend #2 Inh. Wt." -script  \
        "setweight dend2/Inh_channel <widget>" -value 0
    create xtoggle dend2_inhwt/source [755,113,77,25] -title "" -script \
         "togglesource dend2/Inh_channel <widget>"
    // Initialize inhibitory connections to source B
    setfield dend2_inhwt/source label0 "Source A" label1 "Source B"  \
        state 1

    pope

    // Dialog boxes for injection currents

    create xlabel /output/control/injection [360,5,190,30] -title  \
        "Injection Current (uA)"
    pushe /output/control/injection
    create xdialog soma_inj [360,40,190,30] -title "Soma Inj" -script  \
        "fieldbutton /input/injectpulse/somacurr gain <widget>" -value  \
        {getfield /input/injectpulse/somacurr gain}

    create xdialog dend1_inj [360,75,190,30] -title "Dend 1 Inj"  \
        -script "fieldbutton /input/injectpulse/dend1curr gain <widget>" \
         -value {getfield /input/injectpulse/dend1curr gain}

    create xdialog dend2_inj [360,110,190,30] -title "Dend 2 Inj"  \
        -script "fieldbutton /input/injectpulse/dend2curr gain <widget>" \
         -value {getfield /input/injectpulse/dend2curr gain}

    pope

    // =================================================
    //                   Popup Menus
    // =================================================
    // Popup Menus for Cell Parameters
    create neutral /output/popups
    make_comp_menu soma
    make_channel_menu soma Ex_channel
    make_channel_menu soma Inh_channel
    make_comp_menu dend1
    make_channel_menu dend1 Ex_channel
    make_channel_menu dend1 Inh_channel
    make_comp_menu dend2
    make_channel_menu dend2 Ex_channel
    make_channel_menu dend2 Inh_channel
    // Popup Menu for Timing of Inputs
    // More dependencies on inputs.g - dialog box initial values are hard-coded
    create xform /output/popups/inputs [745,10,275,380] -title  \
        "Input Timing (msec)"
    pushe /output/popups/inputs
    create xbutton DONE [5,40,250,30] -script  \
        "xhide /output/popups/inputs"
    create xlabel sourceA [5,75,100,100] -title "Source A"
    create xdialog Adelay [110,75,150,30] -title "Delay" -value 10  \
        -script "set_input_timing"
    create xdialog Awidth [110,110,150,30] -title "Width" -value 50  \
        -script "set_input_timing"
    create xdialog Ainterval [110,145,150,30] -title "Interval" -value  \
        10 -script "set_input_timing"

    create xlabel sourceB [5,185,100,100] -title "Source B"
    create xdialog Bdelay [110,185,150,30] -title "Delay" -value 20  \
        -script "set_input_timing"
    create xdialog Bwidth [110,220,150,30] -title "Width" -value 50  \
        -script "set_input_timing"
    create xdialog Binterval [110,255,150,30] -title "Interval" -value 10 \
        -script "set_input_timing"

    create xlabel injection [5,295,100,65] -title "Injection"
    create xdialog Idelay [110,295,150,30] -title "Delay" -value 20  \
        -script "set_input_timing"
    create xdialog Iwidth [110,330,150,30] -title "Width" -value 40  \
        -script "set_input_timing"
    pope
    // popup help menu
    xshow /output/control
    // set up the help screens and menu (helpforms.g)
    loadhelp
    //    showintro		// show a big GIF image (in helpforms.g)
end// loadcontrol

