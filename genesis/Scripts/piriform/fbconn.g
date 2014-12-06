//genesis

echo
echo FB INTERNEURONS
echo


//                            FB to PYR CONNECTIONS
echo fb to pyr connections
// do this over all the cells
// connect to local area of cells
region_connect /fb/interneuron[]/axon to  \
    /pyr/pyramidal[]/soma/Cl_channel with synapse -rel 1 1 -100 -100 100 \
     100 1 1 {-{RFB_to_PYR}} {-{RFB_to_PYR}} {RFB_to_PYR} {RFB_to_PYR}  \
    {PFB_to_PYR}

//                            FB to PYR DELAYS
echo fb to pyr delays
gausspropagation_velocity /fb/interneuron[]/axon {{VFB_PYR_MIN}/{SCALE}} \
     {{VFB_PYR_MAX}/{SCALE}} {{VFB_PYR}/{SCALE}} {{SD_VFB_PYR}/{SCALE}}

/*
** counts the convergent synapses onto a single ff interneuron
*/
float  \
    pyr_sum = {expsum /fb/interneuron[x>={{CORTEX_X}/2 - {RFB_to_PYR}}][y>={{CORTEX_Y}/2 - {RFB_to_PYR}}][x<={{CORTEX_X}/2 + {RFB_to_PYR}}][y<={{CORTEX_Y}/2 + {RFB_to_PYR}}] {-1.0/{LFB_to_PYR}} 1 {AWFB_to_PYR} {{CORTEX_X}/2} {{CORTEX_Y}/2}}

float wpyr = {NSYN_PYR_from_FB}/(pyr_sum*{PFB_to_PYR})

//                            FB to PYR WEIGHTS
echo fb to pyr weights
exponential_weight /fb/interneuron[]/axon {wpyr} {LFB_to_PYR}  \
    {{AWFB_to_PYR}*wpyr}

if ({DISINHIB})
    //                            FB to FB CONNECTIONS
    echo fb to fb connections
    // do this over all cells
    // connect to local area 
    region_connect /fb/interneuron[]/fbaxon to  \
        /fb/interneuron[]/soma/Cl_channel with synapse -rel 1 1 -100  \
        -100 100 100 1 1 {-{RFB_to_PYR}} {-{RFB_to_PYR}} {RFB_to_PYR}  \
        {RFB_to_PYR} {PFB_to_PYR}

    //                            FB to FB DELAYS
    echo fb to fb delays
    gausspropagation_velocity /fb/interneuron[]/fbaxon  \
        {{VFB_PYR_MIN}/{SCALE}} {{VFB_PYR_MAX}/{SCALE}}  \
        {{VFB_PYR}/{SCALE}} {{SD_VFB_PYR}/{SCALE}}


    //                            FB to FB WEIGHTS
    echo fb to fb weights
    exponential_weight /fb/interneuron[]/fbaxon 1 {LFB_to_FB}  \
        {AWFB_to_FB}
    normalize_synapses /fb/interneuron[]/fbaxon  \
        /fb/interneuron[]/soma/Cl_channel -gaussian {NSYN_FB_from_FB}  \
        {SDSYN_FB_from_FB}
end

if ({EXTRA})
    //                            FB to PYR CONNECTIONS
    echo fb to pyr dend connections
    // do this over all the cells
    // connect to local area of cells
    region_connect /fb/interneuron[]/axon2 to  \
        /pyr/pyramidal[]/soma/#Ib_dend/Cl_channel with synapse -rel 1 1  \
        -100 -100 100 100 1 1 {-{RFB_to_PYR}} {-{RFB_to_PYR}}  \
        {RFB_to_PYR} {RFB_to_PYR} {PFB_to_PYR}

    //                            FB to PYR DELAYS
    echo fb to pyr dend delays
    gausspropagation_velocity /fb/interneuron[]/axon2  \
        {{VFB_PYR_MIN}/{SCALE}} {{VFB_PYR_MAX}/{SCALE}}  \
        {{VFB_PYR}/{SCALE}} {{SD_VFB_PYR}/{SCALE}}

    //                            FB to PYR WEIGHTS
    echo fb to pyr dend weights
    exponential_weight /fb/interneuron[]/axon2 {wpyr} {LFB_to_PYR}  \
        {{AWFB_to_PYR}*wpyr}
    normalize_synapses /fb/interneuron[]/axon2  \
        /pyr/pyramidal[]/soma/#Ib_dend/Cl_channel -gaussian  \
        {NSYN_PYR_from_FB} {SDSYN_PYR_from_FB}
end

/*
** perform synapse normalization
** this will adjust the number of convergent synapses per target 
** to within a given range.
** This will compensate for edge imbalances in synaptic enervation
** generated as a result of synaptic distribution based on numbers of 
** divergent synapses per source
*/
normalize_synapses /fb/interneuron[]/axon  \
    /pyr/pyramidal[]/soma/Cl_channel -gaussian {NSYN_PYR_from_FB}  \
    {SDSYN_PYR_from_FB}

