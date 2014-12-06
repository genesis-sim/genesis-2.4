/*======================================================================
  A GENESIS GUI for network models, with a  control panel, a graph with
  axis scaling, and a network view to visualize Vm in each cell
  ======================================================================*/

//=========================================
//      Function definitions used by GUI
//=========================================

function overlaytoggle(widget)
    str widget
    setfield /##[TYPE=xgraph] overlay {getfield {widget} state}
end

function change_stepsize(dialog)
   str dialog
   dt =  {getfield {dialog} value}
   setclock 0 {dt}
   echo "Changing step size to "{dt}
end

function change_runtime(dialog)
   str dialog
   tmax =  {getfield {dialog} value}
end

function inj_toggle // toggles current injection ON/OFF
    if ({getfield /control/injtoggle state} == 1)
        setfield /injectpulse level1 1.0        // ON
    else
        setfield /injectpulse level1 0.0        // OFF
    end
end

function add_injection
   int cell_no
   cell_no = ({getfield /control/cell_no value})
   if (cell_no > {Ex_NX*Ex_NY-1})
      echo "There are only "{Ex_NX*Ex_NY}" excitatory cells - numbering begins with 0"
      return
   end
   InjCell = cell_no
   if (({getmsg /injectpulse/injcurr -outgoing -count}) > 0)
      deletemsg /injectpulse/injcurr  0 -outgoing      // only outgoing message
   end
   addmsg /injectpulse/injcurr /Ex_layer/Ex_cell[{cell_no}]/soma INJECT output
   echo "Current injection is to cell number "{cell_no}
end

function set_injection
   str dialog = "/control"
   set_inj_timing {getfield {dialog}/injectdelay value}  \
       {getfield {dialog}/width value} {getfield {dialog}/interval value}
   setfield /injectpulse/injcurr gain {getfield {dialog}/inject value}
   echo "Injection current = "{getfield {dialog}/inject value}
   echo "Injection pulse delay = "{getfield {dialog}/injectdelay value}" sec"
   echo "Injection pulse width = "{getfield {dialog}/width value}" sec"
   echo "Injection pulse interval = "{getfield {dialog}/interval value}" sec"
end

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

/* Add some interesting colors to any widgets that have been created */
function colorize
    setfield /##[ISA=xlabel] fg white bg blue3
    setfield /##[ISA=xbutton] offbg rosybrown1 onbg rosybrown1
    setfield /##[ISA=xtoggle] onfg red offbg cadetblue1 onbg cadetblue1
    setfield /##[ISA=xdialog] bg palegoldenrod
    setfield /##[ISA=xgraph] bg ivory
end


//==================================
//    Functions to set up the GUI
//==================================

function make_control
    create xform /control [0,50,270,540]
    pushe /control
    create xlabel label -hgeom 25 -bg cyan -label "CONTROL PANEL"
    create xbutton RESET -wgeom 25%       -script reset
    create xbutton RUN  -xgeom 0:RESET -ygeom 0:label -wgeom 25% \
         -script step_tmax
    create xbutton STOP  -xgeom 0:RUN -ygeom 0:label -wgeom 25% \
         -script stop
    create xbutton QUIT -xgeom 0:STOP -ygeom 0:label -wgeom 25% -script quit
    create xdialog stepsize -title "dt (sec)" -value {dt} \
                -script "change_stepsize <widget>"
    create xdialog runtime -title "runtime (sec)" -value {tmax} \
                -script "change_runtime <widget>"
    create xtoggle overlay   -script "overlaytoggle <widget>"
    setfield overlay offlabel "Overlay OFF" onlabel "Overlay ON" state 0
    create xlabel connlabel -label "Connection Parameters"
    // Assume Ex_cell and Inh_cell have the same ex and inh gmax
    create xdialog Ex_gmax -label "Ex gmax (nS)" \
         -value {Ex_ex_gmax*1e9} -script "set_ex_gmax  <v>"
    create xdialog Inh_gmax -label "Inh gmax (nS)" \
          -value {Ex_inh_gmax*1e9} -script "set_inh_gmax  <v>"
    create xdialog weight -label "Weight" inject  \
	-value {syn_weight} -script "set_weights <v>"
    create xdialog propdelay -label "Prop delay (sec/m)" \
	-value {prop_delay}  -script "set_delays <v>"
    create xlabel stimlabel -label "Stimulation Parameters"
    create xtoggle injtoggle -label "" -script inj_toggle
    setfield injtoggle offlabel "Current Injection OFF"
    setfield injtoggle onlabel "Current Injection ON" state 0
    inj_toggle     // initialize
    create xlabel numbering -label "Lower Left = 0; Center = "{middlecell}
    create xdialog cell_no -label "Inject Cell:" \
	-value {InjCell} -script "add_injection"
    create xdialog inject -label "Injection (Amp)" -value {injcurrent}  \
        -script "set_injection"
    create xdialog injectdelay -label "Delay (sec)" \
	 -value {injdelay}   -script "set_injection"
    create xdialog width -label "Width (sec)" \
	-value {injwidth}  -script "set_injection"
    create xdialog interval -label "Interval (sec)" -value {injinterval}  \
        -script "set_injection"
    create xlabel randact -label "Random background activation"
    create xdialog randfreq -label "Frequency (Hz)" -value 0 \
	-script "set_frequency <v>"
    pope
    xshow /control
end

function make_Vmgraph
    float vmin = -0.1
    float vmax = 0.15
    create xform /data [275,50,400,540]
    create xlabel /data/label -hgeom 5% -label {graphlabel}
    create xgraph /data/voltage -hgeom 70% -title "Ex_cell Membrane Potential" -bg white
    setfield ^ XUnits sec YUnits V
    setfield ^ xmax {tmax} ymin {vmin} ymax {vmax}
    makegraphscale /data/voltage

    create xgraph /data/Inh_voltage -hgeom 25% -ygeom 0:voltage \
	-title "Inh_cell Membrane Potential" -bg white
    setfield ^ XUnits sec YUnits V
    setfield ^ xmax {tmax} ymin {vmin} ymax {vmax}
    makegraphscale /data/Inh_voltage

    /* Set up plotting messages, with offsets */
    // middlecell is a middle point (exactly, if NX and NY are odd)
    addmsg /Ex_layer/Ex_cell[{middlecell}]/soma /data/voltage PLOTSCALE \
	Vm *"center "{middlecell} *black 1 0
    addmsg /Ex_layer/Ex_cell[{{round {(Ex_NY-1)/2}}*Ex_NX}/soma  /data/voltage \
        PLOTSCALE Vm *"R edge "{{round {(Ex_NY-1)/2}}*Ex_NX} *blue  1 0.05
    addmsg /Ex_layer/Ex_cell[0]/soma /data/voltage PLOTSCALE \
	Vm *"LL corner 0" *red 1 0.1

    // this is a temporary hack
    addmsg /Inh_layer/Inh_cell[0]/soma /data/Inh_voltage PLOT \
	Vm *"LL corner 0" *magenta
    xshow /data
end

function make_netview  // sets up xview widget to display Vm of each cell
    // Adjust the aspect ratio for rectangular networks of width around 400
    // Make view for Ex_cell[]
    int npixels = {trunc {400/Ex_NX}}
    int Ex_view_width = npixels*Ex_NX
    int Ex_view_height = npixels*Ex_NY

    create xform /Ex_netview [680,50,{Ex_view_width}, {Ex_view_height}]
    create xdraw /Ex_netview/draw [0%,0%,100%, 100%]
    // Make the display region a little larger than the cell array
    setfield /Ex_netview/draw xmin {-Ex_SEP_X} xmax {Ex_NX*Ex_SEP_X} \
	ymin {-Ex_SEP_Y} ymax {Ex_NY*Ex_SEP_Y}
    create xview /Ex_netview/draw/view
    setfield /Ex_netview/draw/view path /Ex_layer/Ex_cell[]/soma field Vm \
	value_min -0.08 value_max 0.03 viewmode colorview sizescale {Ex_SEP_X}
    xshow /Ex_netview

    int Inh_view_width = npixels*Inh_NX
    int Inh_view_height = npixels*Inh_NY
    create xform /Inh_netview [680,{70 + Ex_view_height}, \
	{Inh_view_width + npixels}, {Inh_view_height}]
    create xdraw /Inh_netview/draw [0%,0%,100%, 100%]
    // Make the display region a little larger than the cell array
    setfield /Inh_netview/draw xmin {-Inh_SEP_X} xmax {Inh_NX*Inh_SEP_X} \
	ymin {-Inh_SEP_Y} ymax {Inh_NY*Inh_SEP_Y}

    create xview /Inh_netview/draw/view
    setfield /Inh_netview/draw/view path /Inh_layer/Inh_cell[]/soma field Vm \
	value_min -0.08 value_max 0.03 viewmode colorview sizescale {Inh_SEP_X}
    xshow /Inh_netview

end
