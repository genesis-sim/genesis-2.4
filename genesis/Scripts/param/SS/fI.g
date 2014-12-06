// genesis

//
// fI.g: functions for initializing tables of currents,
//       and functions to stimulate cell with varying depolarizing
//       currents to give frequency vs. current (f/I) outputs.
//

float fIscale       = 1e-9 // currents in table are in units of 1 nA
int   fItable_index = 0
str   currpath      = "/currtable"

function init_fItable(ncurrs)
    int ncurrs // number of currents
    create table {currpath}
    call {currpath} TABCREATE {ncurrs + 1} 0 0
    // need extra table entry to store delimiter (-9999.0)
end


function add_to_fItable(curr)
    float curr
    setfield {currpath} table->table[{fItable_index}] {curr}
    fItable_index = fItable_index + 1
end


function end_fItable
    setfield {currpath} table->table[{fItable_index}] -9999.0
    // -9999.0 in the table means it's the last entry;
    // -9999.0 is not executed
end


function show_fItable
    int i = 0
    float curr

    while ({getfield {currpath} table->table[{i}]} > -1000.0)
        curr = {getfield {currpath} table->table[{i}]}
        echo Current {i}: {curr}
        i = i + 1
    end
end


function make_fItable
    init_fItable 6
    add_to_fItable 0.10
    add_to_fItable 0.20
    add_to_fItable 0.40
    add_to_fItable 0.60
    add_to_fItable 0.80
    add_to_fItable 1.0
    end_fItable
end



function clearfile(filename)
    // clears a disk file
    str       filename
    openfile  {filename} w
    closefile {filename}
end


//
// This function runs the cell simulation with each level of
// current injection in succession.
//

function runfI
    // /Iin is the pulsegen object that generates the currents
    // The output can be viewed with xplot as well as processed
    // to get the spike times or ISIs.

    float current, plotted_current
    int i = 0
    str file

    clearfile {sim_output_file}

    while ({getfield {currpath} table->table[{i}]} > -1000.0)
        reset
        plotted_current = {getfield {currpath} table->table[{i}]}
        current = plotted_current * fIscale
        echo . -n
        setfield /Iin level1 {current}
        openfile  {sim_output_file} a
        writefile {sim_output_file} "/newplot"

        if (orig)
            // Offset the target data by 150 mV for easy viewing.
            writefile {sim_output_file} "/xaxisoffset -0.150"
        end

        writefile {sim_output_file} "/plotname "{plotted_current}
        closefile {sim_output_file}

        step {total_duration} -t
        call /out/{sim_output_file} SAVE

        i = i + 1
    end

    call out/{sim_output_file} FLUSH

    echo
end


