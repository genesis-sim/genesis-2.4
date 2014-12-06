//pavlovian reflex with hebbsynchan
include protodefs.g  //create prototypes for the cell reader

float initial_weight = 1.0  // Initial weight for hebb synapse

setclock 0 5e-5
readcell cell.p /cell

// create hebbsynchan
create hebbsynchan /cell/dend/hebb
setfield /cell/dend/hebb Ek 0.045 tau1 0.003 tau2 0.003 gmax 5e-10 \
       max_weight 5 weight_change_rate 0.1
addmsg /cell/dend /cell/dend/hebb VOLTAGE Vm
addmsg /cell/dend/hebb /cell/dend CHANNEL Gk Ek

//Graphics here
create xform /data [0,0,750,400]
create xgraph /data/Vm
setfield /data/Vm xmin 0 xmax 1.5 ymin -0.1 ymax 0.1 hgeom 50%
create xgraph /data/weight
setfield /data/weight xmin 0 xmax 1.5 ymin 0 ymax 5 hgeom 50% \
    title "Hebbian Weight"
xshow /data

//Buttons here
create xform /control [0,430,250,195]
pushe /control
create xbutton Run -script "step 0.1 -t"
create xbutton Stop -script "stop"
create xbutton Reset -script "reset"
create xbutton Bye -script "exit"
create xtoggle Uncond_stimulus //Toggle unconditioned stimulus
setfield  Uncond_stimulus offlabel "No food!"\
  onlabel "Food!" state 1
create xbutton Unlearn -script reset_weight
create xdialog learning_rate -label "Weight Change Rate" -value 0.2 \
 -script "setfield /cell/dend/hebb weight_change_rate <v>"
pope
xshow /control

// Change the drab colors to something more interesting
setfield /##[ISA=xbutton] offbg DeepSkyBlue onbg grey onfg red
setfield /##[ISA=xtoggle] offfg white offbg blue onbg red onfg white
setfield /##[ISA=xdialog] bg yellow
setfield /##[ISA=xgraph] bg white


function reset_weight
    setfield /cell/dend/hebb synapse[0].weight {initial_weight}
end

//create input spike trains
create spikegen /bell
setfield /bell thresh 0.5 abs_refract 0.01  // fire every 10 msec.
create neutral /bell/source
setfield /bell/source x 1.0
addmsg /bell/source /bell INPUT x // provide steady input to spikegen
addmsg /bell /cell/dend/hebb SPIKE

create spikegen /food
setfield /food thresh 0.5 abs_refract 0.01  // fire every 10 msec.
addmsg /control/Uncond_stimulus /food INPUT state // toggle input on/off

addmsg /food /cell/dend/Ex_channel SPIKE
//set synaptic properties of Ex_channel
setfield /cell/dend/Ex_channel synapse[0].weight 2 synapse[0].delay 0

addmsg /cell/soma /data/Vm PLOT Vm *Volts *darkcyan
addmsg /food  /data/Vm PLOTSCALE state *Food *red 0.01 0.05
addmsg /bell  /data/Vm PLOTSCALE state *Bell *blue 0.01 0.07
addmsg /cell/dend/hebb /data/weight PLOT synapse[0].weight *Weight *black

reset
