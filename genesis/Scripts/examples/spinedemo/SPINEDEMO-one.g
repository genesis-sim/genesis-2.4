//genesis2.2
/* Copyright E. De Schutter BBF-UIA Oct 1998 */

/* Simulates calcium influx through a NMDA channel into a single spine.
**  Incorporates buffered one-dimensional diffusion of calcium.
**  Based upon :
**      De Schutter E & Bower JM: Sensitivity of synaptic plasticity
**        to the Ca2+-permeability of NMDA-channels: a model of
**        long-term potentiation in hippocampal neurons.  Neural
**        Computation 5, 681-694, 1993.
**      Holmes WR & Levy WB: "Insights into associative long-term
**        potentiation from computational models of NMDA receptor-
**        mediated calcium influx and intracellular calcium
**        concentrations changes.  J. Neurophysiol. 63, 1148-1168, 1990.
**  Differences with these papers:
**   - not the same morphology
**   - synapses implemented differently: NMDA conductance behaves differently
**   - voltage-dependence of Ca component of NMDA current not implemented
** For a general description of buffered calcium diffusion:
**      De Schutter E., and Smolen P.: Calcium dynamics in large neuronal
**      models. in Methods in neuronal modeling: from ions to networks
**      (2nd edition), C. Koch and I. Segev editors, 211-250 (1998).
** http://www.bbf.uia.ac.be/models/spine.shtml
*/

/* Constants controlling the simulation */
include parameters.g
include makeshells.g
setclock 0 5e-6 // computation
setclock 1 5e-4 // output

/* make cell and computing hsolve element */
     create hsolve /solve
     make_all_comparts
     make_all_shells
     reset	// compute volumes
     setfield /solve path /solve/[TYPE=compartment] chanmode 4
     call /solve SETUP
     setmethod 11

/* create output */
     create asc_file /output/file
     useclock /output/file 1
     setfield /output/file filename "Spine_one" initialize 1 leave_open 1

/* create graphics */
     str hstr
     create xform /results [200,50,800,800]
     create xlabel /results/label [10,0,98%,25] -label "Spine Demo"

     create xgraph /results/voltage [10,10:label.bottom,47%,45%] -title \
	"membrane potential" bg white
     useclock /results/voltage 1
     setfield /results/voltage xmax 0.3 ymin -0.075 ymax -0.050
     hstr={findsolvefield /solve /solve/head Vm}
     addmsg /solve /output/file SAVE {hstr}
     addmsg /solve /results/voltage PLOT {hstr} *head *red
     hstr={findsolvefield /solve /solve/soma Vm}
     addmsg /solve /results/voltage PLOT {hstr} *soma *blue

     /*
     create xgraph /results/synapse [10,10:voltage.bottom,47%,45%] -title  \
	"synaptic conductance" bg white
     useclock /results/synapse 1
     setfield /results/synapse xmax 0.3 ymin 0 ymax 10.0e-12
     hstr={findsolvefield /solve /solve/head/NMDA Gk}
     addmsg /solve /results/synapse PLOT {hstr} *NMDA_unblocked *red
     hstr={findsolvefield /solve /solve/head/Mg_block Gk}
     addmsg /solve /results/synapse PLOT {hstr} *NMDA_blocked *blue
     hstr={findsolvefield /solve /solve/head/AMPA Gk}
     addmsg /solve /results/synapse PLOT {hstr} *AMPA *green
     */

     create xgraph /results/synapse [10,10:voltage.bottom,47%,45%] -title  \
	"synaptic current" bg white
     useclock /results/synapse 1
     setfield /results/synapse xmax 0.3 ymin 0 ymax 4.0e-13
     hstr={findsolvefield /solve /solve/head/NMDA Ik}
     addmsg /solve /results/synapse PLOT {hstr} *NMDA_unblocked *red
     hstr={findsolvefield /solve /solve/head/Mg_block Ik}
     addmsg /solve /results/synapse PLOT {hstr} *NMDA_blocked *blue
     hstr={findsolvefield /solve /solve/head/AMPA Ik}
     addmsg /solve /results/synapse PLOT {hstr} *AMPA *green
     hstr={findsolvefield /solve /solve/head/headshell0 flux}
     addmsg /solve /results/synapse PLOT {hstr} *Ca_flux *orange

     create xgraph /results/calcium [10:voltage.right,10:label.bottom,47%,45%] \
	-title "calcium concentration" bg white
     useclock /results/calcium 1
     setfield /results/calcium xmax 0.3 ymin 0.00001 ymax 0.0008
     hstr={findsolvefield /solve /solve/head/headshell0 C}
     addmsg /solve /results/calcium PLOT {hstr} *head0 *red
     addmsg /solve /output/file SAVE {hstr}
     hstr={findsolvefield /solve /solve/head/headshell3 C}
     addmsg /solve /results/calcium PLOT {hstr} *head3 *blue
     hstr={findsolvefield /solve /solve/head/neckshell1 C}
     addmsg /solve /results/calcium PLOT {hstr} *neck1 *green
     hstr={findsolvefield /solve /solve/head/dendshell0 C}
     addmsg /solve /results/calcium PLOT {hstr} *dend0 *orange
     hstr={findsolvefield /solve /solve/head/dendshell9 C}
     addmsg /solve /results/calcium PLOT {hstr} *dend9 *yellow

     create xgraph /results/buffer [10:synapse.right,10:calcium.bottom,47%,45%] \
	-title "free buffer concentration" bg white
     useclock /results/buffer 1
     setfield /results/buffer xmax 0.3 ymin 0.000 ymax 0.20
     hstr={findsolvefield /solve head/headbuf0 Bfree}
     addmsg /solve /results/buffer PLOT {hstr} *head0 *red
     hstr={findsolvefield /solve head/headbuf3 Bfree}
     addmsg /solve /results/buffer PLOT {hstr} *head3 *blue
     hstr={findsolvefield /solve head/neckbuf1 Bfree}
     addmsg /solve /results/buffer PLOT {hstr} *neck1 *green
     hstr={findsolvefield /solve head/dendbuf0 Bfree}
     addmsg /solve /results/buffer PLOT {hstr} *dend0 *orange
     hstr={findsolvefield /solve head/dendbuf9 Bfree}
     addmsg /solve /results/buffer PLOT {hstr} *dend9 *yellow

     xshow /results

reset

step 0.010 -time
echo "Firing synapse"
setfield /solve/presyn z {1/{getclock 0}}
step 1
setfield /solve/presyn z 0
step 0.290 -time
