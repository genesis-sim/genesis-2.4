// genesis

create xform /xconn [312,0,800,480]
ce /xconn

// Creating a view to look at the input pattern

float ret_xscale = 0.7
float ret_yscale = 0.8
float ret_xoffset = REC_SEPX/2
float ret_yoffset = REC_SEPX/2

create xlabel /xconn/"Retinal cells" [2%,2%,45%,30] 

create xdraw /xconn/draw [2%,10%,45%,300] 
setfield     /xconn/draw \
	     xmin {ret_xmin / ret_xscale} \
	     xmax {ret_xmax / ret_xscale} \
	     ymin {ret_ymin / ret_yscale} \
	     ymax {ret_ymax / ret_yscale}


function showprojfield(widget)
    str widget
    str value = {getfield {widget} value}
    str dest
    int index

    echo showing projective field for {value}

    int syncount = {getsyncount {value}}
    int i

    for (i = 0; i < {syncount}; i = i + 1)
	dest = {getsyndest {value} {i}}
	index = {getsyndest {value} {i} -index}
        echo {dest} synapse {index} \
	     weight: {getfield {dest} synapse[{i}].weight} \
	     delay:  {getfield {dest} synapse[{i}].delay}
//        addmsg {dest} /xconn/draw2/horiz VAL1 
    end

end



create xview /xconn/draw/retina 
setfield     /xconn/draw/retina \
	     tx {ret_xoffset}   \
	     ty {ret_yoffset}   \
	     sizescale {REC_SEPX * 30}         \  
	     morph_val 0                       \
	     path /retina/recplane/rec[]/input \
	     script "showprojfield <w>"

// Creating a pair of views in the same draw to look at the V1 cells.

float V1_xscale  = 0.7
float V1_yscale  = 0.8
float V1_xoffset = 3 * V1_SEPX
float V1_yoffset = V1_SEPX/2

create xlabel /xconn/"V1 cells" [52%,2%,45%,30]

create xdraw /xconn/draw2 [52%,10%,45%,300] 
setfield     /xconn/draw2  \
    xmin     -0.000559    \
    xmax      0.000457    \
    ymin     -0.00044435  \
    ymax      0.00044465  \
    zmin     -0.0807691   \
    zmax      0.08077     \
    vx       -0.211083    \
    vy       -0.779934    \
    vz        0.589192    \
    transform ortho3d     


function showrecfield(widget)
    str widget
    str value = {getfield {widget} value} @ "/exc_syn"
    str src

    echo showing receptive field for {value}

    int syncount = {getsyncount {value}}
    int i

    for (i = 0; i < {syncount}; i = i + 1)
	src = {getsynsrc {value} {i}}
        echo {src} weight {getfield {value} synapse[{i}].weight} \
		   delay  {getfield {value} synapse[{i}].delay}
//        addmsg {src} /xconn/draw2/horiz VAL1 
    end
end


// create labels for the xview widgets corresponding to V1 cells

create xshape /xconn/draw2/horiz_label -text horiz  -tx -0.00028 -ty -0.0003
create xshape /xconn/draw2/vert_label  -text vert   -tx  0.0002  -ty -0.0003

// Creating the horiz cells to the left of the vert cells.

create xview /xconn/draw2/horiz 
setfield     /xconn/draw2/horiz                 \
             tx {-V1_xoffset}                   \  
             ty { V1_yoffset}                   \
	     value_min[0] -0.090                \
	     value_max[0]  0.060                \
             sizescale {V1_SEPX * 30.0}         \
	     morph_val 0                        \
	     path /V1/horiz/soma[]              \
	     script "showrecfield <w>"

// Creating the vert cells to the right of the horiz cells 

create xview /xconn/draw2/vert
setfield     /xconn/draw2/vert            \
             tx {V1_xoffset}              \
             ty {V1_yoffset}              \
	     value_min[0] -0.090          \
	     value_max[0]  0.060          \
             sizescale {V1_SEPX * 30.0}   \
	     morph_val 0                  \
	     path /V1/vert/soma[]         \
	     script "showrecfield <w>"



//================================================
// old stuff...
//================================================

function set_conns 
//    execute ExpWeight /retina/recplane/rec[]/axon \
//	{{getfield /xconn/"Weight decay"   value} / V1_SEPX } \
//	{getfield /xconn/"Set max weight" value} 0.0
end

function field_toggle(widget)
    str widget

    if ({getfield {widget} state} == 1)
//	setfield /xconn/draw/rec_conn field weight
//	setfield /xconn/draw/V1_conn field weight
	echo field is weight
    else
//	setfield /xconn/draw/rec_conn field delay
//	setfield /xconn/draw/V1_conn field delay
	echo field is delay
    end
end

//create xtoggle /xconn/"delay | weight"  [0,80%,100%,30] -script "field_toggle <w>"
create xtoggle /xconn/delay_weight  [0,80%,100%,30] -label0 "Displaying delays" \
        -label1 "Displaying weights" -script "field_toggle <w>"
create xdialog /xconn/"Set max weight" -script "set_conns" -value  10000.0
create xdialog /xconn/"Weight decay"   -script "set_conns" -value -1.0000

disable /xconn
