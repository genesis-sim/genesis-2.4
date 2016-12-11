#include "struct_defs.h"

struct facdep_rules_type
{
  ELEMENT_TYPE
  /* Fields that are set by the user */
  char *cellpath; /* full wildcard path to the postsynaptic cells */
  char *synpath;  /* relative path to the cell synchan to be modified */
  float dF  ;      /* increment for facilitating weight changes */
  float dD1  ;     /* increment for depression factor D1 weight changes */
  float dD2  ;     /* increment for depression factor D2 weight changes */
  float tau_F;     /* decay constant for facilitating weight changes */
  float tau_D1;    /* decay constant for depression factor D1 weight changes */
  float tau_D2;    /* decay constant for depression factor D2 weight changes */
  short use_depdep ;    /* flag: 0 = use F and D1, otherwise use D1 and D2 */
  short change_weights; /* flag: 0 = plasticity off, otherwise on */
  short debug_level; /* flag: 0 = no messages, 1 = some messages, 2 = more messages */
  /* Fields that are set by the 'facdep_rules' element */
  ElementList *celllist; /* GENESIS ElementList of cells */

};
