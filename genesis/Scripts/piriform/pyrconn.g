//genesis
str dst

echo
echo "                            PYRAMIDAL CELLS"
echo

// ===================================================================
//                                CONNECTIONS
// ===================================================================

//                             CAUDAL CONNECTIONS
echo caudal connections
if ({MULTI})
    dst = "/pyr/pyramidal[]/soma/supIb_dend/CA_Na_channel"
else
    dst = "/pyr/pyramidal[]/soma/CA_Na_channel"
end
/*
** spatial units are in mm
*/
// do this over all the cells
// connect to all cells caudal to it 
//  dont connect to the local region
region_connect /pyr/pyramidal[]/CAaxon to {dst} with synapse -rel 1 1  \
    -100 -100 100 100 2 1 0 -100 100 100 -1 {-{RPYR_to_LOCAL}}  \
    {-{RPYR_to_LOCAL}} {RPYR_to_LOCAL} {RPYR_to_LOCAL} {PPYR_to_CDIST}


//                             ROSTRAL CONNECTIONS
echo rostral connections
if ({MULTI})
    dst = "/pyr/pyramidal[]/soma/deepIb_dend/RA_Na_channel"
else
    dst = "/pyr/pyramidal[]/soma/RA_Na_channel"
end
// do this over all the cells
// connect to all cells rostral to it 
// dont connect to the local region
region_connect /pyr/pyramidal[]/RAaxon to {dst} with synapse -rel 1 1  \
    -100 -100 100 100 2 1 -100 -100 {-{PYR_DX}} 100 -1  \
    {-{RPYR_to_LOCAL}} {-{RPYR_to_LOCAL}} {RPYR_to_LOCAL}  \
    {RPYR_to_LOCAL} {PPYR_to_CDIST}

//                             LOCAL CONNECTIONS
echo local connections
if ({MULTI})
    dst = "/pyr/pyramidal[]/soma/III_dend/Local_Na_channel"
else
    dst = "/pyr/pyramidal[]/soma/Local_Na_channel"
end
// do this over all the cells
//  connect to the local region
//  dont connect to itself
region_connect /pyr/pyramidal[]/LRAaxon to {dst} with synapse -rel 1 1  \
    -100 -100 100 100 2 1 {-{RPYR_to_LOCAL}} {-{RPYR_to_LOCAL}}  \
    {-{PYR_DX}} {RPYR_to_LOCAL} -1 0 0 0 0 {PPYR_to_RLOCAL}

// do this over all the cells
//  connect to the local region
//  dont connect to itself
region_connect /pyr/pyramidal[]/LCAaxon to {dst} with synapse -rel 1 1  \
    -100 -100 100 100 2 1 0 {-{RPYR_to_LOCAL}} {RPYR_to_LOCAL}  \
    {RPYR_to_LOCAL} -1 0 0 0 0 {PPYR_to_CLOCAL}

//                             PYR to FF CONNECTIONS
echo pyr to ff connection
if ({LOCAL_FF})
// do this over all the cells
// connect to a local area of cells
region_connect /pyr/pyramidal[]/FFaxon to  \
    /ff/interneuron[]/soma/Na_channel with synapse -rel 1 1 -100 -100  \
    100 100 1 1 {-{RPYR_to_FF}} {-{RPYR_to_FF}} {RPYR_to_FF}  \
    {RPYR_to_FF} {PPYR_to_FF}
else
// do this over all the cells
// connect to all cells 
// dont connect to the local region
region_connect /pyr/pyramidal[]/FFaxon to  \
    /ff/interneuron[]/soma/Na_channel with synapse -rel 1 1 -100 -100  \
    100 100 2 1 -100 -100 100 100 -1 {-{RPYR_to_LOCAL}}  \
    {-{RPYR_to_LOCAL}} {RPYR_to_LOCAL} {RPYR_to_LOCAL} {PPYR_to_FF}
end
//                            PYR to FB CONNECTIONS
echo pyr to fb connection
// do this over all the cells
// connect to a local area of cells
region_connect /pyr/pyramidal[]/FBaxon to  \
    /fb/interneuron[]/soma/Na_channel with synapse -rel 1 1 -100 -100  \
    100 100 1 1 {-{RPYR_to_FB}} {-{RPYR_to_FB}} {RPYR_to_FB}  \
    {RPYR_to_FB} {PPYR_to_FB}

// ===================================================================
//                                DELAYS
// ===================================================================
// velocities in m/s or mm/msec

//                           CAUDAL DELAYS
echo caudal delays
gausspropagation_velocity /pyr/pyramidal[]/CAaxon  \
    {{VPYR_CA_MIN}/{SCALE}} {{VPYR_CA_MAX}/{SCALE}} {{VPYR_CA}/{SCALE}}  \
    {{SD_VPYR_CA}/{SCALE}}

//                           ROSTRAL DELAYS
echo rostral delays
gausspropagation_velocity /pyr/pyramidal[]/RAaxon  \
    {{VPYR_RA_MIN}/{SCALE}} {{VPYR_RA_MAX}/{SCALE}} {{VPYR_RA}/{SCALE}}  \
    {{SD_VPYR_RA}/{SCALE}}

//                           LOCAL DELAYS
echo local delays
gausspropagation_velocity /pyr/pyramidal[]/LRAaxon  \
    {{VPYR_RA_MIN}/{SCALE}} {{VPYR_RA_MAX}/{SCALE}} {{VPYR_RA}/{SCALE}}  \
    {{SD_VPYR_RA}/{SCALE}}
gausspropagation_velocity /pyr/pyramidal[]/LCAaxon  \
    {{VPYR_CA_MIN}/{SCALE}} {{VPYR_CA_MAX}/{SCALE}} {{VPYR_CA}/{SCALE}}  \
    {{SD_VPYR_CA}/{SCALE}}

//                           PYR to FF DELAYS
echo pyr to ff delays
gausspropagation_velocity /pyr/pyramidal[]/FFaxon  \
    {{VPYR_FF_MIN}/{SCALE}} {{VPYR_FF_MAX}/{SCALE}} {{VPYR_FF}/{SCALE}}  \
    {{SD_VPYR_FF}/{SCALE}}

//                           PYR to FB DELAYS
echo pyr to fb delays
gausspropagation_velocity /pyr/pyramidal[]/FBaxon  \
    {{VPYR_FB_MIN}/{SCALE}} {{VPYR_FB_MAX}/{SCALE}} {{VPYR_FB}/{SCALE}}  \
    {{SD_VPYR_FB}/{SCALE}}


// ===================================================================
//                                WEIGHTS
// ===================================================================
/*
** use an exponential distribution with distance for weights
** which represent the number of synaptic contacts
** the arguments are (peakweight spaceconstant minweight)
** The space constant is in mm.
*/

/*
** counts the convergent synapses onto a single pyramidal cell
*/
float  \
    cloc_sum = {expsum /pyr/pyramidal[x<={{CORTEX_X}/2}][y<={CORTEX_Y}] {-1.0/{LPYR_to_CLOCAL}} 1 {AWPYR_to_CLOCAL} {{CORTEX_X}/2} {{CORTEX_Y}/2}}

float  \
    cloc_sum = {expsum /pyr/pyramidal[x>={{CORTEX_X}/2 - {RPYR_to_LOCAL}}][y>={{CORTEX_Y}/2 - {RPYR_to_LOCAL}}][x<={{CORTEX_X}/2}][y<={{CORTEX_Y}/2 + {RPYR_to_LOCAL}}] {-1.0/{LPYR_to_CLOCAL}} 1 {AWPYR_to_CLOCAL} {{CORTEX_X}/2} {{CORTEX_Y}/2}}

float  \
    rloc_sum = {expsum /pyr/pyramidal[x>={{CORTEX_X}/2}][y>={{CORTEX_Y}/2 - {RPYR_to_LOCAL}}][x<={{CORTEX_X}/2 + {RPYR_to_LOCAL}}][y<={{CORTEX_Y}/2 + {RPYR_to_LOCAL}}] {-1.0/{LPYR_to_RLOCAL}} 1 {AWPYR_to_RLOCAL} {{CORTEX_X}/2} {{CORTEX_Y}/2}}

float  \
    caloc_sum = {expsum /pyr/pyramidal[x>={{CORTEX_X}/2 - {RPYR_to_LOCAL}}][y>={{CORTEX_Y}/2 - {RPYR_to_LOCAL}}][x<={{CORTEX_X}/2}][y<={{CORTEX_Y}/2 + {RPYR_to_LOCAL}}] {-1.0/{LPYR_to_CA}} 1 {AWPYR_to_CA} {{CORTEX_X}/2} {{CORTEX_Y}/2}}

float  \
    raloc_sum = {expsum /pyr/pyramidal[x>={{CORTEX_X}/2}][y>={{CORTEX_Y}/2 - {RPYR_to_LOCAL}}][x<={{CORTEX_X}/2 + {RPYR_to_LOCAL}}][y<={{CORTEX_Y}/2 + {RPYR_to_LOCAL}}] {-1.0/{LPYR_to_RA}} 1 {AWPYR_to_RA} {{CORTEX_X}/2} {{CORTEX_Y}/2}}

float  \
    ca_sum = {expsum /pyr/pyramidal[x<={{CORTEX_X}/2}][y<={CORTEX_Y}] {-1.0/{LPYR_to_CA}} 1 {AWPYR_to_CA} {{CORTEX_X}/2} {{CORTEX_Y}/2}} - caloc_sum

float  \
    ra_sum = {expsum /pyr/pyramidal[x>={{CORTEX_X}/2}][x<={CORTEX_X}][y<={CORTEX_Y}] {-1.0/{LPYR_to_RA}} 1 {AWPYR_to_RA} {{CORTEX_X}/2} {{CORTEX_Y}/2}} - raloc_sum

/*
** counts the convergent synapses onto a single fb interneuron
*/
float  \
    fb_sum = {expsum /pyr/pyramidal[x>={{CORTEX_X}/2 - {RPYR_to_FB}}][y>={{CORTEX_Y}/2 - {RPYR_to_FB}}][x<={{CORTEX_X}/2 + {RPYR_to_FB}}][y<={{CORTEX_Y}/2 + {RPYR_to_FB}}] {-1.0/{LPYR_to_FB}} 1 {AWPYR_to_FB} {{CORTEX_X}/2} {{CORTEX_Y}/2}}

/*
** counts the convergent synapses onto a single ff interneuron
*/
float  \
    ff_sum = {expsum /pyr/pyramidal[x>={{CORTEX_X}/2 - {RPYR_to_FF}}][y>={{CORTEX_Y}/2 - {RPYR_to_FF}}][x<={{CORTEX_X}/2 + {RPYR_to_FF}}][y<={{CORTEX_Y}/2 + {RPYR_to_FF}}] {-1.0/{LPYR_to_FF}} 1 {AWPYR_to_FF} {{CORTEX_X}/2} {{CORTEX_Y}/2}}

float wca = {NSYN_PYR_from_CA}/(ca_sum*{PPYR_to_CDIST})
float wra = {NSYN_PYR_from_RA}/(ra_sum*{PPYR_to_RDIST})
float wrloc = {NSYN_PYR_from_RLOCAL}/(rloc_sum*{PPYR_to_RLOCAL})
float wcloc = {NSYN_PYR_from_CLOCAL}/(cloc_sum*{PPYR_to_CLOCAL})
float wfb = {NSYN_FB_from_PYR}/(fb_sum*{PPYR_to_FB})
float wff = {NSYN_FF_from_PYR}/(ff_sum*{PPYR_to_FF})

//                            CAUDAL WEIGHTS
echo caudal weights
exponential_weight /pyr/pyramidal[]/CAaxon {wca} {LPYR_to_CA}  \
    {{AWPYR_to_CA}*wca}

//                            ROSTRAL WEIGHTS
echo rostral weights
exponential_weight /pyr/pyramidal[]/RAaxon {wra} {LPYR_to_RA}  \
    {{AWPYR_to_RA}*wra}

//                            LOCAL WEIGHTS
echo local weights
exponential_weight /pyr/pyramidal[]/LRAaxon {wrloc} {LPYR_to_RLOCAL}  \
    {{AWPYR_to_RLOCAL}*wrloc}

exponential_weight /pyr/pyramidal[]/LCAaxon {wcloc} {LPYR_to_CLOCAL}  \
    {{AWPYR_to_CLOCAL}*wcloc}

//                            PYR to FF WEIGHTS
echo pyr to ff weights
exponential_weight /pyr/pyramidal[]/FFaxon {wff} {LPYR_to_FF}  \
    {{AWPYR_to_FF}*wff}

//                            PYR to FB WEIGHTS
echo pyr to fb weights
exponential_weight /pyr/pyramidal[]/FBaxon {wfb} {LPYR_to_FB}  \
    {{AWPYR_to_FB}*wfb}

/*
** normalize local interneuron weights
*/
normalize_synapses /pyr/pyramidal[]/FBaxon  \
    /fb/interneuron[]/soma/Na_channel -gaussian {NSYN_FB_from_PYR}  \
    {SDSYN_FB_from_PYR}

normalize_synapses /pyr/pyramidal[]/FFaxon  \
    /ff/interneuron[]/soma/Na_channel -gaussian {NSYN_FF_from_PYR}  \
    {SDSYN_FF_from_PYR}
