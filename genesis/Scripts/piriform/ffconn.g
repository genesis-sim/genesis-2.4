//genesis
echo
echo FF INTERNEURONS
echo
//                            FF to PYR CONNECTIONS
echo ff to pyr connections
if ({MULTI})
    dst = "/pyr/pyramidal[]/soma/Ia_dend/K_channel"
else
    dst = "/pyr/pyramidal[]/soma/K_channel"
end
// do this over all the cells
// connect to local area of cells
region_connect /ff/interneuron[]/axon to {dst} with synapse -rel 1 1  \
    -100 -100 100 100 1 1 {-{RFF_to_PYR}} {-{RFF_to_PYR}} {RFF_to_PYR}  \
    {RFF_to_PYR} {PFF_to_PYR}

//                            FF to PYR DELAYS
echo ff to pyr delays
gausspropagation_velocity /ff/interneuron[]/axon {{VFF_PYR_MIN}/{SCALE}} \
     {{VFB_PYR_MAX}/{SCALE}} {{VFF_PYR}/{SCALE}} {{SD_VFF_PYR}/{SCALE}}

//                            FF to PYR WEIGHTS
echo ff to pyr weights

/*
** counts the convergent synapses onto a single ff interneuron
*/
float  \
    pyr_sum = {expsum /ff/interneuron[x>={{CORTEX_X}/2 - {RFF_to_PYR}}][y>={{CORTEX_Y}/2 - {RFF_to_PYR}}][x<={{CORTEX_X}/2 + {RFF_to_PYR}}][y<={{CORTEX_Y}/2 + {RFF_to_PYR}}] {-1.0/{LFF_to_PYR}} 1 {AWFF_to_PYR} {{CORTEX_X}/2} {{CORTEX_Y}/2}}

float wpyr = {NSYN_PYR_from_FF}/(pyr_sum*{PFF_to_PYR})

exponential_weight /ff/interneuron[]/axon {wpyr} {LFF_to_PYR}  \
    {{AWFF_to_PYR}*wpyr}

normalize_synapses /ff/interneuron[]/axon {dst} -gaussian  \
    {NSYN_PYR_from_FF} {SDSYN_PYR_from_FF}
