/***********************************************************************/

//genesis - tab_logplot.g - used to plot log(Vm)
/***************************************************************************
 As the GENESIS xgraph object doesn't have a way to plot with a logarithmic
 scale, we create a "logifier" object which uses a lookup table to calculate
 a natural log.  The log of a quantity can then be plotted with messages
 like:
	addmsg /cell/soma /logtable INPUT Vm
	addmsg /logtable loggraph   PLOT output *soma *black
***************************************************************************/

function make_logtable
int xdivs = 1000
int xmax = 100
int i
float dx
float x
float y
float SCALE = 1.0  // We may later want to scale and shift the plot
float OFFSET = 0.0 // if the range includes x = 0.

create table /logtable
call logtable TABCREATE {xdivs} 0 {xmax}
//call table TABCREATE xdivs xmin xmax (mV)
dx = {getfield logtable table->dx}  // this will be 0.0
x = {getfield logtable table->xmin} // kludge to set log(0) = log(dx)
y = SCALE*{log {dx + OFFSET}}
setfield logtable table->table[{0}] {y}
x = x + dx

    for (i = 1; i <= (xdivs); i = i + 1)
	y = SCALE*{log {x + OFFSET}} // OFFSET needed if x < 0
	setfield logtable table->table[{i}] {y}
	x = x + dx
    end
end

// By default the table uses linear interpolation, which is what we want.

make_logtable // do it

// ----------------------------------------------------------------------
// Now make a simple example to use the "logifier"

create compartment /compt

// set some internal fields with injection to give a rising Vm
// Vm will rise from 0 to almost 100 over 100 time units as Cm is charged

setfield /compt Rm 10 Cm 2 Em 0 inject 10

create xform /data [430,20,420,600] -nolabel
create xgraph /data/voltage [0,0,420,250]
setfield /data/voltage xmax 100 ymax 100
create xgraph /data/loggraph [0,250,420,250]
setfield /data/loggraph ymin -1 ymax 5 YUnits "ln(Vm)"

// make some buttons to execute simulation commands
create xbutton /data/RESET -script reset
create xbutton /data/RUN -script "step 100"
create xbutton /data/QUIT -script quit

xshow /data

// send a message (PLOT Vm) to the voltage graph
addmsg /compt /data/voltage PLOT Vm *volts *red

// send the mesage through the log table to the log graph
addmsg /compt /logtable INPUT Vm
addmsg /logtable /data/loggraph PLOT output *log *black

reset
