//genesis  - example script for the "freq_monitor" object - Dave Beeman

float tmax =  1.0       // default simulation time = 1 sec
float dt = 0.0001       // simulation time step in sec
setclock  0  {dt}       // set the simulation clock
float rate = 200        // default average spike rate
float tmin = 0.05       // default "window" for the frequency monitor
float exponent = 0      // default exponent used for weighting

// some function used by the dialog boxe to set the average spiking rate
function set_rate
    rate = {getfield /data/rate value}
    echo "Average spike rate: "{rate}" spikes per second"
    setfield /randomspike rate {rate}
    setfield /data/frequency ymax {2*rate}
    reset
end

// Make a graph to show the spike input and the spike frequency
float ymax = 400
create xform /data [50,50,500,480]  -title "freq_monitor demo"
create xgraph /data/frequency -hgeom 60% -title "Spike Frequency"
setfield ^ xmax {tmax} ymax {ymax} XUnits sec YUnits Freq
create xbutton /data/RESET  -script reset
create xbutton /data/RUN  -script "step "{tmax}" -time"
create xdialog /data/rate -title "Average Spike Rate" -value {rate} \
        -script set_rate
create xdialog /data/tmin -title "freq_monitor 'tmin' (Window)" \
        -value {tmin}   -script "setfield /freqmon tmin <v>; reset"
create xdialog /data/exp -title "exponent" -value {exponent} \
    -script "setfield /freqmon exponent <v>; reset"
create xbutton /data/QUIT -script quit
xshow /data

// use a "randomspike" element to make an average of {rate} spikes/unit time,
create randomspike /randomspike
setfield ^ min_amp 1.0 max_amp 1.0 rate {rate} reset 1 reset_value 0
// create a frequency monitor to display the average spike rate
create freq_monitor /freqmon
setfield /freqmon tmin {tmin}
addmsg /randomspike /freqmon POSITIVE_TRIG state // trigger if > 0
addmsg /randomspike /data/frequency \
        PLOTSCALE state  *input *red  {0.2*ymax}  0
addmsg /freqmon /data/frequency PLOT frequency *frequency *black
check
reset
