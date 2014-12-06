// genesis

create xform /xgraphs [0,450,700,420]
ce /xgraphs

/* Viewgraph plots the output from the files set up in V1_output.g */
create xgraph /xgraphs/viewgraph [2%,10,45%,350] \
    -range [0,-0.09,0.1,0.05] 	

/* Simgraph plots out the membrane potential of selected cells */
create xgraph /xgraphs/simgraph [52%,10,45%,350] \
    -range [0,-0.5,0.1,0.1] -title "membrane potential Vm (V)"  
setfield /xgraphs/simgraph yoffset -0.1

addmsg /V1/vert/soma[14] /xgraphs/simgraph PLOT Vm *Vm14 *black
addmsg /V1/vert/soma[13] /xgraphs/simgraph PLOT Vm *Vm13 *blue
addmsg /V1/vert/soma[12] /xgraphs/simgraph PLOT Vm *Vm12 *orange
addmsg /V1/vert/soma[11] /xgraphs/simgraph PLOT Vm *Vm11 *red
addmsg /V1/vert/soma[10] /xgraphs/simgraph PLOT Vm *Vm10 *green

create xdialog index -script plotindex -value 12 \
    [2%,5:viewgraph,15%,30]
 
create xdialog filename -script plotindex -value vert_disk \
    [10:index,5:viewgraph,30%,30]

create xdialog /xgraphs/xmin -script "setxrange" -value 0 \
    [10:filename,5:viewgraph,23%,30] 

create xdialog /xgraphs/xmax -script "setxrange" -value 0.1 \
    [10:xmin,5:viewgraph,23%,30] 


function setxrange
    setfield /xgraphs/viewgraph xmin {getfield /xgraphs/xmin value}
    setfield /xgraphs/viewgraph xmax {getfield /xgraphs/xmax value}
    setfield /xgraphs/simgraph  xmin {getfield /xgraphs/xmin value}
    setfield /xgraphs/simgraph  xmax {getfield /xgraphs/xmax value}
end

// the function below will not work since xsimplot no longer exists.

function plotindex
    str index, filename
    index    = {getfield /xgraphs/index value}
    filename = {getfield /xgraphs/filename value}
//    xsimplot /xgraphs/viewgraph -c {index} {filename}
    echo "Not available yet!"
end

function plotviewgraph(widget)
    str widget
    str element
    int index

    element = {getfield {widget}  value}
    index   = {getfield {element} index}

    setfield /xgraphs/index value {index}
    if ({strncmp {element} /V1/vert 8} == 0)
        setfield /xgraphs/filename value vert_disk
    else 
        setfield /xgraphs/filename value horiz_disk
    end

    plotindex
end

setxrange // In order to make the graphs appear in the correct size
xshow /xgraphs
useclock /xgraphs/##[] 1
