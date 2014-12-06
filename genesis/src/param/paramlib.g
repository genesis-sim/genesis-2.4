/*
 *
 * Copyright (c) 1997, 1998, 1999 Michael Christopher Vanier
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * In no event shall Michael Vanier or the Genesis Developer's Group
 * be liable to any party for direct, indirect, special, incidental, or
 * consequential damages arising out of the use of this software and its
 * documentation, even if Michael Vanier and the Genesis Developer's
 * Group have been advised of the possibility of such damage.
 *
 * Michael Vanier and the Genesis Developer's Group specifically
 * disclaim any warranties, including, but not limited to, the implied
 * warranties of merchantability and fitness for a particular purpose.
 * The software provided hereunder is on an "as is" basis, and Michael
 * Vanier and the Genesis Developer's Group have no obligation to
 * provide maintenance, support, updates, enhancements, or modifications.
 *
 */

// genesis


//**************************************************************************
//**                           OBJECT TABLE                               **
//**************************************************************************

//==========================================================================
//  name          data               function         class
//==========================================================================

newclass  param
addaction TABCREATE       200
addaction TABDELETE       202
addaction RANDOMIZE       1003
addaction REPRODUCE       1004
addaction CROSSOVER       1005
addaction INVERT          1006
addaction MUTATE          1007
addaction FITSTATS        1008
addaction RECENTER        1009
addaction UPDATE_PARAMS   1010
addaction ACCEPT          1011
addaction DISPLAY         1012
addaction SAVE3           1013
addaction RESTORE3        1014
addaction INITSEARCH      1015
addaction RESTART         1016
addaction EVALUATE        1017
addaction DISPLAY2        1018
addaction RESTART2        1019
addaction LOADBEST        1020
addaction UPDATE_PARAMS2  1021
addaction LOADTRACE       1022
addaction NEXTTRACE       1023
addaction RESETALL        1024
addaction SAVEBEST        1025
addaction RESTOREBEST     1026



// This object is for brute-force (BF) parameter searches.

object  paramtableBF  paramtableBF_type  ParamtableBF  param  \
    -author   "Mike Vanier"   \
    -actions  CREATE          \
              TABCREATE       \
              DELETE          \
              TABDELETE       \
              INITSEARCH      \
              EVALUATE        \
              UPDATE_PARAMS   \
              SAVE            \
              SAVEBEST        \
              RESTORE         \
              RESTOREBEST     \
              DISPLAY         \
              CHECK           \
    -readonly  iteration_number      "number of simulations so far"        \
    -readonly  num_params            "number of parameters in the table"   \
    -readonly  num_params_to_search  "number of parameters to search over" \
    -readwrite search                "which parameters to search over"     \
    -readwrite type       "array of type of parameter"                     \
    -readwrite range      "array of range values of parameter table"       \
    -readwrite min        "array of minimum values of parameter table"     \
    -readwrite max        "array of maximum values of parameter table"     \
    -readwrite label      "array of labels of parameters"                  \
    -readonly  current    "array of current values of parameter table"     \
    -readonly  current_match  "match of most recent simulation"            \
    -readonly  best       "array of best parameter values so far"          \
    -readonly  best_match "best match so far (bookkeeping only)"           \
    -readonly  new_best_match "flag: 1 if last match was the best so far"  \
    -readonly  done       "flag: if 1, search is finished."                \
    -readwrite filename   "name of file to store parameters etc. in"       \
    -readonly  alloced    "flag for whether the tables are allocated"      \
    -readwrite orig       "array of original values of parameter table"    \
    -readwrite search_divisions  "number of divisions per parameter"       \
    -readwrite search_rangemod   "fraction of full range to search over"   \
    -readonly  search_count      "where we are in the search"              \
    -hidden    search_range      "actual ranges to search over"            \
    -description "An object for storing a parameter table and computing "  \
                 "match values for specified regions of parameter space."

addfunc initparamBF do_initparamBF int



// This object is for conjugate gradient (gradient descent)
// parameter searches.

object  paramtableCG  paramtableCG_type  ParamtableCG  param  \
    -author   "Mike Vanier"   \
    -actions  CREATE          \
              TABCREATE       \
              DELETE          \
              TABDELETE       \
              INITSEARCH      \
              RANDOMIZE       \
              EVALUATE        \
              UPDATE_PARAMS   \
              UPDATE_PARAMS2  \
              LOADBEST        \
              RESTART         \
              SAVE            \
              SAVEBEST        \
              RESTORE         \
              RESTOREBEST     \
              DISPLAY         \
              CHECK           \
    -readonly  iteration_number "number of simulations so far"            \
    -readwrite num_params       "number of parameters"                    \
    -readonly  num_params_to_search  "number of params to search over"    \
    -readwrite search                "flags: 1 = param is in search"      \
    -readwrite type           "of param: 0: additive, 1: multiplicative"  \
    -readwrite center         "actual center of parameter values"         \
    -hidden    realcenter     "actual center of parameter values"         \
    -readwrite range          "of parameter values"                       \
    -hidden    realrange      "actual range of parameter values"          \
    -readwrite min            "of parameter values"                       \
    -hidden    realmin        "actual min of parameter values"            \
    -readwrite max            "of parameter values"                       \
    -hidden    realmax        "actual max of parameter values"            \
    -readonly  label          "label of parameter"                        \
    -readonly  current        "array of current parameter values"         \
    -hidden    realcurrent    "actual current parameter values"           \
    -readonly  current_match  "match of most recent simulation"           \
    -readonly  best           "best parameter values"                     \
    -readonly  best_match     "best match value"                          \
    -readonly  new_best_match "flag: 1 if last match was the best so far" \
    -readonly  done           "flag: 1 means search is done"              \
    -readwrite filename       "where param file information is stored"    \
    -readonly  alloced        "flag for whether the tables are allocated" \
    -readonly  linemin_number   "which line minimization we're doing"     \
    -readonly  state            "of parameter search"                     \
    -readwrite deriv_method     "0 = proper; 1 = quick-and-dirty" 0       \
    -hidden    orig_param       "original parameter value"                \
    -hidden    h                "spatial step size, for deriv calc"       \
    -hidden    h_init           "initial spatial step size"               \
    -hidden    deriv_count      "# func evaluations for a given deriv"    \
    -hidden    deriv_state      "state of derivative routines"            \
    -readonly  deriv_index      "index of param we're taking deriv of"    \
    -readwrite deriv_h_init     "init vals of h to use in deriv calc"     \
    -readwrite deriv_h_decrease "how fast to decrease h"                  \
    -readwrite deriv_h_min      "lowest permissible value of h"           \
    -hidden    deriv_matrix     "internal storage for deriv calcs"        \
    -readonly  deriv            "array of d(match)/d(param)"              \
    -hidden    g                "internal vector"                         \
    -readonly  dir              "direction vector"                        \
    -readwrite tolerance        "tolerance of param search"               \
    -hidden    linemin_state    "overall state of line min routines"      \
    -hidden    linemin_init     "flag: 1 = first line min"                \
    -hidden    linemin_bracket_state "state of bracketing routine"        \
    -hidden    linemin_bracket       "3 points bracketing line min"       \
    -hidden    linemin_mbracket      "match values at bracket points"     \
    -hidden    linemin_brent_state   "state of Brent routines"            \
    -hidden    linemin_point         "point on line we're simulating"     \
    -hidden    linemin_match         "match value at linemin_point"       \
    -hidden    prev_linemin_match    "previous linemin match"             \
    -hidden    linemin_origin        "point where line min starts at"     \
    -readwrite linemin_tolerance     "tolerance of line min routines"     \
    -description "An object for storing a parameter table and computing"  \
                 "new parameter values using a conjugate gradient"        \
                 "algorithm."

addfunc initparamCG do_initparamCG  int



// This object is for genetic-algorithm (GA) based parameter searches.

object  paramtableGA  paramtableGA_type  ParamtableGA  param  \
    -author   "Mike Vanier"   \
    -actions  CREATE          \
              TABCREATE       \
              DELETE          \
              TABDELETE       \
              INITSEARCH      \
              RANDOMIZE       \
              UPDATE_PARAMS   \
              REPRODUCE       \
              CROSSOVER       \
              MUTATE          \
              FITSTATS        \
              RECENTER        \
              RESTART         \
              SAVE            \
              SAVEBEST        \
              RESTORE         \
              RESTOREBEST     \
              CHECK           \
    -readwrite      generation            "generation number"              \
    -readonly       num_tables            "# of param tables"              \
    -readonly       num_params            "# of params per table"          \
    -readonly       num_params_to_search  "# of params to search over"     \
    -readwrite      search         "flags: 1 = search this param"          \
    -readwrite      type           "param types"                           \
    -readwrite      center         "center values of param table"          \
    -readwrite      range          "range values of param table"           \
    -readwrite      label          "labels of params"                      \
    -readwrite      best           "best matching param values"            \
    -readwrite      best_match     "best match (fitness) value so far"     \
    -readwrite      filename         "where param file info is stored"     \
    -readonly       alloced          "flag: are tables allocated?"         \
    -readonly       param_size         "size of params in bytes" 4         \
    -hidden         bits_per_parameter "size of params in bits"            \
    -hidden         max_parameter      "max size of param in bit rep"      \
    -readwrite      param          "2D parameter array"                    \
    -hidden         temp           "2D parameter array for temp storage"   \
    -readwrite      fitness        "fitness values for param sets"         \
    -hidden         tempfitness    "temp storage of fitness values"        \
    -hidden         fitrank        "fitness ranks in order"                \
    -readonly       min_fitness    "minimum fitness value"                 \
    -readonly       max_fitness    "maximum fitness value"                 \
    -readonly       avg_fitness    "average fitness value"                 \
    -readonly       stdev_fitness  "std dev of fitness values"             \
    -readonly       min_fitness_index  "index of min fit in fit array"     \
    -readonly       max_fitness_index  "index of max fit in fit array"     \
    -readonly       normfitness        "normalized fitness values"         \
    -readonly       cumulfitness       "cumulative norm fitness values"    \
    -hidden         selectindex        "available indices of param tables" \
    -readwrite      preserve           "# of best matches to retain" 1     \
    -readwrite      crossover_type     "type of CO algorithm" 1            \
    -readwrite      crossover_probability "probability of CO" 0.5          \
    -readwrite      crossover_number      "# of COs per param string" 1    \
    -readwrite      crossover_break_param "0 = COs can't break params" 0   \
    -readwrite      mutation_probability  "prob of mutation per bit" 0.01  \
    -readwrite      use_gray_code    "flag: use Gray code encoding" 1      \
    -readwrite      do_restart       "1 = restart periodically" 1          \
    -readwrite      restart_after    "restart after N bad gens" 25         \
    -readonly       restart_count    "count of unproductive gens" 0        \
    -readonly       old_fitness      "fit val we have to beat" 0.0         \
    -readwrite      restart_thresh   "threshold for restarting" 1.0        \
    -description    "An object for storing parameter tables and "          \
                    "computing the next generation according to a "        \
                    "genetic algorithm."

// I'd prefer if the set/getparamGA functions were eliminated and replaced
// by an overloading of SET and SHOW, or else have a next[] array for the
// next series of params to be simulated (which would mean you could just
// use getfield).

addfunc getparamGA  do_getparamGA   double
addfunc setparamGA  do_setparamGA
addfunc initparamGA do_initparamGA  int



// This object is for simulated-annealing (SA) based parameter searches.
// All parameters in simplex array are stored as additive parameters.
// The "realcenter", "realrange", "realmin" and "realmax" arrays are
// the same as the center, range, min and max arrays for additive parameters
// but are the logs of those for multiplicative parameters.  The user can
// safely ignore them.

object  paramtableSA  paramtableSA_type  ParamtableSA  param  \
    -author   "Mike Vanier"   \
    -actions  CREATE          \
              TABCREATE       \
              DELETE          \
              TABDELETE       \
              INITSEARCH      \
              EVALUATE        \
              UPDATE_PARAMS   \
              RECENTER        \
              RESTART         \
              RESTART2        \
              SAVE            \
              SAVEBEST        \
              RESTORE         \
              DISPLAY         \
              DISPLAY2        \
              CHECK           \
    -readonly   iteration_number      "# iterations in sim so far"         \
    -readwrite  num_params    "# of parameters"                            \
    -readonly   num_params_to_search  "# of parameters to search over"     \
    -readwrite  search     "flags; 1 = do search this param"               \
    -readwrite  type       "of param: 0: additive, 1: multiplicative"      \
    -readwrite  center     "of parameter values"                           \
    -hidden     realcenter "actual center of parameter values in simplex"  \
    -readwrite  range      "of param values"                               \
    -hidden     realrange  "actual range of parameter values in simplex"   \
    -readwrite  min        "of param values"                               \
    -hidden     realmin    "actual min of parameter values in simplex"     \
    -readwrite  max        "of param values"                               \
    -hidden     realmax    "actual max of parameter values in simplex"     \
    -readwrite  label      "label of param"                                \
    -readonly   current       "array of param values to be simulated next" \
    -readonly   current_match "match value of last point simulated"        \
    -readonly   best      "array of param values giving best match so far" \
    -readonly   best_match "best match value"                              \
    -hidden     best_match_iteration  "iteration # w/ best match" 0        \
    -readonly   new_best_match "flag: 1 if last match was the best so far" \
    -readonly   done          "flag: 1 means search is done"               \
    -readwrite  filename      "where parameter file information is stored" \
    -readonly   alloced       "flag: 1 means tables are allocated"         \
    -readwrite  iterations_per_temp   "# iterations per temperature" 1     \
    -readwrite  temperature           "of annealing process"               \
    -readwrite  inittemp         "init temp of annealing process" 0.0      \
    -readwrite  annealing_method "0: manual; 1: lin decay; 2: exp decay" 0 \
    -readwrite  max_iterations   "for linear decay only"                   \
    -readwrite  annealing_rate   "for proportional decay only"             \
    -readwrite  testtemp      "test for sim done when temp is <= this" 0.0 \
    -readwrite  tolerance     "criterion for stopping search"              \
    -readwrite  stop_after    "(this many unproductive iterations)" 200    \
    -readwrite  restart_every "call RESTART action every x iterations" 0   \
    -readonly   state         "of search process"                          \
    -hidden     next_index    "index of point on simplex to evaluate next" \
    -readwrite  simplex_init_noise  "initial noise in simplex" 0.0         \
    -readonly   simplex       "points on the simplex"                      \
    -readwrite  simplex_match "match values for each point in the simplex" \
    -hidden     partial_sum   "for calculating new points"                 \
    -hidden     test_point    "test point to be evaluated"                 \
    -readwrite  scale         "length scale of starting points" 1.0        \
    -readwrite  scalemod      "modifiers of length scales"                 \
    -description "An object for storing a parameter table and computing"   \
                 "new parameter values according to a simulated annealing" \
                 "algorithm."

addfunc initparamSA do_initparamSA  int



// This object is for stochastic-search (SS) based parameter searches.

object  paramtableSS  paramtableSS_type  ParamtableSS  param  \
    -author   "Mike Vanier"   \
    -actions  CREATE          \
              TABCREATE       \
              DELETE          \
              TABDELETE       \
              INITSEARCH      \
              RANDOMIZE       \
              UPDATE_PARAMS   \
              ACCEPT          \
              RECENTER        \
              SAVE            \
              SAVEBEST        \
              RESTORE         \
              DISPLAY         \
              CHECK           \
    -readonly       iteration_number "# of simulations so far"           \
    -readonly       num_params   "# of params in the table"              \
    -readwrite      search       "flags; 1 = search this param"          \
    -readwrite      type         "of params: 0 = add, 1 = mult"          \
    -readwrite      range        "range values of param table"           \
    -readwrite      min          "minimum values of param table"         \
    -readwrite      max          "maximum values of param table"         \
    -readwrite      label        "labels of params"                      \
    -readwrite      current      "current values of param table"         \
    -readwrite      best         "param values giving best match so far" \
    -readonly       best_match   "best match value so far"               \
    -readwrite      filename     "name of file to store params etc. in"  \
    -readonly       alloced      "flag: 1 means tables are allocated"    \
    -readonly       round_number     "# of expansion-contraction cycles" \
    -readwrite      variance     "current variance of gaussian dist"     \
    -readwrite      minvariance  "minimum variance of algorithm" 0.05    \
    -readwrite      maxvariance  "maximum variance of algorithm" 0.5     \
    -readwrite      addvarscale  "scaling for vars of add params" 1.0    \
    -readwrite      multvarscale "scaling for vars of mult params" 1.0   \
    -readwrite      contract     "rate of contraction of variance" 0.95  \
    -description    "An object for storing a param table and computing"  \
                    "new param values according to a stochastic search"  \
                    "algorithm."

addfunc initparamSS do_initparamSS  int


// Functions to test the match between genesis output and sample
// (e.g. experimental) data:

addfunc  gen2spk     Gen2spk
addfunc  spkcmp      Spkcmp  double
addfunc  shapematch  do_shapematch double


// utility function to determine which parameters to search over:

addfunc  setsearch  do_setsearch  int


// other utilities:

addfunc  shiftarg ShiftArg char*
addfunc  remarg   RemArg   char*


