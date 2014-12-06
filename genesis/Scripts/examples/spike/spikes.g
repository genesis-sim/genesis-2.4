/* making timetable elements to store spike times demonstrating the 
	timetable object functionality
*/

function makespikett

pushe /

create neutral spikes

/*  tt1 is filled with a 2nd order gamma distribution with
	a mean interval of 8 ms and a refractory time of 1.5 ms
*/

create timetable spikes/tt1
setfield spikes/tt1 maxtime 2000 method 2 act_val 1.0 meth_desc1 8 \
	meth_desc2 1.5 meth_desc3 2 seed 12345
call spikes/tt1 TABFILL
create spikegen spikes/tt1/spike
setfield spikes/tt1/spike output_amp 1 thresh 0.5 abs_refract 0.1
addmsg spikes/tt1 spikes/tt1/spike INPUT activation
addmsg spikes/tt1/spike /cell/dend2/Ex_channel SPIKE


/*  tt2 is filled with events specified in ascii file inspikes 
*/

create timetable spikes/tt2
setfield spikes/tt2 maxtime 2000 method 4 act_val 1.0 fname inspikes
call spikes/tt2 TABFILL
create spikegen spikes/tt2/spike
setfield spikes/tt2/spike output_amp 1 thresh 0.5 abs_refract 0.1
addmsg spikes/tt2 spikes/tt2/spike INPUT activation
addmsg spikes/tt2/spike /cell/dend2/Inh_channel SPIKE


/*  tt3 is filled with evenly spaced events with an interval of 10 ms */

create timetable spikes/tt3
setfield spikes/tt3 maxtime 2000 method 3 act_val 1.0 meth_desc1 10
call spikes/tt3 TABFILL
create spikegen spikes/tt3/spike
setfield spikes/tt3/spike output_amp 1 thresh 0.5 abs_refract 0.1
addmsg spikes/tt3 spikes/tt3/spike INPUT activation
addmsg spikes/tt3/spike /cell/dend1/Ex_channel SPIKE


/* create a file output for spike train 1, demonstrating the event_tofile
	object
*/

create event_tofile spikes/tt1_tofile
setfield spikes/tt1_tofile threshold 0.5 fname tt1_spikes.txt
addmsg spikes/tt1 spikes/tt1_tofile INPUT activation
call spikes/tt1_tofile RESET


pope

end // makespikett




/***************   create spike train analysis objects ********/
/* This demonstrates the usage of interspike, autocorr, and crosscorr */
/* Note that autocorr and crosscorr only add bins after simulationtime */
/* exceeds binwidth * num_bins */

function makespkanal (source1, source2)
str source1

pushe /

create neutral spkanal

create interspike spkanal/intvl1
setfield spkanal/intvl1 binwidth 2 num_bins 100 threshold 0.5 reset_mode 2 trigger_mode 0
call spkanal/intvl1 RESET // creates intvl table
addmsg {source1} spkanal/intvl1 INPUT activation

create autocorr spkanal/ac1
setfield spkanal/ac1 binwidth 2 num_bins 100 threshold 0.5 reset_mode 1
call spkanal/ac1 RESET
addmsg {source1} spkanal/ac1 SPIKE activation

/* make a spikegen to capture somatic spikes */
create spikegen spkanal/somaspk
setfield spkanal/somaspk thresh 0 abs_refract 2 output_amp 1
addmsg /cell/soma spkanal/somaspk INPUT Vm

create interspike spkanal/intvl2
setfield spkanal/intvl2 binwidth 2 num_bins 100 threshold 0.5 reset_mode 2 trigger_mode 0
call spkanal/intvl2 RESET
addmsg spkanal/somaspk spkanal/intvl2 INPUT state

create autocorr spkanal/ac2
setfield spkanal/ac2 binwidth 5 num_bins 100 threshold 0.5 reset_mode 1
call spkanal/ac2 RESET
addmsg spkanal/somaspk spkanal/ac2 SPIKE state

create crosscorr spkanal/xc
setfield spkanal/xc binwidth 5 num_bins 100 threshold 0.5 reset_mode 1
call spkanal/xc RESET
addmsg {source1} spkanal/xc XSPIKE activation
addmsg /spkanal/somaspk spkanal/xc YSPIKE state

pope

end

/**************   create graphics for spike train analysil    *******/
/* This routine could be much improved. The x-axis labeling is not
	exactly right
*/

function makeanalgraph

create xform /output/anal1graph [610,10,500,800] -title spkin1stats
pushe /output/anal1graph

create xgraph intvlg1 [0,0,100%,32%] -title "TT1"
setfield intvlg1 xmin 0 xmax 100 ymin 0 ymax 20 XUnits "t (msec)" \
	YUnits "events"
useclock intvlg1 2
makegraphscale /output/anal1graph/intvlg1
create xplot intvlg1/intvlp1 
setfield intvlg1/intvlp1 npts 100 linewidth 1
create xplot intvlg1/acp1 
setfield intvlg1/acp1 npts 100 fg red linewidth 1

create xgraph intvlg2 [0,0:intvlg1.bottom,100%,32%] -title "Soma"
setfield intvlg2 xmin 0 xmax 100 ymin 0 ymax 20 XUnits "t (msec)" \
	YUnits "events"
useclock intvlg2 2
makegraphscale /output/anal1graph/intvlg2
create xplot intvlg2/intvlp1
setfield intvlg2/intvlp1 npts 100 linewidth 1
create xplot intvlg2/acp1 
setfield intvlg2/acp1 npts 100 fg red linewidth 1

create xgraph xcg [0,0:intvlg2.bottom,100%,32%] -title "Cross-correlation"
setfield xcg xmin -50 xmax 50 ymin 0 ymax 20 XUnits "t (msec)" \
	YUnits "events"
useclock xcg 2
makegraphscale /output/anal1graph/xcg
create xplot xcg/xcp 
setfield xcg/xcp npts 100 fg blue

create xbutton clear [0,0:xcg.bottom,100%,3%] -label "Clear Plots" -script clearplots

xshow /output/anal1graph

create script_out doplot
setfield doplot command plotanalgraph

setclock 3 100
useclock doplot 3

pope

end

/* Some functions for resetting the spike analysis objects and
   creating new histograms  */
function clearbins
    setfield /spkanal/##[ISA=interspike] reset_mode 1
    call /spkanal/##[ISA=interspike] RESET
    setfield /spkanal/##[ISA=interspike] reset_mode 0
end

function clearplots
    delete   /output/anal1graph/
    makeanalgraph
    clearbins
    reset_kludge
end

/******************    plot spike analysis data ********************/

function plotanalgraph
int i
int j
int ymax
int num_bins
float bv // bin_value
float bw // binwidth

pushe /output/anal1graph/intvlg1

num_bins = {getfield /spkanal/intvl1 num_bins}
setfield intvlp1 npts {num_bins*4}
bw = {getfield /spkanal/intvl1 binwidth}
ymax = 1

pushe intvlp1
for (i = 0; i < {num_bins}; i = i + 1)
	j = 4*i
	bv = {getfield /spkanal/intvl1 table[{i}]}
	if (ymax < bv) 
		ymax = bv
	end
	setfield xpts->table[{j}] {i*bw} \
	 ypts->table[{j}] 0 \
	 xpts->table[{j+1}] {i*bw} \
	 ypts->table[{j+1}] {bv} \
	 xpts->table[{j+2}] {i*bw+bw} \
	 ypts->table[{j+2}] {bv} \
	 xpts->table[{j+3}] {i*bw+bw} \
	 ypts->table[{j+3}] 0
end
pope

num_bins = {getfield /spkanal/ac1 num_bins}
setfield acp1 npts {num_bins*4}
bw = {getfield /spkanal/ac1 binwidth}
int lw
lw = {getfield intvlp1 linewidth}

pushe acp1
for (i = 0; i < {num_bins}; i = i + 1)
	j = 4*i
	bv = {getfield /spkanal/ac1 acarray[{i}]}
	if (ymax < bv) 
		ymax = bv
	end
	setfield xpts->table[{j}] {i*bw+lw} \
	 ypts->table[{j}] 0 \
	 xpts->table[{j+1}] {i*bw+lw} \
	 ypts->table[{j+1}] {bv} \
	 xpts->table[{j+2}] {i*bw+bw+lw} \
	 ypts->table[{j+2}] {bv} \
	 xpts->table[{j+3}] {i*bw+bw+lw} \
	 ypts->table[{j+3}] 0
end
pope 

setfield . ymax {ymax}

pope

pushe /output/anal1graph/intvlg2

num_bins = {getfield /spkanal/intvl2 num_bins}
setfield intvlp1 npts {num_bins*4}
bw = {getfield /spkanal/intvl2 binwidth}
ymax = 1

pushe intvlp1
for (i = 0; i < {num_bins}; i = i + 1)
	j = 4*i
	bv = {getfield /spkanal/intvl2 table[{i}]}
	if (ymax < bv) 
		ymax = bv
	end
	setfield xpts->table[{j}] {i*bw} \
	 ypts->table[{j}] 0 \
	 xpts->table[{j+1}] {i*bw} \
	 ypts->table[{j+1}] {bv} \
	 xpts->table[{j+2}] {i*bw+bw} \
	 ypts->table[{j+2}] {bv} \
	 xpts->table[{j+3}] {i*bw+bw} \
	 ypts->table[{j+3}] 0
end
pope

num_bins = {getfield /spkanal/ac2 num_bins}
setfield acp1 npts {num_bins*4}
bw = {getfield /spkanal/ac2 binwidth}
lw = {getfield intvlp1 linewidth}

pushe acp1
for (i = 0; i < {num_bins}; i = i + 1)
	j = 4*i
	bv = {getfield /spkanal/ac2 acarray[{i}]}
	if (ymax < bv) 
		ymax = bv
	end
	setfield xpts->table[{j}] {i*bw+lw} \
	 ypts->table[{j}] 0 \
	 xpts->table[{j+1}] {i*bw+lw} \
	 ypts->table[{j+1}] {bv} \
	 xpts->table[{j+2}] {i*bw+bw+lw} \
	 ypts->table[{j+2}] {bv} \
	 xpts->table[{j+3}] {i*bw+bw+lw} \
	 ypts->table[{j+3}] 0
end
pope

setfield . ymax {ymax}

pope

pushe /output/anal1graph/xcg

num_bins = {getfield /spkanal/xc num_bins}
setfield xcp npts {num_bins*4}
bw = {getfield /spkanal/xc binwidth}
int half_bins
half_bins = num_bins / 2 * bw
ymax = 1

pushe xcp
for (i = 0; i < {num_bins}; i = i + 1)
	j = 4*i
	bv = {getfield /spkanal/xc xcarray[{i}]}
	if (ymax < bv) 
		ymax = bv
	end
	setfield xpts->table[{j}] {i*bw-half_bins} \
	 ypts->table[{j}] 0 \
	 xpts->table[{j+1}] {i*bw-half_bins} \
	 ypts->table[{j+1}] {bv} \
	 xpts->table[{j+2}] {i*bw+bw-half_bins} \
	 ypts->table[{j+2}] {bv} \
	 xpts->table[{j+3}] {i*bw+bw-half_bins} \
	 ypts->table[{j+3}] 0
end
pope

setfield . ymax {ymax}

pope

end
