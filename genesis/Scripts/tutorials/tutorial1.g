//genesis script for a simple compartment simulation (Tutorial #1)

// create a parent element
create neutral /cell

// create an instance of the compartment object
create compartment /cell/soma

// set some internal fields
setfield /cell/soma Rm 10 Cm 2 Em 25 inject 5

// create and display a graph inside a form
create xform /data
create xgraph /data/voltage
xshow /data

// set up a message (PLOT Vm) to the graph
addmsg /cell/soma /data/voltage PLOT Vm *volts *red
addmsg /cell/soma /data/voltage PLOT inject *current *blue

// make some buttons to execute simulation commands
create xbutton /data/RESET -script reset
create xbutton /data/RUN -script "step 100"
create xbutton /data/QUIT -script quit

check       // perform a consistency check for each element
reset       // initialize each element before starting the simulation
