//genesis - test of rasterplots with single compartment cells basic-cell.p

float tmax = 0.25		// simulation time in sec
float dt = 0.00005		// simulation time step in sec
setclock  0  {dt}		// set the simulation clock
float injcurrent = 0.3e-9

// int Ncells = 32
int Ncells = 128
int Nplots = Ncells

str timedata ="/time_data"  // name of the array of tables
str spike_time_file="spike_times.txt"  // output file
str cellfile = "basic-cell.p"

/* Some Booleans indicating the type of calculations or output */
int batch = 0 // if (batch) run simulation with default injcurrent, tmax
int graphics = 1     // display control panel, graphs

//===============================
//      Function Definitions
//===============================


// The number of spike times will be around tmax/abs_refract
// A very conservative estimate is tmax/0.001

int Npoints = {round {tmax/0.001}}

// make array of tables to hold time series data to display
function make_time_data_tables(Nplots, tmax, tablename)
    float tmax
    int Nplots
    int Npoints = {round {tmax/0.001}}
    str timedata = tablename
    int i
    int xdivs = Npoints
    float xmin = 0; float xmax = {tmax} // this is arbitrary
    for (i = 0; i < {Nplots}; i = i + 1)
        if ({exists {timedata}[{i}]}) // Get rid of any existing one with old xdivs
            delete {timedata}[{i}]
        end
        create table {timedata}[{i}]
        call {timedata}[{i}] TABCREATE {xdivs} {xmin} {xmax}
        setfield {timedata}[{i}] step_mode 4 stepsize 0.0
    end
//    useclock {timedata}  1
end

function make_time_data_file(Nplots,filename, tablename)
    str filename, tablename
    int i,j, ntimes, Nplots
    float spike_time
    openfile {filename} w
    for (i = 0; i < {Nplots}; i = i + 1)
        ntimes = {getfield {tablename}[{i}] output}
            for (j=0; j < {ntimes}; j = j + 1)
                spike_time = {getfield {tablename}[{i}] table->table[{j}]}
                // write it to the file
                writefile {filename} {spike_time} " " -n
            end
        writefile {filename}  // put a final newline
    end // i = 0; i < {Nplots}
    closefile {filename} 
end

function set_inject(value)
    str value
    float fract = 0.1
    setrandfield /cell[]/soma inject \
	-uniform {value*(1-fract)} {value*(1+fract)}
end

/* The Vm message source will have to be customized for
   other cell arrays
 */
function make_rasterplot_msgs(Nplots, Vm_src, dest_table)
    int i, Nplots
    str Vm_src, dest_table
    for (i = 0; i < {Nplots}; i = i + 1)
	addmsg {Vm_src}[{i}]/soma {dest_table}[{i}] INPUT Vm
    end
end

function step_tmax
    step {tmax} -time
    make_time_data_file {Nplots} {spike_time_file} {timedata}
end

//===============================
//    Graphics Functions
//===============================

function make_control
    create xform /control [10,50,250,145]
    create xlabel /control/label -hgeom 50 -bg cyan -label "CONTROL PANEL"
    create xbutton /control/RESET -wgeom 33%       -script reset
    create xbutton /control/RUN  -xgeom 0:RESET -ygeom 0:label -wgeom 33% \
         -script step_tmax
    create xbutton /control/QUIT -xgeom 0:RUN -ygeom 0:label -wgeom 34% \
        -script quit
    create xdialog /control/Injection -label "Injection (amperes)" \
                -value 0.3e-9 -script "set_inject <value>"
    xshow /control
end

/* GENESIS 2 cannot make raster plots.  make_fastplots generates Nplots
   tiny plots of Vm within a rarely-used G2 xfastplot element.
*/
function make_fastplots(form,plotname,Nplots)
    int form_height = 800
    int i, Nplots
    str form, plotname
    float vmin = -0.100
    float vmax = 0.05
    int plot_height = {round {form_height/Nplots}}
    create xform {form}  [265,370,500,525]
    for (i = 0; i < {Nplots}; i = i + 1)
        create xfastplot {form}/{plotname}[{i}] -ygeom {plot_height*i} \
                -hgeom {plot_height}
        setfield ^ xaxis_flag 0 yaxis_flag 0
        setfield ^ xmax {tmax} ymin {vmin} ymax {vmax}
        setfield ^ npts {round {tmax/{getclock 0}}}
        setfield ^ oscope_flag 0 line_flag 0 color 2
        setfield ^ lborder 0 bborder 0
    end
    xshow {form}
end

/* Create a library of prototype elements to be used by the cell reader
   Included files are in genesis/Scripts/neurokit/prototypes */
// include the definitions for the functions to create H-H tabchannels
include hh_tchan

EREST_ACT = -0.07	// resting membrane potential (volts)
ENA   = 0.045           // sodium equilibrium potential
EK    = -0.082          // potassium equilibrium potential

// Make a "library element" to hold the prototypes, which will be used
// by the cell reader to add compartments and channels to the cell.
create neutral /library

// We don't want the library to try to calculate anything, so we disable it
disable /library

// To ensure that all subsequent elements are made in the library
pushe /library

create compartment compartment
make_Na_hh_tchan
make_K_hh_tchan
create spikegen spike
setfield spike  thresh -0.03  abs_refract 2.0e-3  output_amp 1

pope // Return to the original place in the element tree

//===============================
//         Main Script
//===============================

readcell {cellfile} /cell

// provide current injection to the soma - to be modified
setfield /cell/soma inject  0.3e-9      // 0.3 nA injection current

// This should later become a function using createmap
// Make the array of cells
    int i
    for (i = 1; i < {Ncells}; i = i + 1)
        copy /cell /cell[{i}]
    end

set_inject {injcurrent}

// Make the array of {timedata}[]} tables
make_time_data_tables {Nplots} {tmax} {timedata}
make_rasterplot_msgs {Nplots} /cell {timedata}

if (graphics)
    // make the control panel
    make_control

    // make the fastplot to display soma Vm for all cells
    make_fastplots "/raster" "plot" {Nplots}
    // Add the messages to plot
    for (i = 0; i < {Nplots}; i = i + 1)
        addmsg /cell[{i}]/soma /raster/plot[{i}] NPLOT Vm 1 0
    end
end

check
reset
if (batch)
    step_tmax
end
