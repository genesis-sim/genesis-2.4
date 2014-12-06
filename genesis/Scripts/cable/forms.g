//genesis
// forms.g - for the cable tutorial

include xtools.g 
include helpforms.g 

// ==========================================
//   Functions used for popup parameter menus
// ==========================================

// Popup Menu for timing, value and location of injection current
function make_inject_menu// Dialog boxes for injection current and timing

    create xform /output/inject_menu [695,340,330,215] -nolabel
    pushe /output/inject_menu
    // in inputs.g
    create xtoggle injtoggle -title "" -script inj_toggle
    setfield injtoggle offlabel "Current Injection OFF"
    setfield injtoggle onlabel "Current Injection ON" state 1
    // initialize
    inj_toggle
    create xbutton DISMISS  -script "xhide /output/inject_menu"
    create xdialog inject -title "Current (uA)" -value {injcurrent}  \
        -script "set_injection"
    create xdialog delay -title "Delay (msec)" -value {injdelay}  \
        -script "set_injection"
    create xdialog width -title "Width (msec)" -value {injwidth}  \
        -script "set_injection"
    create xlabel numbering -title "Compartment #0 = Soma"
    create xdialog comp_no -title "Inject Compartment:" -value 0  \
        -script "add_injection"
    pope
end

// Popup Menu for location and parameters for synaptic input
function make_synapse_menu
    create xform /output/synapse_menu [365,45,330,240] -nolabel
    pushe /output/synapse_menu
    // in inputs.g
    create xtoggle syntoggle -title "" -script syn_toggle
    setfield syntoggle offlabel "Synaptic Input OFF"
    // initially  OFF
    setfield syntoggle onlabel "Synaptic Input ON" state 0
    // initialize
    syn_toggle
    create xbutton DISMISS  -script  "xhide /output/synapse_menu"
    create xdialog delay -title "Delay (msec)" -value {syndelay}  \
        -script "set_inputs"
    create xdialog gsyn -title "gsyn (mS)" -value {gsyn} -script  \
        "set_inputs"
    create xdialog tsyn -title "tsyn (msec)" -value {tsyn} -script  \
        "set_inputs"
    create xdialog vsyn -title "Esyn (mV)" -value {Vsyn} -script  \
        "set_inputs"
    create xlabel numbering -title "Compartment #0 = Soma"
    create xdialog comp_no -title "Input to Compartment:" -value 0  \
        -script "add_syn_input"
    pope
end

// Menu for setting the integration method (uses set_method in compartment.g)
function make_method_menu
    create xform /output/method_menu [695,580,330,265] -title  \
        "Integration Method"
    pushe /output/method_menu
    create xbutton DISMISS -script "xhide /output/method_menu"
    create xbutton euler -title "Forward Euler" -script "set_method -1"
    create xbutton exponential -title "Exponential Euler" -script  \
        "set_method 0"
    create xbutton abash2 -title "Adams-Bashforth #2" -script  \
        "set_method 2"
    create xbutton abash3 -title "Adams-Bashforth #3" -script  \
        "set_method 3"
    create xbutton abash4 -title "Adams-Bashforth #4" -script  \
        "set_method 4"
    create xbutton abash5 -title "Adams-Bashforth #5" -script  \
        "set_method 5"
    create xbutton backward -title "Backward Euler" -script  \
        "set_method 10"
    create xbutton crankn -title "Crank-Nicholson" -script  \
        "set_method 11"
    pope
end

// Menu for changing cell parameters
function make_param_menu
    str compartment
    str formpath = "/output/param_menu"
    create xform {formpath} [695,0,330,315] -nolabel
    disable {formpath}
    pushe {formpath}
    create xlabel cell_params -title "Cell Parameters"
    create xbutton DISMISS -script "xhide "{formpath}
    create xlabel specific_rc -title "Specific Resistance/Capacitance"
    create xdialog RM -title "RM (Kohm*cm^2)" -value {RM} -script setRM
    create xdialog CM -title "CM (uF/cm^2)" -value {CM} -script setCM
    create xdialog RA -title "RA (Kohm*cm)" -value {RA} -script setRA
    create xlabel sizes -title "Compartment length/diameter"
    create xdialog soma_l -title "soma length (cm)" -value {soma_l}  \
        -script setsoma_l
    create xdialog soma_d -title "soma diam (cm)" -value {soma_d}  \
        -script setsoma_d
    create xdialog dend_l -title "cable compt length (cm)" -value  \
        {dend_l} -script setdend_l
    create xdialog dend_d -title "cable compt diam (cm)" -value  \
        {dend_d} -script setdend_d
    pope
end

function set_cell_params// uses global RM, CM, RA, len, diam

  int i
  pushe /cell
  setcompartment soma {soma_l} {soma_d} {EREST}
  echo
  echo "Soma Rm (membrane resistance, Kohms) = "{getfield soma Rm}
  echo "Soma Cm (membrane capacitance, uF)   = "{getfield soma Cm}
  echo "Soma Ra (axial resistance, Kohms)    = "{getfield soma Ra}
  if ({NCableSects} != 0)
     for (i = 0; i < {NCableSects}; i = i + 1)
       setcompartment /cell/cable[{i}] {dend_l} {dend_d} {EREST}
     end
  echo  \
      "Cable compartment Rm (membrane resistance, Kohms) = "{getfield cable[0] Rm}
  echo  \
      "Cable compartment Cm (membrane capacitance, uF)   = "{getfield cable[0] Cm}
  echo  \
      "Cable compartment Ra (axial resistance, Kohms)    = "{getfield cable[0] Ra}
  echo "Total cable length = "{{NCableSects}*{dend_l}}  \
      " cm. ("{NCableSects}" sections)"
  end  // if

  pope
  //  if we set the xcell fatfield to calc_dia, we need to remake the widget
  //  however, propagation_toggle is defined later in this script
  //  set /output/control/propagation state 0
  //  propagation_toggle
end

// The following six specialized functions represent an offensively ugly
// kludge to deal with the way the "-script" field is parsed by GENESIS 1.4

function setRM
  RM = {getfield /output/param_menu/RM value}
  echo "RM now set to "{RM}
  set_cell_params
end

function setCM
  CM = {getfield /output/param_menu/CM value}
  echo "CM now set to "{CM}
  set_cell_params
end

function setRA
  RA = {getfield /output/param_menu/RA value}
  echo "RA now set to "{RA}
  set_cell_params
end

function setsoma_l
  soma_l = {getfield /output/param_menu/soma_l value}
  echo "soma_l now set to "{soma_l}
  set_cell_params
end

function setsoma_d
  soma_d = {getfield /output/param_menu/soma_d value}
  echo "soma_d now set to "{soma_d}
  set_cell_params
end

function setdend_l
  dend_l = {getfield /output/param_menu/dend_l value}
  echo "dend_l now set to "{dend_l}
  set_cell_params
end

function setdend_d
  dend_d = {getfield /output/param_menu/dend_d value}
  echo "dend_d now set to "{dend_d}
  set_cell_params
end

// Popup menu for adding/removing cable Vm plots
function make_plot_menu
    create xform /output/plot_menu [215,50,220,120] -title  \
        "Add/Remove Plots"
    pushe /output/plot_menu
    create xbutton DISMISS -script "xhide /output/plot_menu"
    create xbutton remove -title "Remove Cable Plots" -script  \
        "remove_cable_plots"
    create xdialog add -title "Plot Compt. #" -value 0 -script  \
        "add_cable_plot"
    pope
end

//functions used to add and remove cable plots
function add_cable_plot
  // cable section to plot (numbered from 1)
  int section
  section = ({getfield /output/plot_menu/add value})
  if ({exists /cell/cable[{section-1}]/logtable})
     echo "Plot already exists!"
     return
  end
  if ((section) < 1)
     echo "In our notation, the first cable compartment is #1."
     return
  end

  if ((section) > ({NCableSects}))
    echo "There are only "{NCableSects}" cable compartments!"
  else
    copy /logtable /cell/cable[{section-1}]/logtable
    pushe /output/somagraphs
    addmsg /cell/cable[{section - 1}] somaVm_grf PLOT Vm  \
        *"cable_"{section} *red
    addmsg /cell/cable[{section-1}] /cell/cable[{section-1}]/logtable \
        INPUT Vm
    addmsg /cell/cable[{section-1}]/logtable  loggraph \
        PLOT output  *"cable_"{section} *red
    pope
    echo "Vm plots added for cable compartment #"{section}
    // needed to initialize the table
    reset
  end
end

function remove_cable_plots
  int i, n, nmsg
  for (i = 0; i < {NCableSects}; i = i + 1)
    if ({exists /cell/cable[{i}]/logtable})
     // then plot messages exist for cable[{i}]
     // To play it safe, we don't assume we know the message number
     //loop over outgoing messages
     nmsg = {getmsg /cell/cable[{i}] -outgoing -count}
     for (n = 0; n < nmsg; n = n + 1)
      if ({getmsg /cell/cable[{i}] -outgoing -destination {n}} == "/output/somagraphs/somaVm_grf")
echo deleting message {n}
       deletemsg /cell/cable[{i}]  {n} -outgoing // should be"PLOT Vm" message
       break       // bail out of "for" loop - we found the message
      end      // if
     end     // for n
     delete /cell/cable[{i}]/logtable // get rid of table and all its messages
    end    //if exists
  end  // for i

  setfield /output/plot_menu/add value 0
  echo "Plotting of Vm for cable compartments disabled."
end

// functions used in MAIN CONTROL PANEL

// change_stepsize  -  sets clock 0

function change_stepsize
    dt = {getfield /output/control/stepsize value}
    setclock 0 {dt}
    echo dt = {dt}
    // re-makes solver, if needed
    make_hsolve
end

// toggle the display of channel conductance
function conductance_toggle
  if ({getfield /output/control/conductance state} == 1)
      xshowontop /output/condgraphs
  else
      xhide /output/condgraphs
  end
end

// toggle the display of Vm along the cable
function propagation_toggle
  if ({getfield /output/control/propagation state} == 1)
      make_draw {{NCableSects} + 1}
      xshowontop /viewform
  else
      if ({exists /viewform/draw})
          delete_draw
      end
      xhide /viewform
  end
end

// =======================================================
//  The two main functions, loadgraphs and load_menus
//  set up the forms used for graphs and control buttons
// =======================================================
function loadgraphs
     // activate XODUS
     
    // give it color
    xcolorscale hot

    // make graphs for soma Vm
    create xform /output/somagraphs [215,0,480,600] -nolabel
    pushe /output/somagraphs
//    create xgraph somaVm_grf [0,0,100%,50%] -title  \
    create xgraph somaVm_grf [0,0,100%,300] -title  \
        "Soma membrane potential Vm (mV)"
    setfield somaVm_grf xmin 0 xmax {tmax} ymin 0 ymax 25 XUnits "msec"  \
        YUnits "Vm (mV)"
    useclock somaVm_grf 1
    makegraphscale /output/somagraphs/somaVm_grf

//    create xgraph loggraph [0,50%,100%,50%] -title "natural log of Vm"
    create xgraph loggraph [0,300,100%,300] -title "natural log of Vm"
    setfield loggraph xmax {tmax} ymin -1 ymax 4 XUnits "msec"  \
        YUnits "ln(Vm)"
    useclock loggraph 1
    makegraphscale /output/somagraphs/loggraph
    pope
    xshow /output/somagraphs

    // make a graph for conductances
    create xform /output/condgraphs [215,320,480,300] -nolabel
    pushe /output/condgraphs
    create xgraph channel_Gk [0,0,100%,100%] -title  \
        "Channel Conductance (mS)"
    setfield channel_Gk xmin 0 xmax {tmax} ymin 0 ymax {{gsyn}*2}
    setfield channel_Gk XUnits "msec" YUnits "Gk (mS)"
    useclock channel_Gk 1
    makegraphscale /output/condgraphs/channel_Gk
    pope
end// loadgraphs


    // =================================================
    //                MAIN CONTROL PANEL
    // =================================================
function make_control_menu
    create xform /output/control [0,0,215,410] -nolabel
    pushe /output/control
    create xbutton HELP -script "xshowontop /output/helpmenu"
    create xbutton resetdefaults -title "Reset Defaults" -script  \
        "ResetDefaults"
    create xbutton stop -title STOP -script stop
    create xbutton quit -title QUIT -script "quitbutton <widget>"
    makeconfirm quit "Quit CABLE?" "quit" "cancelquit <widget>" 10 10

    create xtoggle overlay -title "" -script "overlaytoggle <widget>"
    setfield overlay offlabel "Overlay OFF" onlabel "Overlay ON" state 0
    overlaytoggle /output/control/overlay
    // be sure that it is in state 0 after Reset Defaults
    create xbutton reset -title RESET -script reset
    create xdialog step -title "STEP (msec)" -script  \
        "stepbutton <widget>" -value {tmax}
    create xdialog stepsize -title "dt" -script "change_stepsize"  \
        -value {dt}
    create xbutton params -title "Change Cell Parameters" -script  \
        "xshow /output/param_menu"
    create xbutton inject -title "Change Current Injection" -script  \
        "xshow /output/inject_menu"
    create xbutton synapse -title "Change Synaptic Input" -script  \
        "xshow /output/synapse_menu"
    create xbutton method -title "Change Integration Method" -script  \
        "xshow /output/method_menu"
    // defined in addcable.g
    create xdialog nsections -title "Cable Compts." -value 0 -script  \
        "add_cable"
    create xbutton addplot -title "Add/Remove Plots" -script  \
        "xshow /output/plot_menu"

    create xtoggle conductance -title "" -script conductance_toggle
        setfield conductance offlabel "Conductance Plot Hidden"
	setfield conductance onlabel "Conductance Plot Visible" state 0
	// be sure it is properly initialized
	conductance_toggle

//    create xtoggle propagation -title "" -script propagation_toggle
//        setfield propagation offlabel "Vm Propagation Hidden"
//	setfield propagation onlabel "Vm Propagation Visible" state 0
	// be sure it is properly initialized
//	propagation_toggle
    pope
end // make_control_menu

function load_menus
    make_control_menu
    make_param_menu
    make_inject_menu
    make_synapse_menu
    make_method_menu
    make_plot_menu
    xshow /output/control
end
