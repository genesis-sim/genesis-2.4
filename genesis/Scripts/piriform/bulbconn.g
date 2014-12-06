//genesis
function bulb_connect(path)
str path

    echo
    echo AFFERENT INPUT
    echo
    if ({MULTI})
	dst = "/pyr/pyramidal[]/soma/Ia_dend/Aff_Na_channel"
    else
	dst = "/pyr/pyramidal[]/soma/Aff_Na_channel"
    end
    echo afferent connections
    // do this over all the cells
    // connect to all the cells
    region_connect {path} to {dst} with synapse -rel 1 1 -100 -100 100  \
        100 1 1 -100 -100 100 100 {PBULB_to_PYR}

    // do this over all the cells
    // connect to all the cells
    region_connect {path}2 to /ff/interneuron[]/soma/Na_channel with  \
        synapse -rel 1 1 -100 -100 100 100 1 1 -100 -100 100 100  \
        {PBULB_to_FF}

    // do this over all the cells
    // connect to all the cells
    region_connect {path}3 to /fb/interneuron[]/soma/Na_channel with  \
        synapse -rel 1 1 -100 -100 100 100 1 1 -100 -100 100 100  \
        {PBULB_to_FB}

    //                            AFFERENT DELAYS
    echo afferent delays
					// use a special delay function
					// which takes into account the
					// different propagation 
					// characteristics of the main
					// LOT tract and its collaterals

					// the main tract has a propagation
					// velocity of 7.0 m/s
					// the collaterals 1.25 m/s
					// and the angle of exit
					// for the collaterals is 45 deg
					/*
     affdelay {path}	45 -vx {VLOT_MAIN/SCALE}  -vy {VLOT_COLL/SCALE}
     affdelay {path}2	45 -vx {VLOT_MAIN/SCALE}  -vy {VLOT_COLL/SCALE}
     affdelay {path}3	45 -vx {VLOT_MAIN/SCALE}  -vy {VLOT_COLL/SCALE}
*/
     affdelay {path} 45 -uniformvx {{VLOT_MAIN_MIN}/{SCALE}}  \
        {{VLOT_MAIN_MAX}/{SCALE}} -uniformvy {{VLOT_COLL_MIN}/{SCALE}}  \
        {{VLOT_COLL_MAX}/{SCALE}}
     affdelay {path}2 45 -uniformvx {{VLOT_MAIN_MIN}/{SCALE}}  \
        {{VLOT_MAIN_MAX}/{SCALE}} -uniformvy {{VLOT_COLL_MIN}/{SCALE}}  \
        {{VLOT_COLL_MAX}/{SCALE}}
     affdelay {path}3 45 -uniformvx {{VLOT_MAIN_MIN}/{SCALE}}  \
        {{VLOT_MAIN_MAX}/{SCALE}} -uniformvy {{VLOT_COLL_MIN}/{SCALE}}  \
        {{VLOT_COLL_MAX}/{SCALE}}

    //                            AFFERENT WEIGHTS
    echo afferent weights
					// the afferent weights are set using 
					// an exponential distribution with
					// cangle max mlambda clambda min
    aff_weight {path} 45 {WBULB_to_PYR} {LBULB_to_MAIN} {LBULB_to_COLL}  \
        {AWBULB_to_PYR}
    aff_weight {path}2 45 {WBULB_to_FF} {LBULB_to_MAIN} {LBULB_to_COLL}  \
        {AWBULB_to_FF}
    aff_weight {path}3 45 {WBULB_to_FB} {LBULB_to_MAIN} {LBULB_to_COLL}  \
        {AWBULB_to_FB}
end

function parallel(path)
    setconn {path}:# weight {WBULB_to_PYR}
    setconn {path}2:# weight {WBULB_to_PYR}
    setconn {path}3:# weight {WBULB_to_PYR}
    setconn {path}:# delay 0
    setconn {path}2:# delay 0
    setconn {path}3:# delay 0
end

if ({BULB})
    bulb_connect /bulb/mitral[]/axon
else
    bulb_connect /bulb/glomplane/glom[]/mitplane/axon
end

