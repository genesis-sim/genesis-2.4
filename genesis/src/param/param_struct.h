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

#ifndef PARAM_STRUCT_H
#define PARAM_STRUCT_H

#include "struct_defs.h"

/* NOTE: The following is implementation-dependent: */

typedef unsigned char   Param_short;  /* 1 byte  */
typedef unsigned short  Param_medium; /* 2 bytes */
typedef unsigned long   Param_long;   /* 4 bytes */


/*
 * The following object is for doing "brute force" parameter searches,
 * and also for assessing the robustness of parameter values obtained
 * by one of the other methods by allowing one to sweep through parameter
 * space and assess how match values change with variations in parameters.
 */

struct paramtableBF_type
{
    ELEMENT_TYPE
    int      iteration_number;     /* for bookkeeping only */
    int      num_params;
    int      num_params_to_search; /* number of parameters to search over */
    short   *search;               /* flags: 1 if param is part
                                    * of the search                       */
    short   *type;                 /* of parameter: 0 = additive,
                                    * 1 = multiplicative                  */
    double  *range;                /* of parameter values                 */
    double  *min;                  /* of parameter values                 */
    double  *max;                  /* of parameter values                 */
    char   **label;                /* label of parameter,
                                    * for documentation purposes          */
    double  *current;              /* array of current parameter values   */
    double   current_match;        /* match values from most recent
                                    * simulation                          */
    double  *best;                 /* array of parameter values giving
                                    * best match so far                   */
    double   best_match;           /* best match value;
                                    * for bookkeeping                     */
    short    new_best_match;       /* flag for whether last match was
                                    * the best so far                     */
    short    done;                 /* flag: if 1, search is finished.     */
    char    *filename;             /* where parameter file information
                                    * is stored                           */
    short    alloced;

    /* Method-specific fields: */

    double  *orig;                 /* array of original parameter values  */
    int     *search_divisions;     /* number of points on the range
                                    * to test                             */
    double  *search_rangemod;      /* fraction of the full range to
                                    * search over                         */
    int     *search_count;         /* where we are in the search for
                                    * each param                          */
    double  *search_range;         /* actual ranges to search over        */
};




/*
 * The following object is for doing conjugate-gradient (gradient descent)
 * parameter searches.
 */

struct paramtableCG_type
{
    ELEMENT_TYPE
    int       iteration_number; /* for bookkeeping only                 */
    int       num_params;
    int       num_params_to_search;  /* # of parameters to search over     */
    short    *search;                /* flags: 1 if param is part
                                      * of the search; zero otherwise      */
    short    *type;                  /* of parameter: 0 = additive,
                                      * 1 = multiplicative                 */
    double   *center;                /* actual center of parameter values  */
    double   *realcenter;            /* actual center of parameter values  */
    double   *range;                 /* of parameter values */
    double   *realrange;             /* actual range of parameter values   */
    double   *min;                   /* of parameter values */
    double   *realmin;               /* actual min of parameter values     */
    double   *max;                   /* of parameter values */
    double   *realmax;               /* actual max of parameter values     */
    char    **label;                 /* label of parameter, for
                                      * documentation purposes             */
    double   *current;               /* array of current parameter values  */
    double   *realcurrent;           /* actual array of current parameter
                                      * values                             */
    double    current_match;         /* match values from most recent
                                      * simulation                         */
    double   *best;                  /* array of parameter values giving
                                      * best match so far                  */
    double    best_match;            /* best match value; for bookkeeping  */
    short     new_best_match;        /* flag for whether last match was
                                      * the best so far                    */
    short     done;                  /* flag for when parameter search
                                      * is done                            */
    char     *filename;              /* where parameter file information
                                      * is stored                          */
    short     alloced;

    /* Method-specific fields: */

    int       linemin_number;   /* which line minimization we're doing  */
    short     state;            /* of parameter search                  */

    /* The following fields are for calculating derivatives. */

    short     deriv_method;      /* 0 = do a proper derivative;
                                  * 1 = quick-and-dirty estimate        */
    double    orig_param;        /* original parameter value            */
    double    h;                 /* current spatial step size           */
    double    h_init;            /* initial spatial step size           */
    int       deriv_count;       /* number of function evaluations
                                  * for a given derivative              */
    short     deriv_state;       /* state of state transition table for
                                  * derivative calculations             */
    int       deriv_index;       /* index of parameter we're taking
                                  * the derivative of                   */
    double   *deriv_h_init;      /* initial values of h to use in
                                  * derivative calculations             */
    double   *deriv_h_decrease;  /* how fast to decrease h              */
    double   *deriv_h_min;       /* lowest permissible value of h       */
    double  **deriv_matrix;      /* internal storage for derivative
                                  * calculations                        */

    /*
     * The following fields are for calculating conjugate gradient
     * directions.
     */

    double   *deriv;      /* array of 1st partial derivatives:
                           * d(match)/d(param)                          */
    double   *g;          /* internal vector                            */
    double   *dir;        /* direction vector                           */
    double    tolerance;  /* tolerance of parameter search as a whole   */

    /* The following fields are for doing line minimizations. */

    short     linemin_state;         /* overall state of line minimization
                                      * routines                           */
    short     linemin_init;          /* flag: 1 means it's the first line
                                      * minimization                       */
    short     linemin_bracket_state; /* state of bracketing routine        */
    double   *linemin_bracket;       /* 3 points bracketing the minimum
                                      * of the line                        */
    double   *linemin_mbracket;      /* match values at bracket points     */
    short     linemin_brent_state;   /* state of Brent routines            */
    double    linemin_point;         /* point on line we're simulating     */
    double    linemin_match;         /* match value at linemin_point       */
    double    prev_linemin_match;    /* previous linemin match             */
    double   *linemin_origin;        /* point where line minimization
                                      * starts at                          */
    double    linemin_tolerance;     /* tolerance of line minimization
                                      * routines                           */
};




/*
 * The following object is for doing genetic algorithm (GA)-based
 * parameter searches.
 */


/*
 * Param is an object that can hold a parameter value of any size
 * in bit representation.
 */

typedef union
{
    Param_short  shortp;
    Param_medium mediump;
    Param_long   longp;
} Param;


/*
 * crossover_locations is an array for storing the values of positions
 * where crossovers will occur.
 */

typedef struct
{
    int   number;
    long *location;
} Crossover_locations;


struct paramtableGA_type
{
    ELEMENT_TYPE
    int         generation;            /* generation number,
                                        * for bookkeeping only             */
    int         num_tables;            /* number of parameter tables       */
    int         num_params;            /* number of parameters per table   */
    int         num_params_to_search;  /* number of parameters to
                                        * search over                      */
    short      *search;                /* array of flags; 0 = don't search
                                        * this param; 1 = do search        */
    short      *type;                  /* array of type of parameter:
                                        * 0 = additive, 1 = multiplicative */
    double     *center;                /* array of center values of
                                        * parameter table                  */
    double     *range;                 /* array of range values of
                                        * parameter table                  */
    char      **label;                 /* array of labels of parameters,
                                        * for documentation purposes       */
    double     *best;                  /* array of parameter values giving
                                        * best match (fitness) so far      */
    double      best_match;            /* best match (fitness) so far      */
    char       *filename;              /* where parameter file information
                                        * is stored                        */
    short       alloced;               /* flag for whether the tables
                                        * are allocated                    */

    /* Method-specific fields: */

    short       param_size;            /* size of parameters in bytes:
                                        * 1, 2, 4 are the only choices     */
    short       bits_per_parameter;    /* size of parameters in bits       */
    Param_long  max_parameter;         /* maximum size of parameter in bit
                                        * representation                   */
    Param     **param;                 /* two-dimensional parameter array  */
    Param     **temp;                  /* two-dimensional parameter array
                                        * for temporary storage            */
    double     *fitness;               /* array of fitness values for
                                        * parameter sets                   */
    double     *tempfitness;           /* array for temporary storage
                                        * of fitness values                */
    int        *fitrank;               /* array to store the fitness ranks
                                        * in order                         */
    double      min_fitness;           /* minimum fitness value            */
    double      max_fitness;           /* maximum fitness value            */
    double      avg_fitness;           /* average fitness value            */
    double      stdev_fitness;         /* standard deviation of
                                        * fitness values                   */
    int         min_fitness_index;     /* index of minimum fitness
                                        * in fitness array                 */
    int         max_fitness_index;     /* index of maximum fitness
                                        * in fitness array                 */
    double     *normfitness;           /* array of normalized fitness
                                        * values                           */
    double     *cumulfitness;          /* array of cumulative normalized
                                        * fitness values                   */
    int        *selectindex;           /* array of available indices of
                                        * parameter tables                 */
    int         preserve;              /* number of best matches to
                                        * retain unchanged                 */
    short       crossover_type;        /* type of crossover algorithm      */
    double      crossover_probability; /* probability of crossover         */
    int         crossover_number;      /* number of crossovers per
                                        * parameter string                 */
    short       crossover_break_param; /* flag: if 0, crossovers can't
                                        * break params                     */
    double      mutation_probability;  /* probability of mutation per bit  */
    short       use_gray_code;         /* flag: if nonzero, use Gray code
                                        * for encoding numbers             */

    /* The next 5 parameters are for controlling the RESTART process. */

    short       do_restart;            /* flag for whether to restart ever */
    int         restart_after;         /* restart after this many
                                        * unproductive generations         */
    int         restart_count;         /* count of unproductive
                                        * generations */
    double      old_fitness;           /* old fitness value, that we have
                                        * to do better than                */
    double      restart_thresh;        /* need to get this much above
                                        * old_fitness to not restart       */
};




/*
 * The following object is for doing simulated-annealing (SA)-based
 * parameter searches.
 */

struct paramtableSA_type
{
    ELEMENT_TYPE
    int       iteration_number;
    int       num_params;
    int       num_params_to_search; /* number of parameters to search over */
    short    *search;               /* array of flags; 0 = don't search
                                     * this param; 1 = do search           */
    short    *type;                 /* of parameter: 0 = additive,
                                     * 1 = multiplicative                  */
    double   *center;               /* of parameter values                 */
    double   *realcenter;           /* actual center of parameter values
                                     * in simplex                          */
    double   *range;                /* of parameter values                 */
    double   *realrange;            /* actual range of parameter values
                                     * in simplex                          */
    double   *min;                  /* of parameter values                 */
    double   *realmin;              /* actual min of parameter values
                                     * in simplex                          */
    double   *max;                  /* of parameter values                 */
    double   *realmax;              /* actual max of parameter values
                                     * in simplex                          */
    char    **label;                /* label of parameter, for
                                     * documentation purposes              */
    double   *current;              /* array of parameter values to be
                                     * simulated next                      */
    double    current_match;        /* match value of current point        */
    double   *best;                 /* array of parameter values giving
                                     * best match so far                   */
    double    best_match;           /* best match value; for bookkeeping   */
    int       best_match_iteration; /* iteration where best match
                                     * occurred; for bookkeeping only      */
    short     new_best_match;       /* flag: 1 if last match was the
                                     * best so far                         */
    short     done;                 /* Normally zero; set to 1 when the
                                     * simulation is finished.             */
    char     *filename;             /* where parameter file information
                                     * is stored                           */
    short     alloced;               /* flag: 1 means tables are allocated */

    /* Method-specific fields: */

    int       iterations_per_temp;
    double    temperature;          /* of annealing process                */
    double    inittemp;             /* initial temperature of annealing
                                     * process                             */
    short     annealing_method;     /* 0 = manual; 1 = linear decay;
                                     * 2 = exponential decay               */
    int       max_iterations;       /* for linear decay only               */
    double    annealing_rate;       /* for proportional decay only         */
    double    testtemp;             /* test for whether simulation is
                                     * finished when temp is below this    */
    double    tolerance;            /* If matches are within this distance
                                     * of each other we're done.           */
    int       stop_after;           /* If best match hasn't changed after
                                     * this many iterations then stop.     */
    int       restart_every;        /* call RESTART action every
                                     * x iterations                        */
    int       state;                /* of search process                   */
    int       next_index;           /* index of point on simplex to
                                     * evaluate next                       */
    double    simplex_init_noise;   /* proportion of initial noise in
                                     * simplex; a number in (0,1);
                                     * default = 0                         */
    double  **simplex;              /* points on the simplex:
                                     * (num_params+1) x (num_params)       */
    double   *simplex_match;        /* match values for each point
                                     * in the simplex                      */
    double   *partial_sum;          /* for calculating new points          */
    double   *test_point;           /* test point to be evaluated          */
    double    scale;                /* "typical" length scale of
                                     * starting points                     */
    double   *scalemod;             /* modifiers of length scales in
                                     * (num_params) dimensions;
                                     * default: all = 1                    */
};




/*
 * The following object is for doing stochastic search (SS)-based
 * parameter searches.
 */

struct paramtableSS_type
{
    ELEMENT_TYPE
    int       iteration_number; /* number of simulations so far           */
    int       num_params;       /* number of parameters in the table      */
    short    *search;           /* array of flags; 0 = don't search
                                 * this param; 1 = do search              */
    short    *type;             /* array of type of parameter:
                                 * 0 = additive, 1 = multiplicative       */
    double   *range;            /* array of range values of
                                 * parameter table                        */
    double   *min;              /* array of minimum values of
                                 * parameter table                        */
    double   *max;              /* array of maximum values of
                                 * parameter table                        */
    char    **label;            /* array of labels of parameters,
                                 * for documentation purposes             */
    double   *current;          /* array of current values of
                                 * parameter table                        */
    double   *best;             /* array of parameter values giving
                                 * best match so far                      */
    double    best_match;       /* best match value so far                */
    char     *filename;         /* where parameter file information
                                 * is stored                              */
    short     alloced;          /* flag: 1 means tables are allocated     */

    /* Method-specific fields: */

    int       round_number;     /* number of expansion-contraction cycles */
    double    variance;         /* current variance of gaussian
                                 * distribution                           */
    double    minvariance;      /* minimum variance of algorithm          */
    double    maxvariance;      /* maximum variance of algorithm          */
    double    addvarscale;      /* scaling factor for variances of
                                 * additive parameters                    */
    double    multvarscale;     /* scaling factor for variances of
                                 *  multiplicative parameters             */
    double    contract;         /* rate of variance contraction           */
};

#endif  /* PARAM_STRUCT_H */

