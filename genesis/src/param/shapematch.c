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

/*
 * This code was brutally hacked (or unhacked, depending on your point of
 * view) from Upi Bhalla's "shapematch" code by Mike Vanier, Oct. 1995.
 */

#include <math.h>
#include "param_ext.h"
#include "olf_struct.h"

#define MAXISIS  500       /* maximum number of interspike intervals */
#define SDT      0.0001    /* 0.1 msec, a typical output interval    */
#define TINYT    1e-8      /* acceptable difference in time values   */

/* A data structure to store the beginning and end times of an ISI:  */

typedef struct ISIinfo_type
{
    int    lo,  hi;
    double tlo, thi;
} ISIinfo;


/*
 * Find the largest point in an array.
 * This is called est_max2 instead of est_max to avoid a conflict with
 * Upi's code elsewhere in genesis.
 */

double
est_max2(arr, npts)
    double *arr;
    int npts;
{
    int    i;
    double max;

    if (npts <= 0)
        return 0.0;

    max = arr[0];

    for (i = 1; i < npts; i++)
    {
        if (max < arr[i])
            max = arr[i];
    }

    return max;
}



/*
 * Find the smallest point in an array.
 * This is called est_min2 instead of est_min to avoid a conflict with
 * Upi's code elsewhere in genesis.
 */

double
est_min2(arr, npts)
    double *arr;
    int npts;
{
    int    i;
    double min;

    if (npts <= 0)
        return 0.0;

    min = arr[0];

    for (i = 1; i < npts; i++)
    {
        if (min > arr[i])
            min = arr[i];
    }

    return min;
}


/*
 * This is taken from Upi's code and cleaned up.  All floats were changed to
 * doubles.
 */

#define MAXPTS 10000

int
load_file_or_table2(name, px, py)
    char *name;
    double **px;
    double **py;
{
    Element *elm;
    int      i;
    double  *x, *y;

    elm = GetElement(name);

    if (elm)
    {
        short datatype;
        Interpol **pip;

        if (strcmp(elm->object->type, "table_type") != 0)
        {
            Error();
            printf("shape_match: load_file_or_table2: element %s is not a table object\n",
                   name);
            return 0;
        }

        if ((pip  = (Interpol **)GetFieldAdr(elm, "table", &datatype))
            != NULL)
        {
            if (datatype == INVALID)
            {
                double dt = ((*pip)->xmax - (*pip)->xmin) / (*pip)->xdivs;
                double currt = ((*pip)->xmin);

                if (!(*pip && (*pip)->allocated))
                {
                    /* The interpol has not been allocated */
                    Error();
                    printf("shape_match: load_file_or_table2: table on element %s has not been allocated\n",
                           name);
                    return 0;
                }

                *px = x = (double *)calloc(MAXPTS, sizeof(double));

                if ((void *)x == NULL)
                {
                    Error();
                    printf("shape_match: load_file_or_table2: could not allocate memory!\n");
                    return 0;
                }

                *py = y = (double *)calloc(MAXPTS, sizeof(double));

                if ((void *)y == NULL)
                {
                    Error();
                    printf("shape_match: load_file_or_table2: could not allocate memory!\n");
                    return 0;
                }

                for (i = 0; (i <= (*pip)->xdivs) && (i < MAXPTS); i++)
                {
                    y[i] = (*pip)->table[i];
                    x[i] = currt;
                    currt += dt;
                }

                return i;
            }
        }
        else
        {
            Error();
            printf("shape_match: load_file_or_table2: could not get table address.\n");
            return 0;
        }
    }
    else
    {
        FILE *fp;

        if (!(fp = fopen(name, "r")))
        {
            Error();
            printf("shape_match: load_file_or_table2: could not open file or table %s\n", name);
            return 0;
        }

        *px = x = (double *)calloc(MAXPTS, sizeof(double));

        if ((void *)x == NULL)
        {
            Error();
            printf("shape_match: load_file_or_table2: could not allocate memory!\n");
            return 0;
        }

        *py = y = (double *)calloc(MAXPTS, sizeof(double));

        if ((void *)y == NULL)
        {
            Error();
            printf("shape_match: load_file_or_table2: could not allocate memory!\n");
            return 0;
        }

        for (i = 0;
             (i < MAXPTS) && (fscanf(fp, "%lf %lf", &(x[i]), &y[i]) != EOF);
             i++);

        fclose(fp);

        return i;
    }

    return 0;
}




/*
 * ShapeMatch
 *
 * FUNCTION
 *     This function compares the interspike intervals of
 *     two traces and computes a match value based on
 *     the sum of the squares of the differences between
 *     the ISIs of the two traces.  Used in do_shapematch.
 *
 * ARGUMENTS
 *     ref   -- reference values
 *     reft  -- reference times
 *     nref  -- number of reference samples
 *     sim   -- simulation values
 *     simt  -- simulation times
 *     nsim  -- number of simulation samples
 *     match -- computed match value
 *     dt    -- time step size
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * AUTHORS
 *     Upi Bhalla, Mike Vanier (cleaned up and commented)
 *
 * NOTES
 *     This supplants the old shapematch routine; I wanted
 *     to call it just ShapeMatch but the old routine still
 *     exists in the tools library, so ShapeMatch it is.
 *
 */

int
ShapeMatch(ref, reft, nref, sim, simt, nsim, match, dt)
    double *ref;
    double *reft;
    int nref;
    double *sim;
    double *simt;
    int nsim;
    double *match;
    double dt;
{
    int        i, k, m, count;
    int        nrefspk, nsimspk, nminspk;
    ISIinfo   *refspk, *simspk;
    double     thresh;
    double     maxest, minest;
    double     t, st;
    double     ry, sy;
    double     sumsq;
    double     scalet;

    refspk = (ISIinfo *)calloc(MAXISIS, sizeof(ISIinfo));

    if (((void *)refspk) == NULL)
    {
        Error();
        printf("shape_match: could not allocate memory!\n");
        return 0;
    }

    simspk = (ISIinfo *)calloc(MAXISIS, sizeof(ISIinfo));

    if (((void *)simspk) == NULL)
    {
        Error();
        printf("shape_match: could not allocate memory!\n");
        return 0;
    }

    count = 0;   /* count of the total number of points
                  * used to calculate match */
    sumsq = 0.0;

    /* Setting the max and min values for the spike peaks */
    maxest = est_max2(ref, nref);
    minest = est_min2(ref, nref);

    /* Set the threshold for spikes: 1/6 of the way from minest to maxest. */
    thresh = (maxest + 5 * minest) / 6.0;

    /* Find the spikes in the reference trace: */

    nrefspk = 0;
    refspk[nrefspk].lo  = 0;
    refspk[nrefspk].tlo = reft[0];

    for (i = 1; i < (nref - 2); i++)
    {
        if ((ref[i-1] < thresh) && (ref[i] <= thresh)
            && (ref[i+1] > thresh) && (ref[i+2] > thresh))
        {
            /*
             * We're above threshold: spike starts here so
             * interspike interval ends here.  The time of
             * the interspike interval is between tlo and thi
             * for a given refspk.
             */

            refspk[nrefspk].hi  = i;
            refspk[nrefspk].thi = reft[i];
            nrefspk++;
        }

        if ((ref[i-1] > thresh) && (ref[i] >= thresh)
            && (ref[i+1] < thresh) && (ref[i+2] < thresh))
        {
            /*
             * We're below threshold: spike ends here so
             * interspike interval starts here.
             */

            refspk[nrefspk].lo  = i + 1;
            refspk[nrefspk].tlo = reft[i+1];
        }
    }

    /*
     * The last sample is considered to be the end of the
     * last interspike interval.
     */

    refspk[nrefspk].hi  = nref - 1;
    refspk[nrefspk].thi = reft[nref-1];
    nrefspk++;

    /* Find the spikes for the simulation trace: */

    nsimspk = 0;
    simspk[nsimspk].lo  = 0;
    simspk[nsimspk].tlo = simt[0];

    for (i = 1; i < (nsim - 2); i++)
    {
        if ((sim[i-1] < thresh) && (sim[i] <= thresh)
            && (sim[i+1] > thresh) && (sim[i+2] > thresh))
        {
            simspk[nsimspk].hi  = i;
            simspk[nsimspk].thi = simt[i];
            nsimspk++;
        }

        if ((sim[i-1] > thresh) && (sim[i] >= thresh)
            && (sim[i+1] < thresh) && (sim[i+2] < thresh))
        {
            simspk[nsimspk].lo  = i + 1;
            simspk[nsimspk].tlo = simt[i+1];
        }
    }

    simspk[nsimspk].hi  = nsim - 1;
    simspk[nsimspk].thi = simt[nsim-1];
    nsimspk++;

    nminspk = (nsimspk > nrefspk) ? nrefspk : nsimspk;

    k = 1;
    m = 1;
    t = 0.0;

    /*
     * Calculate the sum of the squared differences for the interspike
     * intervals.
     */

    for (i = 0; i < nminspk; i++)
    {
        /*
         * scalet compensates for the possible difference in the
         * length of the interspike intervals.  If scalet is very
         * different from 1.0 this becomes a pretty odd way to
         * compare traces, but it won't penalize too heavily
         * if a spike happens to come a bit early or a bit late
         * in a trace.
         */

        scalet = (simspk[i].thi - simspk[i].tlo) /
            (refspk[i].thi - refspk[i].tlo);

        /* Loop through one interspike interval. */

        for (; t < refspk[i].thi; t += dt)
        {
            /*
             * Find the position in the reference array
             * which corresponds to this time.
             */

            for (; (t > reft[k]) && (k < nref); k++);

            /*
             * Calculate the reference trace's value at this time
             * using a simple linear interpolation from the
             * last sample in the array up to the current time.
             */

            ry = ref[k-1] + (ref[k] - ref[k-1]) *
                (t - reft[k-1]) / (reft[k] - reft[k-1]);

            /*
             * Find the simulation time which corresponds to
             * this time in the reference trace.
             */

            st = simspk[i].tlo + (t - refspk[i].tlo) * scalet;

            /*
             * Find the position in the simulation array
             * which corresponds to this (simulation) time.
             */

            for (; (st > simt[m]) && (m < nsim); m++);

            /*
             * Calculate the simulation trace's value at this time
             * using a simple linear interpolation from the
             * last sample in the array up to the current time.
             */
            sy = sim[m-1] + (sim[m] - sim[m-1]) *
                (st - simt[m-1]) / (simt[m] - simt[m-1]);

            sumsq += (ry - sy) * (ry - sy);
            ++count;
        }
    }

    *match = sqrt(sumsq / count);

    free(refspk);
    free(simspk);

    return 1;
}




/*
 * FastShapeMatch
 *
 * FUNCTION
 *     This function compares the waveforms of two traces and
 *     computes a match value based on the root-mean-squared
 *     differences between the ISIs of the two traces.  Used in
 *     do_shapematch.  This is a "quick-and-dirty" algorithm,
 *     suitable when there are no spikes in the waveforms and the
 *     number of samples and the times steps of both reference and
 *     simulation traces are the same.  Note that the time steps
 *     are not used here, so you have to know that the dt's are
 *     the same for both reference and simulation traces.
 *
 * ARGUMENTS
 *     ref     -- reference values
 *     reft    -- reference times
 *     nref    -- number of reference samples
 *     sim     -- simulation values
 *     simt    -- simulation times
 *     nsim    -- number of simulation samples
 *     match   -- RMS difference between traces or mean of absolute values of
 *                differences between the traces if -absdev option is on.
 *     absflag -- flag: if 1, compute average of absolute deviations;
 *                otherwise, compute RMS deviation.
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * AUTHORS
 *     Mike Vanier
 */

int
FastShapeMatch(ref, reft, nref, sim, simt, nsim, match, absflag)
    double *ref;
    double *reft;
    int nref;
    double *sim;
    double *simt;
    int nsim;
    double *match;
    short absflag;
{
    int i;
    double sumsq = 0.0;
    double sum   = 0.0;

    if (nref != nsim)
    {
        Error();
        printf("shape_match: number of samples must be the same for reference and test traces!\n");
        printf("    If this is impossible, use the -alternate option for shape_match.\n");
        return 0;
    }

    for (i = 0; i < nsim; i++)
    {
        if (fabs(simt[i] - reft[i]) > TINYT)
        {
            Error();
            printf("shape_match: times of reference and test traces are not equal!\n");
            return 0;
        }

        if (absflag)
        {
            /* compute average of absolute deviations, not RMS error */
            sum += fabs(ref[i] - sim[i]);
        }
        else
        {
            sumsq += (ref[i] - sim[i]) * (ref[i] - sim[i]);
        }
    }

    if (absflag)
        *match = sum / nsim;
    else
        *match = sqrt(sumsq / nsim);

    return 1;
}




/*
 * do_shapematch
 *
 * FUNCTION
 *     This function compares the interspike intervals of
 *     two traces and computes a match value based on
 *     the sum of the squares of the differences between
 *     the ISIs of the two traces.
 *
 * ARGUMENTS
 *     int    argc - number of command arguments
 *     char **argv - command arguments proper
 *
 * RETURN VALUE
 *     double; a huge penalty is returned on an error.
 *
 * AUTHORS
 *     Upi Bhalla, Mike Vanier (mostly cleaned up and commented)
 *
 */

double do_shapematch(argc, argv)
    int argc;
    char **argv;
{
    double   match;
    int      i, j;
    double  *sim, *simt, *ref, *reft;
    int      nsim, nref;
    double   dt = SDT;
    double   startt = 0.0;
    int      status;
    short    alt = 0;
    short    absdev = 0;

    initopt(argc, argv, "reference-file simulation-file -start start-time -stepsize dt -absdev -alternate");

    while ((status = G_getopt(argc, argv)) == 1)
    {
        if (strcmp(G_optopt, "-start") == 0)
        {
            startt = Atod(optargv[1]);

            if ((startt < 0.0))
            {
                Error();
                printf("shape_match: start time = %s is too small\n",
                       optargv[3]);
                return ERROR_PENALTY;
            }
        }
        else if (strcmp(G_optopt, "-stepsize") == 0)
        {
            dt = Atod(optargv[1]);
        }
        else if (strcmp(G_optopt, "-alternate") == 0)
        {
            /*
             * Use Upi's original method, which is slower
             * but more generally applicable.
             */

            alt = 1;
        }
        else if (strcmp(G_optopt, "-absdev") == 0)
        {
            /*
             * Use mean of absolute values of differences between reference
             * and simulation traces to compute match value, not mean
             * squared error.
             */

            absdev = 1;
        }
    }

    if (optargc < 2 || status < 0)
    {
        printoptusage(argc, argv);
        return ERROR_PENALTY;
    }

    if ((absdev == 1) && (alt == 1))
    {
        Error();
        printf("shape_match: -absdev and -alternate options are mutually exclusive!\n");
        return ERROR_PENALTY;
    }

    /* Load reference times and values: */

    if ((nref = (load_file_or_table2(optargv[1], &reft, &ref))) < 1)
        return ERROR_PENALTY;

    /*
     * Load simulation times and values, to be compared with reference
     * values:
     */

    if ((nsim = (load_file_or_table2(optargv[2], &simt, &sim))) < 1)
    {
        free(ref);
        free(reft);
        return ERROR_PENALTY;
    }

    /*
     * Figure out how many samples to skip based on startt.
     */

    for (i = 0; reft[i] < startt; i++);

    for (j = 0; simt[j] < startt; j++);

    if (alt)
    {
        if (ShapeMatch((ref + i), (reft + i), (nref - i),
                        (sim + j), (simt + j), (nsim - j),
                        &match, dt) == 0)
        {
            return ERROR_PENALTY;
        }
    }
    else
    {
        if (FastShapeMatch((ref + i), (reft + i), (nref - i),
                           (sim + j), (simt + j), (nsim - j),
                           &match, absdev) == 0)
        {
            return ERROR_PENALTY;
        }
    }

    free(sim);
    free(simt);
    free(ref);
    free(reft);

    return match;
}


