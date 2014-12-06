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

/*
 * The following functions are (mostly) action functions for paramtableSS,
 * made into separate functions for convenience in programming and
 * modification.
 */

/*
 * ParamtableSS_TABCREATE
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
ParamtableSS_TABCREATE(element, num_params)
    struct paramtableSS_type *element;
    int num_params;
{
    int i;

    if (num_params < 0)
    {
        Error();
        printf("paramtableSS: TABCREATE: number of parameters must be >= 0!\n");
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

    element->search  = (short *) calloc(num_params, sizeof(short));
    element->type    = (short *) calloc(num_params, sizeof(short));
    element->range   = (double *)calloc(num_params, sizeof(double));
    element->min     = (double *)calloc(num_params, sizeof(double));
    element->max     = (double *)calloc(num_params, sizeof(double));
    element->current = (double *)calloc(num_params, sizeof(double));
    element->best    = (double *)calloc(num_params, sizeof(double));

    /*
     * Create array of label strings for parameters.  Allocate an extra
     * character for the terminating null character of the string.
     */

    element->label   = (char **)  calloc(num_params, sizeof(char *));

    for (i = 0; i < num_params; i++)
    {
        element->label[i] = (char *) calloc((LABEL_SIZE + 1), sizeof(char));
    }

    element->alloced = 1;

    return 1;
}




/*
 * ParamtableSS_TABDELETE
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
ParamtableSS_TABDELETE(element)
    struct paramtableSS_type *element;
{
    int i;

    if (element->alloced == 0)
    {
        return;
    }

    paramFree(element->search);
    paramFree(element->type);
    paramFree(element->range);
    paramFree(element->min);
    paramFree(element->max);
    paramFree(element->current);
    paramFree(element->best);

    for (i = 0; i < element->num_params; i++)
    {
        paramFree(element->label[i]);
    }

    paramFree(element->label);
    element->alloced = 0;
}




/*
 * ParamtableSS_RANDOMIZE
 *
 * FUNCTION
 *     Randomizes parameters in tables; uses a uniform distribution from
 *     (best - range, best + range)
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
ParamtableSS_RANDOMIZE(element)
    struct paramtableSS_type *element;
{
    int    i;
    double param, center, range, min, max;

    /* Error checks. */

    if (!element->alloced)
    {
        Error();
        printf("paramtableSS: RANDOMIZE: tables must be allocated before randomization.\n");
        return 0;
    }

    for (i = 0; i < element->num_params; i++)
    {
        if ((element->range[i] < 0.0) &&
            (element->type[i] == 0))
        {
            Error();
            printf("paramtableSS: RANDOMIZE: parameter limits %d are bogus.\n", i);
            printf("range[%d] must be >= 0!\n", i);
            return 0;
        }
        else if ((element->range[i] < 1.0) &&
                 (element->type[i] == 1))
        {
            Error();
            printf("paramtableSS: RANDOMIZE: parameter limits %d are bogus.\n", i);
            printf("range[%d] must be >= 1.0!\n", i);
            return 0;
        }
    }

    for (i = 0; i < element->num_params; i++)
    {
        if (element->search[i] == 0) /* do not vary this parameter */
        {
            continue;
        }

        if (element->type[i] == 0)  /* additive parameter */
        {
            center = element->best[i];
            min    = element->min[i];
            max    = element->max[i];
            param  = frandom(min, max);
            element->current[i] = param;
        }
        else /* multiplicative parameter */
        {
            /*
             * This procedure makes numbers between 1 and range as
             * likely as numbers between 1/range and 1.
             */

            center = element->best[i];
            range  = element->range[i];
            param  = center * exp(frandom(-log(range), log(range)));

            if (param < element->min[i])
            {
                param = element->min[i];
            }

            if (param > element->max[i])
            {
                param = element->max[i];
            }

            element->current[i] = param;
        }
    }

    return 1;
}




/*
 * ParamtableSS_RANDGAUSSIAN
 *
 * FUNCTION
 *     Randomizes parameters in tables; uses a Gaussian distribution from
 *     (best +/- range), where range is the standard deviation.
 *     This is used to calculate the next parameter table.
 *     NOTE: This is not a user-callable action; it is called by the
 *     UPDATE_PARAMS action.
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
ParamtableSS_RANDGAUSSIAN(element)
    struct paramtableSS_type *element;
{
    int    i;
    double param, center, range, min, max, var, addscale, multscale;

    /* Error checks. */

    if (!element->alloced)
    {
        Error();
        printf("paramtableSS: RANDGAUSSIAN: tables must be allocated before randomization.\n");
        return 0;
    }

    if (element->maxvariance < TINY)
    {
        Error();
        printf("paramtableSS: RANDGAUSSIAN: maximum variance must be > 0!\n");
        return 0;
    }


    for (i = 0; i < element->num_params; i++)
    {
        if ((element->range[i] < 0.0)
            && (element->type[i] == 0))
        {
            Error();
            printf("paramtableSS: RANDGAUSSIAN: parameter limits %d are bogus.\n", i);
            printf("range[%d] must be >= 0!\n", i);
            return 0;
        }
        else if ((element->range[i] < 1.0)
                 && (element->type[i] == 1))
        {
            Error();
            printf("paramtableSS: RANDGAUSSIAN: parameter limits %d are bogus.\n", i);
            printf("range[%d] must be >= 1.0!\n", i);
            return 0;
        }
    }

    for (i = 0; i < element->num_params; i++)
    {
        if (element->search[i] == 0) /* do not vary this parameter */
        {
            continue;
        }

        center    = element->best[i];
        range     = element->range[i];
        min       = element->min[i];
        max       = element->max[i];
        var       = element->variance;
        addscale  = element->addvarscale;
        multscale = element->multvarscale;

        if (element->type[i] == 0)  /* additive parameter */
        {
            /*
             * The parameter is set to equal a gaussian-distributed
             * random value centered at best[i] and with the given variance;
             * range[i] sets the limits beyond which the parameter is
             * not allowed to extend.  For maximum variance the "maximum"
             * change is about equal to range.  If you want a narrower
             * or wider range use a value of addvarscale != 1 (which is
             * the default).
             */

            param  = center + range * rangauss(0.0, (var * addscale));
        }
        else /* multiplicative parameter */
        {
            /*
             * Change parameter to be about in the range (1/range, range)
             * for maximum variance.  If you want a narrower or wider range
             * use a multvarscale != 1 (which is the default), although a
             * wider range will give values that will be truncated to the
             * limits much of the time.
             */

            param = center * pow(range, rangauss(0.0, (var * multscale)));
        }

        if (param < min)
        {
            param = min;
        }
        else if (param > max)
        {
            param = max;
        }

        element->current[i] = param;
    }

    return 1;
}




/*
 * ParamtableSS_SAVE
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
ParamtableSS_SAVE(element, filename)
    struct paramtableSS_type *element;
    char *filename;
{
    FILE *fp;
    int   i;

    assert(filename != NULL);

    fp = fopen(filename, "w");

    if (fp == NULL)
    {
        Error();
        printf("paramtableSS: SAVE: file cannot be opened for SAVE action!\n");
        return 0;
    }

    paramFwrite(&element->iteration_number,
                sizeof(int), 1, fp);
    paramFwrite(&element->round_number,
                sizeof(int), 1, fp);
    paramFwrite(&element->num_params,
                sizeof(int), 1, fp);
    paramFwrite(element->search,
                sizeof(short), element->num_params, fp);
    paramFwrite(element->type,
                sizeof(short), element->num_params, fp);
    paramFwrite(element->range,
                sizeof(double), element->num_params, fp);
    paramFwrite(element->min,
                sizeof(double), element->num_params, fp);
    paramFwrite(element->max,
                sizeof(double), element->num_params, fp);
    paramFwrite(element->current,
                sizeof(double), element->num_params, fp);
    paramFwrite(element->best,
                sizeof(double), element->num_params, fp);

    /*
     * Write out the array of label strings.
     */

    for (i = 0; i < element->num_params; i++)
    {
        paramFwrite(element->label[i], LABEL_SIZE * sizeof(char), 1, fp);

        if (ferror(fp))
        {
            Error();
            printf("paramtableSS: SAVE: error writing to file!\n");
            return 0;
        }
    }

    paramFwrite(&element->best_match,   sizeof(double), 1, fp);
    paramFwrite(&element->variance,     sizeof(double), 1, fp);
    paramFwrite(&element->minvariance,  sizeof(double), 1, fp);
    paramFwrite(&element->maxvariance,  sizeof(double), 1, fp);
    paramFwrite(&element->addvarscale,  sizeof(double), 1, fp);
    paramFwrite(&element->multvarscale, sizeof(double), 1, fp);
    paramFwrite(&element->contract,     sizeof(double), 1, fp);

    fclose(fp);
    return 1;
}




/*
 * ParamtableSS_SAVEBEST
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
ParamtableSS_SAVEBEST(element, filename)
    struct paramtableSS_type *element;
    char *filename;
{
    FILE *fp;
    int i;

    assert(filename != NULL);

    fp = fopen(filename, "w");

    if (fp == NULL)
    {
        Error();
        printf("paramtableSS: SAVEBEST: file cannot be opened for SAVEBEST action!\n");
        return 0;
    }

    fprintf(fp, "Best match so far: %g\n",    element->best_match);
    fprintf(fp, "Parameter table data:\n");
    fprintf(fp, "-----------------------------------------------------------------------\n");
    fprintf(fp, "Iteration number: %d\n",     element->iteration_number);
    fprintf(fp, "Round number: %d\n",         element->round_number);
    fprintf(fp, "Number of parameters: %d\n", element->num_params);

    fprintf(fp, "\n");
    fprintf(fp, "Param   Varied?    Type    Range      Best             Label\n");
    fprintf(fp, "-----   -------    ----    -----      ----             -----\n");
    fprintf(fp, "\n");

    for (i = 0; i < element->num_params; i++)
    {
        fprintf(fp, "%-5d   ", i); /* parameter number */

        if (element->search[i] == 0)
        {
            fprintf(fp, "%s        ", "NO ");
        }
        else
        {
            fprintf(fp, "%s        ", "YES");
        }

        if (element->type[i] == 0)
        {
            fprintf(fp, "%s    ", "add ");
        }
        else
        {
            fprintf(fp, "%s    ", "mult");
        }

        fprintf(fp, "%-8g   %-15.6g  %s\n",
                element->range[i], element->best[i], element->label[i]);
    }

    fprintf(fp, "\n");

    fprintf(fp, "Variance: %g\n", element->variance);
    fprintf(fp, "Minimum variance: %g\n", element->minvariance);
    fprintf(fp, "Maximum variance: %g\n", element->maxvariance);
    fprintf(fp, "Scaling factor for additive parameter variances: %g\n",
            element->addvarscale);
    fprintf(fp,
            "Scaling factor for multiplicative parameter variances: %g\n",
            element->multvarscale);
    fprintf(fp, "Rate of variance contraction: %g\n", element->contract);
    fprintf(fp, "Best match so far: %g\n", element->best_match);

    fclose(fp);
    return 1;
}




/*
 * ParamtableSS_RESTORE
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
ParamtableSS_RESTORE(element, filename)
    struct paramtableSS_type *element;
    char *filename;
{
    FILE *fp;
    int   i;

    assert(filename != NULL);

    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        Error();
        printf("paramtableSS: RESTORE: file cannot be opened for RESTORE action!\n");
        return 0;
    }

    /*
     * First delete the old parameter tables.
     */

    ParamtableSS_TABDELETE(element);

    fread(&element->iteration_number, sizeof(int), 1, fp);
    fread(&element->round_number,     sizeof(int), 1, fp);
    fread(&element->num_params,       sizeof(int), 1, fp);

    /*
     * Allocate the new parameter tables.
     */

    if (!ParamtableSS_TABCREATE(element, element->num_params))
        return 0;

    fread(element->search,  sizeof(short),  element->num_params, fp);
    fread(element->type,    sizeof(short),  element->num_params, fp);
    fread(element->range,   sizeof(double), element->num_params, fp);
    fread(element->min,     sizeof(double), element->num_params, fp);
    fread(element->max,     sizeof(double), element->num_params, fp);
    fread(element->current, sizeof(double), element->num_params, fp);
    fread(element->best,    sizeof(double), element->num_params, fp);


    /*
     * Read in the array of label strings.
     */

    for (i = 0; i < element->num_params; i++)
    {
        fread(element->label[i], LABEL_SIZE * sizeof(char), 1, fp);

        if (ferror(fp))
        {
            Error();
            printf("paramtableSS: RESTORE: error writing to file!\n");
            return 0;
        }
    }

    fread(&element->best_match,   sizeof(double), 1, fp);
    fread(&element->variance,     sizeof(double), 1, fp);
    fread(&element->minvariance,  sizeof(double), 1, fp);
    fread(&element->maxvariance,  sizeof(double), 1, fp);
    fread(&element->addvarscale,  sizeof(double), 1, fp);
    fread(&element->multvarscale, sizeof(double), 1, fp);
    fread(&element->contract,     sizeof(double), 1, fp);

    element->alloced = 1;

    fclose(fp);
    return 1;
}

/* FIXME: should have a RESTOREBEST action too. */



/*
 * ParamtableSS_DISPLAY
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
ParamtableSS_DISPLAY(element)
    struct paramtableSS_type *element;
{
    int i;

    printf("Best match so far: %g\n",    element->best_match);
    printf("Parameter table data:\n");
    printf("-----------------------------------------------------------------------\n");
    printf("Iteration number: %d\n",     element->iteration_number);
    printf("Round number: %d\n",         element->round_number);
    printf("Number of parameters: %d\n", element->num_params);

    printf("\n");
    printf("Param   Varied?    Type    Range      Best             Label\n");
    printf("-----   -------    ----    -----      ----             -----\n");
    printf("\n");

    for (i = 0; i < element->num_params; i++)
    {
        printf("%-5d   ", i); /* parameter number */

        if (element->search[i] == 0)
        {
            printf("%s        ", "NO ");
        }
        else
        {
            printf("%s        ", "YES");
        }

        if (element->type[i] == 0)
        {
            printf("%s    ", "add ");
        }
        else
        {
            printf("%s    ", "mult");
        }

        printf("%-8g   %-15.6g  %s\n",
                element->range[i], element->best[i], element->label[i]);
    }

    printf("\n");

    printf("Variance: %g\n", element->variance);
    printf("Minimum variance: %g\n", element->minvariance);
    printf("Maximum variance: %g\n", element->maxvariance);
    printf("Scaling factor for additive parameter variances: %g\n",
            element->addvarscale);
    printf("Scaling factor for multiplicative parameter variances: %g\n",
           element->multvarscale);
    printf("Rate of variance contraction: %g\n", element->contract);
    printf("Best match so far: %g\n", element->best_match);
    printf("\n");
}




/*
 * ParamtableSS_CHECK
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
ParamtableSS_CHECK(element)
    struct paramtableSS_type *element;
{
    int i;

    if (element->num_params < 0)
    {
        Error();
        printf("paramtableSS: CHECK: number of parameters must be >= 0!\n");
    }

    for (i = 0; i < element->num_params; i++)
    {
        if ((element->type[i] != 0) &&
            (element->type[i] != 1))
        {
            Error();
            printf("paramtableSS: CHECK: type field %d must be 0 or 1!\n", i);
        }
    }

    for (i = 0; i < element->num_params; i++)
    {
        if ((element->type[i] == 0) &&
            (element->range[i] < 0.0))
        {
            Error();
            printf("paramtableSS: CHECK: parameter limits %d are bogus.\n", i);
            printf("range[%d] must be > 0!\n", i);
        }
        else if ((element->type[i] == 1) &&
                 (element->range[i] < 1.0))
        {
            Error();
            printf("paramtableSS: CHECK: parameter limits %d are bogus.\n", i);
            printf("range[%d] must be >= 1.0!\n", i);
        }
    }

    if (element->minvariance >= element->maxvariance)
    {
        Error();
        printf("paramtableSS: CHECK: variance limits are incorrect.\n");
    }

    if ((element->minvariance  <= TINY) ||
        (element->maxvariance  <= TINY) ||
        (element->variance     <= TINY) ||
        (element->addvarscale  <= TINY) ||
        (element->multvarscale <= TINY))
    {
        Error();
        printf("paramtableSS: CHECK: variance, variance limits and scaling factors must be > 0!\n");
    }
}








/*
 * OBJECT
 *     paramtableSS: a 1-D table for storing a parameter table.
 *                   Also has actions which allow new parameters
 *                   to be generated using a stochastic search
 *                   algorithm.
 *
 * FUNCTION
 *     ParamtableSS
 *
 * ARGUMENTS
 *     element   -- address of element
 *     action    -- address of action
 *
 * RETURN VALUE
 *     int -- 0 = failure, 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableSS(element, action)
    struct paramtableSS_type *element;
    Action *action;
{
    int   i, num_params;
    char *savefilename, *restorefilename;

    SELECT_ACTION(action)
    {
    case CREATE:
        element->alloced = 0;
    element->best_match = HUGE;
        break;


    case TABCREATE:
        if (action->argc != 1)
        {
            Error();
            printf("paramtableSS: TABCREATE: usage: %s num_params\n", "TABCREATE");
            return 0;
        }

        num_params = atoi(action->argv[0]);

        if (!ParamtableSS_TABCREATE(element, num_params))
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
        ParamtableSS_TABDELETE(element);
        break;


    case INITSEARCH:
        if (!element->alloced)
        {
            Error();
            printf("paramtableSS: INITSEARCH: must create tables with TABCREATE\n");
            printf("                          before calling this action!\n");
            return 0;
        }

        element->iteration_number = 0;
        element->round_number     = 0;

        /*
         * maxvariance must be set BEFORE calling INITSEARCH so that
         * variance can be set equal to it.
         */

        if (element->maxvariance <= TINY)
        {
            Error();
            printf("paramtableSS: INITSEARCH: maxvariance must be > 0!\n");
        }

        element->variance = element->maxvariance;

        break;


    case RANDOMIZE:
        if (!ParamtableSS_RANDOMIZE(element))
            return 0;

        break;


    case UPDATE_PARAMS:
        element->iteration_number++;

        /*
         * Since one can change minvariance and maxvariance between
         * updates we do range-checking here.
         */

        if (element->variance > element->maxvariance)
        {
            element->variance = element->maxvariance;
        }
        else if (element->variance < element->minvariance)
        {
            element->variance = element->minvariance;
        }
        else
        {
            /* contract variance */

            element->variance *= element->contract;

            if (element->variance < element->minvariance)
            {
                element->round_number++;
                element->variance = element->maxvariance;
            }
        }

        if (!ParamtableSS_RANDGAUSSIAN(element))
        {
            return 0;
        }

        break;


    case ACCEPT:
        /*
         * When a new best point is found copy the current parameters
         * to the best array and the current match (which is an
         * argument to this action) to the best_match field.
         */

        if (action->argc != 1)
        {
            Error();
            printf("paramtableSS: ACCEPT: usage: %s match-value\n", "ACCEPT");
            return 0;
        }

        element->best_match = Atod(action->argv[0]);

        for (i = 0; i < element->num_params; i++)
            element->best[i] = element->current[i];

        break;


    case RECENTER:
        /*
         * Recalculate the min and max values according to the best and
         * range values.
         */

        for (i = 0; i < element->num_params; i++)
        {
            if (element->search[i] == 0)
            {
                /* do not vary the parameter's range */
                continue;
            }

            if (element->type[i] == 0) /* additive parameter */
            {
                element->min[i] = element->best[i] - element->range[i];
                element->max[i] = element->best[i] + element->range[i];
            }
            else /* multiplicative parameter */
            {
                element->min[i] = element->best[i] / element->range[i];
                element->max[i] = element->best[i] * element->range[i];
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
            printf("paramtableSS: SAVE: no filename specified!\n");
            return 0;
        }

        if (!ParamtableSS_SAVE(element, savefilename))
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
            printf("paramtableSS: usage: SAVEBEST filename\n");
            return 0;
        }

        if (!ParamtableSS_SAVEBEST(element, savefilename))
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
            printf("paramtableSS: RESTORE: no filename specified!\n");
            return 0;
        }

        if (!ParamtableSS_RESTORE(element, restorefilename))
            return 0;

        break;


    case DISPLAY:
        ParamtableSS_DISPLAY(element);
        break;


    case CHECK:
        ParamtableSS_CHECK(element);
        break;


    default:
        Error();
        printf("paramtableSS: invalid action %s\n", action->name);
        return 0;
    }

    return 1;
}




/*
 * The following is a utility function to put parameters into the table
 * with less fuss.
 */

/*
 * do_initparamSS
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
do_initparamSS(argc, argv)
    int argc;
    char **argv;
{
    int     param;
    char   *path;
    struct  paramtableSS_type *element;
    short   type;
    double  range;
    double  center;
    char   *label;

    initopt(argc, argv, "path param type range center label");

    if (argc != 7)
    {
        printoptusage(argc, argv);

        return 0;
    }

    path    = optargv[1];
    param   = atoi(optargv[2]);
    type    = (short)atoi(optargv[3]);
    range   = Atod(optargv[4]);
    center  = Atod(optargv[5]);
    label   = optargv[6];

    element = (struct paramtableSS_type *) GetElement(path);

    /* error checks */

    if (!CheckClass(element, ClassID("param")))
    {
        Error();
        printf("initparamSS: Element %s is not a parameter table object!\n", element->name);
        return 0;
    }

    if ((param < 0) || (param >= element->num_params))
    {
        Error();
        printf("initparamSS: parameter index is out of range!\n");
        return 0;
    }

    /* FIXME: this is redundant; use a switch statement. */

    if ((type != 0) && (type != 1))
    {
        Error();
        printf("initparamSS: type field must be 0 or 1!\n");
        return 0;
    }

    if (type == 0) /* additive parameter */
    {
        if (range < 0.0)
        {
            Error();
            printf("initparamSS: additive parameter range must be > 0!\n");
            return 0;
        }
    }

    if (type == 1) /* multiplicative parameter */
    {
        if (range < 1.0)
        {
            Error();
            printf("initparamSS: multiplicative parameter range must be > 1.0!\n");
            return 0;
        }

    }

    element->type[param]    = type;
    element->range[param]   = range;
    element->current[param] = element->best[param] = center;

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

    return 1;
}




/*
 * SetSearchSS
 *
 * FUNCTION
 *     Sets the search flags for the paramtableSS object.
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
SetSearchSS(argc, argv, all, not, add)
    int argc;
    char **argv;
    short all;
    short not;
    short add;
{
    int    i, param;
    struct paramtableSS_type *element;

    element = (struct paramtableSS_type*) GetElement(argv[1]);

    if (all)
    {
        for (i = 0; i < element->num_params; i++)
        {
            element->search[i] = 1;
        }

        return 1;
    }
    else if (not)
    {
        for (i = 0; i < element->num_params; i++)
        {
            element->search[i] = 1;
        }
    }
    else if (!add)
    {
        for (i = 0; i < element->num_params; i++)
        {
            element->search[i] = 0;
        }
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

    return 1;
}


