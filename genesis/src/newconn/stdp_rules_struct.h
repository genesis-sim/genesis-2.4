#include "struct_defs.h"

struct stdp_rules_type
{
  ELEMENT_TYPE
  /* Fields that are set by the user */
  char *cellpath; /* full wildcard path to the postsynaptic cells */
  char *synpath;  /* relative path to the cell synchan to be modified */
  char *spikepath;  /* relative path to the cell spikegen */
  float tau_pre;  /* decay constant for pre before post spike */
  float tau_post; /* decay constant for post before pre spike */
  float min_dt;   /* minimum time diff to account for latency */
  float max_weight; /* maximum synaptic weight allowed */
  float min_weight; /* minimum synaptic weight allowed */
  float dAplus;     /* increment for postive weight changes */
  float dAminus;    /* increment for negative weight changes */
  short change_weights; /* flag: 0 = plasticity off, otherwise on */
  short debug_level; /* flag: 0 = no messages, 1 = some messages, 2 = more messages */
  /* Fields that are set by the 'stdp_rules' element */
  ElementList *celllist; /* GENESIS ElementList of cells */

};
