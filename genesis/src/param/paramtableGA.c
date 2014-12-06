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

/* typedef for comparison functions, to make qsort happy. */
typedef int (*comparison_func)();

/*
 * A simple utility function, used in get_crossover_locations.
 */

int
intcompare(i, j)
    int *i;
    int *j;
{
    return (*i - *j);
}




/*
 * utility functions for Param.
 */

#if defined(PARAM_DEBUG)
/* This prints a Param in bit representation.  Used for debugging. */

void
print_param(var, size)
    Param var;
    short size;
{
    int i;
    int length = 8 * size;

    if (size == 1)
    {
        for (i = (length - 1); i >= 0; i--)
        {
            if ((var.shortp >> i) & 1)
                printf("1");
            else
                printf("0");
        }
    }
    else if (size == 2)
    {
        for (i = (length - 1); i >= 0; i--)
        {
            if ((var.mediump >> i) & 1)
                printf("1");
            else
                printf("0");
        }
    }
    else if (size == 4)
    {
        for (i = (length - 1); i >= 0; i--)
        {
            if ((var.longp >> i) & 1)
                printf("1");
            else
                printf("0");
        }
    }
    else
    {
        Error();
        printf("paramtableGA: print_param: inadmissable size %d!\n", size);
        return;
    }

    printf(" ");
}
#endif  /* PARAM_DEBUG */




/*
 * This lets us use a user-defined type for Param, at the price of some ugly
 * and complicated code.
 */

Param
Param_long_to_Param(var, size)
    Param_long var;
    short size;
{
    Param temp;

    if (size == 1)
    {
        temp.shortp  = (Param_short)var;
    }
    else if (size == 2)
    {
        temp.mediump = (Param_medium)var;
    }
    else if (size == 4)
    {
        temp.longp   = (Param_long)var;
    }
    else
    {
        Error();
        printf("paramtableGA: Param_long_to_Param: inadmissable size %d!\n", size);
        temp.longp = (Param_long) 0;
    }

    return temp;
}




Param_long
Param_to_Param_long(var, size)
    Param var;
    short size;
{
    Param_long temp;

    if (size == 1)
    {
        temp = (Param_long)var.shortp;
    }
    else if (size == 2)
    {
        temp = (Param_long)var.mediump;
    }
    else if (size == 4)
    {
        temp = (Param_long)var.longp;
    }
    else
    {
        Error();
        printf("paramtableGA: Param_to_Param_long: inadmissable size %d!\n", size);
        return ((Param_long)0);
    }

    return temp;
}




double
Param_to_double(var, size)
    Param var;
    short size;
{
    double temp;

    if (size == 1)
    {
        temp = (double)var.shortp;
    }
    else if (size == 2)
    {
        temp = (double)var.mediump;
    }
    else if (size == 4)
    {
        temp = (double)var.longp;
    }
    else
    {
        Error();
        printf("paramtableGA: Param_to_double: inadmissable size %d!\n", size);
        return 0.0;
    }

    return temp;
}




/* Utility functions for use with qsort_ranks. */

/*
 * rankswap
 *
 * FUNCTION
 *     Swaps corresponding elements in rank and fit arrays.
 *
 * ARGUMENTS
 *     rank        -- array of ranks (to be sorted)
 *     fit         -- array of fitness values
 *                    (to be used as the basis for the sort)
 *     i, j        -- positions in array to swap
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
rankswap(rank, fit, i, j)
    int *rank;
    double *fit;
    int i;
    int j;
{
    double ftemp;
    int temp;

    temp    = rank[i];
    rank[i] = rank[j];
    rank[j] = temp;

    ftemp   = fit[i];
    fit[i]  = fit[j];
    fit[j]  = ftemp;
}




/*
 * fitcompare
 *
 * FUNCTION
 *     Compares two fitness values.
 *
 * ARGUMENTS
 *     a, b -- the two fitness values
 *
 * RETURN VALUE
 *     int
 *
 * AUTHOR
 *     Mike Vanier
 */

int
fitcompare(a, b)
    double a;
    double b;
{
    if (a > b)
        return 1;

    if (a < b)
        return -1;

    return 0;
}




/*
 * qsort_ranks
 *
 * FUNCTION
 *     Sorts a list of positions in a parameter table according to the
 *     fitness values: 0 = most fit.  Called by sortranks.
 *
 * ARGUMENTS
 *     rank        -- array of ranks (to be sorted)
 *     fit         -- array of fitness values
 *                    (to be used as the basis for the sort)
 *     left        -- leftmost element in the array to be sorted
 *     right       -- rightmost element in the array to be sorted
 *     fitcompare  -- pointer to comparison function
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier; largely stolen from Kernighan and Ritchie 2nd Ed.
 */

void
qsort_ranks(rank, fit, left, right, fitcompare)
    int *rank;
    double *fit;
    int left;
    int right;
    int (*fitcompare)();
{
    int i, last;

    if (left >= right)
        return;

    rankswap(rank, fit, left, (left + right) / 2);
    last = left;

    for (i = (left + 1); i <= right; i++)
    {
        if ((*fitcompare) (fit[i], fit[left]) > 0)
        {
            rankswap(rank, fit, ++last, i);
        }
    }

    rankswap(rank, fit, left, last);
    qsort_ranks(rank, fit, left,       (last - 1), fitcompare);
    qsort_ranks(rank, fit, (last + 1), right,      fitcompare);
}




/*
 * sortranks
 *
 * FUNCTION
 *     Sorts a list of positions in a parameter table according to the
 *     fitness values: 0 = most fit.
 *
 * ARGUMENTS
 *     element  -- pointer to paramtableGA element.
 *
 * RETURN VALUE
 *     void
 *
 * SIDE EFFECTS
 *     At the end of this function, the element->fitranks array is ordered
 *     so that the most fit index (the index of the table whose parameters
 *     gave the highest fitness value) is in fitranks[0], the next most fit
 *     is in fitranks[1], etc.
 *
 * AUTHOR
 *     Mike Vanier
 */

void
sortranks(element)
    struct paramtableGA_type *element;
{
    int i;

    for (i = 0; i < element->num_tables; i++)
        element->fitrank[i] = i;

    /*
     * This function uses the tempfitness array for its own purposes;
     * in other functions it has to be recalculated.
     */

    for (i = 0; i < element->num_tables; i++)
        element->tempfitness[i] = element->fitness[i];

    qsort_ranks(element->fitrank, element->tempfitness,
                0, (element->num_tables - 1), fitcompare);
}




/*
 * binary_to_gray_code
 *
 * FUNCTION
 *     Takes a binary-encoded parameter and transforms it into a
 *     Gray-code-encoded parameter.  Gray-code encoding is probably
 *     advantageous for genetic algorithms.  Gray codes are binary codes
 *     where adjacent numbers always differ by a single bit value only.
 *     See the book Numerical Recipes in C for a discussion of the algorithms
 *     for converting to and from Gray codes.  This algorithm is lifted from
 *     that book.
 *
 * ARGUMENTS
 *     param -- parameter value
 *
 * RETURN VALUE
 *     Param_long
 *
 * AUTHOR
 *     Mike Vanier
 */

Param_long
binary_to_gray_code(param)
    Param_long param;
{
    return param ^ (param >> 1);  /* Simple, huh? :-) */
}




/*
 * gray_code_to_binary
 *
 * FUNCTION
 *     Takes a Gray-code-encoded parameter and transforms it into a
 *     binary-encoded parameter.  See the book Numerical Recipes in C
 *     for a discussion of the algorithms for converting to and from
 *     Gray codes.  This algorithm is lifted from that book.
 *
 * ARGUMENTS
 *     param -- parameter value
 *
 * RETURN VALUE
 *     Param_long
 *
 * AUTHOR
 *     Mike Vanier
 */

Param_long
gray_code_to_binary(param)
    Param_long param;
{
    int ish;
    Param_long ans, idiv, maxshift;

    maxshift = (sizeof(Param_long) * 8) / 2;

    ish = 1;
    ans = param;

    for (;;)
    {
        ans ^= (idiv = (ans >> ish));

        if ((idiv <= ((Param_long) 1)) || (((Param_long)ish) == maxshift))
            return ans;

        ish <<= 1;
    }
}




/*
 * floating_point_to_bit_string
 *
 * FUNCTION
 *     Takes a parameter (double) and turns it into a value of type Param,
 *     with the appropriate scaling between the minimum and maximum values
 *     of that parameter (which depends on the type).
 *
 * ARGUMENTS
 *     par           -- double : the original parameter
 *     output        -- Param* : user-defined (see above)
 *     type          -- short  : 0 = additive, 1 = multiplicative
 *     center        -- double : the center value of the parameter
 *     range         -- double : the range of the parameter
 *     max_parameter -- what it says
 *     param_type    -- size of parameters in bytes
 *     gray          -- flag for whether we're using gray codes
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
floating_point_to_bit_string(par, output, type, center, range,
			     max_parameter, param_size, gray)
    double par;
    Param *output;
    short type;
    double center;
    double range;
    Param_long max_parameter;
    short param_size;
    short gray;
{
    double min, max;
    Param_long temp;

    if (type == 0) /* additive */
    {
        min = center - range;
        max = center + range;

        if (fabs(range) < TINY)
        {
            /*
             * If range is exactly equal to 0 we can't convert the parameter
             * so we set the bit string to 0.
             */

            temp = (Param_long) 0;
        }
        else
        {
            temp = (Param_long)((par - min) / (max - min) * max_parameter);
        }

        if (gray)
        {
            temp = binary_to_gray_code(temp);
        }

        *output = Param_long_to_Param(temp, param_size);
    }
    else                            /* multiplicative */
    {
        par /= center;              /* Scale the parameter to the
                                     * center value. */

        /*
         * What we want is (log(par) - log(1 / range))
         *                 / (log(range) - log(1/range)),
         * which equals 0.5 * (log(par) / log(range) + 1)
         */

        if (fabs(range - 1.0) < TINY)
        {
            /*
             * If range is exactly equal to 1.0 we can't convert the
             * parameter so we set the bit string to 0.  We assume that
             * range is not less than 1.0 (checked in the CHECK action of
             * paramtableGA).
             */

            temp = (Param_long) 0;
        }
        else
        {
            temp = (Param_long)((0.5 * (log(par) / log(range) + 1.0))
                                * max_parameter);
        }

        if (gray)
        {
            temp = binary_to_gray_code(temp);
        }

        *output = Param_long_to_Param(temp, param_size);
    }
}




/*
 * bit_string_to_floating_point
 *
 * FUNCTION
 *     Takes a value of type Param, with the appropriate scaling between
 *     the minimum and maximum values of that parameter (which depends on
 *     the type), and turns it into a double representing the "true"
 *     parameter.
 *
 * ARGUMENTS
 *     par           -- the parameter bit string
 *     output        -- the result
 *     type          -- flag: 0 = additive, 1 = multiplicative
 *     center        -- the center value of the parameter
 *     range         -- the range of the parameter
 *     max_parameter -- what it says
 *     param_size    -- size of Param in bytes
 *     gray          -- flag for whether we're using gray codes
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
bit_string_to_floating_point(par, output, type, center, range,
			     max_parameter, param_size, gray)
    Param par;
    double *output;
    short type;
    double center;
    double range;
    Param_long max_parameter;
    short param_size;
    short gray;
{
    double      min, max, dpar;
    Param_long  temp;

    /*
     * If the parameter is encoded using a Gray code, convert it to
     * regular binary encoding and then to a double.
     */

    if (gray)
    {
        temp = Param_to_Param_long(par, param_size);
        temp = gray_code_to_binary(temp);
        dpar = (double)temp;
    }
    else
    {
        dpar = Param_to_double(par, param_size);
    }

    if (type == 0)  /* additive */
    {
        min = center - range;
        max = center + range;

        if (fabs(range) < TINY)
        {
            *output = center;
        }
        else
        {
            *output = dpar / ((double)(max_parameter)) * (max - min) + min;
        }
    }
    else  /* multiplicative */
    {
        if (fabs(range - 1.0) < TINY)
        {
            *output = center;
        }
        else
        {
            *output = center * exp((dpar / ((double)max_parameter)
                                    * 2.0 - 1.0) * log(range));
        }
    }
}




/*
 * GetparamGA
 *
 * FUNCTION
 *     Gets a param value from a paramtableGA parameter table and returns a
 *     floating-point number.  Called by do_getparamGA and other functions.
 *
 * ARGUMENTS
 *     char *path  -- path of paramtableGA element
 *     int   table -- number of table in element
 *     int   param -- number of parameter
 *
 * RETURN VALUE
 *     double
 *
 * AUTHOR
 *     Mike Vanier
 */

double
GetparamGA(path, table, param)
    char *path;
    int table;
    int param;
{
    Param  par;
    double fpar;
    struct paramtableGA_type *element;

    element = (struct paramtableGA_type *)GetElement(path);

    if (!CheckClass(element, ClassID("param")))
    {
        Error();
        printf("getparamGA: element %s is not a parameter table object!\n", element->name);
        return 0.0;
    }

    if ((table < 0) || (table >= element->num_tables))
    {
        Error();
        printf("getparamGA: table index %d out of range!\n", table);
        return 0.0;
    }

    if (param < 0 || param >= element->num_params)
    {
        Error();
        printf("getparamGA: param index %d out of range!\n", param);
        return 0.0;
    }

    par = element->param[table][param];

    bit_string_to_floating_point(par,
                                 &fpar,
                                 element->type[param],
                                 element->center[param],
                                 element->range[param],
                                 element->max_parameter,
                                 element->param_size,
                                 element->use_gray_code);

    return fpar;
}




/*
 * do_getparamGA
 *
 * FUNCTION
 *     Gets a param value from a paramtableGA parameter table and returns a
 *     floating-point number.
 *
 * ARGUMENTS
 *     argc     -- number of command arguments
 *     argv     -- command arguments proper
 *
 * RETURN VALUE
 *     double
 *
 * AUTHOR
 *     Mike Vanier
 */

double
do_getparamGA(argc, argv)
    int argc;
    char **argv;
{
    int   table, param;
    char *path;

    initopt(argc, argv, "path table param");

    if (argc != 4)
    {
        printoptusage(argc, argv);
        return 0.0;
    }

    path  = optargv[1];
    table = atoi(optargv[2]);
    param = atoi(optargv[3]);

    return GetparamGA(path, table, param);
}




/*
 * do_setparamGA
 *
 * FUNCTION
 *     Takes a floating-point number and uses it to set a value in
 *     a paramtableGA parameter table.
 *
 * ARGUMENTS
 *     argc     -- number of command arguments
 *     argv     -- command arguments proper
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
do_setparamGA(argc, argv)
    int argc;
    char **argv;
{
    int     table, param;
    double  value;
    char   *path;
    struct  paramtableGA_type *element;
    Param   par;

    initopt(argc, argv, "path table param value");

    if (argc != 5)
    {
        printoptusage(argc, argv);
        return;
    }

    path  = optargv[1];
    table = atoi(optargv[2]);
    param = atoi(optargv[3]);
    value = Atod(optargv[4]);

    element = (struct paramtableGA_type *)GetElement(path);

    if (!CheckClass(element, ClassID("param")))
    {
        Error();
        printf("setparamGA: element %s is not a parameter table object!\n", element->name);
        return;
    }

    if ((table < 0) || (table >= element->num_tables))
    {
        Error();
        printf("setparamGA: table index %d out of range!\n", table);
        return;
    }

    if ((param < 0) || (param >= element->num_params))
    {
        Error();
        printf("setparamGA: param index %d out of range!\n", param);
        return;
    }

    floating_point_to_bit_string(value,
                                 &par,
                                 element->type[param],
                                 element->center[param],
                                 element->range[param],
                                 element->max_parameter,
                                 element->param_size,
                                 element->use_gray_code);

    element->param[table][param] = par;
}




/*
 * get_fitness_index
 *
 * FUNCTION
 *     Takes an array of normalized fitness values and selects an index
 *     based on fitness.
 *
 * ARGUMENTS
 *     cumulfitness -- array of doubles representing cumulative
 *                     normalized fitness values; cumulative means
 *                     they are monotonically increasing, and normalized
 *                     means their differences sum to one.
 *     size         -- size of array
 *
 * RETURN VALUE
 *     int -- The index if found; -1 if not found.
 *
 * AUTHOR
 *     Mike Vanier
 */

int
get_fitness_index(cumulfitness, size)
    double *cumulfitness;
    int size;
{
    int i;
    double test = urandom();

    if (cumulfitness[0] >= test)
    {
        return 0;
    }

    for (i = 1; i < size; i++)
    {
        if (cumulfitness[i] >= test && cumulfitness[i - 1] < test)
        {
            return i;
        }
    }

    return -1;
}




/*
 * bit_crossover
 *
 * FUNCTION
 *     Crosses over two parameters at the bit level.  They must be
 *     located at corresponding places on the chromosome and hence
 *     have the same type, center and range value.
 *
 * ARGUMENTS
 *     param1, param2        -- the two original parameters
 *     newparam1, newparam2  -- new values of parameters
 *     breakbit_within_param -- where to cross over
 *     bits_per_parameter    -- what it says
 *     max_parameter         -- maximum parameter value
 *     param_size            -- size of Param in bytes
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
bit_crossover(param1, param2, newparam1, newparam2, breakbit_within_param,
	      bits_per_parameter, max_parameter, param_size)
    Param param1, param2;
    Param *newparam1, *newparam2;
    int breakbit_within_param;
    int bits_per_parameter;
    Param_long max_parameter;
    short param_size;
{
    Param mask, temp1, temp2, temp3, temp4;

    /*
     * Note: breakbit_within_param ranges from 1 to (bits_per_parameter - 1)
     * since if it had been 0 this function would not have been called.
     */

    if ((breakbit_within_param < 1)
        || (breakbit_within_param > bits_per_parameter))
    {
        Error();
        printf("paramtableGA: bit_crossover: invalid breakbit_within_parameter.\n");
        return;
    }

    /* Cross over the bit strings. */

    if (param_size == 1)
    {
        mask.shortp = ((Param_short)max_parameter) >> breakbit_within_param;

        temp1.shortp = param1.shortp & ~mask.shortp;
        temp2.shortp = param2.shortp & ~mask.shortp;
        temp3.shortp = param1.shortp &  mask.shortp;
        temp4.shortp = param2.shortp &  mask.shortp;

        (*newparam1).shortp = temp1.shortp + temp4.shortp;
        (*newparam2).shortp = temp2.shortp + temp3.shortp;
    }
    else if (param_size == 2)
    {
        mask.mediump = ((Param_medium)max_parameter)
            >> breakbit_within_param;

        temp1.mediump = param1.mediump & ~mask.mediump;
        temp2.mediump = param2.mediump & ~mask.mediump;
        temp3.mediump = param1.mediump &  mask.mediump;
        temp4.mediump = param2.mediump &  mask.mediump;

        (*newparam1).mediump = temp1.mediump + temp4.mediump;
        (*newparam2).mediump = temp2.mediump + temp3.mediump;
    }
    else if (param_size == 4)
    {
        mask.longp = ((Param_long)max_parameter) >> breakbit_within_param;

        temp1.longp = param1.longp & ~mask.longp;
        temp2.longp = param2.longp & ~mask.longp;
        temp3.longp = param1.longp &  mask.longp;
        temp4.longp = param2.longp &  mask.longp;

        (*newparam1).longp = temp1.longp + temp4.longp;
        (*newparam2).longp = temp2.longp + temp3.longp;
    }
}




/*
 * select_crossover_indices
 *
 * FUNCTION
 *     Calculates the indices of two unselected parameter tables and
 *     updates array of available indices.  Used in performing crossovers.
 *
 * ARGUMENTS
 *     selectindex      -- array of indices of parameter tables available
 *                         for selection.
 *     last             -- last usable entry in selectindex array + 1
 *     crossover_index1 -- address of 1st selected index.
 *     crossover_index2 -- address of 2nd selected index.
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
select_crossover_indices(selectindex, last, crossover_index1, crossover_index2)
    int *selectindex;
    int *last;
    int *crossover_index1;
    int *crossover_index2;
{
    int index;

    /* Select first index, between 0 and (*last - 1) */

    index = (int)(frandom(0.0, ((float)(*last))));
    *crossover_index1 = selectindex[index];

    if (*crossover_index1 < 0)
    {
        Error();
        printf("paramtableGA: select_crossover_indices: invalid index selected!\n");
        return;
    }

    /*
     * Move last entry into selected location and put a -1 into last entry
     * position.  Note that this works even if the last entry was selected.
     */

    selectindex[index] = selectindex[(*last) - 1];
    selectindex[(*last) - 1] = -1;
    (*last)--;

    /* Select second index. */

    index = (int)(frandom(0.0, ((float)(*last))));
    *crossover_index2 = selectindex[index];

    if (*crossover_index2 < 0)
    {
        Error();
        printf("paramtableGA: select_crossover_indices: invalid index selected!\n");
        return;
    }

    selectindex[index] = selectindex[(*last) - 1];
    selectindex[(*last) - 1] = -1;
    (*last)--;
}




/*
 * get_crossover_locations
 *
 * FUNCTION
 *     Takes a crossover type specifier and a number representing the total
 *     or the average number of crossovers and returns a
 *     crossover_locations struct which contains the number and bit
 *     positions of all crossover points.  This is needed in order to do
 *     multiple crossovers between two bit strings.
 *
 * ARGUMENTS
 *     type        -- type of crossover (see below).
 *     number      -- number of crossovers (see below).
 *     num_params  -- number of parameters in the string.
 *     param_size  -- size of the parameters in bytes.
 *     break_param -- flag: if 1, we can choose locations inside a parameter.
 *
 * RETURN VALUE
 *     Crossover_locations* -- pointer to a structure consisting of an array
 *                             of longs representing the locations of the
 *                             crossovers and the number of crossovers.
 *                             The locations are in ascending order.
 *                             Return NULL if the function fails.
 *
 * SIDE EFFECTS
 *     A crossover_locations object is malloc'ed and it must be freed by the
 *     caller.
 *
 * AUTHOR
 *     Mike Vanier
 */

Crossover_locations *
get_crossover_locations(type, number, num_params, param_size, break_param)
    short type;
    int number;
    int num_params;
    short param_size;
    short break_param;
{
    Crossover_locations *loc;
    long   *temp;                /* Where the potential crossover
                                  * positions are stored. */
    int     max_crossovers;      /* The maximum possible number
                                  * of crossovers. */
    int     crossover_count;
    int     testcount;
    int     param_length;        /* Length of parameter in bits. */
    int     i;
    long    size;
    double  probability;         /* ... of crossing over at any point
                                  * on the bit string. */

    /*
     * NOTE: the <probability> variable shouldn't be confused with the
     *       <crossover_probability> field of the paramtableGA object.
     *       That field is the probability that any given parameter
     *       table will recombine; this variable is the probability that,
     *       _given_ that recombination is going to happen, it'll happen
     *       between any two adjacent bits on the bit string.  The
     *       <probability> variable is only used for type 1 recombination.
     */

    /* Check inputs. */

    param_length = 8 * param_size;
    size = param_length * num_params;

    if (size < 0)
    {
        Error();
        printf("paramtableGA: get_crossover_locations: size must be >= 0!\n");
        return NULL;
    }

    if (break_param)
        max_crossovers = size - 1;
    else
        max_crossovers = num_params - 1;

    if ((number < 0) || (number > max_crossovers))
    {
        Error();
        printf("paramtableGA: get_crossover_locations: number %d must be >= 0 and < %d!\n",
               number, max_crossovers);
        return NULL;
    }


    loc = (Crossover_locations *)malloc(sizeof(Crossover_locations));
    loc->number   = 0;
    loc->location = NULL;


    /*
     * <temp> is the array where the potential crossover points are stored.
     * Since the crossover points are numbered from 1 to <size> by
     * convention, and a point where no crossover occurs is indicated by a
     * -1 in the temp array, we build an array of size <size> and we don't
     * use the first element (so we set it to -1).  This is a bit of a
     * hack.  Each element of temp holds its position to start with (since
     * all crossovers are possible).
     */

    temp = (long *)calloc(size, sizeof(long));

    if (break_param)
    {
        temp[0] = -1;

        for (i = 1; i < size; i++)
            temp[i] = i;
    }
    else
    {
        /*
         * If we can't cross over inside parameters, we set all
         * the possible crossover positions to -1 except those
         * that are exact multiples of the parameter size in bits.
         */

        for (i = 0; i < size; i++)
        {
            if (i % param_length == 0)
                temp[i] = i;
            else
                temp[i] = -1;
        }
    }

    /*
     * Types of recombination:
     *
     * 0: choose exactly <number> crossover points for all crossovers.
     * 1: choose an average of <number> crossover points for all crossovers.
     *
     */

    switch (type)
    {
    case 0:
        /*
         * This case requires us to select exactly <number> places for
         * crossovers.  We do this by randomly picking points in the temp
         * array until we have <number> distinct places.
         */

        loc->number     = number;
        loc->location   = (long *)calloc(number, sizeof(long));
        crossover_count = 0;

        while (crossover_count < number)
        {
            int position;

            if (break_param)
            {
                /*
                 * This gives a uniform probability of picking any integer
                 * in the range [1, max_crossovers].
                 */

                position = (int)(urandom() * max_crossovers) + 1;
            }
            else
            {
                /*
                 * If we can't break a parameter then all crossover
                 * positions must be exact multiples of the size of a
                 * parameter in bits.
                 */

                position = ((int)(urandom() * max_crossovers) + 1)
                    * param_length;
            }

            /*
             * A value of -1 means that <position> has already been
             * selected (or is unavailable e.g. if <break_param> is 0 and
             * it's not a multiple of the parameter length in bits).  If
             * so, walk up the temp array until you find a position that
             * hasn't been selected.  If you get to the end, go back to the
             * beginning and continue.
             */

            while (temp[position] == -1)
            {
                if (position == (max_crossovers - 1))
                    position = 1; /* The first possible crossover point. */
                else
                    position++;
            }

            loc->location[crossover_count++] = temp[position];
            temp[position] = -1;
        }

        /*
         * Now we have to sort the loc->location array.
         */

        qsort((void *)loc->location, loc->number, sizeof(long),
              (comparison_func)intcompare);
        break;

    case 1:
        /*
         * This case requires us to test each possible crossover location
         * randomly for whether a crossover occurs there.  <number> is used
         * to generate <probability> (<number> represents the mean of a
         * binomial distribution).  The temp array already contains all
         * possible crossover positions (see above).  Pick a random number
         * for each position.  If it's less than <probability> that means
         * there is a crossover at that position; increment
         * <crossover_count>.  Otherwise put -1 at that position (-1 means
         * "no crossover here").  Finally, create the location array and
         * fill it with the entries (in order) that aren't equal to -1.
         */

        probability = ((double)number) / ((double)max_crossovers);

#if defined(PARAM_DEBUG)
        printf("paramtableGA: get_crossover_location: probability = %g\n", probability);
        printf("paramtableGA: get_crossover_location: number = %ld\n", number);
        printf("paramtableGA: get_crossover_location: max_crossovers = %d\n", max_crossovers);
#endif

        crossover_count = 0;

        if (break_param)
        {
            for (i = 1; i < size; i++)
            {
                if (urandom() >= probability)
                    temp[i] = -1;
                else
                    crossover_count++;
            }
        }
        else
        {
            for (i = param_length; i < size; i += param_length)
            {
                if (urandom() >= probability)
                    temp[i] = -1;
                else
                    crossover_count++;
            }
        }

        loc->number   = crossover_count;
        loc->location = (long *)calloc(crossover_count, sizeof(long));
        testcount     = 0;

        /*
         * Note that loc->location is automatically sorted in ascending
         * order.
         */

        for (i = 1; i < size; i++)
        {
            if (temp[i] != -1)
                loc->location[testcount++] = temp[i];
        }

        assert(crossover_count == testcount);
        break;

    default:
        Error();
        printf("paramtableGA: get_crossover_location: invalid crossover type: %d\n", type);
        free(loc);
        return NULL;
    }

#if defined (PARAM_DEBUG)
    printf("paramtableGA: get_crossover_location: crossovers at: ");

    for (i = 0; i < loc->number; i++)
        printf("%ld ", loc->location[i]);

    printf("\n");
#endif

    free(temp);
    return loc;
}




/*
 * perform_crossover
 *
 * FUNCTION
 *     Takes two selected parameter tables from a temporary array,
 *     crosses them over, and inserts them into a permanent array.
 *     Multiple crossovers are possible given some parameter settings.
 *
 * ARGUMENTS
 *     param              -- permanent parameter array (to be copied to).
 *     temp               -- temporary array (to be copied from).
 *     num_tables         -- number of tables in the arrays.
 *     num_params         -- number of parameters per table.
 *     index1             -- 1st index from temp array to cross over.
 *     index2             -- 2nd index from temp array to cross over.
 *     position           -- place to start copying into.
 *     bits_per_parameter -- number of bits per parameter.
 *     max_parameter      -- maximum parameter value.
 *     param_size         -- size of Param in bytes.
 *     crossover_type     -- type of crossover (see below).
 *     crossover_number   -- number of crossovers.
 *     break_param        -- flag: if 1 we can cross over within a parameter
 *
 * RETURN VALUE
 *     void
 *
 * SIDE EFFECTS
 *     param[position] and param[position+1] are filled with the new
 *     tables resulting from the crossover.  temp[index1] and temp[index2]
 *     are not used after this function is called.
 *
 * AUTHOR
 *     Mike Vanier
 */

void
perform_crossover(param, temp, num_tables, num_params, index1, index2,
		  position, bits_per_parameter, max_parameter, param_size,
		  crossover_type, crossover_number, break_param)
    Param **param;
    Param **temp;
    int num_tables;
    int num_params;
    int index1;
    int index2;
    int position;
    int bits_per_parameter;
    Param_long max_parameter;
    short param_size;
    short crossover_type;
    int crossover_number;
    short break_param;
{
    int   i, num, lastbit, size, breakbit, breakbit_within_param, breakparam;
    Param newparam1, newparam2;
    Crossover_locations *loc;

    if ((num_tables - 1) < (position + 1))
    {
        Error();
        printf("paramtableGA: perform_crossover: can't insert crossovers into param array.\n");
        return;
    }

    /*
     * <size> is the length of the parameter string.
     * <lastbit> is the last bit position in the parameter string.
     */

    size    = bits_per_parameter * num_params;
    lastbit = size - 1;

    if (crossover_number > lastbit)
    {
        Error();
        printf("paramtableGA: perform_crossover: crossover_number %d is too high!\n", crossover_number);
        return;
    }


    /*
     * Get the locations where crossing over occurs.
     * FIXME! This will only work for types 0 and 1 recombination;
     *        uniform crossover will require more work.
     */


#if defined (PARAM_DEBUG)
    printf("paramtableGA: perform_crossover: crossover_type = %d\n", crossover_type);
#endif

    loc = get_crossover_locations(crossover_type, crossover_number,
                                  num_params, param_size, break_param);
    assert(loc->number >= 0);

    /*
     * Cross over the bit strings <loc->number> times.  For each crossover,
     * the tables in temp are copied to the tables in param.  At the end,
     * if there are any more crossovers, the tables in param are copied
     * back to temp; otherwise they stay in param.
     */

#if defined (PARAM_DEBUG)
    /*
     * Print the parameter string as a bit string.
     */

    printf("Number of crossovers: %d\n", loc->number);
    printf("Crossovers at: ");

    for (i = 0; i < loc->number; i++)
        printf("%ld ", loc->location[i]);

    printf("\n\n");

    printf("temp[index1]: ");

    for (i = 0; i < num_params; i++)
        print_param(temp[index1][i], param_size);

    printf("\n\n");

    printf("temp[index2]: ");

    for (i = 0; i < num_params; i++)
        print_param(temp[index2][i], param_size);

    printf("\n\n");
#endif

    /*
     * If there are no crossovers just copy the temp tables to the
     * target param tables.  Otherwise do the crossovers.
     */

    if (loc->number == 0)
    {
        for (i = 0; i < num_params; i++)
        {
            param[position][i]   = temp[index1][i];
            param[position+1][i] = temp[index2][i];
        }
    }
    else
    {
        for (num = 0; num < loc->number; num++)
        {
            /*
             * <breakbit> is the first bit after the crossover point. It
             * can range from 1 to <lastbit>.
             */

            breakbit = loc->location[num];

            /*
             * Does the crossover occur between parameters?  If so, copy
             * the appropriate parameters to their destinations.  If not,
             * we have to cross over within the parameter.
             */

            /*
             * Calculate where the breakpoint is within the broken
             * parameter.
             */

            breakbit_within_param = breakbit % bits_per_parameter;
            breakparam = breakbit / bits_per_parameter;

            if (breakbit_within_param == 0)
            {
                /* crossover occurred between params */

                for (i = 0; i < breakparam; i++)
                {
                    param[position][i]   = temp[index1][i];
                    param[position+1][i] = temp[index2][i];
                }

                for (i = breakparam; i < num_params; i++)
                {
                    param[position][i]   = temp[index2][i];
                    param[position+1][i] = temp[index1][i];
                }
            }
            else
            {
                /* crossover occured within a parameter;
                 * the usual situation */

                /* Cross over parameters not including the broken one. */

                for (i = 0; i < breakparam; i++)
                {
                    param[position][i]   = temp[index1][i];
                    param[position+1][i] = temp[index2][i];
                }

                for (i = (breakparam + 1); i < num_params; i++)
                {
                    param[position][i]   = temp[index2][i];
                    param[position+1][i] = temp[index1][i];
                }

                /* Now cross over the broken one. */

#if defined (PARAM_DEBUG)
                printf("paramtableGA: perform_crossover: breakparam = %d\n", breakparam);
		printf("paramtableGA: perform_crossover: breakbit_within_param = %d\n",
                       breakbit_within_param);
                printf("paramtableGA: perform_crossover: temp[index1][breakparam]: ");
                print_param(temp[index1][breakparam], param_size);
                printf("\n");
                printf("paramtableGA: perform_crossover: temp[index2][breakparam]: ");
                print_param(temp[index2][breakparam], param_size);
                printf("\n\n");
#endif
                bit_crossover(temp[index1][breakparam],
                              temp[index2][breakparam],
                              &newparam1,
                              &newparam2,
                              breakbit_within_param,
                              bits_per_parameter,
                              max_parameter,
                              param_size);

#if defined (PARAM_DEBUG)
                printf("paramtableGA: perform_crossover: newparam1: ");
                print_param(newparam1, param_size);
                printf("\n");
                printf("paramtableGA: perform_crossover: newparam2: ");
                print_param(newparam2, param_size);
                printf("\n\n");
#endif
                param[position][breakparam]   = newparam1;
                param[position+1][breakparam] = newparam2;
            }

#if defined (PARAM_DEBUG)
            printf("param[position]: ");

            for (i = 0; i < num_params; i++)
                print_param(param[position][i], param_size);

            printf("\n\n");

            printf("param[position+1]: ");

            for (i = 0; i < num_params; i++)
                print_param(param[position+1][i], param_size);

            printf("\n\n");
#endif

            /*
             * If we're not done, copy the parameters from param back to
             * temp so we can do the next crossover.  When <num> is equal
             * to (<loc->number> - 1) this is the last crossover.
             */

            if (num < (loc->number - 1))
            {
                for (i = 0; i < num_params; i++)
                {
                    temp[index1][i] = param[position][i];
                    temp[index2][i] = param[position+1][i];
                }
            }
        }
    }

    /*
     * Free the <loc> structure which was malloc'ed in
     * get_crossover_locations().
     */

    if (loc->location != NULL)
        free(loc->location);

    free(loc);
}




/*
 * mutate_parameter
 *
 * FUNCTION
 *     Takes a parameter and decides which bits if any to flip.
 *
 * ARGUMENTS
 *     param              -- number to be modified.
 *     prob               -- average mutation probability per bit
 *     bits_per_parameter -- what it says
 *     param_size         -- size of Param in bytes
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void
mutate_parameter(param, prob, bits_per_parameter, param_size)
    Param *param;
    double prob;
    int bits_per_parameter;
    short param_size;
{
    int i;
    Param mask;

    /*
     * Calculate the probability that each particular bit is flipped;
     * the probability that any bit is flipped will equal prob.
     */

    if (param_size == 1)
    {
        for (i = 0; i < bits_per_parameter; i++)
        {
            if (urandom() < prob) /* flip the bit */
            {
                mask.shortp = 1 << i;
                (*param).shortp = ((*param).shortp) ^ (mask.shortp);
            }
        }
    }
    else if (param_size == 2)
    {
        for (i = 0; i < bits_per_parameter; i++)
        {
            if (urandom() < prob) /* flip the bit */
            {
                mask.mediump = 1 << i;
                (*param).mediump = ((*param).mediump) ^ (mask.mediump);
            }
        }
    }
    else if (param_size == 4)
    {
        for (i = 0; i < bits_per_parameter; i++)
        {
            if (urandom() < prob) /* flip the bit */
            {
                mask.longp = 1 << i;
                (*param).longp = ((*param).longp) ^ (mask.longp);
            }
        }
    }
}




/*
 * The following functions are action functions for paramtableGA,
 * made into separate functions for convenience in programming and
 * modification.
 */

/*
 * ParamtableGA_TABCREATE
 *
 * FUNCTION
 *     Allocates memory for parameter tables.
 *
 * ARGUMENTS
 *     element    -- address of element
 *     num_tables -- number of tables
 *     num_params -- number of parameters per table
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableGA_TABCREATE(element, num_tables, num_params)
    struct paramtableGA_type *element;
    int num_tables;
    int num_params;
{
    int i;
    short param_size = element->param_size;

    if (num_tables < 0)
    {
        Error();
        printf("paramtableGA: TABCREATE: number of tables must be >= 0!\n");
        return 0;
    }
    else
    {
        element->num_tables = num_tables;
    }

    if (num_params < 0)
    {
        Error();
        printf("paramtableGA: TABCREATE: number of parameters must be >= 0!\n");
        return 0;
    }
    else
    {
        element->num_params = num_params;
    }

    if ((param_size != 1) && (param_size != 2) && (param_size != 4))
    {
        Error();
        printf("paramtableGA: TABCREATE: param_size must be 1, 2, or 4!\n");
        return 0;
    }

    element->bits_per_parameter = (short)(param_size * 8);
    element->max_parameter      = (Param_long)(pow(2.0,
                                  (double)element->bits_per_parameter)) - 1;

    if (IsSilent() < 1)
    {
        printf("Creating %d tables with %d parameters each.\n",
               element->num_tables, element->num_params);
        printf("Using %d-byte parameters i.e. %d bits per parameter.\n",
               element->param_size, element->bits_per_parameter);
    }

    element->search = (short *) calloc(element->num_params, sizeof(short));
    element->type   = (short *) calloc(element->num_params, sizeof(short));
    element->center = (double *)calloc(element->num_params, sizeof(double));
    element->range  = (double *)calloc(element->num_params, sizeof(double));
    element->best   = (double *)calloc(element->num_params, sizeof(double));

    /*
     * Create array of label strings for parameters.  Allocate an extra
     * character for the terminating null character of the string.
     */

    element->label = (char **)calloc(num_params, sizeof(char *));

    for (i = 0; i < num_params; i++)
    {
        element->label[i] = (char *)calloc((LABEL_SIZE + 1), sizeof(char));
    }


    element->param = (Param **)calloc(element->num_tables, sizeof(Param *));

    for (i = 0; i < element->num_tables; i++)
    {
        element->param[i] = (Param *)calloc(element->num_params,
                                            sizeof(Param));
    }


    element->temp = (Param **)calloc(element->num_tables,
                                     sizeof(Param *));

    for (i = 0; i < element->num_tables; i++)
    {
        element->temp[i] = (Param *)calloc(element->num_params,
                                           sizeof(Param));
    }

    element->fitness      = (double *)calloc(element->num_tables,
                                             sizeof(double));
    element->tempfitness  = (double *)calloc(element->num_tables,
                                             sizeof(double));
    element->fitrank      = (int *)   calloc(element->num_tables,
                                             sizeof(int));
    element->normfitness  = (double *)calloc(element->num_tables,
                                             sizeof(double));
    element->cumulfitness = (double *)calloc(element->num_tables,
                                             sizeof(double));
    element->selectindex  = (int *)   calloc(element->num_tables,
                                             sizeof(int));
    element->alloced = 1;

    return 1;
}




/*
 * ParamtableGA_TABDELETE
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
ParamtableGA_TABDELETE(element)
    struct paramtableGA_type *element;
{
    int i;

    if (!element->alloced)
        return;

    paramFree(element->search);
    paramFree(element->type);
    paramFree(element->center);
    paramFree(element->range);
    paramFree(element->best);

    for (i = 0; i < element->num_params; i++)
    {
        paramFree(element->label[i]);
    }

    paramFree(element->label);

    for (i = 0; i < element->num_tables; i++)
    {
        paramFree(element->param[i]);
        paramFree(element->temp[i]);
    }

    paramFree(element->param);
    paramFree(element->temp);

    paramFree(element->fitness);
    paramFree(element->tempfitness);
    paramFree(element->fitrank);
    paramFree(element->normfitness);
    paramFree(element->cumulfitness);
    paramFree(element->selectindex);

    element->alloced = 0;
}




/*
 * ParamtableGA_RANDOMIZE
 *
 * FUNCTION
 *     Randomizes parameters in tables.
 *
 * ARGUMENTS
 *     element         -- address of element
 *     randomize_first -- flag: 1 means randomize first table
 *                        as well as all others; 0 means
 *                        don't randomize the first table
 *                        but instead use the starting parameter
 *                        values for all parameters as the first
 *                        table.
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableGA_RANDOMIZE(element, randomize_first)
    struct paramtableGA_type *element;
    short randomize_first;
{
    int i, j;

    if (!element->alloced)
    {
        Error();
        printf("paramtableGA: RANDOMIZE: tables must be allocated before randomization.\n");
        return 0;
    }

    for (i = 0; i < element->num_params; i++)
    {
        if ((element->range[i] < 0.0) && (element->type[i] == 0))
        {
            Error();
            printf("paramtableGA: RANDOMIZE: parameter limits %d are bogus.\n", i);
            printf("range[%d] must be >= 0!\n", i);
            return 0;
        }
        else if ((element->range[i] < 1.0) && (element->type[i] == 1))
        {
            Error();
            printf("paramtableGA: RANDOMIZE: parameter limits %d are bogus.\n", i);
            printf("range[%d] must be >= 1.0!\n", i);
            return 0;
        }
    }

    for (i = 0; i < element->num_tables; i++)
    {
        for (j = 0; j < element->num_params; j++)
        {
            double center, range, min, max, param;

            if (element->type[j] == 0)  /* additive parameter */
            {
                center = element->center[j];
                range  = element->range[j];
                min    = center - range;
                max    = center + range;

                if ((element->search[j] == 0) ||
                    ((i == 0) && (!randomize_first)))
                {
                    /* Don't vary this parameter. */
                    param = center;
                }
                else
                {
                    param = frandom(min, max);
                }

                floating_point_to_bit_string(param,
                                             &element->param[i][j],
                                             0,
                                             center,
                                             range,
                                             element->max_parameter,
                                             element->param_size,
                                             element->use_gray_code);
            }
            else /* multiplicative parameter */
            {
                /*
                 * This procedure makes numbers between 1 and range as
                 * likely as numbers between 1/range and 1.
                 */

                center = element->center[j];
                range  = element->range[j];

                if ((element->search[j] == 0) ||
                    ((i == 0) && (!randomize_first)))
                {
                    /* Don't vary this parameter. */
                    param = center;
                }
                else
                {
                    param = center * exp(frandom(-log(range), log(range)));
                }

                floating_point_to_bit_string(param,
                                             &element->param[i][j],
                                             1,
                                             center,
                                             range,
                                             element->max_parameter,
                                             element->param_size,
                                             element->use_gray_code);
            }
        }
    }

    return 1;
}




/*
 * ParamtableGA_REPRODUCE
 *
 * FUNCTION
 *     Performs fitness-proportional reproduction on the parameter table.
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
ParamtableGA_REPRODUCE(element)
    struct paramtableGA_type *element;
{
    int    i, j;
    double fitsum, cumulfit;
    int    fitindex, preservenum;

    /*
     * Calculate the rank of all tables according to their fitnesses.  This
     * fills the fitrank table with the numbers of the tables arranged in
     * order of fitness (0 = best).
     */

    sortranks(element);

    /*
     * Copy all the parameters to the temp arrays.
     */

    for (i = 0; i < element->num_tables; i++)
    {
        element->tempfitness[i] = element->fitness[i];

        for (j = 0; j < element->num_params; j++)
            element->temp[i][j] = element->param[i][j];
    }


    /*
     * preservenum is the number of best tables to copy unaltered.
     * Use this to ensure that your best tables don't get lost due
     * to random chance.
     */

    preservenum = element->preserve;


    /*
     * Then calculate the normalized fitness values and the cumulative
     * normalized fitness values.
     */

    fitsum = 0.0;

    for (i = 0; i < element->num_tables; i++)
        fitsum += element->fitness[i];


    /* Check that fitsum is a positive number. */

    if (fitsum <= TINY)
    {
        Error();
        printf("paramtableGA: REPRODUCE: sum of fitness values must be a positive number!\n");
        return 0;
    }

    for (i = 0; i < element->num_tables; i++)
        element->normfitness[i] = element->fitness[i] / fitsum;

    cumulfit = 0.0;

    for (i = 0; i < element->num_tables; i++)
    {
        cumulfit += element->normfitness[i];
        element->cumulfitness[i] = cumulfit;
    }

    if (fabs(cumulfit - 1.0) > TINY)
    {
        Error();
        printf("paramtableGA: REPRODUCE: normalized fitness values do not sum to 1.0!\n");
        return 0;
    }

    /*
     * Now use the cumulative fitness values to choose new parameters.  We
     * are selecting one of the old param tables based on fitness to fill
     * the slots of all the new tables.  We first select the "preserved"
     * slots (i.e. the best ones, whose indices are in the lower end of the
     * fitrank array) and copy them unaltered.
     */

    for (i = 0; i < preservenum; i++)
    {
        for (j = 0; j < element->num_params; j++)
            element->param[i][j] = element->temp[element->fitrank[i]][j];

        element->fitness[i] = element->tempfitness[element->fitrank[i]];
    }

    for (i = preservenum; i < element->num_tables; i++)
    {
        fitindex = get_fitness_index(element->cumulfitness,
                                     element->num_tables);

        if (fitindex == -1)  /* This should never happen. */
        {
            Error();
            printf("paramtableGA: REPRODUCE: could not select fitness index.\n");
            return 0;
        }

        for (j = 0; j < element->num_params; j++)
            element->param[i][j] = element->temp[fitindex][j];

        /*
         * Now copy the fitness values from the temporary array.
         */

        element->fitness[i] = element->tempfitness[fitindex];
    }

    for (i = 0; i < element->num_tables; i++)
    {
        /*
         * Zero the normfitness and cumulfitness arrays; these have to
         * be recalculated.  We don't do this above since we need the
         * cumulative fitness values to calculate the new parameter sets.
         */

        element->normfitness[i]  = 0.0;
        element->cumulfitness[i] = 0.0;
    }

    return 1;
}




/*
 * ParamtableGA_CROSSOVER
 *
 * FUNCTION
 *     Performs crossover of parameter values in tables.
 *
 * ARGUMENTS
 *     element -- address of element
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * NOTES
 *     This function will only work properly if the REPRODUCE action
 *     has been called before this one, because it takes into account
 *     tables which are supposed to be preserved i.e. it assumes that
 *     the first preservenum tables are not to be touched.  If
 *     preservenum is zero it should always work properly.
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableGA_CROSSOVER(element)
    struct paramtableGA_type *element;
{
    int i, j;
    int num_crossovers, last, preservenum;
    int crossover_index1, crossover_index2;

    if ((element->crossover_probability > 1.0) ||
        (element->crossover_probability < 0.0))
    {
        Error();
        printf("paramtableGA: CROSSOVER: crossover probability must be between 0.0 and 1.0!\n");
        return 0;
    }

    /* preservenum is the number of best tables to leave unaltered. */

    preservenum = element->preserve;

    /*
     * Load selectindex array with table indices ordered according to
     * fitness.
     */

    sortranks(element);

    for (i = 0; i < element->num_tables; i++)
    {
        /* All indices are available. */
        element->selectindex[i] = element->fitrank[i];
    }

    /*
     * last is the last available position in selectindex array.  Actually
     * one more, since this will be used to select random integers up to
     * but not including last.  We won't use the last preservenum entries
     * (i.e. the tables with the lowest fitness values) since we have an
     * extra preservenum entries that were already put into the parameter
     * array as a result of the reproduction stage, so we put a -1 in the
     * selectindex array there.
     */

    last = element->num_tables - preservenum;

    for (i = last; i < element->num_tables; i++)
        element->selectindex[i] = -1;

    /* Copy parameters to temp array. */

    for (i = 0; i < element->num_tables; i++)
        for (j = 0; j < element->num_params; j++)
            element->temp[i][j] = element->param[i][j];

    /*
     * Calculate the number of crossovers to perform.  This is a proportion
     * of the total number of tables less the "preserved" tables.  We
     * divide crossover_probability by 2.0 since each crossover crosses
     * over two tables; the probability represents the probability that any
     * given table gets crossed over.
     */

    num_crossovers = (int)((element->num_tables - preservenum)
                           * element->crossover_probability / 2.0);

    /* Do the crossovers. */

    for (i = 0; i < num_crossovers; i++)
    {
        int entrypos = 2 * i + preservenum;

        /*
         * Select two indices of tables to be crossed over.  At the end of
         * this, last is the index of the last unselected parameter table +
         * 1.  All entries in the selectindex array at positions >= last
         * are equal to -1.  Every time select_crossover_indices is called,
         * last goes down by 2.
         */

        select_crossover_indices(element->selectindex, &last,
                                 &crossover_index1, &crossover_index2);

        /*
         * Now perform the crossover, taking the old tables from the temp
         * array and copying the new tables to the parameter array.  The
         * "entrypos" argument is the position where we insert the new
         * tables into the param array.
         */

        perform_crossover(element->param,
                          element->temp,
                          element->num_tables,
                          element->num_params,
                          crossover_index1,
                          crossover_index2,
                          entrypos,
                          (int)element->bits_per_parameter,
                          element->max_parameter,
                          element->param_size,
                          element->crossover_type,
                          element->crossover_number,
                          element->crossover_break_param);
    }

    /*
     * Copy the uncrossed-over parameters from the temp array to the param
     * array.  The indices of these tables are in the selectindex array.
     * last is the number of parameter tables to be copied.
     */

    for (i = 0; i < last; i++)
    {
        int k, index;

        /* Fill in places after crossovers and preserved parameter
         * tables. */

        k = preservenum + num_crossovers * 2 + i;
        index = element->selectindex[i];

        for (j = 0; j < element->num_params; j++)
            element->param[k][j] = element->temp[index][j];
    }

    return 1;
}




/*
 * ParamtableGA_MUTATE
 *
 * FUNCTION
 *     Performs mutation of parameter values in tables.
 *
 * ARGUMENTS
 *     element   -- address of element
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * NOTES
 *     This function will only work properly if the REPRODUCE action
 *     has been called before this one, because it takes into account
 *     tables which are supposed to be preserved i.e. it assumes that
 *     the first preservenum tables are not to be touched.  If
 *     preservenum is zero it should always work properly.
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableGA_MUTATE(element)
    struct paramtableGA_type *element;
{
    int i, j, preservenum;

    /*
     * The field mutation_probability is the probability that any given bit
     * in the parameter is mutated.
     */

    /* preservenum is the number of best tables to leave unaltered. */

    preservenum = element->preserve;

    for (i = preservenum; i < element->num_tables; i++)
    {
        for (j = 0; j < element->num_params; j++)
        {
            if (element->search[j] == 0)  /* do not vary this parameter */
                continue;

            mutate_parameter(&element->param[i][j],
                             element->mutation_probability,
                             (int)element->bits_per_parameter,
                             element->param_size);
        }
    }

    return 1;
}




/*
 * ParamtableGA_FITSTATS
 *
 * FUNCTION
 *     Generates statistics on fitness values in table.  This should be
 *     called after running a series of simulations and loading up the
 *     fitness array.
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
ParamtableGA_FITSTATS(element)
    struct paramtableGA_type *element;
{
    int    i, n;
    double fit, fitsum, fitmin, fitmax, fitsumsq, cumulfit;
    int    min_fitindex, max_fitindex;

    /* This makes compiler warnings go away: */
    min_fitindex = max_fitindex = BIGNUM;

    fitsum = fitsumsq = 0.0;

    fitmin =  BIGNUM;
    fitmax = -BIGNUM;

    for (i = 0; i < element->num_tables; i++)
    {
        fit = element->fitness[i];
        fitsum   += fit;
        fitsumsq += fit * fit;

        if (fit < fitmin)
        {
            fitmin = fit;
            min_fitindex = i;
        }

        if (fit > fitmax)
        {
            fitmax = fit;
            max_fitindex = i;
        }
    }


    /* Check that fitsum is a positive number. */

    if (fitsum <= TINY)
    {
        Error();
        printf("paramtableGA: FITSTATS: sum of fitness values must be a positive number!\n");
        return 0;
    }


    n = element->num_tables;

    element->min_fitness       = fitmin;
    element->min_fitness_index = min_fitindex;
    element->max_fitness       = fitmax;
    element->max_fitness_index = max_fitindex;
    element->avg_fitness       = fitsum / n;

    if (n > 1)
        element->stdev_fitness = sqrt((fitsumsq - (fitsum * fitsum) / n)
                                      / (n - 1));
    else
        element->stdev_fitness = 0.0;

    /*
     * Calculate normalized and cumulative normalized fitness values.
     */

    for (i = 0; i < element->num_tables; i++)
        element->normfitness[i] = element->fitness[i] / fitsum;

    cumulfit = 0.0;

    for (i = 0; i < element->num_tables; i++)
    {
        cumulfit += element->normfitness[i];
        element->cumulfitness[i] = cumulfit;
    }

    if (fabs(cumulfit - 1.0) > TINY)
    {
        Error();
        printf("paramtableGA: FITSTATS: normalized fitness values do not sum to 1.0!\n");
        return 0;
    }

    return 1;
}




/*
 * ParamtableGA_RECENTER
 *
 * FUNCTION
 *     Centers limits of parameter search around existing parameters.
 *     Does not alter the range of search.
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
ParamtableGA_RECENTER(element)
    struct paramtableGA_type *element;
{
    int i, j;
    double par, minpar, maxpar, newcenter;
    Param out;

    for (i = 0; i < element->num_params; i++)
    {
        /* Check range values. */

        if ((element->range[i] < 0.0) && (element->type[i] == 0))
        {
            Error();
            printf("paramtableGA: RECENTER: parameter limits %d are bogus.\n", i);
            printf("range[%d] must be > 0!\n", i);
            return 0;
        }
        else if ((element->range[i] < 1.0) && (element->type[i] == 1))
        {
            Error();
            printf("paramtableGA: RECENTER: parameter limits %d are bogus.\n", i);
            printf("range[%d] must be >= 1.0!\n", i);
            return 0;
        }


        /*
         * Copy parameters to the temp array.
         */

        for (j = 0; j < element->num_tables; j++)
            element->temp[j][i] = element->param[j][i];


        /*
         * Figure out what the new center points should be.
         */

        minpar =  BIGNUM;
        maxpar = -BIGNUM;

        for (j = 0; j < element->num_tables; j++)
        {
            bit_string_to_floating_point(element->temp[j][i], &par,
                                         element->type[i],
                                         element->center[i],
                                         element->range[i],
                                         element->max_parameter,
                                         element->param_size,
                                         element->use_gray_code);

            if (par < minpar)
                minpar = par;

            /*
             * I don't use "else if" here since the first parameter will be
             * both minpar and maxpar.
             */

            if (par > maxpar)
                maxpar = par;
        }

        if (element->type[i] == 0)  /* additive */
            newcenter = (maxpar - minpar) / 2.0;
        else                          /* multiplicative */
            newcenter = sqrt(maxpar * minpar);


        /*
         * Transform the parameter bit strings and copy into the param
         * array.
         */

        for (j = 0; j < element->num_tables; j++)
        {
            bit_string_to_floating_point(element->temp[j][i],
                                         &par,
                                         element->type[i],
                                         element->center[i],
                                         element->range[i],
                                         element->max_parameter,
                                         element->param_size,
                                         element->use_gray_code);

            floating_point_to_bit_string(par,
                                         &out,
                                         element->type[i],
                                         newcenter,
                                         element->range[i],
                                         element->max_parameter,
                                         element->param_size,
                                         element->use_gray_code);

            element->param[j][i] = out;
        }

        /*
         * Change the center value.
         */

        element->center[i] = newcenter;
    }

    return 1;
}




/*
 * ParamtableGA_RESTART
 *
 * FUNCTION
 *     Reseeds the parameter table with random values,
 *     except for the "protected" parameter sets.
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
ParamtableGA_RESTART(element)
    struct paramtableGA_type *element;
{
    int i, j;
    int preservenum;  /* the number of best tables to copy unaltered. */

    if (!element->alloced)
    {
        Error();
        printf("paramtableGA: RESTART: tables must be allocated before randomization.\n");
        return;
    }

    for (i = 0; i < element->num_params; i++)
    {
        if (element->range[i] < 0.0 && element->type[i] == 0)
        {
            Error();
            printf("paramtableGA: RESTART: parameter limits %d are bogus.\n", i);
            printf("range[%d] must be >= 0!\n", i);
            return;
        }
        else if (element->range[i] < 1.0 && element->type[i] == 1)
        {
            Error();
            printf("paramtableGA: RESTART: parameter limits %d are bogus.\n", i);
            printf("range[%d] must be >= 1.0!\n", i);
            return;
        }
    }

    preservenum = element->preserve;


    /* Randomize the parameter values for all non-preserved tables. */

    for (i = preservenum; i < element->num_tables; i++)
    {
        for (j = 0; j < element->num_params; j++)
        {
            double center, range, min, max, param;

            if (element->type[j] == 0)  /* additive parameter */
            {
                center = element->center[j];
                range  = element->range[j];
                min    = center - range;
                max    = center + range;
                param  = frandom(min, max);
                floating_point_to_bit_string(param,
                                             &element->param[i][j],
                                             0,
                                             center,
                                             range,
                                             element->max_parameter,
                                             element->param_size,
                                             element->use_gray_code);
            }
            else /* multiplicative parameter */
            {
                /*
                 * This procedure makes numbers between 1 and range as
                 * likely as numbers between 1/range and 1.
                 */

                center = element->center[j];
                range  = element->range[j];
                param  = center * exp(frandom(-log(range), log(range)));
                floating_point_to_bit_string(param,
                                             &element->param[i][j],
                                             1,
                                             center,
                                             range,
                                             element->max_parameter,
                                             element->param_size,
                                             element->use_gray_code);
            }
        }
    }
}




/*
 * ParamtableGA_SAVE
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
ParamtableGA_SAVE(element, filename)
    struct paramtableGA_type *element;
    char *filename;
{
    FILE *fp;
    int   i;

    assert(filename != NULL);

    fp = fopen(filename, "w");

    if (fp == NULL)
    {
        Error();
        printf("paramtableGA: SAVE: file cannot be opened for SAVE action!\n");
        return 0;
    }

    paramFwrite(&element->generation,
                sizeof(int),        1, fp);
    paramFwrite(&element->num_tables,
                sizeof(int),        1, fp);
    paramFwrite(&element->num_params,
                sizeof(int),        1, fp);
    paramFwrite(&element->num_params_to_search,
                sizeof(int),        1, fp);
    paramFwrite(&element->param_size,
                sizeof(short),      1, fp);
    paramFwrite(&element->bits_per_parameter,
                sizeof(short),      1, fp);
    paramFwrite(&element->max_parameter,
                sizeof(Param_long), 1, fp);
    paramFwrite(element->search,
                sizeof(short),      element->num_params, fp);
    paramFwrite(element->type,
                sizeof(short),      element->num_params, fp);
    paramFwrite(element->center,
                sizeof(double),     element->num_params, fp);
    paramFwrite(element->range,
                sizeof(double),     element->num_params, fp);
    paramFwrite(element->best,
                sizeof(double),     element->num_params, fp);
    paramFwrite(&element->best_match,
                sizeof(double),     1, fp);


    /*
     * Write out the array of label strings.
     */

    for (i = 0; i < element->num_params; i++)
    {
        paramFwrite(element->label[i], (LABEL_SIZE + 1)
                    * sizeof(char), 1, fp);

        if (ferror(fp))
        {
            Error();
            printf("paramtableGA: SAVE: error writing to file!\n");
            return 0;
        }
    }

    for (i = 0; i < element->num_tables; i++)
    {
        paramFwrite(element->param[i], sizeof(Param),
                    element->num_params, fp);
    }

    for (i = 0; i < element->num_tables; i++)
    {
        paramFwrite(element->temp[i], sizeof(Param),
                    element->num_params, fp);
    }

    paramFwrite(element->fitness,
                sizeof(double), element->num_tables, fp);
    paramFwrite(element->tempfitness,
                sizeof(double), element->num_tables, fp);
    paramFwrite(element->fitrank,
                sizeof(int),    element->num_tables, fp);
    paramFwrite(&element->min_fitness,
                sizeof(double), 1, fp);
    paramFwrite(&element->max_fitness,
                sizeof(double), 1, fp);
    paramFwrite(&element->avg_fitness,
                sizeof(double), 1, fp);
    paramFwrite(&element->stdev_fitness,
                sizeof(double), 1, fp);
    paramFwrite(&element->min_fitness_index,
                sizeof(int),    1, fp);
    paramFwrite(&element->max_fitness_index,
                sizeof(int),    1, fp);
    paramFwrite(element->normfitness,
                sizeof(double), element->num_tables, fp);
    paramFwrite(element->cumulfitness,
                sizeof(double), element->num_tables, fp);
    paramFwrite(element->selectindex,
                sizeof(int),    element->num_tables, fp);
    paramFwrite(&element->preserve,
                sizeof(int),    1, fp);
    paramFwrite(&element->crossover_type,
                sizeof(short),  1, fp);
    paramFwrite(&element->crossover_probability,
                sizeof(double), 1, fp);
    paramFwrite(&element->crossover_number,
                sizeof(int),    1, fp);
    paramFwrite(&element->crossover_break_param,
                sizeof(short),  1, fp);
    paramFwrite(&element->mutation_probability,
                sizeof(double), 1, fp);
    paramFwrite(&element->use_gray_code,
                sizeof(short),  1, fp);
    paramFwrite(&element->do_restart,
                sizeof(short),  1, fp);
    paramFwrite(&element->restart_after,
                sizeof(int),    1, fp);
    paramFwrite(&element->restart_count,
                sizeof(int),    1, fp);
    paramFwrite(&element->old_fitness,
                sizeof(double), 1, fp);
    paramFwrite(&element->restart_thresh,
                sizeof(double), 1, fp);

    fclose(fp);
    return 1;
}




/*
 * ParamtableGA_SAVEBEST
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
ParamtableGA_SAVEBEST(element, filename)
    struct paramtableGA_type *element;
    char *filename;
{
    FILE   *fp;
    int     i;
    int     best_table;
    double  best;

    assert(filename != NULL);

    fp = fopen(filename, "w");

    if (fp == NULL)
    {
        Error();
        printf("paramtableGA: SAVEBEST: file cannot be opened for SAVEBEST action!\n");
        return 0;
    }

    best_table = element->max_fitness_index;

    fprintf(fp, "Best fitness so far: %g\n",  element->best_match);
    fprintf(fp, "Parameter table data:\n");
    fprintf(fp, "-----------------------------------------------------------------------\n");
    fprintf(fp, "Generation number: %d\n",    element->generation);
    fprintf(fp, "Number of parameters: %d\n", element->num_params);

    fprintf(fp, "\n");
    fprintf(fp, "Param   Varied?    Type    Range      Best             Label\n");
    fprintf(fp, "-----   -------    ----    -----      ----             -----\n");
    fprintf(fp, "\n");

    for (i = 0; i < element->num_params; i++)
    {
        fprintf(fp, "%-5d   ", i);  /* parameter number */

        if (element->search[i] == 0)
            fprintf(fp, "%s        ", "NO ");
        else
            fprintf(fp, "%s        ", "YES");

        if (element->type[i] == 0)
            fprintf(fp, "%s    ", "add ");
        else
            fprintf(fp, "%s    ", "mult");

        if (element->search[i] == 1)
        {
            bit_string_to_floating_point(element->param[best_table][i],
                                         &best,
                                         element->type[i],
                                         element->center[i],
                                         element->range[i],
                                         element->max_parameter,
                                         element->param_size,
                                         element->use_gray_code);

            fprintf(fp, "%-8g   %-15.6g  %s\n",
                    element->range[i], best, element->label[i]);
        }
        else
        {
            fprintf(fp, "%-8g   %-15.6g  %s\n",
                    element->range[i], element->center[i],
                    element->label[i]);
        }
    }

    fprintf(fp, "\n");
    fprintf(fp, "Best fitness so far: %.10g\n", element->best_match);

    fclose(fp);
    return 1;
}




/*
 * ParamtableGA_RESTORE
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
ParamtableGA_RESTORE(element, filename)
    struct paramtableGA_type *element;
    char *filename;
{
    FILE *fp;
    int   i;

    assert(filename != NULL);

    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        Error();
        printf("paramtableGA: RESTORE: file cannot be opened for RESTORE action!\n");
        return 0;
    }

    /* First delete the old parameter tables. */

    ParamtableGA_TABDELETE(element);

    /*
     * Read in the parameters that let you reallocate the parameter tables.
     */

    fread(&element->generation,           sizeof(int),   1, fp);
    fread(&element->num_tables,           sizeof(int),   1, fp);
    fread(&element->num_params,           sizeof(int),   1, fp);
    fread(&element->num_params_to_search, sizeof(int),   1, fp);
    fread(&element->param_size,           sizeof(short), 1, fp);
    fread(&element->bits_per_parameter,   sizeof(short), 1, fp);

    fread(&element->max_parameter, sizeof(Param_long), 1, fp);

    /* Allocate the new parameter tables. */

    if (!ParamtableGA_TABCREATE(element, element->num_tables,
                                element->num_params))
    {
        return 0;
    }

    fread(element->search,      sizeof(short),  element->num_params, fp);
    fread(element->type,        sizeof(short),  element->num_params, fp);
    fread(element->center,      sizeof(double), element->num_params, fp);
    fread(element->range,       sizeof(double), element->num_params, fp);
    fread(element->best,        sizeof(double), element->num_params, fp);
    fread(&element->best_match, sizeof(double), 1, fp);


    /*
     * Read in the array of label strings.
     */

    for (i = 0; i < element->num_params; i++)
    {
        fread(element->label[i], (LABEL_SIZE + 1) * sizeof(char), 1, fp);

        if (ferror(fp))
        {
            Error();
            printf("paramtableGA: RESTORE: error writing to file!\n");
            return 0;
        }
    }

    for (i = 0; i < element->num_tables; i++)
    {
        fread(element->param[i], sizeof(Param), element->num_params, fp);
    }

    for (i = 0; i < element->num_tables; i++)
    {
        fread(element->temp[i], sizeof(Param), element->num_params, fp);
    }

    fread(element->fitness,
          sizeof(double), element->num_tables, fp);
    fread(element->tempfitness,
          sizeof(double), element->num_tables, fp);
    fread(element->fitrank,
          sizeof(int),    element->num_tables, fp);
    fread(&element->min_fitness,
          sizeof(double), 1, fp);
    fread(&element->max_fitness,
          sizeof(double), 1, fp);
    fread(&element->avg_fitness,
          sizeof(double), 1, fp);
    fread(&element->stdev_fitness,
          sizeof(double), 1, fp);
    fread(&element->min_fitness_index,
          sizeof(int),    1, fp);
    fread(&element->max_fitness_index,
          sizeof(int),    1, fp);
    fread(element->normfitness,
          sizeof(double), element->num_tables, fp);
    fread(element->cumulfitness,
          sizeof(double), element->num_tables, fp);
    fread(element->selectindex,
          sizeof(int),    element->num_tables, fp);
    fread(&element->preserve,
          sizeof(int),    1, fp);
    fread(&element->crossover_type,
          sizeof(short),  1, fp);
    fread(&element->crossover_probability,
          sizeof(double), 1, fp);
    fread(&element->crossover_number,
          sizeof(int),    1, fp);
    fread(&element->crossover_break_param,
          sizeof(short),  1, fp);
    fread(&element->mutation_probability,
          sizeof(double), 1, fp);
    fread(&element->use_gray_code,
          sizeof(short),  1, fp);
    fread(&element->do_restart,
          sizeof(short),  1, fp);
    fread(&element->restart_after,
          sizeof(int),    1, fp);
    fread(&element->restart_count,
          sizeof(int),    1, fp);
    fread(&element->old_fitness,
          sizeof(double), 1, fp);
    fread(&element->restart_thresh,
          sizeof(double), 1, fp);

    element->alloced = 1;

    fclose(fp);
    return 1;
}




/*
 * ParamtableGA_RESTOREBEST
 *
 * FUNCTION
 *     Restores the best parameter table from a text file and randomizes
 *     all the others.  The format of the parameter file is rather
 *     arbitrary -- sorry!
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
ParamtableGA_RESTOREBEST(element, filename)
    struct paramtableGA_type *element;
    char *filename;
{
    FILE   *fp;
    int     i, j;
    char    temp[MAX_LINELEN];
    int     param_num;
    char    varied[LABEL_SIZE];
    char    type[LABEL_SIZE];
    double  center, param, range, best, min, max;
    char    label[LABEL_SIZE];

    assert(filename != NULL);

    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        Error();
        printf("paramtableGA: RESTOREBEST: file cannot be opened for RESTOREBEST action!\n");
        return 0;
    }

    /*
     * Skip the first four lines.  We have to use sscanf because
     * fgets and fscanf do not live together happily.
     */

    for (i = 0; i < 4; i++)
    {
        strcpy(temp, "");
        fgets(temp, MAX_LINELEN, fp);
    }

    fgets(temp, MAX_LINELEN, fp);
    sscanf(temp, "Number of parameters: %d", &(element->num_params));

    /*
     * Skip another four lines.
     */

    for (i = 0; i < 4; i++)
        fgets(temp, MAX_LINELEN, fp);

    for (i = 0; i < element->num_params; i++)
    {
        /*
         * These assignments get rid of compiler warnings:
         */

        param_num = BIGNUM;
        range = (double)BIGNUM;
        best  = (double)BIGNUM;

        fgets(temp, MAX_LINELEN, fp);

        if (sscanf(temp, "%d %s %s %lg %lg",
                   &param_num, varied, type, &range, &best) != 5)
        {
            Error();
            printf("paramtableGA: RESTOREBEST: could not find all items for parameter %d!\n", i);
            return 0;
        }

        if (param_num != i)
        {
            Error();
            printf("paramtableGA: RESTOREBEST: parameters are in incorrect order!\n");
            return 0;
        }

        /* Set parameter-specific fields. */

        if (strcmp(varied, "YES") == 0)
            element->search[i] = 1;
        else /* NO */
            element->search[i] = 0;


        if (strcmp(type, "mult") == 0)
            element->type[i] = 1; /* multiplicative */
        else /* add */
            element->type[i] = 0; /* additive */


        if (element->type[i] == 0) /* additive */
        {
            if (range <= 0.0)
            {
                Error();
                printf("paramtableGA: RESTOREBEST: range of parameter %d must be at least 0.0!\n", i);
                return 0;
            }
        }
        else /* multiplicative */
        {
            if (range == 1.0)
            {
                Error();
                printf("paramtableGA: RESTOREBEST: range of parameter %d must be at least 1.0!\n", i);
                return 0;
            }
        }

        element->range[i] = range;

        strcpy(element->label[i], label);

        /*
         * Keep the old center value.  Use the range value and the center
         * value to check if the best parameter value is within the range.
         * If it is, assign it to table 0 after converting it to a bit
         * string.
         */

        if (element->search[i] == 0)
        {
            if ((fabs(element->center[i] - best)) > TINY)
            {
                Error();
                printf("paramtableGA: RESTOREBEST: fixed parameter %d is different from center value!\n",
                       i);
                return 0;
            }
        }

        if (element->type[i] == 0) /* additive */
        {
            min = element->center[i] - element->range[i];
            max = element->center[i] + element->range[i];
        }
        else /* multiplicative */
        {
            min = element->center[i] / element->range[i];
            max = element->center[i] * element->range[i];
        }

        if ((best < min) || (best > max))
        {
            Error();
            printf("paramtableGA: RESTOREBEST: best param value %d is out of range!\n", i);
            return 0;
        }

        /*
         * Convert the parameter to a bit string and store in table 0.
         */

        floating_point_to_bit_string(best,
                                     &(element->param[0][i]),
                                     element->type[i],
                                     element->center[i],
                                     element->range[i],
                                     element->max_parameter,
                                     element->param_size,
                                     element->use_gray_code);
    }

    /*
     * Now randomize the contents of the other parameter tables.
     * This code is lifted from the RANDOMIZE action.
     */

    for (i = 1; i < element->num_tables; i++)
    {
        for (j = 0; j < element->num_params; j++)
        {
            if (element->type[j] == 0)  /* additive parameter */
            {
                center = element->center[j];
                range  = element->range[j];
                min    = center - range;
                max    = center + range;

                if (element->search[j] == 0)
                {
                    /* don't vary this parameter */
                    param = center;
                }
                else
                {
                    param = frandom(min, max);
                }

                floating_point_to_bit_string(param,
                                             &(element->param[i][j]),
                                             0,
                                             center,
                                             range,
                                             element->max_parameter,
                                             element->param_size,
                                             element->use_gray_code);
            }
            else /* multiplicative parameter */
            {
                /*
                 * This procedure makes numbers between 1 and range as
                 * likely as numbers between 1/range and 1.
                 */

                center = element->center[j];
                range  = element->range[j];

                if (element->search[j] == 0)
                {
                    /* Don't vary this parameter. */
                    param = center;
                }
                else
                {
                    param = center * exp(frandom(-log(range), log(range)));
                }

                floating_point_to_bit_string(param,
                                             &element->param[i][j],
                                             1,
                                             center,
                                             range,
                                             element->max_parameter,
                                             element->param_size,
                                             element->use_gray_code);
            }
        }
    }

    fclose(fp);
    return 1;
}




/*
 * ParamtableGA_CHECK
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
ParamtableGA_CHECK(element)
    struct paramtableGA_type *element;
{
    int i;

    if (element->num_tables <= 0)
    {
        Error();
        printf("paramtableGA: number of tables must be > 0!\n");
    }

    if (element->num_params <= 0)
    {
        Error();
        printf("paramtableGA: number of parameters must be > 0!\n");
    }

    for (i = 0; i < element->num_params; i++)
    {
        if ((element->type[i] != 0) && (element->type[i] != 1))
        {
            Error();
            printf("paramtableGA: type field %d must be 0 or 1!\n", i);
        }
    }

    if ((element->param_size != 1) &&
        (element->param_size != 2) &&
        (element->param_size != 4))
    {
        Error();
        printf("paramtableGA: param_size can only be 1, 2, or 4 bytes!\n");
    }

    for (i = 0; i < element->num_params; i++)
    {
        if ((element->range[i] < 0.0) && (element->type[i] == 0))
        {
            Error();
            printf("paramtableGA: parameter limits %d are bogus.\n", i);
            printf("range[%d] must be > 0!\n", i);
        }
        else if ((element->range[i] < 1.0) && (element->type[i] == 1))
        {
            Error();
            printf("paramtableGA: parameter limits %d are bogus.\n", i);
            printf("range[%d] must be >= 1.0!\n", i);
        }
    }

    if (element->preserve <= 0)
    {
        Error();
        printf("paramtableGA: preserve must be > 0!\n");
    }

    if ((element->crossover_type < 0) || (element->crossover_type > 1))
    {
        Error();
        printf("paramtableGA: crossover probability must be 0 or 1!\n");
    }

    if ((element->crossover_probability < 0.0)
        || (element->crossover_probability > 1.0))
    {
        Error();
        printf("paramtableGA: crossover probability must be between 0.0 and 1.0!\n");
    }

    if (element->crossover_break_param)
    {
        if ((element->crossover_number < 0)
            || (element->crossover_number
                > (element->num_params * element->bits_per_parameter - 1)))
        {
            Error();
            printf("paramtableGA: crossover number must be between 0 and (num_params * bits_per_param - 1)!\n");
        }
    }
    else
    {
        if ((element->crossover_number < 0)
            || (element->crossover_number > (element->num_params - 1)))
        {
            Error();
            printf("paramtableGA: crossover number must be between 0 and (num_params - 1)!\n");
        }
    }

    if ((element->mutation_probability < 0.0)
        || (element->mutation_probability > 1.0))
    {
        Error();
        printf("paramtableGA: mutation_probability must be between 0.0 and 1.0!\n");
    }
}






/*
 * OBJECT
 *     paramtableGA: a 2-D table for storing sets of parameters.
 *                   Also has actions which allow new parameters
 *                   to be generated using a genetic algorithm.
 *
 * FUNCTION
 *     ParamtableGA
 *
 * ARGUMENTS
 *     element -- address of element
 *     action  -- address of action
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 */

int
ParamtableGA(element, action)
    struct paramtableGA_type *element;
    Action *action;
{
    int    num_tables, num_params;
    char  *savefilename = NULL, *restorefilename = NULL;
    short  randomize_first = 0; /* flag; see below */

    SELECT_ACTION(action)
    {
    case CREATE:
        element->alloced = 0;
        break;


    case TABCREATE:
        if ((action->argc < 2) || (action->argc > 3))
        {
            Error();
            printf("paramtableGA: usage: TABCREATE num_tables num_params param_size\n");
            return 0;
        }

        element->generation = 0;
        num_tables = atoi(action->argv[0]);
        num_params = atoi(action->argv[1]);

        /*
         * The default param_size is 4 bytes i.e. 32 bits
         * but it can be overridden here.  Note that param_size
         * CANNOT be set interactively since it has to be set before
         * the tables are allocated.
         */

        if (action->argc == 3)
            element->param_size = (short)atoi(action->argv[2]);

        if (!ParamtableGA_TABCREATE(element, num_tables, num_params))
            return 0;

        break;


    case DELETE:
        /*FALLTHROUGH */


    case TABDELETE:
        ParamtableGA_TABDELETE(element);
        break;


    case INITSEARCH:
        /*
         * Check to see if the "random" option has been selected.
         * If so, all tables will have random values in them.
         * If not, the starting values for all parameters will be
         * used for the first table.
         */

        if ((action->argc > 0)
            && (strcmp(action->argv[0], "random") == 0))
        {
            randomize_first = 1;
        }

        element->generation = 0;

        /*FALLTHROUGH */


    case RANDOMIZE:
        if (!ParamtableGA_RANDOMIZE(element, randomize_first))
            return 0;

        break;


    case UPDATE_PARAMS:
        /*
         * Note that generation numbers start at 0 but when the first
         * generation has been completed the generation number is 1.
         * Therefore you should SAVE the parameter tables before executing
         * an UPDATE_PARAMS action.
         */

        if (element->do_restart)
        {
            if ((element->max_fitness - element->old_fitness)
                > element->restart_thresh)
            {
                element->old_fitness = element->max_fitness;
                element->restart_count = 0;
            }
            else if (element->restart_count
                     >= element->restart_after)
            {
                /* Re-seed the parameter tables,
                 * except for the best table(s). */

                printf("\n\n>>>>> Reseeding parameter table! <<<<<\n\n");
                element->restart_count = 0;
                ParamtableGA_RESTART(element);

                /*
                 * There's no point in doing the REPRODUCE, CROSSOVER,
                 * MUTATE sequence if you're doing a restart.
                 */

                element->generation++;

                break;
            }
            else
            {
                ++(element->restart_count);
            }
        }


        if (!ParamtableGA_REPRODUCE(element))
            return 0;

        if (!ParamtableGA_CROSSOVER(element))
            return 0;

        if (!ParamtableGA_MUTATE(element))
            return 0;

        element->generation++;
        break;


    case REPRODUCE:
        if (!ParamtableGA_REPRODUCE(element))
            return 0;

        break;


    case CROSSOVER:
        if (!ParamtableGA_CROSSOVER(element))
            return 0;

        break;


    case MUTATE:
        if (!ParamtableGA_MUTATE(element))
            return 0;

        break;


    case FITSTATS:
        if (!ParamtableGA_FITSTATS(element))
            return 0;

        break;


    case RECENTER:
        if (!ParamtableGA_RECENTER(element))
            return 0;

        break;


    case RESTART:
        printf("\n\n>>>>> Reseeding parameter table! <<<<<\n\n");
        ParamtableGA_RESTART(element);
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
            printf("paramtableGA: SAVE: no filename specified!\n");
            return 0;
        }

        if (!ParamtableGA_SAVE(element, savefilename))
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
            printf("paramtableGA: usage: SAVEBEST filename\n");
            return 0;
        }

        /* Calculate the best_match value. */

        if (!ParamtableGA_FITSTATS(element))
            return 0;

        /* yes, I know this is redundant: */
        element->best_match = element->max_fitness;

        if (!ParamtableGA_SAVEBEST(element, savefilename))
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
            printf("paramtableGA: RESTORE: no filename specified!\n");
            return 0;
        }

        if (!ParamtableGA_RESTORE(element, restorefilename))
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
            printf("paramtableGA: usage: RESTOREBEST filename\n");
            return 0;
        }

        if (!ParamtableGA_RESTOREBEST(element, restorefilename))
            return 0;

        break;


    case CHECK:
        ParamtableGA_CHECK(element);
        break;


    default:
        Error();
        printf("paramtableGA: invalid action %s\n", action->name);
        return 0;
    }

    return 1;
}






/*
 * The following is a utility function to put parameters into the table
 * with less fuss.
 */

/*
 * do_initparamGA
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
do_initparamGA(argc, argv)
    int argc;
    char **argv;
{
    int     param;
    char   *path;
    struct  paramtableGA_type *element;
    short   type;
    double  center, range;
    char   *label;

    initopt(argc, argv, "path param type range center label");

    if (argc != 7)

    {
        printoptusage(argc, argv);
        return 0;
    }

    path   = optargv[1];
    param  = atoi(optargv[2]);
    type   = (short)atoi(optargv[3]);
    range  = Atod(optargv[4]);
    center = Atod(optargv[5]);
    label  = optargv[6];

    element = (struct paramtableGA_type *)GetElement(path);


    /* Error checks. */

    if (!CheckClass(element, ClassID("param")))
    {
        Error();
        printf("initparamGA: Element %s is not a parameter table object!\n", element->name);
        return 0;
    }

    if ((param < 0) || (param >= element->num_params))
    {
        Error();
        printf("initparamGA: parameter index is out of range!\n");
        return 0;
    }

    if ((type != 0) && (type != 1))
    {
        Error();
        printf("initparamGA: type field must be 0 or 1!\n");
        return 0;
    }

    /* FIXME: this is redundant; use a switch statement. */

    if (type == 0)  /* additive parameter */
    {
        if (range < 0.0)
        {
            Error();
            printf("initparamGA: additive parameter range must be > 0!\n");
            return 0;
        }
    }

    if (type == 1)  /* multiplicative parameter */
    {
        if (range < 1.0)
        {
            Error();
            printf("initparamGA: multiplicative parameter range must be > 1.0!\n");
            return 0;
        }
    }

    element->type[param]   = type;
    element->center[param] = center;
    element->range[param]  = range;

    /* The default is that all parameters are to be searched over. */

    element->search[param] = 1;
    ++element->num_params_to_search;

    strncpy(element->label[param], label, LABEL_SIZE);

    return 1;
}




/*
 * SetSearchGA
 *
 * FUNCTION
 *     Sets the search flags for the paramtableGA object.
 *     Called by do_setsearch.
 *
 * ARGUMENTS
 *     argc          -- number of command arguments (not including flags)
 *     argv          -- command arguments proper
 *     all, not, add -- flags passed from do_setsearch
 *
 * RETURN VALUE
 *     int -- 1 = success; 0 = failure
 *
 * AUTHOR
 *     Mike Vanier
 */

int
SetSearchGA(argc, argv, all, not, add)
    int argc;
    char **argv;
    short all;
    short not;
    short add;
{
    int    i, param, tosearch;
    struct paramtableGA_type *element;

    element = (struct paramtableGA_type *)GetElement(argv[1]);

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
            printf("setsearch: parameter index %d is out of range!\n", param);
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

