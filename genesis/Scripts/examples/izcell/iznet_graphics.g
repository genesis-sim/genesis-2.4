//genesis - iznet_graphics.g GUI for iznet.g

/*======================================================================
  A GENESIS GUI with a simple control panel and graph, with axis scaling,
  adapted for the "iznet" demo.
  ======================================================================*/

include xtools.g    // functions to make "scale" buttons, etc.

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
   if (cell_no > {NX*NY-1})
      echo "There are only "{NX*NY}" cells - numbering begins with 0"
      return
   end
   InjCell = cell_no
   if (({getmsg /injectpulse/injcurr -outgoing -count}) > 0)
      deletemsg /injectpulse/injcurr  0 -outgoing      // only outgoing message
   end
  addmsg /injectpulse/injcurr /network/cell[{cell_no}] INJECT output
   echo "Current injection is to cell number "{cell_no}
end

function set_injection
   str dialog = "/control"
   set_inj_timing {getfield {dialog}/injectdelay value}  \
       {getfield {dialog}/width value} {getfield {dialog}/interval value}
   setfield /injectpulse/injcurr gain {getfield {dialog}/inject value}
   echo "Injection current = "{getfield {dialog}/inject value}
   echo "Injection pulse delay = "{getfield {dialog}/injectdelay value}" "{tunit}
   echo "Injection pulse width = "{getfield {dialog}/width value}" "{tunit}
   echo "Injection pulse interval = "{getfield {dialog}/interval value}" "{tunit}
end

function set_params
    setfield /network/cell[] A {getfield /control/A value}
    setfield /network/cell[] B {getfield /control/B value}
    setfield /network/cell[] C {getfield /control/C value}
    setfield /network/cell[] D {getfield /control/D value}
    echo "Setting the A B C D parameters to: " {getfield {cell} A} \
         {getfield {cell} B}  {getfield {cell} C}  {getfield {cell} D}
end

//==================================
//    Functions to set up the GUI
//==================================

function make_control
    create xform /control [10,50,250,570]
    pushe /control
    create xlabel label -hgeom 25 -bg cyan -label "CONTROL PANEL"
    create xbutton RESET -wgeom 25%       -script reset
    create xbutton RUN  -xgeom 0:RESET -ygeom 0:label -wgeom 25% \
         -script step_tmax
    create xbutton STOP  -xgeom 0:RUN -ygeom 0:label -wgeom 25% \
         -script stop
    create xbutton QUIT -xgeom 0:STOP -ygeom 0:label -wgeom 25% -script quit
    create xdialog stepsize -title "dt ("{tunit}")" -value {dt} \
                -script "change_stepsize <widget>"
    create xtoggle overlay   -script "overlaytoggle <widget>"
    setfield overlay offlabel "Overlay OFF" onlabel "Overlay ON" state 0
    create xlabel connlabel -label "Connection Parameters"
    create xdialog gmax -label "synchan gmax" -value {gmax} \
	-script "setfield /network/cell[]/Ex_channel gmax <v>"
    create xdialog weight -label "Weight" -wgeom 50% \
	-value {syn_weight} -script "set_weights <v>"
    create xdialog propdelay -label "Delay" -wgeom 50% -xgeom 0:weight \
	-ygeom 0:gmax -value {prop_delay}  -script "set_delays <v>"
    create xdialog abs_refract -value {abs_refract} -script "set_abs_refract <v>"
    create xlabel stimlabel -label "Stimulation Parameters"
    create xtoggle injtoggle -label "" -script inj_toggle
    setfield injtoggle offlabel "Current Injection OFF"
    setfield injtoggle onlabel "Current Injection ON" state 1
    inj_toggle     // initialize
    create xlabel numbering -label "Lower Left = 0; Center = "{middlecell}
    create xdialog cell_no -label "Inject Cell:" -value {InjCell}  \
        -script "add_injection"
    create xdialog inject -label "Injection" -value {injcurrent}  \
        -script "set_injection"
    create xdialog injectdelay -label "Delay ("{tunit}")" -value {injdelay}  \
        -script "set_injection"
    create xdialog width -label "Width ("{tunit}")" -value {injwidth}  \
        -script "set_injection"
    create xdialog interval -label "Interval ("{tunit}")" -value {injinterval}  \
        -script "set_injection"
    create xlabel randact -label "Random background activation"
    create xdialog randfreq -label "Frequency" -value 0 \
	-script "set_frequency <v>"

  create xlabel /control/paramlabel -hgeom 25 -bg cyan -label "Spike Parameters"
  create xdialog /control/A  -wgeom 50% -value {A} -script "set_params"
  create xdialog /control/B  -xgeom 0:A -ygeom 0:paramlabel -wgeom 50% \
                -value {B} -script "set_params"
  create xdialog /control/C  -wgeom 50% \
                -value {C} -script "set_params"
  create xdialog /control/D  -xgeom 0:C -ygeom 0:B -wgeom 50% \
                -value {D} -script "set_params"
    pope
    xshow /control
end

function make_Vmgraph
    float vmin = -0.1*uscale
    float vmax = (0.05 + 0.1)*uscale
    create xform /data [265,50,400,500]
    create xlabel /data/label -hgeom 5% \
	-label "Izhikevich simple spiking neuron model network"
    create xgraph /data/voltage -hgeom 80% -title "Membrane Potential" -bg white
    setfield ^ XUnits {tunit} YUnits V
    setfield ^ xmax {tmax} ymin {vmin} ymax {vmax}
    makegraphscale /data/voltage

    create xgraph /data/injection -hgeom 15% -ygeom 0:voltage \
        -title "Injection" -bg white
    setfield ^ XUnits {tunit} YUnits I
    setfield ^ xmax {tmax} ymin {0} ymax {25}
    makegraphscale /data/injection

    /* Set up plotting messages, with offsets */
    // middlecell is a middle point (exactly, if NX and NY are odd)
    addmsg /network/cell[{middlecell}] /data/voltage PLOTSCALE \
	Vm *"center "{middlecell} *black 1 0
    addmsg /network/cell[{{round {(NY-1)/2}}*NX}  /data/voltage \
        PLOTSCALE Vm *"R edge "{{round {(NY-1)/2}}*NX} *blue 1 {0.05*uscale}
    addmsg /network/cell[0] /data/voltage PLOTSCALE \
	Vm *"LL corner 0" *red 1 {0.1*uscale}
    addmsg /injectpulse/injcurr /data/injection PLOT output *injection *black
    xshow /data
end

function make_netview
    create xform /netview [670,50,300,300]
    create xdraw /netview/draw [0%,0%,100%, 100%]
    // Make the display region a little larger than the cell array
    setfield /netview/draw xmin {-SEP_X} xmax {NX*SEP_X} \
	ymin {-SEP_Y} ymax {NY*SEP_Y}
    create xview /netview/draw/view
    setfield /netview/draw/view path /network/cell[] field Vm \
	value_min {-0.1*uscale} value_max {0.03*uscale} viewmode colorview \
	sizescale {SEP_X}
    xshow /netview
end

function colorize
    setfield /##[ISA=xlabel] fg white bg blue3
    setfield /##[ISA=xbutton] offbg rosybrown1 onbg rosybrown1
    setfield /##[ISA=xtoggle] onfg red offbg cadetblue1 onbg cadetblue1
    setfield /##[ISA=xdialog] bg palegoldenrod
    setfield /##[ISA=xgraph] bg ivory
end
