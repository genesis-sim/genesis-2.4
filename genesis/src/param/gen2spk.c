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


/* the following definitions are for the spike detection subroutine */
#define MAX_LINES       11000
#define MAX_SPIKES      10000
#define TEST_WINDOW     5
#define UPDOWN_SIZE     20000
#define MAX_SPIKE_WIDTH 0.003 /* spikes should be no wider than 3 msec */


char output_data[MAX_LINES][120]; /* where the output data is stored */
int  next_line;                   /* the next available line         */



void
clear_output_data()
{
    int i;

    for (i = 0; i < MAX_LINES; i++)
        strcpy(&output_data[i][0], "");
}


void
strip_header(inputfp)
    FILE *inputfp;
{
    /*
     *  Strip everything up to the first /newplot;
     *  this is included in case you want extra bookkeeping
     *  information at the beginning of the file, for example
     *  to add more xplot commands.
     */

    char test[120];

    while ((fgets(test, 120, inputfp)) != NULL)
    {

        if (strstr(test, "/newplot") != NULL)
            break;
    }
}


int
get_data(inputfp, rawdatasize, rawdata, current)
    FILE *inputfp;
    long rawdatasize;
    double *rawdata;
    double *current;
{
    /* rawdatasize: maximum value, supplied to the function */

    /*
     *  This function assumes that the only input is either /plotname,
     *  /newplot or raw data.  It uses /plotname to get the input current,
     *  gets the raw data and puts it into rawdata, and puts the number
     *  of raw data points into rawdatasize.  If it finds any other
     *  commands starting with a /, it ignores them.
     *
     *  This function expects to see the first line as a /plotname;
     *  the last line will be a /newplot for the next trace or an EOF.
     *  Also, the plotname must have the current in it, like this:
     *
     *  /plotname 0.87
     *
     */

    int   i;
    char *done;
    char  temp[120];
    char  currstr[120];
    char  rawstr[120];

    for (i = 0; i < rawdatasize; i++)
        rawdata[i] = 0.0;

    i = 0;

    while (1)
    {

        if (i > rawdatasize)
        {
            Error();
            printf("gen2spk: get_data: Raw data size %ld is too small!\n",
                   rawdatasize);
            return -1;
        }

        done = fgets(temp, 120, inputfp);

        if (done == NULL)
            return 0;  /* no more lines of input left */

        if (strstr(temp, "/newplot") != NULL)
        {
            /* found a /newplot, so quit; mark the end with -9999 */
            rawdata[i] = -9999.0;
            return 1;
        }
        else if (strstr(temp, "/plotname") != NULL)
        {
            sscanf(temp, "%*s %s", currstr);
            *current = Atod(currstr);
        }
        else if (temp[0] != '/')  /* Must be raw data. */
        {
            sscanf(temp, "%*s %s", rawstr);

            /*
             * Check for NaN or nan strings, and return an error if found.
             */

            if ((strcmp(rawstr, "nan") == 0)
                || (strcmp(rawstr, "NaN") == 0))
            {
                Error();
                printf("gen2spk: get_data: found NaN value in input; aborting...\n");
                return -1;
            }

            rawdata[i++] = Atod(rawstr);
        }
        /* Else it must be a random / command, so do nothing. */
    }
    /*NOTREACHED*/
}




int
calc_spikes(rawdata, SPK, firstpt, lastpt, dt)
    double *rawdata;
    double *SPK;
    int firstpt;
    int lastpt;
    double dt;
{
    /*
     *  Given the data record, this function calculates the time
     *  of each spike (relative to the current onset).
     *
     *  The algorithm is as follows:
     *
     *  1) Find the next sequence of increasing values.
     *  2) Find the next sequence of decreasing values after that.
     *  3) If the difference between the largest and the smallest
     *     value is less than 30 mV, go to step 1.
     *  4) Calculate the full width at half maximum of the peak; if it
     *     is less than 2 msec this is a spike; otherwise go to step 1.
     *  5) The threshold is the first point of the ascending sequence.
     *
     *  To get the ascending and descending sequences, the procedure is:
     *
     *  1) Get the longest stretch of monotonically increasing values.
     *  2) If the last point is greater than the next 5 points, it is the
     *     maximum of the sequence.  If not, add all points up to the
     *     largest point into the sequence and go to step 2 again.
     *  3) Repeat for descending sequence (using decreasing values).
     */

    int    i;
    int    more;
    int    next = firstpt;
    int    count = 0;
    double minup, mindown, min, max, avg;
    double maxtime;
    int    fwhmleft, fwhmright;  /* full width, half maximum points */
    double fwhm;
    double updown[UPDOWN_SIZE];  /* ascending and descending sequence */
    double rate = 1/dt;
    int    num_spikes;
    double spike_time[MAX_SPIKES];

    num_spikes = 0;
    fwhmleft = fwhmright = -1;

    while (next < lastpt)
    {
        /* load the next updown sequence */
        count = 0;
        fwhm = 0.0;

        for (i = 0; i < UPDOWN_SIZE; i++) updown[i] = 0.0;
        minup = updown[count++] = rawdata[next++];

        /* load the ascending sequence   */
        do
        {
            /* load monotonically ascending sequence */
            while (rawdata[next] >= rawdata[next - 1] && next <= lastpt)
                updown[count++] = rawdata[next++];
            max = updown[count - 1];  /* tentative maximum */

            /* find the highest point in the next 5 points */
            more = 0;

            for (i = 0; i < TEST_WINDOW; i++)
            {
                if (next + i <= lastpt)
                {
                    if (rawdata[next + i] >= max)
                    {
                        max = rawdata[next + i];
                        more = i + 1;
                    }
                }
            }

            /* incorporate up to the highest point into the sequence */

            for (i = 0; i < more; i++)
                updown[count++] = rawdata[next++];

            max = updown[count - 1];
        }
        while (more != 0); /* and repeat the whole thing over again! */

        maxtime = (next - firstpt - 1) / rate;

        /* load the descending sequence  */
        do
        {
            /* load monotonically descending sequence */
            while (rawdata[next] <= rawdata[next - 1] && next <= lastpt)
                updown[count++] = rawdata[next++];
            mindown = updown[count - 1];

            /* find the lowest point in the next 5 points */
            more = 0;

            for (i = 0; i < TEST_WINDOW; i++)
            {
                if (next + i <= lastpt)
                {
                    if (rawdata[next + i] <= mindown)
                    {
                        mindown = rawdata[next + i];
                        more = i + 1;
                    }
                }
            }

            /* Incorporate up to the highest point into the sequence. */

            for (i = 0; i < more; i++)
                updown[count++] = rawdata[next++];

            mindown = updown[count - 1];
        }
        while (more != 0);

        --next; /* have to start on last point of previous sequence */

        if (max - minup > 0.030 && max - mindown > 0.030)
        {
            /* A spike has probably occurred; to be sure,   */
            /* calculate full width at half maximum.        */

            min = (minup > mindown) ? minup : mindown;

            /* avg is the half-maximum point. */
            avg = 0.5 * (max - min) + min;

            for (i = 0; i < count; i++)
            {
                if ((updown[i] >= avg) && (updown[i - 1] < avg))
                    fwhmleft = i - 1;
            }

            for (i = fwhmleft; i < count; i++)
            {
                if ((updown[i] >= avg) && (updown[i + 1] < avg))
                    fwhmright = i + 1;
            }

            fwhm = (fwhmright - fwhmleft) / rate;

            if (fwhm <= MAX_SPIKE_WIDTH) /* a spike definitely occurred */
            {
                spike_time[num_spikes++] = maxtime;
            }
            else
            {
                /*
                  Error();
                  printf("gen2spk:calc_spikes: Whoa! "
                  "weird looking spike in trace! Check it out!\n");
                  return -1;
                */

                /*
                 * We're accepting weird spikes for the purposes of this
                 * parameter search.  It shouldn't hurt, unlike the
                 * premature return, which makes spkcmp bomb.  The "right
                 * way" to do this is to return an error value to genesis
                 * which is tested for and if present, spkcmp is never
                 * called.
                 */

                spike_time[num_spikes++] = maxtime;
            }
        }
    }

    for (i = 0; i < num_spikes; i++)
        SPK[i] = spike_time[i];

    return num_spikes;
}




int
calc_output(rawdata, rawdatasize, current, current_start,
	    current_duration, dt, accuracy)
    double *rawdata;
    long rawdatasize;
    double current;
    double current_start;
    double current_duration;
    double dt;
    short accuracy;
{
    /*
     *  FORMAT OF DATA RECORD:
     *
     *  input current (nA)
     *  (for each spike:)
     *  Time of spike, relative to current onset.
     */

    int     i;
    int     num_spikes;
    int     firstpt, lastpt;
    double  SPK[MAX_SPIKES];

    for (i = 0; i < MAX_SPIKES; i++)
        SPK[i] = 0.0;

    /*
     *  Have to recalculate rawdatasize from data:
     *  rawdatasize is the first point which equals -9999.
     */

    for (i = 0; i < rawdatasize; i++)
    {
        if (rawdata[i] < -1000.0)
        {
            rawdatasize = i;
            break;
        }
    }

    firstpt = (int)(current_start/dt);
    lastpt  = (int)((current_duration + current_start)/dt);

    num_spikes = calc_spikes(rawdata, SPK, firstpt, lastpt, dt);

    if (num_spikes < 0)
        return 0;

    sprintf(output_data[next_line++], "CURR\t\t%.4f", current);

    for (i = 0; i < num_spikes; i++)
    {
        if (accuracy == 0)
            sprintf(output_data[next_line++], "SPK\t\t%.4f", SPK[i]);
        else
            sprintf(output_data[next_line++], "SPK\t\t%.10f", SPK[i]);
    }

    sprintf(output_data[next_line++],
            "--------------------");  /* 20 dashes */

    return 1;
}




void
print_output(fp)
    FILE *fp;
{
    int i;

    for (i = 0; i < next_line; i++)
        fprintf(fp, "%s\n", output_data[i]);
}



/*
 * Gen2spk
 *
 * FUNCTION
 *     Takes a Vm file generated by genesis and outputs a standardized
 *     text file (ending in .spk.sim) which lists the times of occurrence
 *     of all spikes and the currents used.  For use in doing parameter
 *     searches on f/I curves.
 *
 * ARGUMENTS
 *     int    argc - number of command arguments
 *     char **argv - command arguments proper
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
Gen2spk(argc, argv)
    int argc;
    char **argv;
{
    FILE   *inputfp, *outputfp;
    double *rawdata, current, current_start, current_duration;
    double duration;
    double dt = 0.0001; /* default time step size of inputs */
    long   rawdatasize;
    int    anyleft;
    char   output_filename[256];
    int    status;
    short  accuracy = 0; /* flag for whether to output spike times
                          * with maximal accuracy */

    initopt(argc, argv,
            "datafile current-start current-duration total-duration -stepsize dt -maxaccuracy");

    while ((status = G_getopt(argc, argv)) == 1)
    {
        if (strcmp(G_optopt, "-stepsize") == 0)
            dt = Atod(optargv[1]);
        else if (strcmp(G_optopt, "-maxaccuracy") == 0)
            accuracy = 1;
    }

    if (status < 0)
    {
        printoptusage(argc, argv);
        return;
    }


    if ((argc < 5) || (argc > 8))
    {
        printoptusage(argc, argv);
        return;
    }

    inputfp = fopen(optargv[1], "r");

    if (inputfp == NULL)
    {
        Error();
        printf("gen2spk: Cannot open input file %s\n", optargv[1]);
        return;
    }

    strcpy(output_filename, optargv[1]);
    strcat(output_filename, ".spk.sim");

    outputfp = fopen(output_filename, "w");

    if (outputfp == NULL)
    {
        Error();
        printf("gen2spk: Cannot open output file %s\n", output_filename);
        return;
    }

    current_start    = Atod(optargv[2]);
    current_duration = Atod(optargv[3]);
    duration         = Atod(optargv[4]);

    rawdatasize = (long)(ceil(duration/dt)) + 1;

    rawdata = (double *) calloc(rawdatasize + 10, sizeof(double));

    if (rawdata == NULL)
    {
        Error();
        printf("gen2spk: cannot malloc %ld bytes.\n", rawdatasize);
        return;
    }

    strip_header(inputfp);
    clear_output_data();
    next_line = 0;
    anyleft   = 1;

    while (anyleft)
    {
        anyleft = get_data(inputfp, rawdatasize, rawdata, &current);

        if (anyleft < 0)
            return;

        if (!calc_output(rawdata, rawdatasize, current,
                         current_start, current_duration, dt, accuracy))
        {
            return;
        }

        print_output(outputfp);
        next_line = 0;
        clear_output_data();
    }

    fclose(inputfp);
    fclose(outputfp);
    free(rawdata);
}
