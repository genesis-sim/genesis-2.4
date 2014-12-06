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
 * The following functions are action functions for paramtableBF,
 * made into separate functions for convenience in programming and
 * modification.
 */

/*
 * ParamtableBF_TABCREATE
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
ParamtableBF_TABCREATE(element, num_params)
    struct paramtableBF_type *element;
    int num_params;
{
    int i;

    if (num_params < 0)
    {
        Error();
        printf("paramtableBF: TABCREATE: number of parameters must be >= 0!\n");
        return 0;
    }
    else element->num_params = num_params;

    /*
     * The following are for keeping track of which parameters are being
     * swept through and where we are in the search.  search_rangemod is
     * the fraction of the full range to search over.  All ranges are
     * relative to the center values of the parameters.
     */

    element->search           = (short *)  calloc(element->num_params,
                                                  sizeof(short));
    element->search_divisions = (int *)    calloc(element->num_params,
                                                  sizeof(int));
    element->search_rangemod  = (double *) calloc(element->num_params,
                                                  sizeof(double));
    element->search_count     = (int *)    calloc(element->num_params,
                                                  sizeof(int));
    element->search_range     = (double *) calloc(element->num_params,
                                                  sizeof(double));

    if (IsSilent() < 1)
    {
        printf("Creating parameter table with %d parameters.\n",
               element->num_params);
    }

    element->type    = (short  *) calloc(element->num_params,
                                         sizeof(short));
    element->range   = (double *) calloc(element->num_params,
                                         sizeof(double));
    element->min     = (double *) calloc(element->num_params,
                                         sizeof(double));
    element->max     = (double *) calloc(element->num_params,
                                         sizeof(double));
    element->orig    = (double *) calloc(element->num_params,
                                         sizeof(double));
    element->current = (double *) calloc(element->num_params,
                                         sizeof(double));
    element->best    = (double *) calloc(element->num_params,
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
 * ParamtableBF_TABDELETE
 *
 * FUNCTION
 *     Deallocates memory for parameter tables.
 *
 * ARGUMENTS
 *     element   -- address of element
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
ParamtableBF_TABDELETE(element)
    struct paramtableBF_type *element;
{
    int i;

    if (element->alloced == 0)
        return;

    paramFree(element->search);
    paramFree(element->search_divisions);
    paramFree(element->search_rangemod);
    paramFree(element->search_count);
    paramFree(element->search_range);
    paramFree(element->type);
    paramFree(element->range);
    paramFree(element->min);
    paramFree(element->max);
    paramFree(element->orig);
    paramFree(element->current);
    paramFree(element->best);

    for (i = 0; i < element->num_params; i++)
        paramFree(element->label[i]);

    paramFree(element->label);
    element->alloced = 0;
}




/*
 * ParamtableBF_INITSEARCH
 *
 * FUNCTION
 *     Initializes searchs through a parameter or a group of parameters.
 *
 * ARGUMENTS
 *     element   -- address of element
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableBF_INITSEARCH(element, argc, argv)
    struct paramtableBF_type *element;
    int argc;
    char **argv;
{
    /*
     * Usage: ParamtableBF_INITSEARCH <param-number1> <npts1> <scale1> ...
     *   <param-numbern> <nptsn> <scalen>
     *
     * This function expects its arguments to be in the form of
     * <param_number> <npts> <scale> triplets, where
     *   <param_number> is the index of the parameter to be varied
     *   <npts> is the number of equally-spaced points along the parameter
     *     axis to test
     *   <scale> is the proportion of the total range to use;
     *     1.0 means use the entire range.
     *
     * Note that the range is relative to a center point which is where the
     * parameter search starts from.
     */

    int    i, ntriplets;
    int    param, npts;
    double scale;

    /*
     * Check the argument list.
     */

    if ((argc % 3) != 0 || argc == 0)
    {
        Error();
        printf("paramtableBF: INITSEARCH: argument list must be in the form of\n");
        printf("  <param_number> <npts> <scale> triplets!\n");
        return 0;
    }

    ntriplets = argc / 3;

    if (ntriplets > element->num_params)
    {
        Error();
        printf("paramtableBF: INITSEARCH: number of triplets must be >= number of parameters!\n");
        return 0;
    }

    /* Don't search through any parameters unless instructed to below. */

    for (i = 0; i < element->num_params; i++)
        element->search[i] = 0;

    /*
     * Record the arguments in the object.
     */

    for (i = 0; i < ntriplets; i++)
    {
        param = atoi(argv[3 * i]);
        npts  = atoi(argv[3 * i + 1]);
        scale = Atod(argv[3 * i + 2]);

        element->search[param] = 1;
        element->search_divisions[param] = npts;
        element->search_rangemod[param]  = scale;
    }

    /*
     * Calculate the actual ranges to search over, as a fraction of the
     * full range.  Note that search_range is the distance from the
     * center of the range to the end of the range to be swept
     * through, and not the full end-to-end range.
     */

    for (i = 0; i < element->num_params; i++)
    {
        if (element->type[i] == 0) /* additive */
        {
            element->search_range[i] = element->range[i]
                * element->search_rangemod[i];
        }
        else /* type = 1; i.e. multiplicative */
        {
            element->search_range[i] = pow(element->range[i],
                                           element->search_rangemod[i]);
        }
    }

    /*
     * Copy the "original" parameters to the best and current
     * parameter tables.
     */

    for (i = 0; i < element->num_params; i++)
        element->current[i] = element->best[i] = element->orig[i];

    /*
     * Set all variable parameters to the value at the start of their
     * ranges.
     */

    for (i = 0; i < element->num_params; i++)
    {
        if (element->search[i] == 0)  /* not swept through */
            continue;

        if (element->type[i] == 0) /* additive */
            element->current[i] -= element->search_range[i];
        else /* multiplicative */
            element->current[i] /= element->search_range[i];
    }

    /*
     * Initialize the counts.
     */

    for (i = 0; i < element->num_params; i++)
        element->search_count[i] = 0;

    /* Miscellaneous initializations. */

    element->iteration_number = 0;
    element->done = 0;
    element->best_match = HUGE;

    return 1;
}




/*
 * ParamtableBF_UPDATE_PARAMS
 *
 * FUNCTION
 *     Updates the current parameter table.  Also updates the
 *     search_count fields so we know where we are in the parameter search.
 *
 * ARGUMENTS
 *     element   -- address of element
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableBF_UPDATE_PARAMS(element)
    struct paramtableBF_type *element;
{
    int    i;
    double width;
    int    done = 0;

    /*
     * Change the value of the first variable parameter.  If it's at
     * the end of its range then put it back to the beginning of its
     * range and change the value of the next variable parameter and
     * so on.
     */

    for (i = 0; i < element->num_params; i++)
    {
        if (done)
            break;

        if (element->search[i] == 0)
            continue; /* don't search this parameter */

        if (++element->search_count[i] < element->search_divisions[i])
        {
            if (element->type[i] == 0) /* additive */
            {
                width = 2.0 * element->search_range[i]
                    / ((double) (element->search_divisions[i] - 1));

                element->current[i]
                    = (element->orig[i] - element->search_range[i])
                    + element->search_count[i] * width;
            }
            else /* type = 1 i.e. multiplicative */
            {
                width = 2.0 * log(element->search_range[i])
                    / ((double) (element->search_divisions[i] - 1));

                element->current[i]
                    = exp(log(element->orig[i])
                          - log(element->search_range[i])
                          + element->search_count[i] * width);
            }

            done = 1;
        }
        else /* we've reached the limit of this parameter's range */
        {
            /* Reset the parameter to the beginning of its range. */

            element->search_count[i] = 0;

            if (element->type[i] == 0) /* additive */
            {
                element->current[i]
                    = element->orig[i] - element->search_range[i];
            }
            else /* type = 1 i.e. multiplicative */
            {
                element->current[i]
                    = element->orig[i] / element->search_range[i];
            }
        }
    }

    /*
     * If any parameters have been incremented then "done" will equal
     * one. If the last parameter to be swept has reached the end of its
     * range, there are no more parameters to search and so "done" will
     * still equal zero.  This means that the search is over.  At the end
     * of the search all the variable parameters will have the value at the
     * beginning of their ranges.
     */

    if (!done)
        element->done = 1;

    return 1;
}




/*
 * ParamtableBF_SAVE
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
ParamtableBF_SAVE(element, filename)
    struct paramtableBF_type *element;
    char *filename;
{
    FILE *fp;
    int   i;

    assert(filename != NULL);

    fp = fopen(filename, "w");

    if (fp == NULL)
    {
        Error();
        printf("paramtableBF: SAVE: file cannot be opened for SAVE action!\n");
        return 0;
    }

    paramFwrite(&element->iteration_number,
                sizeof(int),    1, fp);
    paramFwrite(&element->num_params,
                sizeof(int),    1, fp);
    paramFwrite(&element->num_params_to_search,
                sizeof(int),    1, fp);
    paramFwrite(element->search,
                sizeof(short),  element->num_params, fp);
    paramFwrite(element->type,
                sizeof(short),  element->num_params, fp);
    paramFwrite(element->range,
                sizeof(double), element->num_params, fp);
    paramFwrite(element->min,
                sizeof(double), element->num_params, fp);
    paramFwrite(element->max,
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
            printf("paramtableBF: SAVE: error writing to file!\n");
            return 0;
        }
    }

    paramFwrite(element->current,
                sizeof(double), element->num_params, fp);
    paramFwrite(&element->current_match,
                sizeof(double), 1, fp);
    paramFwrite(element->best,
                sizeof(double), element->num_params, fp);
    paramFwrite(&element->best_match,
                sizeof(double), 1, fp);
    paramFwrite(element->orig,
                sizeof(double), element->num_params, fp);
    paramFwrite(element->search_divisions,
                sizeof(int),    element->num_params, fp);
    paramFwrite(element->search_rangemod,
                sizeof(double), element->num_params, fp);
    paramFwrite(element->search_count,
                sizeof(int),    element->num_params, fp);
    paramFwrite(element->search_range,
                sizeof(double), element->num_params, fp);


    fclose(fp);
    return 1;
}




/*
 * ParamtableBF_SAVEBEST
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
ParamtableBF_SAVEBEST(element, filename)
    struct paramtableBF_type *element;
    char *filename;
{
    FILE *fp;
    int   i;

    assert(filename != NULL);

    fp = fopen(filename, "w");

    if (fp == NULL)
    {
        Error();
        printf("paramtableBF: SAVEBEST: file cannot be opened for SAVEBEST action!\n");
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
 * ParamtableBF_RESTORE
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
ParamtableBF_RESTORE(element, filename)
    struct paramtableBF_type *element;
    char *filename;
{
    FILE *fp;
    int   i;

    assert(filename != NULL);

    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        Error();
        printf("paramtableBF: RESTORE: file cannot be opened for RESTORE action!\n");
        return 0;
    }

    /*
     * First delete the old parameter tables.
     */

    ParamtableBF_TABDELETE(element);

    fread(&element->iteration_number,     sizeof(int), 1, fp);
    fread(&element->num_params,           sizeof(int), 1, fp);
    fread(&element->num_params_to_search, sizeof(int), 1, fp);

    /*
     * Allocate the new parameter tables.
     */

    if (!ParamtableBF_TABCREATE(element, element->num_params))
        return 0;

    fread(element->search,
          sizeof(short),  element->num_params, fp);
    fread(element->type,
          sizeof(short),  element->num_params, fp);
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
        fread(element->label[i], LABEL_SIZE * sizeof(char), 1, fp);

        if (ferror(fp))
        {
            Error();
            printf("paramtableBF: RESTORE: error writing to file %s!\n",
                   filename);
            return 0;
        }
    }

    fread(element->current,
          sizeof(double), element->num_params, fp);
    fread(&element->current_match,
          sizeof(double), 1, fp);
    fread(element->best,
          sizeof(double), element->num_params, fp);
    fread(&element->best_match,
          sizeof(double), 1, fp);
    fread(element->orig,
          sizeof(double), element->num_params, fp);
    fread(element->search_divisions,
          sizeof(int),    element->num_params, fp);
    fread(element->search_rangemod,
          sizeof(double), element->num_params, fp);
    fread(element->search_count,
          sizeof(int),    element->num_params, fp);
    fread(element->search_range,
          sizeof(double), element->num_params, fp);

    element->alloced = 1;

    fclose(fp);
    return 1;
}




/*
 * ParamtableBF_RESTOREBEST
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
ParamtableBF_RESTOREBEST(element, filename)
    struct paramtableBF_type *element;
    char *filename;
{
    FILE *fp;
    int   i;
    char  line[MAX_LINELEN];

    assert(filename != NULL);

    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        Error();
        printf("paramtableBF: RESTOREBEST: file cannot be opened for RESTOREBEST action!\n");
        return 0;
    }

    /*
     * First delete the old parameter tables.
     */

    ParamtableBF_TABDELETE(element);

    /* Skip the first three lines. */

    for (i = 0; i < 3; i++)
        fgets(line, MAX_LINELEN, fp);

    fgets(line, MAX_LINELEN, fp);
    sscanf(line, "Iteration number: %d", &element->iteration_number);

    fgets(line, MAX_LINELEN, fp);
    sscanf(line, "Number of parameters: %d", &element->num_params);

    /*
     * Allocate the new parameter tables.
     */

    if (!ParamtableBF_TABCREATE(element, element->num_params))
        return 0;

    fgets(line, MAX_LINELEN, fp); /* Skip the next line */

    for (i = 0; i < element->num_params; i++)
    {
        fgets(line, MAX_LINELEN, fp);
        sscanf(line, "%*d %hd %lg %lg %s", &element->type[i],
               &element->range[i], &element->best[i], element->label[i]);
    }

    /*
     * Copy the best parameter values to the current table.
     */

    for (i = 0; i < element->num_params; i++)
        element->current[i] = element->best[i];

    fgets(line, MAX_LINELEN, fp);
    sscanf(line, "Best match so far: %lg", &element->best_match);

    element->alloced = 1;

    fclose(fp);
    return 1;
}




/*
 * ParamtableBF_DISPLAY
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
ParamtableBF_DISPLAY(element)
    struct paramtableBF_type *element;
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
        printf("%d\t%d\t%g\t\t%-10.6g\t%-10.6g\t%s\n", i,
               element->type[i], element->range[i],
               element->current[i], element->best[i], element->label[i]);
    }

    printf("Best match so far: %g\n", element->best_match);
}




/*
 * ParamtableBF_CHECK
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
ParamtableBF_CHECK(element)
    struct paramtableBF_type *element;
{
    int i;

    if (element->num_params < 0)
    {
        Error();
        printf("paramtableBF: number of parameters must be >= 0!\n");
    }

    for (i = 0; i < element->num_params; i++)
    {
        if (element->type[i] != 0 && element->type[i] != 1)
        {
            Error();
            printf("paramtableBF: type field %d must be 0 or 1!\n", i);
        }
    }

    for (i = 0; i < element->num_params; i++)
    {
        if (element->search_divisions[i] <= 0)
        {
            Error();
            printf("paramtableBF: search_divisions[%d] must be > 0!\n", i);
        }

        if (element->search_rangemod[i] <= 0.0 ||
            element->search_rangemod[i] > 1.0)
        {
            Error();
            printf("paramtableBF: search_rangemod[i] must be in range (0,1)!\n");
        }
    }

    for (i = 0; i < element->num_params; i++)
    {
        if (element->type[i] == 0 && element->range[i] < 0.0)
        {
            Error();
            printf("paramtableBF: parameter limits %d are bogus.\n", i);
            printf("range[%d] must be > 0!\n", i);
        }
        else if (element->type[i] == 1 && element->range[i] < 1.0)
        {
            Error();
            printf("paramtableBF: parameter limits %d are bogus.\n", i);
            printf("range[%d] must be >= 1.0!\n", i);
        }
    }
}









/*
 * OBJECT
 *     paramtableBF: a 1-D table for storing a parameter table.
 *                   Also has actions which allow a "brute force"
 *                   search of parameter space along single or
 *                   multiple dimensions.
 *
 *
 * FUNCTION
 *     ParamtableBF
 *
 * ARGUMENTS
 *     element   -- address of element
 *     action    -- address of action
 *
 * RETURN VALUE
 *     int  -- 0 = failure and 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableBF(element, action)
    struct paramtableBF_type *element;
    Action *action;
{
    int     i, num_params;
    char   *savefilename = NULL, *restorefilename = NULL;
    double  match;

    SELECT_ACTION(action)
    {
    case CREATE:
        element->alloced    = 0;
        break;


    case TABCREATE:
        if (action->argc != 1)
        {
            Error();
            printf("paramtableBF: usage: %s num_params\n",
                   "TABCREATE");
            return 0;
        }

        element->iteration_number = 0;

        num_params = atoi(action->argv[0]);

        if (!ParamtableBF_TABCREATE(element, num_params))
            return 0;

        break;


    case DELETE:
        /*FALLTHROUGH*/


    case TABDELETE:
        ParamtableBF_TABDELETE(element);
        break;


    case INITSEARCH:
        element->iteration_number++;

        if (!ParamtableBF_INITSEARCH(element,
                                     action->argc, action->argv))
        {
            return 0;
        }

        break;


    case UPDATE_PARAMS:
        element->iteration_number++;

        if (!ParamtableBF_UPDATE_PARAMS(element))
            return 0;

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
            printf("paramtableBF: usage: EVALUATE match");

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

            for (i = 0; i < element->num_params; i++)
                element->best[i] = element->current[i];

            element->best_match = match;
            element->new_best_match = 1;
        }
        else
        {
            element->new_best_match = 0;
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
            printf("paramtableBF: SAVE: no filename specified!\n");
            return 0;
        }

        if (!ParamtableBF_SAVE(element, savefilename))
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
            printf("paramtableBF: usage: SAVEBEST filename\n");
            return 0;
        }

        if (!ParamtableBF_SAVEBEST(element, savefilename))
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
            printf("paramtableBF: RESTORE: no filename specified!\n");
            return 0;
        }

        if (!ParamtableBF_RESTORE(element, restorefilename))
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
            printf("paramtableBF: usage: RESTOREBEST filename\n");
            return 0;
        }

        if (!ParamtableBF_RESTOREBEST(element, restorefilename))
            return 0;

        break;


    case DISPLAY:
        ParamtableBF_DISPLAY(element);
        break;


    case CHECK:
        ParamtableBF_CHECK(element);
        break;


    default:
        Error();
        printf("paramtableBF: invalid action %s\n", action->name);
        return 0;
    }

    return 1;
}




/*
 * The following is a utility function to put parameters into the table
 * with less fuss.
 */

/*
 * do_initparamBF
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
do_initparamBF(argc, argv)
    int argc;
    char **argv;
{
    int      param;
    char    *path;
    struct paramtableBF_type *element;
    short    type;
    double   range;
    double   center;
    char    *label;

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

    element = (struct paramtableBF_type *)GetElement(path);

    if (!CheckClass(element, ClassID("param")))
    {
        Error();
        printf("initparamBF: element %s is not a parameter table object!\n",
               element->name);
        return 0;
    }

    if ((param < 0) || (param >= element->num_params))
    {
        Error();
        printf("initparamBF: parameter index is out of range!\n");
        return 0;
    }

    /* FIXME: this is redundant; use a switch statement. */

    if ((type != 0) && (type != 1))
    {
        Error();
        printf("initparamBF: type field must be 0 or 1!\n");
        return 0;
    }

    if (type == 0) /* additive parameter */
    {
        if (range < 0.0)
        {
            Error();
            printf("initparamBF: additive parameter range must be > 0!\n");
            return 0;
        }
    }

    if (type == 1) /* multiplicative parameter */
    {
        if (range < 1.0)
        {
            Error();
            printf("initparamBF: multiplicative parameter range must be > 1.0!\n");
            return 0;
        }
    }

    element->type[param]  = type;
    element->range[param] = range;
    element->orig[param]  = center;

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
 * SetSearchBF
 *
 * FUNCTION
 *     Sets the search flags for the paramtableBF object.
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
SetSearchBF(argc, argv, all, not, add)
    int argc;
    char **argv;
    short all;
    short not;
    short add;
{
    int    i, param, tosearch;
    struct paramtableBF_type *element;

    element = (struct paramtableBF_type*) GetElement(argv[1]);

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
