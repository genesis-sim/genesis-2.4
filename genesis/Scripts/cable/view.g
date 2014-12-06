//genesis - view.g

float SEPX = 1.05

//====================================================================
// Create the view form, graphs and buttons
//====================================================================
// use slow clock for updates
setclock 2 0.1
create xform /viewform [0,445,800,235] -nolabel

pushe /viewform
create xlabel label [5,5,98%,30] -title  \
    "Propagation of Vm Along the Cable"
create xtoggle autoscale [5,195,25%,30] -script  \
    "autoscale_toggle <widget>"

setfield autoscale label0 "Autoscaling OFF" label1 "Autoscaling ON"  \
    state 0

create xdialog Vmin [10:autoscale.right,195,34%,30] -value 0.0 -script  \
    "set_Vmin <widget>"
create xdialog Vmax [10:Vmin.right,195,34%,30] -value 20.0 -script  \
    "set_Vmax <widget>"

pope
//===================================================================
//		Functions invoked by buttons
//===================================================================

function set_Vmin(widget)
    str min
    min = ({getfield {widget} value})
    setfield /viewform/draw/cable vmin {min}
end

function set_Vmax(widget)
    str max
    max = ({getfield {widget} value})
    setfield /viewform/draw/cable vmax {max}
end

function autoscale_toggle(widget)
str widget
        if (({getfield {widget} state}) == 1)
                setfield /viewform/draw/cable autoscale TRUE
        else
                setfield /viewform/draw/cable autoscale FALSE
		set_Vmin /viewform/Vmin
		set_Vmax /viewform/Vmax
        end
end

//========================================================================
// Functions to create and delete the draw and view widgets for the cable
//========================================================================
function make_draw(Ncompart)
  // number of compartments (soma + cable)
  float Ncompart
  create xdraw /viewform/draw [5,40,98%,150] -wx  \
      {1.2*(SEPX*(Ncompart))} -wy 3.0 -transform z2d
  setfield ^ refresh_flag FALSE
  useclock /viewform/draw 2

  create xview /viewform/draw/cable -path /cell/##[TYPE=compartment]  \
      -field Vm -vmin 0 -vmax 20 -viewmode ycolbar -ox  \
      {-0.45*SEPX*(Ncompart)}
  // set autoscale from toggle
  autoscale_toggle /viewform/autoscale
  // use a slow clock for updates
  useclock /viewform/draw/cable 2
  // Apparently needs to be reset before it will properly plot
  reset
end

function delete_draw
  delete /viewform/draw/cable
  delete /viewform/draw
  // At one time I found that without this, I get core dumps!  Seems OK in 1.4.1.
  //  check
end
