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

/***************************************************/
/* These #defines are for derivative calculations. */
/***************************************************/

/* default rate of decrease of derivative step size */
#define DERIV_DECR     1.4

/* a ridiculously large error */
#define DERIV_HUGE_ERR HUGE

/* default size of the derivative matrix */
#define DERIV_NTAB     10

/* derivative routine returns when the error is */
/* DERIV_SAFE worse than the best so far        */
#define DERIV_SAFE     2.0

/* amount to decrease h_init when error in derivatives is too large */
#define DERIV_DECR_ON_ERROR  5.0

/* Maximum proportional error in derivative calculations. */
#define DERIV_MAX_ERROR      0.2

/* Maximum number of function evaluations for one derivative. */
#define DERIV_MAX_COUNT      (4 * DERIV_NTAB)

#define DERIV_SMALL_VALUE 0.001
#define DERIV_SMALL_ERROR 0.001

#define DERIV_TINY 0.0001

/* These #defines are the states of the derivative routines. */

#define DERIV1HI  1
#define DERIV1LO  2
#define DERIV2HI  3
#define DERIV2LO  4

/*
 * These #defines are the default initial spatial step sizes for the
 * derivative routines.  They are different for additive and multiplicative
 * parameters even though all parameters are internally manipulated as
 * additive parameters.  This is because additive parameters typically
 * represent things like midpoints of minf curves which are extremely
 * sensitive to changes, whereas multiplicative parameters represent things
 * like Gbars which are less sensitive.  These values can be overridden by
 * the user.
 */

#define DERIV_INIT_H_ADD  0.002
#define DERIV_INIT_H_MULT 0.01

/*
 * Don't do smaller steps than this;
 * it won't be very informative or accurate...
 */
#define DERIV_MIN_H_ADD      0.00002
#define DERIV_MIN_H_MULT     0.0001

/*
 * These #defines are used in check_derivative() to determine how good the
 * estimated derivative is.
 */

#define DERIV_OK             1001
#define DERIV_REDUCE_H       1002
#define DERIV_UNSATISFACTORY 1003


/**************************************************************/
/* These #defines are used by the line minimization routines. */
/**************************************************************/

/* These #defines are the states of the line minimization routines. */

#define LINEMIN_BRACKET 1  /* bracketing the minimum                 */
#define LINEMIN_BRENT   2  /* line minimization using Brent's method */

/*
 * This #define is the initial position of the line minimization
 * bracketing routine.
 */

#define LINEMIN_BRACKET_INITSTEP 0.01

/* These #defines are the states of the bracketing routine. */

#define LINEMIN_BRACKET_INIT   1001
#define LINEMIN_BRACKET_INIT2  1002
#define LINEMIN_BRACKET_TEST1  1003
#define LINEMIN_BRACKET_TEST2  1004
#define LINEMIN_BRACKET_TEST3  1005
#define LINEMIN_BRACKET_TEST4  1006

/* These #defines are the states of the brent routine. */

#define LINEMIN_BRENT_INIT   1007
#define LINEMIN_BRENT_MAIN   1008

/* These macros are used by the bracketing routine. */

#define SWAP(A, B)         temp = (A); (A) = (B); (B) = temp;
#define SHIFT(A, B, C, D)  (A) = (B); (B) = (C); (C) = (D);
#define GOLD               1.61803398874989484820  /* Golden ratio */
#define CGOLD              0.38196601125010515180  /* 1 - 1/GOLD   */
#define GLIMIT             100.0

/* maximum number of iterations of linemin algorithm */
#define ITMAX              100

#define ZEPS               1.0e-10

/*
 * Default tolerance of line minimization estimates;
 * 3.0e-8 will give maximum accuracy.
 */
#define TOL                1.0e-5

#define EPS                1.0e-10

/* Default convergence tolerance of CG routine as a whole;  */
/* if match doesn't change by at least this much then stop. */
#define FTOL               1.0e-4


#define DMAX(a, b) (dmaxarg1 = (a), dmaxarg2 = (b),  \
                    (dmaxarg1) > (dmaxarg2) ?        \
                    (dmaxarg1) : (dmaxarg2))

#define SIGN(a, b) ((b) > 0.0 ? fabs(a) : -fabs(a))


/*********************************************************************/
/* These #defines are the states of the parameter search as a whole. */
/*********************************************************************/

#define START    0
#define GRADIENT 1
#define LINEMIN  2

/* forward reference */
void
ParamtableCG_ACCEPT();


/*
 * adjust_to_range
 *
 * FUNCTION
 *     Adjusts the value of a number to be within a min/max
 *     range using a wraparound algorithm.
 *
 * ARGUMENTS
 *     num -- the number to be adjusted
 *     min -- the minimum allowed value
 *     max -- the maximum allowed value
 *
 * RETURN VALUE
 *     double -- the adjusted value of num
 *
 * AUTHOR
 *     Mike Vanier
 */

double
adjust_to_range(num, min, max)
    double num;
    double min;
    double max;
{
    double range = min - max;

    if (num > max)
    {
        num -= ((int)((num - min) / range) * range);
        return num;
    }
    else if (num < min)
    {
        num += ((int)((max - num) / range) * range);
        return num;
    }
    else
    {
        return num;
    }
}




/*
 * set_next_point
 *
 * FUNCTION
 *     Assigns values to the current fields based on the
 *     realcurrent fields.  Used by UPDATE_PARAMS action.  The
 *     script can only access the current fields.
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
set_next_point(element)
    struct paramtableCG_type *element;
{
    int i;

    for (i = 0; i < element->num_params; i++)
    {
        if (element->type[i] == 0) /* additive */
        {
            element->current[i] = element->realcurrent[i];
        }
        else /* multiplicative */
        {
            element->current[i] = exp(element->realcurrent[i]);
        }
    }
}




/*
 * check_derivative
 *
 * FUNCTION
 *     Checks the quality of the returned derivative.
 *
 * ARGUMENTS
 *     deriv -- value of derivative
 *     error -- error estimate of derivative
 *     count -- number of function evaluations in the derivative so far
 *     h     -- spatial step size
 *     hmin  -- minimum spatial step size
 *
 * RETURN VALUE
 *     int -- gives the estimate of the quality of the result.
 *
 * AUTHOR
 *     Mike Vanier
 */

int
check_derivative(deriv, error, count, h, hmin)
    double deriv;
    double error;
    int count;
    double h;
    double hmin;
{
    /*
     * If derivative is essentially zero with a (reasonably) small error
     * then return OK.
     */

    if ((fabs(deriv) < DERIV_SMALL_VALUE)
        && (fabs(error) < DERIV_SMALL_ERROR))
    {
        return DERIV_OK;
    }

    /*
     * If h is less than minimum allowed h value then accept the result
     * regardless.  Smaller step sizes will probably not improve the
     * accuracy since if the step is too small you often get derivatives of
     * zero (at least for spike comparisons).
     */

    if (h < hmin)
    {
        if (IsSilent() < 1)
        {
            printf("paramtableCG: check_derivative: Accepting derivative %f with error %f for step size %f.\n",
                   deriv, error, h);
        }

        return DERIV_OK;
    }

    if ((fabs(error / deriv) > DERIV_MAX_ERROR) &&
        (count < DERIV_MAX_COUNT))
    {
        return DERIV_REDUCE_H;
    }
    else if ((fabs(error / deriv) > DERIV_MAX_ERROR) &&
             (count >= DERIV_MAX_COUNT))
    {
        return DERIV_UNSATISFACTORY;
    }
    else
    {
        return DERIV_OK;
    }
}




/*
 * init_derivative
 *
 * FUNCTION
 *     Initializes derivative calculation for the currently
 *     selected parameter.
 *
 * ARGUMENTS
 *     element     -- address of element
 *     reset_count -- flag: if nonzero, deriv_count is set to 0
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
init_derivative(element, reset_count)
    struct paramtableCG_type *element;
    short reset_count;
{
    int i, j;
    int index;

    for (i = 0; i < DERIV_NTAB; i++)
    {
        for (j = 0; j < DERIV_NTAB; j++)
        {
            element->deriv_matrix[i][j] = 0.0;
        }
    }

    index                        = element->deriv_index;
    element->orig_param          = element->realcurrent[index];
    element->h                   = element->h_init;
    element->realcurrent[index] += element->h;
    element->deriv_state         = DERIV1HI;

    if (reset_count)
    {
        element->deriv_count = 0;
    }
}




/*
 * do_derivative
 *
 * FUNCTION
 *     Calculates d(match)/d(param) for a given parameter.
 *
 * ARGUMENTS
 *     element  -- address of element
 *     match    -- most recent match value
 *     next     -- next point to be simulated
 *     deriv    -- result, if available
 *     err      -- error estimate of result, if available
 *     saveflag -- flag for saving/restoring static variables;
 *                 0 = ignore, 1 = save, 2 = restore
 *     savefp   -- file pointer to save in.
 *
 * RETURN VALUE
 *     int -- 0 = not done; 1 = done
 *
 * AUTHOR
 *     Mike Vanier; this borrows heavily from Numerical Recipes in C
 *     (Press et. al., 2nd. ed., pp. 188-189) to whom all blame should
 *     be addressed :-)
 */

int
do_derivative(element, match, next, deriv, err, saveflag, savefp)
    struct paramtableCG_type *element;
    double match;
    double *next;
    double *deriv;
    double *err;
    short saveflag;
    FILE *savefp;
{
    static int    i;
    int           j;
    double        fac, errt;
    static double hval, lval;  /* low and high values of the
                                * derivative calculations              */
    double        deriv_est;   /* estimate of the derivative           */
    static double err_est;     /* estimate of the error                */
    static double answer;      /* the best derivative estimate so far  */
    double        decr, decr2;


    /*
     * Save or restore static variables if necessary.
     */

    if (saveflag == 1) /* Save static variables. */
    {
        paramFwrite(&i,       sizeof(int), 1, savefp);
        paramFwrite(&hval,    sizeof(double), 1, savefp);
        paramFwrite(&lval,    sizeof(double), 1, savefp);
        paramFwrite(&err_est, sizeof(double), 1, savefp);
        paramFwrite(&answer,  sizeof(double), 1, savefp);

        return 0;
    }
    else if (saveflag == 2) /* Restore static variables. */
    {
        fread(&i,       sizeof(int), 1, savefp);
        fread(&hval,    sizeof(double), 1, savefp);
        fread(&lval,    sizeof(double), 1, savefp);
        fread(&err_est, sizeof(double), 1, savefp);
        fread(&answer,  sizeof(double), 1, savefp);

        return 0;
    }


    /*
     * If we got to here we assume that the static variables are correct.
     */

    /* decr is the rate of h decrease */
    decr  = element->deriv_h_decrease[element->deriv_index];
    decr2 = decr * decr;

    switch (element->deriv_state)
    {
    case DERIV1HI:

        err_est = DERIV_HUGE_ERR; /* give it a large value to start with */
        hval    = match;
        *next   = element->orig_param - element->h;
        element->deriv_state = DERIV1LO;

        break;


    case DERIV1LO:

        lval = match;
        deriv_est = (hval - lval) / (2.0 * element->h);
        element->deriv_matrix[0][0] = deriv_est;
        i = 1;
        element->h /= decr;  /* decrease step size */
        *next = element->orig_param + element->h;
        element->deriv_state = DERIV2HI;

        break;


    case DERIV2HI:

        hval = match;
        *next = element->orig_param - element->h;
        element->deriv_state = DERIV2LO;

        break;


    case DERIV2LO:

        lval = match;
        deriv_est = (hval - lval) / (2.0 * element->h);
        element->deriv_matrix[0][i] = deriv_est;
        fac = decr2;

        for (j = 1; j <= i; j++)
        {
            double arg1, arg2;

            element->deriv_matrix[j][i] =
                (element->deriv_matrix[j-1][i] * fac -
                 element->deriv_matrix[j-1][i-1]) / (fac - 1.0);
            fac *= decr2;

            arg1 = fabs(element->deriv_matrix[j][i]
                        - element->deriv_matrix[j-1][i]);
            arg2 = fabs(element->deriv_matrix[j][i]
                        - element->deriv_matrix[j-1][i-1]);

            /* Find maximum of arg1 and arg2 and assign to errt */

            errt = arg1 > arg2 ? arg1 : arg2;

            if (errt < err_est)
            {
                err_est = errt;
                answer  = element->deriv_matrix[j][i];
            }
        }

        /*
         * Quit early if the higher order estimate is worse by a
         * significant factor DERIV_SAFE.
         */

        if (fabs(element->deriv_matrix[i][i]
                 - element->deriv_matrix[i-1][i-1])
            >= (DERIV_SAFE * err_est))
        {
            *deriv = answer;
            *err   = err_est;
            return 1;
        }

        i++;

        /* Quit if we've reached the end of the estimation process. */

        if (i == DERIV_NTAB)
        {
            *deriv = answer;
            *err   = err_est;
            return 1;
        }

        element->h /= DERIV_DECR;
        *next = element->orig_param + element->h;
        element->deriv_state = DERIV2HI;

        break;


    default:

        Error();
        printf("paramtableCG: do_derivative: unknown state %d\n",
               element->deriv_state);

        break;
    }

    return 0;
}




/*
 * init_gradient
 *
 * FUNCTION
 *     Initializes the parameter search object for a gradient calculation.
 *
 * ARGUMENTS
 *     element -- address of element
 *
 * RETURN VALUE
 *     int -- 0 = failure, 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
init_gradient(element)
    struct paramtableCG_type *element;
{
    element->deriv_index = 0; /* Start with parameter 0. */

    /* Don't run search on parameters unless explicitly told to. */

    while (element->search[element->deriv_index] == 0)
    {
        ++element->deriv_index;

        if (element->deriv_index == element->num_params)
        {
            Error();
            printf("paramtableCG: init_gradient: must have at least one searchable parameter!\n");
            return 0;
        }
    }

    element->h_init = element->deriv_h_init[element->deriv_index];
    init_derivative(element, 1);
    return 1;
}




/*
 * do_gradient
 *
 * FUNCTION
 *     Calculates d(match)/d(param) for all parameters i.e. the
 *     gradient.
 *
 * ARGUMENTS
 *     element -- address of element
 *     match   -- most recent match value
 *
 * RETURN VALUE
 *     int -- 0 = not done; 1 = done
 *
 * AUTHOR
 *     Mike Vanier
 */

int
do_gradient(element, match)
    struct paramtableCG_type *element;
    double match;
{
    double next, deriv;
    double err;
    int    deriv_quality;

    ++element->deriv_count;

    if (do_derivative(element, match, &next, &deriv, &err,
                      0, (FILE *)NULL) == 1)
    {
        /* Finished the derivative calculation for the current parameter. */

        /*
         * Check if the error is too great; if so, decrease the step size
         * and try again.
         */

        deriv_quality
            = check_derivative(deriv, err, element->deriv_count,
                               element->h,
                               element->deriv_h_min[element->deriv_index]);

        if (deriv_quality == DERIV_REDUCE_H)
            /*
             * The derivative estimate is unreliable so reduce h_init and
             * start over.
             */
        {
            /* restore original value */
            element->realcurrent[element->deriv_index]
                = element->orig_param;
            element->h_init /= DERIV_DECR_ON_ERROR;
            init_derivative(element, 0);

            return 0;  /* not done yet */
        }

        if (deriv_quality == DERIV_UNSATISFACTORY)
        {
            /*
             * The derivative estimate is unreliable but we've done the
             * maximum permitted number of function evaluations anyway, so
             * continue with a warning.
             */

            Warning();
            printf("paramtableCG: do_gradient: estimated derivative of parameter %d is %f with error %f !\n",
                   element->deriv_index, deriv, err);
        }

        element->deriv[element->deriv_index] = deriv;
        element->realcurrent[element->deriv_index]
            = element->orig_param; /* restore original value */
        ++element->deriv_index;

        if (element->deriv_index == element->num_params)
        {
            /* We're done computing derivatives for all parameters. */
            return 1;
        }

        /*
         * Get the next parameter to be searched if any;
         * don't search parameters unless explicitly told to.
         */

        while (element->search[element->deriv_index] == 0)
        {
            ++element->deriv_index;

            if (element->deriv_index == element->num_params)
            {
                /* We're done computing derivatives for all parameters. */
                return 1;
            }
        }

        element->h_init = element->deriv_h_init[element->deriv_index];
        init_derivative(element, 1);
    }
    else
    {
        element->realcurrent[element->deriv_index] = next;
    }

    return 0;  /* not done yet */
}




/*
 * init_linemin
 *
 * FUNCTION
 *     Initializes the parameter search object for a line minimization.
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
init_linemin(element)
    struct paramtableCG_type *element;
{
    int i;
    double numerator = 0.0, denominator = 0.0, gamma;

    ++element->linemin_number;
    element->prev_linemin_match = element->linemin_match;

    /*
     * Generate a conjugate direction.
     */

    if (element->linemin_init)
    {
        /*
         * If this is the first line minimization use the gradient
         * direction.
         */

        for (i = 0; i < element->num_params; i++)
        {
            if (!element->search[i]) /* Don't search this parameter. */
            {
                element->dir[i] = element->g[i] = 0.0;
            }
            else
            {
                element->dir[i] = element->g[i] = -element->deriv[i];
            }
        }
    }
    else /* Calculate a new direction conjugate to previous directions. */
    {
        for (i = 0; i < element->num_params; i++)
        {
            if (!element->search[i]) /* Don't search this parameter. */
            {
                continue;
            }

            numerator   += (element->deriv[i] + element->g[i])
                * element->deriv[i];
            denominator += element->g[i] * element->g[i];
        }

        gamma = numerator / denominator;

        for (i = 0; i < element->num_params; i++)
        {
            if (!element->search[i]) /* Don't search this parameter. */
            {
                element->dir[i] = element->g[i] = 0.0;
            }
            else
            {
                element->g[i]   = -element->deriv[i];
                element->dir[i] = element->g[i] + gamma * element->dir[i];
            }
        }
    }

    /*
     * Put the (real)current parameter values into the linemin_origin
     * fields.
     */

    for (i = 0; i < element->num_params; i++)
    {
        element->linemin_origin[i] = element->realcurrent[i];
    }

    element->linemin_state         = LINEMIN_BRACKET;
    element->linemin_bracket_state = LINEMIN_BRACKET_INIT;
    element->linemin_bracket[0]    = 0.0;
    element->linemin_mbracket[0]   = element->linemin_match;

    element->linemin_bracket[1]    = LINEMIN_BRACKET_INITSTEP;
    element->linemin_point         = element->linemin_bracket[1];


    /* Choose new values for realcurrent[] based on linemin_point. */

    for (i = 0; i < element->num_params; i++)
    {
        if (!element->search[i]) /* Don't search this parameter. */
        {
            element->realcurrent[i] = element->linemin_origin[i];
        }
        else
        {
            element->realcurrent[i] = element->linemin_origin[i] +
                element->dir[i] * element->linemin_point;
        }

        /*
         * Use a wraparound algorithm to make sure realcurrent is in the
         * correct range.
         */

        element->realcurrent[i]
            = adjust_to_range(element->realcurrent[i],
                              element->realmin[i], element->realmax[i]);
    }
}




/*
 * do_bracket
 *
 * FUNCTION
 *     Brackets a 1-d minimum.
 *
 * ARGUMENTS
 *     element  -- address of element
 *     match    -- most recent match value
 *     saveflag -- flag for saving/restoring static variables;
 *                 0 = ignore, 1 = save, 2 = restore
 *     savefp   -- file pointer to save in.
 *
 * RETURN VALUE
 *     int -- 0 if not done, 1 if done.
 *
 * AUTHOR
 *     Mike Vanier; this borrows heavily from the mnbrak routine
 *     in Numerical Recipes in C, 2nd. ed. pp. 400-401
 */

int
do_bracket(element, match, saveflag, savefp)
    struct paramtableCG_type *element;
    double match;
    short saveflag;
    FILE *savefp;
{
    int    done = 0;
    double temp;               /* used by SWAP macro */
    double dmaxarg1, dmaxarg2; /* used by DMAX macro */
    static double r, q, u, ulim, fu, fu2;
    static double ax, bx, cx, fa, fb, fc; /* bracketing triple and
                                           * match values of same */

    /*
     * Save or restore static variables if necessary.
     */

    if (saveflag == 1) /* Save static variables. */
    {
        paramFwrite(&r,    sizeof(double), 1, savefp);
        paramFwrite(&q,    sizeof(double), 1, savefp);
        paramFwrite(&u,    sizeof(double), 1, savefp);
        paramFwrite(&ulim, sizeof(double), 1, savefp);
        paramFwrite(&fu,   sizeof(double), 1, savefp);
        paramFwrite(&fu2,  sizeof(double), 1, savefp);
        paramFwrite(&ax,   sizeof(double), 1, savefp);
        paramFwrite(&bx,   sizeof(double), 1, savefp);
        paramFwrite(&cx,   sizeof(double), 1, savefp);
        paramFwrite(&fa,   sizeof(double), 1, savefp);
        paramFwrite(&fb,   sizeof(double), 1, savefp);
        paramFwrite(&fc,   sizeof(double), 1, savefp);

        return 0;
    }
    else if (saveflag == 2) /* Restore static variables. */
    {
        fread(&r,    sizeof(double), 1, savefp);
        fread(&q,    sizeof(double), 1, savefp);
        fread(&u,    sizeof(double), 1, savefp);
        fread(&ulim, sizeof(double), 1, savefp);
        fread(&fu,   sizeof(double), 1, savefp);
        fread(&fu2,  sizeof(double), 1, savefp);
        fread(&ax,   sizeof(double), 1, savefp);
        fread(&bx,   sizeof(double), 1, savefp);
        fread(&cx,   sizeof(double), 1, savefp);
        fread(&fa,   sizeof(double), 1, savefp);
        fread(&fb,   sizeof(double), 1, savefp);
        fread(&fc,   sizeof(double), 1, savefp);

        return 0;
    }


    /*
     * When this function is first called, the match value is expected to
     * correspond to the initial point on the line where minimization is to
     * take place.  The routine first chooses a new point and simulates
     * that point, and then, based on the results of the two matches,
     * attempts to find a third value bracketing the two points.
     */

    switch (element->linemin_bracket_state)
    {
    case LINEMIN_BRACKET_INIT:

        ax = element->linemin_bracket[0];
        fa = element->linemin_mbracket[0];
        bx = element->linemin_bracket[1];
        fb = match;

        /*
         * Swap values if necessary so that ax has a higher match
         * value than bx.
         */

        if (fb > fa)
        {
            SWAP(bx, ax);
            SWAP(fb, fa);
        }

        cx = bx + GOLD * (bx - ax);  /* first guess for c */
        element->linemin_point = cx;
        element->linemin_bracket_state = LINEMIN_BRACKET_INIT2;

        break;


    case LINEMIN_BRACKET_INIT2:

        fc = match;

    MAIN:  /* keep returning here until we bracket */
        /*
         * If point 2 is higher than point 1 then we're done.
         */

        if (fc > fb)
        {
            done = 1;
            break;
        }

        /*
         * Compute u by parabolic extrapolation from ax, bx, cx.
         * TINY is used to prevent division by zero.
         */

        r = (bx - ax) * (fb - fc);
        q = (bx - cx) * (fb - fa);
        u = bx - ((bx - cx) * q  - (bx - ax) * r) /
            (2.0 * SIGN(DMAX(fabs(q - r), STINY), (q - r)));

        /* Don't go farther than ulim. */

        ulim = bx + GLIMIT * (cx - bx);

        /* Test various possibilities. */

        if ((bx - u) * (u - cx) > 0.0)
        {
            /* Parabolic u is between b and c; try it. */

            element->linemin_point = u;
            element->linemin_bracket_state = LINEMIN_BRACKET_TEST1;

            break;
        }
        else if ((cx - u) * (u - ulim) > 0.0)
        {
            /* Parabolic u is between c and its allowed limit. */

            element->linemin_point = u;
            element->linemin_bracket_state = LINEMIN_BRACKET_TEST3;

            break;
        }
        else if ((u - ulim) * (ulim - cx) >= 0.0)
        {
            /* Limit parabolic u to its maximum allowed value. */

            u = ulim;
            element->linemin_point = u;
            element->linemin_bracket_state = LINEMIN_BRACKET_TEST2;

            break;
        }
        else
        {
            /* Reject parabolic u; use default magnification. */

            u = cx + GOLD * (cx - bx);
            element->linemin_point = u;
            element->linemin_bracket_state = LINEMIN_BRACKET_TEST2;

            break;
        }


    case LINEMIN_BRACKET_TEST1:

        fu = match;

        if (fu < fc) /* Got a minimum between b and c. */
        {
            ax = bx;
            bx = u;
            fa = fb;
            fb = fu;
            done = 1;

            break;
        }
        else if (fu > fb) /* Got a minimum between a and u. */
        {
            cx = u;
            fc = fu;
            done = 1;

            break;
        }

        u = cx + GOLD * (cx - bx);
        element->linemin_point = u;
        element->linemin_bracket_state = LINEMIN_BRACKET_TEST2;

        break;


    case LINEMIN_BRACKET_TEST2:

        fu = match;
        /* eliminate oldest point and continue */
        SHIFT(ax, bx, cx, u);
        SHIFT(fa, fb, fc, fu);

        goto MAIN;

        /*NOTREACHED*/
        break;


    case LINEMIN_BRACKET_TEST3:

        fu = match;

        if (fu < fc)
        {
            SHIFT(bx, cx, u, (cx + GOLD * (cx - bx)));
            element->linemin_point = u;
            element->linemin_bracket_state = LINEMIN_BRACKET_TEST4;

            break;
        }
        else
        {
            /* Eliminate oldest point and continue. */

            SHIFT(ax, bx, cx, u);
            SHIFT(fa, fb, fc, fu);

            goto MAIN;
        }

        /*NOTREACHED*/
        break;


    case LINEMIN_BRACKET_TEST4:

        fu2 = match;
        SHIFT(fb, fc, fu, fu2);

        /* Eliminate oldest point and continue. */

        SHIFT(ax, bx, cx, u);
        SHIFT(fa, fb, fc, fu);

        goto MAIN;

        /*NOTREACHED*/
        break;


    default:

        Error();
        printf("paramtableCG: do_bracket: unknown state %d\n",
               element->linemin_bracket_state);
    }

    if (done)
    {
        element->linemin_bracket[0]  = ax;
        element->linemin_bracket[1]  = bx;
        element->linemin_bracket[2]  = cx;
        element->linemin_mbracket[0] = fa;
        element->linemin_mbracket[1] = fb;
        element->linemin_mbracket[2] = fc;

        return 1;  /* done bracketing */
    }
    else
    {
        return 0; /* not done yet */
    }
}




/*
 * do_brent
 *
 * FUNCTION
 *     Applies Brent's method for one-d line minimization.
 *
 * ARGUMENTS
 *     element -- address of element
 *     match   -- most recent match value
 *     saveflag -- flag for saving/restoring static variables;
 *                 0 = ignore, 1 = save, 2 = restore
 *     savefp   -- file pointer to save in.
 *
 * RETURN VALUE
 *     int -- 0 if not done, 1 if done.
 *
 * AUTHOR
 *     Mike Vanier; this borrows heavily from code in Numerical Recipes
 *     in C, 2nd. Ed. p. 404-5.
 */

int
do_brent(element, match, saveflag, savefp)
    struct paramtableCG_type *element;
    double match;
    short saveflag;
    FILE *savefp;
{
    double ax, bx, cx, etemp, p, q, r, xm, tol1, tol2;
    static double a, b, u, x, w, v, fw, fu, fv, fx;
    static int iter;
    static double d;
    static double e;  /* This is the distance moved
                       * on the step before last.   */


    /*
     * Save or restore static variables if necessary.
     */

    if (saveflag == 1) /* Save static variables. */
    {
        paramFwrite(&a,    sizeof(double), 1, savefp);
        paramFwrite(&b,    sizeof(double), 1, savefp);
        paramFwrite(&u,    sizeof(double), 1, savefp);
        paramFwrite(&x,    sizeof(double), 1, savefp);
        paramFwrite(&w,    sizeof(double), 1, savefp);
        paramFwrite(&v,    sizeof(double), 1, savefp);
        paramFwrite(&fw,   sizeof(double), 1, savefp);
        paramFwrite(&fu,   sizeof(double), 1, savefp);
        paramFwrite(&fv,   sizeof(double), 1, savefp);
        paramFwrite(&fx,   sizeof(double), 1, savefp);
        paramFwrite(&iter, sizeof(int),    1, savefp);
        paramFwrite(&d,    sizeof(double), 1, savefp);
        paramFwrite(&e,    sizeof(double), 1, savefp);

        return 0;
    }
    else if (saveflag == 2) /* Restore static variables. */
    {
        fread(&a,    sizeof(double), 1, savefp);
        fread(&b,    sizeof(double), 1, savefp);
        fread(&u,    sizeof(double), 1, savefp);
        fread(&x,    sizeof(double), 1, savefp);
        fread(&w,    sizeof(double), 1, savefp);
        fread(&v,    sizeof(double), 1, savefp);
        fread(&fw,   sizeof(double), 1, savefp);
        fread(&fu,   sizeof(double), 1, savefp);
        fread(&fv,   sizeof(double), 1, savefp);
        fread(&fx,   sizeof(double), 1, savefp);
        fread(&iter, sizeof(int),    1, savefp);
        fread(&d,    sizeof(double), 1, savefp);
        fread(&e,    sizeof(double), 1, savefp);

        return 0;
    }


    switch (element->linemin_brent_state)
    {
    case LINEMIN_BRENT_INIT:

        ax = element->linemin_bracket[0];
        bx = element->linemin_bracket[1];
        cx = element->linemin_bracket[2];

        d = e = 0.0;

        /*
         * a and b must be in ascending order, but the
         * corresponding match values need not be.
         */

        a = (ax < cx ? ax : cx);
        b = (ax > cx ? ax : cx);

        x = w = v = bx;
        fw = fv = fx = element->linemin_mbracket[1];
        iter = 0;
        element->linemin_brent_state = LINEMIN_BRENT_MAIN;

    MAIN:  /* label for a goto */

        if (iter == ITMAX)
        {
            Error();
            printf("paramtableCG: do_brent: too many iterations!\n");
            element->linemin_point = x;
            element->linemin_match = fx;

            return 1;
        }

        xm   = 0.5 * (a + b);
        tol1 = element->linemin_tolerance * fabs(x) + ZEPS;
        tol2 = 2.0 * tol1;

        if (fabs(x - xm) <= (tol2 - 0.5 * (b - a)))
        {
            /* We're done. */

            element->linemin_point = x;
            element->linemin_match = fx;

            return 1;
        }

        if (fabs(e) > tol1)
        {
            /* Construct a trial parabolic fit. */

            r = (x - w) * (fx - fv);
            q = (x - v) * (fx - fw);
            p = (x - v) * q - (x - w) * r;
            q = 2.0 * (q - r);

            if (q > 0.0)
            {
                p = -p;
            }

            q = fabs(q);
            etemp = e;
            e = d;

            if ((fabs(p) >= fabs(0.5 * q * etemp))
                || (p <= q * (a - x))
                || (p >= q * (b - x)))
            {
                /*
                 * Is the parabolic fit acceptable?  If not, take the
                 * golden section step into the larger of the two segments.
                 */

                e = (x >= xm ? (a - x) : (b - x));
                d = CGOLD * e;
            }
            else
            {
                /* Take the parabolic step. */

                d = p / q;
                u = x + d;

                if (((u - a) < tol2) ||
                    ((b - u) < tol2))
                {
                    d = SIGN(tol1, (xm - x));
                }
            }
        }
        else
        {
            e = (x >= xm ? (a - x) : (b - x));
            d = CGOLD * e;
        }

        u = (fabs(d) >= tol1) ? (x + d) : (x + SIGN(tol1, d));
        element->linemin_point = u;

        break; /* go get another match value */


    case LINEMIN_BRENT_MAIN:

        ++iter;
        fu = element->linemin_match = match;

        if (fu <= fx)
        {
            if (u >= x)
                a = x;
            else
                b = x;

            SHIFT(v, w, x, u);
            SHIFT(fv, fw, fx, fu);
        }
        else
        {
            if (u < x)
                a = u;
            else
                b = u;

            if ((fu <= fw) || (w == x))
            {
                v  = w;
                w  = u;
                fv = fw;
                fw = fu;
            }
            else if ((fu <= fw) || (v == x) || (v == w))
            {
                v  = u;
                fv = fu;
            }
        }

        goto MAIN;
    }

    return 0; /* not done yet */
}




/*
 * do_linemin
 *
 * FUNCTION
 *     Does calculations for one step of line minimization.
 *
 * ARGUMENTS
 *     element -- address of element
 *     match   -- most recent match value
 *
 * RETURN VALUE
 *     int -- 0 if not done, 1 if done
 *
 * AUTHOR
 *     Mike Vanier
 */

int
do_linemin(element, match)
    struct paramtableCG_type *element;
    double match;
{
    int i;

    switch (element->linemin_state)
    {
    case LINEMIN_BRACKET:

        if (do_bracket(element, match, 0, (FILE *)NULL) == 1)
        {
            element->linemin_state       = LINEMIN_BRENT;
            element->linemin_brent_state = LINEMIN_BRENT_INIT;
            /* This initializes the brent routine: */
            (void) do_brent(element, 0.0, 0, (FILE *)NULL);
        }

        break;


    case LINEMIN_BRENT:

        if (do_brent(element, match, 0, (FILE *)NULL) == 1)
            return 1;

        break;


    default:

        Error();
        printf("paramtableCG: do_linemin: unknown state %d!\n",
               element->linemin_state);
    }

    /* Choose new values for realcurrent[] based on linemin_point. */

    /*
     * This is where we check to see if the bracketed range
     * is beyond the min/max bounds of any parameters.  If so do
     * a wraparound.  This is also done in init_linemin above.
     */

    for (i = 0; i < element->num_params; i++)
    {
        if (!element->search[i]) /* Don't search this parameter. */
        {
            element->realcurrent[i] = element->linemin_origin[i];
        }
        else
        {
            element->realcurrent[i] = element->linemin_origin[i] +
                element->dir[i] * element->linemin_point;
        }

        /*
         * Use a wraparound algorithm to make sure realcurrent is in the
         * correct range.
         */

        element->realcurrent[i]
            = adjust_to_range(element->realcurrent[i],
                              element->realmin[i], element->realmax[i]);
    }

    return 0; /* not done yet */
}





/*
 * The following functions are (mostly) action functions for paramtableCG,
 * made into separate functions for convenience in programming and
 * modification.
 */

/*
 * ParamtableCG_TABCREATE
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
ParamtableCG_TABCREATE(element, num_params)
    struct paramtableCG_type *element;
    int num_params;
{
    int i;

    if (num_params < 0)
    {
        Error();
        printf("paramtableCG: TABCREATE: number of parameters must be >= 0!\n");
        return 0;
    }
    else
    {
        element->num_params = num_params;
    }

    if (IsSilent() < 1)
    {
        printf("Creating parameter table with %d parameters.\n",
               element->num_params);
    }

    element->deriv_h_init
        = (double *)calloc(element->num_params, sizeof(double));
    element->deriv_h_decrease
        = (double *)calloc(element->num_params, sizeof(double));
    element->deriv_h_min
        = (double *)calloc(element->num_params, sizeof(double));
    element->deriv_matrix
        = (double **)calloc(DERIV_NTAB, sizeof(double*));

    for (i = 0; i < DERIV_NTAB; i++)
    {
        element->deriv_matrix[i] = (double *)calloc(DERIV_NTAB,
                                                    sizeof(double));
    }

    element->deriv = (double *)calloc(element->num_params, sizeof(double));
    element->g     = (double *)calloc(element->num_params, sizeof(double));
    element->dir   = (double *)calloc(element->num_params, sizeof(double));

    element->linemin_bracket  = (double *)calloc(3, sizeof(double));
    element->linemin_mbracket = (double *)calloc(3, sizeof(double));
    element->linemin_origin   = (double *)calloc(element->num_params,
                                                 sizeof(double));

    element->search      = (short *) calloc(element->num_params,
                                            sizeof(short));
    element->type        = (short *) calloc(element->num_params,
                                            sizeof(short));
    element->center      = (double *)calloc(element->num_params,
                                            sizeof(double));
    element->range       = (double *)calloc(element->num_params,
                                            sizeof(double));
    element->min         = (double *)calloc(element->num_params,
                                            sizeof(double));
    element->max         = (double *)calloc(element->num_params,
                                            sizeof(double));
    element->current     = (double *)calloc(element->num_params,
                                            sizeof(double));
    element->realcenter  = (double *)calloc(element->num_params,
                                            sizeof(double));
    element->realrange   = (double *)calloc(element->num_params,
                                            sizeof(double));
    element->realmin     = (double *)calloc(element->num_params,
                                            sizeof(double));
    element->realmax     = (double *)calloc(element->num_params,
                                            sizeof(double));
    element->realcurrent = (double *)calloc(element->num_params,
                                            sizeof(double));
    element->best        = (double *)calloc(element->num_params,
                                            sizeof(double));

    /*
     * Create array of label strings for parameters.  Allocate an extra
     * character for the terminating null character of the string.
     */

    element->label = (char **)calloc(num_params, sizeof(char *));

    for (i = 0; i < num_params; i++)
        element->label[i] = (char *)calloc((LABEL_SIZE + 1), sizeof(char));

    element->alloced = 1;

    return 1;
}




/*
 * ParamtableCG_TABDELETE
 *
 * FUNCTION
 *     Deallocates memory for parameter tables.
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
ParamtableCG_TABDELETE(element)
    struct paramtableCG_type *element;
{
    int i;

    if (element->alloced == 0)
        return;

    paramFree(element->deriv_h_init);
    paramFree(element->deriv_h_decrease);
    paramFree(element->deriv_h_min);

    for (i = 0; i < DERIV_NTAB; i++)
        paramFree(element->deriv_matrix[i]);

    paramFree(element->deriv_matrix);

    paramFree(element->deriv);
    paramFree(element->g);
    paramFree(element->dir);

    paramFree(element->linemin_bracket);
    paramFree(element->linemin_mbracket);
    paramFree(element->linemin_origin);

    paramFree(element->search);
    paramFree(element->type);
    paramFree(element->center);
    paramFree(element->range);
    paramFree(element->min);
    paramFree(element->max);
    paramFree(element->current);
    paramFree(element->realcenter);
    paramFree(element->realrange);
    paramFree(element->realmin);
    paramFree(element->realmax);
    paramFree(element->realcurrent);
    paramFree(element->best);

    for (i = 0; i < element->num_params; i++)
        paramFree(element->label[i]);

    paramFree(element->label);

    element->alloced = 0;
}




/*
 * ParamtableCG_INITSEARCH
 *
 * FUNCTION
 *     Initializes the parameter search.
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
ParamtableCG_INITSEARCH(element)
    struct paramtableCG_type *element;
{
    /*
     * This action assumes that initparamCG has been called for all
     * parameters.
     */

    int i;

    element->iteration_number = 0;
    element->linemin_init     = 1;
    element->done             = 0;
    element->state            = START;
    element->best_match       = HUGE;  /* a ridiculously bad match */

    for (i = 0; i < element->num_params; i++)
    {
        element->current[i]     = element->center[i];
        element->realcurrent[i] = element->realcenter[i];
    }
}




/*
 * ParamtableCG_RANDOMIZE
 *
 * FUNCTION
 *     Randomizes the parameters being searched,
 *     to give a random starting point.
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
ParamtableCG_RANDOMIZE(element)
    struct paramtableCG_type *element;
{
    int i;

    for (i = 0; i < element->num_params; i++)
    {
        if (element->search[i]) /* search this parameter */
            element->realcurrent[i] = frandom(element->realmin[i],
                                              element->realmax[i]);
    }

    set_next_point(element);
}




/*
 * ParamtableCG_UPDATE_PARAMS
 *
 * FUNCTION
 *     Updates the current parameter table, given the match
 *     value of the previous parameter set simulated.
 *
 * ARGUMENTS
 *     element -- address of element
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableCG_UPDATE_PARAMS(element)
    struct paramtableCG_type *element;
{
    int    i;
    int    gradient_done = 0;
    int    linemin_done  = 0;
    double match = element->current_match;

    switch (element->state)
    {
    case START:
        /*
         * The first match value is the match value of the point
         * where the gradient will be calculated and also where
         * the line minimization will start at.
         */

        element->linemin_match = match;

        if (!init_gradient(element))
        {
            return 0;
        }

        element->state = GRADIENT;

        break;


    case GRADIENT:
        gradient_done = do_gradient(element, match);

        if (gradient_done)
        {
            /*
             * Check to see that there is in fact a gradient we can move
             * along.  If all derivatives are (nearly) zero then stop.
             */

            short no_gradient = 1;  /* a flag to test for valid gradients */

            for (i = 0; i < element->num_params ; i++)
            {
                if (fabs(element->deriv[i]) > DERIV_TINY)
                {
                    no_gradient = 0;
                    break;
                }
            }

            if (no_gradient)
            {
                element->done = 1;
                return 1;
            }

            element->state = LINEMIN;
            init_linemin(element);
        }

        break;


    case LINEMIN:
        /*
         * Do a line minimization.  If the results of two successive
         * line minimizations are very close to one another then stop;
         * you probably won't do appreciably better in this region of
         * parameter space.
         */

        linemin_done = do_linemin(element, match);

        if (linemin_done)
        {
            if (!init_gradient(element))
            {
                return 0;
            }

            element->state = GRADIENT;

            /* Test for done: */

            if ((2.0 * fabs(element->linemin_match
                            - element->prev_linemin_match))
                <= (element->tolerance
                    * (fabs(element->linemin_match)
                       + fabs(element->prev_linemin_match)
                       + EPS)))
            {
                element->done = 1;
            }
            else
            {
                element->done = 0;
            }
        }

        break;


    default:

        Error();
        printf("paramtableCG: UPDATE_PARAMS: unknown state %d!\n",
               element->state);

        return 0;
    }

    set_next_point(element);
    return 1;
}




/*
 * ParamtableCG_ACCEPT
 *
 * FUNCTION
 *     Accepts current parameter table as best parameter table.
 *     Not an actual action; called by the EVALUATE action.
 *
 * ARGUMENTS
 *     element -- address of element
 *     match   -- match value to be accepted
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
ParamtableCG_ACCEPT(element, match)
    struct paramtableCG_type *element;
    double match;
{
    int i;

    for (i = 0; i < element->num_params; i++)
        element->best[i] = element->current[i];

    element->best_match = match;
}




/*
 * ParamtableCG_LOADBEST
 *
 * FUNCTION
 *     Copies best parameter values to current values;
 *     can be used to start the search at the best values
 *     found so far.
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
ParamtableCG_LOADBEST(element)
    struct paramtableCG_type *element;
{
    int i;

    for (i = 0; i < element->num_params; i++)
    {
        if (element->type[i] == 0) /* additive */
        {
            element->current[i]
                = element->realcurrent[i] = element->best[i];
        }
        else /* multiplicative */
        {
            element->current[i]     = element->best[i];
            element->realcurrent[i] = log(element->current[i]);
        }
    }
}




/*
 * ParamtableCG_RESTART
 *
 * FUNCTION
 *     Re-initializes parameter search at the
 *     best point found so far.
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
ParamtableCG_RESTART(element)
    struct paramtableCG_type *element;
{
    int i;

    for (i = 0; i < element->num_params; i++)
    {
        element->current[i] = element->best[i];

        if (element->type[i] == 0) /* additive */
        {
            element->realcurrent[i] = element->best[i];
        }
        else /* multiplicative */
        {
            element->realcurrent[i] = log(element->best[i]);
        }
    }

    element->linemin_init = 1;
    element->done         = 0;
    element->state        = START;
}




/*
 * ParamtableCG_SAVE
 *
 * FUNCTION
 *     Saves the object's state to a binary file.
 *
 * ARGUMENTS
 *     element -- address of element
 *     filename -- name of file to save in
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableCG_SAVE(element, filename)
    struct paramtableCG_type *element;
    char *filename;
{
    FILE *fp;
    int   i;

    assert(filename != NULL);

    fp = fopen(filename, "w");

    if (fp == NULL)
    {
        Error();
        printf("paramtableCG: SAVE: file cannot be opened for SAVE action!\n");
        return 0;
    }

    paramFwrite(&element->iteration_number, sizeof(int),    1, fp);
    paramFwrite(&element->linemin_number,   sizeof(int),    1, fp);
    paramFwrite(&element->num_params,       sizeof(int),    1, fp);
    paramFwrite(&element->state,            sizeof(short),  1, fp);

    paramFwrite(&element->deriv_method,     sizeof(short),  1, fp);
    paramFwrite(&element->orig_param,       sizeof(double), 1, fp);
    paramFwrite(&element->h,                sizeof(double), 1, fp);
    paramFwrite(&element->h_init,           sizeof(double), 1, fp);
    paramFwrite(&element->deriv_count,      sizeof(int),    1, fp);
    paramFwrite(&element->deriv_state,      sizeof(short),  1, fp);
    paramFwrite(&element->deriv_index,      sizeof(int),    1, fp);
    paramFwrite(element->deriv_h_init,      sizeof(double),
                element->num_params, fp);
    paramFwrite(element->deriv_h_decrease,  sizeof(double),
                element->num_params, fp);
    paramFwrite(element->deriv_h_min,       sizeof(double),
                element->num_params, fp);

    for (i = 0; i < DERIV_NTAB; i++)
    {
        paramFwrite(element->deriv_matrix[i],
                    sizeof(double), DERIV_NTAB, fp);
    }

    paramFwrite(element->deriv,
                sizeof(double), element->num_params, fp);
    paramFwrite(element->g,
                sizeof(double), element->num_params, fp);
    paramFwrite(element->dir,
                sizeof(double), element->num_params, fp);
    paramFwrite(&element->tolerance,
                sizeof(double), 1, fp);

    paramFwrite(&element->linemin_state,          sizeof(short),  1, fp);
    paramFwrite(&element->linemin_init,           sizeof(short),  1, fp);
    paramFwrite(&element->linemin_bracket_state,  sizeof(short),  1, fp);
    paramFwrite(element->linemin_bracket,         sizeof(double), 3, fp);
    paramFwrite(element->linemin_mbracket,        sizeof(double), 3, fp);
    paramFwrite(&element->linemin_brent_state,    sizeof(short),  1, fp);
    paramFwrite(&element->linemin_point,          sizeof(double), 1, fp);
    paramFwrite(&element->linemin_match,          sizeof(double), 1, fp);
    paramFwrite(&element->prev_linemin_match,     sizeof(double), 1, fp);
    paramFwrite(element->linemin_origin,          sizeof(double),
                element->num_params, fp);
    paramFwrite(&element->linemin_tolerance,      sizeof(double), 1, fp);

    paramFwrite(&element->num_params_to_search,   sizeof(int), 1, fp);
    paramFwrite(element->search,  sizeof(short),  element->num_params, fp);
    paramFwrite(element->type,    sizeof(short),  element->num_params, fp);
    paramFwrite(element->center,  sizeof(double), element->num_params, fp);
    paramFwrite(element->range,   sizeof(double), element->num_params, fp);
    paramFwrite(element->min,     sizeof(double), element->num_params, fp);
    paramFwrite(element->max,     sizeof(double), element->num_params, fp);
    paramFwrite(element->current, sizeof(double), element->num_params, fp);
    paramFwrite(element->realcenter, sizeof(double),
                element->num_params, fp);
    paramFwrite(element->realrange,  sizeof(double),
                element->num_params, fp);
    paramFwrite(element->realmin, sizeof(double), element->num_params, fp);
    paramFwrite(element->realmax, sizeof(double), element->num_params, fp);
    paramFwrite(element->realcurrent,
                sizeof(double), element->num_params, fp);
    paramFwrite(element->best,    sizeof(double), element->num_params, fp);

    /*
     * Write out the array of label strings.
     */

    for (i = 0; i < element->num_params; i++)
    {
        paramFwrite(element->label[i], LABEL_SIZE * sizeof(char), 1, fp);

        if (ferror(fp))
        {
            Error();
            printf("paramtableCG: SAVE: error writing to file!\n");
            return 0;
        }
    }

    paramFwrite(&element->current_match,          sizeof(double), 1, fp);
    paramFwrite(&element->best_match,             sizeof(double), 1, fp);
    paramFwrite(&element->new_best_match,         sizeof(short), 1, fp);

    /*
     * Finally, save all the static variables from the functions that use
     * them.  This is a horrible hack, but there doesn't seem to be a nice
     * way to do this.
     */

    (void)do_derivative((struct paramtableCG_type *)NULL,
                        0.0,
                        (double *)NULL,
                        (double *)NULL,
                        (double *)NULL,
                        1,
                        fp);
    (void)do_bracket((struct paramtableCG_type *)NULL, 0.0, 1, fp);
    (void)do_brent((struct paramtableCG_type *)NULL, 0.0, 1, fp);

    fclose(fp);
    return 1;
}




/*
 * ParamtableCG_SAVEBEST
 *
 * FUNCTION
 *     Saves the best parameter table obtained so far to a text file.
 *
 * ARGUMENTS
 *     element  -- address of element
 *     filename -- name of file to save to
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableCG_SAVEBEST(element, filename)
    struct paramtableCG_type *element;
    char *filename;
{
    FILE *fp;
    int i;

    assert(filename != NULL);

    fp = fopen(filename, "w");

    if (fp == NULL)
    {
        Error();
        printf("paramtableCG: SAVEBEST: file cannot be opened for SAVEBEST action!\n");
        return 0;
    }

    fprintf(fp, "Best match so far: %g\n", element->best_match);
    fprintf(fp, "Parameter table data:\n");
    fprintf(fp, "-------------------------------------------------------------\n");
    fprintf(fp, "Iteration number: %d\n", element->iteration_number);
    fprintf(fp, "Number of parameters: %d\n", element->num_params);
    fprintf(fp, "Param\tType\tRange\t\tBest\t\tLabel\n");

    for (i = 0; i < element->num_params; i++)
    {
        fprintf(fp, "%d\t%d\t%g\t\t%-10.6g\t%s\n", i,
                element->type[i], element->range[i],
                element->best[i], element->label[i]);
    }

    fprintf(fp, "Best match so far: %g\n", element->best_match);

    fclose(fp);
    return 1;
}




/*
 * ParamtableCG_RESTORE
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
ParamtableCG_RESTORE(element, filename)
    struct paramtableCG_type *element;
    char *filename;
{
    FILE *fp;
    int   i;

    assert(filename != NULL);

    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        Error();
        printf("paramtableCG: RESTORE: file cannot be opened for RESTORE action!\n");
        return 0;
    }

    /*
     * First delete the old parameter tables.
     */

    ParamtableCG_TABDELETE(element);

    fread(&element->iteration_number, sizeof(int), 1, fp);
    fread(&element->linemin_number,   sizeof(int), 1, fp);
    fread(&element->num_params,       sizeof(int), 1, fp);

    /*
     * Allocate the new parameter tables.
     */

    if (!ParamtableCG_TABCREATE(element, element->num_params))
        return 0;

    fread(&element->state,           sizeof(short),  1, fp);

    fread(&element->deriv_method,    sizeof(short),  1, fp);
    fread(&element->orig_param,      sizeof(double), 1, fp);
    fread(&element->h,               sizeof(double), 1, fp);
    fread(&element->h_init,          sizeof(double), 1, fp);
    fread(&element->deriv_count,     sizeof(int),    1, fp);
    fread(&element->deriv_state,     sizeof(short),  1, fp);
    fread(&element->deriv_index,     sizeof(int),    1, fp);
    fread(element->deriv_h_init,     sizeof(double),
          element->num_params, fp);
    fread(element->deriv_h_decrease, sizeof(double),
          element->num_params, fp);
    fread(element->deriv_h_min,      sizeof(double),
          element->num_params, fp);

    for (i = 0; i < DERIV_NTAB; i++)
    {
        fread(element->deriv_matrix[i],
              sizeof(double), DERIV_NTAB, fp);
    }

    fread(element->deriv,
          sizeof(double), element->num_params, fp);
    fread(element->g,
          sizeof(double), element->num_params, fp);
    fread(element->dir,
          sizeof(double), element->num_params, fp);
    fread(&element->tolerance,
          sizeof(double), 1, fp);

    fread(&element->linemin_state,          sizeof(short),  1, fp);
    fread(&element->linemin_init,           sizeof(short),  1, fp);
    fread(&element->linemin_bracket_state,  sizeof(short),  1, fp);
    fread(element->linemin_bracket,         sizeof(double), 3, fp);
    fread(element->linemin_mbracket,        sizeof(double), 3, fp);
    fread(&element->linemin_brent_state,    sizeof(short),  1, fp);
    fread(&element->linemin_point,          sizeof(double), 1, fp);
    fread(&element->linemin_match,          sizeof(double), 1, fp);
    fread(&element->prev_linemin_match,     sizeof(double), 1, fp);
    fread(element->linemin_origin,
          sizeof(double), element->num_params, fp);
    fread(&element->linemin_tolerance,      sizeof(double), 1, fp);

    fread(&element->num_params_to_search,   sizeof(int), 1, fp);
    fread(element->search,
          sizeof(short), element->num_params, fp);
    fread(element->type,
          sizeof(short), element->num_params, fp);
    fread(element->center,
          sizeof(double), element->num_params, fp);
    fread(element->range,
          sizeof(double), element->num_params, fp);
    fread(element->min,
          sizeof(double), element->num_params, fp);
    fread(element->max,
          sizeof(double), element->num_params, fp);
    fread(element->current,
          sizeof(double), element->num_params, fp);
    fread(element->realcenter,
          sizeof(double), element->num_params, fp);
    fread(element->realrange,
          sizeof(double), element->num_params, fp);
    fread(element->realmin,
          sizeof(double), element->num_params, fp);
    fread(element->realmax,
          sizeof(double), element->num_params, fp);
    fread(element->realcurrent,
          sizeof(double), element->num_params, fp);
    fread(element->best,
          sizeof(double), element->num_params, fp);

    /*
     * Read in the array of label strings.
     */

    for (i = 0; i < element->num_params; i++)
    {
        fread(element->label[i], LABEL_SIZE * sizeof(char), 1, fp);

        if (ferror(fp))
        {
            Error();
            printf("paramtableCG: RESTORE: error writing to file %s!\n",
                   filename);
            return 0;
        }
    }

    fread(&element->current_match,  sizeof(double), 1, fp);
    fread(&element->best_match,     sizeof(double), 1, fp);
    fread(&element->new_best_match, sizeof(short),  1, fp);

    /*
     * Finally, restore all the static variables to the functions that use
     * them.  This is a horrible hack, but there doesn't seem to be a nice
     * way to do this.
     */

    (void)do_derivative((struct  paramtableCG_type *)NULL,
                        0.0,
                        (double *)NULL,
                        (double *)NULL,
                        (double *)NULL,
                        2,
                        fp);
    (void)do_bracket((struct  paramtableCG_type *)NULL, 0.0, 2, fp);
    (void)do_brent((struct  paramtableCG_type *)NULL, 0.0, 2, fp);

    element->alloced = 1;

    fclose(fp);
    return 1;
}




/*
 * ParamtableCG_RESTOREBEST
 *
 * FUNCTION
 *     Restores parameter table from a text file.
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
ParamtableCG_RESTOREBEST(element, filename)
    struct paramtableCG_type *element;
    char *filename;
{
    FILE *fp;
    int   i;

    char  line[MAX_LINELEN];
    char  searchstr[4]; /* holds "YES" or "NO " string   */
    char  typestr[5];   /* holds "add " or "mult" string */

    assert(filename != NULL);

    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        Error();
        printf("paramtableCG: RESTOREBEST: file cannot be opened for RESTOREBEST action!\n");
        return 0;
    }

    /*
     * First delete the old parameter tables.
     */

    ParamtableCG_TABDELETE(element);

    /* Skip the first three lines. */

    for (i = 0; i < 3; i++)
    {
        fgets(line, MAX_LINELEN, fp);
    }


    fgets(line, MAX_LINELEN, fp);
    sscanf(line, "Iteration number: %d", &element->iteration_number);

    fgets(line, MAX_LINELEN, fp);
    sscanf(line, "Number of parameters: %d", &element->num_params);

    /*
     * Allocate the new parameter tables.
     */

    if (!ParamtableCG_TABCREATE(element, element->num_params))
        return 0;

    fgets(line, MAX_LINELEN, fp); /* Skip the next line */

    for (i = 0; i < element->num_params; i++)
    {
        fgets(line, MAX_LINELEN, fp);
        sscanf(line, "%*d %s %s %lg %lg", searchstr, typestr,
               &element->range[i], &element->best[i]);

        if (strcmp(searchstr, "YES") == 0)
        {
            element->search[i] = 1;
        }
        else
        {
            element->search[i] = 0;
        }

        if (strcmp(typestr, "mult") == 0)
        {
            element->type[i] = 1;
        }
        else
        {
            element->type[i] = 0;
        }

        CopyParamLabel(element->label[i], line, 5); /* copy label field */

        /*
         * Copy the best parameter values to the current and center tables.
         */

        element->center[i] = element->current[i] = element->best[i];
    }

    fgets(line, MAX_LINELEN, fp);
    sscanf(line, "Best match so far: %lg", &element->best_match);

    element->alloced = 1;

    fclose(fp);
    return 1;
}




/*
 * ParamtableCG_DISPLAY
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
ParamtableCG_DISPLAY(element)
    struct paramtableCG_type *element;
{
    int i;

    printf("Best match so far: %g\n", element->best_match);
    printf("Parameter table data:\n");
    printf("-----------------------------------------------------------------------------\n");
    printf("Iteration number: %d\n", element->iteration_number);
    printf("Number of parameters: %d\n", element->num_params);
    printf("Param\tType\tRange\t\tCurrent\t\tBest\t\tLabel\n");

    for (i = 0; i < element->num_params; i++)
    {
        printf("%d\t%d\t%g\t\t%-10.6g\t%-10.6g\t%s\n",
               i, element->type[i], element->range[i],
               element->current[i], element->best[i], element->label[i]);
    }

    printf("Best match so far: %g\n", element->best_match);
}




/*
 * ParamtableCG_CHECK
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
ParamtableCG_CHECK(element)
    struct paramtableCG_type *element;
{
    int i;

    if (element->num_params < 0)
    {
        Error();
        printf("paramtableCG: number of parameters must be >= 0!\n");
    }

    for (i = 0; i < element->num_params; i++)
    {
        if ((element->type[i] != 0) &&
            (element->type[i] != 1))
        {
            Error();
            printf("paramtableCG: type field %d must be 0 or 1!\n", i);
        }
    }

    for (i = 0; i < element->num_params; i++)
    {
        if ((element->type[i] == 0) &&
            (element->range[i] < 0.0))
        {
            Error();
            printf("paramtableCG: parameter limits %d are bogus.\n", i);
            printf("range[%d] must be > 0!\n", i);
        }
        else if ((element->type[i] == 1) &&
                 (element->range[i] < 1.0))
        {
            Error();
            printf("paramtableCG: parameter limits %d are bogus.\n", i);
            printf("range[%d] must be >= 1.0!\n", i);
        }
    }

    for (i = 0; i < element->num_params; i++)
    {
        if (element->min[i] > element->max[i])
        {
            Error();
            printf("paramtableCG: min[%d] must be <= max[%d]\n", i, i);
        }
    }
}






/*
 * OBJECT
 *     paramtableCG: a 1-D table for storing a parameter table.
 *                   Also has actions which allow a "brute force"
 *                   search of parameter space along single or
 *                   multiple dimensions.
 *
 *
 * FUNCTION
 *     ParamtableCG
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
ParamtableCG(element, action)
    struct paramtableCG_type *element;
    Action *action;
{
    int     num_params;
    char   *savefilename = NULL, *restorefilename = NULL;
    double  match;

    SELECT_ACTION(action)
    {
    case CREATE:
        element->alloced           = 0;
        element->tolerance         = FTOL;
        element->linemin_tolerance = TOL;
        break;


    case TABCREATE:
        if (action->argc != 1)
        {
            Error();
            printf("paramtableCG: usage : %s num_params\n",
                   "TABCREATE");

            return 0;
        }

        element->iteration_number = element->linemin_number = 0;

        num_params = atoi(action->argv[0]);

        if (!ParamtableCG_TABCREATE(element, num_params))
            return 0;

        break;


    case DELETE:
        /*FALLTHROUGH*/


    case TABDELETE:
        ParamtableCG_TABDELETE(element);
        break;


    case INITSEARCH:
        ParamtableCG_INITSEARCH(element);
        break;


    case RANDOMIZE:
        ParamtableCG_RANDOMIZE(element);
        break;


    case EVALUATE:
        /*
         * Compares the current match with the best match.  If the
         * current match is better than the (previous) best match,
         * the current parameter set becomes the new best parameter
         * set.
         */

        if (action->argc != 1)
        {
            Error();
            printf("paramtableCG: usage: EVALUATE match");

            return 0;
        }
        else
        {
            match = Atod(action->argv[0]);
        }

        element->current_match = match;

        if (match < element->best_match)
        {
            if (IsSilent() < 2)
            {
                printf("Accepting new best match...\n");
            }

            ParamtableCG_ACCEPT(element, match);
            element->new_best_match = 1;
        }
        else
        {
            element->new_best_match = 0;
        }

        break;


    case UPDATE_PARAMS:
        element->iteration_number++;

        if (!ParamtableCG_UPDATE_PARAMS(element))
        {
            return 0;
        }

        break;


    case UPDATE_PARAMS2:
        /*
         * This action is used for pure random searches before
         * running the actual CG search, to get a reasonable
         * starting point.  Therefore it skips the gradient and
         * linemin calculations.
         */

        element->iteration_number++;
        break;


    case LOADBEST:
        ParamtableCG_LOADBEST(element);
        break;


    case RESTART:
        ParamtableCG_RESTART(element);
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
            printf("paramtableCG: SAVE: no filename specified!\n");
            return 0;
        }

        if (!ParamtableCG_SAVE(element, savefilename))
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
            printf("paramtableCG: usage: SAVEBEST filename\n");
            return 0;
        }

        if (!ParamtableCG_SAVEBEST(element, savefilename))
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
            printf("paramtableCG: RESTORE: no filename specified!\n");
            return 0;
        }

        if (!ParamtableCG_RESTORE(element, restorefilename))
            return 0;

        break;


    case RESTOREBEST:
        /*
         * This action restores the best parameter table from a text file.
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
            printf("paramtableCG: usage: RESTOREBEST filename\n");
            return 0;
        }

        if (!ParamtableCG_RESTOREBEST(element, restorefilename))
            return 0;

        break;


    case DISPLAY:
        ParamtableCG_DISPLAY(element);
        break;


    case CHECK:
        ParamtableCG_CHECK(element);
        break;

    default:
        Error();
        printf("paramtableCG: invalid action %s\n", action->name);
        return 0;
    }

    return 1;
}





/*
 * The following is a utility function to put parameters into the tables
 * with less fuss.
 */

/*
 * do_initparamCG
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
do_initparamCG(argc, argv)
    int argc;
    char **argv;
{
    int      param;
    char    *path;
    struct paramtableCG_type *element;
    short    type;
    double   range, center;
    char    *label;

    initopt(argc, argv, "path param type range center label");

    if (argc != 7)
    {
        printoptusage(argc, argv);

        return 0;
    }

    path    = optargv[1];
    param   = atoi(optargv[2]);
    type    = (short) atoi(optargv[3]);
    range   = Atod(optargv[4]);
    center  = Atod(optargv[5]);
    label   = optargv[6];

    element = (struct paramtableCG_type *)GetElement(path);

    /* error checks */

    if (!CheckClass(element, ClassID("param")))
    {
        Error();
        printf("initparamCG: element %s is not a parameter table object!\n", element->name);
        return 0;
    }

    if ((param < 0) || (param >= element->num_params))
    {
        Error();
        printf("initparamCG: parameter index is out of range!\n");
        return 0;
    }

    /* FIXME: this is redundant; use a switch statement. */

    if ((type != 0) && (type != 1))
    {
        Error();
        printf("initparamCG: type field must be 0 or 1!\n");
        return 0;
    }

    if (type == 0) /* additive parameter */
    {
        if (range < 0.0)
        {
            Error();
            printf("initparamCG: additive parameter range must be > 0!\n");
            return 0;
        }
    }

    if (type == 1) /* multiplicative parameter */
    {
        if (range < 1.0)
        {
            Error();
            printf("initparamCG: multiplicative parameter range must be > 1.0!\n");
            return 0;
        }
    }

    element->type[param]    = type;
    element->range[param]   = range;
    element->center[param]  = center;

    strncpy(element->label[param], label, LABEL_SIZE);

    /*
     * We store the min and max values of the parameter as well as the
     * range, so that we can prevent the parameter from going beyond
     * initially established limits.  We use realmin, realmax etc. to turn
     * multiplicative parameters into additive ones so that all parameters
     * are treated the same way.
     */

    if (type == 0) /* additive parameter */
    {
        element->min[param]        = center - range;
        element->max[param]        = center + range;
        element->realcenter[param] = center;
        element->realrange[param]  = range;
        element->realmin[param]    = element->min[param];
        element->realmax[param]    = element->max[param];
    }
    else /* multiplicative parameter */
    {
        element->min[param]        = center / range;
        element->max[param]        = center * range;
        element->realcenter[param] = log(center);
        element->realrange[param]  = log(range);
        element->realmin[param]    = element->realcenter[param]
            - element->realrange[param];
        element->realmax[param]    = element->realcenter[param]
            + element->realrange[param];
    }

    /*
     * Set the default value of deriv_h_init based on the type of the
     * parameter.  These can be changed by the user if necessary.
     */

    if (type == 0) /* additive parameter */
    {
        element->deriv_h_init[param] = DERIV_INIT_H_ADD;
    }
    else /* multiplicative parameter */
    {
        element->deriv_h_init[param] = DERIV_INIT_H_MULT;
    }

    /*
     * Set the default value of deriv_h_decrease.
     * This can be changed by the user if necessary.
     */

    element->deriv_h_decrease[param] = DERIV_DECR;

    /*
     * Set the default value of deriv_h_min based on the type of the
     * parameter.  These can be changed by the user if necessary.
     */

    if (type == 0) /* additive parameter */
    {
        element->deriv_h_min[param] = DERIV_MIN_H_ADD;
    }
    else /* multiplicative parameter */
    {
        element->deriv_h_min[param] = DERIV_MIN_H_MULT;
    }

    /*
     * Set the param_to_search field to 1 unless the range is effectively 0
     * (for additive parameters) or 1 (for multiplicative parameters).
     */

    if (((type == 0) && (range < TINY))
        || ((type == 1) && ((range - 1.0) < TINY)))
    {
        element->search[param] = 0;
    }
    else
    {
        element->search[param] = 1;
    }

    return 1;
}




/*
 * SetSearchCG
 *
 * FUNCTION
 *     Sets the search flags for the paramtableCG object.
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
SetSearchCG(argc, argv, all, not, add)
    int argc;
    char **argv;
    short all;
    short not;
    short add;
{
    int    i, param, tosearch;
    struct paramtableCG_type *element;

    element = (struct paramtableCG_type*)GetElement(argv[1]);

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
     * just for bookkeeping purposes.
     */

    tosearch = 0;

    for (i = 0; i < element->num_params; i++)
    {
        if (element->search[i] == 1)
            ++tosearch;
    }

    element->num_params_to_search = tosearch;

    return 1;
}
