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
 *
 *   spkcmp.c -- routine to compare spike time files and output a number
 *               corresponding to the difference in the spike times
 */

#include <math.h>
#include <string.h>
#include "param_ext.h"

#define SMALL                    1.0e-5
#define DELIMITER               -9999    /* delimits last value in array  */
#define CURRENT_TOLERANCE        0.01    /* maximum permissible difference
                                          * in input currents */
#define MISSING_SPIKE_PENALTY    1.0     /* default value for missing spike
                                          * penalty */

/*
 * Default penalty per second of nonmonotonicity in consecutive ISIs per
 * spike:
 */

#define NONMONOTONICITY_PENALTY  0.0

/*
 * ACCEPTABLE_ISI_JITTER:
 *
 * It's o.k. for isi[n] to be this much smaller than isi[n-1],
 * but no smaller than that; this is taken to be "noise".
 */

#define ACCEPTABLE_ISI_JITTER    0.002


/*
 * This function figures out the maximum numbers of spikes for
 * any current and the total number of currents.
 */

void
get_sizes(fp, num_currs, num_spikes)
    FILE *fp;
    int *num_currs;
    int *num_spikes;
{
    int spike_cnt, curr_cnt, maxspike_cnt;
    char str[80];

    maxspike_cnt = -1;
    curr_cnt = spike_cnt =  0;

    while (fgets(str, 80, fp) != NULL)
    {
        if (strstr(str, "CURR") != NULL)
        {
            ++curr_cnt;
        }
        else if (strstr(str, "SPK") != NULL)
        {
            ++spike_cnt;
        }
        else if (strstr(str, "----") != NULL)
        {
            if (spike_cnt > maxspike_cnt)
                maxspike_cnt = spike_cnt;

            spike_cnt = 0;
        }
    }

    /*
     * Increment spike counter to leave room for delimiter,
     * which also gets loaded into the array.
     */

    *num_currs   = curr_cnt;
    *num_spikes  = ++maxspike_cnt;

}



/*
 * This function reads the contents of one file into current
 * and spike arrays.  The last entry in the spike array is
 * marked by a large negative number, DELIMITER.  The total
 * number of spikes in the file is returned to the calling
 * function through num_spikes.  We also use end_time to
 * limit our spike arrays to times less than end_time; if
 * end_time is negative there is no limit.
 */

void
read_spk_data(fp, curr, spike, num_spikes, end_time)
    FILE *fp;
    double *curr;
    double **spike;
    int *num_spikes;
    double end_time;
{
    int curr_cnt, spike_cnt;
    char str[80], curr_str[80], spike_str[80];
    double spike_time;

    rewind(fp);
    curr_cnt = spike_cnt = *num_spikes = 0;

    while (fgets(str, 80, fp) != NULL)
    {
        if (strstr(str, "CURR") != NULL)
        {
            sscanf(str, "%*s %s", curr_str);
            curr[curr_cnt] = Atod(curr_str);
        }
        else if (strstr(str, "SPK") != NULL)
        {
            sscanf(str, "%*s %s", spike_str);
            spike_time = Atod(spike_str);

            if ((end_time > 0.0) && (spike_time > end_time))
                continue;  /* Ignore spikes beyond end_time. */
            else
                spike[curr_cnt][spike_cnt++] = Atod(spike_str);
        }
        else if (strstr(str, "----") != NULL)
        {
            *num_spikes += spike_cnt;
            spike[curr_cnt][spike_cnt++] = DELIMITER;
            ++curr_cnt;
            spike_cnt = 0;
        }
    }

    fclose(fp);
}




int
allocate_space(curr, spk, num_currs, num_spks)
    double **curr;
    double ***spk;
    int num_currs;
    int num_spks;
{
    int i;

    if ((*curr = (double *) calloc(num_currs, sizeof(double))) == NULL)
    {
        Error();
        printf("spkcmp: allocate_space: Can't allocate memory.");
        return 0;
    }

    if ((*spk = (double **) calloc(num_currs, sizeof(double *))) == NULL)
    {
        Error();
        printf("spkcmp: allocate_space: Can't allocate memory.");
        return 0;
    }

    for (i = 0; i < num_currs; i++)
    {
        if (((*spk)[i] = (double *) calloc(num_spks, sizeof(double)))
            == NULL)
        {
            Error();
            printf("spkcmp:allocate_space: Can't allocate memory.");
            return 0;
        }
    }

    return 1;
}




int
load_data(rawfile, simfile, raw_curr, raw_spk, sim_curr, sim_spk,
	  num_currs, num_spks, raw_spike_total, sim_spike_total, end_time)
    FILE *rawfile;
    FILE *simfile;
    double **raw_curr;
    double ***raw_spk;
    double **sim_curr;
    double ***sim_spk;
    int *num_currs;
    int *num_spks;
    int *raw_spike_total;
    int *sim_spike_total;
    double end_time;
{
    int raw_curr_cnt, raw_spk_cnt, sim_curr_cnt, sim_spk_cnt;
    int num_raw_spikes, num_sim_spikes;

    /*
     * Figure out the maximum sizes of the arrays and allocate memory
     * for the curr and spk arrays accordingly.  Even though it wastes
     * memory, it's easier to make the arrays for the real and simulated
     * data the same size.  Note that we ignore end_time here, since we're
     * just trying to figure out an upper bound for the array (this is
     * pretty lame).
     */

    get_sizes(rawfile, &raw_curr_cnt, &raw_spk_cnt);
    get_sizes(simfile, &sim_curr_cnt, &sim_spk_cnt);

    if (raw_curr_cnt != sim_curr_cnt)
    {
        Error();
        printf("spkcmp: load_data: Unequal number of currents -- rerun simulations.\n");
        return 0;
    }

    *num_currs = raw_curr_cnt;

    /*
     * *num_spks is the maximum number of spikes for any current in either
     * the raw or simulation spike time file.
     */

    *num_spks  = MAX(raw_spk_cnt, sim_spk_cnt);

    if (!allocate_space(raw_curr, raw_spk, *num_currs, *num_spks))
        return 0;

    if (!allocate_space(sim_curr, sim_spk, *num_currs, *num_spks))
        return 0;

    /*
     * Finally, load in the data for the two arrays.  Also add up the
     * total number of SPKs.
     */

    read_spk_data(rawfile, *raw_curr, *raw_spk, &num_raw_spikes, end_time);
    read_spk_data(simfile, *sim_curr, *sim_spk, &num_sim_spikes, end_time);

    *raw_spike_total = num_raw_spikes;
    *sim_spike_total = num_sim_spikes;

    return 1;
}




/*
 * This function compares current values; if they're different return 0;
 * else return 1.
 */

int
compare_currents(num_currs, raw_curr, sim_curr)
    int num_currs;
    double *raw_curr;
    double *sim_curr;
{
    int i;

    for (i = 0; i < num_currs; i++)
    {
        if (fabs(raw_curr[i] - sim_curr[i]) > CURRENT_TOLERANCE)
            return 0;
    }

    return 1;
}





/*
 * This function compares spike values and outputs a double which is, in
 * general, the average absolute difference in spike times, although a lot of
 * other criteria are also used to get the number.  Also returns a count of
 * mismatched spikes.
 */

double
compare_spikes(num_currs, num_spikes, raw_spk, sim_spk, offset,
	       missing_spike_penalty, mismatch, raw_spike_total,
	       sim_spike_total, pow1, pow2, nonmonotonicity_penalty)
    int num_currs;
    int num_spikes;
    double **raw_spk;
    double **sim_spk;
    double offset;
    double missing_spike_penalty;
    int *mismatch;
    int raw_spike_total;
    int sim_spike_total;
    double pow1;
    double pow2;
    double nonmonotonicity_penalty;
{
    int    i, j, rawdone, simdone, real_tracecount, sim_tracecount;
    double spike_total;
    double isi, previous_isi, disi, previous_spk;
    double current_mismatch;
    double mismatch_sum  = 0.0;

    rawdone = simdone = 0;
    real_tracecount = sim_tracecount = -1;
    previous_isi = 0.0;
    previous_spk = 0.0;


    /*
     * Check for cases where there are no spikes.
     */

    if ((raw_spike_total == 0) && (sim_spike_total == 0))
        return 0.0;


    /* Calculate the spike counts. */

    for (i = 0; i < num_currs; i++)
    {
        rawdone = simdone = 0;

        /*
         * Calculate the number of spikes in the trace for the
         * real cell.
         */

        for (j = 0; j < num_spikes; j++)
        {
            if (fabs(raw_spk[i][j] - DELIMITER) < SMALL)
            {
                real_tracecount = j;
                break;
            }
        }

        /*
         * Calculate the number of spikes in the trace for the
         * simulated cell.  Also add the offset to the spike
         * times.
         *
         * WARNING: the change in the spike times propagates out of this
         * function!
         */

        for (j = 0; j < num_spikes; j++)
        {
            if (fabs(sim_spk[i][j] - DELIMITER) < SMALL)
            {
                sim_tracecount = j;
                break;
            }
            else
            {
                sim_spk[i][j] += offset;
            }
        }

        /*
         * Calculate the mismatch.  We also have to calculate the interspike
         * interval (isi) and the difference in consecutive interspike
         * intervals (disi) for the simulated traces to penalize for
         * non-monotonicity of ISIs.  This is specific to the cells I'm
         * modeling; a different kind of cell behavior will require a
         * different function here.
         */

        /*
         * Calculate the difference in consecutive ISIs for the sim array
         * only and add in the penalty for non-monotonicity to mismatch_sum.
         * The nonmonotonicity penalty is in units of sec^(-1) i.e. add that
         * penalty for 1 second of total nonmonotonicity.
         */

        for (j = 0; j < num_spikes; j++)
        {
            if (fabs(sim_spk[i][j] - DELIMITER) < SMALL)
                break;  /* no more spikes in sim array */

            if (j == 0)  /* first spike */
            {
                previous_spk = sim_spk[i][0];
            }
            else if (j == 1) /* first ISI */
            {
                previous_isi = sim_spk[i][1] - previous_spk;
                previous_spk = sim_spk[i][1];
            }
            else
            {
                isi  = sim_spk[i][j] - previous_spk;
                disi = previous_isi - isi;
                previous_spk = sim_spk[i][j];
                previous_isi = isi;

                /*
                 * Don't penalize a tiny amount of non-monotonicity that
                 * could be just due to noise.
                 */

                if (disi > ACCEPTABLE_ISI_JITTER)
                    mismatch_sum += disi * nonmonotonicity_penalty;
            }
        }

        /*
         * Then compare the timings for the raw vs. the sim arrays.
         */

        for (j = 0; j < num_spikes; j++)
        {
            current_mismatch = 0.0;

            /* Are there no more spikes for either array? */

            if (fabs(raw_spk[i][j] - DELIMITER) < SMALL)
                rawdone = 1;

            if (fabs(sim_spk[i][j] - DELIMITER) < SMALL)
                simdone = 1;

            if (!rawdone && !simdone)
            {
                if (fabs(pow1) < SMALL)
                {
                    current_mismatch = fabs(raw_spk[i][j] - sim_spk[i][j]);
                }
                else
                {
                    /*
                     * The idea here is to weight spike mismatches more
                     * heavily the closer they are to the start.  The degree
                     * of weighting is set by pow1.  In order to prevent
                     * really early spikes (e.g. 1 msec) from dominating the
                     * error score, the factor raised to the power of pow1 is
                     * set to be a maximum of 0.010, which gives a weighting
                     * of 100 for a pow1 of 1.0.
                     */

                    current_mismatch
                        = (fabs(raw_spk[i][j] - sim_spk[i][j]))
                        / pow(MAX(0.010, raw_spk[i][j]), pow1);
                }
            }
            else if ((rawdone && !simdone) || (!rawdone && simdone))
            {
                /*
                 * There is either a missing spike or an extra spike in the
                 * simulated trace as compared to the real trace.  Give a
                 * maximal penalty when there are no spikes in the real
                 * trace or when pow2 is zero; otherwise, reduce the penalty
                 * as a function of a scale factor calculated from the spike
                 * counts of both the real and the simulated traces.  The
                 * idea here is that spike mismatches are less significant
                 * when both traces have a lot of spikes but more
                 * significant when the spike counts are very imbalanced.
                 */

                if ((fabs(pow2) < SMALL) || (real_tracecount == 0))
                {
                    current_mismatch = missing_spike_penalty;
                }
                else
                {
                    double scale_factor;
                    int min_spikes = MIN(real_tracecount, sim_tracecount);
                    int max_spikes = MAX(real_tracecount, sim_tracecount);

                    assert(max_spikes > 0);
                    /* n.b. max_spikes cannot equal min_spikes */
                    assert(min_spikes < max_spikes);


                    /*
                     * min_spikes/max_spikes is always in the range (0, 1).
                     * So as pow2 approaches infinity, scale_factor
                     * approaches 1.0.  As pow2 approaches zero,
                     * scale_factor also approaches zero.
                     */

                    scale_factor = 1.0 - (pow(((double)min_spikes)
                                              / ((double)max_spikes), pow2));
                    current_mismatch = missing_spike_penalty * scale_factor;
                }

                (*mismatch)++;
            }
            else if (rawdone && simdone) /* Neither array has any more spikes
                                          * left. */
            {
                break;
            }

            mismatch_sum += current_mismatch;
        }
    }

    /*
     * For the final result we divide by the smaller of the spike totals
     * since that's what gives the larger penalty.  This is mainly a factor
     * when the two traces have very different numbers of spikes, which we
     * want to penalize harshly.
     */

    spike_total = (double) (MIN(raw_spike_total, sim_spike_total));

    /*
     * Don't allow a spike_total of less than 1.0; if it's zero
     * (which can easily happen) you get division by zero errors.
     */

    spike_total = MAX(spike_total, 1.0);

    return (mismatch_sum / spike_total);
}




/*
 * Spkcmp
 *
 * FUNCTION
 *     Compares two spike time files, one generated by gen2spk and
 *     the other generated from real data and returns a value
 *     which indicates how close the spike timings of the two
 *     files are.  For more information, see spkcmp.txt.
 *
 * ARGUMENTS
 *     int    argc - number of command arguments
 *     char **argv - command arguments proper
 *
 * RETURN VALUE
 *     float
 *
 * AUTHOR
 *     Mike Vanier
 */

double
Spkcmp(argc, argv)
    int argc;
    char **argv;
{
    FILE   *rawfile, *simfile;
    double  offset   =  0.0;
    double  pow1     =  0.0;
    double  pow2     =  0.0;
    double  end_time = -1.0;
    double  missing_spike_penalty   = MISSING_SPIKE_PENALTY;
    double  nonmonotonicity_penalty = NONMONOTONICITY_PENALTY;
    int     print_missing_spikes = 0;  /* flag for whether to print number of
                                        * missing spikes instead of adding a
                                        * penalty per spike to the result.
                                        */

    double *raw_curr, **raw_spk, *sim_curr, **sim_spk;
    double  out;
    int     num_currs, num_spikes, raw_spike_total, sim_spike_total;
    int     mismatch = 0;
    int     status;
    int     i;

    /* command-line arguments */

    initopt(argc, argv,
            "reference-data simulation-data -offset time -pow1 p -pow2 q -msp missing_spike_penalty -nmp nonmonotonicity_penalty -et end_time -spk");

    while ((status = G_getopt(argc, argv)) == 1)
    {
        if (strcmp(G_optopt, "-offset") == 0)
            offset = Atod(optargv[1]);
        else if (strcmp(G_optopt, "-pow1") == 0)
            pow1 = Atod(optargv[1]);
        else if (strcmp(G_optopt, "-pow2") == 0)
            pow2 = Atod(optargv[1]);
        else if (strcmp(G_optopt, "-msp") == 0)
            missing_spike_penalty = Atod(optargv[1]);
        else if (strcmp(G_optopt, "-nmp") == 0)
            nonmonotonicity_penalty = Atod(optargv[1]);
        else if (strcmp(G_optopt, "-et") == 0)
            end_time = Atod(optargv[1]);
        else if (strcmp(G_optopt, "-spk") == 0)
            print_missing_spikes = 1;
    }

    if (status < 0)
    {
        printoptusage(argc, argv);
        return ERROR_PENALTY;
    }

    if ((rawfile = fopen(optargv[1], "r")) == NULL)
    {
        Error();
        printf("spkcmp: Cannot open input file \"%s\".\n", optargv[1]);
        return ERROR_PENALTY;
    }

    if ((simfile = fopen(optargv[2], "r")) == NULL)
    {
        Error();
        printf("spkcmp: Cannot open input file \"%s\".\n", optargv[2]);
        return ERROR_PENALTY;
    }

    if ((pow1 < 0.0) || (pow2 < 0.0))
    {
        Error();
        printf("spkcmp: pow1 and pow2 must be >= 0.0!\n");
        return ERROR_PENALTY;
    }


    if (print_missing_spikes)
        missing_spike_penalty = 0.0;

    /*
     * Load in the raw current array, the simulation current array, the raw
     * spike array, the simulation spike array, the total number of
     * currents, the maximum number of spikes for any given current in
     * either the raw or simulation spike time file (*num_spikes), and the
     * total number of spikes in the raw or simulation spike time files
     * (*raw_spike_total and *sim_spike_total).  If any error occurs issue
     * the appropriate error message (done in the lower-level functions) and
     * return a huge penalty value.  The "end_time" variable is a time point
     * beyond which we don't want to compare spikes; spikes after that time
     * point are not loaded into the spike arrays.
     */

    if (!load_data(rawfile, simfile, &raw_curr, &raw_spk,
                   &sim_curr, &sim_spk, &num_currs, &num_spikes,
                   &raw_spike_total, &sim_spike_total, end_time))
    {
        return ERROR_PENALTY;
    }

    /*
     * Compare the currents in the raw and simulation spike time files; if
     * they are different then the parameter search was incorrectly run and
     * needs to be redone.
     */

    if (!compare_currents(num_currs, raw_curr, sim_curr))
    {
        Error();
        printf("spkcmp: input currents are different! Rerun simulations.");

        return ERROR_PENALTY;
    }

    out = compare_spikes(num_currs, num_spikes, raw_spk,
                         sim_spk, offset, missing_spike_penalty,
                         &mismatch, raw_spike_total, sim_spike_total,
                         pow1, pow2, nonmonotonicity_penalty);

    /*
     * Output error (and count of spike mismatches if requested).  Due to
     * defects in the getarg function of genesis, the count of spike
     * mismatches is printed rather than returned.  Eventually I'd like to
     * change that.
     */

    if (print_missing_spikes)
        printf("%.4f\t\t%d\n", out, mismatch);

    free(raw_curr);
    free(sim_curr);

    for (i = 0; i < num_currs; i++)
    {
        free(raw_spk[i]);
        free(sim_spk[i]);
    }

    free(raw_spk);
    free(sim_spk);

    return out;
}




