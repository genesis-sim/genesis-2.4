//genesis - izgraphics.g - GUI for izcell.g

/*======================================================================
  A GENESIS GUI with a simple control panel and graph, with axis scaling,
  adapted for the "izcell" demo.
  ======================================================================*/

include xtools.g    // functions to make "scale" buttons, etc.
include helpforms.g // functions to make help screens, and function loadhelp
loadhelp    // make the help screens

//===============================
//      Function Definitions
//===============================

function overlaytoggle(widget)
    str widget
    setfield /##[TYPE=xgraph] overlay {getfield {widget} state}
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
    setfield {cell} A {getfield /control/A value}
    setfield {cell} B {getfield /control/B value}
    setfield {cell} C {getfield /control/C value}
    setfield {cell} D {getfield /control/D value}
    echo "Setting the A B C D parameters to: " {getfield {cell} A} \
	 {getfield {cell} B}  {getfield {cell} C}  {getfield {cell} D} 
end

function change_stepsize(dialog)
   str dialog
   dt =  {getfield {dialog} value}
   setclock 0 {dt}
   echo "Changing step size to "{dt}
end

function reset_sim
   str color
   int cnum
   reset
   setfield {cell} Vm {EREST}
   /* Do some tricky business to allow cycling of colors with
      overlayed  Vm plots                                      */
   pushe /data/voltage
   color = "black"
   if ({getfield . overlay} ==1)
      cnum = {getfield . overlay_no}; cnum = cnum % 4 // modulo 4
      if (cnum==0)
          color = "black"
      else
          if (cnum==1)
             color = "red"
          else
             if (cnum==2)
                color = "blue"
             else
                color = "green"
             end
          end
      end
   end
   setfield Vm fg {color}
   pope
end

//===============================
//    Graphics Functions
//===============================

function make_control
  create xform /control [10,50,250,410]
  create xlabel /control/label -hgeom 25 -bg cyan -label "CONTROL PANEL"
  create xbutton /control/RESET -wgeom 33%       -script reset_sim
  create xbutton /control/RUN  -xgeom 0:RESET -ygeom 0:label -wgeom 33% \
         -script step_tmax
  create xbutton /control/QUIT -xgeom 0:RUN -ygeom 0:label -wgeom 34% \
        -script quit
  create xdialog /control/stepsize -title "dt ("{tunit}")" -value {dt} \
                -script "change_stepsize <widget>"
  create xtoggle /control/overlay   -script "overlaytoggle <widget>"
  setfield /control/overlay offlabel "Overlay OFF" onlabel "Overlay ON" state 0
  create xlabel /control/injlabel -label "Injection Parameters"
  create xdialog /control/inject -label "Injection" -value {injcurrent}  \
        -script "set_injection"
  create xdialog /control/injectdelay -label "Delay ("{tunit}")" \
	 -value {injdelay}  -script "set_injection"
  create xdialog /control/width -label "Width ("{tunit}")" \
	-value {injwidth} -script "set_injection"
  create xdialog /control/interval -label "Interval ("{tunit}")" \
	-value {injinterval}  -script "set_injection"

  create xlabel /control/paramlabel -hgeom 25 -bg cyan -label "Spike Parameters"
  create xdialog /control/A  -wgeom 50% -value {A} -script "set_params"
  create xdialog /control/B  -xgeom 0:A -ygeom 0:paramlabel -wgeom 50% \
                -value {B} -script "set_params"
  create xdialog /control/C  -wgeom 50% \
                -value {C} -script "set_params"
  create xdialog /control/D  -xgeom 0:C -ygeom 0:B -wgeom 50% \
                -value {D} -script "set_params"
  create xbutton /control/HELP -script "xshow /output/help"
  create xbutton /control/figure1 -label "Izhikevich (2004) Fig. 1"\
    -script "xshow /output/figure1"
  create xbutton /control/figure2 -label "Izhikevich (2003) Fig. 2"\
    -script "xshow /output/figure2"
  xshow /control
end

function make_Vmgraph
    float vmin = -0.1*uscale
    float vmax = 0.05*uscale
    create xform /data [265,50,400,500]
    create xlabel /data/label -hgeom 5% \
       -label "Izhikevich simple spiking neuron model"
    create xgraph /data/voltage -hgeom 80% -title "Membrane Potential" -bg white
    setfield ^ XUnits {tunit} YUnits V
    setfield ^ xmax {tmax} ymin {vmin} ymax {vmax}
    makegraphscale /data/voltage
    create xgraph /data/injection -hgeom 15% -ygeom 0:voltage \
        -title "Injection" -bg white
    setfield ^ XUnits {tunit} YUnits I
    setfield ^ xmax {tmax} ymin {0} ymax {25}
    makegraphscale /data/injection
    // set up the PLOT messages
    addmsg {cell} /data/voltage PLOT Vm *Vm *red
    // uncomment next line to plot the variable u in the Vm equation
    // addmsg {cell} /data/voltage PLOTSCALE u *u *blue 0.01 0
    addmsg /injectpulse/injcurr /data/injection PLOT output *injection *black
    xshow /data
end

function colorize
    setfield /##[ISA=xlabel] fg white bg blue3
    setfield /##[ISA=xbutton] offbg rosybrown1 onbg rosybrown1
    setfield /##[ISA=xtoggle] onfg red offbg cadetblue1 onbg cadetblue1
    setfield /##[ISA=xdialog] bg palegoldenrod
    setfield /##[ISA=xgraph] bg ivory
end
