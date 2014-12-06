//genesis
// Neuron.g

// set the prompt
setprompt "Neuron !"
// simulation time step in msec
setclock 0 0.01 //  max size for reasonable accuracy - for spike plots, too
setclock 1 0.05 // output interval for Vm plots
setclock 2 0.05 // output interval for others
floatformat %.3g // do reasonable rounding for display

echo SIMULATION CONFIGURATION
echo =========================
echo Hodgkin-Huxley
echo multi-compartmental neuron (soma + dendrite)
echo with synaptic connections

// start with no passive compartments between dendrites
int NCableSects = 0

//==================================================
//                  STARTUP SCRIPTS
//==================================================
include constants.g 
include tools.g 
include channel.g 
include compartment.g 
include cell.g 
include inputs.g 
include make_cable.g 
include forms.g 
include spikes.g
include setcolors.g // function colorize sets widget colors

//=========================================================================
//  Some functions used in the main script

//  SendPlotMsgs  - sends messages to the graphics forms

//  ResetDefaults - similar to the main script, but graph forms pre-exist
//=========================================================================

function SendPlotMsgs
  pushe /output/dend1graphs
  addmsg /input/injectpulse input_grf PLOTSCALE output *Inject *black 1 12
  addmsg /input/Apulse/spiketrain input_grf PLOTSCALE state *SourceA  \
      *red 1 10
  addmsg /input/Bpulse/spiketrain input_grf PLOTSCALE state *SourceB  \
      *blue 1 8
  addmsg /spikes/tt1/spike input_grf PLOTSCALE state *tt1spk  \
      *green 1 6
  addmsg /spikes/tt2/spike input_grf PLOTSCALE state *tt2spk  \
      *brown 1 4
  addmsg /spikes/tt3/spike input_grf PLOTSCALE state *tt3spk  \
      *orange 1 2
  addmsg /cell/dend1/Ex_channel dend1Gk_grf PLOT Gk *Gex *black
  addmsg /cell/dend1/Inh_channel dend1Gk_grf PLOT Gk *Ginh *red
  addmsg /cell/dend1 dend1Vm_grf PLOT Vm *dend1 *black
  pope

  pushe /output/dend2graphs
  addmsg /cell/dend2/Ex_channel dend2Gk_grf PLOT Gk *Gex *black
  addmsg /cell/dend2/Inh_channel dend2Gk_grf PLOT Gk *Ginh *red
  addmsg /cell/dend2 dend2Vm_grf PLOT Vm *dend2 *black
  pope

  pushe /output/somagraphs
  addmsg /cell/soma/Ex_channel activation_grf PLOT X *m *black
  addmsg /cell/soma/Ex_channel activation_grf PLOT Y *h *red
  addmsg /cell/soma/Inh_channel activation_grf PLOT X *n *blue

  addmsg /cell/soma/Ex_channel somaGk_grf PLOT Gk *Na *black
  addmsg /cell/soma/Inh_channel somaGk_grf PLOT Gk *K *red
  addmsg /cell/soma somaVm_grf PLOT Vm *soma *black
  pope
end// SendPlotMsgs


function ResetDefaults
    stop
    delete /cell
    delete /input
    delete /output/control
    delete /output/popups
    // re-initialize toggle state
    xhide /output/dend2graphs
    NCableSects = 0
    makeneuron /cell {soma_l} {soma_d} {dend_l} {dend_d}    // in cell.g
    makeinputs    // inputs.g
	makespikett   // spike tables for inputs
    loadcontrol
    SendPlotMsgs
//    check
    colorize
    reset_kludge    // in cell.g
end

//=========================================================================

//                         MAIN SCRIPT - Neuron   

//=========================================================================

//==================================================
// check and initialize the simulation
//==================================================
makeneuron /cell {soma_l} {soma_d} {dend_l} {dend_d}// in cell.g

// inputs.g
makeinputs

// spikes.g
makespikett

// in forms.g (This is done after the elements are created)
loadgraphs
loadcontrol
SendPlotMsgs
// check
colorize
// Set up the spike analysis obects defined in spikes.g
makespkanal /spikes/tt1
// create the spike analysis histograms
makeanalgraph

//set up some defaults for the modified Neuron demo
setfield /output/dend1graphs/input_grf ymax 15
setfield /cell/dend2/Ex_channel synapse[2].weight 20
setfield /output/dend1graphs/##[ISA=xgraph] xmax 1200
setfield /output/dend2graphs/##[ISA=xgraph] xmax 1200
setfield /output/somagraphs/##[ISA=xgraph] xmax 1200
setfield /output/control/step value 600

reset_kludge // in cell.g

echo ==================
echo Simulation loaded.
echo ==================
