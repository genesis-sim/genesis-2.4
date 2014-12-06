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

#include "param_ext.h"
#include "olf_struct.h"

#define BIGLINELEN 10000

/*
 * Use this with RemArg (see below):
 */

char remainder_argstr[10000];

extern void StringToArgList();
extern void FreeArgv();

extern int SetSearchBF();
extern int SetSearchCG();
extern int SetSearchGA();
extern int SetSearchSA();
extern int SetSearchSS();

/*
 * The following function is a version of free() that checks
 * for NULL pointers.  It is used elsewhere in the param library.
 * Ultimately it will probably be put into the base code as
 * Free() or something.
 */

void
paramFree(ptr)
    void *ptr;
{
    if (ptr == NULL)
        return;
    else
    {
        free(ptr);

        /*
         * Setting a freed pointer value to NULL may not be necessary,
         * but I prefer it this way.
         */

        ptr = NULL;
    }
}




/*
 * The following function is a version of fwrite() that checks
 * for NULL pointers.  It is used throughout the param library.
 * Ultimately it will probably be put into the base code as
 * Fwrite() or something.
 */

size_t
paramFwrite(ptr, size, nitems, stream)
    void *ptr;
    size_t size;
    size_t nitems;
    FILE *stream;
{
    if (ptr == NULL)
    {
        Error();
        printf("paramFwrite: attempted to write from NULL pointer!\n");
        return ((size_t) 0);
    }
    else
        return fwrite(ptr, size, nitems, stream);
}




/*
 * do_setsearch
 *
 * FUNCTION
 *     Sets the search flags for the any paramtable object.
 *
 * ARGUMENTS
 *     int    argc - number of command arguments
 *     char **argv - command arguments proper
 *
 * RETURN VALUE
 *     int -- 1 = success; 0 = failure
 *
 * AUTHOR
 *     Mike Vanier
 *
 */

int
do_setsearch(argc, argv)
    int argc;
    char **argv;
{
    short    all, not, add; /* flags */
    int      status;
    Element *element;

    all = not = add = 0;

    initopt(argc, argv, "path [param1] [param2] ... -all -not -add");

    while ((status = G_getopt(argc, argv)) == 1)
    {
        if (strcmp(G_optopt, "-all") == 0)
            all = 1;
        else if (strcmp(G_optopt, "-not") == 0)
            not = 1;
        else if (strcmp(G_optopt, "-add") == 0)
            add = 1;
    }

    /* error checks */

    if (optargc < 2)
    {
        printoptusage(argc, argv);
        return 0;
    }

    if (status < 0)
    {
        printoptusage(argc, argv);
        return 0;
    }

    if (all && not)
    {
        Error();
        printf("setsearch: -all and -not options are mutually exclusive!\n");
        return 0;
    }

    if (not && add)
    {
        Error();
        printf("setsearch: -not and -add options are mutually exclusive!\n");
        return 0;
    }

    element = (Element *) GetElement(argv[1]);

    if (strcmp(element->object->name, "paramtableBF") == 0)
    {
        return SetSearchBF(optargc, optargv, all, not, add);
    }
    else if (strcmp(element->object->name, "paramtableCG") == 0)
    {
        return SetSearchCG(optargc, optargv, all, not, add);
    }
    else if (strcmp(element->object->name, "paramtableGA") == 0)
    {
        return SetSearchGA(optargc, optargv, all, not, add);
    }
    else if (strcmp(element->object->name, "paramtableSA") == 0)
    {
        return SetSearchSA(optargc, optargv, all, not, add);
    }
    else if (strcmp(element->object->name, "paramtableSS") == 0)
    {
        return SetSearchSS(optargc, optargv, all, not, add);
    }
    else
    {
        Error();
        printf("setsearch: The object %s cannot be used with this function.\n", element->name);
        return 0;
    }
}




/*
 * CopyParamLabel
 *
 * FUNCTION
 *     Copies the label field from a string to the label field of a
 *     paramtable object.  Also removes a trailing newline if there
 *     is one.
 *
 * ARGUMENTS
 *     char *dest -- destination of copy
 *     char *line -- line of text
 *     int   n    -- number of non-whitespace fields to skip
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */


void
CopyParamLabel(dest, line, n)
    char *dest;
    char *line;
    int n;
{
    int   startpos = 0;
    int   i, endpos;
    int   fields = 0;
    short in_word = 0; /* flag: in a word */
    char  c;

    /* error checks */

    if (strlen(line) == 0)
    {
        Error();
        printf("CopyParamLabel: input string has zero length!\n");
    }

    if (n < 1)
    {
        Error();
        printf("CopyParamLabel: n must be >= 1!\n");
        return;
    }

    /* find starting position for copy */

    while ((c = line[startpos]) != '\0') /* not at end of line */
    {
        if ((c == ' ') || (c == '\t'))  /* whitespace */
        {
            if (in_word == 1)
            {
                /* we've reached the end of a non-whitespace field */
                fields++;
            }

            in_word = 0;
        }
        else
        {
            if (in_word == 0) /* first character in non-whitespace field */
            {
                if (fields == n)
                    break;

                in_word = 1;
            }
        }

        startpos++;
    }

    /* find ending position for copy */

    endpos = strlen(line);

    /* don't copy trailing newlines */

    if (line[endpos-1] == '\n')
        endpos--;

    /*
     * Copy the source string to the destination.
     * Stop if the source string is too long.
     */

    for (i = startpos; (i < endpos) && ((i - startpos) < LABEL_SIZE) ; i++)
        dest[i-startpos] = line[i];

    /* Pad the rest of the string with null characters. */

    for (i = (endpos - startpos); i <= LABEL_SIZE ; i++)
        dest[i] = '\0';
}




/*
 * ShiftArg
 *
 * FUNCTION
 *     Removes one argument from an arglist-type string
 *     and returns it.  The string must consist of a series
 *     of string arguments separated by whitespace (tabs or spaces only).
 *     This is used for parallel genesis to pass strings representing
 *     parameter tables between nodes.
 *
 * ARGUMENTS
 *     int    argc - number of command arguments
 *     char **argv - command arguments proper
 *
 * RETURN VALUE
 *     char *; if there is no non-whitespace string in argstr then
 *             a null string ("") is returned (NOT a NULL pointer!).
 *
 * AUTHOR
 *     Mike Vanier
 */

char *
ShiftArg(argc, argv)
    int argc;
    char **argv;
{
    int   i, j;
    int   start, end;
    int   argstrlen;
    char  argstr[BIGLINELEN];
    char  newargstr[BIGLINELEN];


    initopt(argc, argv, "argument_string");

    /* error checks */

    if (optargc != 2)
    {
        printoptusage(argc, argv);
        return CopyString("");
    }

    strcpy(argstr, optargv[1]);

    /* Get rid of leading spaces if any. */

    start = 0;
    argstrlen = strlen(argstr);

    for (i = 0; i < argstrlen; i++)
    {
        if ((argstr[i] == ' ') || (argstr[i] == '\t'))
        {
            start++;
        }
        else
        {
            break;
        }
    }

    if (start != 0)
    {
        if (start == argstrlen)
        {
            /*
             * The string is empty, so just return an
             * empty string and put one in remainder_argstr too.
             */

            remainder_argstr[0] = '\0';
            return CopyString("");
        }

        /*
         * Copy argstr + start into argstr to get rid of whitespace.
         */

        for (j = 0; j < argstrlen - start; j++)
        {
            argstr[j] = argstr[j + start];
        }

        argstr[argstrlen - start] = '\0';
        argstrlen -= start;  /* Length has changed. */
    }

    /* Find ending position of first token. */

    end = argstrlen;  /* The default if there is no whitespace. */

    for (i = 0; i < argstrlen; i++)
    {
        if ((argstr[i] == ' ') || (argstr[i] == '\t'))
        {
            end = i;
            break;
        }
    }

    strncpy(newargstr, argstr, end);
    newargstr[end] = '\0';

    if (end == argstrlen)
    {
        argstr[0] = '\0';
    }

    strcpy(remainder_argstr, argstr + end);
    return CopyString(newargstr);
}




/*
 * RemArg
 *
 * FUNCTION
 *     Returns the remainder of an arg string after ShiftArg
 *     has been called.  If ShiftArg has not been called prior
 *     to calling this function it will happily return garbage.
 *
 * ARGUMENTS
 *     int    argc - number of command arguments
 *     char **argv - command arguments proper
 *
 * RETURN VALUE
 *     char *; if there is no non-whitespace string in argstr then
 *             a null string ("") is returned (NOT a NULL pointer!).
 *
 * AUTHOR
 *     Mike Vanier
 */

char *
RemArg(argc, argv)
    int argc;
    char **argv;
{
    initopt(argc, argv, "");

    /* error checks */

    if (optargc != 1)
    {
        printoptusage(argc, argv);
        return CopyString("");
    }

    return CopyString(remainder_argstr);
}

