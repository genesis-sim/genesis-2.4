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

#include <math.h>
#include "param_ext.h"

/* Forward declaration: */
int
ParamtableSA_RESTART();

/* a utility function, for debugging */
#if 0
void
print_simplex(struct paramtableSA_type *element)
{
    int i, j;
    int simplex_size = element->num_params + 1;

    puts("printing simplex...");

    for (i = 0; i < simplex_size; i++)
    {
        printf("simplex[%d] =\t", i);

        for (j = 0; j < element->num_params; j++)
        {
            if (element->type[j] == 0)
                printf("%f\t", element->simplex[i][j]);
            else
                printf("%f\t", exp(element->simplex[i][j]));
        }

        printf("\n");
    }

    printf("\n");
}
#endif




/***********************************************************************
 * The following functions are utility functions for paramtableSA,
 * made into separate functions for convenience in programming and
 * modification.
 ***********************************************************************/

/*
 * get_next_parameter_set
 *
 * FUNCTION
 *     Transforms the parameters in a simplex point to a form
 *     that the user can access.
 *
 * ARGUMENTS
 *     element -- address of element
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
get_next_parameter_set(element)
    struct paramtableSA_type *element;
{
    int i;
    int state = element->state;

    /*
     * If we're in state 0 or 5, just grab the next point on the simplex.
     * If we're in any other state, use the values in element->test_point.
     */

    for (i = 0; i < element->num_params; i++)
    {
        if (state == 0 || state == 5)
        {
            int next = element->next_index;

            if (element->type[i] == 0) /* additive parameter       */
                element->current[i] = element->simplex[next][i];
            else                         /* multiplicative parameter */
                element->current[i] = exp(element->simplex[next][i]);
        }
        else
        {
            if (element->type[i] == 0)  /* additive parameter */
                element->current[i] = element->test_point[i];
            else                        /* multiplicative parameter */
                element->current[i] = exp(element->test_point[i]);
        }
    }
}




/*
 * calc_partial_sums
 *
 * FUNCTION
 *     Recalculates partial sums on simplex.
 *
 * ARGUMENTS
 *     element -- address of element
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
calc_partial_sums(element)
    struct paramtableSA_type *element;
{
    int    i, j, simplex_size;
    double sum;

    simplex_size = element->num_params_to_search + 1;

    for (i = 0; i < element->num_params; i++)
    {
        if (element->search[i] == 0)
        {
            /* this parameter is not part of the search */
            continue;
        }

        for (j = 0, sum = 0.0; j < simplex_size; j++)
            sum += element->simplex[j][i];

        element->partial_sum[i] = sum;
    }
}




/*
 * calc_ranks
 *
 * FUNCTION
 *     Determines which points on the simplex are
 *     the best, second-worst, and worst.
 *
 * ARGUMENTS
 *     element             -- address of element
 *     best_index          -- index of best matching point on simplex
 *     second_worst_index  -- index of second worst point
 *     worst_index         -- index of worst point
 *     best_match          -- best match on simplex
 *     second_worst_match  -- second worst match
 *     worst_match         -- worst match
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
calc_ranks(element, best_index, worst_index, best_match,
	   second_worst_match, worst_match)
    struct paramtableSA_type *element;
    int *best_index;
    int *worst_index;
    double *best_match;
    double *second_worst_match;
    double *worst_match;
{

    int    i;
    int    simplex_size = element->num_params_to_search + 1;
    int    besti, worsti;                 /* indices */
    double bestm, second_worstm, worstm;  /* matches */
    double testm;
    double tt = -element->temperature;

    /*
     * Figure out which are the best, second-worst, and worst points on the
     * simplex.  Remember, best has the *lowest* match value.  Also get the
     * indices of the best, second-worst, and worst points.
     */

    besti  = 0;
    worsti = 1;

    /* Add noise to all match values. */

    second_worstm = bestm = element->simplex_match[0] + tt * log(urandom());
    worstm        = element->simplex_match[1] + tt * log(urandom());

    if (bestm > worstm)
    {
        worsti        = 0;
        besti         = 1;
        second_worstm = worstm;
        worstm        = bestm;
        bestm         = second_worstm;

        /* Now second_worstm is the same as bestm. */
    }

    for (i = 2; i < simplex_size; i++)
    {
        testm = element->simplex_match[i] + tt * log(urandom());

        if (testm <= bestm)       /* better than the best so far */
        {
            besti = i;
            bestm = testm;
        }
        else if (testm > worstm)  /* worse than the worst so far */
        {
            second_worstm = worstm;
            worsti        = i;
            worstm        = testm;
        }
        else if (testm > second_worstm)   /* second worst so far */
        {
            second_worstm = testm;
        }
    }

    *best_index         = besti;
    *worst_index        = worsti;
    *best_match         = bestm;
    *second_worst_match = second_worstm;
    *worst_match        = worstm;
}




/*
 * calc_point
 *
 * FUNCTION
 *     Determines which parameter set to simulate next.  Puts the result
 *     into test_point.
 *
 * ARGUMENTS
 *     element -- address of element
 *     index   -- of a point in the simplex (usually the worst point)
 *     fac     -- factor to extrapolate by
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
calc_point(element, index, fac)
    struct paramtableSA_type *element;
    int index;
    double fac;
{
    int i, num;
    double fac1, fac2, fullrange;

    num = element->num_params_to_search;

    /* Calculate the partial sums of the simplex values. */

    calc_partial_sums(element);

    fac1 = (1.0 - fac) / num;
    fac2 = fac1 - fac;

    for (i = 0; i < element->num_params; i++)
    {
        if (element->search[i] == 0)
        {
            /* this parameter is not part of the search */
            element->test_point[i] = element->realcenter[i];
            continue;
        }

        element->test_point[i] = element->partial_sum[i] * fac1 -
            element->simplex[index][i] * fac2;
    }

    /*
     * If our test point is outside of the limits in any dimension,
     * wrap around to the other side of that dimension.
     *
     * FIXME: make this an OPTION; the alternative is to set the
     *        value outside the limit to the limit.
     */

    for (i = 0; i < element->num_params; i++)
    {
        if (element->search[i] == 0)
        {
            /* this parameter is not part of the search */
            continue;
        }

        fullrange = element->realmax[i] - element->realmin[i];

        while (element->test_point[i] < element->realmin[i])
            element->test_point[i] += fullrange;

        while (element->test_point[i] > element->realmax[i])
            element->test_point[i] -= fullrange;
    }
}




/*
 * evaluate_match
 *
 * FUNCTION
 *     Evaluates the match value relative to the match of a given point
 *     on the simplex.  If it's better than the point on the simplex with
 *     index "old_index" (which is usually but not always the worst point)
 *     then it replaces that point with the new point.
 *
 * ARGUMENTS
 *     element   -- address of element
 *     match     -- match of new point (test_point)
 *     old_match -- match of a point in the simplex
 *     old_index -- index of a point in the simplex
 *
 * RETURN VALUE
 *     double
 *
 * AUTHOR
 *     Mike Vanier
 */

double
evaluate_match(element, match, old_match, old_index)
    struct paramtableSA_type *element;
    double match;
    double *old_match;
    int old_index;
{
    int    i;
    double noisy_match;
    double tt = -element->temperature;

    /*
     * We added thermal perturbations in calc_ranks() above; here we
     * subtract thermal perturbations in order to make the algorithm want
     * to accept the new point.
     */

    noisy_match = match - tt * log(urandom());

    if (noisy_match < *old_match)
    {
        /* Accept new point in place of old point. */
        element->simplex_match[old_index] = match;

        for (i = 0; i < element->num_params; i++)
            element->simplex[old_index][i] = element->test_point[i];

        *old_match = noisy_match;
    }

    /*
     * FIXME?  It seems weird to return noisy_match and also (sometimes) to
     * set *old_match to be equal to noisy_match.
     */

    return noisy_match;
}




/*
 * is_search_done
 *
 * FUNCTION
 *     Tests to see if (a) the temperature is below a threshold,
 *     (b) if all match values on simplex are within a given distance
 *     of each other, and (c) the number of iterations between the last
 *     simulation which gave the best match and the current simulation
 *     is greater than the stop_after field.  If so, the field "done" is
 *     set to 1.  This field can be used to tell the user when the
 *     simulation is done or the user can ignore it.
 *
 * ARGUMENTS
 *     element -- address of element
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
is_search_done(element)
    struct paramtableSA_type *element;
{
    int    i;
    int    simplex_size = element->num_params_to_search + 1;
    double match, best_match, worst_match;

    /*
     * If temperature is above threshold then we're not done.
     */

    if (element->temperature > element->testtemp)
    {
        element->done = 0;
        return;
    }

    /*
     * Calculate the difference between the highest and the lowest match
     * values.  If it's less than element->tolerance then we're done.
     *
     * Also, if the number of iterations between the last simulation which
     * gave a best match and the current iteration is greater than the
     * stop_after field, you're done.  This is mainly useful when no noise
     * is present, since under some bizarre circumstances you can get into
     * nonconverging loops in the simplex algorithm.
     */

    best_match  =  BADMATCH;
    worst_match = -BADMATCH;

    for (i = 0; i < simplex_size; i++)
    {
        match = element->simplex_match[i];

        if (match < best_match)
            best_match = match;

        if (match > worst_match)
            worst_match = match;
    }

    if (fabs(best_match - worst_match) <= element->tolerance)
    {
        /*
         * If the matches are within the tolerance but the best match ever
         * achieved is significantly lower, then do a RESTART.
         */

        if (fabs(best_match - element->best_match) > element->tolerance)
        {
            (void) ParamtableSA_RESTART(element);
            element->done = 0;
        }
        else
        {
            element->done = 1;
        }
    }
    else if ((element->stop_after > 0)
             && abs(element->iteration_number
                    - element->best_match_iteration)
             >= element->stop_after)
    {
        element->done = 1;
    }
    else
    {
        element->done = 0;
    }
}




/*
 * The following functions are action functions for paramtableSA,
 * made into separate functions for convenience in programming and
 * modification.
 */

/*
 * ParamtableSA_TABCREATE
 *
 * FUNCTION
 *     Allocates memory for parameter tables.
 *
 * ARGUMENTS
 *     element    -- address of element
 *     num_params -- number of parameters in table
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableSA_TABCREATE(element, num_params)
    struct paramtableSA_type *element;
    int num_params;
{
    int i;

    if (num_params < 0)
    {
        Error();
        printf("paramtableSA: TABCREATE: number of parameters must be >= 0!\n");
        return 0;
    }
    else element->num_params = num_params;

    element->search = (short *)  calloc(num_params, sizeof(short));
    element->type   = (short *)  calloc(num_params, sizeof(short));
    element->center = (double *) calloc(num_params, sizeof(double));
    element->range  = (double *) calloc(num_params, sizeof(double));
    element->min    = (double *) calloc(num_params, sizeof(double));
    element->max    = (double *) calloc(num_params, sizeof(double));

    /*
     * Create array of label strings for parameters.  Allocate an extra
     * character for the terminating null character of the string.
     */

    element->label   = (char **) calloc(num_params, sizeof(char *));

    for (i = 0; i < num_params; i++)
        element->label[i] = (char *) calloc((LABEL_SIZE + 1), sizeof(char));

    element->realcenter = (double *) calloc(num_params, sizeof(double));
    element->realrange  = (double *) calloc(num_params, sizeof(double));
    element->realmin    = (double *) calloc(num_params, sizeof(double));
    element->realmax    = (double *) calloc(num_params, sizeof(double));
    element->best       = (double *) calloc(num_params, sizeof(double));
    element->current    = (double *) calloc(num_params, sizeof(double));

    /*
     * Create the simplex array and the array of match values for the
     * simplex.  The dimensions of the simplex are (num_params + 1) X
     * (num_params).
     */

    element->simplex = (double **) calloc((num_params + 1),
                                          sizeof(double *));

    for (i = 0; i < (num_params + 1); i++)
    {
        element->simplex[i]  = (double *) calloc(num_params,
                                                 sizeof(double));
    }

    element->simplex_match = (double *) calloc((num_params + 1),
                                               sizeof(double));
    element->partial_sum   = (double *) calloc(num_params, sizeof(double));
    element->test_point    = (double *) calloc(num_params, sizeof(double));

    /*
     * Create the scalemod array and initialize all values to 1.0.  They
     * can be set to other values by the user if desired.
     */

    element->scalemod = (double *) calloc(num_params, sizeof(double));

    for (i = 0; i < num_params; i++)
        element->scalemod[i] = 1.0;

    element->alloced = 1;
    return 1;
}




/*
 * ParamtableSA_TABDELETE
 *
 * FUNCTION
 *     Deallocates memory for all arrays.
 *
 * ARGUMENTS
 *     element -- address of element
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
ParamtableSA_TABDELETE(element)
    struct paramtableSA_type *element;
{
    int i;

    if (element->alloced == 0)
        return;

    paramFree(element->search);
    paramFree(element->type);
    paramFree(element->center);
    paramFree(element->range);
    paramFree(element->min);
    paramFree(element->max);

    for (i = 0; i < element->num_params; i++)
        paramFree(element->label[i]);

    paramFree(element->label);

    paramFree(element->realcenter);
    paramFree(element->realrange);
    paramFree(element->realmin);
    paramFree(element->realmax);
    paramFree(element->best);
    paramFree(element->current);

    for (i = 0; i < (element->num_params + 1); i++)
        paramFree(element->simplex[i]);

    paramFree(element->simplex);

    paramFree(element->simplex_match);
    paramFree(element->partial_sum);
    paramFree(element->test_point);
    paramFree(element->scalemod);
    element->alloced = 0;
}




/*
 * ParamtableSA_INITSEARCH
 *
 * FUNCTION
 *     Sets up initial simplex values.  This function should be
 *     called whenever starting a new parameter search.
 *
 * ARGUMENTS
 *     element -- address of element
 *     action  -- address of action
 *
 * RETURN VALUE
 *     int -- 0 = failure, 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableSA_INITSEARCH(element, action)
    struct paramtableSA_type *element;
    Action *action;
{
    int    i, j, k;
    int    simplex_size;
    double temp, min, max, fullrange;
    short  randomize = 0; /* flag for whether to choose initial parameters
                           * randomly within range */

    if (!element->alloced)
    {
        Error();
        printf("paramtableSA: INITSEARCH: must create tables with TABCREATE\n");
        printf("                          before calling this action!\n");
        return 0;
    }

    /*
     * If all parameters have been loaded with initparamSA then next_index
     * will equal num_params.
     *
     * FIXME! This is a hack and unnecessarily brittle.
     */

    if (element->next_index != element->num_params)
    {
        Error();
        printf("paramtableSA: INITSEARCH: must initialize all parameters\n");
        printf("                          using initparamSA before calling this action!\n");
        return 0;
    }

    /* Check to see if the "random" option has been selected. */

    if ((action->argc > 0) && (strcmp(action->argv[0], "random") == 0))
    {
        randomize = 1;
    }

    /* Determine how many parameters to search over. */

    element->num_params_to_search = 0;

    for (i = 0; i < element->num_params; i++)
    {
        if (element->search[i] != 0)
            ++(element->num_params_to_search);
    }

    if (element->num_params_to_search < 2)
    {
        Error();
        printf("paramtableSA: INITSEARCH: need to search over at least two parameters!\n");
        return 0;
    }

    simplex_size = element->num_params_to_search + 1;

    /*
     * Clear the contents of the simplex.  Since the simplex is readable by
     * the user, but not all points on the simplex will have values loaded
     * into them (e.g. if some parameters are not searched over) this is so
     * that the simplex won't contain garbage.  The same is true with
     * simplex_match.
     */

    for (i = 0; i < simplex_size; i++)
    {
        for (j = 0; j < element->num_params; j++)
            element->simplex[i][j] = 0.0;

        element->simplex_match[i] = 0.0;
    }

    if (!randomize)
    {
        for (i = 0; i < element->num_params; i++)
            element->simplex[0][i] = element->realcenter[i];
    }
    else
    {
        /*
         * Pick the coordinates of point 0 of the simplex at random
         * (within the limits of the min and max values, of course).
         */

        for (i = 0; i < element->num_params; i++)
        {
            if (element->search[i] == 0) /* do not vary this parameter */
            {
                element->simplex[0][i] = element->realcenter[i];
            }
            else
            {
                min = element->realmin[i];
                max = element->realmax[i];
                element->simplex[0][i] = frandom(min, max);
            }
        }
    }

    /*
     * Pick other points according to the formula Pi = Po + lambda(i) * Ei,
     * where lambda(i) are the scalemod values multiplied by scale, and the
     * Ei are unit vectors in all the directions.  If a randomly picked
     * value is outside of the (min, max) range for that parameter, then
     * that parameter is wrapped around until it is in the range.  Note
     * that all parameters are internally stored as additive parameters
     * (except for the best parameter set).  This makes the algorithm
     * easier to program.
     */

    for (i = 1, j = 0; i < simplex_size; i++, j++)
    {
        /* Copy point zero of the simplex onto this point. */

        for (k = 0; k < element->num_params; k++)
            element->simplex[i][k] = element->simplex[0][k];

        /* Find the next parameter to be varied. */

        while (element->search[j] == 0) /* do not vary this parameter */
        {
            j++;

            if (j >= element->num_params) /* This can't happen. */
            {
                Error();
                printf("paramtableSA: INITSEARCH: tried to choose a non-existent parameter!\n");
                return 0;
            }
        }

        /*
         * Get the minimum and maximum values for this parameter, plus the
         * full range.
         */

        min       = element->realmin[j];
        max       = element->realmax[j];
        fullrange = max - min;

        /* Vary the new point in this parameter's direction only. */

        temp = element->simplex[i][j] + element->scale
            * element->scalemod[j];

        /*
         * If temp is outside of the limits in any dimension,
         * wrap around to the other side of that dimension.
         *
         * FIXME: make this an OPTION; the alternative is to set the
         *        value outside the limit to the limit.
         */

        while (temp < min)
            temp += fullrange;

        while (temp > max)
            temp -= fullrange;

        element->simplex[i][j] = temp;
    }

    /*
     * Add randomness to all parameter values on all points on simplex,
     * depending on simplex_init_noise.
     */

    for (i = 1; i < simplex_size; i++)
    {
        for (j = 0; j < element->num_params; j++)
        {
            if (element->search[j] == 0)  /* do not vary this parameter */
                continue;

            min = element->realmin[j];
            max = element->realmax[j];
            element->simplex[i][j] = element->simplex[i][j]
                * (1.0 - element->simplex_init_noise)
                + element->simplex_init_noise * frandom(min, max);
        }
    }

    calc_partial_sums(element);

    /* Start simulating with point 0 of the simplex. */

    element->temperature   = element->inittemp;
    element->best_match    = 1.0e10;
    element->state         = 0;
    element->next_index    = 0;
    get_next_parameter_set(element);

    /*
     * The "best" parameter set is initially set to be
     * the first one to be simulated.
     */

    for (i = 0; i < element->num_params; i++)
        element->best[i] = element->current[i];

    return 1;
}




/*
 * ParamtableSA_EVALUATE
 *
 * FUNCTION
 *     Looks at match value of most recently-simulated point
 *     and copies the point into best and the match
 *     into best_match if it's the best point so far.
 *
 * ARGUMENTS
 *     element -- address of element
 *     match   -- match value
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
ParamtableSA_EVALUATE(element, match)
    struct paramtableSA_type *element;
    double match;
{
    int i;

    element->current_match = match;

    if (match < element->best_match)
    {
        element->best_match = match;
        element->best_match_iteration = element->iteration_number;

        for (i = 0; i < element->num_params; i++)
            element->best[i] = element->current[i];

        element->new_best_match = 1;
    }
    else
    {
        element->new_best_match = 0;
    }
}






/*
 * ParamtableSA_UPDATE_PARAMS
 *
 * FUNCTION
 *     This function chooses the next set of parameter values to be
 *     simulated as a function of the last match value.  When this function
 *     is called it expects that current_match holds the match value
 *     corresponding to current (which will be true if the EVALUATE <match>
 *     action has been previously called).
 *
 * ARGUMENTS
 *     element -- address of element
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
ParamtableSA_UPDATE_PARAMS(element)
    struct paramtableSA_type *element;
{
    int    i, j;
    int    simplex_size = element->num_params_to_search + 1;
    static int best_index, worst_index;    /* indices of simplex points */
    static double best_match, second_worst_match, worst_match, temp_match;
    double match, noisy_match;

    match = element->current_match;

    /*
     * The algorithm implemented here is a multidimensional
     * gradient-descent algorithm called a simplex algorithm, with noise
     * added according to the annealing schedule (which I will ignore here
     * but which is automatically taken into account in calculating match
     * values).  The number of simplex points will be designated as s
     * (which will be the number of parameters being searched (n) plus
     * one). The pseudocode version of the algorithm goes as follows:
     *
     * a) Initialize the simplex.  This means pick s points which span a
     *    volume in n-dimensional space.  Pick the first point at random.
     *    Then pick the others by adding a component to the ith direction
     *    for the (i + 1)th point.  This is all done in the INITSEARCH
     *    action.  I also add some noise to the parameter values at each
     *    point since this makes the algorithm work better.
     *
     * b) Simulate all points on the simplex.  Calculate their match
     *    values.  This corresponds to state 0 of this action.
     *
     * c) Choose a test point by reflecting through the worst point on the
     *    simplex.  Calculate the match value for the test point.  If the
     *    match value is better than the worst value on the simplex,
     *    replace the worst simplex point with the test point.  If the new
     *    match value is worse than the second-worst value on the old
     *    simplex (so it's the worst point on the new simplex) go to (d).
     *    If it's better than the best point on the old simplex (so it's
     *    now the best point on the new simplex) go to (e).
     *
     * d) Choose a new test point by doing a one-dimensional contraction by
     *    one-half around the worst point.  Calculate the match value for
     *    the test point.  If it's better than the worst match value accept
     *    it and go to (c).  Otherwise, go to (f).
     *
     * e) Choose a test point by expanding by a factor of 2 through the
     *    best point.  If this point is better than the best point, replace
     *    the best point with the test point.  Go to (c).
     *
     * f) Do a multidimensional contraction by one-half around the best
     *    point.  Replace all the simplex points except the best one with
     *    the new points.  Calculate the match values for all the new
     *    points.  Go to (c).
     *
     */

    /*
     * This code uses a state-transition-table-like concept to mimic the
     * Numerical Recipes algorithm.  I apologize for the extremely gross
     * spaghetti code here, but it is an accurate reflection of the source
     * :-) Actually, it's what happens if you can't call a function
     * directly but have to go outside of this function to get the value of
     * the function being minimized (the match function), which requires
     * that you do a simulation and run the match function external to this
     * routine.
     *
     * Note that at the end of an UPDATE_PARAMS step the state cannot be in
     * state 1.  State 1 is just a convenient way of indicating that we
     * need to recalculate the ranks, reflect through the worst point and
     * choose a new point accordingly.  Since a lot of states go back to
     * state 1, and since state 1 doesn't require a new function evaluation
     * at its beginning I've used a goto here instead of duplicating the
     * code several times.
     *
     */

    switch (element->state)
    {
    case 0:

        /*
         * This state occurs only at the start of a parameter search.
         * Simulate all the points on the simplex and return.  Once you're
         * done, go to state 1.  Note that the last point on the simplex
         * has the index num_params due to zero-indexing.  Note also that
         * the first time this is called next_index should be 0 (which it
         * is set to in INITSEARCH above).
         */

        if (element->next_index < (simplex_size - 1))
        {
            element->simplex_match[element->next_index] = match;
            ++element->next_index;
            get_next_parameter_set(element);
            return;
        }

        /* If you reached here, this is the last point on the simplex. */

        element->simplex_match[element->next_index] = match;
        element->state = 1;

        /* State 0 falls through to state 1. */

    STATE1:

        /*
         * Determine the ranks.  Then take the worst point and reflect it
         * through the simplex (using calc_point()).  Put the new point
         * into test_point.  Simulate the new point.  Go to state 2.
         */

        calc_ranks(element, &best_index, &worst_index,
                   &best_match, &second_worst_match, &worst_match);
        calc_point(element, worst_index, -1.0);
        get_next_parameter_set(element);
        element->state = 2;

        break;


    case 2:

        noisy_match = evaluate_match(element, match,
                                     &worst_match, worst_index);

        /*
         * If the match of the test point is better than the best point
         * (according to value returned from evaluate_match()), try an
         * additional extrapolation of the last point by a factor of 2
         * (using calc_point()).  Simulate the new point. Go to state 3.
         * Notice that worst_index here is actually the *best* index in
         * this case, since evaluate_match() in this case replaced the old
         * worst point with the last point simulated (i.e. the best).
         * Confusing?  You bet, but that's what you get when you use
         * Numerical Recipes code as a starting point :-(
         */

        if (noisy_match <= best_match)
        {
            calc_point(element, worst_index, 2.0);
            get_next_parameter_set(element);
            element->state = 3;
            return;
        }

        /*
         * If the match of the last point simulated is worse than the
         * second-worst point, do a one-dimensional contraction around the
         * worst point (using calc_point()).  Simulate the new point.  Go
         * to state 4.  Note that worst_match can either be the old worst
         * match (if the last point simulated was even worse than it) or
         * the match of the last point simulated (if it was better than the
         * old worst match).  In either case the index is worst_index, as
         * it should be.
         */

        if (noisy_match >= second_worst_match)
        {
            temp_match = worst_match;
            calc_point(element, worst_index, 0.5);
            get_next_parameter_set(element);
            element->state = 4;
            return;
        }

        /* Otherwise, go to state 1.  */

        element->state = 1;
        goto STATE1;

        /*NOTREACHED*/

        break;


    case 3:

        /*
         * If the match of test_point is better than the best point
         * (according to value returned from evaluate_match()), store the
         * new point into the simplex in place of the *best* point (yes,
         * you read that correctly).  This is done automatically by
         * evaluate_match().  Again, the "worst_index" here is actually the
         * index of the best of the previous values.  Go to state 1.
         */

        noisy_match = evaluate_match(element, match,
                                     &worst_match, worst_index);

        /* We don't use the noisy_match value in state 3. */

        element->state = 1;
        goto STATE1;

        /*NOTREACHED*/

        break;


    case 4:

        /*
         * If the match of the last point simulated is worse than the
         * match of the worst point (which we saved above as temp_match),
         * go to state 5.  Otherwise, go directly to state 1.
         */

        noisy_match = evaluate_match(element, match,
                                     &worst_match, worst_index);

        if (noisy_match >= temp_match)
        {
            /*
             * Since we can't seem to get rid of that high point, we do an
             * s-dimensional contraction around the best point.  Set
             * next_index to 0.  Skip the best point which is in
             * best_index; run through all the rest and simulate them.
             * Keep the count in next_index; when it hits n, stop and go to
             * state 1.
             */

            for (i = 0; i < simplex_size; i++)
            {
                if (i == best_index)
                    continue;

                for (j = 0; j < element->num_params; j++)
                {
                    if (element->search[i] == 0)
                    {
                        /* this parameter is not part of the search */
                        continue;
                    }

                    element->simplex[i][j]
                        = 0.5 * (element->simplex[i][j]
                                 + element->simplex[best_index][j]);
                }
            }

            element->next_index = 0;

            if (best_index == element->next_index)
                ++element->next_index;

            element->state = 5;
            get_next_parameter_set(element);

            return;
        }
        else
        {
            element->state = 1;
            goto STATE1;
        }

        /*NOTREACHED*/

        break;


    case 5:

        if (element->next_index < (simplex_size - 1))
        {
            element->simplex_match[element->next_index] = match;
            ++element->next_index;

            if (element->next_index == best_index)
            {
                /* If best_index was the last index then you're done. */
                if (element->next_index == (simplex_size - 1))
                {
                    element->state = 1;
                    goto STATE1;
                }
                else
                {
                    ++element->next_index;
                }
            }

            get_next_parameter_set(element);
            return;
        }

        /* If you reached here, this is the last point on the simplex. */

        element->simplex_match[element->next_index] = match;
        element->state = 1;
        goto STATE1;

        /*NOTREACHED*/

        break;
    }
}




/*
 * ParamtableSA_RESTART
 *
 * FUNCTION
 *     Replaces worst point on simplex with the point corresponding
 *     to the best match.  This is only useful in an annealing search
 *     (i.e. with noise) since then the search can move away from the
 *     best point.  This routine also checks to see if a point with
 *     the same match value, within a tolerance is already on the simplex;
 *     if so, it doesn't replace the value.  This keeps the simplex
 *     from getting corrupted with multiple copies of the same parameter
 *     set.
 *
 * ARGUMENTS
 *     element -- address of element
 *
 * RETURN VALUE
 *     int -- 0 means failure and 1 means success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableSA_RESTART(element)
    struct paramtableSA_type *element;
{
    int    i;
    int    simplex_size = element->num_params + 1;
    int    worst_index  = -1;
    int    best_index   = -1;
    double worst_match  = -BADMATCH;
    double best_match   =  BADMATCH;

    if (!element->alloced)
    {
        Error();
        printf("paramtableSA: RESTART: must create tables with TABCREATE\n");
        printf("                       before calling this action!\n");
        return 0;
    }


    /*
     * Find the worst and best points on the simplex and their associated
     * matches.
     */

    for (i = 0; i < simplex_size; i++)
    {
        if (element->simplex_match[i] > worst_match)
        {
            worst_match = element->simplex_match[i];
            worst_index = i;
        }

        if (element->simplex_match[i] < best_match)
        {
            best_match = element->simplex_match[i];
            best_index = i;
        }
    }

    if (worst_index == -1 || best_index == -1)
    {
        /* This should never happen. */
        Error();
        printf("paramtableSA: RESTART: simplex_matches are incorrect!\n");
        return 0;
    }

    /*
     * Does best point already exist on simplex?  If so, return, since a
     * RESTART will not help you.
     */

    /*
     * FIXME: we should test whether the simplex points are within a tiny
     * amount of the best point and not use the match values for this.
     */

    if (fabs(best_match - element->best_match) <= element->tolerance)
        return 1;

    /*
     * Replace worst point by best point.
     */

    for (i = 0; i < element->num_params; i++)
    {
        if (element->type[i] == 0) /* additive parameter */
            element->simplex[worst_index][i] = element->best[i];
        else /* multiplicative parameter */
            element->simplex[worst_index][i] = log(element->best[i]);
    }

    /*
     * Replace worst match by best match.
     */

    element->simplex_match[worst_index] = element->best_match;

    return 1;
}




/*
 * ParamtableSA_RESTART2
 *
 * FUNCTION
 *     Restarts search from best match obtained so far
 *     (i.e. re-initializes simplex).  This function is
 *     identical to INITSEARCH except that the initial
 *     point is not randomly selected and a random component
 *     is added to the other points on the simplex so that
 *     multiple RESTART2s from the same point will not
 *     always do the same thing.
 *
 * ARGUMENTS
 *     element -- address of element
 *
 * RETURN VALUE
 *     int -- 0 means failure and 1 means success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableSA_RESTART2(element)
    struct paramtableSA_type *element;
{
    int    i, j, k;
    int    simplex_size = element->num_params + 1;
    double min, max, fullrange, temp, rand;

    /* Error checks. */

    if (!element->alloced)
    {
        Error();
        printf("paramtableSA: RESTART2: must create tables with TABCREATE\n");
        printf("                        before calling this action!\n");
        return 0;
    }

    /*
     * If all parameters have been loaded with initparamSA then next_index
     * will equal element->num_params.
     */

    if (element->next_index != element->num_params)
    {
        Error();
        printf("paramtableSA: RESTART2: must initialize all parameters\n");
        printf("                        using initparSA before calling this action!\n");
        return 0;
    }

    /*
     * Pick the coordinates of point 0 of the simplex to correspond to the
     * best point.
     */

    for (i = 0; i < element->num_params; i++)
    {
        if (element->type[i] == 0) /* additive parameter       */
            element->simplex[0][i] = element->best[i];
        else                       /* multiplicative parameter */
            element->simplex[0][i] = log(element->best[i]);
    }

    /*
     * The following is the same as the INITSEARCH action except
     * that a random component is added to the new points.
     */

    for (i = 1, j = 0; i < simplex_size; i++, j++)
    {
        /* Copy point zero of the simplex onto this point. */

        for (k = 0; k < element->num_params; k++)
            element->simplex[i][k] = element->simplex[0][k];

        /* Find the next parameter to be varied. */

        while (element->search[j] == 0) /* do not vary this parameter */
        {
            j++;

            if (j >= element->num_params) /* This can't happen. */
            {
                Error();
                printf("paramtableSA: RESTART2: tried to choose a non-existent parameter!\n");
                return 0;
            }
        }

        /*
         * Get the minimum and maximum values for this parameter, plus the
         * full range.
         */

        min       = element->realmin[j];
        max       = element->realmax[j];
        fullrange = max - min;
        rand      = urandom();

        /*
         * Vary the new point in this parameter's direction only.  Add
         * randomness.
         */

        temp = element->simplex[i][j] + element->scale
            * element->scalemod[j] * rand;

        /* Wrap around if outside the limits. */

        while (temp < min)
            temp += fullrange;

        while (temp > max)
            temp -= fullrange;

        element->simplex[i][j] = temp;
    }

    calc_partial_sums(element);

    /* Start simulating with point 0 of the simplex. */

    element->temperature = element->inittemp;
    element->best_match  = 1e10;
    element->state       = 0;
    element->next_index  = 0;
    get_next_parameter_set(element);

    /*
     * The "best" parameter set is initially set to be
     * the first one to be simulated.
     */

    for (i = 0; i < element->num_params; i++)
        element->best[i] = element->current[i];

    return 1;
}




/*
 * ParamtableSA_SAVE
 *
 * FUNCTION
 *     Saves the object's state to a binary file.
 *
 * ARGUMENTS
 *     element  -- address of element
 *     filename -- name of file to save in
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableSA_SAVE(element, filename)
    struct paramtableSA_type *element;
    char *filename;
{
    FILE *fp;
    int   i;

    assert(filename != NULL);

    fp = fopen(filename, "w");

    if (fp == NULL)
    {
        Error();
        printf("paramtableSA: SAVE: file cannot be opened for SAVE action!\n");
        return 0;
    }

    paramFwrite(&element->iteration_number,     sizeof(int),    1, fp);
    paramFwrite(&element->iterations_per_temp,  sizeof(int),    1, fp);
    paramFwrite(&element->temperature,          sizeof(double), 1, fp);
    paramFwrite(&element->inittemp,             sizeof(double), 1, fp);
    paramFwrite(&element->annealing_method,     sizeof(short),  1, fp);
    paramFwrite(&element->max_iterations,       sizeof(int),    1, fp);
    paramFwrite(&element->annealing_rate,       sizeof(double), 1, fp);
    paramFwrite(&element->testtemp,             sizeof(double), 1, fp);
    paramFwrite(&element->tolerance,            sizeof(double), 1, fp);
    paramFwrite(&element->stop_after,           sizeof(int),    1, fp);
    paramFwrite(&element->done,                 sizeof(short),  1, fp);
    paramFwrite(&element->restart_every,        sizeof(int),    1, fp);
    paramFwrite(&element->state,                sizeof(int),    1, fp);
    paramFwrite(&element->num_params,           sizeof(int),    1, fp);
    paramFwrite(&element->num_params_to_search, sizeof(int),    1, fp);
    paramFwrite(element->search,                sizeof(short),
                element->num_params, fp);
    paramFwrite(element->type,                  sizeof(short),
                element->num_params, fp);
    paramFwrite(element->center,                sizeof(double),
                element->num_params, fp);
    paramFwrite(element->range,                 sizeof(double),
                element->num_params, fp);
    paramFwrite(element->min,                   sizeof(double),
                element->num_params, fp);
    paramFwrite(element->max,                   sizeof(double),
                element->num_params, fp);

    /*
     * Write out the array of label strings.
     */

    for (i = 0; i < element->num_params; i++)
    {
        paramFwrite(element->label[i],
                    (LABEL_SIZE + 1) * sizeof(char), 1, fp);

        if (ferror(fp))
        {
            Error();
            printf("paramtableSA: SAVE: error writing to file!\n");
            return 0;
        }
    }

    paramFwrite(element->realcenter,
                sizeof(double), element->num_params, fp);
    paramFwrite(element->realrange,
                sizeof(double), element->num_params, fp);
    paramFwrite(element->realmin,
                sizeof(double), element->num_params, fp);
    paramFwrite(element->realmax,
                sizeof(double), element->num_params, fp);
    paramFwrite(element->best,
                sizeof(double), element->num_params, fp);
    paramFwrite(&element->best_match,
                sizeof(double), 1, fp);
    paramFwrite(&element->best_match_iteration,
                sizeof(int), 1, fp);
    paramFwrite(&element->new_best_match,
                sizeof(short), 1, fp);
    paramFwrite(element->current,
                sizeof(double), element->num_params, fp);
    paramFwrite(&element->current_match,
                sizeof(double), 1, fp);
    paramFwrite(&element->next_index,
                sizeof(int), 1, fp);
    paramFwrite(&element->simplex_init_noise,
                sizeof(double), 1, fp);

    /* Write out the (n+1) points of the simplex. */

    for (i = 0; i < (element->num_params + 1); i++)
    {
        paramFwrite(element->simplex[i],
                    sizeof(double), element->num_params, fp);
    }

    paramFwrite(element->simplex_match,
                sizeof(double), element->num_params, fp);
    paramFwrite(element->partial_sum,
                sizeof(double), element->num_params, fp);
    paramFwrite(element->test_point,
                sizeof(double), element->num_params, fp);
    paramFwrite(&element->scale,
                sizeof(int), 1, fp);
    paramFwrite(element->scalemod,
                sizeof(double), element->num_params, fp);

    fclose(fp);
    return 1;
}




/*
 * ParamtableSA_SAVEBEST
 *
 * FUNCTION
 *     Saves the best parameter table obtained so far to a text file.
 *
 * ARGUMENTS
 *     element  -- address of element
 *     filename -- name of file to save in
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableSA_SAVEBEST(element, filename)
    struct paramtableSA_type *element;
    char *filename;
{
    FILE *fp;
    int   i;

    assert(filename != NULL);

    fp = fopen(filename, "w");

    if (fp == NULL)
    {
        Error();
        printf("paramtableSA: SAVEBEST: file cannot be opened for SAVEBEST action!\n");
        return 0;
    }

    fprintf(fp, "Best match so far: %g\n", element->best_match);
    fprintf(fp, "Parameter table data:\n");
    fprintf(fp, "-----------------------------------------------------------------------\n");
    fprintf(fp, "Iteration number: %d\n",     element->iteration_number);
    fprintf(fp, "Temperature: %f\n",          element->temperature);
    fprintf(fp, "Number of parameters: %d\n", element->num_params);

    fprintf(fp, "\n");
    fprintf(fp, "Param  Varied?  Type  Range      Best                       Label\n");
    fprintf(fp, "-----  -------  ----  -----      ----                       -----\n");
    fprintf(fp, "\n");

    for (i = 0; i < element->num_params; i++)
    {
        fprintf(fp, "%-5d  ", i); /* parameter number */

        if (element->search[i] == 0)
            fprintf(fp, "%s      ", "NO ");
        else
            fprintf(fp, "%s      ", "YES");

        if (element->type[i] == 0)
            fprintf(fp, "%s  ", "add ");
        else
            fprintf(fp, "%s  ", "mult");

        /*
         * NOTE: we use a very high precision to make it possible to
         *       manually enter these parameters later without losing
         *       too much accuracy.
         *
         * MAYBE-TODO: have user-specifiable precision!
         */

        fprintf(fp, "%-8g   %-25.20g  %s\n",
                element->range[i], element->best[i], element->label[i]);
    }

    fprintf(fp, "\n");

    fprintf(fp, "Best match so far: %.10g\n", element->best_match);

    fclose(fp);
    return 1;
}




/*
 * ParamtableSA_RESTORE
 *
 * FUNCTION
 *     Restores the object's state from a binary file.
 *
 * ARGUMENTS
 *     element  -- address of element
 *     filename -- name of file to restore from
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableSA_RESTORE(element, filename)
    struct paramtableSA_type *element;
    char *filename;
{
    FILE *fp;
    int   i;

    assert(filename != NULL);

    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        Error();
        printf("paramtableSA: RESTORE: file cannot be opened for RESTORE action!\n");
        return 0;
    }

    /*
     * First delete the old parameter tables.
     */

    ParamtableSA_TABDELETE(element);

    fread(&element->iteration_number,     sizeof(int),    1, fp);
    fread(&element->iterations_per_temp,  sizeof(int),    1, fp);
    fread(&element->temperature,          sizeof(double), 1, fp);
    fread(&element->inittemp,             sizeof(double), 1, fp);
    fread(&element->annealing_method,     sizeof(short),  1, fp);
    fread(&element->max_iterations,       sizeof(int),    1, fp);
    fread(&element->annealing_rate,       sizeof(double), 1, fp);
    fread(&element->testtemp,             sizeof(double), 1, fp);
    fread(&element->tolerance,            sizeof(double), 1, fp);
    fread(&element->stop_after,           sizeof(int),    1, fp);
    fread(&element->done,                 sizeof(short),  1, fp);
    fread(&element->restart_every,        sizeof(int),    1, fp);
    fread(&element->state,                sizeof(int),    1, fp);
    fread(&element->num_params,           sizeof(int),    1, fp);
    fread(&element->num_params_to_search, sizeof(int),    1, fp);

    /*
     * Allocate the new parameter tables.
     */

    if (!ParamtableSA_TABCREATE(element, element->num_params))
    {
        return 0;
    }

    fread(element->search,
          sizeof(short),  element->num_params, fp);
    fread(element->type,
          sizeof(short),  element->num_params, fp);
    fread(element->center,
          sizeof(double), element->num_params, fp);
    fread(element->range,
          sizeof(double), element->num_params, fp);
    fread(element->min,
          sizeof(double), element->num_params, fp);
    fread(element->max,
          sizeof(double), element->num_params, fp);

    /*
     * Read in the array of label strings.
     */

    for (i = 0; i < element->num_params; i++)
    {
        fread(element->label[i], (LABEL_SIZE + 1) * sizeof(char), 1, fp);

        if (ferror(fp))
        {
            Error();
            printf("paramtableSA: RESTORE: error writing to file!\n");
            return 0;
        }
    }

    fread(element->realcenter,
          sizeof(double), element->num_params, fp);
    fread(element->realrange,
          sizeof(double), element->num_params, fp);
    fread(element->realmin,
          sizeof(double), element->num_params, fp);
    fread(element->realmax,
          sizeof(double), element->num_params, fp);
    fread(element->best,
          sizeof(double), element->num_params, fp);
    fread(&element->best_match,
          sizeof(double), 1, fp);
    fread(&element->best_match_iteration,
          sizeof(int),    1, fp);
    fread(&element->new_best_match,
          sizeof(short),  1, fp);
    fread(element->current,
          sizeof(double), element->num_params, fp);
    fread(&element->current_match,
          sizeof(double), 1, fp);
    fread(&element->next_index,
          sizeof(int),    1, fp);
    fread(&element->simplex_init_noise,
          sizeof(double), 1, fp);

    /* Read in the (n+1) points of the simplex. */

    for (i = 0; i < (element->num_params_to_search + 1); i++)
    {
        fread(element->simplex[i], sizeof(double), element->num_params, fp);
    }

    fread(element->simplex_match,
          sizeof(double), element->num_params, fp);
    fread(element->partial_sum,
          sizeof(double), element->num_params, fp);
    fread(element->test_point,
          sizeof(double), element->num_params, fp);
    fread(&element->scale,
          sizeof(int),    1, fp);
    fread(element->scalemod,
          sizeof(double), element->num_params, fp);
    element->alloced = 1;

    fclose(fp);
    return 1;
}

/* FIXME: need a RESTOREBEST action. */




/*
 * ParamtableSA_DISPLAY
 *
 * FUNCTION
 *     Displays the best parameter table obtained so far on standard
 *     output.
 *
 * ARGUMENTS
 *     element -- address of element
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
ParamtableSA_DISPLAY(element)
    struct paramtableSA_type *element;
{
    int i;

    printf("\n");
    printf("Best match so far: %g\n", element->best_match);
    printf("Parameter table data:\n");
    printf("-----------------------------------------------------------------------\n");
    printf("Iteration number: %d\n",     element->iteration_number);
    printf("Temperature: %f\n",          element->temperature);
    printf("Number of parameters: %d\n", element->num_params);
    printf("\n");
    printf("Param   Varied?    Type    Range    Best              Label\n");
    printf("-----   -------    ----    -----    ----              -----\n");
    printf("\n");

    for (i = 0; i < element->num_params; i++)
    {
        printf("%-5d   ", i); /* parameter number */

        if (element->search[i] == 0)
            printf("%s        ", "NO ");
        else
            printf("%s        ", "YES");

        if (element->type[i] == 0)
            printf("%s    ", "add ");
        else
            printf("%s    ", "mult");

        printf("%-8g   %-15.6g   %s\n",
               element->range[i], element->best[i], element->label[i]);
    }

    printf("Best match so far: %g\n", element->best_match);
    printf("\n");
}



/*
 * ParamtableSA_DISPLAY2
 *
 * FUNCTION
 *     Displays best parameter table on standard output;
 *     also includes current parameter table; useful for debugging.
 *
 * ARGUMENTS
 *     element -- address of element
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

/* FIXME!  This needs to be revised to conform to the pattern of DISPLAY */

void
ParamtableSA_DISPLAY2(element)
    struct paramtableSA_type *element;
{
    int i;

    printf("Best match so far: %g\n", element->best_match);
    printf("Parameter table data:\n");
    printf("------------------------------------------------------------------------------\n");

    printf("Iteration number: %d\n",     element->iteration_number);
    printf("Temperature: %f\n",          element->temperature);
    printf("Number of parameters: %d\n", element->num_params);
    printf("Param\tType\tRange\t\tCurrent\t\tBest\t\tLabel\n");

    for (i = 0; i < element->num_params; i++)
        printf("%d\t%d\t%g\t\t%-10.6g\t%-10.6g\t%s\n",
               i, element->type[i], element->range[i],
               element->current[i], element->best[i],
               element->label[i]);

    printf("Match of current point: %g\n", element->current_match);
    printf("Best match so far: %g\n", element->best_match);
}




/*
 * ParamtableSA_CHECK
 *
 * FUNCTION
 *     Checks fields for obvious errors.
 *
 * ARGUMENTS
 *     element -- address of element
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
ParamtableSA_CHECK(element)
    struct paramtableSA_type *element;
{
    int i;

    if (element->iterations_per_temp < 1)
    {
        Error();
        printf("paramtableSA: iterations_per_temp must be >= 1!\n");
    }

    if (element->temperature < 0.0)
    {
        Error();
        printf("paramtableSA: temperature must be >= 0.0!\n");
    }

    if (element->inittemp < 0.0)
    {
        Error();
        printf("paramtableSA: inittemp must be >= 0.0!\n");
    }


    if (element->annealing_method < 0 || element->annealing_method > 2)
    {
        Error();
        printf("paramtableSA: nealing_method must be 0, 1, or 2!\n");
    }

    if (element->max_iterations < 0)
    {
        Error();
        printf("paramtableSA: max_iterations must be > 0!\n");
    }

    if (element->annealing_rate < 0.0)
    {
        Error();
        printf("paramtableSA: nealing_rate must be >= 0.0 and < 1.0!\n");
    }

    if (element->testtemp < 0.0)
    {
        Error();
        printf("paramtableSA: testtemp must be >= 0.0!\n");
    }

    if (element->tolerance < 0.0)
    {
        Error();
        printf("paramtableSA: tolerance must be >= 0.0!\n");
    }

    if (element->stop_after < 0)
    {
        Error();
        printf("paramtableSA: stop_after must be >= 0!\n");
    }

    if (element->restart_every < 0)
    {
        Error();
        printf("paramtableSA: restart_every must be >= 0!\n");
    }

    if (element->num_params < 0)
    {
        Error();
        printf("paramtableSA: number of parameters must be >= 0!\n");
    }

    if (element->num_params_to_search < 2)
    {
        Error();
        printf("paramtableSA: number of parameters searched over must be at least 2!\n");
    }

    for (i = 0; i < element->num_params; i++)
    {
        if (element->type[i] != 0 && element->type[i] != 1)
        {
            Error();
            printf("paramtableSA: pe field %d must be 0 or 1!\n", i);
        }
    }

    if ((element->simplex_init_noise < 0.0)
        || (element->simplex_init_noise > 1.0))
    {
        Error();
        printf("paramtableSA: simplex_init_noise must be in (0, 1)!\n");
    }

    if (element->scale < 0.0)
    {
        Error();
        printf("paramtableSA: scale must be >= 0.0!\n");
    }
}









/*
 * OBJECT
 *     paramtableSA: a 1-D table for storing a parameter table.
 *                   Also has actions which allow new parameters
 *                   to be generated using a simulated-annealing/
 *                   simplex algorithm (borrowed from Press et. al.:
 *                   Numerical Recipes in C, 2nd. Ed.)
 *
 * FUNCTION
 *     ParamtableSA
 *
 * ARGUMENTS
 *     element -- address of element
 *     action  -- address of action
 *
 * RETURN VALUE
 *     int -- 0 = failure, 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableSA(element, action)
    struct paramtableSA_type *element;
    Action *action;
{
    int     i, num_params;
    char   *savefilename, *restorefilename;
    double  match;

    SELECT_ACTION(action)
    {
    case CREATE:
        element->alloced = 0;
        break;


    case TABCREATE:
        if (action->argc != 1)
        {
            Error();
            printf("paramtableSA: TABCREATE: usage : %s num_params\n", "TABCREATE");
            return 0;
        }

        element->iteration_number = 0;
        element->next_index       = 0;
        num_params = atoi(action->argv[0]);

        if (!ParamtableSA_TABCREATE(element, num_params))
        {
            return 0;
        }

        /*
         * Set all the search flags to 1 to start with i.e. the default
         * is to search over all parameters.
         */

        for (i = 0; i < num_params; i++)
        {
            element->search[i] = 1;
        }

        break;


    case DELETE:
        /*FALLTHROUGH*/


    case TABDELETE:
        ParamtableSA_TABDELETE(element);
        break;


    case INITSEARCH:
        if (!ParamtableSA_INITSEARCH(element, action))
        {
            return 0;
        }

        break;


    case EVALUATE:
        if (action->argc != 1)
        {
            Error();
            printf("paramtableSA: EVALUATE: usage : EVALUATE match\n");
            return 0;
        }

        match = Atod(action->argv[0]);
        ParamtableSA_EVALUATE(element, match);

        break;


    case UPDATE_PARAMS:
        /*
         * Adjust temperature of annealing process.
         * The possible schemes are:
         *
         * 1) temp = max(0, iteration/max_iterations)
         *
         * which is a linear decay which reaches zero at
         * max_iterations, or
         *
         * 2) temp *= annealing_rate
         *
         *    where annealing_rate is in (0, 1]
         *
         * which is a proportional (exponential) decay.
         *
         * Note that if annealing_method = 0 then all temperature
         * variations are manually set by the user.  Note also that
         * temperature is only changed after iterations_per_temp
         * iterations.  We use (iteration_number + 1) since 0 %
         * <anything> = 0 by definition and we want to run through
         * iterations_per_temp iterations initially before changing the
         * temperature.  The same holds for the restart code below.
         *
         */

        if (((element->iteration_number + 1)
             % element->iterations_per_temp) == 0)
        {
            /* If annealing_method is zero (the default) do nothing. */

            if (element->annealing_method == 1)
            {
                element->temperature =
                    MAX(0.0, ((double) element->iteration_number) /
                        ((double) element->max_iterations));
            }
            else if (element->annealing_method == 2)
                element->temperature *= element->annealing_rate;
        }

        ParamtableSA_UPDATE_PARAMS(element);

        /*
         * If the restart_every field is nonzero, restart if necessary.
         */

        if (element->restart_every != 0)
        {
            if (((element->iteration_number + 1)
                 % element->restart_every) == 0)
            {
                if (!ParamtableSA_RESTART(element))
                    return 0;
            }
        }

        ++element->iteration_number;

        /*
         * Check to see if the parameter search is finished i.e.
         * if all points on the simplex are within a certain
         * distance of each other.  All this does is to set the
         * flag element->done; the user can use this information
         * as he/she pleases.
         */

        is_search_done(element);

        break;


    case RESTART:
        if (!ParamtableSA_RESTART(element))
            return 0;

        break;


    case RESTART2:
        if (!ParamtableSA_RESTART2(element))
            return 0;

        break;


    case RECENTER:
        /*
         * Recalculate the min and max values according to the best and
         * range values.
         */

        for (i = 0; i < element->num_params; i++)
        {
            element->center[i] = element->best[i];

            if (element->type[i] == 0) /* additive parameter */
            {
                element->min[i]
                    = element->center[i] - element->range[i];
                element->max[i]
                    = element->center[i] + element->range[i];
            }
            else /* multiplicative parameter */
            {
                element->min[i]
                    = element->center[i] / element->range[i];
                element->max[i]
                    = element->center[i] * element->range[i];
            }

            if (element->type[i] == 1) /* multiplicative parameter */
            {
                element->realcenter[i] = log(element->center[i]);
                element->realrange[i]  = log(element->range[i]);
                element->realmin[i]    = log(element->min[i]);
                element->realmax[i]    = log(element->max[i]);
            }
            else /* additive parameter */
            {
                element->realcenter[i] = element->center[i];
                element->realrange[i]  = element->range[i];
                element->realmin[i]    = element->min[i];
                element->realmax[i]    = element->max[i];
            }
        }

        break;


    case SAVE:
        /*
         * This action saves the object's state to a binary file.
         * Use the filename supplied with the action if any; otherwise use
         * the filename in the element->filename field.  If that's also
         * NULL, issue an error message.
         */

        if (action->argc > 0)
        {
            savefilename = action->argv[0];
        }
        else if (element->filename != NULL)
        {
            savefilename = element->filename;
        }
        else
        {
            Error();
            printf("paramtableSA: SAVE: no filename specified!\n");
            return 0;
        }

        if (!ParamtableSA_SAVE(element, savefilename))
            return 0;

        break;


    case SAVEBEST:
        /*
         * This action saves the best parameter table to a text file.
         * Note that element->filename is *not* used for this action,
         * only for SAVE/RESTORE actions.
         */

        if (action->argc == 1)
        {
            savefilename = action->argv[0];
        }
        else
        {
            Error();
            printf("paramtableSA: usage: SAVEBEST filename\n");
            return 0;
        }

        if (!ParamtableSA_SAVEBEST(element, savefilename))
            return 0;

        break;


    case RESTORE:
        /*
         * This action restores the object's state from a binary file.
         * Use the filename supplied with the action if any; otherwise use
         * the filename in the element->filename field.  If that's also
         * NULL, issue an error message.
         */

        if (action->argc > 0)
        {
            restorefilename = action->argv[0];
        }
        else if (element->filename != NULL)
        {
            restorefilename = element->filename;
        }
        else
        {
            Error();
            printf("paramtableSA: RESTORE: no filename specified!\n");
            return 0;
        }

        if (!ParamtableSA_RESTORE(element, restorefilename))
            return 0;

        break;


    case DISPLAY:
        ParamtableSA_DISPLAY(element);
        break;


    case DISPLAY2:
        ParamtableSA_DISPLAY2(element);
        break;


    case CHECK:
        ParamtableSA_CHECK(element);
        break;


    default:
        Error();
        printf("paramtableSA: invalid action %s\n", action->name);
        return 0;
    }

    return 1;
}




/*
 * The following is a utility function to put parameters into the table
 * with less fuss.
 */

/*
 * do_initparamSA
 *
 * FUNCTION
 *     Sets several fields for a given parameter in the table.
 *
 * ARGUMENTS
 *     int    argc -- number of command arguments
 *     char **argv -- command arguments proper
 *
 * RETURN VALUE
 *     int -- 1 = success; 0 = failure
 *
 * AUTHOR
 *     Mike Vanier
 */

int
do_initparamSA(argc, argv)
    int argc;
    char **argv;
{
    int     param;
    char   *path;
    struct  paramtableSA_type *element;
    short   type;
    double  range, center, scalemod;
    char   *label;

    initopt(argc, argv, "path param type range center scalemod label");

    if (argc != 8)
    {
        printoptusage(argc, argv);
        return 0;
    }

    path     = optargv[1];
    param    = atoi(optargv[2]);
    type     = (short)atoi(optargv[3]);
    range    = Atod(optargv[4]);
    center   = Atod(optargv[5]);
    scalemod = Atod(optargv[6]);
    label    = optargv[7];

    element  = (struct paramtableSA_type *)GetElement(path);

    /* error checks */

    if (!CheckClass(element, ClassID("param")))
    {
        Error();
        printf("initparamSA: Element %s is not a parameter table object!\n",
               element->name);
        return 0;
    }

    if ((param < 0) || (param >= element->num_params))
    {
        Error();
        printf("initparamSA: parameter index is out of range!\n");
        return 0;
    }

    if (type != 0 && type != 1)
    {
        Error();
        printf("initparamSA: type field must be 0 or 1!\n");
        return 0;
    }

    /* FIXME: this is redundant; use a switch statement. */

    if (type == 0) /* additive parameter */
    {
        if (range < 0.0)
        {
            Error();
            printf("initparamSA: additive parameter range must be > 0!\n");
            return 0;
        }
    }

    if (type == 1) /* multiplicative parameter */
    {
        if (range < 1.0)
        {
            Error();
            printf("initparamSA: multiplicative parameter range must be > 1.0!\n");
            return 0;
        }
    }

    if (scalemod < 0.0)
    {
        Error();
        printf("initparamSA: scalemod must be >= 0!\n");
        return 0;
    }

    element->type[param]     = type;
    element->center[param]   = center;
    element->range[param]    = range;

    /* The default is that all parameters are to be searched over. */

    element->search[param]   = 1;

    strncpy(element->label[param], label, LABEL_SIZE);

    /*
     * We store the min and max values of the parameter as well as the
     * range, so that we can either prevent the parameter from going beyond
     * initially established limits (using min and max) or let the best
     * match so far be the center of the parameter's range and recalculate
     * the limits according to the range value (done on a RECENTER action).
     */

    if (type == 0) /* additive parameter */
    {
        element->min[param] = center - range;
        element->max[param] = center + range;
    }
    else /* multiplicative parameter */
    {
        element->min[param] = center / range;
        element->max[param] = center * range;
    }

    /*
     * We want to consider all parameters as additive for the search
     * algorithm; in order to do this we need the center, range, min and
     * max values to be turned into additive values, which means taking the
     * log of the actual values for multiplicative parameters.  We keep the
     * original values for bookkeeping purposes.
     */

    if (type == 1) /* multiplicative parameter */
    {
        element->realcenter[param] = log(element->center[param]);
        element->realrange[param]  = log(element->range[param]);
        element->realmin[param]    = log(element->min[param]);
        element->realmax[param]    = log(element->max[param]);

        /*
         * We change the given scalemod so that with a scale and a scalemod
         * of 1.0, for instance, the trial values of multiplicative
         * parameters on the simplex will be the center plus 1.0.  If we
         * just used the given scalemod this wouldn't be true, since all
         * parameters are stored as additive parameters.  In other words:
         * this looks weird but it's correct.
         */

        element->scalemod[param]   = log(1.0 + scalemod/center);
    }
    else /* additive parameter */
    {
        element->realcenter[param] = element->center[param];
        element->realrange[param]  = element->range[param];
        element->realmin[param]    = element->min[param];
        element->realmax[param]    = element->max[param];
        element->scalemod[param]   = scalemod;
    }

    /*
     * Use next_index to keep track of the number of parameters loaded
     * by this function.  This is a bit of a hack but it works o.k.
     */

    ++element->next_index;

    return 1;
}




/*
 * SetSearchSA
 *
 * FUNCTION
 *     Sets the search flags for the paramtableSA object.
 *     called by do_setsearch
 *
 * ARGUMENTS
 *     int     argc          -- number of command arguments
 *                              (not including flags)
 *     char  **argv          -- command arguments proper
 *     short   all, not, add -- flags passed from do_setsearch
 *
 * RETURN VALUE
 *     int -- 1 = success; 0 = failure
 *
 * AUTHOR
 *     Mike Vanier
 */

int
SetSearchSA(argc, argv, all, not, add)
    int argc;
    char **argv;
    short all;
    short not;
    short add;
{
    int    i, param, tosearch;
    struct paramtableSA_type *element;

    element = (struct paramtableSA_type*) GetElement(argv[1]);

    if (all)
    {
        for (i = 0; i < element->num_params; i++)
            element->search[i] = 1;
        return 1;
    }
    else if (not)
    {
        for (i = 0; i < element->num_params; i++)
            element->search[i] = 1;
    }
    else if (!add)
    {
        for (i = 0; i < element->num_params; i++)
            element->search[i] = 0;
    }


    /* Range checking.  Abort on any range errors. */

    for (i = 2; i < argc; i++)
    {
        param = atoi(argv[i]);

        if ((param >= element->num_params) ||
            (param < 0))
        {
            Error();
            printf("setsearch: parameter index %d is out of range!\n",
                   param);
            return 0;
        }
    }


    /* Range is OK, so go ahead. */

    for (i = 2; i < argc; i++)
    {
        param = atoi(argv[i]);

        if (not)
            element->search[param] = 0;
        else
            element->search[param] = 1;
    }

    /*
     * Calculate the number of parameters to search over.  This is
     * needed by the simplex algorithm.
     */

    tosearch = 0;

    for (i = 0; i < element->num_params; i++)
        if (element->search[i] == 1)
            ++tosearch;

    element->num_params_to_search = tosearch;

    return 1;
}
